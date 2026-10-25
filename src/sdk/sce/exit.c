/* libkernl.a(exit.o), SDK 2.5 (KR only), written from the KR disc: Exit,
   ExecPS2, LoadExecPS2 and ExecOSD copy their arguments into the kernel's
   argument area (_kExecArg) and reset the TLB before leaving. */
#include "eekernel.h"
/* file statics named as the split names them (other libkernl members repeat the names) */
#include "libkernl_internal.h"

#define SYSCALL(num) __asm__ __volatile__("addiu $3, $0, " #num "\n\tsyscall 0" : : : "$3", "memory")

extern int *_kExecArg;
extern int exit_SysEntry[2];   /* STATIC_EXTERN (SysEntry, a name initsys.o repeats): {syscall, handler}, kept in the bins */
int strlen(const char *s);
int InitTLB(void);
int _ExecPS2(void *entry, void *gp, int argc, char **argv);
void _LoadExecPS2(const char *name, int argc, char **argv);
void _ExecOSD(int argc, char **argv);
void _Exit(int status);

/* syscall 90: Copy(dst, src, len) in kernel mode; declared int (its v0 is
   dead after the call, which decides SetArg's registers) */
static int exit_Copy(void *dst, const void *src, int len)
{
    SYSCALL(90);
}

static int exit_kCopy(char *dst, char *src, unsigned int n)
{
    unsigned int i;
    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
    return 0;
}

/* syscall 116: SetSyscall(num, handler) */
static void exit_setup(int num, int handler)
{
    SYSCALL(116);
}

static char *SetArg(const char *name, int argc, char **argv)
{
    int *argp;
    char *str;
    char *first;
    int len;
    int i;

    argp = _kExecArg;
    str = (char *)(argp + 16);
    first = str;
    exit_setup(exit_SysEntry[0], exit_SysEntry[1]);
    if (argc >= 16) {
        argc = 15;
    }
    exit_Copy(argp++, &str, 4);
    len = strlen(name) + 1;
    exit_Copy(str, name, len);
    str += len;
    for (i = 0; i < argc; i++) {
        exit_Copy(argp++, &str, 4);
        len = strlen(argv[i]) + 1;
        exit_Copy(str, argv[i], len);
        str += len;
    }
    return first;
}

static void TerminateLibrary(void)
{
    InitTLB();
}

int ExecPS2(void *entry, void *gp, int argc, char **argv)
{
    SetArg("", argc, argv);
    TerminateLibrary();
    return _ExecPS2(entry, gp, argc, (char **)(_kExecArg + 1));
}

void LoadExecPS2(const char *name, int argc, char **argv)
{
    char *s = SetArg(name, argc, argv);
    TerminateLibrary();
    _LoadExecPS2(s, argc, (char **)(_kExecArg + 1));
}

void Exit(int status)
{
    TerminateLibrary();
    _Exit(status);
}

void ExecOSD(int argc, char **argv)
{
    SetArg("", argc, argv);
    TerminateLibrary();
    _ExecOSD(argc, (char **)(_kExecArg + 1));
}
