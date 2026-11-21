#!/usr/bin/env python3
"""fpu_lower.py <in.ll> <out.ll> [--report <tu.json>]: route a Core TU's floating point through the EE's rules.

The Core TU is compiled to LLVM IR by clang's front end only
(-Xclang -disable-llvm-passes), before any optimisation, and this rewrites
every floating-point instruction in it as a call that computes what the EE
computes (ADR-006, PORT-003):

  - float add, sub, mul, div, neg, compares and int<->float conversions
    become the EE-FPU Model (common/fpu/ee_fpu.c), as the FPU instructions
    ee-gcc emits for them (add.s ... cvt.w.s);
  - double arithmetic and compares, float<->double and double<->int
    conversions, and float to unsigned int become the EE's software floating
    point (common/fpu/ee_softfp.c over the decomp's matched fp-bit), the calls
    ee-gcc emits for them (dpadd ... fptoui);
  - the conversions ee-gcc spells as a sequence (unsigned to float, float to
    a narrow unsigned) follow that sequence.

Rewriting before the optimiser matters: LLVM would otherwise narrow
`(float)((double)f + 1.0)` to a float add, which the EE never does (it adds in
software, rounding to nearest, then converts). After the rewrite the
optimiser sees calls to pure functions it can't fold.

Any floating-point construct not listed (an intrinsic, a vector, frem, a
64-bit conversion ee-gcc would call a routine for that isn't in the Core) is
an error, so nothing reaches the Core unrouted; tools/fpu_check.py checks the
linked module again. The report counts each routed operation per function,
for tools/fpu_audit.py.

It also lists the TU's statics in @llvm.compiler.used (keep_statics), so the
optimiser keeps their memory representation: EE RAM must hold what the EE's
holds (PORT-006).
"""
import argparse
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

VAL = r'(?:%[-\w.$]+|@[-\w.$]+|[-+]?[\w.+-]+|poison|undef)'
META = r'((?:, ![\w.]+ ![\w.]+)*)'
BINOP = re.compile(rf'^(\s+)(%[-\w.$]+) = (fadd|fsub|fmul|fdiv) (float|double) ({VAL}), ({VAL}){META}$')
NEG = re.compile(rf'^(\s+)(%[-\w.$]+) = fneg (float|double) ({VAL}){META}$')
CMP = re.compile(rf'^(\s+)(%[-\w.$]+) = fcmp (\w+) (float|double) ({VAL}), ({VAL}){META}$')
CAST = re.compile(rf'^(\s+)(%[-\w.$]+) = (sitofp|uitofp|fptosi|fptoui|fpext|fptrunc) '
                  rf'(\w+) ({VAL}) to (\w+){META}$')
DEFINE = re.compile(r'^define .*?@("?[-\w.$]+"?)\(')
# anything floating-point left on an instruction line after the rewrite
LEFT = re.compile(r'\b(fadd|fsub|fmul|fdiv|frem|fneg|fcmp|sitofp|uitofp|fptosi|fptoui|fpext|fptrunc)\b'
                  r'|@llvm\.[\w.]*\b(f32|f64|fmuladd|fma|sqrt|fabs|copysign|minnum|maxnum|floor|ceil|rint|round)'
                  r'|x (float|double)>')
STRING = re.compile(r'c"[^"]*"')

# fcmp predicate -> (model compare, swap operands, negate) for floats
FLOAT_CMP = {'oeq': ('c_eq', 0, 0), 'ueq': ('c_eq', 0, 0), 'one': ('c_eq', 0, 1), 'une': ('c_eq', 0, 1),
             'olt': ('c_lt', 0, 0), 'ult': ('c_lt', 0, 0), 'ole': ('c_le', 0, 0), 'ule': ('c_le', 0, 0),
             'ogt': ('c_lt', 1, 0), 'ugt': ('c_lt', 1, 0), 'oge': ('c_le', 1, 0), 'uge': ('c_le', 1, 0)}
# fcmp predicate -> how ee-gcc tests dpcmp(a, b) against 0
DOUBLE_CMP = {'oeq': 'eq', 'ueq': 'eq', 'one': 'ne', 'une': 'ne', 'olt': 'slt', 'ult': 'slt',
              'ole': 'sle', 'ule': 'sle', 'ogt': 'sgt', 'ugt': 'sgt', 'oge': 'sge', 'uge': 'sge'}

