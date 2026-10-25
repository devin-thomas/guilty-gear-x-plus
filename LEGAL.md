# Legal

This repository holds a matching decompilation of *Guilty Gear X Plus* for the PlayStation 2. A native port will follow in `platforms/`. The repository contains source code and documentation only.

## What is not here

- No game assets: no graphics, sound, music, text dumps, fonts or video.
- No binaries: no game executables, ELFs, overlays or IOP modules.
- No disc data: no disc images, sectors or files taken from a disc.
- No disassembly: no assembly listings of the game, split or whole. The build generates its own from your disc and keeps it out of git.
- No logos or artwork of Arc System Works or Sammy.
- No compiler: the PS2 toolchain is not vendored (see below).

Pull requests that add any of these are closed.

## Bring your own disc

To build, you need your own legally obtained copy of one of these discs:

| Disc | Release | Main executable | Size (bytes) | SHA-256 | SHA-1 |
| --- | --- | --- | ---: | --- | --- |
| SLPM-64525 | Guilty Gear X Plus, Korea | `SLPM_645.25` | 7,844,099 | `ef3278dcf50feee1da1ac423042db6a3eeeaf845a3644a1d625bb97d0024755f` | `cf019ab02686781ee7736fdc61c11007a841aef2` |
| SLPS-20137 | Guilty Gear X Plus, Japan | `SLPS_201.37` | 5,035,748 | `8bd352acb95a533abd22f11c7f9da4a3157dc8836fc7d91ae8340bde92d8a411` | `c195c98465b3c0828607c934fe9cfb9fdfa5f2cf` |

The build reads the executable from your disc and refuses any file with a different hash. The port reads its assets from your disc image the same way, on your machine.

We don't provide disc images or executables, and we don't help anyone find them. Requests for them, and links to them, are removed. Don't pirate the game.

## Toolchain

The matching build needs the PS2 compiler EE-GCC 2.9-ee-991111. It is never stored in this repository. `scripts/fetch_toolchain.py` downloads it at build time from [decompme/compilers](https://github.com/decompme/compilers) and checks its hash.

## Not affiliated

This is an unofficial fan project. It is not affiliated with, endorsed by or sponsored by Arc System Works, Sammy, Sony Interactive Entertainment or any of their affiliates. *Guilty Gear* and *Guilty Gear X* are trademarks of Arc System Works. *PlayStation* is a trademark of Sony Interactive Entertainment. Every trademark belongs to its owner and is used only to identify the game this code works with.

## Licences

- The decompilation (game C, headers, build scripts, configs and documentation) is dedicated to the public domain under [CC0 1.0](LICENSE).
- `platforms/` (the port, when it lands) will be under the MIT licence.
- Files that carry their own notice keep it. This applies to some C-runtime code in `src/sdk/`; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

The licences cover this project's own code. They grant no rights in the game, its code as shipped on the disc, or its assets.

## Lineage

The project started from the research in [WistfulHopes/ggx](https://github.com/WistfulHopes/ggx), which has no licence. Nothing derived from its code remains here: every function, header and config was rewritten or regenerated from our own work and from the Korean disc's own debug data, and a line-by-line comparison checked it before publishing. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for credits.

This file is not legal advice.
