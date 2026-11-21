#!/usr/bin/env python3
"""statics.py <KR boot ELF> <JP boot ELF>: where the game's static variables live on each disc.

    python platforms/tools/statics.py <SLPM_645.25> <SLPS_201.37>
    writes platforms/common/core/symbols/statics_kr.txt and statics_jp.txt

The Core keeps every game variable at its PS2 address (wasm_place.py), so
EE RAM can be compared with the PS2's byte for byte (PORT-006). The symbol
maps can't place two kinds of static by name:

- a function-local static: clang names it `<function>.<var>`, EE-GCC
  `<var>.<N>`;
- a file static whose name another TU also uses (`count` is select.c's and
  kprintf's): the map has one address per name, and wasm_place.py used to
  give the Core's file static the other TU's global of that name.

SLPM-645.25 (KR) shipped with its ECOFF .mdebug, which holds GCC's stabs:
each source file, each function in it, and every static with its address.
That gives the KR table. The JP ELF is stripped; its addresses come from the
KR ones through code references (scripts/xref.py's method): in every function
pair data/align_jp.csv grades identical up to relocations, the addresses the
two discs' instructions build are paired up. A static the votes don't reach
is placed between two that move by the same distance, or by a name both
symbol maps give it, or with its TU's nearest placed statics in the same
section (a TU keeps its statics together), or left out.

Each table line is `<TU> <Core name> <address>`: `select count 0x005ce008`,
`select CharIcon.max 0x005cdfa4`. Names and addresses only. A static
declared in more than one block of one function is left out (clang numbers
those), and so is anything the .mdebug has no address for.
"""
import bisect
import csv
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

PLAT = Path(__file__).resolve().parent.parent
DECOMP = PLAT.parent
sys.path.insert(0, str(DECOMP / 'scripts'))
from align import load_elf_text   # noqa: E402
from xref import SAME, refs       # noqa: E402

KR_GP, JP_GP = 0x5d40f0, 0x5d2a70


