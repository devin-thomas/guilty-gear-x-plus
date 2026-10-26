#!/usr/bin/env python3
"""Audit a tree for things the public repo must never contain.

Checks every file (git-tracked and untracked-but-not-ignored, or every file of
a plain directory) for:

- binaries: NUL bytes, or the magic of an ELF, ISO 9660 image, archive, image,
  audio or video file;
- assets, disc images and build products, by extension (images, audio, video,
  fonts, archives, .iso/.bin/.cue/.chd, .elf/.irx/.o/.a, ...);
- logos and artwork: any image or icon file, and files named logo/banner/boxart;
- assembly: .s/.S/.asm/.inc files, `.incbin`, INCLUDE_ASM/INCLUDE_RODATA, and
  text with 10 or more lines that look like a disassembly listing (spimdisasm's
  `/* offset vram word */` comments, objdump's `addr: word  insn`, `glabel`);
- disc data in source: a C initializer with more than 1,024 numeric literals
  (the largest game table in src/ has 490), or a run of 2,000+ base64/hex
  characters;
- compiler dumps (gcc -da RTL, .i preprocessor output): not game data, but
  never wanted in the repository either;
- generator tables (.tbl, the input scripts/klib.py turns into the kernel's
  syscall stubs at build time): every line must be a comment, blank, or a
  `number name [jp|kr]` row. A table holds names and numbers, never code.

Writes a Markdown report. Exit status 1 if any finding is in scope.

  audit_repo.py [--root DIR] [--publish] [--extra DIR=PREFIX] [--out AUDIT.md]

In the public repo, run it with no arguments.
"""
import argparse
import os
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent.parent

# The public file set, as paths in the working tree it is staged from: the matching
# build and what it needs, the toolchain fetch, this audit and FAKEMATCHES.md.
# (--publish limits an audit of that larger tree to these paths.)
PUBLISH = ['build.py', 'FAKEMATCHES.md', 'src/', 'include/', 'nonmatching/',
           'kr/slpm_645.25.yaml', 'kr/symbol_addrs.txt', 'jp/slps_201.37.yaml', 'jp/symbol_addrs.txt',
           'scripts/align.py', 'scripts/csplit.py', 'scripts/datafrag.py', 'scripts/sdkprofile.py',
           'scripts/verify.py', 'scripts/xref.py', 'scripts/patch_kr_as.py', 'scripts/fetch_toolchain.py',
           'scripts/make_kr_symbols.py', 'scripts/audit_repo.py', 'scripts/klib.py']
PUBLISH_EXCLUDE = ['include/generated/README.md']

ASSET_EXT = {
    'image': {'.png', '.jpg', '.jpeg', '.gif', '.bmp', '.tga', '.tif', '.tiff', '.webp', '.svg', '.ico',
              '.icns', '.psd', '.tim', '.tm2', '.tex', '.dds', '.xcf', '.kra'},
    'audio': {'.wav', '.mp3', '.ogg', '.flac', '.aiff', '.vag', '.vb', '.vh', '.adpcm', '.at3', '.sq', '.hd', '.bd', '.mid', '.midi'},
    'video': {'.mp4', '.mov', '.avi', '.mkv', '.webm', '.pss', '.m2v', '.ipu'},
    'font': {'.ttf', '.otf', '.fnt', '.woff', '.woff2'},
    'disc image': {'.iso', '.bin', '.cue', '.chd', '.img', '.mdf', '.mds', '.nrg', '.cso', '.zso', '.ecm'},
    'executable or object': {'.elf', '.irx', '.erx', '.o', '.a', '.so', '.dll', '.exe', '.dylib', '.lib', '.obj', '.pyc'},
    'archive': {'.zip', '.7z', '.rar', '.tar', '.gz', '.tgz', '.xz', '.bz2', '.zst', '.lz4'},
    'assembly': {'.s', '.asm', '.inc', '.mips'},
    'compiler dump': {'.i', '.rtl', '.jump', '.cse', '.gcse', '.loop', '.cse2', '.bp', '.flow', '.combine', '.regmove',
                      '.sched', '.lreg', '.greg', '.sched2', '.flow2', '.jump2', '.mach', '.dbr', '.addressof', '.range',
                      '.blob', '.orig', '.both', '.endlive', '.recount'},
}
MAGIC = [(0, b'\x7fELF', 'ELF binary'), (0x8001, b'CD001', 'ISO 9660 image'), (0, b'\x89PNG', 'PNG image'),
         (0, b'\xff\xd8\xff', 'JPEG image'), (0, b'GIF8', 'GIF image'), (0, b'TIM2', 'PS2 TIM2 image'),
         (0, b'RIFF', 'RIFF audio/video'), (0, b'VAGp', 'PS2 VAG audio'), (0, b'\x00\x00\x01\xba', 'MPEG/PSS video'),
         (0, b'PK\x03\x04', 'zip archive'), (0, b'\x1f\x8b', 'gzip archive'), (0, b'\xfd7zXZ', 'xz archive'),
         (0, b'7z\xbc\xaf', '7z archive'), (0, b'OggS', 'Ogg audio'), (0, b'ID3', 'MP3 audio')]
NAMED_ART = re.compile(r'(logo|banner|boxart|box-art|splash|artwork|cover-?art|key-?art)', re.I)
LISTING = re.compile(r'/\*\s*[0-9A-Fa-f]{4,8}\s+[0-9A-Fa-f]{8}\s+[0-9A-Fa-f]{8}\s*\*/'
                     r'|^\s*[0-9a-fA-F]{6,8}:\s+[0-9a-fA-F]{8}\s+\w'
                     r'|^\s*glabel\s', re.M)
