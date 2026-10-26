#!/usr/bin/env python3
"""Byte-identical rebuild of GGX Plus JP (SLPS-201.37) or Korean (SLPM-645.25).

splat splits .text into ggx's translation units, one asm file per named
function. Every function that has matched C in src/ggxeuc/<tu>.c is compiled
on its own (the file's declarations plus that one function) with EE-GCC
2.9-ee-991111 -O2 -G8 and the target's assembler, checked by
scripts/verify.py, and linked in place of its asm. Data, rodata and ELF
metadata stay opaque local bins, except where C owns data: a function's
jump tables, literals and statics (each its own section, scripts/datafrag.py,
placed where verify.py found it), or a TU-level variable defined with an
initializer in src/ggxeuc/<tu>.c, static or not (compiled once with
-fdata-sections and placed at its original address, data_object). Each such
range is cut out of the bin and filled from the compiled object, so the
bytes are checked too.

A TU in GENERATED has no C: a script of ours writes its assembly from a
table of names and numbers (scripts/klib.py, the Sony kernel's syscall
stubs). Each generated function is assembled on its own, byte-checked against
the original at its address, and linked in place of the split's copy.

Generated asm, bins and build output go to --work, never into git.
"""
import argparse
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace

import yaml
from elftools.elf.elffile import ELFFile

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / 'scripts'))
import csplit   # noqa: E402
import verify   # noqa: E402
import sdkprofile   # noqa: E402
import klib   # noqa: E402
from datafrag import kind_of   # noqa: E402

TARGETS = {
    'jp': dict(sha256='8bd352acb95a533abd22f11c7f9da4a3157dc8836fc7d91ae8340bde92d8a411',
               yaml='jp/slps_201.37.yaml', symbols='jp/symbol_addrs.txt',
               basename='SLPS_201.37', gp=0x5d2a70, as_dir=verify.CC_DIR),
    'kr': dict(sha256='ef3278dcf50feee1da1ac423042db6a3eeeaf845a3644a1d625bb97d0024755f',
               yaml='kr/slpm_645.25.yaml', symbols='kr/symbol_addrs.txt',
               basename='SLPM_645.25', gp=0x5d40f0, as_dir=verify.KR_AS_DIR),
}
FLAGS = ['-EL', '-march=5900', '-mabi=eabi', '-no-pad-sections']
# TUs whose assembly is generated from a committed table: TU name -> module with functions(target)
GENERATED = {klib.TU: klib}
DATA_SECTIONS = ('.rodata', '.data', '.sdata', '.lit4', '.lit8')
COP2_LINE = re.compile(r'^(\s*/\* [0-9A-F]+ [0-9A-F]+ ([0-9A-F]{8}) \*/\s+)(v[a-z0-9.]+\s.*|vnop|vwaitq)$',
                       re.M)


def fix_cop2(asm):
    """GAS rejects spimdisasm's VU0 macro-mode operand syntax; emit those as words."""
    for f in asm.rglob('*.s'):
        text = f.read_text()
        new = COP2_LINE.sub(lambda m: f'{m.group(1)}.word 0x{bytes.fromhex(m.group(2))[::-1].hex().upper()}'
                            f' /* {m.group(3)} */', text)
        if new != text:
            f.write_text(new)


def split_tu(text, drop, keep_pad=False):
    """Drop the functions in `drop` from a spimdisasm TU file; put each run of
    kept functions in its own section .text.p<k>. Returns (asm, order) where
    order lists section names and C function names in address order.
    keep_pad: the padding after a dropped function's endlabel stays as asm
    (SDK objects: padding to 16 or more, and objects that start 4 mod 8)."""
    lines = text.splitlines(keepends=True)
    head_end = next(i for i, l in enumerate(lines) if l.startswith('.section .text'))
    starts = []
    for i, l in enumerate(lines):
        m = re.match(r'nonmatching (\S+),', l)
        if m:
            j = i - 1 if i > 0 and lines[i - 1].startswith('.align') else i
            starts.append((j, m.group(1)))
    out, order, part, open_part = lines[:head_end], [], 0, False
    out += lines[head_end + 1:starts[0][0]] if starts else lines[head_end + 1:]
    for k, (j, name) in enumerate(starts):
        end = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
        if name in drop:
            order.append(name)
            open_part = False
            tail = next((i + 1 for i in range(j, end) if lines[i].startswith('endlabel ')), end)
            pad = [l for l in lines[tail:end] if re.match(r'\s*/\* [0-9A-F]+ [0-9A-F]+ [0-9A-F]+ \*/', l)]
            if keep_pad and pad:
                # only the padding words: an .align here belongs to the next function
                out.append(f'\n.section .text.p{part}, "ax"\n')
                order.append(f'.text.p{part}')
                part += 1
                open_part = True
                out += pad
            continue
        if not open_part:
            out.append(f'\n.section .text.p{part}, "ax"\n')
            order.append(f'.text.p{part}')
            part += 1
            open_part = True
        out += lines[j:end]
    if keep_pad:
        # every word is in place, padding included; an .align would make the
        # linker pad a part that starts 4 mod 8 after a C function
        out = out[:head_end] + [l for l in out[head_end:] if not l.startswith('.align')]
    return ''.join(out), order


