/* Adapted from nathanialf/ico (MIT; e0881204, sce/libkernl/glue.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libkernl.a(glue.o) */
#include "eeregs.h"
#include "eekernel.h"
#include "errno.h"
#include "signal.h"
#include "stat.h"
#include "unistd.h"
#include "libkernl_internal.h"

/* glue.o's .data: whether the tty is open (write and read open it on first
   use), then sbrk's current break, which starts at the end of the program's
   bss (the linker's _end).  Neither has a global symbol, so both are
   statics. */
static int tty_opened = 0; /* derived name */

/* static char *heap_ptr = _end; (the link's end of .bss).  The build defines
   no _end, so the variable is a stand-in and its bytes stay in the bins. */
extern char *heap_ptr; /* STATIC_EXTERN */ /* derived name */

void sceResetttyinit(void)
{
    tty_opened = 0;
}

#ifdef VERSION_JP
/* SDK 2.3: through a pointer, as VSync2 does; gcse then folds the constant
   address into the loop's load and the last store */
void VSync(void)
{
    volatile int *p;

    p = INTC_STAT;
    *p = 4;
    while ((*p & 4) == 0) {}
    *p = 4;
}
#else
/* SDK 2.5: C, with the INTC_STAT acknowledgements under DIntr */
void VSync(void)
{
    int oldi;

    oldi = DIntr();
    *INTC_STAT = 4;
    __asm__ __volatile__("sync.l");
    if (oldi) {
        EIntr();
    }
    while ((*INTC_STAT & 4) == 0) {}
    oldi = DIntr();
    *INTC_STAT = 4;
    __asm__ __volatile__("sync.l");
    if (oldi) {
        EIntr();
    }
}
#endif


#ifdef VERSION_JP
long long VSync2(void)
{
    volatile int flag;
    volatile long long val;
    volatile int *p;
    flag = 0;
    SetVSyncFlag((void *)&flag, (void *)&val);
    p = INTC_STAT;
    *p = 4;
    while ((*p & 4) == 0 && flag == 0) {}
    *p = 4;
    return val;
}
#else
/* SDK 2.5: the INTC_STAT acknowledgements run with interrupts disabled */
long long VSync2(void)
{
    volatile int flag;
    volatile long long val;
    int oldi;

    flag = 0;
    SetVSyncFlag((void *)&flag, (void *)&val);
    oldi = DIntr();
    *INTC_STAT = 4;
    __asm__ __volatile__("sync.l");
    if (oldi) {
        EIntr();
    }
    while ((*INTC_STAT & 4) == 0 && flag == 0) {}
    oldi = DIntr();
    *INTC_STAT = 4;
    __asm__ __volatile__("sync.l");
    if (oldi) {
        EIntr();
    }
    return val;
}
#endif

int write(int fd, const void *buf, int size)
{
    if (fd - 1 < 2U) {
        if (tty_opened == 0) {
            if (sceTtyInit() == 0) {
                return -1;
            }
            tty_opened = 1;
        }
        return sceTtyWrite(buf, size);
    }
    return -1;
}

int read(int fd, void *buf, int size)
{
    if (fd == 0) {
        if (tty_opened == 0) {
            if (sceTtyInit() == 0) {
                return -1;
            }
            tty_opened = 1;
        }
        return sceTtyRead(buf, size);
    }
    return -1;
}

int open(void)
{
    *(int *)__errno() = 5;
    return -1;
}

int close(int fd)
{
    return -1;
}

int ioctl(void)
{
    return -1;
}

long lseek(int fd, long offset, int whence)
{
    return -1;
}
/* C with the kernel's interrupt-disable idiom: di until Status.EIE reads clear.
   Three asm statements, not one: cc1 pads loops it counts as short with nops
   (the R5900 short-loop workaround), and it counts each asm as one insn. */
void *sbrk(int incr)
{
    char *new;
    int ie;
    int st;
    char *old;

    __asm__ __volatile__("mfc0 %0, $12" : "=r"(ie));
    ie &= 0x10000;
    if (ie) {
        do {
            __asm__ __volatile__("di");
            __asm__ __volatile__("sync.p");
            __asm__ __volatile__("mfc0 %0, $12" : "=r"(st));
        } while (st & 0x10000);
    }
    new = heap_ptr + incr;
    if ((char *)EndOfHeap() < new) {
        *(int *)__errno() = 12;
        if (ie) {
            __asm__ __volatile__("ei");
        }
        return (void *)-1;
    }
    old = heap_ptr;
    heap_ptr = new;
    if (ie) {
        __asm__ __volatile__("ei");
    }
    return old;
}





int isatty(int fd)
{
    return 1;
}

int fstat(int fd, struct stat *st)
{
    st->st_blksize = 0;
    st->st_mode = S_IFCHR;
    return 0;
}

int getpid(void)
{
    return 1;
}

int kill(int pid, int sig)
{
    if (pid == 1) {
        Exit(sig);
    }
    return 0;
}

int stat(void)
{
    *(int *)__errno() = 5;
    return -1;
}

int unlink(void)
{
    *(int *)__errno() = 5;
    return -1;
}