BITS = {'float': 'i32', 'double': 'i64'}
# callee -> (result type, parameter types)
CALLEES = {
    'ee_fpu_add': ('i32', 'i32 i32'), 'ee_fpu_sub': ('i32', 'i32 i32'), 'ee_fpu_mul': ('i32', 'i32 i32'),
    'ee_fpu_div': ('i32', 'i32 i32'), 'ee_fpu_neg': ('i32', 'i32'),
    'ee_fpu_c_eq': ('i32', 'i32 i32'), 'ee_fpu_c_lt': ('i32', 'i32 i32'), 'ee_fpu_c_le': ('i32', 'i32 i32'),
    'ee_fpu_cvt_s_w': ('i32', 'i32'), 'ee_fpu_cvt_s_wu': ('i32', 'i32'), 'ee_fpu_cvt_w_s': ('i32', 'i32'),
    'ee_soft_dpadd': ('i64', 'i64 i64'), 'ee_soft_dpsub': ('i64', 'i64 i64'),
    'ee_soft_dpmul': ('i64', 'i64 i64'), 'ee_soft_dpdiv': ('i64', 'i64 i64'),
    'ee_soft_dpcmp': ('i32', 'i64 i64'), 'ee_soft_litodp': ('i64', 'i32'), 'ee_soft_litodp_u': ('i64', 'i32'),
    'ee_soft_dptoli': ('i32', 'i64'), 'ee_soft_dptoul': ('i32', 'i64'), 'ee_soft_fptodp': ('i64', 'i32'),
    'ee_soft_dptofp': ('i32', 'i64'), 'ee_soft_fptoui': ('i32', 'i32'),
    'ee_soft_floatdisf': ('i32', 'i64'), 'ee_soft_floatdisf_u': ('i32', 'i64'),
}
FLOAT_BINOP = {'fadd': 'ee_fpu_add', 'fsub': 'ee_fpu_sub', 'fmul': 'ee_fpu_mul', 'fdiv': 'ee_fpu_div'}
DOUBLE_BINOP = {'fadd': 'ee_soft_dpadd', 'fsub': 'ee_soft_dpsub', 'fmul': 'ee_soft_dpmul', 'fdiv': 'ee_soft_dpdiv'}
# what each routed operation is on the EE, for the report
EE_NAME = {'ee_fpu_add': 'add.s', 'ee_fpu_sub': 'sub.s', 'ee_fpu_mul': 'mul.s', 'ee_fpu_div': 'div.s',
           'ee_fpu_neg': 'neg.s', 'ee_fpu_c_eq': 'c.eq.s', 'ee_fpu_c_lt': 'c.lt.s', 'ee_fpu_c_le': 'c.le.s',
           'ee_fpu_cvt_s_w': 'cvt.s.w', 'ee_fpu_cvt_s_wu': 'cvt.s.w+', 'ee_fpu_cvt_w_s': 'cvt.w.s',
           'ee_soft_dpadd': 'dpadd', 'ee_soft_dpsub': 'dpsub', 'ee_soft_dpmul': 'dpmul',
           'ee_soft_dpdiv': 'dpdiv', 'ee_soft_dpcmp': 'dpcmp', 'ee_soft_litodp': 'litodp',
           'ee_soft_litodp_u': 'litodp+', 'ee_soft_dptoli': 'dptoli', 'ee_soft_dptoul': 'dptoul',
           'ee_soft_fptodp': 'fptodp', 'ee_soft_dptofp': 'dptofp', 'ee_soft_fptoui': 'fptoui',
           'ee_soft_floatdisf': '__floatdisf', 'ee_soft_floatdisf_u': '__floatdisf+'}


class Unsupported(Exception):
    pass