def strip_attributes(text):
    """Drop every __attribute__((...)) (balanced parentheses), so that
    csplit.data_definition sees `int t[4] __attribute__((section(".rodata"))) = {...}`
    as the data definition it is."""
    while True:
        m = re.search(r'__attribute__\s*\(', text)
        if not m:
            return text
        depth, i = 0, m.end() - 1
        while i < len(text):
            depth += {'(': 1, ')': -1}.get(text[i], 0)
            if depth == 0:
                break
            i += 1
        text = text[:m.start()] + text[i + 1:]


def unstatic(text):
    """Drop a leading `static` (after comments) from a function or prototype."""
    return re.sub(r'^((?:\s|/\*.*?\*/)*)static\s+', r'\1', text, count=1, flags=re.S)


def is_proto(chunk):
    body = re.sub(r'/\*.*?\*/|//[^\n]*', '', chunk, flags=re.S).strip()
    return body.startswith('static') and body.endswith(');') and '(' in body and '=' not in body


def section_bytes(obj, name):
    with obj.open('rb') as f:
        return ELFFile(f).get_section_by_name(name).data()


def dedupe(carves, nobits):
    """Two functions may each carry a copy of one original table or literal
    (gcc shares a TU's identical strings; a local array written in both).
    A copy that lies inside data already placed, with the same bytes there:
    keep the first as file bytes, link the others there as NOLOAD so their
    references resolve."""
    out = []
    for c in carves:
        if out and out[-1][0] <= c[0] and c[0] + c[1] <= out[-1][0] + out[-1][1]:
            at = c[0] - out[-1][0]
            if section_bytes(out[-1][2], out[-1][3])[at:at + c[1]] == section_bytes(c[2], c[3]):
                nobits.append((c[0], c[2], c[3]))
                continue
        out.append(c)
    return out


