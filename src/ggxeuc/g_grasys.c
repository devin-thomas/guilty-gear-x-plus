#include "generated/g_grasys.h"

void SetPaltbNo(unsigned int id, unsigned int palid) {
}

void set_garbage_texture(void) {
}

static int ReloadStack[4];
static int ReloadType[4];
static int DecodeWH[2];
static unsigned short PVR_MinWidth;
static unsigned short PVR_MinHeight;

int PVR_Load(char *IstAddr, unsigned int id)
{
    return 0;
}

int PVR_ReLoad(char *IstAddr, unsigned int id)
{
    return 0;
}

int IST_Load_Bt(int *IstAddrTb, unsigned int id, unsigned int palid)
{
    return 0;
}

char *get_paladdr(void)
{
    return 0;
}

void PXF_SendPalette(char *PalAddr, unsigned int palid);

void CurrentPaletteLoad(unsigned int palid, char *PalAddr)
{
    PXF_SendPalette(PalAddr, palid);
}

int *Get_Decode_Size(void)
{
    return DecodeWH;
}

void SetPVRMinSize(unsigned short x, unsigned short y)
{
    PVR_MinWidth = x;
    PVR_MinHeight = y;
}

void ISTSetDualLoad(int flag)
{
    if (flag == 1) {
        DualLoad = flag;
        return;
    }
    DualLoad = 0;
}

void nlSprPut(NL_SPR_ARG *sprite);

void SpriteDraw(NL_SPR_ARG *sprite)
{
    if ((unsigned short)sprite->attr != 0xFFFF) {
        nlSprPut(sprite);
    }
}

void AddReloadStack(int id)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (ReloadStack[i] == -1) {
            ReloadStack[i] = id;
            ReloadType[i] = 0;
            return;
        }
    }
}

void AddSprtloadStack(int id)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (ReloadStack[i] == -1) {
            ReloadStack[i] = id;
            ReloadType[i] = 1;
            return;
        }
    }
}

void nlSetPaletteData(int id, u_short *tb, int num);

void ColorPalette_Load(unsigned short col, unsigned int palid)
{
    unsigned short coltb[16];
    int i;

    for (i = 15; i >= 0; i--) {
        coltb[i] = col;
    }
    nlSetPaletteData((palid << 4) | 1, coltb, 15);
}

void ReloadExecute(void)
{
    int i;

    while (DigestReloadStack()) {
    }
    for (i = 0; i < 4; i++) {
        ReloadStack[i] = -1;
    }
}

void SetBlendingMode(short mode);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void ReversColor(void)
{
    SetBlendingMode(3);
    polydraw_FBox(0, 0, 640.0f, 480.0f, 1.0f, 0xFEFFFFFF);
    SetBlendingMode(0);
}

int *GetSphByIST(char *IstAddr)
{
    IST_HEADER *IstHeader;
    unsigned short flag;

    IstHeader = (IST_HEADER *)IstAddr;
    IstAddr += 4;
    flag = IstHeader->id >> 16;
    if (flag & 1) {
        IstAddr += *(int *)IstAddr & 0xFFFFFF;
    }
    if (flag & 2) {
        IstAddr += *(int *)IstAddr & 0xFFFFFF;
    }
    if (flag & 4) {
        return (int *)IstAddr;
    }
    return 0;
}

void EasySpriteDrawTR(int id, int x, int y, float z, float width)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = width;
    sprite.listType = 1;
    sprite.attr = 6;
    sprite.zm_y = 1.0f;
    sprite.u0 = 0;
    sprite.u1 = 1.0f;
    sprite.v0 = 0;
    sprite.v1 = 1.0f;
    sprite.trnsl = 1.0f;
    sprite.ang = 0;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
void gSpriteDrawPS(NL_SPR_ARG *sprite);

