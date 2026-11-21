# libchdr, vendored

The port reads a player's CHD disc image with
[libchdr](https://github.com/rtissera/libchdr) (BSD 3-Clause, `LICENSE.txt`),
copied here unchanged from commit `47d794151b2b64f6aba60304e6480f4f5a2bc24b`
(2026-10-11). `cmake/Chdr.cmake` builds it.

Copied: `LICENSE.txt`, `include/`, `src/` and the three decoders libchdr
bundles in `deps/`, each with its licence:

| Directory | Library | Licence |
| --- | --- | --- |
| `deps/lzma-26.02/` | LZMA SDK decoder | public domain (`LICENSE`) |
| `deps/miniz-3.1.2/` | miniz (zlib inflate) | MIT (the notice at the top of `miniz.c`) |
| `deps/zstd-1.5.7/` | Zstandard single-file decoder | BSD (`LICENSE`, from zstd v1.5.7; zstd is dual BSD/GPLv2 and is used under BSD) |
| `include/dr_libs/` | dr_flac | public domain or MIT-0 (end of `dr_flac.h`) |

Left out: upstream's build files, tests, `src/link.T`, the C++ micro-flac
backend (`src/libchdr_flac_microflac.cpp`, Apache-2.0, unused) and the LZMA
assembly decoders (`deps/lzma-26.02/Asm/`), so every OS compiles the same C.

To update, copy the same files from a newer upstream commit and change the
commit above and the versions in `cmake/Chdr.cmake` and
`THIRD_PARTY_NOTICES.md`.