class Lowering:
    def __init__(self):
        self.n = 0
        self.used = set()
        self.counts = defaultdict(Counter)
        self.func = None

    def tmp(self):
        self.n += 1
        return f'%fpu.{self.n}'

    def call(self, out, ind, fn, *args):
        """emit `t = call fn(args)`, args as (type, value); returns t"""
        ret, _ = CALLEES[fn]
        self.used.add(fn)
        self.counts[self.func][EE_NAME[fn]] += 1
        t = self.tmp()
        out.append(f'{ind}{t} = call {ret} @{fn}(' + ', '.join(f'{ty} {v}' for ty, v in args) + ')')
        return t

    def bits(self, out, ind, ty, v):
        """the bit pattern of float/double v"""
        t = self.tmp()
        out.append(f'{ind}{t} = bitcast {ty} {v} to {BITS[ty]}')
        return t

    def to_i32(self, out, ind, ity, v, signed):
        if ity == 'i32':
            return v
        width = int(ity[1:])
        if width > 32:
            raise Unsupported(f'{ity} operand')
        t = self.tmp()
        out.append(f'{ind}{t} = {"sext" if signed else "zext"} {ity} {v} to i32')
        return t

    def lower(self, line):
        """the replacement lines for one instruction, or None to keep it"""
        out = []
        m = BINOP.match(line)
        if m:
            ind, res, op, ty, a, b, meta = m.groups()
            fn = (FLOAT_BINOP if ty == 'float' else DOUBLE_BINOP)[op]
            r = self.call(out, ind, fn, (BITS[ty], self.bits(out, ind, ty, a)), (BITS[ty], self.bits(out, ind, ty, b)))
            out.append(f'{ind}{res} = bitcast {BITS[ty]} {r} to {ty}{meta}')
            return out
        m = NEG.match(line)
        if m:
            ind, res, ty, a, meta = m.groups()
            if ty == 'float':
                r = self.call(out, ind, 'ee_fpu_neg', ('i32', self.bits(out, ind, ty, a)))
            else:   # ee-gcc negates a double as 0.0 - x
                r = self.call(out, ind, 'ee_soft_dpsub', ('i64', '0'), ('i64', self.bits(out, ind, ty, a)))
            out.append(f'{ind}{res} = bitcast {BITS[ty]} {r} to {ty}{meta}')
            return out
        m = CMP.match(line)
        if m:
            ind, res, pred, ty, a, b, meta = m.groups()
            if pred not in FLOAT_CMP:
                raise Unsupported(f'fcmp {pred}')
            x, y = self.bits(out, ind, ty, a), self.bits(out, ind, ty, b)
            if ty == 'float':
                fn, swap, negate = FLOAT_CMP[pred]
                if swap:
                    x, y = y, x
                c = self.call(out, ind, f'ee_fpu_{fn}', ('i32', x), ('i32', y))
                out.append(f'{ind}{res} = icmp {"eq" if negate else "ne"} i32 {c}, 0{meta}')
            else:
                c = self.call(out, ind, 'ee_soft_dpcmp', ('i64', x), ('i64', y))
                out.append(f'{ind}{res} = icmp {DOUBLE_CMP[pred]} i32 {c}, 0{meta}')
            return out
        m = CAST.match(line)
        if m:
            return self.cast(out, *m.groups())
        return None

    def cast(self, out, ind, res, op, src, v, dst, meta):
        if op in ('sitofp', 'uitofp'):
            signed = op == 'sitofp'
            if src == 'i64':
                if dst != 'float':
                    raise Unsupported(f'{op} i64 to {dst}')
                r = self.call(out, ind, 'ee_soft_floatdisf' if signed else 'ee_soft_floatdisf_u', ('i64', v))
            else:
                narrow = src != 'i32'
                w = self.to_i32(out, ind, src, v, signed)
                if dst == 'float':
                    fn = 'ee_fpu_cvt_s_w' if signed or narrow else 'ee_fpu_cvt_s_wu'
                else:
                    fn = 'ee_soft_litodp' if signed or narrow else 'ee_soft_litodp_u'
                r = self.call(out, ind, fn, ('i32', w))
            out.append(f'{ind}{res} = bitcast {BITS[dst]} {r} to {dst}{meta}')
            return out
        if op in ('fptosi', 'fptoui'):
            width = int(dst[1:])
            if width > 32:
                raise Unsupported(f'{op} {src} to {dst}')
            x = self.bits(out, ind, src, v)
            if src == 'float':
                # ee-gcc converts to a narrow unsigned through cvt.w.s too
                fn = 'ee_soft_fptoui' if op == 'fptoui' and width == 32 else 'ee_fpu_cvt_w_s'
            else:
                fn = 'ee_soft_dptoul' if op == 'fptoui' and width == 32 else 'ee_soft_dptoli'
            r = self.call(out, ind, fn, (BITS[src], x))
            if width == 32:
                out.append(f'{ind}{res} = add i32 {r}, 0{meta}')
            else:
                out.append(f'{ind}{res} = trunc i32 {r} to {dst}{meta}')
            return out
        if op == 'fpext' and (src, dst) == ('float', 'double'):
            r = self.call(out, ind, 'ee_soft_fptodp', ('i32', self.bits(out, ind, src, v)))
        elif op == 'fptrunc' and (src, dst) == ('double', 'float'):
            r = self.call(out, ind, 'ee_soft_dptofp', ('i64', self.bits(out, ind, src, v)))
        else:
            raise Unsupported(f'{op} {src} to {dst}')
        out.append(f'{ind}{res} = bitcast {BITS[dst]} {r} to {dst}{meta}')
        return out


