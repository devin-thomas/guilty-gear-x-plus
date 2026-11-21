#!/usr/bin/env bash
# package_windows.sh <KR build> <JP build> <out dir> <repository root> [version]
#
# The Windows release package: <out dir>/ggx-plus-<version>-windows-x64.zip, holding one folder,
# ggx-plus-<version>/, with both programs (ggx-kr.exe and ggx-jp.exe, each the ggx.exe of its release build),
# ggx-save.exe and ggx-replays.exe, every DLL they need (as cmake/BundleDlls.cmake put them beside ggx.exe),
# text\ (the English translation and its fonts) and the licences. No installer: unzip and start ggx-kr.exe;
# delete the folder to uninstall. For MSYS2's bash (UCRT64); needs objdump, coreutils and Python 3.
set -euo pipefail
kr=${1:?usage: package_windows.sh <KR build> <JP build> <out dir> <repository root> [version]}
jp=${2:?KR build, JP build, out dir and repository root}
out=${3:?out dir}
root=${4:?repository root (the folder holding LICENSE, THIRD_PARTY_NOTICES.md and platforms/)}
ver=${5:-1.0.0}
here=$(cd "$(dirname "$0")" && pwd)
name=ggx-plus-$ver
zip=$out/$name-windows-x64.zip
stage=$out/$name
for f in "$kr/ggx.exe" "$jp/ggx.exe" "$kr/ggx-save.exe" "$kr/ggx-replays.exe" "$kr/text/en" "$kr/text/fonts" \
         "$root/LICENSE" "$root/THIRD_PARTY_NOTICES.md" "$root/platforms/LICENSE"; do
    [ -e "$f" ] || { echo "package_windows: $f is missing" >&2; exit 1; }
done
rm -rf "$stage" "$zip"
mkdir -p "$stage/licenses"
cp "$kr/ggx.exe" "$stage/ggx-kr.exe"
cp "$jp/ggx.exe" "$stage/ggx-jp.exe"
cp "$kr/ggx-save.exe" "$kr/ggx-replays.exe" "$stage/"
shopt -s nullglob nocaseglob
for d in "$kr"/*.dll; do
    cp "$d" "$stage/"
done
for d in "$jp"/*.dll; do
    [ "$(sha256sum < "$d")" = "$(sha256sum < "$stage/$(basename "$d")")" ] || { echo "package_windows: $(basename "$d") differs between the builds" >&2; exit 1; }
done
shopt -u nocaseglob
cp -R "$kr/text" "$stage/text"
# the programs and every DLL beside them need nothing from MSYS2
bash "$here/check_dlls.sh" "$stage"
# licences: the port (MIT), the decompilation (CC0), the notices, and each bundled DLL's own
cp "$root/platforms/LICENSE" "$stage/LICENSE.txt"
cp "$root/LICENSE" "$stage/licenses/CC0-1.0.txt"
cp "$root/THIRD_PARTY_NOTICES.md" "$stage/THIRD_PARTY_NOTICES.md"
bash "$here/bundle_licenses.sh" "$here/.." "$kr" "$stage/licenses"
lic=/ucrt64/share/licenses
for d in "$stage"/*.dll; do
    case $(basename "$d") in
    SDL3.dll) pkg=sdl3 ;;
    libiconv-2.dll) pkg=libiconv ;;
    libwinpthread-1.dll) pkg=libwinpthread ;;
    *) echo "package_windows: no licence known for $(basename "$d"); add it here" >&2; exit 1 ;;
    esac
    [ -d "$lic/$pkg" ] || { echo "package_windows: no $lic/$pkg for $(basename "$d")" >&2; exit 1; }
    mkdir -p "$stage/licenses/$pkg"
    cp -R "$lic/$pkg/." "$stage/licenses/$pkg/"
done
cat > "$stage/README.txt" <<EOT
Guilty Gear X Plus PC port $ver (Windows 10 and 11, 64-bit)

Start ggx-kr.exe. On the first run it asks for an image of your own disc
(ISO, BIN/CUE or CHD of SLPM-645.25 Korea or SLPS-201.37 Japan), checks it
and copies its files. A Japanese disc is handed over to ggx-jp.exe.

This build is not signed. If Windows SmartScreen says "Windows protected
your PC", choose More info, then Run anyway.

Your saves, replays and settings are in %APPDATA%\\ggx-plus\\. To uninstall,
delete this folder.

Documentation: https://github.com/devin-thomas/guilty-gear-x-plus/tree/main/docs
EOT
sed -i 's/$/\r/' "$stage/README.txt"
python3 - "$stage" "$zip" "$name" <<'EOT'
import os, sys, zipfile
stage, zpath, name = sys.argv[1:]
with zipfile.ZipFile(zpath, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as z:
    for dirpath, dirs, files in os.walk(stage):
        dirs.sort()
        for f in sorted(files):
            p = os.path.join(dirpath, f)
            arc = name + '/' + os.path.relpath(p, stage).replace(os.sep, '/')
            z.write(p, arc)
EOT
echo "package_windows: $zip"
(cd "$stage" && find . -type f | sort)
