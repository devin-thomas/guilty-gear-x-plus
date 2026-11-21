# platforms/: the PC port

The native port of *Guilty Gear X Plus* to Windows and macOS, built from the
same game C as the matching decompilation in `src/`. It runs the PlayStation
2 game's own code, field for field, with the PS2 around it replaced: the
SDK by shims, the GS by an SDL_GPU renderer, the sound chip by a model of
it, the memory card by a native save store. It ships no game data: the
player's own disc is read once, on the first run.

Player documentation is in [`docs/`](../docs/README.md). This file is for
people building or changing the port.

Comments in the code name the port's design records: `PORT-nnn` for a piece
of work, `ADR-nnn` for a decision, and design notes such as `SPEC.md`,
`LOCKSTEP.md`, `STATE-HASH.md`, `HASH.md`, `AUDIO.md`, `ROLLBACK.md`,
`REPLAYS.md`, `DRAW-INVENTORY.md` and the translation's `STYLE.md`. Those records are not part of this repository; the
comments say what each piece does on their own.

## What it is exact to

The game's logic runs exactly as it runs in PCSX2's interpreter: after every
field, the port's EE RAM and scratchpad hash the same as a trace recorded
from the real game in PCSX2, for both releases, through every mode. The
floating point follows PCSX2's EE interpreter (exact result, chopped), not
the console's own multiplier and divider, which differ from it in the last
bit of some results. No game decision is known to depend on that bit.

## How the game C becomes a native program

The game is a 32-bit program, and its data assumes it. Files loaded from the
disc hold offsets that `LinkAddrTb` patches, in place, into 32-bit pointers;
tables in the ELF's data hold PS2 pointers and function addresses; structs
read straight out of loaded files have 4-byte pointer fields. Compiled as
ordinary 64-bit C, none of that works, and Apple Silicon has no 32-bit mode
to fall back on. So the Core keeps the EE's memory model exactly:

1. `tools/port_tu.py` gives each game translation unit the view the matching
   build gives each function (all of the file's declarations first).
2. clang compiles it for **wasm32**: 32-bit pointers, the EE's struct
   layout, little-endian. `long` is redefined as `long long`, since EE-GCC's
   `long` is 64 bits. The front end runs alone, to LLVM IR;
   `tools/fpu_lower.py` routes every floating-point operation through the
   EE's rules (below); then the optimiser and the wasm back end make the
   object.
3. wasm-ld links the Core module with relocations kept.
4. `tools/wasm_place.py` moves every data symbol to its **PS2 address** and
   makes every function's "pointer" its PS2 address, using the symbol map in
   `common/core/symbols/`. The function table is indexed by PS2 address.
5. wasm2c (wabt, pinned and built from source) turns the module into C;
   `tools/w2c_patch.py` lets through the indirect calls whose C type
   disagrees with the function in ways the EE ignores.
6. The host compiles that C natively, with the shims and the host loop.

At run time the host loads the player's boot ELF into the Core's memory at
its own addresses (it is identified by SHA-256 and refused if it is the
wrong release). Every EE address then means the same thing it means on the
PS2, and any PS2 pointer in the image or in a loaded file just works.

| Core address | What |
| --- | --- |
| `0x00000000-0x02000000` | EE RAM: the boot ELF at `0x100000`, then bss, the newlib heap and the PS2 stack area |
| `0x10000000-0x12002000` | EE and GS registers: plain memory, so the game's direct register writes land harmlessly |
| `0x70000000-0x70004000` | the scratchpad |
| `0x70010000-` | the Core module's own data and stack |

### Determinism

Every host computes the same thing.

- Game C is built with `-fwrapv -fno-strict-aliasing -fno-delete-null-pointer-checks`,
  so the optimiser exploits no undefined behaviour the EE didn't have, and
  `-ftrivial-auto-var-init=zero`, so an uninitialised local reads 0 on
  every host.
- The compiler is pinned: every host builds the Core with wasi-sdk 34.0's
  clang and `wasm-ld`, so every host builds the same `core.wasm`, byte for
  byte. `core-wasm.sha256` records its hash for each release, and a build
  that differs is not the released Core.
- The Core's C and wasm2c's output are compiled with `-ffp-contract=off`.
- The C library parts with one right answer (string functions, integer
  `sprintf`) are in `common/core/libc.c`; the ones whose state or rounding
  matter are the decompilation's own matched C: newlib's `malloc`, `rand`
  and `errno`, and fdlibm.

### Floating point

The EE's floating point is not IEEE 754. Its FPU is single precision only
and has rules of its own; double precision is software. The Core follows
both with integer code, so every host computes the same bits.

- **Single precision** goes through the EE-FPU Model,
  `common/fpu/ee_fpu.c`, which computes what PCSX2's EE interpreter
  computes: operands with a zero exponent read as signed zero, exponent 255
  reads as the largest finite value, results round toward zero and never
  overflow to infinity, x / 0 is the largest finite value. `ee_fpu.h` lists
  the rules. `tests/fpu/ee_fpu_vectors.txt` holds 56,257 results computed
  by PCSX2's own interpreter code, and the model gives every one.
- **Double precision** goes through the EE's own software floating point:
  the decompilation's matched `src/sdk/gcc/dp_bit.c`, `fp_bit.c` and
  `_floatdisf.c`, compiled into the Core and called through
  `common/fpu/ee_softfp.c`.
- **Routing:** `tools/fpu_lower.py` rewrites the front end's IR so float
  arithmetic, compares and conversions become calls to the model, and double
  operations become the fp-bit call EE-GCC emits. `tools/fpu_check.py`
  decodes the linked Core and fails the build if any function does
  floating-point arithmetic natively.

## Building

Needs CMake 3.20 or later, Ninja, Python 3 and a host C compiler. The first
configure downloads the Core's compiler, wasi-sdk 34.0, for the host (about
190 MB, 620 MB on Windows), checks it against its pinned SHA-256 and
unpacks it into `GGX_TOOLS_DIR` (an environment variable or CMake option;
set it to share one copy between build directories, default
`<build>/tools`). The first build also downloads wabt 1.0.41 and builds its
`wasm2c` (needs a C++17 compiler).

    cmake -S platforms -B build-KR -G Ninja -DGGX_VERSION=KR -DCMAKE_BUILD_TYPE=Release
    cmake --build build-KR

