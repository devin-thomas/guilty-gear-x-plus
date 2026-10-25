/* Adapted from nathanialf/ico (MIT; e0881204, sce/libdma/libdma.c): SDK 2.2 source checked against GGX's SDK 2.3 (JP) and 2.5 (KR). */
/* libdma.a. SDK 2.3 (JP) keeps the 2.2 channel calls below; SDK 2.5 (KR) rewrites them.  The archive's member boundaries in this build are not known,
 * so this file is the archive's whole run. */
#include "stdio.h"
#include "eeregs.h"
#include "libdma.h"
#include "libvu0_internal.h"

extern DmaChan *dch[10]; /* DATA_EXTERN: defined with an initializer in the original; bytes kept in the bins */


extern int sceDmaDebugMode; /* DATA_EXTERN: defined with an initializer in the original; bytes kept in the bins */


/* libdma.a's build stamp, exactly sixteen characters with no terminator. */
#ifdef VERSION_JP
static char sceDmaVersion[16] = "PsIIlibdma  2300"; /* derived name */
#else
static char sceDmaVersion[16] = "PsIIlibdma  2500"; /* derived name */
#endif

/* Which channels sceDmaReset clears: all but the three SIF channels. */
static int resetChan[10] = {1, 1, 1, 1, 1, 0, 0, 0, 1, 1}; /* derived name */

/* sceDmaPutEnv's lookups from the DmaEnv's first three bytes to the D_CTRL
 * MFD, STS and STD field codes. */
static unsigned char mfdCode[16] = {0, 0, 0, 3, 0, 1, 0, 0, 2}; /* derived name */

static unsigned char stsCode[16] = {0, 1, 2, 0, 0, 0, 3}; /* derived name */

static unsigned char stdCode[16] = {0, 2, 3}; extern DmaEnv sceDmaCurrentEnv; /* DATA_EXTERN: defined with an initializer in the original; bytes kept in the bins */


int scePrintf(const char *fmt, ...);
int DIntr(void);
int EIntr(void);
int sceDmaPause(DmaChan *ch);

/* D_CHCR's fields, as in Sony's public eeregs.h */
typedef struct {
    unsigned int DIR : 1;
    unsigned int p0 : 1;
    unsigned int MOD : 2;
    unsigned int ASP : 2;
    unsigned int TTE : 1;
    unsigned int TIE : 1;
    unsigned int STR : 1;
    unsigned int p1 : 7;
    unsigned int TAG : 16;
} tD_CHCR;

#ifdef VERSION_KR
/* SDK 2.5: the channel calls share a bounded wait that pauses a stuck
   channel, and addresses in the scratchpad's 0x7xxxxxxx alias are mapped
   to 0x8xxxxxxx */
static void WaitDma(DmaChan *ch)
{
    int n = 0x1000000;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            scePrintf("libdma: sync timeout\n");
            sceDmaPause(ch);
        }
    }
}

static void WatchDma(DmaChan *ch, unsigned int addr, int n)
{
    int cnt = n ? n : 0x1000000;

    while (*(volatile unsigned int *)&ch->madr < addr) {
        if (--cnt < 0) {
            scePrintf("libdma: sync timeout\n");
            sceDmaPause(ch);
        }
    }
}

static unsigned int CheckAddress(unsigned int addr)
{
    if ((addr >> 28) == 7) {
        addr &= 0x0FFFFFFF;
        addr |= 0x80000000;
    }
    return addr;
}
#endif

void memclr(void *p, int n)
{
    unsigned char *c = p;
    int i;
    for (i = n - 1; i != -1; i--) {
        *c++ = 0;
    }
}



DmaChan *sceDmaGetChan(unsigned int id)
{
    if (id < 0xA) {
        return dch[id];
    }
    return 0;
}



int sceDmaReset(int mode)
{
    DmaEnv env;
    int old;
    int i;

    old = *D_CTRL & 1;
    for (i = 0; i < 10; i++) {
        if (resetChan[i] != 0) {
            int *ch = (int *)dch[i];
            ch[0x00 / 4] = 0;
            ch[0x30 / 4] = 0;
            ch[0x10 / 4] = 0;
            ch[0x50 / 4] = 0;
            ch[0x40 / 4] = 0;
            ch[0x80 / 4] = 0;
        }
    }
    *D_STAT = 0xFF1F;
    *(int *)D_STAT = *D_STAT & 0xFF1F0000;
    memclr(&env, 0x14);
    sceDmaPutEnv(&env);
    if (mode == 1) {
        *D_CTRL = *D_CTRL | 1;
    }
    return old;
}

