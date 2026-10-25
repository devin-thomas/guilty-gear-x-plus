#include "generated/graph.h"

void MakeFrameBuffer(void) {
}


void BackColor(u_int rgb)
{
    back_color = rgb;
}

void PacketBegin(void);
void PacketEnd(int wait);

void GraphBegin(void)
{
    PacketBegin();
    MakeFullBuffer();
    PacketEnd(1);
    PacketBegin();
}

int sceGsSyncPath(int mode, u_short timeout);
int sceGsSyncV(int mode);

void GraphEnd(void)
{
    PacketEnd(1);
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    PacketBegin();
    GraphConv();
    PacketEnd(1);
    sceGsSyncPath(0, 0);
    frame++;
}

int abs(int x);

void GraphSetOffset(int x, int y)
{
    dispoffset.sizey = 0x1BF - abs(y);
    dispoffset.x = x;
    dispoffset.sizex = (640 - abs(x)) * 4 - 1;
    dispoffset.y = y;
    if (x > 0) {
        disp.display.DX = x * 4;
    } else {
        disp.display.DX = 0;
    }
    if (y > 0) {
        disp.display.DY = y;
    } else {
        disp.display.DY = 0;
    }
}

sceDmaChan *sceDmaGetChan(int id);

void dma_chcr_tte_set(int channel, int flag)
{
    sceDmaGetChan(channel)->chcr.TTE = (flag == 1);
}

u_long *PacketDmaTagCnt(int id);
u_long *PacketPackRegWithTag(int id, u_long reg, u_long data);
u_long *PacketGifTag(int id, u_long tag, u_long taglist);
void PacketPointerSet(int id, u_long *ptr);

void MakeFullBuffer(void)
{
    u_long *ptr;

    PacketDmaTagCnt(0);
    PacketPackRegWithTag(0, 0x4C, 0xA008C);
    PacketPackRegWithTag(0, 0x40, 0x01BF0000027F0000UL);
    PacketPackRegWithTag(0, 0x42, 0x80000000A8UL);
    ptr = PacketGifTag(0, 0x4400000000000001UL, 0x5510);
    *ptr++ = 0x146;
    *ptr++ = back_color;
    *ptr++ = 0x79006C00;
    *ptr++ = 0x95009400;
    PacketPointerSet(0, ptr);
}

sceDmaEnv *sceDmaGetEnv(sceDmaEnv *env);
int sceDmaPutEnv(sceDmaEnv *env);

void dma_notify_channel_set(int channel)
{
    sceDmaEnv dma_env;

    sceDmaGetEnv(&dma_env);
    dma_env.notify = 1 << channel;
    sceDmaPutEnv(&dma_env);
}

u_long *PacketPackRegsWithTag(int id, u_long *reglist, int num);

void GraphConv(void)
{
    u_long reg[8] = {
        0x50, 0xA0000000A1180UL,
        0x51, 0,
        0x52, 0x1C000000280UL,
        0x53, 2,
    };
    int sx;
    int sy;
    int sizex;
    int sizey;

    if (dispoffset.x >= 0) {
        sx = 0;
        sizex = 640;
    } else {
        sx = -dispoffset.x;
        sizex = dispoffset.x + 640;
    }
    sy = dispoffset.y;
    if (sy >= 0) {
        sy = 0;
        sizey = 448;
    } else {
        sizey = sy + 448;
        sy = -sy;
    }
    reg[5] = sizex | ((u_long)sizey << 32);
    reg[3] = sx | ((u_long)sy << 16);
    PacketDmaTagCnt(3);
    PacketPackRegsWithTag(3, reg, 4);
    PacketPackRegWithTag(3, 0x42, 0x8000000064UL);
}

#define G_DISPLAY(dx, dy, magh, magv, dw, dh) ((u_long)(dx) | ((u_long)(dy) << 12) | ((u_long)(magh) << 23) | ((u_long)(magv) << 27) | ((u_long)(dw) << 32) | ((u_long)(dh) << 44))

void GS_runtime_preset(void)
{
    *(volatile u_long *)0x12000000 = 0x7F27;
    *(volatile u_long *)0x12000070 = 0x1400;
    *(volatile u_long *)0x12000090 = 0x1400;
    *(volatile u_long *)0x12000080 = G_DISPLAY(disp.display.DX + 636, disp.display.DY + 50, 3, 0, dispoffset.sizex, dispoffset.sizey);
    *(volatile u_long *)0x120000A0 = G_DISPLAY(disp.display.DX + 636, disp.display.DY + 51, 3, 0, dispoffset.sizex, dispoffset.sizey);
    *(volatile u_long *)0x120000D0 = 0;
}