ASM_MACRO = re.compile(r'\bINCLUDE_(ASM|RODATA)\s*\(|^\s*\.incbin\b', re.M)
BLOB = re.compile(r'[A-Za-z0-9+/=]{2000,}|(?:\\x[0-9a-fA-F]{2}){1000,}')
INIT = re.compile(r'=\s*\{')
NUM = re.compile(r'\b(?:0x[0-9a-fA-F]+|\d+)\b')
MAX_LITERALS = 1024
# a generator table row (scripts/klib.py): syscall number, name, optional disc
TABLE_ROW = re.compile(r'^\s*(?:#.*|-?\d+\s+[A-Za-z_]\w*(?:\s+(?:jp|kr))?\s*)?$')


def list_files(root):
    try:
        r = subprocess.run(['git', 'ls-files', '-z', '--cached', '--others', '--exclude-standard', '.'],
                           cwd=root, capture_output=True, check=True)
        files = sorted({p for p in r.stdout.decode().split('\0') if p and (root / p).is_file()})
        return files, 'git (tracked and untracked, not ignored)'
    except (subprocess.CalledProcessError, FileNotFoundError):
        return sorted(str(p.relative_to(root)).replace('\\', '/') for p in root.rglob('*') if p.is_file()), 'directory walk'


def largest_initializer(text):
    best = 0
    for m in INIT.finditer(text):
        i, depth = m.end(), 1
        while depth and i < len(text):
            depth += {'{': 1, '}': -1}.get(text[i], 0)
            i += 1
        best = max(best, len(NUM.findall(text[m.end():i])))
    return best


def audit_file(path, rel):
    found = []
    ext = Path(rel).suffix.lower()
    for kind, exts in ASSET_EXT.items():
        if ext in exts:
            found.append((kind, f'extension {ext}'))
    if NAMED_ART.search(Path(rel).name):
        found.append(('logo or artwork', 'file name'))
    data = path.read_bytes()
    for off, magic, what in MAGIC:
        if data[off:off + len(magic)] == magic:
            found.append(('binary', what))
    if b'\0' in data[:65536]:
        found.append(('binary', 'NUL bytes'))
    if any(k == 'binary' for k, _ in found):
        return found
    text = data.decode('utf-8', errors='replace')
    n = len(LISTING.findall(text))
    if n >= 10:
        found.append(('assembly', f'{n} disassembly-listing lines'))
    if ASM_MACRO.search(text) and ext in ('.c', '.h', '.s', '.inc'):
        found.append(('assembly', 'INCLUDE_ASM/INCLUDE_RODATA/.incbin'))
    if BLOB.search(text):
        found.append(('disc data', 'long base64/hex run'))
    if ext in ('.c', '.h'):
        n = largest_initializer(text)
        if n > MAX_LITERALS:
            found.append(('disc data', f'initializer with {n} literals'))
    if ext == '.tbl':
        bad = [i for i, line in enumerate(text.splitlines(), 1) if not TABLE_ROW.match(line)]
        if bad:
            found.append(('assembly', f'generator table with {len(bad)} lines that are not "number name" rows '
                                      f'(first: line {bad[0]})'))
    return found


def in_publish(rel):
    return rel not in PUBLISH_EXCLUDE and any(rel == p or (p.endswith('/') and rel.startswith(p)) for p in PUBLISH)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=str(HERE))
    ap.add_argument('--publish', action='store_true', help='limit the decomp tree to the PUBLISH set')
    ap.add_argument('--extra', action='append', default=[], metavar='DIR=PREFIX',
                    help='also audit DIR, reported under PREFIX (e.g. ../ggx-port/publish=.)')
    ap.add_argument('--out', help='Markdown report')
    a = ap.parse_args()

    trees = [(Path(a.root).resolve(), '', a.publish)]
    for e in a.extra:
        d, _, prefix = e.partition('=')
        trees.append((Path(d).resolve(), prefix.strip('./') and prefix.rstrip('/') + '/', False))

    findings, counts, methods = defaultdict(list), defaultdict(int), []
    total = 0
    for root, prefix, publish in trees:
        files, method = list_files(root)
        methods.append(f'`{root.name}` ({method}{", PUBLISH set" if publish else ""})')
        for rel in files:
            if publish and not in_publish(rel):
                continue
            total += 1
            counts[Path(rel).suffix.lower() or '(none)'] += 1
            for kind, why in audit_file(root / rel, rel):
                findings[kind].append((prefix + rel, why))

    lines = ['# Repo audit', '',
             f'Generated by `scripts/audit_repo.py`. Scanned {total:,} files: ' + '; '.join(methods) + '.', '',
             '| Check | Findings |', '| --- | ---: |']
    kinds = ['binary', 'image', 'audio', 'video', 'font', 'disc image', 'executable or object', 'archive',
             'logo or artwork', 'assembly', 'disc data', 'compiler dump']
    for k in kinds:
        lines.append(f'| {k} | {len(findings[k])} |')
    lines += ['', '**Result: ' + ('clean.' if not findings else f'{sum(map(len, findings.values()))} findings.') + '**', '']
    for k in kinds:
        if findings[k]:
            lines += [f'## {k}', '']
            for rel, why in findings[k][:200]:
                lines.append(f'- `{rel}`: {why}')
            if len(findings[k]) > 200:
                lines.append(f'- ... and {len(findings[k]) - 200} more')
            lines.append('')
    lines += ['## File types scanned', '', ', '.join(f'{e} {n:,}' for e, n in sorted(counts.items(), key=lambda x: -x[1])), '']
    report = '\n'.join(lines)
    if a.out:
        Path(a.out).write_text(report, encoding='utf-8', newline='\n')
    print(report)
    return 1 if findings else 0


if __name__ == '__main__':
    sys.exit(main())