#ifdef VERSION_JP


int sceDmaDebug(int mode)
{
    int old = sceDmaDebugMode;
    sceDmaDebugMode = mode;
    return old;
}
#endif



int sceDmaPutEnv(DmaEnv *env)
{
    int ctrl = *D_CTRL;
    int pcr = *D_PCR;
    int sqwc = *D_SQWC;
    int rbor = *D_RBOR;
    int rbsr = *D_RBSR;

    if (env->mfd >= 10) {
        return -1;
    }
    if (env->sts >= 10) {
        return -2;
    }
    if (env->std >= 10) {
        return -3;
    }
    if (env->rcyc >= 7) {
        return -4;
    }
    ctrl = (ctrl & 0xFFFFFFCF) | (mfdCode[env->mfd] << 4);
    ctrl = (ctrl & 0xFFFFFF3F) | (stsCode[env->sts] << 6);
    ctrl = (ctrl & 0xFFFFFFF3) | (stdCode[env->std] << 2);
    if (env->rcyc != 0) {
        ctrl |= 2;
        ctrl = (ctrl & 0xFFFFFCFF) | ((env->rcyc - 1) << 8);
    } else {
        ctrl &= 0xFFFFFFFD;
    }
    pcr = (env->cde << 16) | env->cpc;
    sqwc = (env->tqwc << 16) | env->sqwc;
    rbor = (int)env->rbadr;
    rbsr = env->rbsize;
    *D_CTRL = ctrl;
    *D_PCR = pcr;
    *D_SQWC = sqwc;
    *D_RBOR = rbor;
    *D_RBSR = rbsr;
    sceDmaCurrentEnv = *env;
    return 0;
}



DmaEnv *sceDmaGetEnv(DmaEnv *env)
{
    *env = sceDmaCurrentEnv;
    return env;
}

#ifdef VERSION_JP


/* 0x1000E060 is the DMAC stall address register (D_STADR). */
int sceDmaPutStallAddr(unsigned int addr)
{
    int old = *D_STADR;

    if (addr != 0xFFFFFFFF) {
        *D_STADR = addr;
    }
    return old;
}



void sceDmaSend(DmaChan *ch, unsigned int addr)
{
    int n = 0x1000000;
    int v;
    int t;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    if ((unsigned int)ch->tadr != 0xFFFFFFFF) {
        ch->tadr = addr;
    }
    ch->qwc = 0;
    ch->chcr = (ch->chcr & ~0xC) | 0x105;
}



void sceDmaSendN(DmaChan *ch, unsigned int addr, int size)
{
    int n = 0x1000000;
    int v;
    int t;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    ch->chcr = (ch->chcr & ~0xC) | 0x101;
}



void sceDmaSendI(DmaChan *ch, unsigned int addr, int size)
{
    int n = 0x1000000;
    int v;
    int t;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    ch->chcr = (ch->chcr & ~0xC) | 0x109;
}



void sceDmaRecv(DmaChan *ch)
{
    int n = 0x1000000;
    int v;
    int t;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    ch->qwc = 0;
    ch->chcr = ((((ch->chcr & ~0xC) | 4) & ~1) | 0x100);
}



void sceDmaRecvN(DmaChan *ch, unsigned int addr, int size)
{
    int n = 0x1000000;
    int v;
    int t;
    int c;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    c = ch->chcr;
    c &= ~0xC;
    c &= ~1;
    c |= 0x100;
    ch->chcr = c;
}



void sceDmaRecvI(DmaChan *ch, unsigned int addr, int size)
{
    int n = 0x1000000;
    int v;
    int t;

    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    ch->chcr = (((ch->chcr & ~0xC) | 8) & ~1) | 0x100;
}



int sceDmaSync(DmaChan *ch, int mode, int n)
{
    int v;
    int t;

    if (mode == 1) {
        return ((unsigned int)ch->chcr >> 8) & 1;
    }
    if (n == 0) {
        n = 0x1000000;
    }
    while (ch->chcr & 0x100) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    return 0;
}



