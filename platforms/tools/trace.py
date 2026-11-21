#!/usr/bin/env python3
"""trace.py: read Lockstep Traces (common/trace/trace.h). Standard library only.

  trace.py info <trace>                      header, fields, calls per SDK function, size by part
  trace.py dump <trace> [--from N] [--to M]  one block per field: calls, transient masks, hashes, pages
  trace.py compare <a> <b> [--symbols kr]    the first field where two traces differ, and how
  trace.py inputs <trace>                    both pads as the game read them, one line per run of fields
  trace.py pages <trace> <field> [--symbols kr]  the trace's page hashes at a field, with symbols

--symbols names a release's symbol map (common/core/symbols/<kr|jp>.txt) to name the pages.
"""
import argparse
import re
import struct
import sys
from pathlib import Path

PLAT = Path(__file__).resolve().parent.parent
RAM_PAGES = 0x02000000 // 4096
PAGES = RAM_PAGES + 4
SPR_BASE = 0x70000000
RELEASES = {1: 'KR', 2: 'JP'}


def env_names():
    text = (PLAT / 'common/trace/trace_env.h').read_text()
    return {int(i): n for i, n in re.findall(r'X\((\d+), (\w+)\)', text)}


ENV = env_names()


def page_base(p):
    return p * 4096 if p < RAM_PAGES else SPR_BASE + (p - RAM_PAGES) * 4096


class Reader:
    def __init__(self, data, pos=0):
        self.d, self.p = data, pos

    def u8(self):
        v = self.d[self.p]
        self.p += 1
        return v

    def u32(self):
        v, = struct.unpack_from('<I', self.d, self.p)
        self.p += 4
        return v

    def u64(self):
        v, = struct.unpack_from('<Q', self.d, self.p)
        self.p += 8
        return v

    def varint(self):
        v = shift = 0
        while True:
            b = self.u8()
            v |= (b & 0x7f) << shift
            shift += 7
            if b < 0x80:
                return v

    def bytes(self, n):
        v = self.d[self.p:self.p + n]
        self.p += n
        return v


class Field:
    __slots__ = ('field', 'vblanks', 'calls', 'transient', 'state', 'ram', 'spr', 'pages', 'offset', 'size')


class Trace:
    def __init__(self, path):
        self.path = Path(path)
        d = self.path.read_bytes()
        if d[:4] == b'\x28\xb5\x2f\xfd':           # zstd-compressed whole (the corpus's .ggxt.zst)
            try:
                from compression import zstd   # Python 3.14
            except ImportError:
                sys.exit(f'{path}: zstd-compressed; this needs Python 3.14 (compression.zstd), or unzstd it first')
            d = zstd.decompress(d)
        if d[:8] != b'GGXTRACE':
            sys.exit(f'{path}: not a Lockstep Trace')
        r = Reader(d, 8)
        self.version, self.hash_version, self.release, self.header_size = r.u32(), r.u32(), r.u32(), r.u32()
        if self.version != 1:
            sys.exit(f'{path}: trace format {self.version}; this reader knows 1')
        self.elf_sha256 = r.bytes(32).hex()
        self.map_hash = r.u64()
        nmasks, self.flags = r.u32(), r.u32()
        self.recorder = r.bytes(64).rstrip(b'\0').decode()
        self.settings = r.bytes(64).rstrip(b'\0').decode()
        self.masks = [(r.u32(), r.u32()) for _ in range(nmasks)]
        self.outputs = {}
        for _ in range(r.u32()):
            eid, n = r.u8(), r.u8()
            specs = []
            for _ in range(n):
                arg = r.u8()
                specs.append((arg & 7, r.u8() if arg & 0x80 else 0))   # (argument, env whose last call it is)
            self.outputs[eid] = specs
        self.note = r.bytes(r.u32()).decode()
        self.data = d
        self.end_fields = None

    def fields(self):
        """Every field record in order. Each Field's pages is {page index: hash} for the pages that changed."""
        r = Reader(self.data, self.header_size)
        while r.p < len(r.d):
            start = r.p
            tag = r.u8()
            if tag == ord('E'):
                self.end_fields = r.varint()
                return
            if tag != ord('F'):
                sys.exit(f'{self.path}: bad record tag {tag:#x} at {start}')
            f = Field()
            f.offset = start
            f.field = r.varint()
            f.vblanks = r.u8()
            f.calls = []
            for _ in range(r.varint()):
                eid = r.u8()
                rep = r.varint() if eid & 0x80 else 1
                eid &= 0x7f
                ret = r.varint()
                outs = [r.bytes(r.u8()) for _ in range(r.u8())]
                f.calls.append((eid, ret, outs, rep))
            # transient masks: (address, size, stride, count)
            f.transient = [(r.varint(), r.varint(), r.varint(), r.varint()) for _ in range(r.varint())]
            f.state, f.ram, f.spr = r.u64(), r.u64(), r.u64()
            f.pages = {}
            p = -1
            for _ in range(r.varint()):
                p += r.varint() + 1
                f.pages[p] = r.u32()     # the page hash's low 32 bits
            f.size = r.p - start
            yield f


