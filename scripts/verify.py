#!/usr/bin/env python3
"""Byte-check C functions against GGX Plus JP (SLPS-201.37) or Korean (SLPM-645.25).

For each C file: preprocess and compile with EE-GCC 2.9-ee-991111's cc1
(-O2 -G8 -ffunction-sections) and assemble with the matching GAS: the 991111
package's for JP, the newer one shipped in the 991111-01 package for Korean
(it does not insert FPU hazard nops; scripts/patch_kr_as.py fixes its R5900
short-loop count). Link every function section at its
address, with all other symbols from jp/symbol_addrs.txt plus the split's
undefined_syms_auto.txt (JP) or the ELF's own .symtab (Korean). The object's
own data sections (.rodata jump tables and literals, .lit4, .sdata/.sbss,
.data/.bss), its common variables and any extern without an address are
linked where the original code points, solved from the references at the
same instructions. Each data label is its own section (datafrag.py), so a
function's literals, switch tables and statics are placed one by one, as
they sit in the original's whole-TU sections; a reference is attributed by
its relocation, and data that only placed data points at (a table of
strings) is solved through those pointers. A fragment nothing refers to is
reported as unused.

A function matches when its compiled bytes equal the original (every
relocation resolving to the original address) and the original's remaining
bytes up to the next function are zero padding. data_ok separately reports
whether the object's own initialized data equals the ELF where it was placed.

The preprocessor defines VERSION_JP or VERSION_KR, for the few functions
whose code differs between the two builds.

The compiler runs in a container (Linux i386 binaries via qemu); --runner
gives the command prefix and --map the host=guest path pairs.
"""
import argparse
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

from elftools.elf.elffile import ELFFile

sys.path.insert(0, os.path.dirname(__file__))
from xref import refs  # noqa: E402
import datafrag  # noqa: E402

# split each compiled file's data into per-label sections (GGX_NOFRAG=1: off)
FRAGMENTS = not os.environ.get('GGX_NOFRAG')

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
# the toolchain directory (scripts/fetch_toolchain.py --dest), as the runner sees it
TOOLS = os.environ.get('GGX_TOOLS', '/tools')
CC_DIR = f'{TOOLS}/ee-gcc2.9-991111'
CC_VER = '2.9-ee-991111'
# KR game assembler: the 991111-01 GAS with its R5900 short-loop count fixed, made by
# scripts/patch_kr_as.py (the stock one pads some long loops with nops the KR binary lacks)
KR_AS_DIR = os.environ.get('GGX_KR_AS_DIR', f'{TOOLS}/ee-gcc2.9-991111-01-fix')
# how the i386 compiler runs: GGX_QEMU overrides it ('' runs it directly on an x86 host with i386 libraries)
QEMU = os.environ.get('GGX_QEMU', f'{TOOLS}/linux-runtime/usr/bin/qemu-i386-static {TOOLS}/linux-runtime/lib32/ld-linux.so.2 '
                                  f'--library-path {TOOLS}/linux-runtime/lib32')
CPP_DEFS = '-D__GNUC__=2 -D__GNUC_MINOR__=9 -D_LANGUAGE_C -DLANGUAGE_C -D__mips__ -DSPLAT'
GP = 0x5d2a70
# probe addresses for the object's own data; the real ones are solved from
# the original code's references (gp-relative probes must stay near $gp)
PROBES = {}
# where each kind's fragment probes start (datafrag.py), and where they must stop
FRAG_PROBES = {}


def set_gp(gp):
    global GP
    GP = gp
    PROBES.update({'.rodata': 0x0f000000, '.data': 0x0e800000, '.bss': 0x0e400000,
                   '.sdata': GP + 0x5000, '.lit4': GP + 0x5800, '.lit8': GP + 0x6000,
                   '.sbss': GP + 0x6800})
    # below $gp, clear of near_gp_slots (GP-0x4c00 and up)
    FRAG_PROBES.update({'.rodata': (0x0f100000, 0x0f800000), '.data': (0x0e900000, 0x0f000000),
                        '.bss': (0x0e500000, 0x0e800000),
                        '.sdata': (GP - 0x7ff0, GP - 0x6400), '.sbss': (GP - 0x6400, GP - 0x4c00),
                        '.lit4': (GP + 0x7000, GP + 0x7ff0)})


