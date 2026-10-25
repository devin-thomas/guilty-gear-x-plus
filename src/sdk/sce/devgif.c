/* libdev.a(devgif.o): GIF control, written from the GGX discs (the same code
   on SDK 2.3 and 2.5). Register names follow Sony's public eeregs.h. */
#include "eeregs.h"

typedef unsigned int u_int;
typedef int u_long128 __attribute__((mode(TI)));

#define GIFR(off) (*(volatile u_int *)(0x10003000 + (off)))
#define GIF_MODE_R GIFR(0x10)
#define GIF_FIFO_R (*(volatile u_long128 *)0x10006000)

typedef struct {
    u_int tag0;
    u_int tag1;
    u_int tag2;
    u_int tag3;
    u_int stat;
    u_int count;
    u_int p3count;
    u_int p3tag;
} sceDevGifCnd;

int sceDevGifSync(void);
u_int sceDevGifGetImtMode(void);

void sceDevGifReset(void)
{
    GIFR(0x00) = 1;
}

int sceDevGifPause(void)
{
    GIFR(0x00) = 8;
    return 1;
}

int sceDevGifContinue(void)
{
    if (sceDevGifSync() != 1) {
        return 0;
    }
    GIFR(0x00) = 0;
    return 1;
}

/* both PutImtMode and PutP3msk read the mode back through GetImtMode, as
   Sony's code does */
void sceDevGifPutImtMode(int mode)
{
    if (mode == 0) {
        GIF_MODE_R = sceDevGifGetImtMode() & ~4;
    } else if (mode == 1) {
        GIF_MODE_R = sceDevGifGetImtMode() | 4;
    }
}

u_int sceDevGifGetImtMode(void)
{
    return (GIFR(0x20) & 4) >> 2;
}

int sceDevGifPutP3msk(int enable)
{
    if (enable == 0) {
        GIF_MODE_R = sceDevGifGetImtMode() & ~1;
    } else if (enable == 1) {
        GIF_MODE_R = sceDevGifGetImtMode() | 1;
    }
    return 1;
}

int sceDevGifGetP3msk(void)
{
    return sceDevGifGetImtMode() & 1;
}

int sceDevGifSync(void)
{
    u_int stat = GIFR(0x20);

    if (stat & 8) {
        return 1;
    }
    return (stat & 0xFC0) ? 2 : 0;
}

int sceDevGifGetCnd(sceDevGifCnd *cnd)
{
    if (sceDevGifSync() == 2) {
        return 0;
    }
    cnd->tag0 = GIFR(0x40);
    cnd->tag1 = GIFR(0x50);
    cnd->tag2 = GIFR(0x60);
    cnd->tag3 = GIFR(0x70);
    cnd->stat = GIFR(0x20);
    cnd->count = GIFR(0x80);
    cnd->p3count = GIFR(0x90);
    cnd->p3tag = GIFR(0xA0);
    return 1;
}

int sceDevGifPutFifo(u_long128 *addr, int n)
{
    int i = 0;

    while (n--) {
        if (GIFR(0x20) & 0x10000000) {
            return i;
        }
        GIF_FIFO_R = *addr++;
        i++;
    }
    return i;
}