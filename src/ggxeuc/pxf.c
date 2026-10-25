#include "generated/pxf.h"

static PXF_BUF pxf_work[1280];

void PXF_PalReset(void)
{
    NextPalTex = 0;
}

void PXF_Init(void)
{
    int i;

    VramTop = 0x2362;
    VramPage = 0;
    for (i = 1279; i >= 0; i--) {
        pxf_work[i].used = 0;
    }
}

u_char *read_clut32(u_char *out, u_char *in, int num)
{
    int i;
    u_int *inl;
    u_int *outl;

    inl = (u_int *)in;
    outl = (u_int *)out;
    for (i = 0; i < num; i++) {
        *outl++ = *inl++;
    }
    return (u_char *)outl;
}

u_char *read_clut16(u_char *out, u_char *in, int num)
{
    int i;
    u_int *inl;
    u_int *outl;

    inl = (u_int *)in;
    outl = (u_int *)out;
    for (i = 0; i < num / 2; i++) {
        *outl++ = *inl++;
    }
    return (u_char *)outl;
}

void debugf(char *format, ...);

int vram_sort(int num)
{
    int i;
    int wvram;
    PXF_BUF *spr;
    int j;
    u_long *s;
    u_long *d;

    debugf("vram_sort called\n");
    wvram = 0;
    for (i = 0; i < num; i++) {
        spr = &pxf_work[vramsorted[i]];
        if (wvram < spr->vram) {
            s = &PxfVRAM[spr->vram];
            d = &PxfVRAM[wvram];
            for (j = 0; j < spr->wvramsize; j++) {
                *d++ = *s++;
            }
            spr->vram = wvram;
            wvram += spr->wvramsize;
        }
    }
    return wvram;
}

void PXF_Transfer(void)
{
    PXF_BUF *spr;
    int i;
    int j;
    int k;
    int wr;
    int vram;
    PXF_BUF *p;

    wr = 0;
    for (i = 0; i < vramsortnum; i++) {
        spr = &pxf_work[vramsorted[i]];
        if (spr->vram < 0) {
            continue;
        }
        if (spr->used) {
            spr->vrampage = -1;
            vramsorted[wr++] = vramsorted[i];
            spr->used = 0;
        }
    }
    spr = pxf_work;
    for (i = 0; i < 1280; i++, spr++) {
        if (spr->used == 0) {
            continue;
        }
        if (spr->mode & 0xF000) {
            vram = 0;
            for (j = 0; j < wr; j++) {
                p = &pxf_work[vramsorted[j]];
                if (p->vram - vram >= spr->wvramsize) {
                    for (k = wr; k > j; k--) {
                        vramsorted[k] = vramsorted[k - 1];
                    }
                    break;
                }
                vram = p->vram + p->wvramsize;
            }
            if (vram + spr->wvramsize > 0x60000) {
                vram = vram_sort(wr);
                if (vram + spr->wvramsize > 0x60000) {
                    debugf("VRAM OVERFLOW\n");
                    spr->vram = -1;
                    continue;
                }
            }
            if ((int)spr->src >= 4) {
                wr++;
                PXF_Uncompress((u_char *)&PxfVRAM[vram], spr->src);
                spr->vram = vram;
                vramsorted[j] = i;
            } else {
                spr->vram = -1;
            }
        } else {
            spr->vram = 0;
        }
        spr->used = 0;
        spr->vrampage = -1;
    }
    vramsortnum = wr;
    VramTop = 0x2362;
    VramPage = 0;
}

u_long PXF_SetTexture(int id)
{
    PXF_BUF *p;
    u_long tex;

    p = &pxf_work[id];
    if ((int)p->src < 5) {
        debugf("No Source File %d %x(%x,%d,%d)\n", id, p->src, p->mode, p->sizex, p->sizey);
        p->tex = 0x444004000;
        return -1;
    }
    p->used++;
    tex = NextPalTex;
    if (tex) {
        return tex;
    }
    if (p->palid < 0) {
        return 0;
    }
    return (u_long)p->paltex << 37;
}

void PXF_GetSize(int id, int *x, int *y)
{
    *x = pxf_work[id].sizex;
    *y = pxf_work[id].sizey;
}

