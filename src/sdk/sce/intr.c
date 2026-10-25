/* Adapted from nathanialf/ico (MIT; e0881204, sce/libkernl/intr.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libkernl.a(intr.o) */
#include "eekernel.h"
#include "eeregs.h"
#include "libkernl_internal.h"

/* R5900 opcodes with no C spelling.  Defined in this member. */
#define SYNC() __asm__ __volatile__("sync" : : : "memory")
/* COP0 Status ($12), bit 16 = interrupts enabled.  Taken as an lvalue: the
   wrappers mask the word in place. */
#define MFC0_STATUS(dst) __asm__ __volatile__("mfc0 %0, $12" : "=r"(dst))
#define COP0_STATUS_EIE 0x10000

#ifdef VERSION_KR
/* SDK 2.5: nonzero when called outside an interrupt handler (Status.IE set) */
int QueryIntrContext(void)
{
    int status;
    __asm__ __volatile__("mfc0 %0, $12" : "=r"(status));
    return (status ^ 1) & 1;
}
#endif

int DisableIntc(int cause)
{
    int eie;
    int rv;
    MFC0_STATUS(eie);
    eie &= COP0_STATUS_EIE;
    if (eie) {
        DIntr();
    }
    rv = _DisableIntc(cause);
    SYNC();
    if (eie) {
        EIntr();
    }
    return rv;
}

int EnableIntc(int cause)
{
    int eie;
    int rv;
    MFC0_STATUS(eie);
    eie &= COP0_STATUS_EIE;
    if (eie) {
        DIntr();
    }
    rv = _EnableIntc(cause);
    SYNC();
    if (eie) {
        EIntr();
    }
    return rv;
}

int DisableDmac(int channel)
{
    int eie;
    int rv;
    MFC0_STATUS(eie);
    eie &= COP0_STATUS_EIE;
    if (eie) {
        DIntr();
    }
    rv = _DisableDmac(channel);
    SYNC();
    if (eie) {
        EIntr();
    }
    return rv;
}

int EnableDmac(int channel)
{
    int eie;
    int rv;
    MFC0_STATUS(eie);
    eie &= COP0_STATUS_EIE;
    if (eie) {
        DIntr();
    }
    rv = _EnableDmac(channel);
    SYNC();
    if (eie) {
        EIntr();
    }
    return rv;
}

int iEnableIntc(int cause)
{
    int r = _iEnableIntc(cause);
    SYNC();
    return r;
}

int iDisableIntc(int cause)
{
    int r = _iDisableIntc(cause);
    SYNC();
    return r;
}

int iEnableDmac(int channel)
{
    int r = _iEnableDmac(channel);
    SYNC();
    return r;
}

int iDisableDmac(int channel)
{
    int r = _iDisableDmac(channel);
    SYNC();
    return r;
}