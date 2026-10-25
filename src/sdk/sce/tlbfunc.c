/* libkernl.a(tlbfunc.o), SDK 2.5 (KR only; SDK 2.3 has no TLB functions).
   SetTLBHandler and SetDebugHandler are adapted from nathanialf/ico (MIT;
   e0881204, sce/libkernl/tlbfunc.c, SDK 2.2); the rest is written from the
   KR disc. The syscall leaves are `addiu $3,$0,N; syscall 0` with gcc's
   `jr $31; nop`. InitTLB32MB keeps a counter in $t9 across its calls and
   stays assembly. */
#include "eekernel.h"
/* file statics named as the split names them (other libkernl members repeat the names) */
#include "libkernl_internal.h"

#define SYSCALL(num) __asm__ __volatile__("addiu $3, $0, " #num "\n\tsyscall 0" : : : "$3", "memory")


extern int _kTLBRefillHandler; /* DATA_EXTERN: defined with an initializer in the original; bytes kept in the bins */

extern int _kDebugHandler[16]; /* DATA_EXTERN: defined with an initializer in the original; bytes kept in the bins */

int _kExecArg;
extern unsigned int tlbsrc[204]; /* STATIC_EXTERN: the kernel-mode TLB code copied to 0x80075000, kept in the bins */
extern int tlbfunc_SysEntry[16]; /* STATIC_EXTERN (SysEntry, a name initsys.o repeats): {syscall, handler} pairs, kept in the bins */

void *SetTLBHandler(void *handler)
{
    _kTLBRefillHandler = (int)handler;
    SetVTLBRefillHandler(1, _kTLBException);
    SetVTLBRefillHandler(2, _kTLBException);
    SetVTLBRefillHandler(3, _kTLBException);
    return handler;
}

int SetDebugHandler(int cause, int handler)
{
    int old;
    int orig = cause;
    unsigned int err = 0xFFFFFFFF;
    if ((unsigned)(cause - 1) >= 13) {
        return (int)err;
    }
    old = _kDebugHandler[orig];
    _kDebugHandler[orig] = handler;
    if ((unsigned)(cause - 1) < 3) {
        SetVTLBRefillHandler(orig, (void *)_kDebugException);
    } else {
        SetVCommonHandler(orig, (void *)_kDebugException);
    }
    return old;
}
/* syscall 90: tlbfunc_Copy(dst, src, len) in kernel mode */
static void tlbfunc_Copy(void *dst, void *src, int len)
{
    SYSCALL(90);
}

static int tlbfunc_kCopy(int *dst, int *src, unsigned int n)
{
    unsigned int i;
    n >>= 2;
    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
    return 0;
}

/* syscall 91 */
static int tlbfunc_GetEntryAddress(int num)
{
    SYSCALL(91);
}

/* syscall 116: SetSyscall(num, handler) */
static void tlbfunc_setup(int num, int handler)
{
    SYSCALL(116);
}

void InitTLBFunctions(void)
{
    int *p;
    unsigned int i;

    tlbfunc_setup(tlbfunc_SysEntry[0], tlbfunc_SysEntry[1]);
    tlbfunc_Copy((void *)0x80075000, tlbsrc, 0x330);
    FlushCache(0);
    FlushCache(2);
    tlbfunc_setup(tlbfunc_SysEntry[2], tlbfunc_SysEntry[3]);
    tlbfunc_setup(tlbfunc_SysEntry[4], tlbfunc_SysEntry[5]);
    p = &tlbfunc_SysEntry[6];
    for (i = 3; i < 8; i++) {
        tlbfunc_setup(p[0], tlbfunc_GetEntryAddress(p[0]));
        p += 2;
    }
    _kExecArg = tlbfunc_GetEntryAddress(3);
}

int PutTLBEntry(unsigned int pagemask, unsigned int hi, unsigned int lo0, unsigned int lo1)
{
    SYSCALL(85);
}

int iPutTLBEntry(unsigned int pagemask, unsigned int hi, unsigned int lo0, unsigned int lo1)
{
    SYSCALL(-85);
}

