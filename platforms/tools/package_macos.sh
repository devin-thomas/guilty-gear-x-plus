#!/usr/bin/env bash
# package_macos.sh <KR build> <JP build> <out dir> <repository root> [version]
#
# The macOS release package: <out dir>/ggx-plus-<version>-macos-arm64.dmg, a disk image holding "GGX Plus.app"
# and a link to /Applications. The app holds both programs in Contents/MacOS (ggx-kr, the one Finder starts, and
# ggx-jp, each the ggx of its release build), ggx-save and ggx-replays, SDL3 in Contents/Frameworks, and text/
# (the English translation and its fonts) and the licences in Contents/Resources. SDL_GetBasePath is
# Contents/Resources/ in a bundle, so the hand-over to the other release (sdl/firstrun.c) finds ggx-kr and ggx-jp
# there, as links to Contents/MacOS. The app is ad-hoc signed (no identity; Apple Silicon needs a signature to
# run): a player opens it once through System Settings > Privacy & Security > Open Anyway.
#
# Both builds must link the same SDL3 (CMAKE_PREFIX_PATH), built for the deployment target the release claims.
set -euo pipefail
kr=${1:?usage: package_macos.sh <KR build> <JP build> <out dir> <repository root> [version]}
jp=${2:?KR build, JP build, out dir and repository root}
out=${3:?out dir}
root=${4:?repository root (the folder holding LICENSE, THIRD_PARTY_NOTICES.md and platforms/)}
ver=${5:-1.0.0}
min=${GGX_MACOS_MIN:-14.0}
name="GGX Plus"
dmg=$out/ggx-plus-$ver-macos-arm64.dmg
stage=$out/dmg
app=$stage/$name.app
for f in "$kr/ggx" "$jp/ggx" "$kr/ggx-save" "$kr/ggx-replays" "$kr/text/en" "$kr/text/fonts" \
         "$root/LICENSE" "$root/THIRD_PARTY_NOTICES.md" "$root/platforms/LICENSE"; do
    [ -e "$f" ] || { echo "package_macos: $f is missing" >&2; exit 1; }
done
rm -rf "$stage" "$dmg"
mkdir -p "$app/Contents/MacOS" "$app/Contents/Frameworks" "$app/Contents/Resources/licenses"
cp "$kr/ggx" "$app/Contents/MacOS/ggx-kr"
cp "$jp/ggx" "$app/Contents/MacOS/ggx-jp"
cp "$kr/ggx-save" "$kr/ggx-replays" "$app/Contents/MacOS/"
cp -R "$kr/text" "$app/Contents/Resources/text"
for exe in ggx-kr ggx-jp; do
    ln -s "../MacOS/$exe" "$app/Contents/Resources/$exe"
done