def sections(d):
    shoff, = struct.unpack_from('<I', d, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', d, 0x2e)
    secs = [struct.unpack_from('<10I', d, shoff + i * shentsize) for i in range(shnum)]
    names = secs[shstrndx]

    def nm(s):
        o = names[4] + s[0]
        return d[o:d.index(b'\0', o)].decode()
    return {nm(s): s for s in secs}


def elf_sections(elf_path):
    """[(name, start, end)] of the ELF's allocated data sections."""
    d = Path(elf_path).read_bytes()
    out = []
    for name, sec in sections(d).items():
        if name.startswith('.') and sec[2] & 2 and not sec[2] & 4 and sec[3]:   # SHF_ALLOC, not SHF_EXECINSTR
            out.append((name, sec[3], sec[3] + sec[5]))
    return out


def mdebug_statics(elf_path):
    """[(source file, function or None, var, address)] from the ELF's .mdebug stabs."""
    d = Path(elf_path).read_bytes()
    md = sections(d).get('.mdebug')
    if md is None:
        sys.exit(f'{elf_path} has no .mdebug (give the KR SLPM_645.25 first)')
    # HDRR: magic vstamp ilineMax cbLine cbLineOffset idnMax cbDnOffset ipdMax cbPdOffset isymMax
    # cbSymOffset ioptMax cbOptOffset iauxMax cbAuxOffset issMax cbSsOffset issExtMax cbSsExtOffset ifdMax
    # cbFdOffset crfd cbRfdOffset iextMax cbExtOffset (file offsets)
    h = struct.unpack_from('<hh23i', d, md[4])
    if h[0] != 0x7009:
        sys.exit('.mdebug: not an ECOFF symbolic header')
    cb_sym, cb_ss, ifd_max, cb_fd = h[10], h[16], h[19], h[20]
    out = []
    for f in range(ifd_max):
        _, rss, iss_base, _, isym_base, csym = struct.unpack_from('<6i', d, cb_fd + f * 72)

        def string(iss):
            o = cb_ss + iss_base + iss
            return d[o:d.index(b'\0', o)].decode('latin-1')
        fname = Path(string(rss).replace('\\', '/')).name if rss >= 0 else '?'
        # each symbol's name is a stab string: "Name:F.."/"Name:f.." opens a function, "var:V.." is a
        # static declared in it, "var:S.." a file static
        func = None
        for i in range(csym):
            iss, value, _ = struct.unpack_from('<iiI', d, cb_sym + (isym_base + i) * 12)
            m = re.match(r'([A-Za-z_]\w*):([FfVS])', string(iss))
            if not m:
                continue
            if m[2] in 'Ff':
                func = m[1]
            elif m[2] == 'V' and func:
                out.append((fname, func, m[1], value))
            elif m[2] == 'S':
                out.append((fname, None, m[1], value))
    return out


def kr_to_jp(kr_elf, jp_elf):
    """KR data address -> JP data address, from the instructions of identical function pairs."""
    ka, kw = load_elf_text(kr_elf)
    ja, jw = load_elf_text(jp_elf)
    votes = defaultdict(Counter)
    for r in csv.DictReader(open(DECOMP / 'data/align_jp.csv')):
        if r['status'] not in SAME or not r['addr']:
            continue
        k, j, n = int(r['kr_addr'], 16), int(r['addr'], 16), int(r['size']) // 4
        kr = {i: a for i, a, _ in refs(kw[(k - ka) // 4:(k - ka) // 4 + n], k, KR_GP)}
        jr = {i: a for i, a, _ in refs(jw[(j - ja) // 4:(j - ja) // 4 + n], j, JP_GP)}
        for i, a in kr.items():
            if i in jr:
                votes[a][jr[i]] += 1
    amap = {a: next(iter(c)) for a, c in votes.items() if len(c) == 1}
    keys = sorted(amap)

    def to_jp(a):
        if a in amap:
            return amap[a], 'xref'
        i = bisect.bisect_left(keys, a)
        lo = keys[i - 1] if i > 0 else None
        hi = keys[i] if i < len(keys) else None
        if lo is not None and hi is not None and amap[lo] - lo == amap[hi] - hi and hi - lo <= 0x1000:
            return a + amap[lo] - lo, 'between'
        return None, None
    return to_jp


def symbol_map(rel):
    """(address -> names, name -> address) for a port symbol map, unique names only."""
    by_addr, by_name = defaultdict(list), {}
    for line in (PLAT / f'common/core/symbols/{rel}.txt').read_text().splitlines():
        if line and not line.startswith('#'):
            name, addr, kind, binding = line.split()
            if addr != '-' and kind == 'D':
                by_addr[int(addr, 16)].append(name)
                by_name[name] = int(addr, 16)
    return by_addr, by_name


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    found = mdebug_statics(sys.argv[1])
    xref_jp = kr_to_jp(sys.argv[1], sys.argv[2])
    kr_by_addr, _ = symbol_map('kr')
    _, jp_by_name = symbol_map('jp')

    def to_jp(a):
        """Cross-reference votes first, then a name the KR and JP maps both give the address."""
        j, h = xref_jp(a)
        if j is None:
            for n in kr_by_addr.get(a, ()):
                if n in jp_by_name and not n.startswith('D_'):
                    return jp_by_name[n], 'name'
        return j, h
    names = Counter((Path(f).stem, f'{fn}.{v}' if fn else v) for f, fn, v, _ in found)
    rows = {'kr': [], 'jp': []}
    skipped, how = [], Counter()
    entries = []
    for fname, func, var, addr in found:
        tu = Path(fname).stem
        core = f'{func}.{var}' if func else var
        if names[(tu, core)] > 1:
            skipped.append(f'{tu} {core}: declared more than once in the TU')
            continue
        if not 0x100000 <= addr < 0x2000000:
            continue
        rows['kr'].append(f'{tu} {core} 0x{addr:08x}')
        j, h = to_jp(addr)
        entries.append([tu, core, addr, j, h])
    # A TU keeps its statics together in each section on both discs, so one the votes miss moves by the same
    # distance as its TU's nearest placed statics in the same section: both neighbours if they agree, else the
    # one that exists (select.c's disp_tag.flag ... fade_bg.frame move with its randlogcnt, -0x1690, as a JP
    # RAM dump confirms).
    sections = elf_sections(sys.argv[1])

    def section(a):
        return next((n for n, lo, hi in sections if lo <= a < hi), None)
    by_tu = defaultdict(list)
    for e in entries:
        if e[3] is not None:
            by_tu[(e[0], section(e[2]))].append((e[2], e[3] - e[2]))
    for placed in by_tu.values():
        placed.sort()
    for e in entries:
        placed = by_tu.get((e[0], section(e[2])))
        if e[3] is None and placed:
            i = bisect.bisect_left(placed, (e[2], -1 << 40))
            lo = placed[i - 1] if i > 0 else None
            hi = placed[i] if i < len(placed) else None
            if lo and hi and lo[1] != hi[1]:
                continue
            e[3], e[4] = e[2] + (lo or hi)[1], 'TU'
    for tu, core, addr, j, h in entries:
        how[h] += 1
        if j is None:
            skipped.append(f'{tu} {core} (KR 0x{addr:08x}): no JP address')
        else:
            rows['jp'].append(f'{tu} {core} 0x{j:08x}')
    for rel, title in (('kr', 'SLPM-645.25'), ('jp', 'SLPS-201.37')):
        p = PLAT / f'common/core/symbols/statics_{rel}.txt'
        p.write_text(f'# {title}: the game\'s static variables, as <TU> <Core name> <PS2 address>. From the KR disc\'s .mdebug\n'
                     '# (and, for JP, code cross-references) by platforms/tools/statics.py. Names and addresses only.\n'
                     + '\n'.join(sorted(set(rows[rel]))) + '\n', newline='\n')
        print(f'{p}: {len(set(rows[rel]))} statics')
    print(f'JP: {how["xref"]} by cross-reference, {how["between"]} between neighbours, {how["name"]} by name, '
          f'{how["TU"]} between statics of their own TU; '
          f'{len(skipped)} left out')
    for s in skipped:
        print('  left out:', s)


if __name__ == '__main__':
    main()
