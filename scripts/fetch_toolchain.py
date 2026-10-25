#!/usr/bin/env python3
"""Fetch the PS2 toolchain the matching build needs. It is never vendored.

Downloads, unpacks and checks:
- the EE-GCC packages from decomp.me's archive (github.com/decompme/compilers,
  release `compilers`):
  - ee-gcc2.9-991111: cc1, cpp and the JP game assembler;
  - ee-gcc2.9-991111-01: its newer GAS, the KR game assembler (then patched
    into ee-gcc2.9-991111-01-fix by scripts/patch_kr_as.py);
  - ee-gcc2.96: the 2.10-ee-001003-1 assembler the Sony SDK TUs need;
- decompals' binutils for mips-ps2 (github.com/decompals/binutils-mips-ps2-decompals), v0.10.

Every archive is checked by SHA-256: GitHub's published digest for binutils, and for
EE-GCC (decompme/compilers publishes none) the digest of the archives fetched for the
first public build. Each EE-GCC package is then also checked by the SHA-256 of the
files the build runs. A mismatch stops the script and leaves nothing in place.

EE-GCC is a Linux i386 program. On other hosts it runs under qemu-i386 (see
scripts/verify.py's QEMU, or GGX_QEMU); that runtime is a host package, not fetched here.

  fetch_toolchain.py [--dest tools] [--host linux-x86-64|macos-arm64|windows-x86-64] [--dry-run]
"""
import argparse
import hashlib
import io
import os
import platform
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
COMPILERS = 'https://github.com/decompme/compilers/releases/download/compilers/'
BINUTILS = 'https://github.com/decompals/binutils-mips-ps2-decompals/releases/download/v0.10/'

# package dir -> (archive, archive sha256, {path in package: sha256})
EE_GCC = {
    'ee-gcc2.9-991111': ('ee-gcc2.9-991111.tar.xz',
                         '99cb5db603cd985b50fc6851157020ad69489f2227309ccf825bd43ac7bc19dd', {
        'lib/gcc-lib/ee/2.9-ee-991111/cc1': '548c962776e6d01ea0e83a08dc8ea99b87eb7b77bcfd2350d5bce7dc9395398d',
        'lib/gcc-lib/ee/2.9-ee-991111/cpp': '260a2ae5d55d223539be9f4a09a9890a1bc12bcdccf50e015d95e581089df499',
        'ee/bin/as': '82e98546816d6d97ede5cda29deff86bb0c93b9fa456dd6a50520475bee39ff3',
    }),
    'ee-gcc2.9-991111-01': ('ee-gcc2.9-991111-01.tar.xz',
                            'ed684fd98f89d36b0121caab311052089103e3b36241fcef4338cc9ea41c75b8', {
        'ee/bin/as': '296af123052ee39d175e5b3254102aafca105d4f6e975b351a13867f59b04019',
    }),
    'ee-gcc2.96': ('ee-gcc2.96.tar.xz',
                   '0590d2ca9da8f5903889d66761220d14b47a8d14ba987ca53db84a1650a1fd0a', {
        'ee/bin/as': 'b8cfdb6ecb6931642914020f92a62e653378f57230eed4895f69afa050d47b5e',
    }),
}
KR_AS_FIX = ('ee-gcc2.9-991111-01-fix', 'ee/bin/as',
             '62ba29739437bcdecffea49607b690834ff159c7d77beded8dbe6333d9fb7ed3')
BINUTILS_ARCHIVES = {
    'linux-x86-64': ('binutils-mips-ps2-decompals-linux-x86-64.tar.gz',
                     '9fe31ea3ee1a37536f9f0e2e12c668e7c3cb99e59f5154bd2f1fc4762473094c'),
    'macos-arm64': ('binutils-mips-ps2-decompals-macos-arm64.tar.gz',
                    '948cc736e01b2c135168caabfb616f6ed8efe89f1dad1ed8ef1ca74acb1b742c'),
    'windows-x86-64': ('binutils-mips-ps2-decompals-windows-x86-64.zip',
                       '2bb5904ee174e04f1600cc70ffdb3dc36b0efa4d1892ea446be588ba8dc6198c'),
}


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def default_host():
    m = platform.machine().lower()
    if sys.platform == 'darwin':
        return 'macos-arm64'
    if sys.platform == 'win32':
        return 'windows-x86-64'
    return 'linux-x86-64' if m in ('x86_64', 'amd64') else None


