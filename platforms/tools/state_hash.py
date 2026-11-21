#!/usr/bin/env python3
"""state_hash.py: the State Hash (common/host/core_state.h), outside the port.

A second implementation of State Hash v1, written from the definition
rather than from core_state.c, for checking the two against each other and
for hashing PS2 memory the port never saw (PORT-006 hashes PCSX2's EE RAM
and scratchpad with it).

    state_hash.py snapshot <file>
        Checks a snapshot file (ggx-headless --save): its header, its page
        hash table and its recorded State Hash, all recomputed from its
        pages. Prints the State Hash and each region's hash.

    state_hash.py regions <id>:<base>:<dump> [...]
        The State Hash of raw region dumps, for example PCSX2's EE RAM and
        scratchpad: `regions 1:0:eeMem.bin 2:0x70000000:scratch.bin`.

Needs the xxhash package (pip install xxhash; XXH3 64-bit, seed 0).
"""
import struct
import sys

import xxhash

HASH_VERSION = 1
FORMAT_VERSION = 1
PAGE = 4096
NAMES = {1: 'ram', 2: 'spr', 3: 'eeio', 4: 'vumem', 5: 'gspriv', 6: 'own', 7: 'host'}


def page_hashes(data):
    return [xxhash.xxh3_64_intdigest(data[i:i + PAGE]) for i in range(0, len(data), PAGE)]


def region_hash(rid, base, size, hashes):
    assert len(hashes) == (size + PAGE - 1) // PAGE
    return xxhash.xxh3_64_intdigest(struct.pack('<III', rid, base, size) + struct.pack(f'<{len(hashes)}Q', *hashes))


def state_hash(regions):
    """regions: (id, region hash) pairs, any order."""
    regions = sorted(regions)
    b = b'GGXH' + struct.pack('<II', HASH_VERSION, len(regions))
    for rid, h in regions:
        b += struct.pack('<IQ', rid, h)
    return xxhash.xxh3_64_intdigest(b)


def snapshot(path):
    d = open(path, 'rb').read()
    if d[:8] != b'GGXSTATE':
        raise SystemExit(f'{path}: not a snapshot')
    fmt, hver, release, count, build = struct.unpack_from('<IIIIQ', d, 8)
    recorded, pages, table_at = struct.unpack_from('<QII', d, 64)
    if fmt != FORMAT_VERSION or hver != HASH_VERSION:
        raise SystemExit(f'{path}: format {fmt}, State Hash {hver}; this script knows {FORMAT_VERSION}, {HASH_VERSION}')
    table = struct.unpack_from(f'<{pages}Q', d, table_at)
    print(f'{path}: release {"KR" if release == 1 else "JP"}, boot ELF SHA-256 {d[32:64].hex()}, '
          f'Core build {build:016x}')
    bad, first, out = 0, 0, []
    for r in range(count):
        rid, base, size, off = struct.unpack_from('<IIII', d, 80 + 16 * r)
        hs = page_hashes(d[off:off + size])
        stored = table[first:first + len(hs)]
        wrong = sum(a != b for a, b in zip(hs, stored))
        bad += wrong
        h = region_hash(rid, base, size, hs)
        out.append((rid, h))
        print(f'  {rid} {NAMES.get(rid, "?"):6} 0x{base:08x}+0x{size:x}: {h:016x}'
              + (f'  ({wrong} page hashes in the table are wrong)' if wrong else ''))
        first += len(hs)
    got = state_hash(out)
    print(f'State Hash v{HASH_VERSION}: {got:016x} (recorded {recorded:016x}: '
          f'{"matches" if got == recorded else "DIFFERS"})')
    ee = state_hash([x for x in out if x[0] in (1, 2)])
    print(f'State Hash v{HASH_VERSION} of ram+spr: {ee:016x}')
    return 0 if got == recorded and not bad else 1


def regions(specs):
    out = []
    for spec in specs:
        rid, base, path = spec.split(':', 2)
        data = open(path, 'rb').read()
        rid, base = int(rid, 0), int(base, 0)
        h = region_hash(rid, base, len(data), page_hashes(data))
        out.append((rid, h))
        print(f'  {rid} {NAMES.get(rid, "?"):6} 0x{base:08x}+0x{len(data):x}: {h:016x}')
    print(f'State Hash v{HASH_VERSION}: {state_hash(out):016x}')
    return 0


def main():
    if len(sys.argv) >= 3 and sys.argv[1] == 'snapshot':
        return snapshot(sys.argv[2])
    if len(sys.argv) >= 3 and sys.argv[1] == 'regions':
        return regions(sys.argv[2:])
    raise SystemExit(__doc__)


if __name__ == '__main__':
    sys.exit(main())
