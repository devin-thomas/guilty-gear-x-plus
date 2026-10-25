/* Adapted from nathanialf/ico (MIT; e0881204, sce/libkernl/intr.c, its alarm part): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
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

/* intr.o's own file static setup (initsys.o holds the global of the same
   name); InitAlarm calls it. */
static void setup(int num, int addr)
{
    __asm__ __volatile__("addiu $3, $0, 116\n\tsyscall 0" : : : "$3", "memory");
}

/* syscall 90 is Copy(dst, src, len); the leaf ignores its arguments, the
   kernel reads them out of $a0..$a2. */
void Copy(char *dst, char *src, int len)
{
    __asm__ __volatile__("addiu $3, $0, 90\n\tsyscall 0" : : : "$3", "memory");
}

int kCopy(int *dst, int *src, unsigned int n)
{
    unsigned int i;
    n >>= 2;
    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
    return 0;
}

/* An EE syscall leaf (`addiu $3,$zero,NUM; syscall 0`, the `jr $31; nop`
   from gcc's epilogue) that takes an argument and returns the kernel's $v0:
   syscall 91 is GetEntryAddress(num). */
int GetEntryAddress(int num)
{
    __asm__ __volatile__("addiu $3, $0, 91\n\tsyscall 0" : : : "$3", "memory");
}

/* The member's .data: the kernel-mode alarm handler InitAlarm copies to
   0x80076000 (0x740 B of R5900 code and its tables, linked at that address,
   so the words carry no relocation), the 0x20-byte entry code it copies to
   0x82000, and the syscall table it installs: {number, handler} pairs, the
   first two handlers given, the other six looked up with GetEntryAddress. */
extern unsigned int alarm_handler[464]; /* STATIC_EXTERN: the member's .data, kept in the bins */

extern unsigned int alarm_entry[10]; /* STATIC_EXTERN: the member's .data, kept in the bins */

extern int alarm_syscalls[16]; /* STATIC_EXTERN: the member's .data, kept in the bins */

void InitAlarm(void)
{
    unsigned int i;

    if (*T3_MODE & 0x100)
        return;
    setup(alarm_syscalls[0], alarm_syscalls[1]);
    Copy((char *)0x80076000, (char *)alarm_handler, 0x740);
    Copy((char *)0x82000, (char *)alarm_entry, 0x28);
    FlushCache(0);
    FlushCache(2);
    setup(alarm_syscalls[2], alarm_syscalls[3]);
    for (i = 2; i < 8; i++) {
        setup(alarm_syscalls[i * 2], GetEntryAddress(alarm_syscalls[i * 2]));
    }
}