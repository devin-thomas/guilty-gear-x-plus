#!/usr/bin/env python3
"""abi_check.py <obj.o | @list>...: every function whose WebAssembly signature differs between objects.

C lets one TU declare `void f(int)` while another defines `int f(int)`; the
EE ignores the difference, but wasm-ld turns each mismatched call into a trap.
Only direct calls are compared.
This lists every such function at once (wasm-ld reports one pair per symbol),
with the signature each object uses. Exit status 1 if any are found.
"""
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import wasmfile   # noqa: E402


def signatures(paths):
    """{name: {signature: [(object, 'def'|'use')]}} for global function symbols."""
    sigs = defaultdict(lambda: defaultdict(list))
    for p in paths:
        m = wasmfile.Module(Path(p).read_bytes())
        # only calls are checked: wasm-ld gives an address-only use through a
        # K&R declaration the definition's own table entry
        called = {i for entries in m.relocs.values() for t, _, i, _ in entries
                  if t == wasmfile.R_FUNCTION_INDEX_LEB}
        for k, s in enumerate(m.symbols):
            if s.kind != wasmfile.SYM_FUNCTION or s.local or (s.undefined and k not in called):
                continue
            sig = wasmfile.fmt_sig(m.signature(s.index))
            sigs[s.name][sig].append((Path(p).stem, 'use' if s.undefined else 'def'))
    return sigs


def main():
    paths = []
    for a in sys.argv[1:]:          # @file: one object per line
        paths += [l.strip() for l in Path(a[1:]).read_text().splitlines() if l.strip()] if a.startswith('@') else [a]
    bad = 0
    for name, by_sig in sorted(signatures(paths).items()):
        if len(by_sig) < 2:
            continue
        bad += 1
        print(name)
        for sig, users in sorted(by_sig.items(), key=lambda kv: not any(k == 'def' for _, k in kv[1])):
            print(f'    {sig:40} ' + ' '.join(f'{o}({k})' if k == 'def' else o for o, k in sorted(users)))
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
