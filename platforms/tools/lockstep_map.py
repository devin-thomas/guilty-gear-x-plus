#!/usr/bin/env python3
"""lockstep_map.py <kr|jp> <boot elf> <out.map>: the lockstep map for one release.

The PCSX2 recorder (the gg-layers fork's GGTrace, GG_TRACE_MAP=<map>) needs
to know where, in the game's own code, three things happen:

  field   the top of main()'s loop. Each time the EE reaches it a field has
          ended (or, the first time, the boot), and the recorder hashes RAM.
          It is the point where the port's host returns from GameField.
  env     every SDK call the port replaces with a shim (common/trace/
          trace_env.h): the function's entry, to see its arguments.
  ret     every address a call from code the Core compiles returns to: after
          each jal, and for a tail call, where the tail-calling function
          returns. There the recorder reads the return value and the bytes
          the PS2 wrote back, if the env entry it saw had that return
          address. Calls made inside the SDK itself are not seen, as in the
          port.
  mask    EE RAM the PS2 and the port can't agree on: the kernel, the PS2
          stack, the replaced libraries' own data, buffers the IOP fills
          (common/trace/masks.txt, with the reason for each).
  ptrmask the same for fields of a structure the game allocates: masked
          through the pointer variable that holds it, each field; optionally
          only while a variable holds a value (when address, value; 0 =
          always) and only in entries whose key field holds a value (key
          offset, bytes, value; 0 bytes = every entry).

Each hook carries a 16-bit check of the instruction at its address (the
low 16 bits of XXH3-64 of the address and the instruction word, both u32
little-endian), so the recorder ignores other code that runs there (the
BIOS, before the game is loaded). The map holds addresses and these checks,
no bytes of the game; it is
committed beside the symbol maps (common/trace/<kr|jp>.map) and the
recorder embeds its masks in every trace. common/trace/trace.h
describes the format.
"""
import hashlib
import re
import struct
import sys
from pathlib import Path

import xxhash

HERE = Path(__file__).resolve().parent
PLAT = HERE.parent
DECOMP = PLAT.parent
TITLES = {'kr': ('KR', 1, 'SLPM_645.25', 'kr/slpm_645.25.yaml'),
          'jp': ('JP', 2, 'SLPS_201.37', 'jp/slps_201.37.yaml')}
# the C library parts the Core compiles (CMakeLists.txt); their calls are the Core's
CORE_SDK = {f'sdk/newlib/{n}' for n in ('malloc', 'mallocr', 'mallocr_calloc', 'mallocr_free', 'sbrkr', 'rand',
                                         'mlock', 'errno')} | {f'sdk/gcc/{n}' for n in ('dp_bit', 'fp_bit',
                                                                                         '_floatdisf')}
JR_RA = 0x03e00008
REGS = {n: i for i, n in enumerate('zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 s0 s1 s2 s3 s4 s5 s6 s7 '
                                   't8 t9 k0 k1 gp sp fp ra'.split())}

# What the PS2 writes back that the game reads (LOCKSTEP.md, "env"): the bytes at a
# pointer argument after the call returns. "a2:8" is 8 bytes at a2; "t1:t2" is t2
# bytes at t1 (capped at 64; only sceSifCallRpc's 16-byte replies are used); "f.a2:4" is
# through another function f's argument, from its last call.
OUTPUTS = {
    'scePadRead': ['a2:8'],
    'sceSifCallRpc': ['t1:t2'],
    'sceCdSearchFile': ['a0:32'],
    # libmc writes sceMcGetInfo's type, free and format when the command completes, in sceMcSync:
    # "sceMcGetInfo.a2:4" is 4 bytes at a2 of the last sceMcGetInfo call.
    'sceMcSync': ['a1:4', 'a2:4', 'sceMcGetInfo.a2:4', 'sceMcGetInfo.a3:4', 'sceMcGetInfo.t0:4'],
}
# Transfers that land in EE RAM later than the call that starts them: the bytes
# are masked from the call until the given call returns the given value.
INFLIGHT = [
    # sceCdRead(lsn, sectors, buf, mode): buf, sectors * 2048, until sceCdSync returns 0 (idle)
    ('sceCdRead', 'a2', 'a1', 2048, 'sceCdSync', 0),
]


def check(addr, word):
    """The hook's check: the recorder computes the same from the instruction it finds there."""
    return xxhash.xxh3_64(struct.pack('<II', addr, word)).intdigest() & 0xffff


