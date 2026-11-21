#!/usr/bin/env python3
"""wasm_place.py: give the linked Core module the PS2's own address layout.

    wasm_place.py --symbols platforms/common/core/symbols/kr.txt --objects objs.rsp in.wasm out.wasm [--report r.txt]

The Core's linear memory is EE RAM: the host loads the player's ELF at its
PS2 addresses, and every game pointer is a PS2 address. wasm-ld places
data and numbers functions its own way, so this pass, run on a module linked
with --emit-relocs, rewrites every relocated reference:

- a data symbol with a PS2 address (the symbol map from export_symbols.py)
  is referred to at that address, so code reads and writes the ELF image's
  bytes;
- a function with a PS2 address has that address as its "pointer": every
  table-index reference becomes the address, and the function is put in the
  table at that index, so a function pointer read from PS2 data calls the
  right function.

A static of one TU, a file static or a function-local one (clang's
`<function>.<var>`), is placed by its TU and name through
common/core/symbols/statics_<release>.txt (tools/statics.py, from the KR
disc's stabs), before any lookup by name: a file static's name can be another
TU's global. Symbols without a PS2 address (string literals, port code, the
statics the table leaves out) keep wasm-ld's placement above EE RAM. So does a file static
whose name is defined in more than one Core object, or in more than one PS2
TU: its name can't say which one is meant. The report lists them.
"""
import argparse
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import symmap   # noqa: E402
import wasmfile as W   # noqa: E402


def tu_of(path):
    """The game TU a Core object is: gen/core/obj/ggxeuc_select.o -> select, sdk_libm_e_pow.o -> e_pow."""
    stem = Path(path).stem
    if stem.startswith('ggxeuc_'):
        return stem[len('ggxeuc_'):]
    if stem.startswith('sdk_'):
        return stem.split('_', 2)[2]
    return stem


def local_definitions(objects):
    """How many objects define each (kind, name), and the TUs that define each local data name."""
    seen = Counter()
    owners = defaultdict(set)
    for p in objects:
        m = W.Module(Path(p).read_bytes())
        names = set()
        for s in m.symbols:
            # gen_data_syms.py's weak placeholders are not definitions here
            if s.kind in (W.SYM_DATA, W.SYM_FUNCTION) and not s.undefined and s.name                     and not s.flags & W.FLAG_BINDING_WEAK:
                names.add((s.kind, s.name, s.local))
        for kind, name, local in names:
            seen[(kind, name)] += 1
            if local and kind == W.SYM_DATA:
                owners[name].add(tu_of(p))
    return seen, owners


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--symbols', required=True)
    ap.add_argument('--objects', required=True, help='file listing the linked objects, one per line')
    ap.add_argument('--report')
    ap.add_argument('input')
    ap.add_argument('output')
    args = ap.parse_args()

    ps2 = symmap.SymbolMap(args.symbols)
    objects = [l.strip() for l in Path(args.objects).read_text().splitlines() if l.strip()]
    defs, owners = local_definitions(objects)
    m = W.Module(Path(args.input).read_bytes())
    if not m.relocs:
        raise SystemExit(f'{args.input}: no relocations; link with --emit-relocs')

    # data: by symbol, since only relocations refer to data
    data_at, ambiguous = {}, []
    for i, s in enumerate(m.symbols):
        if s.kind != W.SYM_DATA or not s.name:
            continue
        # a static of one TU: its own address from the disc's stabs (statics.py), whatever other TUs use the name
        if s.local and len(owners[s.name]) == 1:
            addr = ps2.static(next(iter(owners[s.name])), s.name)
            if addr is not None:
                data_at[i] = addr
                continue
        if s.local and (defs[(s.kind, s.name)] > 1 or s.name in ps2.repeated):
            ambiguous.append(s.name)
            continue
        addr = ps2.address(s.name, False, s.local)
        if addr is not None:
            data_at[i] = addr

    # functions: by function index, from the name section, which names every
    # function; the symbol table only has the ones some relocation refers to,
    # and a function reached only through PS2 data (an action table entry)
    # has none
    func_addr = {}
    for fidx, name in m.function_names().items():
        if fidx < m.num_imported_funcs:
            continue
        if defs[(W.SYM_FUNCTION, name)] > 1 or name in ps2.repeated:
            ambiguous.append(name)
            continue
        addr = ps2.address(name, True, False)
        if addr is not None:
            func_addr[fidx] = addr
    func_at = {i: (func_addr[s.index], s.index) for i, s in enumerate(m.symbols)
               if s.kind == W.SYM_FUNCTION and not s.undefined and s.index in func_addr}

    # rewrite the relocated fields in place
    counts = Counter()
    for sec_index, entries in m.relocs.items():
        base = m.sections[sec_index].start
        for rtype, offset, index, addend in entries:
            at = base + offset
            if rtype in W.MEMORY_RELOCS and index in data_at:
                value = data_at[index] + addend
                counts['data'] += 1
            elif rtype in W.TABLE_RELOCS and index in func_at:
                value = func_at[index][0]
                counts['func'] += 1
            else:
                continue
            if rtype in (W.R_MEMORY_ADDR_I32, W.R_TABLE_INDEX_I32):
                struct.pack_into('<I', m.data, at, value)
            elif rtype == W.R_MEMORY_ADDR_LEB:
                m.data[at:at + 5] = W.uleb_bytes(value, 5)
            else:
                m.data[at:at + 5] = W.sleb_bytes(value, 5)

    # every PS2-addressed function goes in the table at its address (one
    # segment each); wasm-ld's own segment stays for everything else
    placed = sorted((addr, fidx) for fidx, addr in func_addr.items())
    elem = m.section(W.SEC_ELEM)
    table = m.section(W.SEC_TABLE)
    if elem is None or table is None:
        raise SystemExit('module has no table or element section')
    r = W.Reader(m.data, elem.start, elem.end)
    count = r.uleb()
    body = bytes(m.data[r.pos:elem.end])
    for addr, fidx in placed:
        body += b'\0' + b'\x41' + W.sleb_bytes(addr) + b'\x0b' + W.uleb_bytes(1) + W.uleb_bytes(fidx)
    new_elem = W.uleb_bytes(count + len(placed)) + body

    r = W.Reader(m.data, table.start, table.end)
    assert r.uleb() == 1, 'one table'
    reftype = r.u8()
    _, lo, hi = W.Module._limits(r)
    size = max([lo] + [a + 1 for a, _ in placed])
    new_table = W.uleb_bytes(1) + bytes([reftype, 1]) + W.uleb_bytes(size) + W.uleb_bytes(size)

    out = bytearray(m.data[:8])
    for s in m.sections:
        if s.id == 0 and s.name == 'producers':
            continue    # the compiler's version label, which differs between builds of the same LLVM
        payload = bytes(m.data[s.start:s.end])
        if s is elem:
            payload = new_elem
        elif s is table:
            payload = new_table
        out += W.section_bytes(s.id, payload)
    Path(args.output).write_bytes(out)

    if args.report:
        lines = [f'data references moved: {counts["data"]}', f'function references moved: {counts["func"]}',
                 f'data symbols at PS2 addresses: {len(data_at)}', f'functions in the table at PS2 addresses: {len(placed)}',
                 f'table size: {size}', f'ambiguous file statics (kept in wasm placement): {len({n for n in ambiguous if not n.startswith(".L")})}']
        lines += ['    ' + n for n in sorted(set(ambiguous)) if not n.startswith('.L')]
        Path(args.report).write_text('\n'.join(lines) + '\n')


if __name__ == '__main__':
    main()
