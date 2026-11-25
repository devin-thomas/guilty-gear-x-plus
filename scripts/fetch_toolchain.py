#!/usr/bin/env python3
"""Fetch the PS2 toolchain the matching build needs. It is never vendored.

The packages, their URLs and their SHA-256 pins are in scripts/toolchain.json; uppercut-ps2-match downloads, unpacks and checks them:
- the EE-GCC packages from decomp.me's archive (github.com/decompme/compilers, release `compilers`):
  - ee-gcc2.9-991111: cc1, cpp and the JP game assembler;
  - ee-gcc2.9-991111-01: its newer GAS, the KR game assembler (then patched
    into ee-gcc2.9-991111-01-fix by scripts/patch_kr_as.py);
  - ee-gcc2.96: the 2.10-ee-001003-1 assembler the Sony SDK TUs need;
- decompals' binutils for mips-ps2 (github.com/decompals/binutils-mips-ps2-decompals), v0.10.

Every archive is checked by SHA-256: GitHub's published digest for binutils, and for EE-GCC (decompme/compilers publishes none) the
digest of the archives fetched for the first public build. Each EE-GCC package is then also checked by the SHA-256 of the files the
build runs. A mismatch stops the script and leaves nothing in place.

EE-GCC is a Linux i386 program. On other hosts it runs under qemu-i386 (see scripts/verify.py's QEMU, or GGX_QEMU); that runtime is a
host package, not fetched here.

  fetch_toolchain.py [--dest tools] [--host linux-x86-64|macos-arm64|windows-x86-64] [--dry-run]
"""
import argparse
import json
import subprocess
import sys
from pathlib import Path

from uppercut_ps2_match import MatchError, fetch

HERE = Path(__file__).resolve().parent
HOSTS = ['linux-x86-64', 'macos-arm64', 'windows-x86-64']
KR_AS_FIX = ('ee-gcc2.9-991111-01-fix', 'ee/bin/as',
             '62ba29739437bcdecffea49607b690834ff159c7d77beded8dbe6333d9fb7ed3')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--dest', default=str(HERE.parent / 'tools'), help='install directory (default: ./tools)')
    ap.add_argument('--host', choices=HOSTS, default=fetch.default_host(), help='binutils build to fetch')
    ap.add_argument('--no-binutils', action='store_true')
    ap.add_argument('--dry-run', action='store_true', help='print what would be fetched, download nothing')
    a = ap.parse_args()
    dest = Path(a.dest).resolve()

    config = json.loads((HERE / 'toolchain.json').read_text())
    if a.no_binutils:
        config['packages'] = [p for p in config['packages'] if p['name'] != 'binutils']
    elif not a.host:
        raise SystemExit('no binutils build for this host; pass --host or --no-binutils')
    try:
        packages = fetch.load_packages(config, a.host)
        fix_pkg, fix_rel, fix_sha = KR_AS_FIX
        shown = []

        def log(line):
            print(line)
            shown.append(line)
            if len(shown) == len(packages):  # after the plan, as before
                print(f'{fix_pkg:24} {"present" if (dest / fix_pkg).exists() else "make":8} scripts/patch_kr_as.py')

        fetch.fetch_toolchain(packages, dest, dry_run=a.dry_run, log=log)
        if a.dry_run:
            return 0
        fixed = dest / fix_pkg
        if not fixed.exists():
            subprocess.run([sys.executable, str(HERE / 'patch_kr_as.py'),
                            str(dest / 'ee-gcc2.9-991111-01'), str(fixed)], check=True)
        fetch.check_files(fixed, {fix_rel: fix_sha}, fix_pkg)
    except MatchError as error:
        raise SystemExit(str(error))
    print(f'{fix_pkg}: ok')
    print(f'toolchain ready in {dest}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