u_long *PacketDmaTagCnt(int id);
u_long *PacketDmaTagRefs(int id, void *adr, int size);
void PacketPointerSet(int id, u_long *ptr);
void PacketSendNow(int id);

void PXF_Entry(u_char *src, u_int id, int palid, int paltex)
{
    PXF_BUF *p;
    u_short x;
    u_short y;

    p = &pxf_work[id];
    if (p->src == src) {
        return;
    }
    p->src = src;
    p->mode = ((u_short *)src)[0];
    p->tex = 0;
    p->sizex = ((u_short *)src)[1];
    p->sizey = ((u_short *)src)[2];
    x = ((u_short *)src)[3];
    y = ((u_short *)src)[4];
    p->tw = x >> 12;
    p->th = y >> 12;
    x &= 0xFFF;
    y &= 0xFFF;
    p->tbw = x >> 6;
    if (p->tbw == 0) {
        p->tbw = 1;
    }
    switch (p->mode & 0xFF) {
    case 0:
        p->vramsize = x * y / 64;
        p->wvramsize = p->sizex * p->sizey / 2;
        palid = -1;
        break;
    case 2:
        p->vramsize = x * y / 128;
        p->wvramsize = p->sizex * p->sizey / 4;
        palid = -1;
        break;
    case 0x13:
        switch (p->mode & 0xF00) {
        default:
            p->vramsize = x * y / 256;
            p->wvramsize = p->sizex * p->sizey / 8;
            break;
        case 0:
            p->vramsize = x * y / 256 + 4;
            p->wvramsize = p->sizex * p->sizey / 8 + 0x80;
            break;
        case 0x200:
            p->vramsize = x * y / 256 + 2;
            p->wvramsize = p->sizex * p->sizey / 8 + 0x40;
            break;
        }
        break;
    case 0x14:
        switch (p->mode & 0xF00) {
        default:
            p->vramsize = x * y / 512;
            p->wvramsize = p->sizex * p->sizey / 16;
            break;
        case 0:
            p->vramsize = x * y / 512 + 1;
            p->wvramsize = p->sizex * p->sizey / 16 + 8;
            break;
        case 0x200:
            p->vramsize = x * y / 512 + 1;
            p->wvramsize = p->sizex * p->sizey / 16 + 4;
            break;
        }
        break;
    }
    if (p->wvramsize & 1) {
        p->wvramsize++;
    }
    p->palid = palid;
    p->vram = -1;
    p->paltex = paltex;
    p->used = 0;
}

void PXF_WhiteSet(void)
{
    u_long *dma;
    u_short *s;
    int i;

    s = (u_short *)PxfPaletteBuf;
    *s++ = 0;
    for (i = 0; i < 255; i++) {
        *s++ = 0xFFFF;
    }
    dma = PacketDmaTagCnt(2);
    *dma++ = 0x4000000000000001;
    *dma++ = 0xEEEE;
    *dma++ = 0x0201236000000000;
    *dma++ = 0x50;
    *dma++ = 0;
    *dma++ = 0x51;
    *dma++ = 0x0000001000000010;
    *dma++ = 0x52;
    *dma++ = 0;
    *dma++ = 0x53;
    *dma++ = 0x0800000000008020;
    *dma++ = 0;
    PacketPointerSet(2, dma);
    PacketDmaTagRefs(2, PxfPaletteBuf, 0x200);
    PacketSendNow(2);
}

void PXF_Free(int id, int pal)
{
    pxf_work[id].vram = -1;
    pxf_work[id].src = 0;
    if (pal >= 0) {
        pxf_pal_work[pal] = 0;
    }
}

void PXF_PaletteClear(void)
{
    int i;

    for (i = 31; i >= 0; i--) {
        pxf_pal_work[i] = 0;
    }
    PXF_PalID = 0;
}

void PXF_SpriteClear(void)
{
    int i;

    for (i = 32; i < 1280; i++) {
        PXF_Free(i, -1);
    }
    PXF_PaletteClear();
}

void PXF_SysSpriteClear(void)
{
    int i;

    for (i = 0; i < 32; i++) {
        PXF_Free(i, -1);
    }
    PXF_PaletteClear();
}

int PXF_GetSysSprite(void)
{
    int i;

    for (i = 1; i < 32; i++) {
        if (pxf_work[i].src == 0) {
            pxf_work[i].src = (u_char *)4;
            return i;
        }
    }
    debugf("System Sprite Area Overflow\n");
    return -1;
}