int _SetTLBEntry(unsigned int index, unsigned int pagemask, unsigned int hi, unsigned int lo0, unsigned int lo1)
{
    SYSCALL(86);
}

int SetTLBEntry(unsigned int index, unsigned int pagemask, unsigned int hi, unsigned int lo0, unsigned int lo1)
{
    if (index - 13 >= 35) {
        return -1;
    }
    return _SetTLBEntry(index, pagemask, hi, lo0, lo1);
}

int iSetTLBEntry(unsigned int index, unsigned int pagemask, unsigned int hi, unsigned int lo0, unsigned int lo1)
{
    SYSCALL(-86);
}

int GetTLBEntry(unsigned int index, unsigned int *pagemask, unsigned int *hi, unsigned int *lo0, unsigned int *lo1)
{
    SYSCALL(87);
}

int iGetTLBEntry(unsigned int index, unsigned int *pagemask, unsigned int *hi, unsigned int *lo0, unsigned int *lo1)
{
    SYSCALL(-87);
}

int ProbeTLBEntry(unsigned int hi, unsigned int *pagemask, unsigned int *lo0, unsigned int *lo1)
{
    SYSCALL(88);
}

int iProbeTLBEntry(unsigned int hi, unsigned int *pagemask, unsigned int *lo0, unsigned int *lo1)
{
    SYSCALL(-88);
}

int ExpandScratchPad(unsigned int page)
{
    SYSCALL(89);
}

int _InitTLB(void);

typedef struct {
    unsigned int pagemask;
    unsigned int hi;
    unsigned int lo0;
    unsigned int lo1;
} TLBEntry; /* derived name */

typedef struct {
    int nkernel;
    int ndefault;
    int nextended;
    int wired;
    TLBEntry *kernel;
    TLBEntry *def;
    TLBEntry *extended;
} TLBInfoRec; /* derived name */

extern TLBInfoRec TLBInfo;

/* the 32 MB machine's TLB: the kernel, default and extended entries from
   TLBInfo, then the rest unmapped. The entry index lives in $t9 across the
   kernel calls (a register variable in the original). */
int InitTLB32MB(void)
{
    register int i __asm__("$25");
    TLBEntry *e;
    int n;
    int j;

    kprintf("# TLB spad=0 kernel=1:%d default=%d:%d extended=%d:%d\n", TLBInfo.nkernel - 1, TLBInfo.nkernel,
            TLBInfo.nkernel + TLBInfo.ndefault - 1, TLBInfo.nkernel + TLBInfo.ndefault,
            TLBInfo.nkernel + TLBInfo.ndefault + TLBInfo.nextended - 1);
    __asm__ __volatile__("mtc0 $0, $6\n\tsync.p");
    n = TLBInfo.nkernel;
    i = 0;
    if (n > 48) {
        kprintf("# TLB over flow (1)");
        Exit(1);
    }
    for (e = TLBInfo.kernel; i < n; i++, e++) {
        _SetTLBEntry(i, e->pagemask, e->hi, e->lo0, e->lo1);
    }
    n = i + TLBInfo.ndefault;
    if (n > 48) {
        kprintf("# TLB over flow (2)");
        Exit(1);
    }
    for (e = TLBInfo.def; i < n; i++, e++) {
        _SetTLBEntry(i, e->pagemask, e->hi, e->lo0, e->lo1);
    }
    TLBInfo.wired = i;
    __asm__ __volatile__("mtc0 %0, $6\n\tsync.p" : : "r"(i));
    if (TLBInfo.nextended > 0) {
        n = i + TLBInfo.nextended;
        if (n > 48) {
            kprintf("# TLB over flow (3)");
            Exit(1);
        }
        for (e = TLBInfo.extended; i < n; i++, e++) {
            _SetTLBEntry(i, e->pagemask, e->hi, e->lo0, e->lo1);
        }
    }
    for (j = i; j < 48; j++) {
        _SetTLBEntry(j, 0, 0xE0000000 + j * 0x2000, 0, 0);
    }
    return i;
}

int InitTLB(void)
{
    if (GetMemorySize() == 0x2000000) {
        return InitTLB32MB();
    }
    return _InitTLB();
}