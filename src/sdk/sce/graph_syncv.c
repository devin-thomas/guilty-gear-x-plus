/* Adapted from nathanialf/ico (MIT; e0881204, sce/libgraph/graph011.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libgraph.a member graph011.o */
#include "libgraph.h"
#include "eeregs.h"
#include "eekernel.h"

int sceGsSyncV(int mode)
{
    sceGsGParam *gp = sceGsGetGParam();
    long c;

    if (gp->intcUsed == 0) {
        VSync();
        if (gp->inter != 1) {
            return 1;
        }
        return (int)((*GS_CSR >> 13) & 1);
    }
    c = VSync2();
    c = (c >> 13) & 1;
    if (gp->inter != 1) {
        return 1;
    }
    return (int)c;
}