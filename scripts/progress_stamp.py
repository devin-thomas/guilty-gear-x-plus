#!/usr/bin/env python3
"""Tie committed objdiff reports to the source they were made from.

CI can't build the game (no disc data in the repo), so the reports in
progress/<version>/report.json are made locally from a byte-identical build
(build.py --objdiff --objdiff-cli ...) and committed. This script hashes every
input that decides what the build compiles and matches, so CI can fail when the
source changed and the reports weren't regenerated.

  python scripts/progress_stamp.py write progress/stamp.json   # after copying in new reports
  python scripts/progress_stamp.py check progress/stamp.json   # in CI

The stamp also records each report's SHA-256, so a report edited by hand fails too.
Line endings are normalised (CRLF -> LF) so a Windows checkout gives the same hash.
The write command also generates Shields.io endpoint JSON badges per version;
check verifies that those badges agree with the committed objdiff reports.
"""
import hashlib
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
INPUTS = ['build.py', 'src', 'include', 'kr', 'jp',
          'scripts/verify.py', 'scripts/datafrag.py', 'scripts/xref.py', 'scripts/csplit.py',
          'scripts/sdkprofile.py', 'scripts/objdiff_units.py', 'scripts/klib.py']
REPORTS = {'SLPM_645.25': 'progress/SLPM_645.25/report.json',
           'SLPS_201.37': 'progress/SLPS_201.37/report.json'}


def files():
    out = []
    for entry in INPUTS:
        p = ROOT / entry
        if p.is_file():
            out.append(p)
        elif p.is_dir():
            out += [f for f in p.rglob('*') if f.is_file() and '__pycache__' not in f.parts]
        else:
            raise SystemExit(f'missing input: {entry}')
    # sort by the POSIX path string: Windows sorts Path objects case-insensitively
    return sorted(out, key=lambda f: f.relative_to(ROOT).as_posix())


def digest(path):
    return hashlib.sha256(path.read_bytes().replace(b'\r\n', b'\n')).hexdigest()


def stamp():
    h = hashlib.sha256()
    n = 0
    for f in files():
        h.update(f.relative_to(ROOT).as_posix().encode() + b'\0' + digest(f).encode() + b'\n')
        n += 1
    reports = {}
    for version, rel in REPORTS.items():
        rep = ROOT / rel
        if not rep.exists():
            raise SystemExit(f'missing report: {rel}')
        m = json.loads(rep.read_text())['measures']
        reports[version] = {'path': rel, 'sha256': digest(rep),
                            'matched_functions': m.get('matched_functions', 0),
                            'total_functions': m.get('total_functions', 0)}
    return {'inputs_sha256': h.hexdigest(), 'input_files': n, 'reports': reports}



def badge_payload(version):
    """Make a Shields.io endpoint badge from matched code bytes in an objdiff report."""
    m = json.loads((ROOT / REPORTS[version]).read_text())['measures']
    matched, total = int(m['matched_code']), int(m['total_code'])
    if total <= 0 or matched < 0 or matched > total:
        raise SystemExit(f'{version}: invalid matched code totals')
    percent = 100 * matched / total
    label = 'JP code match' if version == 'SLPS_201.37' else 'KR code match'
    color = ('brightgreen' if percent >= 95 else 'green' if percent >= 80
             else 'yellow' if percent >= 60 else 'red')
    return {'schemaVersion': 1, 'label': label,
            'message': f'{percent:.2f}%', 'color': color}


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in ('write', 'check'):
        raise SystemExit(__doc__)
    path = Path(sys.argv[2])
    now = stamp()
    badges = {version: badge_payload(version) for version in REPORTS}
    if sys.argv[1] == 'write':
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(now, indent=2) + '\n')
        for version, badge in badges.items():
            badge_path = ROOT / 'progress' / version / 'badge.json'
            badge_path.write_text(json.dumps(badge, indent=2) + '\n')
        print(f'wrote {path}: {now["input_files"]} input files, inputs {now["inputs_sha256"][:16]}')
        return
    old = json.loads(path.read_text())
    if old['inputs_sha256'] != now['inputs_sha256']:
        raise SystemExit('The source changed since the committed reports were made. Rebuild both discs with '
                         'build.py --objdiff --objdiff-cli, copy the reports into progress/, and run '
                         '`progress_stamp.py write`.')
    for version, r in now['reports'].items():
        if old['reports'].get(version, {}).get('sha256') != r['sha256']:
            raise SystemExit(f'{version}: report.json differs from the one the stamp recorded')
    for version, badge in badges.items():
        badge_path = ROOT / 'progress' / version / 'badge.json'
        if not badge_path.is_file() or json.loads(badge_path.read_text()) != badge:
            raise SystemExit(f'{version}: stale code-match badge; run '
                             '`python scripts/progress_stamp.py write progress/stamp.json`')
    print(f'reports and badges are current ({now["input_files"]} input files)')


if __name__ == '__main__':
    main()