def lower_module(text):
    """(rewritten IR, {function: Counter of EE operations})"""
    low = Lowering()
    out = []
    declared = set(re.findall(r'^(?:declare|define) .*?@([-\w.$]+)\(', text, re.M))
    for no, line in enumerate(text.split('\n'), 1):
        d = DEFINE.match(line)
        if d:
            low.func = d.group(1).strip('"')
        try:
            new = low.lower(line) if line.startswith(' ') else None
        except Unsupported as e:
            raise SystemExit(f'line {no}: {e}: {line.strip()}')
        if new is None:
            if line.startswith(' ') and LEFT.search(STRING.sub('', line)):
                raise SystemExit(f'line {no}: unrouted floating point: {line.strip()}')
            out.append(line)
        else:
            out += [l for l in new if l]
    for fn in sorted(low.used - declared):
        ret, params = CALLEES[fn]
        out.append(f'declare {ret} @{fn}({", ".join(params.split())}) nounwind willreturn memory(none)')
    out.append('')
    return '\n'.join(out), low.counts


STATIC = re.compile(r'^(@[-\w.$]+) = internal (?:unnamed_addr |local_unnamed_addr )?global ', re.M)
COMPILER_USED = re.compile(r'^@llvm\.compiler\.used = appending global \[(\d+) x ptr\] \[(.*?)\], section "llvm\.metadata"$',
                           re.M)


def keep_statics(text):
    """List every static variable (internal, not constant) in @llvm.compiler.used.

    The Core keeps the EE's memory layout: wasm_place.py puts each static at its
    PS2 address, and PORT-006's Lockstep Traces compare EE RAM byte for byte.
    LLVM's GlobalOpt may store a static it can see every use of in another form:
    CharIcon's `max`, only ever assigned 0x13, became a one-byte flag holding 1.
    A static in @llvm.compiler.used escapes that analysis, so the optimiser keeps
    the representation the C declared, as EE-GCC did. Its uses are still
    optimised as usual."""
    names = STATIC.findall(text)
    if not names:
        return text
    m = COMPILER_USED.search(text)
    if m:
        items = m.group(2) + ', ' + ', '.join(f'ptr {n}' for n in names)
        line = (f'@llvm.compiler.used = appending global [{int(m.group(1)) + len(names)} x ptr] [{items}], '
                f'section "llvm.metadata"')
        return text[:m.start()] + line + text[m.end():]
    items = ', '.join(f'ptr {n}' for n in names)
    return text + f'@llvm.compiler.used = appending global [{len(names)} x ptr] [{items}], section "llvm.metadata"\n'


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('inp', type=Path)
    ap.add_argument('out', type=Path)
    ap.add_argument('--report', type=Path)
    args = ap.parse_args()

    try:
        text, counts = lower_module(args.inp.read_text(encoding='utf-8'))
        text = keep_statics(text)
    except SystemExit as e:
        sys.exit(f'{args.inp}: {e}')
    args.out.write_text(text, encoding='utf-8', newline='\n')
    if args.report:
        args.report.write_text(json.dumps({f: dict(sorted(c.items())) for f, c in sorted(counts.items())},
                                          indent=1) + '\n', encoding='utf-8', newline='\n')


if __name__ == '__main__':
    main()