def data_object(venv, work, tu, prelude, funcs, names, known, statics, near, where, flags, as_dir, local_h=()):
    """Compile a TU's data definitions alone with -fdata-sections and datafrag's
    per-label sections. Each named variable goes at its address: `names`
    (global, from the symbol list) and `known` (statics the TU's matched
    functions reach, name -> address). Sections nothing names (string
    literals, statics no function reaches) are found through the pointers in
    placed data, as verify.py does. A section is linked at its address only
    if its linked bytes equal the original there; the rest stay in the bins.
    A static nothing points at (idtb) is looked for by its bytes, if it has no
    relocations, in a window around the TU's other placed data (`near`:
    address ranges); `where` says whether the source defines it before or
    after all the TU's functions ('before'/'after'), so the TU's data must
    start right after it or end right before it. It is placed only where that
    leaves exactly one occurrence, and its bytes are checked like the rest.
    Returns (carves, nobits, left): carves (address, size, object, section),
    sections to link out of the way, and the variables left in the bins."""
    ddir = work / 'cc' / tu / '_data'
    ddir.mkdir(parents=True, exist_ok=True)
    protos = ''.join(csplit.prototype(t) + '\n' for n, t in funcs if not t.lstrip().startswith('static'))
    stem = Path(tu).name
    (ddir / f'{stem}_data.c').write_text(''.join(prelude) + '\n' + protos)
    for hdr in local_h:
        shutil.copy(hdr, ddir / hdr.name)
    obj, out = verify.compile_c(venv, ddir / f'{stem}_data.c', ddir / 'c', flags + ' -fdata-sections',
                                as_dir=as_dir)
    if obj is None:
        raise SystemExit(f'{tu}: data does not compile:\n{out[-2000:]}')
    dobj = work / 'build' / 'src' / tu / '_data.o'
    dobj.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy(obj, dobj)
    with dobj.open('rb') as f:
        e = ELFFile(f)
        allsecs = list(e.iter_sections())
        secs = {s.name: (s['sh_size'], s['sh_type'] == 'SHT_NOBITS') for s in allsecs
                if s['sh_flags'] & 2 and s['sh_size'] and s.name != '.reginfo'}
        home, undefined = {}, set()
        for sym in e.get_section_by_name('.symtab').iter_symbols():
            sh = sym['st_shndx']
            if sym.name and sh == 'SHN_UNDEF':
                undefined.add(sym.name)
            elif sym.name and isinstance(sh, int) and sym['st_info']['type'] in ('STT_OBJECT', 'STT_NOTYPE'):
                home.setdefault(sym.name, (allsecs[sh].name, sym['st_value']))
    sym_addr = {}
    for n in sorted(undefined - {'_gp'}):
        if n not in venv.syms:
            raise SystemExit(f'{tu}: data refers to {n}, which has no address in this build')
        sym_addr[n] = venv.syms[n]
    fixed = {}
    for n, a in [(n, venv.syms.get(n)) for n in names] + sorted(known.items()):
        sec, value = home.get(n, (None, None))
        if sec in secs and value == 0 and a is not None:
            fixed[sec] = a
    probes, cursor = {}, 0x0d000000
    for sname, (size, nobits) in secs.items():
        if sname not in fixed and not nobits:
            probes[sname] = cursor
            cursor = (cursor + size + 0x1f) & ~0xf
    progbits = [n for n, (_, nb) in secs.items() if not nb]
    solved = dict(fixed)
    rmap = verify.reloc_map(dobj)                     # (section, offset) of each pointer
    vdir = ddir / 'v'
    vdir.mkdir(exist_ok=True)
    data = {}
    for _ in range(8):
        sec_addr = {n: solved.get(n, probes.get(n, 0x0e000000)) for n in progbits}
        elf, missing, err = verify.link(venv, dobj, vdir, {}, sec_addr, sym_addr, progbits)
        if not elf:
            raise SystemExit(f'{tu}: data does not link: {err[-800:]}')
        with elf.open('rb') as f:
            data = {sec.name: (sec['sh_addr'], sec['sh_size'], sec.data()) for sec in ELFFile(f).iter_sections()
                    if sec.name in progbits}
        more = {}
        for sname in list(solved):
            pa, size, content = data.get(sname, (None, 0, None))
            orig = venv.read(pa, size) if content is not None else None
            if orig is None:
                continue
            for off in range(0, size - 3, 4):
                if (sname, off) not in rmap:
                    continue
                w = struct.unpack_from('<I', content, off)[0]
                ow = struct.unpack_from('<I', orig, off)[0]
                for p, base in probes.items():
                    if p not in solved and base <= w <= base + secs[p][0]:
                        more.setdefault(p, set()).add(ow - (w - base))
        new = {p: a.pop() for p, a in more.items() if len(a) == 1}
        if not new:
            break
        solved.update(new)
    # statics no code or data points at: search their bytes near the TU's data
    relocated = {sec for sec, _ in rmap}
    for n in statics:
        sec, value = home.get(n, (None, None))
        if sec not in secs or sec in solved or sec in relocated or value != 0 or secs[sec][1]:
            continue
        with dobj.open('rb') as f:
            want = ELFFile(f).get_section_by_name(sec).data()
        hits = set()
        for lo, hi in near:
            lo, hi = lo - 0x40, hi + 0x40
            hay = venv.read(lo, hi - lo) or b''
            at = hay.find(want)
            while at != -1:
                hits.add(lo + at)
                at = hay.find(want, at + 1)
        side = where.get(n)
        if len(hits) > 1 and side:
            # the TU's data starts right after it ('before') or ends right
            # before it ('after'): keep the hit with the smallest such gap
            gaps = {}
            for h in hits:
                g = [lo - (h + len(want)) for lo, _ in near] if side == 'before' else [h - hi for _, hi in near]
                g = [x for x in g if 0 <= x <= 0x10]
                if g:
                    gaps[h] = min(g)
            best = min(gaps.values(), default=None)
            hits = {h for h, g in gaps.items() if g == best}
        if len(hits) == 1:
            solved[sec] = hits.pop()
    if any(sec in solved and data.get(sec, (None,))[0] != solved[sec] for sec in progbits):
        sec_addr = {n: solved.get(n, probes.get(n, 0x0e000000)) for n in progbits}
        elf, missing, err = verify.link(venv, dobj, vdir, {}, sec_addr, sym_addr, progbits)
        if not elf:
            raise SystemExit(f'{tu}: data does not link: {err[-800:]}')
        with elf.open('rb') as f:
            data = {sec.name: (sec['sh_addr'], sec['sh_size'], sec.data()) for sec in ELFFile(f).iter_sections()
                    if sec.name in progbits}
    carves, nobits, ok = [], [], set()
    for sname in progbits:
        pa, size, content = data.get(sname, (None, 0, None))
        if sname in solved and content is not None and venv.read(pa, size) == content:
            carves.append((pa, size, dobj, sname))
            ok.add(sname)
        else:
            nobits.append((None, dobj, sname))
    nobits += [(None, dobj, n) for n, (_, nb) in secs.items() if nb]
    left = sorted(n for n in set(names) | set(known) | set(statics) if home.get(n, (None,))[0] not in ok)
    return carves, nobits, left