int PXF_GetFreePalette(void)
{
    int i;

    for (i = PXF_PalID; i < 32; i++) {
        if (pxf_pal_work[i] == 0) {
            pxf_pal_work[i] = 1;
            PXF_PalID = i;
            return i;
        }
    }
    return 0;
}

int PXF_GetFreePaletteNum(int num)
{
    int i;

    for (i = 0; i < num; i++) {
        pxf_pal_work[PXF_PalID + i] = 1;
    }
    i = PXF_PalID;
    PXF_PalID = i + num;
    return i;
}

int PXF_GetFreePaletteEnd(void)
{
    int i;

    for (i = 31; i >= 0; i--) {
        if (pxf_pal_work[i] == 0) {
            pxf_pal_work[i] = 1;
            return i;
        }
    }
    return 0;
}

void SetCurrentPalette(char *src)
{
    CurrentPalette = src;
}

int IST_Load(int *texadr, int sprid, int palid)
{
    int ret;
    int paltex;

    ret = 1;
    if (texadr == 0) {
        debugf("IST Load Danger\n");
        return 1;
    }
    if (CurrentPalette && palid >= 0) {
        paltex = PXF_SendPalette((u_char *)CurrentPalette, palid);
        if (paltex) {
            pxf_pal_work[palid] = 1;
            ret = 0;
            PXF_Entry((u_char *)*texadr, sprid, palid, paltex);
        } else {
            PXF_Entry((u_char *)*texadr, sprid, -1, 0);
        }
        CurrentPalette = 0;
    } else {
        PXF_Entry((u_char *)*texadr, sprid, -1, 0);
    }
    return ret;
}

void PXF_SetPaletteMain(void)
{
    u_long *dma;

    dma = PacketDmaTagCnt(2);
    *dma++ = 0x4000000000000001;
    *dma++ = 0xEEEE;
    *dma++ = 0x0001232000000000;
    *dma++ = 0x50;
    *dma++ = 0;
    *dma++ = 0x51;
    *dma++ = 0x0000001000000010;
    *dma++ = 0x52;
    *dma++ = 0;
    *dma++ = 0x53;
    *dma++ = 0x0800000000008040;
    *dma++ = 0;
    PacketPointerSet(2, dma);
    PacketDmaTagRefs(2, PxfPaletteBuf, 0x400);
    dma = PacketDmaTagCnt(2);
    *dma++ = 0x4000000000000001;
    *dma++ = 0xEEEE;
    *dma++ = 0x0001234000000000;
    *dma++ = 0x50;
    *dma++ = 0;
    *dma++ = 0x51;
    *dma++ = 0x0000001000000010;
    *dma++ = 0x52;
    *dma++ = 0;
    *dma++ = 0x53;
    *dma++ = 0x0800000000008040;
    *dma++ = 0;
    PacketPointerSet(2, dma);
    PacketDmaTagRefs(2, &PxfPaletteBuf[256], 0x400);
    PacketSendNow(2);
}

void PXF_SetNextPalette(int dst)
{
    int vram;

    if ((u_int)dst > 512) {
        NextPalTex = 0;
        return;
    }
    vram = dst * 2 + 0x2320;
    if (dst & 0xF) {
        vram = dst * 2 + 0x2324;
    }
    NextPalTex = ((u_long)vram << 37) | ((u_long)3 << 60);
}

int MeterColorChange(int col);

