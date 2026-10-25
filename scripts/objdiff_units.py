#!/usr/bin/env python3
"""Write an objdiff project (objdiff.json plus one target and one base object per
TU) into a finished build.py work directory, and optionally run
`objdiff-cli report generate` on it.

Run it after `build.py --target <jp|kr> --work <work>`, or pass `--objdiff` to
build.py, which calls emit() at the end of a byte-identical build.

Per splat segment (TU) in the work dir's frozen splat.yaml:
- target `objdiff/target/<tu>.o`: splat's full disassembly of the TU,
  `asm/<tu>.s`, assembled the way build.py assembles an all-asm TU. It holds
  the original code, so it stays in the work dir like everything else there.
- base `objdiff/base/<tu>.o`: every matched function's object that build.py
  linked (`build/src/<tu>/<function>.o`), combined with `ld -r`. These are the
  exact objects in the byte-identical ELF. A TU with no linked C has no base.

objdiff.json sits in the work dir, so open the work dir as the project in the
objdiff GUI, or run `objdiff-cli report generate -p <work>`. Unit names are the
splat segment names (`ggxeuc/tension`); `source_path` is relative to the repo
root (`src/ggxeuc/tension.c`). Progress categories follow scripts/progress.py:
`game` (ggxeuc/*), `sdk` (sdk/* except crt0) and `crt0`.

Only build.py's own outputs are read. Nothing is written outside <work>.
"""
import argparse
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

import yaml

HERE = Path(__file__).resolve().parent.parent
FLAGS = ['-EL', '-march=5900', '-mabi=eabi', '-no-pad-sections']   # build.py's FLAGS
RECORD = re.compile(r'^nonmatching\s+(\S+),', re.M)
CATEGORIES = [
    {'id': 'game', 'name': 'Game code (ggxeuc)'},
    {'id': 'sdk', 'name': 'Sony SDK and C runtime'},
    {'id': 'crt0', 'name': 'crt0'},
]
# decomp.me: EE-GCC 2.9-ee-991111 is the game's cc1. The JP build also uses that
# package's GAS; KR uses the 991111-01 GAS (patched, scripts/patch_kr_as.py),
# which decomp.me doesn't have, so a KR scratch can differ in loop padding.
SCRATCH = {'platform': 'ps2', 'compiler': 'ee-gcc2.9-991111', 'c_flags': '-O2 -G8'}
VERSIONS = {'jp': 'SLPS_201.37', 'kr': 'SLPM_645.25'}


def group(tu):
    if tu == 'sdk/crt0':
        return 'crt0'
    return 'sdk' if tu.startswith('sdk/') else 'game'


def emit(work, target, binutils, objdiff_cli=None, report=None):
    """Write <work>/objdiff.json and its objects. Returns the objdiff.json path.
    With objdiff_cli, also writes the report (default <work>/report.json)."""
    work, binutils = Path(work).resolve(), Path(binutils).resolve()
    result = json.loads((work / 'result.json').read_text())
    if result.get('target') != VERSIONS[target]:
        raise SystemExit(f'{work} is a {result.get("target")} build, not {VERSIONS[target]}')
    if not result.get('elf_byte_identical'):
        raise SystemExit(f'{work}: the build is not byte-identical; no objdiff project written')
    linked = set(result['c_function_names'])
    config = yaml.safe_load((work / 'splat.yaml').read_text())
    tus = [s for s in config['segments'] if isinstance(s, dict)][0]['subsegments']
    as_, ld = binutils / 'mips-ps2-decompals-as', binutils / 'mips-ps2-decompals-ld'
    out = work / 'objdiff'
    shutil.rmtree(out, ignore_errors=True)

    def run(cmd):
        r = subprocess.run([str(c) for c in cmd], cwd=work, capture_output=True, text=True)
        if r.returncode:
            raise SystemExit(f'{" ".join(map(str, cmd))}\n{r.stdout}{r.stderr}')

    units = []
    for _, kind, name in tus:
        tobj = out / 'target' / f'{name}.o'
        tobj.parent.mkdir(parents=True, exist_ok=True)
        asm = work / 'asm' / f'{name}.s'
        run([as_, *FLAGS, '-I', 'include', '-o', tobj, asm])
        records = RECORD.findall(asm.read_text())
        objs = []
        fdir = work / 'build' / 'src' / name
        if kind != 'asm' and fdir.is_dir():
            # _data.o is the TU's compiled data; its bytes are checked by build.py, not here
            objs = sorted(p for p in fdir.glob('*.o') if p.stem in linked and p.name != '_data.o')
        unit = {'name': name, 'target_path': str(tobj.relative_to(work))}
        if objs:
            bobj = out / 'base' / f'{name}.o'
            bobj.parent.mkdir(parents=True, exist_ok=True)
            run([ld, '-EL', '-r', '-o', bobj, *objs])
            unit['base_path'] = str(bobj.relative_to(work))
        matched = {p.stem for p in objs}
        meta = {'complete': bool(records) and all(r in matched for r in records),
                'progress_categories': [group(name)]}
        src = HERE / 'src' / f'{name}.c'
        if src.exists():
            meta['source_path'] = f'src/{name}.c'
        unit['metadata'] = meta
        if group(name) == 'game':
            unit['scratch'] = dict(SCRATCH)
        units.append(unit)

    project = {
        'min_version': '3.0.0',
        # build.py builds everything in one pass; objdiff only reads the objects
        'build_target': False,
        'build_base': False,
        'watch_patterns': [],
        'options': {'mips.instrCategory': 'r5900'},
        'progress_categories': CATEGORIES,
        'units': units,
    }
    path = work / 'objdiff.json'
    path.write_text(json.dumps(project, indent=2) + '\n')
    print(f'objdiff: {path} ({len(units)} units, {sum("base_path" in u for u in units)} with C)', flush=True)
    if objdiff_cli:
        report = Path(report or work / 'report.json').resolve()
        r = subprocess.run([str(objdiff_cli), 'report', 'generate', '-p', str(work), '-o', str(report),
                            '-f', 'json-pretty'], capture_output=True, text=True)
        if r.returncode:
            raise SystemExit(f'objdiff-cli failed:\n{r.stdout}{r.stderr}')
        print(summary(report), flush=True)
    return path


def summary(report):
    """One line per progress category (and overall) from a report.json."""
    rep = json.loads(Path(report).read_text())
    rows = [('all', rep['measures'])] + [(c['id'], c['measures']) for c in rep.get('categories', [])]
    lines = []
    for cid, m in rows:
        lines.append(f'{cid:5} functions {m.get("matched_functions", 0)}/{m.get("total_functions", 0)}'
                     f'  code {m.get("matched_code", 0)}/{m.get("total_code", 0)} bytes'
                     f' ({m.get("matched_code_percent", 0):.2f}%)'
                     f'  fuzzy {m.get("fuzzy_match_percent", 0):.2f}%'
                     f'  units complete {m.get("complete_units", 0)}/{m.get("total_units", 0)}')
    return '\n'.join(lines)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--target', choices=VERSIONS, required=True)
    ap.add_argument('--work', required=True, type=Path, help='a finished build.py --work directory')
    ap.add_argument('--binutils', required=True, type=Path)
    ap.add_argument('--objdiff-cli', type=Path, help='run objdiff-cli report generate as well')
    ap.add_argument('--report', type=Path, help='report path (default <work>/report.json)')
    args = ap.parse_args()
    emit(args.work, args.target, args.binutils, args.objdiff_cli, args.report)


if __name__ == '__main__':
    sys.exit(main())
