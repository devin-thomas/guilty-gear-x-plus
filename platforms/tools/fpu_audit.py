#!/usr/bin/env python3
"""fpu_audit.py --elf <boot ELF> --symbols <kr|jp.txt> --build <port build dir> [--draw <csv>] [--out <md>]

The PORT-003 float audit: every floating-point operation the Core's C
routes through the EE's rules, next to what the disc's own code does.

The port side is tools/fpu_lower.py's per-TU reports (gen/core/ir/*.fpu.json:
each operation in the source, before optimisation) and the optimised Core
(gen/core/core.linked.wasm). The EE side is the player's boot ELF, decoded
function by function with the symbol map: FPU instructions (add.s, cvt.w.s,
c.lt.s, ...) and calls into the software floating point (dpadd, fptodp,
fptoui, ...). Both are in the EE's terms. Nothing from the ELF is written
out but counts. Three tables:

  1. per TU: the functions with floating point, how many are on the drawing
     path (--draw, PORT-005's draw-inventory.csv), the EE's operations and
     the port's;
  2. every function where the EE executes more of an operation than the
     port's source has. ee-gcc's delay-slot filling, loop rotation, inlining
     and tail merging repeat an operation, and it rewrites some exactly
     (x*2 as x+x, x/2^k as x*2^-k); anything else would mean the two
     compute differently, so each row needs a reason;
  3. every operation left in the optimised Core with only constant operands
     that can round. ee-gcc folds constant expressions at compile time,
     rounding to nearest; the port computes these at run time under the EE's
     rules. Each row needs the EE to compute it at run time too.

The port's source counts exceed the EE's wherever ee-gcc merged a repeated
expression; that is not listed.
"""
import argparse
import csv
import json
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import fpu_check    # noqa: E402
import fpu_lower    # noqa: E402
import wasmfile     # noqa: E402

COP1_S = {0x00: 'add.s', 0x01: 'sub.s', 0x02: 'mul.s', 0x03: 'div.s', 0x04: 'sqrt.s', 0x05: 'abs.s',
          0x07: 'neg.s', 0x16: 'rsqrt.s', 0x18: 'adda.s', 0x19: 'suba.s', 0x1a: 'mula.s', 0x1c: 'madd.s',
          0x1d: 'msub.s', 0x1e: 'madda.s', 0x1f: 'msuba.s', 0x24: 'cvt.w.s', 0x28: 'max.s', 0x29: 'min.s',
          0x30: 'c.f.s', 0x32: 'c.eq.s', 0x34: 'c.lt.s', 0x36: 'c.le.s'}
SOFT = {'dpadd', 'dpsub', 'dpmul', 'dpdiv', 'dpcmp', 'litodp', 'dptoli', 'dptoul', 'fptodp', 'dptofp',
        'fptoui', '__floatdisf', '__floatdidf', '__fixunsdfdi', '__negdf2', 'fpadd', 'fpsub', 'fpmul',
        'fpdiv', 'fpcmp', 'sitofp', 'fptosi', '__negsf2'}
# fpu_lower.py's names for the sequences ee-gcc emits
SEQUENCES = {'cvt.s.w+': {'cvt.s.w': 2, 'add.s': 1}, 'litodp+': {'litodp': 1, 'dpadd': 1},
             '__floatdisf+': {'__floatdisf': 2, 'add.s': 1}}
# operations whose result can't depend on rounding (with the operand ranges
# that keep them exact)
EXACT = {'neg.s', 'c.eq.s', 'c.lt.s', 'c.le.s', 'cvt.w.s', 'dpcmp', 'litodp', 'dptoli', 'dptoul'}
ARITY = {fpu_lower.EE_NAME[fn]: len(params.split()) for fn, (_, params) in fpu_lower.CALLEES.items()}


def functions(path):
    """[(address, name)] of every function in a port symbol map"""
    out = []
    for line in Path(path).read_text().splitlines():
        if line and not line.startswith('#'):
            name, addr, kind, _ = line.split()
            if kind == 'F':
                out.append((int(addr, 16), name))
    return out


def load_segments(elf):
    """[(vaddr, data)] of the ELF's PT_LOAD segments"""
    assert elf[:4] == b'\x7fELF' and elf[4] == 1 and elf[5] == 1, 'not a 32-bit little-endian ELF'
    phoff, = struct.unpack_from('<I', elf, 0x1c)
    phentsize, phnum = struct.unpack_from('<HH', elf, 0x2a)
    segs = []
    for i in range(phnum):
        ptype, off, vaddr, _, filesz = struct.unpack_from('<IIIII', elf, phoff + i * phentsize)
        if ptype == 1:
            segs.append((vaddr, elf[off:off + filesz]))
    return segs


def word_reader(segs):
    def word(addr):
        for vaddr, data in segs:
            if vaddr <= addr < vaddr + len(data) - 3:
                return struct.unpack_from('<I', data, addr - vaddr)[0]
        return None
    return word


def ee_counts(word, start, end, names_at):
    """Counter of FPU instructions and software floating point calls in [start, end)"""
    c = Counter()
    for pc in range(start, end, 4):
        w = word(pc)
        if w is None:
            break
        op = w >> 26
        if op == 0x11:
            fmt = (w >> 21) & 0x1f
            if fmt == 0x10 and (w & 0x3f) in COP1_S:
                c[COP1_S[w & 0x3f]] += 1
            elif fmt == 0x14 and (w & 0x3f) == 0x20:
                c['cvt.s.w'] += 1
        elif op == 0x03:
            target = ((w & 0x3ffffff) << 2) | (pc & 0xf0000000)
            for n in names_at.get(target, ()):
                if n in SOFT:
                    c[n] += 1
    return c


