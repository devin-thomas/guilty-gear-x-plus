#!/usr/bin/env bash
# check_dlls.sh <dir>: every DLL that the programs (*.exe) and the DLLs beside them import is beside it too, or is Windows' own
# (System32, or an api-ms-/ext-ms- API set), so ggx.exe starts from a shortcut with nothing else on PATH (PORT-018,
# cmake/BundleDlls.cmake). For MSYS2's bash on a Windows runner; needs objdump.
set -u
dir=${1:?usage: check_dlls.sh <dir with ggx.exe>}
sys=$(cygpath -u "${SYSTEMROOT:-C:/Windows}")/System32
bad=0
shopt -s nullglob nocaseglob
for f in "$dir"/*.exe "$dir"/*.dll; do
    while read -r dll; do
        lower=${dll,,}
        case $lower in
        api-ms-*|ext-ms-*) continue ;;
        esac
        if compgen -G "$dir/$dll" >/dev/null || [ -e "$sys/$dll" ]; then
            continue
        fi
        echo "::error::$(basename "$f") needs $dll, which is neither beside ggx.exe nor in $sys"
        bad=1
    done < <(objdump -p "$f" | sed -n 's/^[[:space:]]*DLL Name: //p' | tr -d '\r')
done
if [ $bad = 0 ]; then
    echo "check_dlls: every DLL ggx.exe needs is beside it or Windows' own:" $(cd "$dir" && ls *.dll 2>/dev/null)
fi
exit $bad
