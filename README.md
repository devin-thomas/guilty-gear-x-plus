# Guilty Gear X Plus

A matching decompilation of *Guilty Gear X Plus* for the PlayStation 2. The C in `src/` compiles back to the original game, byte for byte.

## Status

**3,615 / 3,615 game functions** are C on both supported discs, and both rebuild byte-identical:

| Disc | Executable | SHA-256 |
| --- | --- | --- |
| SLPS-20137 (Japan) | `SLPS_201.37` | `8bd352acb95a533abd22f11c7f9da4a3157dc8836fc7d91ae8340bde92d8a411` |
| SLPM-64525 (Korea) | `SLPM_645.25` | `ef3278dcf50feee1da1ac423042db6a3eeeaf845a3644a1d625bb97d0024755f` |

One function, `TensionDraw`, is a tagged fake match; see [FAKEMATCHES.md](FAKEMATCHES.md). The Sony SDK and C-runtime layer in `src/sdk/` is partly C and partly assembly.

## Building

You need Linux (or WSL) on x86-64 with the 32-bit C library (Debian/Ubuntu: `libc6-i386`), Python 3.12+, and the executable from your own disc. No game data is included; see [LEGAL.md](LEGAL.md).

```sh
python3 -m venv .venv && .venv/bin/pip install -r requirements.txt
.venv/bin/python scripts/fetch_toolchain.py --dest tools  # EE-GCC and binutils; never committed
export GGX_TOOLS=$PWD/tools GGX_QEMU=                     # the i386 compiler runs directly
.venv/bin/python build.py --target jp --elf /path/to/SLPS_201.37 --work /tmp/ggx-jp \
    --binutils $PWD/tools/binutils --runner env --map /=/
.venv/bin/python build.py --target kr --elf /path/to/SLPM_645.25 --work /tmp/ggx-kr \
    --binutils $PWD/tools/binutils --runner env --map /=/
```

On a host without i386 support, set `GGX_QEMU` to a qemu-i386 command instead. Copy the executable off your disc first (it's the `SLPS_201.37` or `SLPM_645.25` file at the root of the disc image). The build checks its hash and fails on any difference from the original. The work directory must be outside a git checkout.

## Licence

The decompilation is dedicated to the public domain under [CC0 1.0](LICENSE). Some C-runtime files in `src/sdk/` keep their upstream terms; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). This is an unofficial fan project, not affiliated with Arc System Works or Sammy.

## Credits

- [WistfulHopes/ggx](https://github.com/WistfulHopes/ggx) was the first decompilation of Guilty Gear X Plus and this project's starting point. None of its code is included here, but it showed the way. Thank you, WistfulHopes.
- [decomp.me](https://decomp.me) and decompals, for the compiler archive and the toolchain.
- [nathanialf/ico](https://github.com/nathanialf/ico), for SDK and C-runtime reconstructions (MIT).
- [chaoticgd/ccc](https://github.com/chaoticgd/ccc), for the STABS reader that read the Korean disc's debug data.