void PXF_Uncompress(u_char *out, u_char *in)
{
    u_short mode;
    int x;
    int y;
    int xy;
    int i;
    int j;
    u_int *out_l;
    u_short *out_s;
    int m;

    m = MeterColorChange(0xFF00);
    mode = ((u_short *)in)[0];
    x = ((u_short *)in)[1];
    y = ((u_short *)in)[2];
    in += 16;
    if (x == 0 || y == 0) {
        return;
    }
    switch (mode & 0xFF) {
    case 0:
        xy = x * y;
        out_l = (u_int *)out;
        if ((mode & 0x3000) == 0x1000) {
            for (i = 0; i < xy; i++) {
                *out_l++ = (in[0] + (in[1] << 8) + (in[2] << 16)) | 0x80000000;
                in += 3;
            }
        } else if ((mode & 0x3000) == 0x2000) {
            u_char d;
            u_char *in2;

            in2 = in + xy / 8;
            for (i = 0; i < xy; i += 8) {
                d = *in++;
                for (j = 0; j < 8; j++) {
                    if (d & 0x80) {
                        *out_l++ |= 0x80000000 | (in2[0] + (in2[1] << 8) + (in2[2] << 16));
                        in2 += 3;
                    } else {
                        *out_l++ = 0;
                    }
                    d <<= 1;
                }
            }
        } else if ((mode & 0x3000) == 0x3000) {
            u_int d;
            u_char *in2;

            in2 = in + xy;
            for (i = 0; i < xy; i++) {
                d = *in++;
                if (d) {
                    *out_l |= (in2[0] + (in2[1] << 8) + (in2[2] << 16)) | (d << 24);
                    in2 += 3;
                } else {
                    *out_l = 0;
                }
                out_l++;
            }
        } else {
            for (i = 0; i < xy; i++) {
                *out_l++ = in[0] + (in[1] << 8) + (in[2] << 16) + (in[3] << 24);
                in += 4;
            }
        }
        break;
    case 2:
        xy = x * y;
        out_s = (u_short *)out;
        if ((mode & 0x3000) == 0x1000) {
            for (i = 0; i < xy; i++) {
                *out_s++ = (in[0] + (in[1] << 8)) | 0x80;
                in += 2;
            }
        } else if ((mode & 0x3000) == 0x2000) {
            u_char d;
            u_char *in2;

            in2 = in + xy / 8;
            for (i = 0; i < xy; i += 8) {
                d = *in++;
                for (j = 0; j < 8; j++) {
                    if (d & 0x80) {
                        *out_s++ = (in2[0] + (in2[1] << 8)) | 0x8000;
                        in2 += 2;
                    } else {
                        *out_s++ = 0;
                    }
                    d <<= 1;
                }
            }
        } else {
            for (i = 0; i < xy; i++) {
                *out_s++ = in[0] + (in[1] << 8);
                in += 2;
            }
        }
        break;
    case 0x13:
        xy = x * y;
        if ((mode & 0xF00) == 0) {
            out = read_clut32(out, in, 256);
            in += 0x400;
        } else if ((mode & 0xF00) == 0x200) {
            out = read_clut16(out, in, 256);
            in += 0x200;
        }
        read_pix8(out, in, mode, xy);
        break;
    case 0x14:
        xy = x * y / 2;
        if ((mode & 0xF00) == 0) {
            out = read_clut32(out, in, 16);
            in += 0x40;
        } else if ((mode & 0xF00) == 0x200) {
            out = read_clut16(out, in, 16);
            in += 0x20;
        }
        read_pix8(out, in, mode, xy);
        break;
    }
    MeterColorChange(m);
}

u_long *PacketPackRegWithTag(int id, u_long reg, u_long data);

#define PXF_GIFTAG(nloop, eop, pre, prim, flg, nreg) \
    ((u_long)(nloop) | ((u_long)(eop) << 15) | ((u_long)(pre) << 46) | ((u_long)(prim) << 47) | \
     ((u_long)(flg) << 58) | ((u_long)(nreg) << 60))
#define PXF_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp) | ((u_long)(sbw) << 16) | ((u_long)(spsm) << 24) | ((u_long)(dbp) << 32) | \
     ((u_long)(dbw) << 48) | ((u_long)(dpsm) << 56))
#define PXF_TRXREG(rrw, rrh) ((u_long)(rrw) | ((u_long)(rrh) << 32))
#define PXF_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp) | ((u_long)(tbw) << 14) | ((u_long)(psm) << 20) | ((u_long)(tw) << 26) | \
     ((u_long)(th) << 30) | ((u_long)(tcc) << 34) | ((u_long)(tfx) << 35) | ((u_long)(cbp) << 37) | \
     ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | ((u_long)(csa) << 56) | ((u_long)(cld) << 61))