# SDL3: the dylib both programs link, copied into the bundle and found through @rpath
sdl=""
for exe in ggx-kr ggx-jp ggx-save ggx-replays; do
    bin=$app/Contents/MacOS/$exe
    dep=$(otool -L "$bin" | awk '/libSDL3/ { print $1 }')
    [ -z "$dep" ] && continue
    case $dep in
    @rpath/*)
        real=""
        for r in $(otool -l "$bin" | awk '/LC_RPATH/ { getline; getline; print $2 }'); do
            [ -e "$r/${dep#@rpath/}" ] && { real=$r/${dep#@rpath/}; break; }
        done ;;
    *) real=$dep ;;
    esac
    [ -n "$real" ] && [ -e "$real" ] || { echo "package_macos: $exe links $dep, which isn't found" >&2; exit 1; }
    real=$(realpath "$real")
    if [ -z "$sdl" ]; then
        sdl=$real
        cp "$sdl" "$app/Contents/Frameworks/libSDL3.0.dylib"
        chmod u+w "$app/Contents/Frameworks/libSDL3.0.dylib"
        install_name_tool -id @rpath/libSDL3.0.dylib "$app/Contents/Frameworks/libSDL3.0.dylib"
    elif ! cmp -s "$sdl" "$real"; then
        echo "package_macos: $exe links another SDL3 ($real, not $sdl)" >&2; exit 1
    fi
    install_name_tool -change "$dep" @rpath/libSDL3.0.dylib "$bin"
    for r in $(otool -l "$bin" | awk '/LC_RPATH/ { getline; getline; print $2 }'); do
        install_name_tool -delete_rpath "$r" "$bin"
    done
    install_name_tool -add_rpath @executable_path/../Frameworks "$bin"
done
[ -n "$sdl" ] || { echo "package_macos: ggx links no SDL3" >&2; exit 1; }

# nothing outside the bundle and the system, and nothing newer than the release claims
for bin in "$app"/Contents/MacOS/* "$app/Contents/Frameworks/libSDL3.0.dylib"; do
    if otool -L "$bin" | tail -n +2 | awk '{ print $1 }' | grep -Ev '^(/usr/lib/|/System/Library/|@rpath/libSDL3\.0\.dylib$)'; then
        echo "package_macos: $(basename "$bin") links a library outside the bundle and the system" >&2; exit 1
    fi
    os=$(otool -l "$bin" | awk '/LC_BUILD_VERSION/ { f = 1 } f && $1 == "minos" { print $2; exit }')
    if [ "$(printf '%s\n%s\n' "$os" "$min" | sort -V | tail -1)" != "$min" ]; then
        echo "package_macos: $(basename "$bin") needs macOS $os, newer than $min" >&2; exit 1
    fi
done

cat > "$app/Contents/Info.plist" <<EOT
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>en</string>
	<key>CFBundleDisplayName</key>
	<string>$name</string>
	<key>CFBundleExecutable</key>
	<string>ggx-kr</string>
	<key>CFBundleIdentifier</key>
	<string>com.lilgohan.fgc.ggx-plus</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>$name</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>$ver</string>
	<key>CFBundleVersion</key>
	<string>$ver</string>
	<key>LSApplicationCategoryType</key>
	<string>public.app-category.games</string>
	<key>LSArchitecturePriority</key>
	<array>
		<string>arm64</string>
	</array>
	<key>LSMinimumSystemVersion</key>
	<string>$min</string>
	<key>NSHighResolutionCapable</key>
	<true/>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright (c) 2026 Lil Gohan and contributors. MIT licence.</string>
</dict>
</plist>
EOT
plutil -lint "$app/Contents/Info.plist" >/dev/null

cp "$root/platforms/LICENSE" "$app/Contents/Resources/licenses/LICENSE.txt"
cp "$root/LICENSE" "$app/Contents/Resources/licenses/CC0-1.0.txt"
cp "$root/THIRD_PARTY_NOTICES.md" "$app/Contents/Resources/licenses/THIRD_PARTY_NOTICES.md"
bash "$(cd "$(dirname "$0")" && pwd)/bundle_licenses.sh" "$(cd "$(dirname "$0")/.." && pwd)" "$kr" "$app/Contents/Resources/licenses"
sdl_lic=""
for c in "$(dirname "$sdl")/../share/licenses/SDL3/LICENSE.txt" "$(dirname "$sdl")/../LICENSE.txt" \
         "$(dirname "$sdl")/../share/doc/SDL3/LICENSE.txt"; do
    [ -e "$c" ] && { sdl_lic=$c; break; }
done
[ -n "$sdl_lic" ] || sdl_lic=${GGX_SDL_LICENSE:-}
[ -n "$sdl_lic" ] && [ -e "$sdl_lic" ] || { echo "package_macos: SDL3's LICENSE.txt not found; set GGX_SDL_LICENSE" >&2; exit 1; }
cp "$sdl_lic" "$app/Contents/Resources/licenses/SDL3-LICENSE.txt"

codesign --force --deep --sign - "$app"
codesign --verify --deep --strict "$app"

cp "$root/platforms/LICENSE" "$stage/LICENSE.txt"
cat > "$stage/README.txt" <<EOT
Guilty Gear X Plus PC port $ver (macOS $min or later, Apple Silicon)

Drag GGX Plus to Applications and open it. On the first run it asks for an
image of your own disc (ISO, BIN/CUE or CHD of SLPM-645.25 Korea or
SLPS-201.37 Japan), checks it and copies its files.

This app is not notarized. The first time, macOS says it can't check it:
open System Settings > Privacy & Security, scroll to Security, choose
Open Anyway next to the message about GGX Plus, and confirm. This is
needed once.

Your saves, replays and settings are in
~/Library/Application Support/ggx-plus/. To uninstall, move the app to
the Trash.

Documentation: https://github.com/devin-thomas/guilty-gear-x-plus/tree/main/docs
EOT
ln -s /Applications "$stage/Applications"
hdiutil create -quiet -volname "$name $ver" -srcfolder "$stage" -fs HFS+ -format UDZO -ov "$dmg"
hdiutil verify -quiet "$dmg"
echo "package_macos: $dmg"
(cd "$stage" && find . | sort)