set_gp(GP)
INPUTS = {'.rodata': '*(.rodata)', '.data': '*(.data)', '.bss': '*(.bss) *(COMMON)',
          '.sdata': '*(.sdata)', '.lit4': '*(.lit4)', '.lit8': '*(.lit8)',
          '.sbss': '*(.sbss) *(.scommon)'}


class Env:
    def __init__(self, args):
        self.runner = args.runner.split()
        self.maps = [m.split('=', 1) for m in args.map]
        self.bu = Path(args.binutils)
        self.elf = Path(args.elf)
        self.target = getattr(args, 'target', 'jp')
        self.syms = {}
        self.funcs = {}       # Korean: every address of each function name (file statics repeat)
        if getattr(args, 'target', 'jp') == 'kr':
            # the Korean ELF keeps its symbol table
            with open(args.elf, 'rb') as f:
                # A name can be both a local object and a global function (e.g. a static `sin`
                # table and the libm `sin`): a global wins, then the first symbol seen.
                rank = {}
                for sym in ELFFile(f).get_section_by_name('.symtab').iter_symbols():
                    if sym.name and sym['st_shndx'] != 'SHN_UNDEF' and sym['st_value'] \
                            and sym['st_info']['type'] in ('STT_FUNC', 'STT_OBJECT', 'STT_NOTYPE'):
                        r = 0 if sym['st_info']['bind'] == 'STB_GLOBAL' else 1
                        if sym['st_info']['type'] == 'STT_FUNC':
                            self.funcs.setdefault(sym.name, set()).add(sym['st_value'])
                        if sym.name not in self.syms or r < rank[sym.name]:
                            self.syms[sym.name], rank[sym.name] = sym['st_value'], r
            # the split's names for file statics the .symtab repeats (kr/symbol_addrs.txt)
            for l in open(ROOT / 'kr/symbol_addrs.txt'):
                m = re.match(r'\s*([A-Za-z_.$][\w.$]*)\s*=\s*(0x[0-9a-fA-F]+)\s*;', l)
                if m and 'type:func' in l:
                    self.syms.setdefault(m.group(1), int(m.group(2), 16))
        else:
            for f in [ROOT / 'jp/symbol_addrs.txt', *([Path(args.undefined)] if args.undefined else [])]:
                for l in open(f):
                    m = re.match(r'\s*([A-Za-z_.$][\w.$]*)\s*=\s*(0x[0-9a-fA-F]+)\s*;', l)
                    if m:
                        self.syms.setdefault(m.group(1), int(m.group(2), 16))
        self.starts = sorted(set(self.syms.values()) | {a for v in self.funcs.values() for a in v})
        # names with more than one function address (file statics such as the SDK's setup/Copy)
        self.dups = {n: sorted(v) for n, v in self.funcs.items() if len(v) > 1}
        with open(self.elf, 'rb') as f:
            e = ELFFile(f)
            self.segs = [(s['p_vaddr'], s['p_offset'], s['p_filesz']) for s in e.iter_segments()]
        self.data = self.elf.read_bytes()

    def guest(self, p):
        p = str(p)
        for h, g in self.maps:
            if p.startswith(h):
                return g + p[len(h):]
        raise SystemExit(f'{p} is not visible in the container')

    def read(self, addr, n):
        for va, off, sz in self.segs:
            if va <= addr and addr + n <= va + sz:
                return self.data[off + addr - va: off + addr - va + n]
        return None

    def sh(self, script):
        r = subprocess.run(self.runner + ['sh', '-c', script], capture_output=True, text=True)
        return r.returncode, r.stdout + r.stderr