# what a matched function's verify folder keeps: its object (linked, and the cache)
KEEP = {'out.o'}


def prune(v):
    """Drop a matched function's compile intermediates (header copy, preprocessed source,
    assembly, link output): about 3.8 MB per function, 16 GB per full build otherwise."""
    for p in v.iterdir():
        if p.name in KEEP:
            continue
        if p.is_dir() and not p.is_symlink():
            shutil.rmtree(p, ignore_errors=True)
        else:
            p.unlink(missing_ok=True)


def generated_functions(gen, tu, target, work, as_, venv, tu_range, run):
    """Assemble each function `gen` writes for TU `tu` into build/src/<tu>/<name>.o and
    byte-check it against the original at its address. Returns (rows, failed): the
    linker-script line of each match, and the names that differ (left as the split's asm)."""
    rows, failed = {}, []
    lo, hi = tu_range[tu]
    d = work / 'build' / 'src' / tu
    d.mkdir(parents=True, exist_ok=True)
    for fname, text in gen.functions(target):
        addrs = sorted(venv.funcs.get(fname, ())) or ([venv.syms[fname]] if fname in venv.syms else [])
        addr = next((a for a in addrs if lo <= a < hi), None)
        if addr is None:
            raise SystemExit(f'{tu}: {fname} has no address in this TU')
        src, fobj = d / f'{fname}.s', d / f'{fname}.o'
        src.write_text(text)
        run([as_, *FLAGS, '-o', fobj, src])
        with fobj.open('rb') as f:
            e = ELFFile(f)
            code = e.get_section_by_name(f'.text.{fname}').data()
            relocated = any(sec.name.endswith(f'.text.{fname}') and sec['sh_type'] in ('SHT_REL', 'SHT_RELA')
                            for sec in e.iter_sections())
        if relocated or not code or venv.read(addr, len(code)) != code:
            failed.append(fname)
            continue
        rows[fname] = f'{fobj.relative_to(work).as_posix()}(.text.{fname});'
    return rows, failed


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', choices=TARGETS, default='jp')
    ap.add_argument('--elf', required=True, type=Path)
    ap.add_argument('--work', required=True, type=Path)
    ap.add_argument('--binutils', required=True, type=Path)
    ap.add_argument('--no-split', action='store_true', help='reuse an existing split')
    ap.add_argument('--asm-only', action='store_true', help='ignore src/ (repo zero)')
    ap.add_argument('--runner', default='env',
                    help='command prefix for the EE-GCC container')
    ap.add_argument('--map', action='append', default=[],
                    help='host=guest path pair visible in the container (repeatable)')
    ap.add_argument('--flags', default='-O2 -G8')
    ap.add_argument('--keep-going', action='store_true',
                    help='leave non-matching C functions as assembly, list them, and exit 1 at the end')
    ap.add_argument('--no-cache', action='store_true',
                    help='re-verify every function even if its inputs are unchanged')
    ap.add_argument('--objdiff', action='store_true',
                    help='after a byte-identical build, write <work>/objdiff.json and its objects '
                         '(scripts/objdiff_units.py)')
    ap.add_argument('--objdiff-cli', type=Path,
                    help='with --objdiff: also run objdiff-cli report generate into <work>/report.json')
    args = ap.parse_args()
    t = TARGETS[args.target]
    elf, work, bu = args.elf.resolve(), args.work.resolve(), args.binutils.resolve()
    base = t['basename']

    original = elf.read_bytes()
    if hashlib.sha256(original).hexdigest() != t['sha256']:
        raise SystemExit(f'input is not {base} (SHA-256 mismatch)')
    if any((p / '.git').exists() for p in [work, *work.parents]):
        raise SystemExit('refusing to write generated files inside a git clone')
    work.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, TMPDIR=str(work / 'tmp'), XDG_CACHE_HOME=str(work / 'cache'))
    (work / 'tmp').mkdir(exist_ok=True)
    log = (work / 'build.log').open('a')

    def run(cmd):
        log.write('COMMAND ' + ' '.join(map(str, cmd)) + '\n')
        log.flush()
        subprocess.run([str(c) for c in cmd], cwd=work, env=env, stdout=log, stderr=log, check=True)

    config = yaml.safe_load((HERE / t['yaml']).read_text())
    config['sha1'] = hashlib.sha1(original).hexdigest()
    config['options']['target_path'] = str(elf)
    config['options']['base_path'] = str(work)
    shutil.copy(HERE / t['symbols'], work / 'symbol_addrs.txt')
    (work / 'splat.yaml').write_text(yaml.safe_dump(config, sort_keys=False))
    if not args.no_split:
        for d in ('asm', 'src', 'assets', 'build'):
            shutil.rmtree(work / d, ignore_errors=True)
        run([sys.executable, '-m', 'splat', 'split', 'splat.yaml'])
        fix_cop2(work / 'asm')

    tus = [s for s in config['segments'] if isinstance(s, dict)][0]['subsegments']
    bins = [(s[0], s[2]) for s in config['segments'] if isinstance(s, list) and len(s) == 3]
    as_ = bu / 'mips-ps2-decompals-as'
    (work / 'build').mkdir(exist_ok=True)
    ld_text = (work / f'{base}.ld').read_text()

    venv = None
    if not args.asm_only:
        verify.set_gp(t['gp'])
        venv = verify.Env(SimpleNamespace(runner=args.runner, map=args.map, binutils=str(bu),
                                          elf=str(elf), target=args.target,
                                          undefined=str(work / 'undefined_syms_auto.txt')))

    # everything besides a function's own source that decides its object
    h = hashlib.sha256(f'{args.target} {args.flags} {t["as_dir"]}'.encode())
    # verify.py and the modules it imports decide how a function compiles and where its data goes
    tools = [HERE / 'scripts' / n for n in ('verify.py', 'datafrag.py', 'xref.py')]
    for f in sorted((HERE / 'include').rglob('*')) + tools + [HERE / t['symbols']]:
        if f.is_file():
            h.update(str(f.relative_to(HERE)).encode() + f.read_bytes())
    stamp = h.hexdigest()
    ported, carves, commons, nobits, solved, failed, data_skipped = [], [], {}, [], {}, [], []
    generated = []          # the part of `ported` assembled from a GENERATED table, not compiled
    # each TU's address range: a repeated function name (a file static) resolves to the copy in it
    tu_range = {name: (off + 0xff000, (tus[k + 1][0] if k + 1 < len(tus) else off + 0x100000) + 0xff000)
                for k, (off, kind, name) in enumerate(tus)}
    for off, kind, name in tus:
        obj = work / 'build' / (f'asm/{name}.s.o' if kind == 'asm' else f'src/{name}.c.o')
        obj.parent.mkdir(parents=True, exist_ok=True)
        src_c = HERE / 'src' / f'{name}.c'

        def link_parts(rows, keep_pad):
            """Assemble the split TU minus the functions in `rows` into `obj`, and put
            those functions' objects in its place in the linker script."""
            nonlocal ld_text
            asm, order = split_tu((work / f'asm/{name}.s').read_text(), set(rows), keep_pad=keep_pad)
            missing = set(rows) - set(order)
            if missing:
                raise SystemExit(f'{name}: functions not found in the split: {sorted(missing)}')
            parts = work / f'build/src/{name}.parts.s'
            parts.parent.mkdir(parents=True, exist_ok=True)
            parts.write_text(asm)
            run([as_, *FLAGS, '-I', 'include', '-o', obj, f'build/src/{name}.parts.s'])
            here = obj.relative_to(work).as_posix()
            lines = [f'{here}({x});' if x.startswith('.text.p') else rows[x] for x in order]
            old = f'{here}(.text);'
            if old not in ld_text:
                raise SystemExit(f'{name}: no input line in the linker script')
            ld_text = ld_text.replace(old, '\n        '.join(lines))

        if name in GENERATED and venv is not None:
            # assembly a script writes from a committed table (scripts/klib.py)
            rows, bad = generated_functions(GENERATED[name], name, args.target, work, as_, venv, tu_range, run)
            for fname in bad:
                msg = f'{name}: generated {fname} does not match {base}'
                if not args.keep_going:
                    raise SystemExit(msg)
                print(msg, flush=True)
                failed.append(fname)
            if rows:
                link_parts(rows, keep_pad=True)
            else:
                run([as_, *FLAGS, '-I', 'include', '-o', obj, f'asm/{name}.s'])
            ported += list(rows)
            generated += list(rows)
            continue
        if kind == 'asm' or venv is None or not src_c.exists():
            # all-asm TU: splat's full-TU disassembly (make_full_disasm_for_code)
            run([as_, *FLAGS, '-I', 'include', '-o', obj, f'asm/{name}.s'])
            continue
        prelude, funcs = csplit.split(sdkprofile.source(name, src_c.read_text(), args.target))
        # SDK and C-runtime TUs have their own compiler flags and assembler
        flags, as_dir = sdkprofile.profile(name, args.flags, t['as_dir'])
        sdk_tu = name.startswith('sdk/')
        tstamp = stamp if (flags, as_dir) == (args.flags, t['as_dir']) else f'{stamp} {flags} {as_dir}'
        # headers kept next to the source (src/sdk/...) are copied into each capsule
        local_h = sorted(src_c.parent.glob('*.h'))
        tstamp += ''.join(h.name + hashlib.sha256(h.read_bytes()).hexdigest() for h in local_h)
        # TU-level data with initializers: compiled once (one section per
        # variable), linked at its original address (data_object); the
        # functions see externs. Static ones too: their names (idtb) recur
        # across TUs, so a static is placed only where this TU's matched
        # functions, or pointers in its placed data, show its address.
        defs = [(c, csplit.data_definition(strip_attributes(c))) for c in prelude]
        is_static = lambda c: re.sub(r'/\*.*?\*/|//[^\n]*', '', c, flags=re.S).strip().startswith('static')
        datavars = [d[0] for c, d in defs if d and not is_static(c)]
        statics = [d[0] for c, d in defs if d and is_static(c)]
        fprelude = [d[1] + '\n' if d else c for c, d in defs]
        # static inline stand-ins (INLINE_STANDIN) have no code of their own in the
        # original: callers inline them. They join every function's prelude and are not placed.
        inline = [(n, t) for n, t in funcs if n not in venv.syms and
                  re.sub(r'/\*.*?\*/|//[^\n]*', '', t, flags=re.S).lstrip().startswith('static inline')]
        funcs = [f for f in funcs if f not in inline]
        fprelude += [t + '\n' for _, t in inline]
        reached = set()
        rows, objs = {}, {}
        for fname, ftext in funcs:
            if fname not in venv.syms:
                raise SystemExit(f'{name}: {fname} has no address in {base}')
            fdir = work / 'cc' / name / fname
            fdir.mkdir(parents=True, exist_ok=True)
            # the TU's declarations, prototypes of its other functions (gcc's
            # register allocation depends on callee return types), this function
            # Each function is linked from its own object, so a static one must
            # be global there (linkage doesn't change the code)
            protos = ''.join(unstatic(csplit.prototype(t)) + '\n' for n, t in funcs if n != fname)
            src_text = ''.join(unstatic(c) if is_proto(c) else c for c in fprelude) + '\n' + protos + \
                '\n' + unstatic(ftext) + '\n'
            (fdir / f'{fname}.c').write_text(src_text)
            for hdr in local_h:
                shutil.copy(hdr, fdir / hdr.name)
            # cache: same source, headers, verifier, flags and target -> same object
            key = hashlib.sha256((tstamp + src_text).encode()).hexdigest()
            cached = fdir / 'cache.json'
            r = None
            if not args.no_cache and cached.exists() and (fdir / 'v' / 'out.o').exists():
                c = json.loads(cached.read_text())
                if c.get('key') == key:
                    r = c['result']
            if r is None:
                res = verify.check(venv, fdir / f'{fname}.c', fdir / 'v', flags,
                                   as_dir=as_dir, prefer=tu_range[name])
                r = next((x for x in res if x.get('name') == fname), res[0])
                r.pop('log', None)
                if r.get('status') == 'match':
                    cached.write_text(json.dumps({'key': key, 'result': r}))
                    prune(fdir / 'v')
            if r.get('status') != 'match':
                msg = (f'{name}: {fname} does not match {base}: '
                       f'{json.dumps({k: v for k, v in r.items() if k != "log"})[:400]}')
                if not args.keep_going:
                    raise SystemExit(msg)
                print(msg, flush=True)
                failed.append(fname)
                continue                      # stays assembly
            problem = next((f'{sym} solved to {solved[sym]} and {a}' for sym, a in r.get('solved', {}).items()
                            if sym in solved and solved[sym] != a), None) or \
                next((f'{name}: {fname} {s} not placed/checked: {w}' for s, w in r.get('data', {}).items()
                      if kind_of(s) in DATA_SECTIONS and not w.startswith('unused')
                      and (w.startswith('unplaced') or not r.get('data_ok'))), None)
            if problem:
                if not args.keep_going:
                    raise SystemExit(problem)
                print(problem, flush=True)
                failed.append(fname)
                continue                      # stays assembly
            fobj = work / 'build' / 'src' / name / f'{fname}.o'
            fobj.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(fdir / 'v' / 'out.o', fobj)
            _, secs, objcommons, undefined = verify.obj_info(fobj)
            commons.update(objcommons)
            reached |= undefined
            reached |= {sym for sym in r.get('solved', {}) if sym in statics}
            for sym, a in r.get('solved', {}).items():
                # an extern with no symbol_addrs entry, located by verify.py
                solved.setdefault(sym, a)
            for sname, where in r.get('data', {}).items():
                if where.startswith('unused'):
                    # a fragment of data this function doesn't use (another
                    # function's literal or static): linked out of the way
                    nobits.append((None, fobj, sname))
                elif kind_of(sname) in DATA_SECTIONS:
                    addr, size = where.split('+')
                    carves.append((int(addr, 16), int(size), fobj, sname))
                elif kind_of(sname) in ('.bss', '.sbss'):
                    # static variables: no file bytes, but the code must see
                    # them at their original address
                    addr = None if where.startswith('unplaced') else int(where.split('+')[0], 16)
                    nobits.append((addr, fobj, sname))
            # game functions end 8-aligned; an SDK function's padding is kept as asm
            rows[fname] = f'{fobj.relative_to(work)}(.text.{fname});' + ('' if sdk_tu else ' . = ALIGN(8);')
        if datavars or statics:
            known = {n: int(solved[n], 16) if n in solved else venv.syms.get(n)
                     for n in statics if n in reached}
            try:
                # where this TU's functions put their data: windows for a static
                # nothing refers to
                near = sorted((c[0], c[0] + c[1]) for c in carves if f'/src/{name}/' in str(c[2]))
                text = sdkprofile.source(name, src_c.read_text(), args.target)
                first = min(text.find(ft) for _, ft in funcs) if funcs else len(text)
                last = max(text.find(ft) + len(ft) for _, ft in funcs) if funcs else 0
                where = {}
                for c, d in defs:
                    if d and is_static(c):
                        at = text.find(c)
                        where[d[0]] = 'before' if 0 <= at < first else 'after' if at >= last else None
                dcarves, dnobits, left = data_object(venv, work, name, prelude, funcs, datavars,
                                                     {n: a for n, a in known.items() if a is not None},
                                                     statics, near, where, flags, as_dir, local_h)
                carves += dcarves
                nobits += dnobits
                if left:
                    print(f'{name}: data {left} left in the bins', flush=True)
                    data_skipped.append(name)
            except SystemExit as e:
                if not args.keep_going:
                    raise
                # the functions still reach the variables by symbol; the bytes stay in the bins
                print(f'{e} -> data left in the bins', flush=True)
                data_skipped.append(name)
        link_parts(rows, keep_pad=sdk_tu)
        ported += list(rows)

    # Opaque bins; cut out the ranges that compiled data fills.
    (work / 'build/assets').mkdir(parents=True, exist_ok=True)
    for i, (foff, bname) in enumerate(bins):
        data = (work / f'assets/{bname}.bin').read_bytes()
        vram = foff + 0xff000                 # single LOAD segment: file 0x1000 -> 0x100000
        mine = sorted((c for c in carves if vram <= c[0] < vram + len(data)), key=lambda c: (c[0], -c[1]))
        mine = dedupe(mine, nobits)
        pieces, pos = [], 0
        for addr, size, fobj, sname in mine:
            cut = addr - vram
            if cut < pos or cut + size > len(data):
                raise SystemExit(f'overlapping or out-of-range data at 0x{addr:x}')
            pieces.append(('bin', data[pos:cut]))
            pieces.append(('obj', f'{fobj.relative_to(work)}({sname});'))
            pos = cut + size
        pieces.append(('bin', data[pos:]))
        rows = []
        for k, (kind, p) in enumerate(pieces):
            if kind == 'obj':
                rows.append(p)
                continue
            if not p:
                continue
            stem = bname if len(pieces) == 1 else f'{bname}.part{k}'
            (work / f'build/assets/{stem}.bin').write_bytes(p)
            wrapper = work / f'build/assets/{stem}.s'
            wrapper.write_text(f'.section .data\n.incbin "build/assets/{stem}.bin"\n')
            run([as_, *FLAGS, '-o', work / f'build/assets/{stem}.bin.o', wrapper])
            rows.append(f'build/assets/{stem}.bin.o(.data);')
        old = f'build/assets/{bname}.bin.o(.data);'
        if old not in ld_text:
            raise SystemExit(f'{bname}: no input line in the linker script')
        ld_text = ld_text.replace(old, '\n        '.join(rows))
    if nobits:
        secs = []
        for k, (addr, fobj, sname) in enumerate(nobits):
            # unreferenced copies of a TU's statics go somewhere harmless
            at = addr if addr is not None else 0x0f800000 + 0x10000 * k
            secs.append(f'    .nobits{k} 0x{at:08x} (NOLOAD) : {{ {fobj.relative_to(work)}({sname}) }}')
        i = ld_text.rindex('/DISCARD/')
        ld_text = ld_text[:i] + '\n'.join(secs) + '\n\n    ' + ld_text[i:]
    (work / 'build/link.ld').write_text(ld_text)

    # common (tentative) variables in the C live in the opaque data: pin them
    known = dict(venv.syms) if venv else {}
    known.update({n: int(a, 16) for n, a in solved.items()})
    unknown = sorted(n for n in commons if n not in known)
    if unknown:
        raise SystemExit(f'common variables without an address: {unknown}')
    pins = sorted(set(commons) | set(solved))
    # A name can mean one address only. A JP-named D_ stand-in solved on another disc can collide
    # with that disc's own auto symbol of the same name (KR: file.c D_5B83A0 vs SDK SetArg's).
    auto = dict(re.findall(r'^(\w+) = (0x[0-9A-Fa-f]+);', (work / 'undefined_syms_auto.txt').read_text(), re.M))
    clash = [n for n in pins if n in auto and int(auto[n], 16) != known[n]]
    if clash:
        raise SystemExit(f"symbol name collisions (C stand-in vs this disc's auto symbol): "
                         + ', '.join(f'{n} C=0x{known[n]:x} auto={auto[n]}' for n in clash))
    (work / 'build/commons.ld').write_text(''.join(f'{n} = 0x{known[n]:08x};\n' for n in pins))

    linked, output = work / 'build/linked.elf', work / f'build/{base}'
    # --no-check-sections: NOLOAD statics sit inside the trailer bin's address
    # range; they emit nothing, and the byte comparison below checks the rest
    run([bu / 'mips-ps2-decompals-ld', '-EL', '--no-check-sections',
         # commons.ld last: a later assignment wins, and a solved extern (a JP-named
         # D_ stand-in placed on this disc) must beat splat's auto symbol of the same name
         '-T', 'build/link.ld', '-T', 'undefined_funcs_auto.txt', '-T', 'undefined_syms_auto.txt',
         '-T', 'build/commons.ld',
         '-Map', f'build/{base}.map', '-o', linked])      # the script names every object
    with linked.open('rb') as f:
        built_text = ELFFile(f).get_section_by_name('.text').data()
    with elf.open('rb') as f:
        target_text = ELFFile(f).get_section_by_name('.text').data()
    if built_text != target_text:
        n = min(len(built_text), len(target_text))
        first = next((i for i in range(n) if built_text[i] != target_text[i]), n)
        raise SystemExit(f'.text differs (built {len(built_text)}, target {len(target_text)}, '
                         f'first diff at 0x{0x100000 + first:x})')
    run([bu / 'mips-ps2-decompals-objcopy', '-O', 'binary', linked, output])
    result = output.read_bytes()
    for addr, size, fobj, sname in carves:      # name the C data that differs
        off = addr - 0xff000
        if result[off:off + size] != original[off:off + size]:
            raise SystemExit(f'C data {fobj.name}({sname}) at 0x{addr:x} differs from {base}')
    if result != original:
        n = min(len(result), len(original))
        first = next((i for i in range(n) if result[i] != original[i]), n)
        raise SystemExit(f'full ELF differs although .text matches (first diff at file 0x{first:x})')
    report = {'target': base, 'original_sha256': t['sha256'],
              'rebuilt_sha256': hashlib.sha256(result).hexdigest(),
              'elf_bytes': len(result), 'text_bytes': len(built_text),
              'text_byte_identical': True, 'elf_byte_identical': True,
              'c_functions': len(ported), 'c_data_ranges': len(carves),
              'c_function_names': ported}
    report['c_functions_not_linked'] = failed
    # c_function_names counts these too: functions assembled from a committed table, not C
    report['generated_asm_functions'] = generated
    report['tu_data_left_in_bins'] = data_skipped
    (work / 'result.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k != 'c_function_names'}, indent=2))
    if args.objdiff:
        import objdiff_units   # noqa: E402
        objdiff_units.emit(work, args.target, bu, args.objdiff_cli)
    if failed:
        sys.exit(f'{len(failed)} C functions did not match and were left as assembly: {failed}')


if __name__ == '__main__':
    main()
