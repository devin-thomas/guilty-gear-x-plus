#!/usr/bin/env python3
"""Sony libkernl's syscall stubs (TU sdk/asm/klib), generated from a table.

src/sdk/asm/klib.tbl lists the stubs in link order: a syscall number, a name and,
for a stub only one disc links, that disc. Every stub is the same four
instructions, so the assembly is written here at build time, the way PS2SDK
writes its kernel stubs from a list of names and numbers:

    li      $v1, <number>       (addiu $v1, $zero, <number>)
    syscall
    jr      $ra
    nop

build.py calls functions(target) and assembles each stub in its own
.text.<name> section. Nothing generated is kept in git.

  klib.py check --target <jp|kr> --elf <executable>

checks the table against a disc without writing anything: each stub's address
(from <target>/symbol_addrs.txt) is read from the ELF and decoded, and the stubs
must fill the TU's span in the splat layout exactly, in table order. It prints
one line per mismatch and a summary, never the disc's bytes.
"""
import argparse
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TU = 'sdk/asm/klib'
TABLE = ROOT / 'src' / f'{TU}.tbl'
ROW = re.compile(r'^\s*(-?\d+)\s+([A-Za-z_]\w*)(?:\s+(jp|kr))?\s*$')
STUB_BYTES = 16
SYSCALL, JR_RA, NOP = 0x0000000C, 0x03E00008, 0x00000000


def read_table(path=TABLE):
    """[(number, name, disc or None)] in table order. Comments (#) and blank lines are skipped;
    anything else that is not a row is an error."""
    rows, names = [], set()
    for i, line in enumerate(Path(path).read_text().splitlines(), 1):
        if not line.strip() or line.lstrip().startswith('#'):
            continue
        m = ROW.match(line)
        if not m:
            raise ValueError(f'{path}:{i}: not a "number name [jp|kr]" row: {line!r}')
        number, name, disc = int(m.group(1)), m.group(2), m.group(3)
        if not -0x8000 <= number < 0x8000:
            raise ValueError(f'{path}:{i}: syscall number {number} does not fit li\'s 16 bits')
        if name in names:
            raise ValueError(f'{path}:{i}: {name} listed twice')
        names.add(name)
        rows.append((number, name, disc))
    return rows


def stubs(target, path=TABLE):
    """[(name, number)] for one disc, in link order."""
    return [(name, number) for number, name, disc in read_table(path) if disc in (None, target)]


def stub_asm(name, number):
    """One stub in its own section, as GAS source."""
    return (f'\t.set\tnoreorder\n'
            f'\t.section .text.{name}, "ax", @progbits\n'
            f'\t.align\t2\n'
            f'\t.globl\t{name}\n'
            f'\t.type\t{name}, @function\n'
            f'\t.ent\t{name}\n'
            f'{name}:\n'
            f'\taddiu\t$3, $0, {number}\n'
            f'\tsyscall\n'
            f'\tjr\t$31\n'
            f'\tnop\n'
            f'\t.end\t{name}\n'
            f'\t.size\t{name}, . - {name}\n')


def functions(target):
    """[(name, assembly)] for build.py: one source per stub, in link order."""
    return [(name, stub_asm(name, number)) for name, number in stubs(target)]


def expected_words(number):
    return (0x24030000 | (number & 0xFFFF), SYSCALL, JR_RA, NOP)


def check(target, elf_path):
    """Compare the table with the disc. Returns a list of problems (empty: all good)."""
    import yaml
    from elftools.elf.elffile import ELFFile

    sym_file = ROOT / ('kr/symbol_addrs.txt' if target == 'kr' else 'jp/symbol_addrs.txt')
    yaml_file = ROOT / ('kr/slpm_645.25.yaml' if target == 'kr' else 'jp/slps_201.37.yaml')
    syms = {m.group(1): int(m.group(2), 16) for m in
            re.finditer(r'^(\w+)\s*=\s*(0x[0-9A-Fa-f]+);', sym_file.read_text(), re.M)}
    config = yaml.safe_load(yaml_file.read_text())
    text = [s for s in config['segments'] if isinstance(s, dict) and s.get('name') == 'text'][0]
    subs = text['subsegments']
    k = next(i for i, s in enumerate(subs) if s[2] == TU)
    delta = text['vram'] - text['start']
    lo, hi = subs[k][0] + delta, subs[k + 1][0] + delta
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)
        sec = elf.get_section_by_name('.text')
        base, data = sec['sh_addr'], sec.data()
    problems, at = [], lo
    for name, number in stubs(target):
        addr = syms.get(name)
        if addr is None:
            problems.append(f'{name}: no address in {sym_file.name}')
            continue
        if addr != at:
            problems.append(f'{name}: at 0x{addr:08x}, expected 0x{at:08x} (table order)')
        got = struct.unpack_from('<4I', data, addr - base)
        want = expected_words(number)
        if got != want:
            bad = [('li $v1', 'syscall', 'jr $ra', 'nop')[i] for i in range(4) if got[i] != want[i]]
            problems.append(f'{name} ({number}): instruction(s) differ: {", ".join(bad)}')
        at = addr + STUB_BYTES
    if at != hi:
        problems.append(f'the stubs end at 0x{at:08x}, but {TU} ends at 0x{hi:08x}')
    return problems


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    c = sub.add_parser('check', help='check the table against a disc executable')
    c.add_argument('--target', choices=('jp', 'kr'), required=True)
    c.add_argument('--elf', required=True, type=Path)
    s = sub.add_parser('asm', help='print the generated assembly for one disc')
    s.add_argument('--target', choices=('jp', 'kr'), required=True)
    a = ap.parse_args()
    if a.cmd == 'asm':
        sys.stdout.write(''.join(src for _, src in functions(a.target)))
        return 0
    problems = check(a.target, a.elf)
    for p in problems:
        print(p)
    n = len(stubs(a.target))
    print(f'{a.target}: {n} stubs, ' + ('all match the disc' if not problems else f'{len(problems)} problems'))
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