def env_list():
    text = (PLAT / 'common/trace/trace_env.h').read_text()
    return [(int(i), n) for i, n in re.findall(r'X\((\d+), (\w+)\)', text)]


def symbols(path):
    funcs, data = {}, {}
    for line in Path(path).read_text().splitlines():
        if not line or line.startswith('#'):
            continue
        name, addr, kind, binding = line.split()
        if binding == 'R':
            continue
        (funcs if kind == 'F' else data).setdefault(name, int(addr, 16))
    return funcs, data


def text_tus(yaml_path):
    """[(vram start, vram end, name)] for the text segment's subsegments."""
    rows = re.findall(r'^\s+- \[(0x[0-9a-f]+), (?:c|asm), ([\w/]+)\]', Path(yaml_path).read_text(), re.M)
    text_end = int(re.search(r'^\s+- \[(0x[0-9a-f]+), bin, text_pad\]', Path(yaml_path).read_text(), re.M)[1], 16)
    starts = [int(a, 16) for a, _ in rows] + [text_end]
    return [(starts[i] - 0x1000 + 0x100000, starts[i + 1] - 0x1000 + 0x100000, n) for i, (_, n) in enumerate(rows)]


class Elf:
    def __init__(self, path):
        self.data = d = Path(path).read_bytes()
        assert d[:4] == b'\x7fELF' and d[4] == 1 and d[5] == 1, 'ELF32 little-endian'
        phoff, = struct.unpack_from('<I', d, 0x1c)
        phentsize, phnum = struct.unpack_from('<HH', d, 0x2a)
        self.segs = []
        self.end = 0      # where the loaded image ends, .bss included: the heap starts here
        for i in range(phnum):
            ptype, off, vaddr, _, filesz, memsz, _, _ = struct.unpack_from('<8I', d, phoff + i * phentsize)
            if ptype == 1:
                self.segs.append((vaddr, off, filesz))
                self.end = max(self.end, vaddr + memsz)

    def word(self, addr):
        for vaddr, off, size in self.segs:
            if vaddr <= addr < vaddr + size:
                return struct.unpack_from('<I', self.data, off + addr - vaddr)[0]
        raise KeyError(f'{addr:#x} not in the ELF')


def loop_top(elf, start, end):
    """main()'s endless loop: the target of its last backward jump."""
    best = None
    for a in range(start, end, 4):
        w = elf.word(a)
        if w >> 26 == 2:                                      # j
            t = (a & 0xf0000000) | ((w & 0x3ffffff) << 2)
        elif w >> 16 == 0x1000:                               # beq zero, zero (b)
            t = a + 4 + (((w & 0xffff) ^ 0x8000) - 0x8000) * 4
        else:
            continue
        if start <= t < a:
            best = t
    if best is None:
        sys.exit('main has no backward jump')
    return best


def masks(rel, data_syms, funcs, elf_end):
    """([(base, size, reason)], [(pointer, offset, size, stride, count, reason)]) from masks.txt, with symbols
    resolved for this release."""
    out, ptr = [], []
    addrs = sorted(set(data_syms.values()) | set(funcs.values()))

    def at(name):
        """A data symbol's address, or symbol+0xN."""
        base, _, off = name.partition('+')
        if base == 'elf_end':
            return elf_end + (int(off, 0) if off else 0)
        if base not in data_syms:
            sys.exit(f'masks.txt: no data symbol {base} in the {rel} map')
        return data_syms[base] + (int(off, 0) if off else 0)
    for line in (PLAT / 'common/trace/masks.txt').read_text().splitlines():
        line = line.split('#', 1)[0].strip()
        if not line:
            continue
        kind, rest = line.split(None, 1)
        if kind in TITLES:                       # a line for one release only
            if kind != rel:
                continue
            kind, rest = rest.split(None, 1)
        if kind == 'range':
            r, base, size, reason = rest.split(None, 3)
            if r in (rel, 'all'):
                out.append((int(base, 16), int(size, 16), reason))
        elif kind == 'sym':
            name, size, reason = rest.split(None, 2)
            base = at(name)
            if size == '-':                      # to the next symbol
                size = next(a for a in addrs if a > base) - base
            else:
                size = int(size, 16)
            out.append((base, size, reason))
        elif kind == 'span':
            first, last, reason = rest.split(None, 2)
            out.append((at(first), at(last) - at(first), reason))
        elif kind == 'ptrfield':
            name, off, size, stride, count, rest = rest.split(None, 5)
            when, key = (0, 0), (0xffff, 0, 0)
            while rest.split(None, 1)[0] in ('when', 'key'):
                word, cond, rest = rest.split(None, 2)
                if word == 'when':
                    sym, value = cond.split('=')
                    when = (at(sym), int(value, 16))
                else:
                    where, value = cond.split('=')
                    koff, ksize = where.split(':')
                    key = (int(koff, 16), int(ksize), int(value, 16))
            ptr.append((at(name), int(off, 16), int(size, 16), int(stride, 16), int(count), when, key, rest))
        else:
            sys.exit(f'masks.txt: bad line {line!r}')
    return sorted(out), ptr