def compile_c(env, src, work, flags, cc_dir=CC_DIR, cc_ver=CC_VER, as_dir=None, frag=True):
    work.mkdir(parents=True, exist_ok=True)
    inc = work / 'include'
    shutil.rmtree(inc, ignore_errors=True)
    shutil.copytree(ROOT / 'include', inc)
    shutil.copy(src, work / 'in.c')
    for h in Path(src).parent.glob('*.h'):       # headers next to the source
        shutil.copy(h, work / h.name)
    g = env.guest(work)
    cc = f'{QEMU} {cc_dir}/lib/gcc-lib/ee/{cc_ver}'
    rc, out = env.sh(f'set -e; cd {g}; {cc}/cpp -lang-c -Iinclude {CPP_DEFS} -DVERSION_{env.target.upper()} in.c -o in.i; '
                     f'{cc}/cc1 -quiet -ffunction-sections {flags} in.i -o raw.s')
    if rc:
        return None, out
    as_dir = as_dir or cc_dir
    gas = f'{QEMU} {as_dir}/ee/bin/as -EL -mabi=eabi'
    for f in ('out.o', 'ref.o', 'code.o', 'data.o', 'data.s', 'frag.o', 'merged.o'):
        (work / f).unlink(missing_ok=True)
    # each data label in its own section (datafrag.py); the plain object is
    # the reference, and the fallback
    try:
        tag = datafrag.TAG + '_' + re.sub(r'\W', '_', Path(src).stem) + '_'
        code, data, nfrag = (datafrag.split((work / 'raw.s').read_text(), tag) if FRAGMENTS and frag
                             else (None, '', 0))
    except ValueError as e:
        code, data, nfrag = None, '', 0
        out += f'\ndatafrag: {e}\n'
    shutil.copy(work / 'raw.s', work / 'out.s')
    if not nfrag:
        rc, out2 = env.sh(f'set -e; cd {g}; {gas} out.s -o out.o')
        return (None if rc else work / 'out.o'), out + out2
    (work / 'code.s').write_text(code)
    if data:
        (work / 'data.s').write_text(data)
    rc, out2 = env.sh(f'set -e; cd {g}; {gas} out.s -o ref.o; {gas} code.s -o code.o' +
                      (f'; {gas} data.s -o data.o' if data else ''))
    if not (work / 'ref.o').exists():
        return None, out + out2
    why = None
    if rc:
        why = 'fragmented assembly does not assemble'
    elif data:
        (work / 'merge.ld').write_text('SECTIONS { }\n')
        r = subprocess.run([str(env.bu / 'mips-ps2-decompals-ld'), '-EL', '-r', '-T', str(work / 'merge.ld'),
                            '-o', str(work / 'frag.o'), str(work / 'code.o'), str(work / 'data.o')],
                           capture_output=True, text=True)
        if r.returncode:
            why = 'merge failed: ' + r.stderr[-500:]
    else:
        shutil.copy(work / 'code.o', work / 'frag.o')
    if why is None and datafrag.code_shape(work / 'frag.o') != datafrag.code_shape(work / 'ref.o'):
        why = 'fragmented code differs from the plain assembly'
    if why:
        out2 += f'\ndatafrag: {why}; using the plain object\n'
        shutil.copy(work / 'ref.o', work / 'out.o')
    else:
        shutil.copy(work / 'frag.o', work / 'out.o')
    return work / 'out.o', out + out2


def obj_info(obj):
    """Function sections, data sections, common and undefined symbols of an object."""
    with open(obj, 'rb') as f:
        e = ELFFile(f)
        funcs = [s.name[6:] for s in e.iter_sections() if s.name.startswith('.text.')]
        secs = {s.name: s['sh_size'] for s in e.iter_sections()
                if s['sh_size'] and datafrag.kind_of(s.name)}
        commons, undefined = {}, set()
        for sym in e.get_section_by_name('.symtab').iter_symbols():
            if not sym.name:
                continue
            sh = sym['st_shndx']
            if sh == 'SHN_UNDEF':
                undefined.add(sym.name)
            elif sh == 'SHN_COMMON' or sh == 0xff03:     # 0xff03: SHN_MIPS_SCOMMON
                commons[sym.name] = (sym['st_size'], sh == 0xff03)
    return funcs, secs, commons, undefined


def reloc_targets(obj):
    """{section: set of data sections its relocations point into}."""
    out = {}
    with open(obj, 'rb') as f:
        e = ELFFile(f)
        secs = list(e.iter_sections())
        symtab = e.get_section_by_name('.symtab')
        for s in secs:
            if s['sh_type'] != 'SHT_REL':
                continue
            src = secs[s['sh_info']].name
            for r in s.iter_relocations():
                sym = symtab.get_symbol(r['r_info_sym'])
                sh = sym['st_shndx']
                if isinstance(sh, int) and 0 < sh < len(secs) and datafrag.kind_of(secs[sh].name):
                    out.setdefault(src, set()).add(secs[sh].name)
    return out