class Symbols:
    """Names for EE addresses, from a port symbol map: the nearest symbol at or below."""

    def __init__(self, rel):
        import bisect
        self.bisect = bisect
        rows = []
        for line in (PLAT / f'common/core/symbols/{rel}.txt').read_text().splitlines():
            if line and not line.startswith('#'):
                name, addr, kind, binding = line.split()
                if addr != '-' and not name.endswith(('_START', '_END', '_SIZE', '_VRAM')):
                    rows.append((int(addr, 16), name))
        rows.sort()
        self.addrs = [a for a, _ in rows]
        self.names = [n for _, n in rows]

    def name(self, addr):
        i = self.bisect.bisect_right(self.addrs, addr) - 1
        if i < 0:
            return f'{addr:08x}'
        return f'{self.names[i]}+0x{addr - self.addrs[i]:x}'

    def page(self, p):
        """The symbols that start in a page, or the one it is inside."""
        lo, hi = page_base(p), page_base(p) + 4096
        i = self.bisect.bisect_left(self.addrs, lo)
        inside = []
        while i < len(self.addrs) and self.addrs[i] < hi and len(inside) < 6:
            inside.append(self.names[i])
            i += 1
        if not inside or self.addrs[self.bisect.bisect_left(self.addrs, lo)] != lo:
            inside.insert(0, self.name(lo))
        return ', '.join(inside)


def call_text(c):
    eid, ret, outs, rep = c
    s = f'{ENV.get(eid, eid)}={ret if ret < 0x80000000 else ret - (1 << 32)}'
    if any(outs):
        s += ' [' + ' '.join(o.hex() for o in outs) + ']'
    return s + (f' x{rep}' if rep > 1 else '')


def cmd_info(a):
    t = Trace(a.trace)
    print(f'{t.path.name}: Lockstep Trace v{t.version}, State Hash v{t.hash_version}, '
          f'{RELEASES.get(t.release, t.release)}, boot ELF {t.elf_sha256[:16]}..., map {t.map_hash:016x}')
    print(f'  recorder: {t.recorder}\n  settings: {t.settings}\n  note: {t.note}\n  masks: ' +
          ', '.join(f'{b:08x}+{s:x}' for b, s in t.masks))
    n = calls = pages = vb = 0
    per = {}
    part = {'calls': 0, 'hashes': 0}
    for f in t.fields():
        n += 1
        vb += f.vblanks
        pages += len(f.pages)
        for c in f.calls:
            calls += c[3]
            per[c[0]] = per.get(c[0], 0) + c[3]
        part['hashes'] += 24 + len(f.pages) * 5
    print(f'  {n} field records ({t.end_fields} at the end record), {vb} PS2 vblanks, {calls} calls, '
          f'{pages} page hashes, {len(t.data)} bytes')
    print('  calls: ' + ', '.join(f'{ENV.get(k, k)} {v}' for k, v in sorted(per.items(), key=lambda kv: -kv[1])))


