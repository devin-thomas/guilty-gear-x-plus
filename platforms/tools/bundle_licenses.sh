#!/usr/bin/env bash
# bundle_licenses.sh <platforms dir> <build dir> <dest dir>
#
# Copies the licence texts of the code compiled into the port's programs into <dest dir>, for the release
# packages (tools/package_windows.sh, tools/package_macos.sh): the libraries vendored in third_party/, wabt's
# wasm2c runtime (from the build's wabt source) and the two fonts. SDL3's and any DLL's are the packaging
# scripts' to add.
set -euo pipefail
plat=${1:?usage: bundle_licenses.sh <platforms dir> <build dir> <dest dir>}
build=${2:?build dir}
dest=${3:?dest dir}
tp=$plat/third_party
mkdir -p "$dest"
copy() {
    [ -e "$1" ] || { echo "bundle_licenses: $1 is missing" >&2; exit 1; }
    cp "$1" "$dest/$2"
}
copy "$tp/libchdr/LICENSE.txt" libchdr-LICENSE.txt
copy "$tp/libchdr/deps/lzma-26.02/LICENSE" lzma-LICENSE.txt
copy "$tp/libchdr/deps/zstd-1.5.7/LICENSE" zstd-LICENSE.txt
copy "$tp/stb/LICENSE" stb-LICENSE.txt
copy "$tp/xxhash/LICENSE" xxhash-LICENSE.txt
# miniz and dr_flac keep their licences in the source file
miniz=$tp/libchdr/deps/miniz-3.1.2/miniz.c
[ -e "$miniz" ] || { echo "bundle_licenses: $miniz is missing" >&2; exit 1; }
awk '{ print } /^ \*{20,}\/$/ { exit }' "$miniz" > "$dest/miniz-LICENSE.txt"
drflac=$tp/libchdr/include/dr_libs/dr_flac.h
[ -e "$drflac" ] || { echo "bundle_licenses: $drflac is missing" >&2; exit 1; }
awk '/^This software is available as a choice of the following licenses/ { f = 1 } f' "$drflac" > "$dest/dr_flac-LICENSE.txt"
[ -s "$dest/dr_flac-LICENSE.txt" ] || tail -n 40 "$drflac" > "$dest/dr_flac-LICENSE.txt"
copy "$build/wabt/source/LICENSE" wabt-LICENSE.txt
copy "$plat/text/fonts/LICENSE-DejaVu.txt" DejaVu-LICENSE.txt
copy "$plat/text/fonts/LICENSE-NotoSans-OFL.txt" NotoSans-OFL.txt
