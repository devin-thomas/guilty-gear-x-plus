#!/usr/bin/env python3
"""Carry ggx's data symbols to another GGX build through code references.

For every function pair that align.py graded exact or reloc (body identical
modulo relocations), walk both instruction streams together and pair up the
addresses each one builds: lui/%lo pairs, $gp-relative accesses and jal
targets. The votes give a Korean -> target address map. A ggx data symbol is
then placed directly (seen in the votes, unanimous) or by its nearest mapped
neighbours when both sides agree on the same delta.

Writes symbol_addrs-style output for the target and a CSV of how each symbol
was placed. Names and addresses only.
"""
import argparse
import bisect
import csv
import re
import struct
import sys
from collections import Counter, defaultdict

sys.path.insert(0, __import__('os').path.dirname(__file__))
from align import load_elf_text, load_korean  # noqa: E402

SAME = {'exact', 'reloc'}            # body identical modulo relocations


def simm(w):
    v = w & 0xffff
    return v - 0x10000 if v & 0x8000 else v


GPR_LOADS = {0x1e, 0x20, 0x21, 0x23, 0x24, 0x25, 0x27, 0x37}
MEM = GPR_LOADS | {0x1f, 0x28, 0x29, 0x2b, 0x31, 0x39, 0x3f}
CLOBBERED = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 31)   # at v0-v1 a0-a3 t0-t9 ra


def is_branch(w):
    op = w >> 26
    return op in (0x04, 0x05, 0x06, 0x07, 0x14, 0x15, 0x16, 0x17) or op == 0x01 or \
        (op in (0x10, 0x11) and (w >> 21) & 31 == 8)


