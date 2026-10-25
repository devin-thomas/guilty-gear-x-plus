/* libkernl.a(initsys.o). Written from the GGX discs, with nathanialf/ico (MIT;
   e0881204, sce/libkernl/initsys.c, SDK 2.2) as the starting point: SDK 2.3
   (JP) only creates the two crt0 semaphores and chains the init calls; SDK 2.5
   (KR) also locates the kernel's syscall table and patches it. */
#include "eekernel.h"
#include "libkernl_internal.h"

int __sce_sema_id;
int __sce_eh_sema_id;

void supplement_crt0(void)
{
    struct SemaParam sp;
    struct SemaParam eh;

    sp.maxCount = 1;
    sp.initCount = 1;
    eh.maxCount = 1;
    eh.initCount = 1;
    __sce_sema_id = CreateSema(&sp);
    __sce_eh_sema_id = CreateSema(&eh);
}

#ifdef VERSION_KR
extern int *_SyscallEntry; /* STATIC_EXTERN */
extern int initsys_SysEntry[4]; /* STATIC_EXTERN (SysEntry, a name tlbfunc.o repeats): {syscall, handler} pairs for kFindAddress and initsys_kCopy, kept in the bins */

/* syscall 90: initsys_Copy(dst, src, len), run in kernel mode */
static void initsys_Copy(void *dst, void *src, int len)
{
    __asm__ __volatile__("addiu $3, $0, 90\n\tsyscall 0" : : : "$3", "memory");
}

static int initsys_kCopy(int *dst, int *src, unsigned int n)
{
    unsigned int i;
    n >>= 2;
    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
    return 0;
}

static int *kFindAddress(int *p, int *end, int v)
{
    while (*p != v && p < end) {
        p++;
    }
    if (!(p < end)) {
        p = 0;
    }
    return p;
}

/* syscall 131: FindAddress(start, end, value), run in kernel mode */
static int *FindAddress(int *p, int *end, int v)
{
    __asm__ __volatile__("addiu $3, $0, 131\n\tsyscall 0" : : : "$3", "memory");
}

int GetSystemCallEntry(int num)
{
    int entry;
    initsys_Copy(&entry, _SyscallEntry + num, 4);
    return entry;
}

static void initsys_setup(int num, int handler);

static void InitSystemCallTableAddress(void)
{
    int *f;
    int *c;
    char *a;
    char *b;

    initsys_setup(initsys_SysEntry[0], initsys_SysEntry[1]);
    initsys_setup(initsys_SysEntry[2], initsys_SysEntry[3]);
    f = FindAddress((int *)0x80000000, (int *)0x80080000, (int)kFindAddress);
    c = FindAddress((int *)0x80000000, (int *)0x80080000, (int)initsys_kCopy);
    a = (char *)f - 0x20C;
    b = (char *)c - 0x168;
    while (a != b) {
        if (a < b) {
            f = FindAddress(f + 1, (int *)0x80080000, (int)kFindAddress);
            a = (char *)f - 0x20C;
        } else {
            c = FindAddress(c + 1, (int *)0x80080000, (int)initsys_kCopy);
            b = (char *)c - 0x168;
        }
    }
    _SyscallEntry = (int *)a;
}

/* syscall 116: SetSyscall(num, handler) */
static void initsys_setup(int num, int handler)
{
    __asm__ __volatile__("addiu $3, $0, 116\n\tsyscall 0" : : : "$3", "memory");
}
#endif

void _InitSys(void)
{
    supplement_crt0();
#ifdef VERSION_KR
    InitSystemCallTableAddress();
#endif
    InitAlarm();
    InitThread();
#ifdef VERSION_KR
    InitExecPS2();
    InitTLBFunctions();
#endif
}