def port_counts(raw):
    c = Counter()
    for op, n in raw.items():
        for k, m in SEQUENCES.get(op, {op: 1}).items():
            c[k] += n * m
    return c


def can_round(label, args):
    if label in EXACT:
        return False
    if label == 'cvt.s.w':
        return abs(args[0]) > 1 << 24
    if label == 'fptodp':     # exact for a normal float or a zero
        e = (args[0] >> 23) & 0xff
        return e == 0 and args[0] & 0x7fffffff
    return True


def fmt_counter(c):
    return ', '.join(f'{k} {n}' for k, n in sorted(c.items()) if n) or '-'


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--elf', type=Path, required=True)
    ap.add_argument('--symbols', type=Path, required=True)
    ap.add_argument('--build', type=Path, required=True)
    ap.add_argument('--draw', type=Path)
    ap.add_argument('--out', type=Path)
    args = ap.parse_args()

    names_at = defaultdict(list)
    for addr, name in functions(args.symbols):
        names_at[addr].append(name)
    ranges = defaultdict(list)          # name -> [(start, end)]
    starts = sorted(names_at)
    for i, a in enumerate(starts):
        end = starts[i + 1] if i + 1 < len(starts) else a + 4
        for n in names_at[a]:
            ranges[n].append((a, end))
    word = word_reader(load_segments(args.elf.read_bytes()))

    draw = {}
    if args.draw:
        with open(args.draw, newline='', encoding='utf-8') as f:
            for row in csv.DictReader(f):
                draw[row['function']] = f"{row['class']} {row['role']}"

    rows, more, unmapped = [], [], []
    totals_ee, totals_port = Counter(), Counter()
    tu_of = {}
    for rep in sorted((args.build / 'gen' / 'core' / 'ir').glob('*.fpu.json')):
        tu = rep.name[:-len('.fpu.json')]
        funcs = json.loads(rep.read_text(encoding='utf-8'))
        if not funcs:
            continue
        tu_ee, tu_port, ndraw, nfunc = Counter(), Counter(), 0, 0
        for fn, raw in sorted(funcs.items()):
            tu_of[fn] = tu
            port = port_counts(raw)
            tu_port += port
            nfunc += 1
            ndraw += fn in draw
            if len(ranges.get(fn, ())) != 1:
                unmapped.append(f'{tu}:{fn}' + (f' (name used {len(ranges[fn])} times)' if fn in ranges else ''))
                continue
            ee = ee_counts(word, *ranges[fn][0], names_at)
            tu_ee += ee
            if ee - port:
                more.append((tu, fn, draw.get(fn, ''), ee - port, port - ee))
        totals_ee += tu_ee
        totals_port += tu_port
        rows.append((tu, nfunc, ndraw, tu_ee, tu_port))

    consts = []
    module = wasmfile.Module((args.build / 'gen' / 'core' / 'core.linked.wasm').read_bytes())

    def on_call(fn, label, c):
        n = ARITY[label]
        if len(c) >= n and can_round(label, c[-n:]):
            consts.append((tu_of.get(fn, ''), fn, label, c[-n:]))
    fpu_check.scan(module, fpu_lower.EE_NAME, on_call)

    out = ['### Routed, per TU', '',
           '| TU | functions with floating point | on the draw path | EE: FPU instructions and software '
           'floating point calls | port: operations routed in the source |',
           '| --- | ---: | ---: | --- | --- |']
    for tu, nfunc, ndraw, ee, port in rows:
        out.append(f'| `{tu}` | {nfunc} | {ndraw} | {fmt_counter(ee)} | {fmt_counter(port)} |')
    out += ['', f'All TUs, EE: {fmt_counter(totals_ee)}.', '', f'All TUs, port: {fmt_counter(totals_port)}.']
    out += ['', '### Where the EE does more than the source', '',
            '| TU | function | draw path | the EE has more | the port has more |', '| --- | --- | --- | --- | --- |']
    for tu, fn, d, ee_more, port_more in more:
        out.append(f'| `{tu}` | `{fn}` | {d} | {fmt_counter(ee_more)} | {fmt_counter(port_more)} |')
    out += ['', '### Constant operations left in the optimised Core that can round', '']
    if consts:
        out += ['| TU | function | operation | operands |', '| --- | --- | --- | --- |']
        for tu, fn, label, c in consts:
            out.append(f'| `{tu}` | `{fn}` | {label} | ' + ', '.join(f'0x{v & 0xffffffffffffffff:x}' for v in c) +
                       ' |')
    else:
        out.append('None.')
    if unmapped:
        out += ['', 'Not compared (not in the symbol map: port code, or helpers ee-gcc inlined everywhere): ' +
                ', '.join(f'`{u}`' for u in unmapped) + '.']
    text = '\n'.join(out) + '\n'
    if args.out:
        args.out.write_text(text, encoding='utf-8', newline='\n')
    else:
        sys.stdout.write(text)


if __name__ == '__main__':
    main()
