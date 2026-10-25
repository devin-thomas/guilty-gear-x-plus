/* Adapted from nathanialf/ico (MIT; e0881204, sce/libgraph/graph004.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libgraph.a member graph004.o */
#include "libgraph.h"
#include "eeregs.h"

void sceGsPutDispEnv(void *disp)
{
    long *s = (long *)disp;
    if (sceGsGetGParam()->version == 1) {
        *GS_PMODE = s[0];
        *GS_DISPFB1 = s[2];
        *GS_DISPLAY1 = s[3];
        *GS_EXTDATA = s[4];
    } else {
        *GS_PMODE = s[0];
        *GS_SMODE2 = s[1];
        *GS_DISPFB2 = s[2];
        *GS_DISPLAY2 = s[3];
        *GS_BGCOLOR = s[4];
    }
}