def refs(words, base, gp):
    """Yield (index, address, kind) for each address the code builds.

    A linear scan, plus one control-flow case: a lui in a branch's delay slot
    also reaches the branch target (gcc puts it there with branch-likely)."""
    hi, carry, clobber_after = {}, {}, -1
    for i, w in enumerate(words):
        if w is None:
            hi.clear()
            continue
        if i in carry:
            hi.update(carry.pop(i))
        op = w >> 26
        rs = (w >> 21) & 31
        rt = (w >> 16) & 31
        if op == 0x0f and i > 0 and words[i - 1] is not None and is_branch(words[i - 1]):
            target = i + simm(words[i - 1])          # (i-1) + 1 + offset
            if target > i:
                carry.setdefault(target, {})[rt] = (w & 0xffff) << 16
        if op == 3:
            yield i, ((base + 4 * i) & 0xf0000000) | ((w & 0x3ffffff) << 2), 'call'
            clobber_after = i + 1            # the call kills caller-saved registers,
                                             # after its delay slot has used them
        elif op == 2:                        # j: a tail call to another function (gcc
            yield i, ((base + 4 * i) & 0xf0000000) | ((w & 0x3ffffff) << 2), 'call'
                                             # sibcalls), so its target is solvable too
        elif op == 0x0f:
            hi[rt] = (w & 0xffff) << 16
        elif op in (0x09, 0x19) or op in MEM:
            if rs == 28:
                yield i, (gp + simm(w)) & 0xffffffff, 'gp'
            elif rs in hi:
                yield i, (hi[rs] + simm(w)) & 0xffffffff, 'lo'
            if op in (0x09, 0x19) or op in GPR_LOADS:
                hi.pop(rt, None)
        elif op == 0:
            hi.pop((w >> 11) & 31, None)
            if w & 0x3f == 0x09:             # jalr
                clobber_after = i + 1
        elif op in (0x08, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x18):
            hi.pop(rt, None)
        if i == clobber_after:
            for r in CLOBBERED:
                hi.pop(r, None)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--upstream', required=True)
    ap.add_argument('--elf', required=True)
    ap.add_argument('--align', required=True, help='align.py CSV for this target')
    ap.add_argument('--gp', type=lambda s: int(s, 0), required=True, help='target $gp')
    ap.add_argument('--kgp', type=lambda s: int(s, 0), default=0x5d40f0)
    ap.add_argument('--out', required=True, help='symbol_addrs-style output')
    ap.add_argument('--kr-elf', help="the Korean SLPM-645.25: Korean words from it, not kappa-db")
    ap.add_argument('--csv', required=True)
    args = ap.parse_args()

    kfuncs, kwords = load_korean(args.upstream)
    if args.kr_elf:
        ka, kw = load_elf_text(args.kr_elf)
        kwords = {ka + 4 * i: w for i, w in enumerate(kw)}
    taddr, twords = load_elf_text(args.elf)
    rows = list(csv.DictReader(open(args.align)))

    votes = defaultdict(Counter)
    for r in rows:
        if r['status'] not in SAME:
            continue
        ka, ta, n = int(r['kr_addr'], 16), int(r['addr'], 16), int(r['size']) // 4
        kw = [kwords.get(ka + 4 * k) for k in range(n)]
        tw = twords[(ta - taddr) // 4:(ta - taddr) // 4 + n]
        kr = {i: a for i, a, _ in refs(kw, ka, args.kgp)}
        tr = {i: a for i, a, _ in refs(tw, ta, args.gp)}
        for i, a in kr.items():
            if i in tr:
                votes[a][tr[i]] += 1

    amap = {}
    conflicts = 0
    for a, c in votes.items():
        if len(c) == 1:
            amap[a] = next(iter(c))
        else:
            conflicts += 1
    keys = sorted(amap)

    # ggx symbols
    syms = []
    for l in open(f'{args.upstream}/config/symbol_addrs.txt'):
        m = re.match(r'\s*(\S+)\s*=\s*(0x[0-9a-fA-F]+);(.*)', l)
        if m:
            syms.append((m.group(1), int(m.group(2), 16), m.group(3)))
    fmap = {r['name']: r for r in rows}

    out, report = [], []
    from collections import Counter as C2
    stat = C2()
    for name, a, rest in syms:
        if 'type:func' in rest:
            r = fmap.get(name)
            if r and r['addr'] and r['status'].split('-')[0] in ('exact', 'reloc', 'struct', 'close') \
                    and int(r['addr'], 16) < taddr + 4 * len(twords):
                out.append(f'{name} = 0x{int(r["addr"], 16):08x}; // type:func')
            continue
        how, ta = None, None
        if a in amap:
            how, ta = 'xref', amap[a]
        else:
            i = bisect.bisect_left(keys, a)
            lo = keys[i - 1] if i > 0 else None
            hi = keys[i] if i < len(keys) else None
            if lo is not None and hi is not None and amap[lo] - lo == amap[hi] - hi \
                    and hi - lo <= 0x10000:
                how, ta = 'between', a + amap[lo] - lo
        stat[how] += 1
        report.append([name, f'0x{a:08x}', f'0x{ta:08x}' if ta is not None else '', how or 'unplaced'])
        if ta is not None:
            out.append(f'{name} = 0x{ta:08x};')

    if not any(l.startswith('ENTRYPOINT ') for l in out):
        entry, = struct.unpack_from('<I', open(args.elf, 'rb').read(), 0x18)
        out.insert(0, f'ENTRYPOINT = 0x{entry:08x}; // type:func')   # ELF e_entry
    seen, dedup = {}, []
    for l in out:                        # splat rejects two names at one address
        a = l.split('=')[1].split(';')[0].strip()
        if a in seen:
            dedup.append(f'// alias of {seen[a]}: {l}')
        else:
            seen[a] = l.split()[0]
            dedup.append(l)
    open(args.out, 'w').write('\n'.join(dedup) + '\n')
    with open(args.csv, 'w', newline='') as fh:
        w = csv.writer(fh, lineterminator='\n')
        w.writerow(['name', 'kr_addr', 'addr', 'how'])
        w.writerows(report)
    print(f'address pairs {len(votes)}, unanimous {len(amap)}, conflicting {conflicts}')
    print(f'data symbols {sum(stat.values())}: ' + ', '.join(f'{k or "unplaced"}={v}' for k, v in stat.items()))
    print(f'function symbols written: {sum(1 for l in out if "type:func" in l)}')


if __name__ == '__main__':
    main()