int sceDmaWatch(DmaChan *ch, unsigned int addr, int mode, int n)
{
    int v;
    int t;

    if (mode == 1) {
        return *(volatile unsigned int *)&ch->madr < addr;
    }
    if (n == 0) {
        n = 0x1000000;
    }
    while (*(volatile unsigned int *)&ch->madr < addr) {
        if (--n < 0) {
            printf("libdma: sync timeout\n");
            v = ch->chcr;
            if (((unsigned int)v >> 8) & 1) {
                do {
                    t = v & ~0x100;
                    v = t;
                } while (((unsigned int)v >> 8) & 1);
                ch->chcr = t;
            }
        }
    }
    return 0;
}

int sceDmaPause(DmaChan *ch)
{
    unsigned int v;

    if (*D_ENABLER == 0) {
        *D_ENABLEW = 0x1000;
    }
    v = ch->chcr;
    ch->chcr = v & ~0x100;
    *D_ENABLEW = 0;
    return v;
}
#else
unsigned int sceDmaPutStallAddr(unsigned int addr)
{
    unsigned int old = *D_STADR;
    *D_STADR = addr;
    return old;
}

void sceDmaSend(DmaChan *ch, unsigned int addr)
{
    addr = CheckAddress(addr);
    WaitDma(ch);
    if ((unsigned int)ch->tadr != 0xFFFFFFFF) {
        ch->tadr = addr;
    }
    ch->qwc = 0;
    ch->chcr = (ch->chcr & ~0xC) | 0x105;
}

void sceDmaSendN(DmaChan *ch, unsigned int addr, int size)
{
    addr = CheckAddress(addr);
    WaitDma(ch);
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    ch->chcr = (ch->chcr & ~0xC) | 0x101;
}

void sceDmaSendI(DmaChan *ch, unsigned int addr, int size)
{
    addr = CheckAddress(addr);
    WaitDma(ch);
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    ch->chcr = (ch->chcr & ~0xC) | 0x109;
}

void sceDmaRecv(DmaChan *ch)
{
    WaitDma(ch);
    ch->qwc = 0;
    ch->chcr = (((ch->chcr & ~0xC) | 4) & ~1) | 0x100;
}

void sceDmaRecvN(DmaChan *ch, unsigned int addr, int size)
{
    addr = CheckAddress(addr);
    WaitDma(ch);
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    {
        tD_CHCR c = *(tD_CHCR *)&ch->chcr;

        c.MOD = 0;
        c.DIR = 0;
        c.STR = 1;
        *(tD_CHCR *)&ch->chcr = c;
    }
}

void sceDmaRecvI(DmaChan *ch, unsigned int addr, int size)
{
    addr = CheckAddress(addr);
    WaitDma(ch);
    if ((unsigned int)ch->madr != 0xFFFFFFFF) {
        ch->madr = addr;
    }
    ch->qwc = size;
    ch->chcr = (((ch->chcr & ~0xC) | 8) & ~1) | 0x100;
}

int sceDmaSync(DmaChan *ch, int mode, int n)
{
    if (mode == 1) {
        return ((unsigned int)ch->chcr >> 8) & 1;
    }
    WaitDma(ch);
    return 0;
}

int sceDmaWatch(DmaChan *ch, unsigned int addr, int mode, int n)
{
    if (mode == 1) {
        return (unsigned int)ch->madr < addr;
    }
    WatchDma(ch, addr, n);
    return 0;
}

int sceDmaPause(DmaChan *ch)
{
    int oldi = DIntr();
    unsigned int enabler = *D_ENABLER;
    unsigned int chcr;

    if (!(enabler & 0x10000)) {
        *D_ENABLEW = enabler | 0x10000;
    }
    *D_CTRL;
    chcr = ch->chcr;
    ch->chcr = chcr & ~0x100;
    *D_ENABLEW = enabler;
    if (oldi) {
        EIntr();
    }
    return chcr;
}
#endif

int sceDmaRestart(DmaChan *ch, int chcr)
{
    int v = ch->chcr;
    ch->chcr = chcr;
    return ((unsigned int)v >> 8) & 1;
}