#!/usr/bin/env python3
"""fpu_check.py <core.wasm>: fail if the Core does any floating point natively.

tools/fpu_lower.py routes every floating-point operation in the Core's C
through the EE-FPU Model or the EE's software floating point, all of it
integer code. This checks the linked module: it decodes every function body
and lists each floating-point arithmetic, compare or conversion instruction
it finds, by function. Moving a float's bits (loads, stores, constants,
reinterpret, select) is fine, and so are abs, neg and copysign: WebAssembly
defines them on the sign bit alone, with no rounding and no NaN handling,
which is what the EE's abs.s and neg.s do too. LLVM's optimiser makes them
out of C that masks a sign bit (fdlibm's fabs and atan2). Exit status 1 if
any other is found.
"""
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import wasmfile   # noqa: E402

F32 = ['abs', 'neg', 'ceil', 'floor', 'trunc', 'nearest', 'sqrt', 'add', 'sub', 'mul', 'div', 'min', 'max',
       'copysign']
CMP = ['eq', 'ne', 'lt', 'gt', 'le', 'ge']
FLOAT_OPS = {}
for i, n in enumerate(CMP):
    FLOAT_OPS[0x5b + i] = f'f32.{n}'
    FLOAT_OPS[0x61 + i] = f'f64.{n}'
for i, n in enumerate(F32):
    FLOAT_OPS[0x8b + i] = f'f32.{n}'
    FLOAT_OPS[0x99 + i] = f'f64.{n}'
FLOAT_OPS.update({
    0xa8: 'i32.trunc_f32_s', 0xa9: 'i32.trunc_f32_u', 0xaa: 'i32.trunc_f64_s', 0xab: 'i32.trunc_f64_u',
    0xae: 'i64.trunc_f32_s', 0xaf: 'i64.trunc_f32_u', 0xb0: 'i64.trunc_f64_s', 0xb1: 'i64.trunc_f64_u',
    0xb2: 'f32.convert_i32_s', 0xb3: 'f32.convert_i32_u', 0xb4: 'f32.convert_i64_s',
    0xb5: 'f32.convert_i64_u', 0xb6: 'f32.demote_f64', 0xb7: 'f64.convert_i32_s',
    0xb8: 'f64.convert_i32_u', 0xb9: 'f64.convert_i64_s', 0xba: 'f64.convert_i64_u',
    0xbb: 'f64.promote_f32'})
SAT = ['i32.trunc_sat_f32_s', 'i32.trunc_sat_f32_u', 'i32.trunc_sat_f64_s', 'i32.trunc_sat_f64_u',
       'i64.trunc_sat_f32_s', 'i64.trunc_sat_f32_u', 'i64.trunc_sat_f64_s', 'i64.trunc_sat_f64_u']
BLOCKTYPES = {0x40, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x70, 0x6f}
SIGN_BIT_OPS = {'f32.abs', 'f32.neg', 'f32.copysign', 'f64.abs', 'f64.neg', 'f64.copysign'}


def float_ops(r, end, calls=None, on_call=None):
    """Counter of floating-point instructions in one function body's code;
    with calls ({function index: name}), of calls to those functions too.
    on_call(label, consts) sees each such call with the integer constants
    pushed immediately before it, in order: all its operands when they are
    all constants, fewer when some are computed."""
    found = Counter()
    consts = []
    while r.pos < end:
        op = r.u8()
        if op in (0x41, 0x42):
            consts.append(r.sleb())
            continue
        if op in FLOAT_OPS:
            if FLOAT_OPS[op] not in SIGN_BIT_OPS:
                found[FLOAT_OPS[op]] += 1
        elif op in (0x02, 0x03, 0x04):
            if r.data[r.pos] in BLOCKTYPES:
                r.u8()
            else:
                r.sleb()
        elif op == 0x10:
            callee = r.uleb()
            if calls and callee in calls:
                found[calls[callee]] += 1
                if on_call:
                    on_call(calls[callee], consts)
        elif op in (0x0c, 0x0d, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0xd2):
            r.uleb()
        elif op == 0x0e:
            for _ in range(r.uleb() + 1):
                r.uleb()
        elif op == 0x11:
            r.uleb()
            r.uleb()
        elif op == 0x1c:
            for _ in range(r.uleb()):
                r.u8()
        elif 0x28 <= op <= 0x3e:
            r.uleb()
            r.uleb()
        elif op in (0x3f, 0x40, 0xd0):
            r.u8()
        elif op == 0x43:
            r.bytes(4)
        elif op == 0x44:
            r.bytes(8)
        elif op == 0xfc:
            sub = r.uleb()
            if sub < 8:
                found[SAT[sub]] += 1
            elif sub == 8:
                r.uleb()
                r.u8()
            elif sub in (9, 13, 15, 16, 17):
                r.uleb()
            elif sub == 10:
                r.u8()
                r.u8()
            elif sub == 11:
                r.u8()
            elif sub in (12, 14):
                r.uleb()
                r.uleb()
            else:
                raise ValueError(f'0xfc {sub}')
        elif op in (0x00, 0x01, 0x05, 0x0b, 0x0f, 0x1a, 0x1b, 0xd1) or 0x45 <= op <= 0xc4:
            pass
        else:
            raise ValueError(f'opcode 0x{op:02x} at {r.pos - 1}')
        consts = []
    return found


def scan(module, calls=None, on_call=None):
    """{function name: Counter} for every function with floating point (or
    calls to `calls`, {callee name: label}); on_call(function, label, consts)"""
    names = module.function_names()
    by_index = {i: calls[n] for i, n in names.items() if calls and n in calls}
    s = module.section(wasmfile.SEC_CODE)
    r = wasmfile.Reader(module.data, s.start, s.end)
    bad = {}
    for k in range(r.uleb()):
        size = r.uleb()
        end = r.pos + size
        for _ in range(r.uleb()):   # local declarations
            r.uleb()
            r.u8()
        idx = module.num_imported_funcs + k
        name = names.get(idx, f'function {idx}')
        found = float_ops(r, end, by_index, on_call and (lambda label, c: on_call(name, label, c)))
        r.pos = end
        if found:
            bad[name] = found
    return bad


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    bad = scan(wasmfile.Module(Path(sys.argv[1]).read_bytes()))
    for name, found in sorted(bad.items()):
        print(f'{name}: ' + ', '.join(f'{op} x{n}' for op, n in sorted(found.items())))
    if bad:
        print(f'{len(bad)} Core functions do floating point natively; route them through the EE-FPU Model')
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
