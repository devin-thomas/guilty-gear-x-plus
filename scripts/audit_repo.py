#!/usr/bin/env python3
"""Audit a tree for things the public repo must never contain.

Checks every file (git-tracked and untracked-but-not-ignored, or every file of
a plain directory) for:

- binaries: NUL bytes, or the magic of an ELF, ISO 9660 image, archive, image,
  audio or video file;
- assets, disc images and build products, by extension (images, audio, video,
  fonts, archives, .iso/.bin/.cue/.chd, .elf/.irx/.o/.a, ...);
- logos and artwork: any image or icon file, and files named logo/banner/boxart;
  except screenshots (below);
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

Screenshots are allowed: a captured frame of the running game or port is not game
data. A PNG, JPEG or WebP file in a folder named `screenshots` (for example
`docs/screenshots/`), whose content matches its extension and which is at most
2 MB, is listed in the report as an allowed screenshot and is not a finding. Any
other image (outside such a folder, over 2 MB, another format, or named like
artwork) is a finding for review, listed by file. Nothing is skipped silently.

A few files of the port in platforms/ would trip these checks and are not game data:
two licensed fonts, the port's own compiled shaders and libchdr's CD-ROM error
correction tables. KNOWN lists them by path, each with its reason and the SHA-256 of
the file it was checked as; the report lists each one, and a file at that path with
other content is a finding as usual.

Writes a Markdown report. Exit status 1 if any finding is in scope.

  audit_repo.py [--root DIR] [--publish] [--extra DIR=PREFIX] [--out AUDIT.md]

In the public repo, run it with no arguments.
"""
import argparse
import re
import sys
from pathlib import Path

from uppercut_ps2_match import AuditConfig, audit_tree

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

# Files of the port that are not game data but trip the checks (see the docstring): path -> (SHA-256, why)
KNOWN = {
    'platforms/text/fonts/DejaVuSerif.ttf':
        ('b028fae357e71503231e2262c4c74a0e900ab8641899af4dc7668a7158791976', 'DejaVu Serif, subset; DejaVu fonts licence (text/fonts/LICENSE-DejaVu.txt)'),
    'platforms/text/fonts/NotoSans-Regular.ttf':
        ('acc7375814843f0f42d59619d7b0a133fcf7fe112f448eeedbd7fb79879464c7', 'Noto Sans, subset; SIL OFL 1.1 (text/fonts/LICENSE-NotoSans-OFL.txt)'),
    'platforms/render/shaders/dl_shaders.h':
        ('9dadf0b6d49246b7de2dd3725ba1e36df5b5bc084c60683590596188bdce81f6', "the port's own shaders, compiled (render/shaders/)"),
    'platforms/third_party/libchdr/src/libchdr_cdrom.c':
        ('7fc4515eb3598d88797b2786a560bd497720eb82f9b8c6391ba4ab7a74f5e351', "libchdr's CD-ROM ECC tables, vendored (BSD 3-Clause)"),
}
MAX_LITERALS = 1024
# a generator table row (scripts/klib.py): syscall number, name, optional disc
TABLE_ROW = re.compile(r'^\s*(?:#.*|-?\d+\s+[A-Za-z_]\w*(?:\s+(?:jp|kr))?\s*)?$')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=str(HERE))
    ap.add_argument('--publish', action='store_true', help='limit the decomp tree to the PUBLISH set')
    ap.add_argument('--extra', action='append', default=[], metavar='DIR=PREFIX',
                    help='also audit DIR, reported under PREFIX (e.g. ../ggx-port/publish=.)')
    ap.add_argument('--out', help='Markdown report')
    a = ap.parse_args()

    cfg = AuditConfig(known=KNOWN, publish=PUBLISH, publish_exclude=PUBLISH_EXCLUDE, max_literals=MAX_LITERALS,
                      table_extension='.tbl', table_row=TABLE_ROW, table_row_name='"number name" rows')
    trees = [(a.root, '')]
    for e in a.extra:
        d, _, prefix = e.partition('=')
        trees.append((d, prefix.strip('./') and prefix.rstrip('/') + '/'))
    report = audit_tree(trees, cfg, publish_first=a.publish)
    text = report.markdown(generated_by='scripts/audit_repo.py', known_heading='Known port files')
    if a.out:
        Path(a.out).write_text(text, encoding='utf-8', newline='\n')
    print(text)
    return report.exit_status


if __name__ == '__main__':
    sys.exit(main())