def main():
    if len(sys.argv) != 4 or sys.argv[1] not in TITLES:
        sys.exit(__doc__)
    rel = sys.argv[1]
    tag, rel_id, title, yaml = TITLES[rel]
    elf_path, out = Path(sys.argv[2]), Path(sys.argv[3])
    funcs, data_syms = symbols(PLAT / f'common/core/symbols/{rel}.txt')
    tus = text_tus(DECOMP / yaml)
    core = [(lo, hi, n) for lo, hi, n in tus if n.startswith('ggxeuc/') or n.startswith('sdk/libm/') or n in CORE_SDK]
    elf = Elf(elf_path)
    fstarts = sorted(set(funcs.values()))

    def fn_end(a):
        return next(x for x in fstarts if x > a)

    lines = ['ggx-lockstep-map 1', f'release {tag} {rel_id} {title}',
             f'elf_sha256 {hashlib.sha256(elf.data).hexdigest()}']
    m = funcs['main']
    top = loop_top(elf, m, fn_end(m))
    lines.append(f'field {top:08x} {check(top, elf.word(top)):04x}')

    def enclosing(a):
        return max(x for x in fstarts if x <= a)

    def sites_of(target, op):
        word = (op << 26) | ((target >> 2) & 0x3ffffff)
        for lo, hi, tu in core:
            for a in range(lo, hi, 4):
                if elf.word(a) == word:
                    yield a, tu

    def return_addresses(target, depth=0):
        """Where a call to target returns to in the Core's code: after each jal, and,
        for a tail call (j target), wherever the tail-calling function returns."""
        out = {a + 8 for a, _ in sites_of(target, 3)}
        for a, tu in sites_of(target, 2):
            if depth > 4:
                sys.exit(f'tail calls nest too deep at {a:08x}')
            out |= return_addresses(enclosing(a), depth + 1)
            notes.append(f'{target:08x}: tail call at {a:08x} ({tu}) returns through its caller')
        return out

    ids = {n: i for i, n in env_list()}
    by_name = {}
    watched = set()
    notes = []
    for eid, name in env_list():
        if name not in funcs:
            notes.append(f'{name}: not in the symbol map, not recorded')
            continue
        entry = funcs[name]
        by_name[name] = eid
        rets = return_addresses(entry)
        outs = []
        for o in OUTPUTS.get(name, []):
            src, dot, rest = o.rpartition('.')
            outs.append(f'{ids[src]}.{rest}' if dot else o)
        lines.append(f'env {eid} {name} {entry:08x} {check(entry, elf.word(entry)):04x} ' + (' '.join(outs) or '-')
                     + ('' if rets else '  # never called by the game'))
        watched |= rets
    for a in sorted(watched):
        lines.append(f'ret {a:08x} {check(a, elf.word(a)):04x}')
    for start, ptr, length, mul, until, value in INFLIGHT:
        if start in by_name and until in by_name:
            lines.append(f'inflight {by_name[start]} {ptr} {length} {mul} {by_name[until]} {value}')
    fixed, ptr = masks(rel, data_syms, funcs, elf.end)
    for base, size, reason in fixed:
        lines.append(f'mask {base:08x} {size:08x} {reason}')
    for addr, off, size, stride, count, when, key, reason in ptr:
        lines.append(f'ptrmask {addr:08x} {off:x} {size:x} {stride:x} {count} {when[0]:08x} {when[1]:x} '
                     f'{key[0]:x} {key[1]} {key[2]:x} {reason}')
    out.write_text('\n'.join(lines) + '\n', newline='\n')
    for w in notes:
        print(f'lockstep_map: {w}', file=sys.stderr)
    print(f'{out}: field at {top:08x}, {sum(l.startswith("ret ") for l in lines)} call sites, '
          f'{sum(l.startswith("mask ") for l in lines)} masks')


if __name__ == '__main__':
    main()
