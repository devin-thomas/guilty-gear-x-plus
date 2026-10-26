# Third-party notices

This project's own code is under [CC0 1.0](LICENSE). The port in `platforms/`, when it lands, will be under the MIT licence. This file lists what comes from elsewhere: libraries the port uses, code in `src/sdk/` that keeps its upstream terms, tools the build fetches, and credits.

## Libraries the port will use (`platforms/`, not here yet)

These will be dependencies fetched by the port's build, not copied into this repository.

| Project | Licence | Use |
| --- | --- | --- |
| [SDL3](https://github.com/libsdl-org/SDL) | zlib | windowing, input, audio, storage, timing; SDL_GPU for rendering |
| [SDL_shadercross](https://github.com/libsdl-org/SDL_shadercross) | zlib | shader cross-compilation (SPIR-V, MSL, DXIL) |
| [GGPO](https://github.com/pond3r/ggpo) | MIT | planned for rollback netcode; not used yet |

Binary releases of the port will include each library's licence text next to the executable.

## Code vendored in `platforms/third_party/`

The port reads CHD disc images with [libchdr](https://github.com/rtissera/libchdr), copied unchanged into `platforms/third_party/libchdr/` from commit `47d79415` with the decoders it bundles. Each keeps its licence text beside it; `platforms/third_party/libchdr/VENDORED.md` lists what was copied.

| Project | Licence | Licence text |
| --- | --- | --- |
| libchdr | BSD 3-Clause | `LICENSE.txt` |
| LZMA SDK 26.02 (decoder) | public domain | `deps/lzma-26.02/LICENSE` |
| miniz 3.1.2 | MIT | top of `deps/miniz-3.1.2/miniz.c` |
| Zstandard 1.5.7 (single-file decoder) | BSD (zstd is dual BSD/GPLv2; used under BSD) | `deps/zstd-1.5.7/LICENSE` |
| dr_flac | public domain or MIT-0 | end of `include/dr_libs/dr_flac.h` |

The State Hash (`games/ggx-port/STATE-HASH.md`) uses [xxHash](https://github.com/Cyan4973/xxHash) 0.8.3's `xxhash.h`, copied unchanged into `platforms/third_party/xxhash/`.

| Project | Licence | Licence text |
| --- | --- | --- |
| xxHash 0.8.3 (`xxhash.h`) | BSD 2-Clause | `platforms/third_party/xxhash/LICENSE` |

## Code in `src/sdk/` under its own terms

`src/sdk/` rebuilds the Sony SDK libraries and the C runtime that the discs link. Most of it was written from the discs for this project and is CC0. The files below say where they come from in their first comment, and they keep their upstream terms:

### nathanialf/ico (MIT)

Files whose header reads "Adapted from nathanialf/ico" (74 files in `src/sdk/sce`, `src/sdk/gcc` and `src/sdk/newlib`, from commit `e0881204`) are adapted from the [ICO decompilation](https://github.com/nathanialf/ico), under this licence:

```
MIT License

Copyright (c) 2026 the ico decompilation contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### newlib 1.8.2 and fdlibm

Files whose header names "newlib 1.8.2" reproduce [newlib](https://sourceware.org/newlib/) 1.8.2, the C library the EE-GCC 2.9 toolchain linked. The `src/sdk/libm/` files (and their `*_k.h` constant headers) come from its fdlibm-based libm, under Sun Microsystems' notice, which each file keeps:

```
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.

Developed at SunPro, a Sun Microsystems, Inc. business.
Permission to use, copy, modify, and distribute this
software is freely granted, provided that this notice
is preserved.
```

Other newlib files keep the notice of their own upstream file.

### GCC's libgcc

`src/sdk/gcc/` holds the libgcc members the discs link: `libgcc2.c` routines (`_divdi3`, `_muldi3`, `__main`, ...) and the `fp-bit.c`/`dp-bit.c` soft-float code of GCC 2.9-ee. Upstream, these are GCC source files under the GNU GPL version 2 or later, with the libgcc exception that lets their compiled code be linked into any program without restriction. Those terms still apply to these files.

## Tools fetched at build time

Nothing below is stored in this repository.

| Tool | Licence | Source |
| --- | --- | --- |
| EE-GCC 2.9-ee-991111, 2.9-ee-991111-01 and 2.96 (cc1, cpp, GAS) | GNU GPL | [decompme/compilers](https://github.com/decompme/compilers), via `scripts/fetch_toolchain.py` |
| binutils for mips-ps2 | GNU GPL | [decompals/binutils-mips-ps2-decompals](https://github.com/decompals/binutils-mips-ps2-decompals) v0.10 |
| splat64, spimdisasm, rabbitizer | MIT | PyPI ([ethteck/splat](https://github.com/ethteck/splat), [Decompollaborate](https://github.com/Decompollaborate)) |
| pyelftools | Public domain | PyPI |
| qemu-user (to run the i386 compiler on other hosts) | GNU GPL | the host's packages |

## Credits

- **[WistfulHopes/ggx](https://github.com/WistfulHopes/ggx)**, the first decompilation project for *Guilty Gear X Plus*. It was this project's starting point: its splat setup for the Korean disc, its translation-unit layout and its first matched functions. That repository has no licence, so nothing derived from its code is included here; a line-by-line comparison checked that before publishing. Thank you to WistfulHopes all the same.
- **[decomp.me](https://decomp.me)** and **decompals**, for the compiler archive and the toolchain this work depends on.
- **[chaoticgd/ccc](https://github.com/chaoticgd/ccc)**, whose `stdump` read the Korean disc's STABS debug data.
- **[nathanialf/ico](https://github.com/nathanialf/ico)**, for the SDK and C-runtime reconstructions credited above.
