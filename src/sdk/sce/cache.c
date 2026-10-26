/* Adapted from nathanialf/ico (MIT; e0881204, sce/libkernl/cache.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libkernl.a member cache.o */
#include "libkernl_internal.h"
#include "eekernel.h"

/* _sceSDC: walk all 64 lines of both ways of the data cache (4 KB per way).
   For each line, read its tag (DXLTG, then TagLo) and, if the line's address
   falls in [start, end], write it back and invalidate it (cache op 0x14,
   DHWBIN). The cache instructions have no C spelling, so each sits in its
   own asm statement, bracketed by sync as the EE manual asks. */
void _sceSDC(int start, int end)
{
    int i;
    int tag;

    for (i = 0; i < 0x1000; i += 0x40) {
        __asm__ __volatile__("sync\n\tcache 0x10, 0(%0)\n\tsync" : : "r"(i));
        __asm__ __volatile__("mfc0 %0, $28" : "=r"(tag));
        tag = (tag & 0xFFFFF000) + i;
        if ((unsigned)tag >= (unsigned)start && (unsigned)tag <= (unsigned)end) {
            __asm__ __volatile__("sync\n\tcache 0x14, 0(%0)\n\tsync" : : "r"(i));
        }
        __asm__ __volatile__("sync\n\tcache 0x10, 1(%0)\n\tsync" : : "r"(i));
        __asm__ __volatile__("mfc0 %0, $28" : "=r"(tag));
        tag = (tag & 0xFFFFF000) + i;
        if ((unsigned)tag >= (unsigned)start && (unsigned)tag <= (unsigned)end) {
            __asm__ __volatile__("sync\n\tcache 0x14, 1(%0)\n\tsync" : : "r"(i));
        }
        __asm__ __volatile__("sync");
    }
}

/* Interrupts off while the loop above walks the lines,
   if they were on */
void SyncDCache(void *start, void *end)
{
    int ie;

    __asm__ __volatile__("mfc0 %0, $12" : "=r"(ie));
    ie &= 0x10000;
    if (ie) {
        DIntr();
    }
    _sceSDC((int)start & 0xFFFFFFC0, (int)end & 0xFFFFFFC0);
    if (ie) {
        EIntr();
    }
}


void iSyncDCache(int start, int end)
{
    _sceSDC(start & 0xFFFFFFC0, end & 0xFFFFFFC0);
}
/* _sceIDC: walk all 64 lines of both ways of the data cache (4 KB per way).
   For each line, read its tag (DXLTG, then TagLo) and, if the line's address
   falls in [start, end], invalidate it (cache op 0x16, DHIN). The cache
   instructions have no C spelling, so each sits in its own asm statement,
   bracketed by sync as the EE manual asks. */
void _sceIDC(int start, int end)
{
    int i;
    int tag;

    for (i = 0; i < 0x1000; i += 0x40) {
        __asm__ __volatile__("sync\n\tcache 0x10, 0(%0)\n\tsync" : : "r"(i));
        __asm__ __volatile__("mfc0 %0, $28" : "=r"(tag));
        tag = (tag & 0xFFFFF000) + i;
        if ((unsigned)tag >= (unsigned)start && (unsigned)tag <= (unsigned)end) {
            __asm__ __volatile__("sync\n\tcache 0x16, 0(%0)\n\tsync" : : "r"(i));
        }
        __asm__ __volatile__("sync\n\tcache 0x10, 1(%0)\n\tsync" : : "r"(i));
        __asm__ __volatile__("mfc0 %0, $28" : "=r"(tag));
        tag = (tag & 0xFFFFF000) + i;
        if ((unsigned)tag >= (unsigned)start && (unsigned)tag <= (unsigned)end) {
            __asm__ __volatile__("sync\n\tcache 0x16, 1(%0)\n\tsync" : : "r"(i));
        }
        __asm__ __volatile__("sync");
    }
}

/* Interrupts off while the loop above walks the lines,
   if they were on */
void InvalidDCache(void *start, void *end)
{
    int ie;

    __asm__ __volatile__("mfc0 %0, $12" : "=r"(ie));
    ie &= 0x10000;
    if (ie) {
        DIntr();
    }
    _sceIDC((int)start & 0xFFFFFFC0, (int)end & 0xFFFFFFC0);
    if (ie) {
        EIntr();
    }
}


void iInvalidDCache(int start, int end)
{
    _sceIDC(start & 0xFFFFFFC0, end & 0xFFFFFFC0);
}