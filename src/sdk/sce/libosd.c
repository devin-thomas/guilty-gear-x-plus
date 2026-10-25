/* libkernl.a(libosd.o), SDK 2.5 (KR only), written from the KR disc in the
   style of nathanialf/ico's (MIT) syscall helpers: when the OSD config says
   the console needs it, install the patched ExecPS2 kernel code. */
#include "eekernel.h"
/* file statics named as the split names them (other libkernl members repeat the names) */
#include "libkernl_internal.h"

#define SYSCALL(num) __asm__ __volatile__("addiu $3, $0, " #num "\n\tsyscall 0" : : : "$3", "memory")

extern int SysExecPS2Entry[6];     /* libosd.o .data, kept in the bins: {syscall, handler} pairs */
extern unsigned int osdsrc[490];   /* STATIC_EXTERN: the kernel-mode code copied to 0x80074000, kept in the bins */

/* syscall 116: SetSyscall(num, handler) */
static void libosd_setup(int num, int handler)
{
    SYSCALL(116);
}

/* syscall 90: libosd_Copy(dst, src, len) in kernel mode */
static void libosd_Copy(void *dst, void *src, int len)
{
    SYSCALL(90);
}

static int libosd_kCopy(int *dst, int *src, unsigned int n)
{
    unsigned int i;
    n >>= 2;
    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
    return 0;
}

/* syscall 91 */
static int libosd_GetEntryAddress(int num)
{
    SYSCALL(91);
}

static int PatchIsNeeded(void)
{
    unsigned int orig;
    unsigned int probe;

    GetOsdConfigParam(&orig);
    probe = (orig & 0xFFFF1FFF) | 0x2000;
    SetOsdConfigParam(&probe);
    GetOsdConfigParam(&probe);
    SetOsdConfigParam(&orig);
    return ((probe >> 13) & 7) == 0;
}

void InitExecPS2(void)
{
    int *p;
    unsigned int i;

    if (PatchIsNeeded()) {
        libosd_setup(SysExecPS2Entry[0], SysExecPS2Entry[1]);
        libosd_Copy((void *)0x80074000, osdsrc, 0x7A8);
        FlushCache(0);
        FlushCache(2);
        libosd_setup(SysExecPS2Entry[2], SysExecPS2Entry[3]);
        p = &SysExecPS2Entry[4];
        for (i = 2; i < 3; i++) {
            libosd_setup(p[0], libosd_GetEntryAddress(p[0]));
            p += 2;
        }
    }
}