def reloc_map(obj):
    """{(section, offset): ('sym', name) for an undefined or common symbol,
    ('sec', section) for a symbol defined in a data section, or None}."""
    out = {}
    with open(obj, 'rb') as f:
        e = ELFFile(f)
        secs = list(e.iter_sections())
        symtab = e.get_section_by_name('.symtab')
        for s in secs:
            if s['sh_type'] != 'SHT_REL':
                continue
            src = secs[s['sh_info']].name
            for r in s.iter_relocations():
                sym = symtab.get_symbol(r['r_info_sym'])
                sh = sym['st_shndx']
                if sh in ('SHN_UNDEF', 'SHN_COMMON', 0xff03):
                    out[src, r['r_offset']] = ('sym', sym.name)
                elif isinstance(sh, int) and 0 < sh < len(secs) and datafrag.kind_of(secs[sh].name):
                    out[src, r['r_offset']] = ('sec', secs[sh].name)
                else:
                    out[src, r['r_offset']] = None
    return out


def near_gp_slots():
    # clear of the section probes above (GP+0x5000..GP+0x7000)
    for k in range(1, 20):
        yield GP + 0x400 * k
        yield GP - 0x400 * k


def link(env, obj, work, func_secs, sec_addr, sym_addr, frags=()):
    """Function sections at JP addresses; data sections and symbols where given."""
    lines = [f'_gp = 0x{GP:08x};']
    lines += [f'{n} = 0x{a:08x};' for n, a in sorted(sym_addr.items())]
    lines.append('SECTIONS {')
    for f, addr in func_secs.items():
        lines.append(f'  .text.{f} 0x{addr:08x} : {{ *(.text.{f}) }}')
    lines.append('  .text 0x0e000000 : { *(.text) }')
    for s, inp in INPUTS.items():
        lines.append(f'  {s} 0x{sec_addr.get(s, PROBES[s]):08x} : {{ {inp} }}')
    for s in frags:
        lines.append(f'  {s} 0x{sec_addr[s]:08x} : {{ *({s}) }}')
    lines.append('  /DISCARD/ : { *(.reginfo) *(.mdebug) *(.comment) *(.pdr) *(.gnu.attributes) }')
    lines.append('}')
    (work / 'link.ld').write_text('\n'.join(lines) + '\n')
    r = subprocess.run([str(env.bu / 'mips-ps2-decompals-ld'), '-EL', '--no-check-sections',
                        '-T', str(work / 'link.ld'), '-o', str(work / 'out.elf'), str(obj)],
                       capture_output=True, text=True)
    missing = sorted({m for m in re.findall(r"undefined reference to `([^']+)'", r.stderr)})
    return (work / 'out.elf' if r.returncode == 0 else None), missing, r.stderr


def func_bytes(elf_path):
    out, data = {}, {}
    with open(elf_path, 'rb') as f:
        e = ELFFile(f)
        for s in e.iter_sections():
            if s.name.startswith('.text.'):
                out[s.name[6:]] = (s['sh_addr'], s.data())
            elif datafrag.kind_of(s.name) and s['sh_size']:
                nobits = s['sh_type'] == 'SHT_NOBITS'
                data[s.name] = (s['sh_addr'], s['sh_size'], None if nobits else s.data())
    return out, data


