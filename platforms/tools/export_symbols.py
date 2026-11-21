#!/usr/bin/env python3
"""export_symbols.py <linked.elf> <kr|jp> <out.txt>: the port's PS2 symbol map for one disc.

build.py links every matched object at its original address into
<work>/build/linked.elf. That ELF's symbol table is the only complete list
of where the game's names live on the disc: symbol_addrs.txt has the named
functions and globals, but not the addresses build.py solves while matching
(D_ labels, which are named after the JP layout, and the STATIC_EXTERN
names of function statics) or the file statics compiled from C.

The output keeps names and addresses only, no bytes:
    <name> <address> <F|D> <G|L>
    <name> - - R
F is a function, D anything else; G a global or solved (absolute) symbol,
L a file static. R marks a file static name that occurs in more than one
TU (idtb is in nine), so the name alone can't say which one is meant;
wasm_place.py keeps such statics in wasm-ld's placement. splat's
.NON_MATCHING aliases, build.py's __rd_ data-fragment labels and symbols
in fragments it left unplaced (outside EE RAM) are left out.
"""
import struct
import sys
from collections import defaultdict
from pathlib import Path

STT_OBJECT, STT_FUNC, STT_SECTION, STT_FILE = 1, 2, 3, 4
STB_LOCAL, STB_GLOBAL, STB_WEAK = 0, 1, 2
SHN_UNDEF, SHN_ABS = 0, 0xfff1
EE_RAM_END = 0x02000000
TITLES = {'kr': 'SLPM_645.25', 'jp': 'SLPS_201.37'}


def symbols(path):
    d = Path(path).read_bytes()
    assert d[:4] == b'\x7fELF' and d[4] == 1 and d[5] == 1, 'ELF32 little-endian'
    shoff, = struct.unpack_from('<I', d, 0x20)
    shentsize, shnum = struct.unpack_from('<HH', d, 0x2e)
    secs = [struct.unpack_from('<10I', d, shoff + i * shentsize) for i in range(shnum)]
    code = [(s[3], s[3] + s[5]) for s in secs if s[2] & 4]        # SHF_EXECINSTR
    in_code = lambda v: any(lo <= v < hi for lo, hi in code)
    for name, stype, flags, addr, off, size, link, info, align, entsize in secs:
        if stype != 2:          # SHT_SYMTAB
            continue
        strtab = secs[link]
        for i in range(size // 16):
            st_name, value, st_size, st_info, st_other, shndx = struct.unpack_from('<IIIBBH', d, off + i * 16)
            end = d.index(b'\0', strtab[4] + st_name)
            kind = st_info & 0xf
            if kind == 0 and in_code(value):      # a solved or splat label in .text (func_001BBF50)
                kind = STT_FUNC
            yield d[strtab[4] + st_name:end].decode(), value, kind, st_info >> 4, shndx


def main():
    elf, target, out = sys.argv[1], sys.argv[2], Path(sys.argv[3])
    globals_, locals_ = {}, defaultdict(set)
    for name, value, stype, bind, shndx in symbols(elf):
        if not name or stype in (STT_SECTION, STT_FILE) or shndx == SHN_UNDEF:
            continue
        # build.py's own labels: data fragments (__rd_), splat aliases
        if name.startswith(('.', '$', '__rd_')) or name.endswith('.NON_MATCHING') or name in ('_MACRO_INC_GUARD', '_gp'):
            continue
        # build.py parks unplaced fragments in sections far outside EE RAM
        # (0x0f810000, 0x18260000): those addresses mean nothing on the PS2
        if shndx != SHN_ABS and value >= EE_RAM_END:
            continue
        kind = 'F' if stype == STT_FUNC else 'D'
        if bind == STB_LOCAL and shndx != SHN_ABS:
            locals_[(name, kind)].add(value)
        else:
            if name in globals_ and globals_[name] != (value, kind):
                raise SystemExit(f'{name}: two global definitions')
            globals_[name] = (value, kind)
    rows = [(n, v, k, 'G') for n, (v, k) in globals_.items()]
    rows += [(n, next(iter(vs)), k, 'L') for (n, k), vs in locals_.items() if len(vs) == 1]
    repeated = sorted({n for (n, _), vs in locals_.items() if len(vs) > 1})
    lines = [f'# {TITLES[target]} symbol map for the port, from the matching build\'s linked ELF,',
             '# by platforms/tools/export_symbols.py. <name> <address> <F|D> <G|L|R>. Names and addresses only.']
    lines += [f'{n} 0x{v:08x} {k} {b}' for n, v, k, b in sorted(rows, key=lambda r: (r[1], r[0], r[3]))]
    lines += [f'{n} - - R' for n in repeated]
    out.write_text('\n'.join(lines) + '\n', newline='\n')
    print(f'{len(globals_)} global, {len(rows) - len(globals_)} unique file statics, '
          f'{len(repeated)} repeated file static names')


if __name__ == '__main__':
    main()
