#include "generated/sprite.h"

void nlObjPutSetFadeColor(float r, float g, float b) {
}

void nlFreeCommonTextureAll(void) {
}

void nlFreePaletteAll(void) {
}


void debugf(char *format, ...);

int CheckSpriteBuffer(int size)
{
    if (spr_count >= 1200) {
        debugf("Number of Sprite Overflow\n");
        return 1;
    }
    if (spr_ptr + size >= 16000) {
        debugf("Sprite Buffer Overflow\n");
        return 1;
    }
    return 0;
}

void PXF_Init(void);
void GraphSetOffset(int x, int y);
extern USER_SETTING userinfo;

void SpriteInit(void)
{
    spr_count = 0;
    spr_ptr = 0;
    PXF_Init();
    GraphSetOffset(userinfo.xpos, userinfo.ypos);
    SetBlendingMode(0);
}

u_long XYConv(float x, float y)
{
    return (((int)x * 16 + 0x6BF8) | ((u_long)((int)y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
}

u_long XYConvP(float *p)
{
    return (((int)p[0] * 16 + 0x6BF8) | ((u_long)((int)p[1] * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
}

void PXF_SetNextPalette(int dst);

void nlDispStripSetPalette(int d)
{
    PXF_SetNextPalette(d);
}

void nlDispStripTex(int num, NLVERTEX03 *vt, int spr)
{
    nlDispStripTexTrnsl(num, vt, spr);
}

void nlDispStripTexBG(NLVERTEX03 *vt, int spr)
{
    nlDispStripTexTrnslBG(vt, spr);
}

void nlsDispStripTex(int num, NLVERTEX03 *vt, int spr)
{
    nlsDispStripTexTrnsl(num, vt, spr);
}

void nlsDispStripTexBG(NLVERTEX03 *vt, int spr)
{
    nlsDispStripTexTrnslBG(vt, spr);
}

void PXF_SetPalette(u_short *src, int id, int num);

void nlSetPaletteData(int id, u_short *tb, int num)
{
    PXF_SetPalette(tb, id, num);
}

void BackColor(u_int rgb);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);

void nlSetBackColor(int c1, int c2, int c3)
{
    int r;
    int b;

    r = (c3 >> 16) & 0xFF;
    b = c3 & 0xFF;
    BackColor(r | (c3 & 0xFF00) | (b << 16));
    polydraw_GBox(0.0f, 0.0f, 640.0f, 480.0f, 1000.0f, c1, c2, c2, c3);
}

void SetBlendingMode(short int mode)
{
    if (mode == 1) {
        SpriteAlpha = 0x8000000048;
    } else if (mode == 2) {
        SpriteAlpha = 0x8000000022;
    } else if (mode == 3) {
        SpriteAlpha = 0x8000000026;
    } else {
        SpriteAlpha = 0x8000000044;
    }
}

u_long UVConv(float u, float v)
{
    int tu;
    int tv;

    tu = u * 16.0f;
    tv = v * 16.0f;
    if (tu == 0x4000) {
        tu = 0x3FFF;
    }
    return tu | ((u_long)tv << 16);
}

u_long *PacketDmaTagRefs(int id, void *adr, int size);
u_long PXF_SendTexture(int id);

static void sprite_trans_set(SPR_INDEX *p)
{
    u_long *addr;
    long qwc;
    u_long tex;

    p->key = 0;
    addr = (u_long *)&spr_buf[p->index];
    qwc = (long)p->size << 4;
    if (p->id >= 0) {
        tex = PXF_SendTexture(p->id);
        if (tex == -1) {
            return;
        }
        if (addr[2]) {
            addr[2] = (tex & (~0UL >> 27)) | addr[2];
        } else {
            addr[2] = tex;
        }
    }
    PacketDmaTagRefs(2, addr, qwc);
}

extern int nlnDispNum; /* STATIC_EXTERN 0x5cc480 */
extern u_long *wrp; /* STATIC_EXTERN 0x5cc484 */

void nl3DispStripTrnsl(void)
{
    spr_ptr += nlnDispNum;
    spr_index[spr_count].size = nlnDispNum;
    spr_index[spr_count].id = -1;
    spr_count++;
}

void nlDispStrip(int num, NLVERTEX00 *vt)
{
    u_long *p;
    int i;

    p = (u_long *)&spr_buf[spr_ptr];
    if (CheckSpriteBuffer(num + 4)) {
        return;
    }
    *p++ = 0x1000000000000002;
    *p++ = 0xE;
    *p++ = 0x88;
    *p++ = 0x42;
    *p++ = 0x14C;
    *p++ = 0;
    *p++ = num | 0x2400000000000000;
    *p++ = 0x51;
    for (i = 0; i < num; i++) {
        *p++ = RGBConv2(vt[i].col);
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].id = -1;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num + 4;
    spr_ptr += num + 4;
    spr_count++;
}

void nlDispStripTrnsl(int num, NLVERTEX00 *vt)
{
    u_long *p;
    int i;

    p = (u_long *)&spr_buf[spr_ptr];
    if (CheckSpriteBuffer(num + 4)) {
        return;
    }
    *p++ = 0x1000000000000002;
    *p++ = 0xE;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x14C;
    *p++ = 0;
    *p++ = num | 0x2400000000000000;
    *p++ = 0x51;
    for (i = 0; i < num; i++) {
        *p++ = RGBConv2(vt[i].col);
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].id = -1;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num + 4;
    spr_ptr += num + 4;
    spr_count++;
}

void nlcDispStrip(int num, NLVERTEX00 *vt)
{
    u_long *p;
    int i;

    p = (u_long *)&spr_buf[spr_ptr];
    if (CheckSpriteBuffer(num + 4)) {
        return;
    }
    *p++ = 0x1000000000000002;
    *p++ = 0xE;
    *p++ = 0x88;
    *p++ = 0x42;
    *p++ = 0x14C;
    *p++ = 0;
    *p++ = num | 0x2400000000000000;
    *p++ = 0x51;
    for (i = 0; i < num; i++) {
        *p++ = vt[i].col;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].id = -1;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num + 4;
    spr_ptr += num + 4;
    spr_count++;
}

void nlcDispStripTrnsl(int num, NLVERTEX00 *vt)
{
    u_long *p;
    int i;

    p = (u_long *)&spr_buf[spr_ptr];
    if (CheckSpriteBuffer(num + 4)) {
        return;
    }
    *p++ = 0x1000000000000002;
    *p++ = 0xE;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x14C;
    *p++ = 0;
    *p++ = num | 0x2400000000000000;
    *p++ = 0x51;
    for (i = 0; i < num; i++) {
        *p++ = vt[i].col;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].id = -1;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num + 4;
    spr_ptr += num + 4;
    spr_count++;
}

void nlcDispFan(int num, NLVERTEX00 *vt)
{
    u_long *p;
    int i;

    p = (u_long *)&spr_buf[spr_ptr];
    if (CheckSpriteBuffer(num + 5)) {
        return;
    }
    *p++ = (num + 4) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = 0x88;
    *p++ = 0x42;
    *p++ = 0x14D;
    *p++ = 0;
    *p++ = vt[0].col;
    *p++ = 1;
    *p++ = (((int)vt[0].x * 16 + 0x6BF8) | ((u_long)((int)vt[0].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    *p++ = vt[1].col;
    *p++ = 1;
    *p++ = (((int)vt[1].x * 16 + 0x6BF8) | ((u_long)((int)vt[1].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    for (i = 0; i < num; i++) {
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].id = -1;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num + 5;
    spr_ptr += num + 5;
    spr_count++;
}

void nlcDispFanTrnsl(int num, NLVERTEX00 *vt)
{
    u_long *p;
    int i;

    p = (u_long *)&spr_buf[spr_ptr];
    if (CheckSpriteBuffer(num + 5)) {
        return;
    }
    *p++ = (num + 4) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x14D;
    *p++ = 0;
    *p++ = vt[0].col;
    *p++ = 1;
    *p++ = (((int)vt[0].x * 16 + 0x6BF8) | ((u_long)((int)vt[0].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    *p++ = vt[1].col;
    *p++ = 1;
    *p++ = (((int)vt[1].x * 16 + 0x6BF8) | ((u_long)((int)vt[1].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    for (i = 2; i < num; i++) {
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].id = -1;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num + 5;
    spr_ptr += num + 5;
    spr_count++;
}

int MeterColorChange(int col);
void PXF_Transfer(void);
void PacketSendNow(int id);

void SpriteDisplay(void)
{
    int i;
    int j;
    int out;
    int mem;
    float pri;

    if (spr_count == 0) {
        return;
    }
    mem = MeterColorChange(0xFF0000);
    PXF_Transfer();
    MeterColorChange(0xFFFFFF);
    for (i = 0; i < spr_count; i++) {
        pri = 0.0f;
        out = -1;
        for (j = 0; j < spr_count; j++) {
            if (pri < spr_index[j].key) {
                pri = spr_index[j].key;
                out = j;
            }
        }
        if (out == -1) {
            break;
        }
        sprite_trans_set(&spr_index[out]);
        for (j = out + 1; j < spr_count; j++) {
            if (spr_index[j].key == pri) {
                sprite_trans_set(&spr_index[j]);
            }
        }
    }
    PacketSendNow(2);
    MeterColorChange(mem);
}

int XYCheck(float x, float y);
u_long PXF_SetTexture(int id);

void nlsDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr)
{
    int i;
    u_long tex;
    u_long *p;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(num * 3 + 4)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConv(vt[i].col);
        *p++ = 1;
        *p++ = UVConvP(&vt[i].u);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
}

void nlsDispStripTexTrnslBG(NLVERTEX03 *vt, int spr)
{
    u_long tex;
    u_long *p;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(9)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    *p++ = 0x1000000000000008;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15E;
    *p++ = 0;
    *p++ = RGBConv(vt[0].col);
    *p++ = 1;
    *p++ = UVConvP(&vt[0].u);
    *p++ = 3;
    *p++ = (((int)vt[0].x * 16 + 0x6BF8) | ((u_long)((int)vt[0].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    *p++ = UVConvP(&vt[3].u);
    *p++ = 3;
    *p++ = (((int)vt[3].x * 16 + 0x6BF8) | ((u_long)((int)vt[3].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = 9;
    spr_index[spr_count].id = spr;
    spr_ptr += 9;
    spr_count++;
}

void PXF_GetSize(int id, int *x, int *y);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr)
{
    int i;
    u_long tex;
    u_long *p;
    int sizex;
    int sizey;
    float sx;
    float sy;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(num * 3 + 4)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    PXF_GetSize(spr, &sizex, &sizey);
    sx = sizex;
    sy = sizey;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConv(vt[i].col);
        *p++ = 1;
        *p++ = UVConv(vt[i].u * sx, vt[i].v * sy);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
}

void nlDispStripTexTrnslBG(NLVERTEX03 *vt, int spr)
{
    u_long tex;
    u_long *p;
    int sizex;
    int sizey;
    float sx;
    float sy;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(9)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    PXF_GetSize(spr, &sizex, &sizey);
    sx = sizex;
    sy = sizey;
    *p++ = 0x1000000000000008;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15E;
    *p++ = 0;
    *p++ = RGBConv(vt[0].col);
    *p++ = 1;
    *p++ = 0;
    *p++ = 3;
    *p++ = (((int)vt[0].x * 16 + 0x6BF8) | ((u_long)((int)vt[0].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    *p++ = UVConv(sx, sy);
    *p++ = 3;
    *p++ = (((int)vt[3].x * 16 + 0x6BF8) | ((u_long)((int)vt[3].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
    *p++ = 5;
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = 9;
    spr_index[spr_count].id = spr;
    spr_ptr += 9;
    spr_count++;
}

u_long UVConvP(float *uv)
{
    int tu;
    int tv;

    tu = uv[0] * 16.0f;
    tv = uv[1] * 16.0f;
    if (tu == 0x4000) {
        tu = 0x3FFF;
    }
    return tu | ((u_long)tv << 16);
}

u_int RGBConv2(u_int rgb)
{
    int a, r, g, b;

    a = rgb >> 24;
    b = rgb & 0xFF;
    g = (rgb >> 8) & 0xFF;
    r = (rgb >> 16) & 0xFF;
    a = (a + 1) >> 1;
    return (u_long)r | ((u_long)g << 8) | ((u_long)b << 16) | ((u_long)a << 24);
}

u_int RGBConvG(u_int rgb)
{
    int a, r, g, b;

    a = rgb >> 24;
    b = rgb & 0xFF;
    g = (rgb >> 8) & 0xFF;
    r = (rgb >> 16) & 0xFF;
    a = (a + 1) >> 1;
    r = r * 9 / 20;
    g = g * 9 / 20;
    b = b * 9 / 20;
    return (u_long)r | ((u_long)g << 8) | ((u_long)b << 16) | ((u_long)a << 24);
}

u_int RGBConv(u_int rgb)
{
    int a;
    u_long r, g, b;

    a = rgb >> 24;
    a = (a + 1) >> 1;
    r = (rgb >> 16) & 0xFF;
    g = (rgb >> 8) & 0xFF;
    b = rgb & 0xFF;
    return (r >> 1) | ((((u_long)a << 24) | ((g >> 1) << 8)) | ((b >> 1) << 16));
}

u_int RGBConvOfs(u_int rgb, u_int rgb2)
{
    int a;
    u_long r, g, b;

    a = rgb >> 24;
    a = (a + 1) >> 1;
    r = (rgb2 >> 16) & 0xFF;
    g = (rgb2 >> 8) & 0xFF;
    b = rgb2 & 0xFF;
    return (r >> 1) | ((((u_long)a << 24) | ((g >> 1) << 8)) | ((b >> 1) << 16));
}

int XYCheck(float x, float y)
{
    if (x < -1700.0f || x > 1700.0f) return 1;
    if (y < -1700.0f || y > 1700.0f) return 1;
    return 0;
}

void nlsDispStripTexOfs(int num, NLVERTEX03 *vt, int spr)
{
    int i;
    u_long tex;
    u_long *p;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(num * 6 + 8)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvG(vt[i].col);
        *p++ = 1;
        *p++ = UVConvP(&vt[i].u);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
    tex = (tex & (~0UL >> 27)) | 0x30146C0000000000;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = 0x8000000048;
    *p++ = 0x42;
    *p++ = 0x14C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvOfs(vt[i].col, vt[i].ofs);
        *p++ = 1;
        *p++ = UVConvP(&vt[i].u);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
}

void nlsDispStripTexOfsTrnsl(int num, NLVERTEX03 *vt, int spr)
{
    int i;
    u_long tex;
    u_long *p;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(num * 6 + 8)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvG(vt[i].col);
        *p++ = 1;
        *p++ = UVConvP(&vt[i].u);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
    tex = (tex & (~0UL >> 27)) | 0x30146C0000000000;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = 0x8000000048;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvOfs(vt[i].col, vt[i].ofs);
        *p++ = 1;
        *p++ = UVConvP(&vt[i].u);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
}

void nlDispStripTexOfs(int num, NLVERTEX03 *vt, int spr)
{
    int i;
    u_long tex;
    u_long *p;
    int sizex;
    int sizey;
    float sx;
    float sy;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(num * 6 + 8)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    PXF_GetSize(spr, &sizex, &sizey);
    sx = sizex;
    sy = sizey;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvG(vt[i].col);
        *p++ = 1;
        *p++ = UVConv(vt[i].u * sx, vt[i].v * sy);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
    tex = (tex & (~0UL >> 27)) | 0x30146C0000000000;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = 0x8000000048;
    *p++ = 0x42;
    *p++ = 0x14C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvOfs(vt[i].col, vt[i].ofs);
        *p++ = 1;
        *p++ = UVConv(vt[i].u * sx, vt[i].v * sy);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
}

void nlDispStripTexOfsTrnsl(int num, NLVERTEX03 *vt, int spr)
{
    int i;
    u_long tex;
    u_long *p;
    int sizex;
    int sizey;
    float sx;
    float sy;

    p = (u_long *)&spr_buf[spr_ptr];
    if (XYCheck(vt->x, vt->y)) {
        return;
    }
    if (CheckSpriteBuffer(num * 6 + 8)) {
        return;
    }
    tex = PXF_SetTexture(spr);
    if (tex == -1) {
        return;
    }
    PXF_GetSize(spr, &sizex, &sizey);
    sx = sizex;
    sy = sizey;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = SpriteAlpha;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvG(vt[i].col);
        *p++ = 1;
        *p++ = UVConv(vt[i].u * sx, vt[i].v * sy);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
    tex = (tex & (~0UL >> 27)) | 0x30146C0000000000;
    *p++ = (num * 3 + 3) | 0x1000000000000000;
    *p++ = 0xE;
    *p++ = tex;
    *p++ = 6;
    *p++ = 0x8000000048;
    *p++ = 0x42;
    *p++ = 0x15C;
    *p++ = 0;
    for (i = 0; i < num; i++) {
        *p++ = RGBConvOfs(vt[i].col, vt[i].ofs);
        *p++ = 1;
        *p++ = UVConv(vt[i].u * sx, vt[i].v * sy);
        *p++ = 3;
        *p++ = (((int)vt[i].x * 16 + 0x6BF8) | ((u_long)((int)vt[i].y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
        *p++ = 5;
    }
    spr_index[spr_count].key = vt->z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].size = num * 3 + 4;
    spr_index[spr_count].id = spr;
    spr_ptr += num * 3 + 4;
    spr_count++;
}

/* INLINE_STANDIN: the original inlines XYConvP here (KR lines 189-191); XYConvP is defined above without inline. */
static inline u_long nl2DispStripTrnsl_XYConvP(float *p)
{
    return (((int)p[0] * 16 + 0x6BF8) | ((u_long)((int)p[1] * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
}

void nl2DispStripTrnsl(int num, NLVERTEX00 *vt)
{
    int i;

    *wrp++ = 0x1000000000000001; *wrp++ = 0xE;
    *wrp++ = 0x14C; *wrp++ = 0;
    *wrp++ = num | 0x2400000000000000; *wrp++ = 0x51;
    for (i = 0; i < num; i++) {
        *wrp++ = vt[i].col; *wrp++ = nl2DispStripTrnsl_XYConvP(&vt[i].x);
    }
    nlnDispNum += num + 3;
}

void nl1DispStripTrnsl(float z)
{
    wrp = (u_long *)&spr_buf[spr_ptr];

    *wrp++ = 0x1000000000000001; *wrp++ = 0xE;
    *wrp++ = SpriteAlpha; *wrp++ = 0x42;
    spr_index[spr_count].key = z;
    spr_index[spr_count].index = spr_ptr;
    spr_index[spr_count].id = -1;
    nlnDispNum = 2;
}

float nlSin(unsigned short r);
float nlCos(unsigned short r);

/* INLINE_STANDIN: the original inlines XYConv here (KR lines 180-182); XYConv is defined above without inline. */
static inline u_long nlSprPut_XYConv(float x, float y)
{
    return (((int)x * 16 + 0x6BF8) | ((u_long)((int)y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
}

void nlSprPut(NL_SPR_ARG *spr)
{
    u_long tex;
    u_long *p;
    int sizex;
    int sizey;
    float sx;
    float sy;
    float x0;
    float y0;
    float x1;
    float y1;
    float u0;
    float v0;
    float u1;
    float v1;

    p = (u_long *)&spr_buf[spr_ptr];
    x1 = y1 = x0 = y0 = 0.0f;
    tex = PXF_SetTexture(spr->sprno);
    if (tex == -1) {
        return;
    }
    PXF_GetSize(spr->sprno, &sizex, &sizey);
    sx = sizex * spr->zm_x;
    sy = sizey * spr->zm_y;
    if (spr->attr & 0x10) {
        u0 = spr->u1 * sizex;
        u1 = spr->u0 * sizex;
    } else {
        u0 = spr->u0 * sizex;
        u1 = spr->u1 * sizex;
    }
    if (spr->attr & 0x20) {
        v0 = spr->v1 * sizey;
        v1 = spr->v0 * sizey;
    } else {
        v0 = spr->v0 * sizey;
        v1 = spr->v1 * sizey;
    }
    if (spr->ang == 0) {
        if (CheckSpriteBuffer(9)) {
            return;
        }
        switch (spr->attr & 3) {
        case 1:
            x0 = spr->x;
            x1 = x0 + sx;
            break;
        case 2:
            x0 = spr->x - sx * 0.5f;
            x1 = spr->x + sx * 0.5f;
            break;
        case 3:
            x0 = spr->x - sx;
            x1 = spr->x;
            break;
        }
        switch ((spr->attr >> 2) & 3) {
        case 1:
            y0 = spr->y;
            y1 = y0 + sy;
            break;
        case 2:
            y0 = spr->y - sy * 0.5f;
            y1 = spr->y + sy * 0.5f;
            break;
        case 3:
            y0 = spr->y - sy;
            y1 = spr->y;
            break;
        }
        if (XYCheck(x0, y0)) {
            return;
        }
        *p++ = 0x1000000000000008; *p++ = 0xE;
        *p++ = tex; *p++ = 6;
        *p++ = SpriteAlpha; *p++ = 0x42;
        *p++ = 0x1D6; *p++ = 0;
        *p++ = RGBConv(0xFFFFFFFF); *p++ = 1;
        *p++ = UVConv(u0, v0); *p++ = 3;
        *p++ = nlSprPut_XYConv(x0, y0); *p++ = 5;
        *p++ = UVConv(u1, v1); *p++ = 3;
        *p++ = nlSprPut_XYConv(x1, y1); *p++ = 5;
        spr_index[spr_count].key = spr->z;
        spr_index[spr_count].index = spr_ptr;
        spr_index[spr_count].size = 9;
        spr_index[spr_count].id = spr->sprno;
        spr_ptr += 9;
    } else {
        float xd0 = 0.0f, yd0 = 0.0f, xd1 = 0.0f, yd1 = 0.0f, x2 = 0.0f, y2 = 0.0f, x3 = 0.0f, y3 = 0.0f;

        if (CheckSpriteBuffer(13)) {
            return;
        }
        switch (spr->attr & 3) {
        case 1:
            xd0 = 0.0f;
            xd1 = sx;
            break;
        case 2:
            xd0 = -sx * 0.5f;
            xd1 = sx * 0.5f;
            break;
        case 3:
            xd0 = -sx;
            break;
        }
        switch ((spr->attr >> 2) & 3) {
        case 1:
            yd0 = 0.0f;
            yd1 = sy;
            break;
        case 2:
            yd0 = -sy * 0.5f;
            yd1 = sy * 0.5f;
            break;
        case 3:
            yd0 = -sy;
            yd1 = 0.0f;
            break;
        }
        x0 = spr->x + nlCos(spr->ang) * xd0 + nlSin(spr->ang) * yd0;
        y0 = spr->y + nlCos(spr->ang) * yd0 - nlSin(spr->ang) * xd0;
        x1 = spr->x + nlCos(spr->ang) * xd1 + nlSin(spr->ang) * yd0;
        y1 = spr->y + nlCos(spr->ang) * yd0 - nlSin(spr->ang) * xd1;
        x2 = spr->x + nlCos(spr->ang) * xd0 + nlSin(spr->ang) * yd1;
        y2 = spr->y + nlCos(spr->ang) * yd1 - nlSin(spr->ang) * xd0;
        x3 = spr->x + nlCos(spr->ang) * xd1 + nlSin(spr->ang) * yd1;
        y3 = spr->y + nlCos(spr->ang) * yd1 - nlSin(spr->ang) * xd1;
        if (XYCheck(x0, y0)) {
            return;
        }
        *p++ = 0x100000000000000C; *p++ = 0xE;
        *p++ = tex; *p++ = 6;
        *p++ = SpriteAlpha; *p++ = 0x42;
        *p++ = 0x1D4; *p++ = 0;
        *p++ = RGBConv(0xFFFFFFFF); *p++ = 1;
        *p++ = UVConv(u0, v0); *p++ = 3;
        *p++ = nlSprPut_XYConv(x0, y0); *p++ = 5;
        *p++ = UVConv(u1, v0); *p++ = 3;
        *p++ = nlSprPut_XYConv(x1, y1); *p++ = 5;
        *p++ = UVConv(u0, v1); *p++ = 3;
        *p++ = nlSprPut_XYConv(x2, y2); *p++ = 5;
        *p++ = UVConv(u1, v1); *p++ = 3;
        *p++ = nlSprPut_XYConv(x3, y3); *p++ = 5;
        spr_index[spr_count].key = spr->z;
        spr_index[spr_count].index = spr_ptr;
        spr_index[spr_count].size = 13;
        spr_index[spr_count].id = spr->sprno;
        spr_ptr += 13;
    }
    spr_count++;
}

float nlSin(unsigned short r);
float nlCos(unsigned short r);

/* INLINE_STANDIN: the original inlines XYConv here (KR lines 180-182); XYConv is defined above without inline. */
static inline u_long nlsSprPut_XYConv(float x, float y)
{
    return (((int)x * 16 + 0x6BF8) | ((u_long)((int)y * 16 + 0x77F8) << 16)) | ((u_long)1000 << 32);
}

void nlsSprPut(NL_SPR_ARG *spr)
{
    u_long tex;
    u_long *p;
    int sizex;
    int sizey;
    float sx;
    float sy;
    float x0;
    float y0;
    float x1;
    float y1;
    float u0;
    float v0;
    float u1;
    float v1;

    p = (u_long *)&spr_buf[spr_ptr];
    x1 = y1 = x0 = y0 = 0.0f;
    tex = PXF_SetTexture(spr->sprno);
    if (tex == -1) {
        return;
    }
    PXF_GetSize(spr->sprno, &sizex, &sizey);
    sx = sizex * spr->zm_x;
    sy = sizey * spr->zm_y;
    if (spr->attr & 0x10) {
        u0 = spr->u1;
        u1 = spr->u0;
    } else {
        u0 = spr->u0;
        u1 = spr->u1;
    }
    if (spr->attr & 0x20) {
        v0 = spr->v1;
        v1 = spr->v0;
    } else {
        v0 = spr->v0;
        v1 = spr->v1;
    }
    if (spr->ang == 0) {
        if (CheckSpriteBuffer(9)) {
            return;
        }
        switch (spr->attr & 3) {
        case 1:
            x0 = spr->x;
            x1 = x0 + sx;
            break;
        case 2:
            x0 = spr->x - sx * 0.5f;
            x1 = spr->x + sx * 0.5f;
            break;
        case 3:
            x0 = spr->x - sx;
            x1 = spr->x;
            break;
        }
        switch ((spr->attr >> 2) & 3) {
        case 1:
            y0 = spr->y;
            y1 = y0 + sy;
            break;
        case 2:
            y0 = spr->y - sy * 0.5f;
            y1 = spr->y + sy * 0.5f;
            break;
        case 3:
            y0 = spr->y - sy;
            y1 = spr->y;
            break;
        }
        if (XYCheck(x0, y0)) {
            return;
        }
        *p++ = 0x1000000000000008; *p++ = 0xE;
        *p++ = tex; *p++ = 6;
        *p++ = SpriteAlpha; *p++ = 0x42;
        *p++ = 0x1D6; *p++ = 0;
        *p++ = RGBConv(0xFFFFFFFF); *p++ = 1;
        *p++ = UVConv(u0, v0); *p++ = 3;
        *p++ = nlsSprPut_XYConv(x0, y0); *p++ = 5;
        *p++ = UVConv(u1, v1); *p++ = 3;
        *p++ = nlsSprPut_XYConv(x1, y1); *p++ = 5;
        spr_index[spr_count].key = spr->z;
        spr_index[spr_count].index = spr_ptr;
        spr_index[spr_count].size = 9;
        spr_index[spr_count].id = spr->sprno;
        spr_ptr += 9;
    } else {
        float xd0 = 0.0f, yd0 = 0.0f, xd1 = 0.0f, yd1 = 0.0f, x2 = 0.0f, y2 = 0.0f, x3 = 0.0f, y3 = 0.0f;

        if (CheckSpriteBuffer(13)) {
            return;
        }
        switch (spr->attr & 3) {
        case 1:
            xd0 = 0.0f;
            xd1 = sx;
            break;
        case 2:
            xd0 = -sx * 0.5f;
            xd1 = sx * 0.5f;
            break;
        case 3:
            xd0 = -sx;
            break;
        }
        switch ((spr->attr >> 2) & 3) {
        case 1:
            yd0 = 0.0f;
            yd1 = sy;
            break;
        case 2:
            yd0 = -sy * 0.5f;
            yd1 = sy * 0.5f;
            break;
        case 3:
            yd0 = -sy;
            yd1 = 0.0f;
            break;
        }
        x0 = spr->x + nlCos(spr->ang) * xd0 + nlSin(spr->ang) * yd0;
        y0 = spr->y + nlCos(spr->ang) * yd0 - nlSin(spr->ang) * xd0;
        x1 = spr->x + nlCos(spr->ang) * xd1 + nlSin(spr->ang) * yd0;
        y1 = spr->y + nlCos(spr->ang) * yd0 - nlSin(spr->ang) * xd1;
        x2 = spr->x + nlCos(spr->ang) * xd0 + nlSin(spr->ang) * yd1;
        y2 = spr->y + nlCos(spr->ang) * yd1 - nlSin(spr->ang) * xd0;
        x3 = spr->x + nlCos(spr->ang) * xd1 + nlSin(spr->ang) * yd1;
        y3 = spr->y + nlCos(spr->ang) * yd1 - nlSin(spr->ang) * xd1;
        if (XYCheck(x0, y0)) {
            return;
        }
        *p++ = 0x100000000000000C; *p++ = 0xE;
        *p++ = tex; *p++ = 6;
        *p++ = SpriteAlpha; *p++ = 0x42;
        *p++ = 0x1D4; *p++ = 0;
        *p++ = RGBConv(0xFFFFFFFF); *p++ = 1;
        *p++ = UVConv(u0, v0); *p++ = 3;
        *p++ = nlsSprPut_XYConv(x0, y0); *p++ = 5;
        *p++ = UVConv(u1, v0); *p++ = 3;
        *p++ = nlsSprPut_XYConv(x1, y1); *p++ = 5;
        *p++ = UVConv(u0, v1); *p++ = 3;
        *p++ = nlsSprPut_XYConv(x2, y2); *p++ = 5;
        *p++ = UVConv(u1, v1); *p++ = 3;
        *p++ = nlsSprPut_XYConv(x3, y3); *p++ = 5;
        spr_index[spr_count].key = spr->z;
        spr_index[spr_count].index = spr_ptr;
        spr_index[spr_count].size = 13;
        spr_index[spr_count].id = spr->sprno;
        spr_ptr += 13;
    }
    spr_count++;
}
