#!/usr/bin/env python3
"""Place ggx's named functions (Korean SLPM-645.25) in another GGX build.

The Korean instruction words come from your SLPM-645.25 (--kr-elf) or,
without it, from ggx's kappa-db.json (each function's disassembly carries the
raw word in its comment). The kappa operands also show which instructions are
relocated (%hi/%lo/%gp_rel, jal/j to a symbol, $gp bases).
Function starts come from ggx's config/symbol_addrs.txt. The target ELF is a
stripped build we own.

Each Korean function's body (its words minus trailing alignment padding) is
placed in the target and graded on two axes.

how (placement):
  hash-unique    the body's relocation-masked pattern occurs once in the target
  hash-neighbor  it occurs several times; the one at the offset implied by the
                 nearest uniquely placed neighbours was taken
  gap            not hashed (missing or ambiguous); aligned word-by-word with
                 difflib inside the gap between placed neighbours

evidence (body comparison):
  exact     raw words identical
  reloc     identical except at instructions the Korean asm shows relocated
  struct    identical after masking, but also differs at a non-relocated
            instruction (masked field offset or constant): same shape only
  close     gap-aligned, >= 80% of masked words match
  weak      gap-aligned, < 80%
  partial   (suffix) the Korean body has missing words inside it, so the
            grade covers known words only

Placements below/after .text or onto an address another name already holds
with better evidence are rejected (status collision / out-of-range).
Writes a CSV of names, addresses and grades only (no game bytes).
"""
import argparse
import csv
import difflib
import json
import re
import struct
from collections import Counter, defaultdict

LOADSTORE = {0x1a, 0x1b, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
             0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x31, 0x37, 0x39, 0x3f}
EVIDENCE_RANK = {'exact': 0, 'reloc': 1, 'struct': 2, 'close': 3, 'weak': 4}
HOW_RANK = {'hash-unique': 0, 'hash-neighbor': 1, 'gap': 2}


def norm(w):
    """Mask fields a relocation could change (and, unavoidably, some offsets)."""
    op = w >> 26
    if op in (2, 3):                    # j, jal
        return w & 0xfc000000
    if op == 0x0f:                      # lui
        return w & 0xffff0000
    if op in (0x09, 0x19, 0x0d):        # addiu, daddiu, ori: often %lo / %gp_rel
        rs = (w >> 21) & 31
        if rs not in (0, 29):           # keep li (rs=$0) and stack offsets
            return w & 0xffff0000
        return w
    if op in LOADSTORE:
        rs = (w >> 21) & 31
        if rs != 29:
            return w & 0xffff0000
        return w
    return w


