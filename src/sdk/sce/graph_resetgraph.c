/* Adapted from nathanialf/ico (MIT; e0881204, sce/libgraph/graph001.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libgraph.a member graph001.o */
#include "libgraph.h"
#include "eeregs.h"
#include "eekernel.h"

/* the member's .data: its build stamp and the record, interlaced NTSC frame
   mode until sceGsResetGraph sets it.  The stamp is 16-aligned in Sony's
   object; as the member's first object its alignment is the section's. */
/* STATIC_EXTERN: "PsIIlibgraph2300" (JP) / "PsIIlibgraph2500" (KR), kept in the bins */
extern char sceGsVersion[16] __attribute__((aligned(16))); /* derived name */

static sceGsGParam gsGParam = {1, 2, 1, 3, 0, 0}; /* derived name */

void sceGsResetGraph(short mode, short inter, short omode, short ffmd)
{
    sceGsGParam *g;

    switch (mode) {
    case 0:
        g = sceGsGetGParam();
        *GS_CSR = 0x200;
        g->inter = inter;
        g->omode = omode;
        g->version = (*GS_CSR >> 16) & 0xFF;
        GsPutIMR(0xFF00);
        g->ffmd = ffmd != 0;
        if (g->intcUsed != 0) {
            DisableIntc(2);
            RemoveIntcHandler(2, g->handler);
            g->intcUsed = 0;
            g->handler = 0;
        }
        SetGsCrt(inter & 1, omode & 0xFF, ffmd & 1);
        break;
    case 1:
        *GS_CSR = 0x100;
        break;
    case 5:
        /* SDK 2.3: change the video mode without resetting the GS */
        g = sceGsGetGParam();
        g->inter = inter;
        g->omode = omode;
        g->version = (*GS_CSR >> 16) & 0xFF;
        g->ffmd = ffmd != 0;
        SetGsCrt(inter & 1, omode & 0xFF, ffmd & 1);
        break;
    }
}

sceGsGParam *sceGsGetGParam(void)
{
    return &gsGParam;
}