u_long PXF_SendTexture(int id)
{
    PXF_BUF *p;
    u_char *src;
    u_long *dma;
    int vram;

    p = &pxf_work[id];
    if (p->vram < 0) {
        return -1;
    }
    if ((int)p->src < 4) {
        return -1;
    }
    if (p->used) {
        return -1;
    }
    if (VramTop + p->vramsize > 0x4000) {
        VramTop = 0x2362;
        VramPage++;
    }
    if (p->vrampage == VramPage) {
        return p->tex;
    }
    vram = VramTop;
    p->vrampage = VramPage;
    VramTop = vram + p->vramsize;
    dma = PacketDmaTagCnt(2);
    if (p->mode & 0xF000) {
        src = (u_char *)&PxfVRAM[p->vram];
    } else {
        src = p->src + 16;
    }
    switch (p->mode & 0xFF) {
    case 0:
        *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
        *dma++ = 0xEEEE;
        *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, p->tbw, 0);
        *dma++ = 0x50;
        *dma++ = 0;
        *dma++ = 0x51;
        *dma++ = PXF_TRXREG(p->sizex, p->sizey);
        *dma++ = 0x52;
        *dma++ = 0;
        *dma++ = 0x53;
        *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 4, 1, 0, 0, 2, 0);
        *dma++ = 0;
        PacketPointerSet(2, dma);
        PacketDmaTagRefs(2, src, p->sizex * p->sizey * 4);
        p->tex = PXF_TEX0(vram, p->tbw, 0, p->tw, p->th, 1, 0, 0, 0, 0, 0, 0);
        break;
    case 2:
        *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
        *dma++ = 0xEEEE;
        *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, p->tbw, 2);
        *dma++ = 0x50;
        *dma++ = 0;
        *dma++ = 0x51;
        *dma++ = PXF_TRXREG(p->sizex, p->sizey);
        *dma++ = 0x52;
        *dma++ = 0;
        *dma++ = 0x53;
        *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 8, 1, 0, 0, 2, 0);
        *dma++ = 0;
        PacketPointerSet(2, dma);
        PacketDmaTagRefs(2, src, p->sizex * p->sizey * 2);
        p->tex = PXF_TEX0(vram, p->tbw, 2, p->tw, p->th, 1, 0, 0, 0, 0, 0, 0);
        break;
    case 0x13:
        switch (p->mode & 0xF00) {
        case 0:
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, 1, 0);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(16, 16);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(64, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src, 1024);
            dma = PacketDmaTagCnt(2);
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram + 4, p->tbw, 0x13);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(p->sizex, p->sizey);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 16, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src + 1024, p->sizex * p->sizey);
            p->tex = PXF_TEX0(vram + 4, p->tbw, 0x13, p->tw, p->th, 1, 0, vram, 0, 0, 0x10, 1);
            break;
        case 0x200:
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, 1, 2);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(16, 16);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(32, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src, 512);
            dma = PacketDmaTagCnt(2);
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram + 2, p->tbw, 0x13);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(p->sizex, p->sizey);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 16, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src + 512, p->sizex * p->sizey);
            p->tex = PXF_TEX0(vram + 2, p->tbw, 0x13, p->tw, p->th, 1, 0, vram, 2, 0, 0x10, 1);
            break;
        default:
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, p->tbw, 0x13);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(p->sizex, p->sizey);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 16, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src, p->sizex * p->sizey);
            p->tex = PXF_TEX0(vram, p->tbw, 0x13, p->tw, p->th, 1, 0, 0, 0, 0, 0, 0);
            break;
        }
        break;
    case 0x14:
        switch (p->mode & 0xF00) {
        case 0:
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, 1, 0);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(8, 2);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(4, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src, 64);
            dma = PacketDmaTagCnt(2);
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram + 1, p->tbw, 0x14);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(p->sizex, p->sizey);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 32, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src + 64, p->sizex * p->sizey / 2);
            p->tex = PXF_TEX0(vram + 1, p->tbw, 0x14, p->tw, p->th, 1, 0, vram, 0, 0, 0x10, 1);
            break;
        case 0x200:
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, 1, 2);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(8, 2);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(2, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src, 32);
            dma = PacketDmaTagCnt(2);
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram + 1, p->tbw, 0x14);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(p->sizex, p->sizey);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 32, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src + 32, p->sizex * p->sizey / 2);
            p->tex = PXF_TEX0(vram + 1, p->tbw, 0x14, p->tw, p->th, 1, 0, vram, 2, 0, 0x10, 1);
            break;
        default:
            *dma++ = PXF_GIFTAG(1, 0, 0, 0, 0, 4);
            *dma++ = 0xEEEE;
            *dma++ = PXF_BITBLTBUF(0, 0, 0, vram, p->tbw, 0x14);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = PXF_TRXREG(p->sizex, p->sizey);
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = PXF_GIFTAG(p->sizex * p->sizey / 32, 1, 0, 0, 2, 0);
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, src, p->sizex * p->sizey / 2);
            p->tex = PXF_TEX0(vram, p->tbw, 0x14, p->tw, p->th, 1, 0, 0, 0, 0, 0, 0);
            break;
        }
        break;
    }
    PacketDmaTagCnt(2);
    PacketPackRegWithTag(2, 0x3F, 0);
    return p->tex;
}