def words(b):
    return list(struct.unpack('<%dI' % (len(b) // 4), b))


def check(env, src, work, flags='', cc_dir=CC_DIR, cc_ver=CC_VER, as_dir=None, frag=True, prefer=None):
    """Return a list of per-function result dicts for one C file. prefer=(lo, hi):
    a function name with several addresses (file statics) resolves to the one in
    [lo, hi), the C file's own TU."""
    obj, log = compile_c(env, src, work, flags, cc_dir, cc_ver, as_dir, frag=frag)
    if not obj:
        return [{'file': str(src), 'status': 'compile-error', 'log': log[-2000:]}]
    syms = env.syms
    if prefer and env.dups:
        syms = dict(syms)
        for n, addrs in env.dups.items():
            mine = [a for a in addrs if prefer[0] <= a < prefer[1]]
            if mine:
                syms[n] = mine[0]
    names, secs, commons, undefined = obj_info(obj)
    func_secs = {n: syms[n] for n in names if n in syms}

    # Probe items: the object's data sections (whole, or one per datafrag
    # fragment), its common symbols and any undefined symbol without a JP
    # address. Each gets its own probe range; the original code's reference at
    # the same instruction gives its address.
    slots = near_gp_slots()
    items = {}                                   # key -> (probe, span)
    sym_addr = {n: syms[n] for n in undefined if n in syms}
    for n, (size, small) in commons.items():
        if n in syms:
            sym_addr[n] = syms[n]
        else:
            items[('sym', n)] = (next(slots) if small or size <= 8 else 0x0c000000 + 0x100000 * len(items),
                                 max(size, 4))
    for n in sorted(undefined - set(syms) - {'_gp'} - set(names)):
        items[('sym', n)] = (next(slots), 0x400)
    frags, cursor = [], {k: lo for k, (lo, hi) in FRAG_PROBES.items()}
    for sname in secs:
        if sname in PROBES:
            items[('sec', sname)] = (PROBES[sname], max(secs[sname], 4))
            continue
        kind = datafrag.kind_of(sname)
        a = cursor[kind]
        if a + secs[sname] > FRAG_PROBES[kind][1]:
            if not frag:
                break
            # more fragments than probe room near $gp: check the plain object
            return check(env, src, work, flags, cc_dir, cc_ver, as_dir, frag=False)
        cursor[kind] = (a + secs[sname] + 4 + 0xf) & ~0xf  # a gap, so an end address is unambiguous
        items[('sec', sname)] = (a, secs[sname])
        frags.append(sname)

    def assign(solved):
        sa = dict(sym_addr)
        sec = {}
        for (kind, n), (probe, _) in items.items():
            a = solved.get((kind, n), probe)
            if kind == 'sym':
                sa[n] = a
            else:
                sec[n] = a
        return sec, sa

    sec, sa = assign({})
    elf, missing, err = link(env, obj, work, func_secs, sec, sa, frags)
    if not elf:
        return [{'file': str(src), 'status': 'link-error', 'missing': missing, 'log': err[-2000:]}]
    got, data = func_bytes(elf)
    from collections import Counter

    def owner(a, linked):
        """The probe item whose range holds a (a fragment's end counts as its own)."""
        for k, (probe, span) in items.items():
            if k in linked:
                continue
            base = data[k[1]][0] if k[0] == 'sec' and k[1] in data else probe
            if base <= a < base + span or (k[0] == 'sec' and k[1] in frags and a == base + span):
                return k, base
        return None, None

    # a reference's relocation names its item; the address only gives the
    # offset (a probe can share an address with a real variable the code uses)
    rmap = reloc_map(obj)

    def item(sec, off, a, linked):
        if (sec, off) not in rmap:
            return owner(a, linked)            # no relocation: go by the address
        k = rmap[sec, off]
        if k not in items or k in linked:
            return None, None
        return k, (data[k[1]][0] if k[0] == 'sec' and k[1] in data else items[k][0])

    votes = {k: Counter() for k in items}
    for n, (addr, b) in got.items():
        orig = env.read(addr, len(b))
        if not orig or n not in func_secs:
            continue
        o = {i: a for i, a, _ in refs(words(orig), addr, GP)}
        for i, a, _ in refs(words(b), addr, GP):
            if i not in o:
                continue
            k, base = item(f'.text.{n}', 4 * i, a, ())
            if k is not None:
                votes[k][o[i] - (a - base)] += 1
    solved = {k: c.most_common(1)[0][0] for k, c in votes.items() if c}
    for _ in range(8):
        if solved:
            sec, sa = assign(solved)
            elf, missing, err = link(env, obj, work, func_secs, sec, sa, frags)
            got, data = func_bytes(elf)
        # pointers in placed data reach items no code refers to directly (a
        # table of strings): the original word at the same offset gives them
        more = {}
        for sname, (pa, size, content) in data.items():
            if content is None or ('sec', sname) not in solved:
                continue
            orig = env.read(pa, size)
            if orig is None:
                continue
            for off in range(0, size - 3, 4):
                if (sname, off) not in rmap:
                    continue                   # not a pointer
                w, ow = struct.unpack_from('<I', content, off)[0], struct.unpack_from('<I', orig, off)[0]
                k, base = item(sname, off, w, solved)
                if k is not None:
                    more.setdefault(k, Counter())[ow - (w - base)] += 1
        if not more:
            break
        solved.update({k: c.most_common(1)[0][0] for k, c in more.items()})
    solved_syms = {n: f'0x{a:08x}' for (kind, n), a in solved.items() if kind == 'sym'}
    unsolved = sorted(n for (kind, n) in items if kind == 'sym' and (kind, n) not in solved)
    # a fragment that neither the code nor placed data points at is another
    # function's copy (or dead): nothing here depends on its address
    targets = reloc_targets(obj)
    wanted = set()
    for src_sec, tg in targets.items():
        if src_sec.startswith('.text') or (datafrag.kind_of(src_sec) and ('sec', src_sec) in solved):
            wanted |= tg

    results = []
    for n in names:
        if n not in func_secs:
            results.append({'name': n, 'status': 'no-jp-address'})
            continue
        addr, b = got[n]
        nxt = next((x for x in env.starts if x > addr), addr + len(b))
        orig = env.read(addr, nxt - addr)
        cw, ow = words(b), words(orig[:len(b)]) if orig and len(orig) >= len(b) else []
        diff = sum(1 for x, y in zip(cw, ow) if x != y) + abs(len(cw) - len(ow))
        pad_ok = orig is not None and all(x == 0 for x in orig[len(b):])
        r = {'name': n, 'addr': f'0x{addr:06x}', 'size': len(b), 'orig_size': nxt - addr,
             'diff_words': diff, 'padding_ok': pad_ok}
        for sname, (pa, size, content) in data.items():
            placed = ('sec', sname) in solved
            if placed:
                r.setdefault('data', {})[sname] = f'0x{pa:06x}+{size}'
            elif sname in frags and sname not in wanted:
                r.setdefault('data', {})[sname] = f'unused+{size}'
            else:
                r.setdefault('data', {})[sname] = f'unplaced+{size}'
            if content is not None and placed:
                r['data_ok'] = r.get('data_ok', True) and env.read(pa, size) == content
        if solved_syms:
            r['solved'] = solved_syms
        if unsolved:
            r['unsolved'] = unsolved
        ok = diff == 0 and pad_ok          # data_ok is reported, not required
        r['status'] = 'match' if ok else 'mismatch'
        results.append(r)
    return results


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('files', nargs='+', type=Path)
    ap.add_argument('--elf', required=True)
    ap.add_argument('--target', choices=('jp', 'kr'), default='jp',
                    help='jp: SLPS-201.37 with jp/symbol_addrs.txt; kr: SLPM-645.25 with its .symtab')
    ap.add_argument('--binutils', required=True)
    ap.add_argument('--undefined', help="split's undefined_syms_auto.txt (D_ labels)")
    ap.add_argument('--work', required=True, type=Path)
    ap.add_argument('--runner', default='env')
    ap.add_argument('--map', action='append', default=[],
                    help='host=guest path pair visible in the container (repeatable)')
    ap.add_argument('--flags', default='-O2 -G8')
    ap.add_argument('--cc-dir', default=CC_DIR, help='compiler package path in the container')
    ap.add_argument('--cc-ver', default=CC_VER, help='lib/gcc-lib/ee/<version>')
    ap.add_argument('--as-dir', help='package whose ee/bin/as assembles (default: per target)')
    ap.add_argument('--prefer-range', help='lo:hi (hex): resolve repeated function names (file statics) '
                    'to the address in this range, the TU being checked')
    ap.add_argument('--json', type=Path, help='append results as JSON lines')
    ap.add_argument('--quiet', action='store_true')
    args = ap.parse_args()
    if args.target == 'kr':
        set_gp(0x5d40f0)
    if not args.as_dir:
        # JP: the 991111 package's GAS (inserts FPU hazard nops); KR: the newer
        # GAS shipped in the 991111-01 package (does not)
        args.as_dir = KR_AS_DIR if args.target == 'kr' else args.cc_dir
    env = Env(args)
    allr = []
    for src in args.files:
        t = time.time()
        prefer = tuple(int(x, 16) for x in args.prefer_range.split(':')) if args.prefer_range else None
        res = check(env, src.resolve(), args.work.resolve() / src.stem, args.flags,
                    args.cc_dir, args.cc_ver, args.as_dir, prefer=prefer)
        for r in res:
            r.setdefault('file', str(src))
            r['seconds'] = round(time.time() - t, 1)
            allr.append(r)
            if not args.quiet or r['status'] != 'match':
                extra = {k: v for k, v in r.items() if k not in ('file', 'log')}
                print(json.dumps(extra))
                if 'log' in r and not args.quiet:
                    print(r['log'])
    if args.json:
        with open(args.json, 'a') as fh:
            for r in allr:
                r.pop('log', None)
                fh.write(json.dumps(r) + '\n')
    m = sum(1 for r in allr if r['status'] == 'match')
    print(f'{m}/{len(allr)} match, {sum(r.get("size", 0) for r in allr if r["status"] == "match")} bytes')


if __name__ == '__main__':
    main()