void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = 1.0f;
    sprite.zm_y = 1.0f;
    sprite.u0 = 0;
    sprite.u1 = 1.0f;
    sprite.v0 = 0;
    sprite.ang = 0;
    sprite.v1 = 1.0f;
    sprite.listType = 1;
    sprite.attr = flg;
    sprite.trnsl = trnsl;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void EasySpriteDrawM(int id, int x, int y, float z, float u, int flg)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = 0.5f;
    sprite.zm_y = 1.0f;
    sprite.u0 = u;
    sprite.u1 = u + 0.5;
    sprite.v0 = 0;
    sprite.v1 = 1.0f;
    sprite.ang = 0;
    sprite.listType = 1;
    sprite.attr = flg;
    sprite.trnsl = 1.0f;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void EasySpriteDrawBustUp(int id, int x, int y, int u0, int u1, int v0, int v1, float z)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = u1 - u0;
    sprite.zm_y = v1 - v0;
    sprite.u0 = u0;
    sprite.u1 = u1;
    sprite.v0 = v0;
    sprite.v1 = v1;
    sprite.ang = 0;
    sprite.listType = 1;
    sprite.attr = 0x20E;
    sprite.trnsl = 1.0f;
    gSpriteDrawPS(&sprite);
}

void EasySpriteDrawVS(int id, int x, int y, float z)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = 160.0f;
    sprite.zm_y = 424.0f;
    sprite.u0 = 104.0f;
    sprite.u1 = 264.0f;
    sprite.v0 = 0;
    sprite.v1 = 424.0f;
    sprite.ang = 0;
    sprite.listType = 1;
    sprite.attr = 0x20E;
    sprite.trnsl = 1.0f;
    gSpriteDrawPS(&sprite);
}

unsigned int GetDCTexSize(unsigned short x, unsigned short y)
{
    unsigned short size;
    unsigned int w;

    size = PVR_MinWidth;
    if (size == 0) {
        size = 8;
    }
    while (size < x) {
        size <<= 1;
    }
    w = size;
    size = PVR_MinHeight;
    if (size == 0) {
        size = 8;
    }
    while (size < y) {
        size <<= 1;
    }
    return w | (size << 16);
}

int DigestReloadStack(void)
{
    int i;
    int id;

    for (i = 0; i < 4; i++) {
        if (ReloadStack[i] >= 0) {
            id = ReloadType[i];
            if (id == 0) {
                ReloadStack[i] = -2;
                return 1;
            }
            ReloadStack[i] = -2;
            return 1;
        }
    }
    return 0;
}

void gSpriteDraw(NL_SPR_ARG *sprite);

void SpriteDrawSph(NL_SPR_ARG *sprite, int *sphaddr)
{
    if ((unsigned short)sprite->attr != 0xFFFF) {
        do {
            gSpriteDraw(sprite);
        } while (0);
    }
}

void PXF_GetSize(int id, int *x, int *y);
void EasySpriteDrawNum(int id, int x, int y, int w, float z, int n)
{
    NL_SPR_ARG sprite;
    int one;
    int width;
    int height;
    int ten = 10;
    int tens;

    PXF_GetSize(id, &width, &height);
    if ((unsigned int)n >= 100) {
        return;
    }
    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = (float)w / width;
    sprite.zm_y = 1.0f;
    sprite.ang = 0;
    sprite.v0 = 0;
    sprite.v1 = 1.0f;
    sprite.listType = 1;
    sprite.attr = 0xD;
    sprite.trnsl = 1.0f;
    tens = n / 10;
    one = n;
    while (one >= ten) {
        one -= ten;
    }
    sprite.u0 = (float)tens * w / width;
    sprite.u1 = (float)(tens + 1) * w / width;
    gSpriteDraw_NaomiLibVersion(&sprite);
    sprite.x += w;
    sprite.u0 = (float)one * w / width;
    sprite.u1 = (float)(one + 1) * w / width;
    gSpriteDraw_NaomiLibVersion(&sprite);
}