int sprintf(char *buf, const char *fmt, ...);
int sceOpen(const char *name, int flags, ...);
int sceWrite(int fd, const void *buf, int nbyte);
int sceClose(int fd);

void dbg_frame_save(void)
{
    char file_name[256];
    int discripter;

    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    grp_except_store_image((void *)0x3000000, 0, 0, 640, 0, 0, 640, 448);
    sprintf(file_name, "host0:test%08d.raw", 0);
    discripter = sceOpen(file_name, 0x202);
    sceWrite(discripter, (void *)0x3000000, 0x118000);
    sceClose(discripter);
}

void sceDevVif0Reset(void);
void sceDevVu0Reset(void);
void sceGsResetPath(void);
void sceDmaReset(int mode);
void sceDevGifPutImtMode(int mode);
void PacketInit(void);
void sceGsResetGraph(short mode, short inter, short omode, short ffmode);
void sceGsSetDefDispEnv(sceGsDispEnv *disp, short psm, short w, short h, short dx, short dy);
void sceGsSetDefDrawEnv(sceGsDrawEnv1 *draw, short psm, short w, short h, short ztest, short zpsm);
void PacketSendNow(int id);
void sceGsPutDispEnv(sceGsDispEnv *disp);

void GraphInit(void)
{
    sceDmaEnv env;

    sceDevVif0Reset();
    sceDevVu0Reset();
    sceGsResetPath();
    sceDmaReset(1);
    sceDevGifPutImtMode(0);
    PacketInit();
    sceDmaGetEnv(&env);
    env.notify = 2;
    sceDmaPutEnv(&env);
    sceGsResetGraph(0, 1, 2, 0);
    sceGsSetDefDispEnv(&disp, 0, 640, 448, 0, 0);
    sceGsSetDefDrawEnv(&draw, 0, 640, 224, 0, 0);
    BackColor(0);
    PacketBegin();
    PacketDmaTagCnt(0);
    PacketPackRegWithTag(0, 0x4C, *(u_long *)&draw.frame1);
    PacketPackRegWithTag(0, 0x4E, *(u_long *)&draw.zbuf1);
    PacketPackRegWithTag(0, 0x18, *(u_long *)&draw.xyoffset1);
    PacketPackRegWithTag(0, 0x40, *(u_long *)&draw.scissor1);
    PacketPackRegWithTag(0, 0x1A, *(u_long *)&draw.prmodecont);
    PacketPackRegWithTag(0, 0x08, 0);
    PacketPackRegWithTag(0, 0x46, *(u_long *)&draw.colclamp);
    PacketPackRegWithTag(0, 0x45, *(u_long *)&draw.dthe);
    PacketPackRegWithTag(0, 0x47, *(u_long *)&draw.test1);
    PacketPackRegWithTag(0, 0x42, 0x44);
    PacketPackRegWithTag(0, 0x49, 0);
    PacketPackRegWithTag(0, 0x3B, 0x8000000000UL);
    PacketPackRegWithTag(0, 0x4A, 0);
    PacketSendNow(0);
    GraphBegin();
    GraphEnd();
    FlushCache(0);
    sceGsPutDispEnv(&disp);
    frame = 0;
}

void sceGsSetDefStoreImage(sceGsStoreImage *sp, short sbp, short sbw, short spsm, short x, short y, short w, short h);
int sceGsExecStoreImage(sceGsStoreImage *sp, u_long128 *dstaddr);

void grp_except_store_image(void *distance_address, int source_format, int source_address, int source_bw, int source_x, int source_y, int source_w, int source_h)
{
    sceGsStoreImage sp;
    int i;
    int k = 256;
    int w = source_w;
    /* REGALLOC_STANDIN: taking &source_format renumbers its pseudo (gen_mem_addressof, purged later),
       so gcse's PRE creates the format<<16 reg after x<<16 and reload gives it the higher spill slot. */
    int fmt = *(int *)&source_format;

    for (i = 0; i < source_h; i += 64) {
        sceGsSetDefStoreImage(&sp, source_address / 256, source_bw / 64, fmt, source_x, source_y, w,
                              (source_h - i > 64) ? 64 : source_h - i);
        FlushCache(0);
        sceGsExecStoreImage(&sp, distance_address);
        source_address += source_bw * k;
        distance_address = (char *)distance_address + w * k;
    }
    dma_notify_channel_set(1);
    dma_chcr_tte_set(1, 1);
}