void read_pix8(u_char *out, u_char *in, int mode, int xy)
{
    int i;
    int j;
    int k;
    u_char d;
    u_char last;
    u_int *writep;
    u_int wcode;
    u_int *inl;

    last = 0;
    if (mode & 0x4000) {
        for (i = xy; i > 0;) {
            d = *in++;
            if (!(d & 0xC0)) {
                for (j = d; j >= 0; j--) {
                    if (!((u_int)out & 3)) {
                        if (!((u_int)in & 3)) {
                            for (; j >= 5; j -= 4) {
                                *(u_int *)out = *(u_int *)in;
                                in += 4;
                                out += 4;
                            }
                        } else if (!((u_int)in & 1)) {
                            for (; j >= 3; j -= 2) {
                                *(u_short *)out = *(u_short *)in;
                                in += 2;
                                out += 2;
                            }
                        } else {
                            for (; j >= 5; j -= 4) {
                                wcode = 0;
                                for (k = 0; k < 4; k++) {
                                    wcode |= *in++ << (k * 8);
                                }
                                *(u_int *)out = wcode;
                                out += 4;
                            }
                        }
                    }
                    *out++ = *in++;
                }
                i -= d + 1;
                last = in[-1];
            } else {
                j = (u_char)(d + 0xC3);
                if (j >= 0 && ((u_int)out & 3)) {
                    i -= j;
                    do {
                        *out++ = last;
                        j--;
                    } while (j >= 0 && ((u_int)out & 3));
                } else {
                    i -= j;
                }
                wcode = last | (last << 8);
                wcode |= wcode << 16;
                for (; j >= 4; j -= 4) {
                    *(u_int *)out = wcode;
                    out += 4;
                }
                while (--j >= 0) {
                    *out++ = last;
                }
            }
        }
    } else {
        writep = (u_int *)out;
        inl = (u_int *)in;
        for (i = xy / 4; --i >= 0;) {
            *writep++ = *inl++;
        }
    }
}

void PXF_SetPalette(u_short *src, int id, int num)
{
    int i;
    u_int *dst;
    u_int a;
    u_int r;
    u_int g;
    u_int b;
    u_int k;
    u_int c;
    int n;

    id <<= 4;
    n = (id & 0x10) ? id - 8 : id;
    dst = &PxfPaletteBuf[1] + n;
    for (i = 0; i < num; i++) {
        c = *src++;
        k = 0xFF000000;
        a = k;
        if (!(c & 0x8000)) a = 0;
        r = ((u_short)c >> 7) & 0xF8;
        g = ((u_short)c << 6) & 0xF800;
        b = (c & 0x1F) << 19;
        *dst++ = (a | g) | (r | b);
        if (i == 6) {
            dst += 8;
        }
    }
}

