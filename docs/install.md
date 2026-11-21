# Installing

The port is a program and the files it needs to draw English text (two
fonts and the translation). It contains nothing from the game. Install it,
then start it: the first run asks for your disc image ([Your disc](disc.md)).

Download the package for your system from the repository's
[Releases](https://github.com/devin-thomas/guilty-gear-x-plus/releases)
page. Each release lists the SHA-256 of every file in `SHA256SUMS.txt`.

## Which program to start

Each package holds two programs, `ggx-kr` and `ggx-jp`, one for each
release of the disc. Start `ggx-kr` (on a Mac, the app starts it for you).
Given a Japanese disc, it extracts it and hands over to `ggx-jp`, and later
launches go straight to it. Starting `ggx-jp` works the same way the other
way round.

## Windows (10 and 11, 64-bit)

1. Download `ggx-plus-1.0.0-windows-x64.zip` and unzip it anywhere you
   like, for example into `Documents` or `C:\Games`. It holds one folder,
   `ggx-plus-1.0.0`.
2. Open the folder and start `ggx-kr.exe`. To start it from the desktop,
   right-click it and choose **Show more options > Send to > Desktop
   (create shortcut)**.
3. The program is not signed, so Windows SmartScreen says "Windows
   protected your PC" on the first launch: choose **More info**, then
   **Run anyway**. Windows asks once.

- The folder holds `SDL3.dll` and the runtime DLLs the programs need;
  nothing else needs installing.
- Graphics: Direct3D 12 or Vulkan, whichever SDL chooses on your PC;
  `--gpu direct3d12` or `--gpu vulkan` picks one.
- `ggx-save.exe` and `ggx-replays.exe`, in the same folder, are the save
  and replay tools ([Saves](saves.md), [Replays](replays.md)).

## macOS (Apple Silicon, macOS 14 or later)

1. Download `ggx-plus-1.0.0-macos-arm64.dmg` and open it.
2. Drag **GGX Plus** to **Applications**.
3. Open GGX Plus. The app is not notarized, so the first time macOS says it
   can't check it for malicious software. Open **System Settings > Privacy
   & Security**, scroll to Security, choose **Open Anyway** next to the
   message about GGX Plus, and confirm. This is needed once.

- The app uses Metal. Intel Macs aren't supported.
- Both programs and the tools are inside the app, in
  `GGX Plus.app/Contents/MacOS/`: `ggx-kr`, `ggx-jp`, `ggx-save` and
  `ggx-replays`. Run them from Terminal with their full path, for example
  `"/Applications/GGX Plus.app/Contents/MacOS/ggx-save" list`.

## Linux

There is no Linux package. The source in `platforms/` builds on Linux
([platforms/README.md](../platforms/README.md)), but Linux builds are not
supported or tested.

## Where the port keeps your data

One folder holds the files extracted from your disc, your saves, replays and
settings:

| System | Folder |
| --- | --- |
| Windows | `%APPDATA%\ggx-plus\` (for example `C:\Users\you\AppData\Roaming\ggx-plus\`) |
| macOS | `~/Library/Application Support/ggx-plus/` |

Inside it:

| | |
| --- | --- |
| `assets/kr/` or `assets/jp/` | the disc's files, about 515 MB per release |
| `saves/` | your saves ([Saves](saves.md)) |
| `replays/` | your replays ([Replays](replays.md)) |
| `ggx.cfg` | your settings ([Settings](settings.md)) |

## Uninstalling

- **Windows:** delete the `ggx-plus-1.0.0` folder (and any shortcut you
  made). Nothing was installed anywhere else.
- **macOS:** move GGX Plus from Applications to the Trash.

Your data folder stays, so a reinstall finds your saves and needn't extract
the disc again. To remove everything, delete that folder too; copy
`saves/` and `replays/` somewhere first if you want to keep them.