`GGX_VERSION` is `KR` (SLPM-645.25, the default) or `JP` (SLPS-201.37);
each build directory builds one. `ggx`, the windowed program, is built when
CMake finds SDL3 3.2 or later (`-DGGX_SDL=OFF` skips it). The shaders are
precompiled in `render/shaders/dl_shaders.h`, so no shader compiler is
needed. `ctest --test-dir build-KR` runs the tests that need no disc.

- **Windows:** MSYS2 UCRT64 (`mingw-w64-ucrt-x86_64-{clang,lld,cmake,ninja,python,sdl3}`),
  with `-DCMAKE_C_COMPILER=clang`. The build copies `SDL3.dll` beside
  `ggx.exe` with every DLL it needs from MSYS2, so `ggx.exe` starts from a
  shortcut; `tools/check_dlls.sh <build dir>` checks that. Keep the build
  path short: CMake warns past about 200 characters.
- **macOS (Apple Silicon):** Homebrew's `cmake` and `ninja`; Xcode's clang
  is the host compiler. Pass SDL3's prefix in `CMAKE_PREFIX_PATH`. The
  release builds set `-DCMAKE_OSX_DEPLOYMENT_TARGET=14.0` and use an SDL3
  built from its source release with the same deployment target.
- **Linux:** the code builds with CMake, Ninja, Python 3, a C compiler and
  SDL3 from the distribution, but Linux is not a supported release target
  and is not tested for releases.

### Release packages

    tools/package_windows.sh build-KR build-JP <out dir> <repository root>
    tools/package_macos.sh build-KR build-JP <out dir> <repository root>

Each takes the two release builds and writes one package holding both
programs, `ggx-kr` and `ggx-jp`: a `.zip` of a folder on Windows (with
`ggx-save.exe`, `ggx-replays.exe`, the DLLs, `text\` and the licences), and
on macOS a `GGX Plus.app` (both programs, the tools, SDL3, `text/`, the
licences; ad-hoc signed) in a `.dmg`. Either program detects the disc it is
given and hands over to the other when it is the other release's.

## Running

`ggx` opens a window and, on the first run, asks for the disc image, checks
it and copies its files into the user data directory (`%APPDATA%\ggx-plus\`
on Windows, `~/Library/Application Support/ggx-plus/` on macOS). Later
launches start straight into the game. `--data-dir <dir>` uses another
directory, `--first-run <image>` extracts that image without the file
dialog, and `--gpu direct3d12`, `--gpu vulkan` or `--gpu metal` picks the
GPU backend.

`ggx-headless` runs the Core with no window, for tests:

    build-KR/ggx-headless --extract "<image>.cue"
    build-KR/ggx-headless --until TitleMain --log title.txt

The log has one line per shim call, every job change, and a hash of EE RAM
and the scratchpad at the end. Both builds run 20,000 fields of the attract
loop without a trap.

`ggx-save` converts saves to and from PCSX2 memory cards, `ggx-replays`
lists and describes replays, and `ggx-replay` replays a Lockstep Trace.

## Layout

| Path | What |
| --- | --- |
| `CMakeLists.txt`, `cmake/` | the build: the Core module, wasm2c, libchdr, the host, the tests |
| `common/core/` | port code compiled into the Core, the prelude, the symbol maps |
| `common/assets/` | disc images, identification, extraction and the extracted asset store, the release table, the user data directory |
| `common/fpu/` | the EE-FPU Model and the EE's software floating point |
| `common/host/` | the Core host, its state and State Hash, sessions, rollback and replays, `ggx-headless`, `ggx-replays` |
| `common/shims/` | the PS2 SDK shims |
| `common/trace/` | Lockstep Traces: the reader and replay, the SDK call ids, the symbol maps and masks |
| `common/saves/` | saves: the native format and the export check, PS2 memory card images, `ggx-save` |
| `common/input/` | the pads as libpad delivers them, GGScript, macros, the confirm button |
| `render/` | the renderer: the GIF stream and the GS Path, GS local memory, the texture cache, SDL_GPU and the shaders |
| `audio/` | the sound driver: the command stream, SNDRV.IRX, the SPU2 model, `ggx-snd` |
| `sdl/` | `ggx`, the SDL3 program: the window, first run, input, settings, display pacing, speakers |
| `prompts/` | the button prompts: letters from the game's HUD font in place of its PS2 symbols |
| `text/` | the English text layer, `ggx-text`, the translation files (`en/`) and the fonts |
| `tests/` | the tests `ctest` runs: no disc needed |
| `third_party/` | libchdr, stb_truetype and xxHash, vendored with their licences |
| `tools/` | the build's Python tools, the DLL check and the packaging scripts |

## Licence

The code in `platforms/` is under the [MIT licence](LICENSE), copyright Lil
Gohan and contributors. The vendored libraries in `third_party/` and the
fonts in `text/fonts/` keep their own licences; see
[THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md).