def download(url):
    print(f'  GET {url}')
    with urllib.request.urlopen(url) as r:
        return r.read()


def unpack(data, name, dest):
    """Unpack an archive into dest, refusing links and paths outside it."""
    dest.mkdir(parents=True)
    if name.endswith('.zip'):
        with zipfile.ZipFile(io.BytesIO(data)) as z:
            for m in z.infolist():
                target = (dest / m.filename).resolve()
                if not target.is_relative_to(dest.resolve()):
                    raise SystemExit(f'{name}: unsafe path {m.filename}')
            z.extractall(dest)
        return
    with tarfile.open(fileobj=io.BytesIO(data)) as t:
        for m in t.getmembers():
            if m.issym() or m.islnk() or not (dest / m.name).resolve().is_relative_to(dest.resolve()):
                raise SystemExit(f'{name}: unsafe member {m.name}')
        t.extractall(dest, filter='data')


def package_root(tmp, expect):
    """The directory that holds `expect`: the unpack dir itself or its single subdirectory."""
    if (tmp / expect).exists():
        return tmp
    subs = [p for p in tmp.iterdir() if p.is_dir()]
    if len(subs) == 1 and (subs[0] / expect).exists():
        return subs[0]
    raise SystemExit(f'{tmp.name}: {expect} not found in the archive')


def check_files(root, files, label):
    for rel, want in files.items():
        got = sha256((root / rel).read_bytes())
        if got != want:
            raise SystemExit(f'{label}: {rel} has SHA-256 {got}, expected {want}')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--dest', default=str(HERE.parent / 'tools'), help='install directory (default: ./tools)')
    ap.add_argument('--host', choices=sorted(BINUTILS_ARCHIVES), default=default_host(),
                    help='binutils build to fetch')
    ap.add_argument('--no-binutils', action='store_true')
    ap.add_argument('--dry-run', action='store_true', help='print what would be fetched, download nothing')
    a = ap.parse_args()
    dest = Path(a.dest).resolve()

    plan = [(pkg, COMPILERS + archive, digest, files) for pkg, (archive, digest, files) in EE_GCC.items()]
    if not a.no_binutils:
        if not a.host:
            raise SystemExit('no binutils build for this host; pass --host or --no-binutils')
        archive, digest = BINUTILS_ARCHIVES[a.host]
        plan.append(('binutils', BINUTILS + archive, digest, {}))

    for pkg, url, digest, _ in plan:
        state = 'present' if (dest / pkg).exists() else 'fetch'
        print(f'{pkg:24} {state:8} {url}')
    print(f'{KR_AS_FIX[0]:24} {"present" if (dest / KR_AS_FIX[0]).exists() else "make":8} scripts/patch_kr_as.py')
    if a.dry_run:
        return 0

    dest.mkdir(parents=True, exist_ok=True)
    for pkg, url, digest, files in plan:
        final = dest / pkg
        if final.exists():
            if files:
                check_files(final, files, pkg)
            print(f'{pkg}: present')
            continue
        data = download(url)
        if digest and sha256(data) != digest:
            raise SystemExit(f'{pkg}: archive SHA-256 {sha256(data)}, expected {digest}')
        with tempfile.TemporaryDirectory(dir=dest) as tmp:
            tmp = Path(tmp) / 'unpack'
            unpack(data, url.rsplit('/', 1)[1], tmp)
            root = package_root(tmp, next(iter(files), '.'))
            check_files(root, files, pkg)
            shutil.move(str(root), final)
        print(f'{pkg}: ok (archive SHA-256 {sha256(data)})')

    fix_pkg, fix_rel, fix_sha = KR_AS_FIX
    fixed = dest / fix_pkg
    if not fixed.exists():
        subprocess.run([sys.executable, str(HERE / 'patch_kr_as.py'),
                        str(dest / 'ee-gcc2.9-991111-01'), str(fixed)], check=True)
    check_files(fixed, {fix_rel: fix_sha}, fix_pkg)
    print(f'{fix_pkg}: ok')
    print(f'toolchain ready in {dest}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
