/* Adapted from nathanialf/ico (MIT; e0881204, sce/libkernl/cache.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libkernl.a member cache.o */
#include "libkernl_internal.h"
#include "eekernel.h"

/* _sceSDC: hand-written assembly, kept in the split */

/* C around the assembly loop: interrupts off while the lines are walked,
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
/* _sceIDC: hand-written assembly, kept in the split */

/* C around the assembly loop: interrupts off while the lines are walked,
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