def load_elf_text(path):
    data = open(path, 'rb').read()
    shoff, = struct.unpack_from('<I', data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', data, 0x2e)
    secs = [struct.unpack_from('<IIIIIIIIII', data, shoff + i * shentsize) for i in range(shnum)]
    strtab = secs[shstrndx]
    for s in secs:
        if data[strtab[4] + s[0]:].split(b'\0', 1)[0] == b'.text':
            addr, off, size = s[3], s[4], s[5]
            return addr, list(struct.unpack_from('<%dI' % (size // 4), data, off))
    raise SystemExit('no .text')


RELOC_OPERAND = re.compile(r'%hi\(|%lo\(|%gp_rel\(|\(\$(?:28|gp)\)')


def load_korean(upstream, with_relocs=False):
    funcs = {}
    for l in open(f'{upstream}/config/symbol_addrs.txt'):
        m = re.match(r'\s*(\S+)\s*=\s*(0x[0-9a-fA-F]+);.*type:func', l)
        if m:
            funcs[int(m.group(2), 16)] = m.group(1)
    db = json.load(open(f'{upstream}/kappa-db.json'))
    words, relocs = {}, set()
    pat = re.compile(r'/\*\s*[0-9A-F]+\s+([0-9A-F]{8})\s+([0-9A-F]{8})\s*\*/\s*(\S+)?\s*(.*)')
    for f in db['decompFunctions']:
        for line in f['asmCode'].splitlines():
            m = pat.search(line)
            if not m:
                continue
            a = int(m.group(1), 16)
            if not 0x100000 <= a < 0x1d4180:
                continue
            words[a] = struct.unpack('<I', bytes.fromhex(m.group(2)))[0]
            op, arg = m.group(3) or '', m.group(4).split('/*')[0].strip()
            last = arg.split(',')[-1].strip() if arg else ''
            # registers print with '$'; a bare identifier is a symbol (ggx's
            # configure.py rewrote '%gp_rel(sym)($28)' to 'sym')
            if RELOC_OPERAND.search(arg) or (op in ('jal', 'j') and not last.startswith('.')) \
                    or re.fullmatch(r'[A-Za-z_]\w*', last):
                relocs.add(a)
    return (funcs, words, relocs) if with_relocs else (funcs, words)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--upstream', required=True)
    ap.add_argument('--elf', required=True)
    ap.add_argument('--csv', required=True)
    ap.add_argument('--kr-elf', help="the Korean SLPM-645.25: take the Korean words from it "
                                     "(complete) instead of kappa-db's partial disassembly")
    ap.add_argument('--text-end', type=lambda s: int(s, 0), default=0x1d4154,
                    help='end of Korean .text (last function size)')
    args = ap.parse_args()

    kfuncs, kwords, krelocs = load_korean(args.upstream, with_relocs=True)
    if args.kr_elf:
        ka, kw = load_elf_text(args.kr_elf)
        kwords = {ka + 4 * i: w for i, w in enumerate(kw)}
        args.text_end = ka + 4 * len(kw)
    taddr, twords = load_elf_text(args.elf)
    tnorm = [norm(w) for w in twords]
    tend = taddr + 4 * len(twords)

    starts = sorted(kfuncs)
    kinfo = []
    for i, a in enumerate(starts):
        end = starts[i + 1] if i + 1 < len(starts) else args.text_end
        n = (end - a) // 4
        ws = [kwords.get(a + 4 * k) for k in range(n)]
        body = len(ws)
        while body and ws[body - 1] is None:     # kappa leaves out trailing padding
            body -= 1
        missing = sum(w is None for w in ws[:body])
        kinfo.append(dict(name=kfuncs[a], kaddr=a, size=end - a, words=ws, body=body,
                          missing=missing, taddr=None, how='', evidence='', diff='',
                          status='unplaced'))

    def grade(f, i):
        """Compare f's body with the target at word index i: exact/reloc/struct or None."""
        ws = f['words'][:f['body']]
        if i < 0 or i + len(ws) > len(twords):
            return None
        ev = 'exact'
        for k, w in enumerate(ws):
            if w is None or twords[i + k] == w:
                continue
            if tnorm[i + k] != norm(w):
                return None
            if f['kaddr'] + 4 * k in krelocs:
                ev = 'reloc' if ev == 'exact' else ev
            else:
                ev = 'struct'
        return ev

    K = 6
    index = defaultdict(list)
    for i in range(len(tnorm) - K + 1):
        index[tuple(tnorm[i:i + K])].append(i)

    for f in kinfo:
        ws = f['words'][:f['body']]
        if len(ws) < 2 or f['missing'] or any(w is None for w in ws[:K]):
            continue
        if len(ws) >= K:
            cands = index.get(tuple(norm(w) for w in ws[:K]), [])
        else:
            pat = [norm(w) for w in ws]
            cands = [i for i in range(len(tnorm) - len(ws) + 1) if tnorm[i:i + len(ws)] == pat]
        hits = [(i, ev) for i in cands for ev in [grade(f, i)] if ev]
        if len(hits) == 1:
            f['taddr'], f['evidence'], f['how'] = taddr + 4 * hits[0][0], hits[0][1], 'hash-unique'
        elif hits:
            f['cands'] = hits

    def nearest_delta(idx):
        for d in range(1, 40):
            for j in (idx - d, idx + d):
                if 0 <= j < len(kinfo) and kinfo[j]['how'] == 'hash-unique':
                    return kinfo[j]['taddr'] - kinfo[j]['kaddr']
        return None

    for idx, f in enumerate(kinfo):
        if f.get('cands'):
            delta = nearest_delta(idx)
            for i, ev in f['cands']:
                if delta is not None and taddr + 4 * i == f['kaddr'] + delta:
                    f['taddr'], f['evidence'], f['how'] = taddr + 4 * i, ev, 'hash-neighbor'
                    break

    # Gap pass: runs of unplaced functions against the target bytes between
    # their placed neighbours (normalized words, difflib).
    idx = 0
    while idx < len(kinfo):
        if kinfo[idx]['taddr'] is not None:
            idx += 1
            continue
        j = idx
        while j < len(kinfo) and kinfo[j]['taddr'] is None:
            j += 1
        prev = kinfo[idx - 1] if idx > 0 else None
        nxt = kinfo[j] if j < len(kinfo) else None
        run = kinfo[idx:j]
        idx = j
        t0 = (prev['taddr'] + prev['size'] - taddr) // 4 if prev else 0
        t1 = (nxt['taddr'] - taddr) // 4 if nxt else len(twords)
        if t1 < t0:
            continue
        a_seq, owner = [], []
        for n, f in enumerate(run):
            for k, w in enumerate(f['words']):
                a_seq.append(norm(w) if w is not None else ('none', n, k))
                owner.append((n, k))
        if len(a_seq) * (t1 - t0) > 4e7:
            continue                     # too far apart to align word by word
        sm = difflib.SequenceMatcher(None, a_seq, tnorm[t0:t1], autojunk=False)
        matched, first = [0] * len(run), [None] * len(run)
        for bl in sm.get_matching_blocks():
            for q in range(bl.size):
                n, k = owner[bl.a + q]
                matched[n] += 1
                if first[n] is None:
                    first[n] = t0 + bl.b + q - k
        for n, f in enumerate(run):
            known = sum(w is not None for w in f['words'])
            if first[n] is None or known == 0:
                continue
            f['taddr'], f['how'], f['matched'], f['known'] = taddr + 4 * first[n], 'gap', matched[n], known
        placed = [f for f in run if f['taddr'] is not None]
        for q, f in enumerate(placed):
            end = placed[q + 1]['taddr'] if q + 1 < len(placed) else taddr + 4 * t1
            tsize = end - f['taddr']
            ev = grade(f, (f['taddr'] - taddr) // 4) if tsize >= 4 * f['body'] else None
            if ev:
                f['evidence'] = ev
            else:
                sim = f['matched'] / max(f['known'], tsize // 4, 1)
                f['evidence'] = 'close' if sim >= 0.8 else 'weak'
            f['diff'] = f'{f["known"] - f["matched"]}/{f["known"]} size{tsize - f["size"]:+d}'

    # Reject out-of-range placements and address collisions.
    by_addr = defaultdict(list)
    for f in kinfo:
        if f['taddr'] is None:
            continue
        if not taddr <= f['taddr'] < tend:
            f['taddr'], f['status'] = None, 'out-of-range'
            continue
        by_addr[f['taddr']].append(f)
    for a, fs in by_addr.items():
        fs.sort(key=lambda f: (EVIDENCE_RANK[f['evidence']], HOW_RANK[f['how']]))
        for f in fs[1:]:
            f['status'], f['diff'] = 'collision', f'with {fs[0]["name"]}'
            f['taddr'] = None
    for f in kinfo:
        if f['taddr'] is not None:
            f['status'] = f['evidence'] + ('-partial' if f['missing'] else '')

    with open(args.csv, 'w', newline='') as fh:
        w = csv.writer(fh, lineterminator='\n')
        w.writerow(['name', 'kr_addr', 'size', 'addr', 'status', 'how', 'missing_words', 'diff'])
        for f in kinfo:
            w.writerow([f['name'], f'0x{f["kaddr"]:06x}', f['size'],
                        f'0x{f["taddr"]:06x}' if f['taddr'] is not None else '', f['status'],
                        f['how'] if f['taddr'] is not None else '', f['missing'], f['diff']])

    tot = len(kinfo)
    c = Counter((f['status'], f['how'] if f['taddr'] is not None else '') for f in kinfo)
    print(f'target .text {taddr:#x}-{tend:#x} ({4 * len(twords)} bytes); Korean funcs {tot}')
    for (s, h), n in sorted(c.items(), key=lambda x: -x[1]):
        print(f'  {s:16s} {h:14s} {n:5d} ({100 * n / tot:5.1f}%)')
    t1 = sum(1 for f in kinfo if f['taddr'] is not None and f['status'] in ('exact', 'reloc')
             and f['how'] in ('hash-unique', 'gap'))
    print(f'tier 1 (exact/reloc, complete body, unique or gap-bounded): {t1} ({100 * t1 / tot:.1f}%)')


if __name__ == '__main__':
    main()
