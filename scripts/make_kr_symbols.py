#!/usr/bin/env python3
"""Write kr/symbol_addrs.txt from the Korean ELF's own symbol table.

SLPM-645.25 shipped unstripped. Its .symtab names every function and most data,
so the splat symbol list is the disc's .symtab, filtered:

- functions: STT_FUNC in .text, written with `// type:func`;
- data: STT_OBJECT in a section, and the .bss/.sbss labels the compiler emits
  as STT_NOTYPE. Absolute linker symbols (_end, _stack_size, ...) are not
  addresses in the image and are left out, except _gp, the small-data base.
  e_pow.c's static coefficients L2-L6 are left out (its C owns them; only L1
  is listed);
- one entry per name. Where several local statics share a name (`idtb`,
  `functb.21`, ...) the lowest address keeps the bare name and the others are
  left unnamed, except where KEEP picks another copy (the one this tree's
  matched C was verified against);
- crt0's entry function is written under the ELF's entry alias ENTRYPOINT
  (same address as _start);

Two kinds of name are this project's own, not the disc's:
- RENAME: same-named local functions of different SDK objects (Copy, kCopy,
  setup, GetEntryAddress, _fpadd_parts) get the object's name as a prefix, so
  each one links as its own symbol;
- EE hardware registers named in src/sdk/sce/eeregs.h, the tree's own header
  (public EE hardware map names).
Jump tables get no entry; splat names the ones it finds itself.

  make_kr_symbols.py --elf SLPM_645.25 --out kr/symbol_addrs.txt [--check]
"""
import argparse
import os
import re
from collections import defaultdict

from elftools.elf.elffile import ELFFile

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

RENAME = {
    0x1c2d40: 'fpbit_fpadd_parts',
    0x1d2248: 'initsys_Copy', 0x1d2258: 'initsys_kCopy', 0x1d2418: 'initsys_setup',
    0x1d2468: 'libosd_setup', 0x1d2478: 'libosd_Copy', 0x1d2488: 'libosd_kCopy',
    0x1d24c0: 'libosd_GetEntryAddress',
    0x1d25e8: 'exit_Copy', 0x1d25f8: 'exit_kCopy', 0x1d2628: 'exit_setup',
    0x1d2c90: 'tlbfunc_Copy', 0x1d2ca0: 'tlbfunc_kCopy', 0x1d2cd8: 'tlbfunc_GetEntryAddress',
    0x1d2ce8: 'tlbfunc_setup',
}
KEEP = {}            # name -> address of the copy that keeps the bare name
SKIP = {('ENTRYPOINT', 0x100008)} | {(f'L{i}', 0x5cae78 + 8 * (i - 1)) for i in range(2, 7)}
ALIAS = {('_start', 0x100008): 'ENTRYPOINT'}   # the entry function under its linker alias


def disc_symbols(elf):
    with open(elf, 'rb') as f:
        e = ELFFile(f)
        secs = {i: s.name for i, s in enumerate(e.iter_sections())}
        rows = [(s.name, s['st_value'], s['st_info']['type'], secs.get(s['st_shndx'], s['st_shndx']))
                for s in e.get_section_by_name('.symtab').iter_symbols()]
    out = []
    for name, addr, typ, sec in rows:
        if not name or name.startswith('$') or (name, addr) in SKIP:
            continue
        name = ALIAS.get((name, addr), name)
        if name == '_gp':
            out.append((name, addr, False))
        elif typ == 'STT_FUNC' and sec == '.text':
            out.append((name, addr, True))
        elif typ == 'STT_OBJECT' and isinstance(sec, str) and sec.startswith('.'):
            out.append((name, addr, False))
        elif typ == 'STT_NOTYPE' and sec in ('.bss', '.sbss'):
            out.append((name, addr, False))
    return out


def register_names():
    regs = {}
    text = open(os.path.join(HERE, 'src/sdk/sce/eeregs.h')).read()
    for name, addr in re.findall(r'^#define\s+(\w+)\s+\(\([^()]*\)\s*(0x1[0-9a-fA-F]{7})\)', text, re.M):
        regs.setdefault(int(addr, 16), name)
    return regs


def build(elf):
    syms = disc_symbols(elf)
    by_name = defaultdict(list)
    for s in syms:
        by_name[s[0]].append(s)
    entries = {}
    for name, copies in by_name.items():
        for s in copies:
            if s[1] in RENAME:
                entries[s[1]] = (RENAME[s[1]], s[2])
        rest = [s for s in copies if s[1] not in RENAME]
        if rest:
            keep = KEEP.get(name, min(s[1] for s in rest))
            s = next(s for s in rest if s[1] == keep)
            entries.setdefault(s[1], (name, s[2]))
    for addr, name in register_names().items():
        entries.setdefault(addr, (name, False))
    return sorted((addr, name, func) for addr, (name, func) in entries.items())


def render(entries):
    return ''.join(f'{name} = 0x{addr:08x};' + (' // type:func' if func else '') + '\n'
                   for addr, name, func in entries)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--elf', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--check', action='store_true', help='compare with --out instead of writing it')
    a = ap.parse_args()
    text = render(build(a.elf))
    if a.check:
        old = open(a.out).read().replace('\r\n', '\n')
        print('identical' if old == text else 'DIFFERS')
        return 0 if old == text else 1
    open(a.out, 'w', newline='\n').write(text)
    print(text.count('\n'), 'entries')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