def cmd_dump(a):
    t = Trace(a.trace)
    for f in t.fields():
        if f.field < a.from_ or (a.to is not None and f.field > a.to):
            continue
        print(f'f{f.field:06d} vb={f.vblanks} state={f.state:016x} ram={f.ram:016x} spr={f.spr:016x} '
              f'pages={len(f.pages)}' + (' transient=' + ','.join(
                  f'{b:08x}+{s:x}' + (f'x{n}/{st:x}' if n > 1 else '') for b, s, st, n in f.transient)
                  if f.transient else ''))
        for c in f.calls:
            print('   ', call_text(c))


def cmd_compare(a):
    ta, tb = Trace(a.a), Trace(a.b)
    sym = Symbols(a.symbols) if a.symbols else None
    print(f'a: {ta.settings} ({ta.note})\nb: {tb.settings} ({tb.note})')
    pa, pb = {}, {}
    n = 0
    for fa, fb in zip(ta.fields(), tb.fields()):
        pa.update(fa.pages)
        pb.update(fb.pages)
        n += 1
        if fa.calls != fb.calls:
            print(f'f{fa.field}: the calls differ')
            for i, (ca, cb) in enumerate(zip(fa.calls, fb.calls)):
                if ca != cb:
                    print(f'  call {i}: a {call_text(ca)}  b {call_text(cb)}')
                    break
            else:
                print(f'  a has {len(fa.calls)} call records, b {len(fb.calls)}')
            return 1
        if fa.state != fb.state:
            regions = [n for n, x, y in (('ram', fa.ram, fb.ram), ('spr', fa.spr, fb.spr)) if x != y]
            diff = sorted(p for p in set(pa) | set(pb) if pa.get(p) != pb.get(p))
            print(f'f{fa.field}: State Hash differs ({", ".join(regions)}): {len(diff)} pages')
            for p in diff[:a.limit]:
                print(f'  page {p} {page_base(p):08x}' + (f'  {sym.page(p)}' if sym else ''))
            return 1
    print(f'identical for {n} fields')
    return 0


def cmd_inputs(a):
    t = Trace(a.trace)
    run, start, prev = 0, 0, None
    for f in t.fields():
        pads = [c[2][0][2:4].hex() if c[2] and c[2][0] else '-' for c in f.calls if c[0] == 2]
        cur = ' '.join(pads) if pads else '(no read)'
        if cur != prev and prev is not None:
            print(f'f{start:06d} x{run:<5d} {prev}')
            run, start = 0, f.field
        prev = cur
        run += 1
    if prev is not None:
        print(f'f{start:06d} x{run:<5d} {prev}')


def cmd_pages(a):
    t = Trace(a.trace)
    sym = Symbols(a.symbols) if a.symbols else None
    pages = {}
    for f in t.fields():
        pages.update(f.pages)
        if f.field == a.field:
            for p in sorted(f.pages):
                print(f'page {p} {page_base(p):08x} {f.pages[p]:08x}' + (f'  {sym.page(p)}' if sym else ''))
            return


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    s = sub.add_parser('info')
    s.add_argument('trace')
    s = sub.add_parser('dump')
    s.add_argument('trace')
    s.add_argument('--from', dest='from_', type=int, default=0)
    s.add_argument('--to', type=int)
    s = sub.add_parser('compare')
    s.add_argument('a')
    s.add_argument('b')
    s.add_argument('--symbols')
    s.add_argument('--limit', type=int, default=40)
    s = sub.add_parser('inputs')
    s.add_argument('trace')
    s = sub.add_parser('pages')
    s.add_argument('trace')
    s.add_argument('field', type=int)
    s.add_argument('--symbols')
    a = ap.parse_args()
    sys.exit({'info': cmd_info, 'dump': cmd_dump, 'compare': cmd_compare, 'inputs': cmd_inputs,
              'pages': cmd_pages}[a.cmd](a) or 0)


if __name__ == '__main__':
    main()
