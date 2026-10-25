#!/usr/bin/env python3
"""patch_kr_as.py [stock-package] [out-package]: make the KR game assembler package.

Defaults: /tools/ee-gcc2.9-991111-01 -> /tools/ee-gcc2.9-991111-01-fix (verify.KR_AS_DIR).
Only ee/bin/as is written; nothing else in the package is used for KR game TUs.

Why: the 991111-01 GAS (append_insn, R5900 only) pads short backward loops. For a
conditional branch to an already-defined label it sums the fixed sizes of the frags
from the label's frag to the current one, subtracts the label's offset in its frag,
and inserts 5 - n nops before the branch when the loop is n <= 4 insns long. Its
early exit, though, tests the running sum *including* the bytes before the label in
the label's own frag (`if (sum >> 2 > 5) break;`). So a long loop whose target label
is followed by 4 or fewer insns before a frag break (a .p2align), with 2 or more insns
before it in that frag, is taken for a short loop and gets nops that the original KR
binary does not have (ObjectExecute and CpObjExecute: the backward `goto retry`).
The patch removes the early exit (jg -> 2 nops): the sum then covers every frag up to
the branch and n is the real loop length. True short loops are padded as before.

Checked: all src/ggxeuc TUs give identical per-function KR results with the stock and
the patched assembler, and a full KR build with ObjectExecute and CpObjExecute added
is byte-identical (only MemAccess_Save, a data placement issue, stays assembly).
"""
import hashlib
import shutil
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile

STOCK_SHA1 = '51e0893fd2076a4d92cd12056de85f043244377c'
PATCHED_SHA1 = '863fc7c8c83ad75eb0eee7ea0f8d7af81f43fb87'
VADDR, OLD, NEW = 0x08067b70, b'\x7f\x62', b'\x90\x90'   # jg .+0x62 -> nop; nop
CONTEXT = b'\x83\xfa\x05'                                # cmpl $0x5, %edx


def main():
    src = Path(sys.argv[1] if len(sys.argv) > 1 else '/tools/ee-gcc2.9-991111-01')
    out = Path(sys.argv[2] if len(sys.argv) > 2 else '/tools/ee-gcc2.9-991111-01-fix')
    stock = src / 'ee/bin/as'
    data = bytearray(stock.read_bytes())
    if hashlib.sha1(data).hexdigest() != STOCK_SHA1:
        raise SystemExit(f'{stock} is not the stock 991111-01 as')
    with stock.open('rb') as f:
        segs = [s for s in ELFFile(f).iter_segments() if s['p_type'] == 'PT_LOAD'
                and s['p_vaddr'] <= VADDR < s['p_vaddr'] + s['p_filesz']]
    if not segs:
        raise SystemExit('patch address not in a load segment')
    off = segs[0]['p_offset'] + VADDR - segs[0]['p_vaddr']
    if bytes(data[off - 3:off + 2]) != CONTEXT + OLD:
        raise SystemExit('unexpected bytes at the patch site')
    data[off:off + 2] = NEW
    if hashlib.sha1(data).hexdigest() != PATCHED_SHA1:
        raise SystemExit('patched binary has an unexpected checksum')
    (out / 'ee/bin').mkdir(parents=True, exist_ok=True)
    dst = out / 'ee/bin/as'
    dst.write_bytes(data)
    shutil.copymode(stock, dst)
    print(f'{dst}: patched at file offset {off:#x}, sha1 {PATCHED_SHA1}')


if __name__ == '__main__':
    main()
