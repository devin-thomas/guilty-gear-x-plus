#include "generated/packet.h"

void packetLastCheck(DMA_HANDLE *h)
{
    if (h->tagptr) {
        *h->tagptr += (u_int)((char *)h->ptr - (char *)h->tagptr - 8) >> 4;
        h->tagptr = 0;
    }
}

u_long *PacketGifTag(int id, u_long tag, u_long taglist)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    *p->ptr++ = tag;
    *p->ptr++ = taglist;
    return p->ptr;
}

u_long *PacketPackRegWithTag(int id, u_long reg, u_long data)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    *p->ptr++ = 0x1000000000000001UL;
    *p->ptr++ = 0xE;
    *p->ptr++ = data;
    *p->ptr++ = reg;
    return p->ptr;
}

u_long *PacketPackReg(int id, u_long reg, u_long data)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    *p->ptr++ = data;
    *p->ptr++ = reg;
    return p->ptr;
}

u_long *PacketPointerGet(int id)
{
    return handle_list[id]->ptr;
}

void PacketPointerSet(int id, u_long *ptr)
{
    handle_list[id]->ptr = ptr;
}

u_long *PacketCopy(int id, u_long *data, int size)
{
    int i;
    DMA_HANDLE *p;

    p = handle_list[id];
    for (i = 0; i < size / 8; i++) {
        *p->ptr++ = *data++;
    }
    return p->ptr;
}

u_long *PacketPackRegsWithTag(int id, u_long *reglist, int num)
{
    DMA_HANDLE *p;
    int i;

    p = handle_list[id];
    *p->ptr++ = num | 0x1000000000000000UL;
    *p->ptr++ = 0xE;
    for (i = 0; i < num; i++) {
        *p->ptr++ = reglist[1];
        *p->ptr++ = reglist[0];
        reglist += 2;
    }
    return p->ptr;
}

void PacketBegin(void)
{
    dma_f->ptr = dma_f->data[dma_f->page];
    dma_1->ptr = dma_1->data[dma_1->page];
    dma_2->ptr = dma_2->data[dma_2->page];
    dma_e->ptr = dma_e->data[dma_e->page];
    dma_f->tagptr = 0;
    dma_1->tagptr = 0;
    dma_2->tagptr = 0;
    dma_e->tagptr = 0;
}

sceDmaChan *sceDmaGetChan(int id);
int sceDmaSync(sceDmaChan *d, int mode, int timeout);
void sceDmaSend(sceDmaChan *d, void *tag);
void FlushCache(int operation);

void PacketDmaSpr(void *spr)
{
    sceDmaChan *dc;

    dc = sceDmaGetChan(2);
    sceDmaSync(dc, 0, 0);
    FlushCache(0);
    sceDmaSend(dc, (void *)((u_int)spr | 0x80000000));
}

void sceGsSyncPath(int mode, unsigned short timeout);

void PacketSendIt(u_long *adr)
{
    sceDmaChan *dc;

    sceGsSyncPath(0, 0);
    dc = sceDmaGetChan(2);
    sceDmaSync(dc, 0, 0);
    FlushCache(0);
    sceDmaSend(dc, adr);
    sceGsSyncPath(0, 0);
}

u_long *PacketDmaTagRef(int id, void *adr, int size)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    packetLastCheck(p);
    if ((u_int)p->ptr & 0xF) {
        p->ptr++;
    }
    *p->ptr++ = (size / 16) | 0x20000000 | ((u_long)(u_int)adr << 32);
    *p->ptr++ = 0;
}

u_long *PacketDmaTagEnd(int id, int size)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    packetLastCheck(p);
    if ((u_int)p->ptr & 0xF) {
        p->ptr++;
    }
    *p->ptr++ = (size / 16) | 0x70000000;
    *p->ptr++ = 0;
    return p->ptr;
}

u_long *PacketDmaTagRefs(int id, void *adr, int size)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    packetLastCheck(p);
    if ((u_int)p->ptr & 0xF) {
        p->ptr++;
    }
    *p->ptr++ = (size / 16) | 0x40000000 | ((u_long)(u_int)adr << 32);
    *p->ptr++ = 0;
    return p->ptr;
}

void *malloc(int size);

void PacketInit(void)
{
    sceDmaChan *dc;

    dc = sceDmaGetChan(2);
    dc->chcr.TTE = 0;
    dma_f->dma_chan = dc;
    dma_1->dma_chan = dc;
    dma_2->dma_chan = dc;
    dma_e->dma_chan = dc;
    dma_f->data[0] = malloc(0x20000);
    dma_1->data[1] = malloc(0x10000);
    dma_2->data[1] = malloc(0x80000);
    dma_e->data[0] = malloc(0x10000);
    dma_f->data[1] = malloc(0x20000);
    dma_1->data[0] = malloc(0x10000);
    dma_2->data[0] = malloc(0x80000);
    dma_e->data[1] = malloc(0x10000);
    dma_f->page = 0;
    dma_1->page = 0;
    dma_2->page = 0;
    dma_e->page = 0;
    handle_list[0] = dma_f;
    handle_list[1] = dma_1;
    handle_list[2] = dma_2;
    handle_list[3] = dma_e;
}

void PacketSendNow(int id)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    if (p->ptr != p->data[p->page]) {
        PacketDmaTagEnd(id, 16);
        PacketGifTag(id, 0x8000, 0);
        sceDmaSync(p->dma_chan, 0, 0);
        FlushCache(0);
        sceDmaSend(p->dma_chan, p->data[p->page]);
        p->page ^= 1;
        p->ptr = p->data[p->page];
        p->tagptr = 0;
    }
}

void PacketEnd(int wait)
{
    int i;
    DMA_HANDLE *p;

    for (i = 0; i < 4; i++) {
        p = handle_list[i];
        if (p->ptr != p->data[p->page]) {
            PacketDmaTagEnd(i, 16);
            PacketGifTag(i, 0x8000, 0);
            sceDmaSync(p->dma_chan, 0, 0);
            FlushCache(0);
            sceDmaSend(p->dma_chan, p->data[p->page]);
            if (wait) {
                sceDmaSync(p->dma_chan, 0, 0);
            }
            p->page ^= 1;
            p->ptr = p->data[p->page];
            p->tagptr = 0;
        }
    }
}

u_long *PacketDmaTagCnt(int id)
{
    DMA_HANDLE *p;

    p = handle_list[id];
    packetLastCheck(p);
    if ((u_int)p->ptr & 0xF) {
        p->ptr++;
    }
    p->tagptr = p->ptr;
    *p->ptr++ = 0x10000000;
    *p->ptr++ = 0;
    return p->ptr;
}
