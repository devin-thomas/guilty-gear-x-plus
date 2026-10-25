/* libdev.a(devvif0.o): VIF0 control, written from the GGX discs (the same
   code on SDK 2.3 and 2.5). Register offsets follow Sony's public eeregs.h. */
typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef int u_long128 __attribute__((mode(TI)));

#define VIF0R(off) (*(volatile u_int *)(0x10003800 + (off)))
#define VIF0_FIFO_R (*(volatile u_long128 *)0x10004000)

typedef struct {
    u_int row[4];
    u_int col[4];
    u_int mask;
    u_int code;
    u_int stat;
    u_short itop;
    u_short itops;
    u_short mark;
    u_short num;
    u_char err;
    u_char cl;
    u_char wl;
    u_char cmod;
} sceDevVif0Cnd;

u_int sceDevVif0Sync(void);

void sceDevVif0Reset(void)
{
    VIF0R(0x10) = 1;
    VIF0R(0x20) = 6;
}

int sceDevVif0Pause(int mode)
{
    u_int s = sceDevVif0Sync();

    if (s == 0) {
        return 0;
    }
    if (s == 1) {
        return 0;
    }
    if (mode == 0) {
        VIF0R(0x10) = 2;
    } else if (mode == 1) {
        VIF0R(0x10) = 4;
    }
    return 1;
}

int sceDevVif0Continue(void)
{
    if (sceDevVif0Sync() != 1) {
        return 0;
    }
    VIF0R(0x10) = 8;
    return 1;
}

int sceDevVif0PutErr(int interrupt, int miss1, int miss2)
{
    int err = interrupt | (miss1 << 1) | (miss2 << 2);

    VIF0R(0x20) = err;
    return err;
}

u_int sceDevVif0GetErr(void)
{
    return VIF0R(0x20);
}

u_int sceDevVif0Sync(void)
{
    u_int stat = VIF0R(0x00);

    if (!(stat & 3)) {
        return 0;
    }
    if (stat & 0x700) {
        return 1;
    }
    if (stat & 0xC) {
        return 2;
    }
    return (stat & 0x3000) ? 3 : 4;
}

int sceDevVif0GetCnd(sceDevVif0Cnd *cnd)
{
    u_int cycle = VIF0R(0x40);

    if (sceDevVif0Sync() >= 2) {
        return 0;
    }
    cnd->row[0] = VIF0R(0x100);
    cnd->row[1] = VIF0R(0x110);
    cnd->row[2] = VIF0R(0x120);
    cnd->row[3] = VIF0R(0x130);
    cnd->col[0] = VIF0R(0x140);
    cnd->col[1] = VIF0R(0x150);
    cnd->col[2] = VIF0R(0x160);
    cnd->col[3] = VIF0R(0x170);
    cnd->mask = VIF0R(0x70);
    cnd->code = VIF0R(0x80);
    cnd->stat = VIF0R(0x00);
    cnd->itop = VIF0R(0xD0);
    cnd->itops = VIF0R(0x90);
    cnd->mark = VIF0R(0x30);
    cnd->num = VIF0R(0x60);
    cnd->err = VIF0R(0x20);
    cnd->cl = cycle;
    cnd->wl = cycle >> 8;
    cnd->cmod = VIF0R(0x50);
    return 1;
}

int sceDevVif0PutFifo(u_long128 *addr, int n)
{
    int i = 0;

    while (n--) {
        if (VIF0R(0x00) & 0x10000000) {
            return i;
        }
        VIF0_FIFO_R = *addr++;
        i++;
    }
    return i;
}