int PXF_SendPalette(u_char *src, int dst)
{
    u_long r;
    u_short mode;
    u_short m2;
    u_long *dma;
    u_char *work;
    int vram;

    work = (u_char *)&PxfPaletteBuf[dst * 16];
    vram = dst * 2 + 0x2320;
    mode = *(u_short *)src;
    m2 = mode & 0xFF;
    if (m2 == 0 || m2 == 2) {
        return 0;
    }
    mode &= 0xF00;
    if (mode == 0) {
        if (m2 != 0x14) {
            int i;
            int j;
            u_int *s;
            u_int *d;

            read_clut32(work, src + 16, 256);
            dma = PacketDmaTagCnt(2);
            *dma++ = 0x4000000000000001;
            *dma++ = 0xEEEE;
            *dma++ = ((u_long)vram << 32) | ((u_long)1 << 48);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = 0x0000001000000010;
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = 0x0800000000008040;
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, work, 1024);
            for (i = 0; i < 12; i++) {
                d = &PxfPaletteBuf2[(i + dst) * 16];
                s = &PxfPaletteBuf[dst * 16 + (i / 2) * 32 + (i % 2) * 8];
                for (j = 0; j < 8; j++) {
                    *d++ = *s++;
                }
                s += 8;
                for (j = 0; j < 8; j++) {
                    *d++ = *s++;
                }
                PxfPaletteBuf2[(i + dst) * 16] = 0;
                dma = PacketDmaTagCnt(2);
                *dma++ = 0x4000000000000001;
                *dma++ = 0xEEEE;
                *dma++ = ((u_long)(vram + (i << 1) + 4) << 32) | ((u_long)1 << 48);
                *dma++ = 0x50;
                *dma++ = 0;
                *dma++ = 0x51;
                *dma++ = 0x0000000200000008;
                *dma++ = 0x52;
                *dma++ = 0;
                *dma++ = 0x53;
                *dma++ = 0x0800000000008004;
                *dma++ = 0;
                PacketPointerSet(2, dma);
                PacketDmaTagRefs(2, &PxfPaletteBuf2[(i + dst) * 16], 64);
            }
        } else {
            read_clut32(work, src + 16, 16);
            dma = PacketDmaTagCnt(2);
            *dma++ = 0x4000000000000001;
            *dma++ = 0xEEEE;
            *dma++ = ((u_long)vram << 32) | ((u_long)1 << 48);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = 0x0000000200000008;
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = 0x0800000000008004;
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, work, 64);
        }
        r = ((u_long)vram << 37) | 0x3000000000000000;
    } else {
        if (m2 != 0x14) {
            int i;
            int j;
            u_int *s;
            u_int *d;

            read_clut16(work, src + 16, 256);
            dma = PacketDmaTagCnt(2);
            *dma++ = 0x4000000000000001;
            *dma++ = 0xEEEE;
            *dma++ = ((u_long)vram << 32) | ((u_long)0x8040 << 42);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = 0x0000001000000010;
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = 0x0800000000008020;
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, work, 512);
            for (i = 0; i < 12; i++) {
                d = &PxfPaletteBuf2[(i + dst) * 16];
                s = &PxfPaletteBuf[dst * 16 + (i / 2) * 32 + (i % 2) * 8];
                for (j = 0; j < 4; j++) {
                    *d++ = *s++;
                }
                s += 4;
                for (j = 0; j < 4; j++) {
                    *d++ = *s++;
                }
                PxfPaletteBuf2[(i + dst) * 16] = 0;
                dma = PacketDmaTagCnt(2);
                *dma++ = 0x4000000000000001;
                *dma++ = 0xEEEE;
                *dma++ = ((u_long)(vram + (i << 1) + 4) << 32) | ((u_long)0x8040 << 42);
                *dma++ = 0x50;
                *dma++ = 0;
                *dma++ = 0x51;
                *dma++ = 0x0000000200000008;
                *dma++ = 0x52;
                *dma++ = 0;
                *dma++ = 0x53;
                *dma++ = 0x0800000000008002;
                *dma++ = 0;
                PacketPointerSet(2, dma);
                PacketDmaTagRefs(2, &PxfPaletteBuf2[(i + dst) * 16], 32);
            }
        } else {
            read_clut16(work, src + 16, 16);
            dma = PacketDmaTagCnt(2);
            *dma++ = 0x4000000000000001;
            *dma++ = 0xEEEE;
            *dma++ = ((u_long)vram << 32) | ((u_long)0x8040 << 42);
            *dma++ = 0x50;
            *dma++ = 0;
            *dma++ = 0x51;
            *dma++ = 0x0000000200000008;
            *dma++ = 0x52;
            *dma++ = 0;
            *dma++ = 0x53;
            *dma++ = 0x0800000000008002;
            *dma++ = 0;
            PacketPointerSet(2, dma);
            PacketDmaTagRefs(2, work, 32);
        }
        r = ((u_long)vram << 37) | 0x3010000000000000;
    }
    PacketSendNow(2);
    return r >> 37;
}
