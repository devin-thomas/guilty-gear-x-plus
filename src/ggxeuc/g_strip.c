#include "generated/g_strip.h"
static unsigned int Gcol;
static unsigned int Gcolsv;
static int OffsetOn;
static float gfadeval;
static NLVERTEX03 gStripBuffer_BG[4];
void *smalloc(int size, char *name);
void nlDispStripTexBG(NLVERTEX03 *vt, int spr);
void nlDispStripTexTrnslBG(NLVERTEX03 *vt, int spr);
void nlDispStripTexOfsTrnsl(int num, NLVERTEX03 *vt, int spr);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);
void nlsDispStripTexOfsTrnsl(int num, NLVERTEX03 *vt, int spr);
void nlsDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);
extern int TotalCounter;
float nlSin(u_short r);

void g_strip_init(void)
{
    Vertex03Work = smalloc(0x400, "Vertex03Work");
}

void gStripSetPalTb(char *tb) {
}

void gCurrentColorSet(float val)
{
    int i;

    i = gfadeval * val * 255.0f;
    Gcol = (i << 16) | (i << 8) | i;
}

void gCurrentColorSetARBG(unsigned int arbg)
{
    unsigned int r;
    unsigned int g;
    unsigned int b;

    r = ((arbg >> 16) & 0xFF) * gfadeval;
    g = ((arbg >> 8) & 0xFF) * gfadeval;
    b = (arbg & 0xFF) * gfadeval;
    Gcol = (r << 16) | (g << 8) | b;
}

void gCurrentColorSetARBGFade(unsigned int arbg, float val)
{
    unsigned int r;
    unsigned int g;
    unsigned int b;
    float f;

    f = gfadeval * val;
    r = ((arbg >> 16) & 0xFF) * f;
    g = ((arbg >> 8) & 0xFF) * f;
    b = (arbg & 0xFF) * f;
    Gcol = (r << 16) | (g << 8) | b;
}

void gCurrentColorClear(void)
{
    gCurrentColorSet(1.0f);
}

void gFadeValSet(float val)
{
    gfadeval = val;
    gCurrentColorClear();
}

void gColPop(void)
{
    Gcol = Gcolsv;
}

void gColPush(unsigned int color)
{
    Gcolsv = Gcol;
    Gcol = color;
}

void gSetObjColorOffset(unsigned int val)
{
    int i;

    if (val) {
        OffsetOn = 1;
        for (i = 0; i < 4; i++) {
            Vertex03Work[i].ofs = val;
        }
    } else {
        OffsetOn = 0;
        for (i = 0; i < 4; i++) {
            Vertex03Work[i].ofs = 0;
        }
    }
}

unsigned int gCurrentColorGet(void)
{
    return Gcol;
}

void DrawStripSubS(int sprno)
{
    if (OffsetOn) {
        nlsDispStripTexOfsTrnsl(4, Vertex03Work, sprno);
    } else {
        nlsDispStripTexTrnsl(4, Vertex03Work, sprno);
    }
}

void DrawStripSub5(int sprno)
{
    if (OffsetOn) {
        nlDispStripTexOfsTrnsl(5, Vertex03Work, sprno);
    } else {
        nlDispStripTexTrnsl(5, Vertex03Work, sprno);
    }
}

void gSpriteDraw(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    int sx;
    int sy;
    float sxf;
    float syf;
    float w;
    float h;
    unsigned int col;

    z = sprite->z;
    x = sprite->x;
    y = sprite->y;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    sxf = sx;
    syf = sy;
    w = sxf * sprite->zm_x;
    h = syf * sprite->zm_y;
    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    if (sprite->attr & 0x10) {
        p = Vertex03Work;
        p->x = x;
        p->y = y;
        p->z = z;
        p->u = 0;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x - w;
        p->y = y;
        p->z = z;
        p->u = sxf;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x;
        p->y = y - h;
        p->z = z;
        p->u = 0;
        p->v = 0;
        p->col = col;
        p++;
        p->x = x - w;
        p->y = y - h;
        p->z = z;
        p->u = sxf;
        p->v = 0;
        p->col = col;
    } else {
        p = Vertex03Work;
        p->x = x;
        p->y = y;
        p->z = z;
        p->u = 0;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x + w;
        p->y = y;
        p->z = z;
        p->u = sxf;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x;
        p->y = y - h;
        p->z = z;
        p->u = 0;
        p->v = 0;
        p->col = col;
        p++;
        p->x = x + w;
        p->y = y - h;
        p->z = z;
        p->u = sxf;
        p->v = 0;
        p->col = col;
    }
    DrawStripSubS(sprite->sprno);
}

void gSpriteDrawUVSph(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    int sx;
    int sy;
    float sxf;
    float syf;
    float u0;
    float u1;
    float v0;
    float v1;
    float x2;
    float y2;
    unsigned int col;

    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    sxf = sx;
    syf = sy;
    u1 = sprite->u1 * sxf;
    u0 = sprite->u0 * sxf;
    v0 = sprite->v0 * syf;
    v1 = sprite->v1 * syf;
    if (u1 > sxf) {
        u1 = sxf;
    }
    if (v1 > syf) {
        v1 = syf;
    }
    if (sprite->attr & 0x10) {
        x2 = x - sprite->zm_x;
    } else {
        x2 = x + sprite->zm_x;
    }
    col = ((int)(sprite->trnsl * 255.0f) << 24) | Gcol;
    y2 = y - sprite->zm_y;
    p = Vertex03Work;
    p->x = x;
    p->y = y;
    p->z = z;
    p->u = u0;
    p->v = v0;
    p->col = col;
    p++;
    p->x = x2;
    p->y = y;
    p->z = z;
    p->u = u1;
    p->v = v0;
    p->col = col;
    p++;
    p->x = x;
    p->y = y2;
    p->z = z;
    p->u = u0;
    p->v = v1;
    p->col = col;
    p++;
    p->x = x2;
    p->y = y2;
    p->z = z;
    p->u = u1;
    p->v = v1;
    p->col = col;
    if (gStripDebug) {
        debugf("gSpriteDrawUVSph( %d ) %f %f %f %f\n", sprite->sprno, u0, u1, x, x2);
    }
    DrawStripSubS(sprite->sprno);
}

void gSpriteDrawUV(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    int sx;
    int sy;
    float sxf;
    float syf;
    float u0;
    float u1;
    float v0;
    float v1;
    float zx;
    float zy;
    unsigned int col;

    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    sxf = sx;
    syf = sy;
    u1 = sprite->u1 * sxf;
    u0 = sprite->u0 * sxf;
    v1 = -sprite->v1 * syf;
    v0 = -sprite->v0 * syf;
    if (u1 > sxf) {
        u1 = sxf;
    }
    if (v0 > syf) {
        v0 = syf;
    }
    if (v1 > syf) {
        v1 = syf;
    }
    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    zx = sprite->zm_x;
    zy = sprite->zm_y;
    if (sprite->attr & 0x20) {
        zy = -zy;
    }
    if (sprite->attr & 0x10) {
        zx = x - zx;
    } else {
        zx = x + zx;
    }
    zy = y - zy;
    p = Vertex03Work;
    p->x = x;
    p->y = y;
    p->z = z;
    p->u = u0;
    p->v = v0;
    p->col = col;
    p++;
    p->x = zx;
    p->y = y;
    p->z = z;
    p->u = u1;
    p->v = v0;
    p->col = col;
    p++;
    p->x = x;
    p->y = zy;
    p->z = z;
    p->u = u0;
    p->v = v1;
    p->col = col;
    p++;
    p->x = zx;
    p->y = zy;
    p->z = z;
    p->u = u1;
    p->v = v1;
    p->col = col;
    if (gStripDebug) {
        debugf("gSpriteDrawUV( %d ) %f %f %f %f\n", sprite->sprno, u0, u1, y, zy);
    }
    DrawStripSubS(sprite->sprno);
}

void gSpriteDrawWH(G_SPR_ARGXYWH *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    float u0;
    float v0;
    float u1;
    float v1;
    float sizex;
    float sizey;
    int sx;
    int sy;
    unsigned int col;

    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    u0 = sprite->u0;
    u1 = sprite->u1;
    v0 = sprite->v0;
    v1 = sprite->v1;
    col = sprite->trnsl;
    col <<= 24;
    col |= Gcol;
    sizex = sprite->zm_x * (u1 - u0);
    sizey = sprite->zm_y * (v1 - v0);
    p = Vertex03Work;
    p->x = x;
    p->y = y;
    p->z = z;
    p->u = u0;
    p->v = v0;
    if (sprite->attr & 1) {
        p->col = col & 0xFFFFFF;
    } else {
        p->col = col;
    }
    p++;
    p->x = x + sizex;
    p->y = y;
    p->z = z;
    p->u = u1;
    p->v = v0;
    if (sprite->attr & 1) {
        p->col = col & 0xFFFFFF;
    } else {
        p->col = col;
    }
    p++;
    p->x = x;
    p->y = y + sizey;
    p->z = z;
    p->u = u0;
    p->v = v1;
    p->col = col;
    p++;
    p->x = x + sizex;
    p->y = y + sizey;
    p->z = z;
    p->u = u1;
    p->v = v1;
    p->col = col;
    DrawStripSubS(sprite->sprno);
}

void gSpriteDrawUVAngle(NL_SPR_ARG *sprite, float *core, u_short angle)
{
    NLVERTEX03 *p;
    NL_POINT3 pt[4];
    NL_POINT3 *pp;
    float x;
    float y;
    float z;
    int sx;
    int sy;
    float sxf;
    float syf;
    float u0;
    float u1;
    float v0;
    float v1;
    float zx;
    float zy;
    float x2;
    float y2;
    unsigned int col;
    int i;

    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    zy = sprite->zm_y;
    zx = sprite->zm_x;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    sxf = sx;
    syf = sy;
    u1 = sprite->u1 * sxf;
    u0 = sprite->u0 * sxf;
    v0 = -sprite->v0 * syf;
    v1 = -sprite->v1 * syf;
    if (u1 > sxf) {
        u1 = sxf;
    }
    if (v1 > syf) {
        v1 = syf;
    }
    p = Vertex03Work;
    if (sprite->attr & 0x10) {
        zx = -zx;
        angle = -angle;
    }
    x2 = x + zx;
    y2 = y - zy;
    pp = pt;
    pp->x = x - core[0];
    pp->y = y - core[1];
    pp->z = z;
    p->col = col;
    p->u = u0;
    p->v = v0;
    p++;
    pp++;
    pp->x = x2 - core[0];
    pp->y = y - core[1];
    pp->z = z;
    p->col = col;
    p->u = u1;
    p->v = v0;
    p++;
    pp++;
    pp->x = x - core[0];
    pp->y = y2 - core[1];
    pp->z = z;
    p->col = col;
    p->u = u0;
    p->v = v1;
    p++;
    pp++;
    pp->x = x2 - core[0];
    pp->y = y2 - core[1];
    pp->z = z;
    p->col = col;
    p->u = u1;
    p->v = v1;
    nlUnitMatrix();
    nlRotateAZ(angle);
    p = Vertex03Work;
    pp = pt;
    for (i = 0; i < 4; i++, pp++, p++) {
        nlCalcPoint(pp, (NL_POINT3 *)p);
        p->x += core[0];
        p->y += core[1];
    }
    if (gStripDebug) {
        debugf("gSpriteDrawUVAngle( %d ) %f %f %f %f\n", sprite->sprno, u0, u1, x, zx);
    }
    DrawStripSubS(sprite->sprno);
}

void gStripDrawTop(float *v, short *sz, int id, int dirflag)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    float sizex;
    float sizey;
    int sx;
    int sy;
    float sxf;
    float syf;
    unsigned int col;

    z = v[2];
    sizex = sz[0];
    sizey = sz[1];
    y = v[1];
    x = v[0];
    col = Gcol | 0xFF000000;
    p = Vertex03Work;
    PXF_GetSize(id, &sx, &sy);
    sxf = sx;
    syf = sy;
    if (dirflag) {
        p->x = x;
        p->y = y + sizey;
        p->z = z;
        p->u = 0;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x - sizex;
        p->y = y + sizey;
        p->z = z;
        p->u = sxf;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x;
        p->y = y;
        p->z = z;
        p->u = 0;
        p->v = 0;
        p->col = col;
        p++;
        p->x = x - sizex;
        p->y = y;
        p->z = z;
        p->u = sxf;
        p->v = 0;
        p->col = col;
    } else {
        p->x = x;
        p->y = y + sizey;
        p->z = z;
        p->u = 0;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x + sizex;
        p->y = y + sizey;
        p->z = z;
        p->u = sxf;
        p->v = syf;
        p->col = col;
        p++;
        p->x = x;
        p->y = y;
        p->z = z;
        p->u = 0;
        p->v = 0;
        p->col = col;
        p++;
        p->x = x + sizex;
        p->y = y;
        p->z = z;
        p->u = sxf;
        p->v = 0;
        p->col = col;
    }
    DrawStripSubS(id);
}

void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    int sx;
    int sy;
    float sxf;
    float syf;
    float w;
    float h;
    float u0;
    float u1;
    float v0;
    float v1;
    unsigned int col;
    int al;

    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    sxf = sx;
    syf = sy;
    w = sxf * sprite->zm_x;
    h = syf * sprite->zm_y;
    if (sprite->attr & 0x10) {
        u0 = sprite->u1 * sxf;
        u1 = sprite->u0 * sxf;
    } else {
        u0 = sprite->u0 * sxf;
        u1 = sprite->u1 * sxf;
    }
    if (sprite->attr & 0x20) {
        v0 = sprite->v1 * syf;
        v1 = sprite->v0 * syf;
    } else {
        v0 = sprite->v0 * syf;
        v1 = sprite->v1 * syf;
    }
    if (u0 > sxf) {
        u0 = sxf;
    }
    if (u1 > sxf) {
        u1 = sxf;
    }
    if (v0 > syf) {
        v0 = syf;
    }
    if (v1 > syf) {
        v1 = syf;
    }
    al = sprite->attr & 3;
    if (al == 1) {
        p = Vertex03Work;
        p->x = x;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x + w;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x + w;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    } else if (al == 2) {
        p = Vertex03Work;
        p->x = x - w * 0.5f;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x + w * 0.5f;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x - w * 0.5f;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x + w * 0.5f;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    } else {
        p = Vertex03Work;
        p->x = x - w;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x - w;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    }
    al = (sprite->attr >> 2) & 3;
    if (al == 1) {
        p = Vertex03Work;
        p->y = y;
        p++;
        p->y = y;
        p++;
        p->y = y + h;
        p++;
        p->y = y + h;
    } else if (al == 2) {
        p = Vertex03Work;
        p->y = y - h * 0.5f;
        p++;
        p->y = y - h * 0.5f;
        p++;
        p->y = y + h * 0.5f;
        p++;
        p->y = y + h * 0.5f;
    } else {
        p = Vertex03Work;
        p->y = y - h;
        p++;
        p->y = y - h;
        p++;
        p->y = y;
        p++;
        p->y = y;
    }
    if (sprite->listType == 0) {
        nlsDispStripTex(4, Vertex03Work, sprite->sprno);
    } else if (sprite->listType == 1) {
        DrawStripSubS(sprite->sprno);
    } else {
        DrawStripSubS(sprite->sprno);
    }
}

void gSpriteDrawPS(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    float w;
    float h;
    float u0;
    float u1;
    float v0;
    float v1;
    unsigned int col;
    int al;

    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    w = sprite->zm_x;
    h = sprite->zm_y;
    if (sprite->attr & 0x10) {
        u0 = sprite->u1;
        u1 = sprite->u0;
    } else {
        u0 = sprite->u0;
        u1 = sprite->u1;
    }
    if (sprite->attr & 0x20) {
        v0 = sprite->v1;
        v1 = sprite->v0;
    } else {
        v0 = sprite->v0;
        v1 = sprite->v1;
    }
    al = sprite->attr & 3;
    if (al == 1) {
        p = Vertex03Work;
        p->x = x;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x + w;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x + w;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    } else if (al == 2) {
        p = Vertex03Work;
        p->x = x - w * 0.5f;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x + w * 0.5f;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x - w * 0.5f;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x + w * 0.5f;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    } else {
        p = Vertex03Work;
        p->x = x - w;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x - w;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    }
    al = (sprite->attr >> 2) & 3;
    if (al == 1) {
        p = Vertex03Work;
        p->y = y;
        p++;
        p->y = y;
        p++;
        p->y = y + h;
        p++;
        p->y = y + h;
    } else if (al == 2) {
        p = Vertex03Work;
        p->y = y - h * 0.5f;
        p++;
        p->y = y - h * 0.5f;
        p++;
        p->y = y + h * 0.5f;
        p++;
        p->y = y + h * 0.5f;
    } else {
        p = Vertex03Work;
        p->y = y - h;
        p++;
        p->y = y - h;
        p++;
        p->y = y;
        p++;
        p->y = y;
    }
    if (sprite->listType == 0) {
        nlsDispStripTex(4, Vertex03Work, sprite->sprno);
    } else if (sprite->listType == 1) {
        DrawStripSubS(sprite->sprno);
    } else {
        DrawStripSubS(sprite->sprno);
    }
}

void gSpriteDraw_Angle(NL_SPR_ARG *sprite, int cx, int cy, int angle)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    int sx;
    int sy;
    float sxf;
    float syf;
    float w;
    float h;
    float u0;
    float u1;
    float v0;
    float v1;
    unsigned int col;
    int al;

    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    PXF_GetSize(sprite->sprno, &sx, &sy);
    sxf = sx;
    syf = sy;
    w = sxf * sprite->zm_x;
    h = syf * sprite->zm_y;
    if (sprite->attr & 0x10) {
        u0 = sprite->u1 * sxf;
        u1 = sprite->u0 * sxf;
    } else {
        u0 = sprite->u0 * sxf;
        u1 = sprite->u1 * sxf;
    }
    if (sprite->attr & 0x20) {
        v0 = -sprite->v1 * syf;
        v1 = -sprite->v0 * syf;
    } else {
        v0 = -sprite->v0 * syf;
        v1 = -sprite->v1 * syf;
    }
    al = sprite->attr & 3;
    if (al == 1) {
        p = Vertex03Work;
        p->x = x;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x + w;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x + w;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    } else if (al == 2) {
        p = Vertex03Work;
        p->x = x - w * 0.5f;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x + w * 0.5f;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x - w * 0.5f;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x + w * 0.5f;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    } else {
        p = Vertex03Work;
        p->x = x - w;
        p->z = z;
        p->u = u0;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u1;
        p->v = v0;
        p->col = col;
        p++;
        p->x = x - w;
        p->z = z;
        p->u = u0;
        p->v = v1;
        p->col = col;
        p++;
        p->x = x;
        p->z = z;
        p->u = u1;
        p->v = v1;
        p->col = col;
    }
    al = (sprite->attr >> 2) & 3;
    if (al == 1) {
        p = Vertex03Work;
        p->y = y;
        p++;
        p->y = y;
        p++;
        p->y = y + h;
        p++;
        p->y = y + h;
    } else if (al == 2) {
        p = Vertex03Work;
        p->y = y - h * 0.5f;
        p++;
        p->y = y - h * 0.5f;
        p++;
        p->y = y + h * 0.5f;
        p++;
        p->y = y + h * 0.5f;
    } else {
        p = Vertex03Work;
        p->y = y - h;
        p++;
        p->y = y - h;
        p++;
        p->y = y;
        p++;
        p->y = y;
    }
    DrawStripSubS(sprite->sprno);
}

void gWaveStripDraw(NL_SPR_ARG *sprite, int sz)
{
    NLVERTEX03 *p;
    int i;
    int ang;
    float famp;
    float x;
    float x2;
    float z;
    float hgt;
    float yy;
    float v;
    float s;
    unsigned int col;

    ang = (TotalCounter & 0x7F) << 9;
    i = TotalCounter & 0x3F;
    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    x = sprite->x;
    z = sprite->z;
    x2 = sprite->zm_x;
    hgt = sprite->zm_y - sprite->y;
    col |= Gcol;
    if (TotalCounter & 0x40) {
        i = 0x40 - i;
    }
    famp = (i + 0x40) * 0.015625f;
    p = Vertex03Work;
    for (i = 0; i < 16; i++) {
        yy = sprite->y + hgt * i / 15.0f;
        v = (float)i / 15.0f;
        s = nlSin((ang + (i << 12)) & 0xFE00) * famp;
        p->x = x + s;
        p->y = yy;
        p->z = z;
        p->u = 0;
        p->v = v;
        p->col = col;
        p++;
        p->x = x2 + s;
        p->y = yy;
        p->z = z;
        p->u = 1.0f;
        p->v = v;
        p->col = col;
        p++;
    }
    nlDispStripTexTrnsl(32, Vertex03Work, sprite->sprno);
}

void gStripDraw_BGFast(NL_SPR_ARG *sprite)
{
    float x;
    float y;
    float x2;
    float y2;

    x = sprite->x;
    y = sprite->y;
    x2 = sprite->zm_x;
    y2 = sprite->zm_y;
    gStripBuffer_BG[0].x = x;
    gStripBuffer_BG[0].y = y;
    gStripBuffer_BG[1].x = x2;
    gStripBuffer_BG[1].y = y;
    gStripBuffer_BG[2].x = x;
    gStripBuffer_BG[2].y = y2;
    gStripBuffer_BG[3].x = x2;
    gStripBuffer_BG[3].y = y2;
    if (sprite->listType == 1) {
        nlDispStripTexTrnslBG(gStripBuffer_BG, sprite->sprno);
    } else {
        nlDispStripTexBG(gStripBuffer_BG, sprite->sprno);
    }
}

void gStripDraw_BGFastSkipY(NL_SPR_ARG *sprite)
{
    float x;
    float x2;

    x = sprite->x;
    x2 = sprite->zm_x;
    gStripBuffer_BG[0].x = x;
    gStripBuffer_BG[1].x = x2;
    gStripBuffer_BG[2].x = x;
    gStripBuffer_BG[3].x = x2;
    if (sprite->listType == 1) {
        nlDispStripTexTrnslBG(gStripBuffer_BG, sprite->sprno);
    } else {
        nlDispStripTexBG(gStripBuffer_BG, sprite->sprno);
    }
}

void gStripDraw_BGFastSkipY_TRS(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float x2;

    x = sprite->x;
    x2 = sprite->zm_x;
    p = gStripBuffer_BG;
    p->x = x;
    p++;
    p->x = x2;
    p++;
    p->x = x;
    p++;
    p->x = x2;
    nlDispStripTexTrnslBG(gStripBuffer_BG, sprite->sprno);
}

void gStripDraw_BGFastSkipY_OPQ(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float x2;

    x = sprite->x;
    x2 = sprite->zm_x;
    p = gStripBuffer_BG;
    p->x = x;
    p++;
    p->x = x2;
    p++;
    p->x = x;
    p++;
    p->x = x2;
    nlDispStripTexBG(gStripBuffer_BG, sprite->sprno);
}

void gStripDraw_BGFastReverse(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float x2;
    float y2;
    int i;
    NLVERTEX03 *q;
    NLVERTEX03 gStripBuffer_BG2[4];

    x = sprite->x;
    y = sprite->y;
    x2 = sprite->zm_x;
    y2 = sprite->zm_y;
    p = gStripBuffer_BG2;
    q = gStripBuffer_BG;
    for (i = 0; i < 4; i++) {
        p[i].col = q->col;
        p[i].z = q->z;
        p[i].ofs = q->ofs;
        q++;
    }
    p[0].u = p[2].u = 0;
    p[1].u = p[3].u = 1.0f;
    p[0].v = p[1].v = 0;
    p[2].v = p[3].v = 1.0f;
    p->x = x2;
    p->y = y;
    p++;
    p->x = x;
    p->y = y;
    p++;
    p->x = x2;
    p->y = y2;
    p++;
    p->x = x;
    p->y = y2;
    if (sprite->listType == 0) {
        nlDispStripTexBG(gStripBuffer_BG2, sprite->sprno);
    } else {
        nlDispStripTexTrnslBG(gStripBuffer_BG2, sprite->sprno);
    }
}

void gStripDraw_Set_z_transl(float z, float trnsl)
{
    NLVERTEX03 *p;
    unsigned int col;

    col = ((int)(trnsl * 255.0) << 24) | Gcol;
    p = gStripBuffer_BG;
    p->z = z;
    p->col = col;
    p->ofs = 0;
    p++;
    p->z = z;
    p->col = col;
    p->ofs = 0;
    p++;
    p->z = z;
    p->col = col;
    p->ofs = 0;
    p++;
    p->z = z;
    p->col = col;
    p->ofs = 0;
}

void gStripDraw_Init(void)
{
    int i;

    gStripBuffer_BG[0].u = 0;
    gStripBuffer_BG[0].v = 0;
    gStripBuffer_BG[1].u = 1.0f;
    gStripBuffer_BG[1].v = 0;
    gStripBuffer_BG[2].u = 0;
    gStripBuffer_BG[2].v = 1.0f;
    gStripBuffer_BG[3].u = 1.0f;
    gStripBuffer_BG[3].v = 1.0f;
    gCurrentColorClear();
    i = 32;
    while (i--) {
        Vertex03Work[i].ofs = 0;
    }
}

void gStripSetPalette(int no) {
}

unsigned int gStripGetFadeColor(unsigned int color)
{
    unsigned int a, g, b;
    int val;
    val = Gcol & 0xFF;
    a = color & 0xFF000000;
    a |= (((color >> 16) & 0xFF) * val / 255) << 16;
    g = (((color >> 8) & 0xFF) * val / 255) << 8;
    b = (color & 0xFF) * val / 255;
    return a | (b | g);
}

void JY_CradDraw(NL_SPR_ARG *sprite, int acttimer)
{
    NLVERTEX03 *p;
    float x;
    float y;
    float z;
    float u0;
    float u1;
    float v0;
    float v1;
    float zx;
    float zy;
    float shift;
    float x1;
    float x2;
    float y1;
    float y2;
    unsigned int col;

    x = sprite->x;
    y = sprite->y;
    z = sprite->z;
    u0 = sprite->u0;
    u1 = sprite->u1;
    zx = sprite->zm_x;
    zy = sprite->zm_y;
    if (acttimer < 61) {
        acttimer = 0;
    } else {
        acttimer -= 60;
    }
    sprite->trnsl -= acttimer * 0.01;
    if (sprite->trnsl < 0) {
        sprite->trnsl = 0;
    }
    col = (unsigned int)(sprite->trnsl * 255.0f) << 24;
    col |= Gcol;
    v0 = -sprite->v0;
    v1 = -sprite->v1;
    if (sprite->attr & 0x20) {
        zy = -zy;
    }
    if (sprite->attr & 0x10) {
        zx = x - zx;
    } else {
        zx += x;
    }
    shift = acttimer * 0.4f;
    zy = y - zy;
    p = Vertex03Work;
    x1 = x - shift - 10.0f;
    x2 = zx - shift - 10.0f;
    y1 = y + shift + 10.0f;
    y2 = zy + shift + 10.0f;
    p->x = x1;
    p->y = y1;
    p->z = z;
    p->u = u0;
    p->v = v0;
    p->col = col;
    p++;
    p->x = x2;
    p->y = y1;
    p->z = z;
    p->u = u1;
    p->v = v0;
    p->col = col;
    p++;
    p->x = x1;
    p->y = y1 + (y2 - y1) * 0.3f;
    p->z = z;
    p->u = u0;
    p->v = v0 + (v1 - v0) * 0.3f;
    p->col = col;
    p++;
    p->x = x2;
    p->y = y1 + (y2 - y1) * 0.3f;
    p->z = z;
    p->u = u1;
    p->v = v0 + (v1 - v0) * 0.3f;
    p->col = col;
    p++;
    p->x = x1;
    p->y = y1 + (y2 - y1) * 0.7f;
    p->z = z;
    p->u = u0;
    p->v = v0 + (v1 - v0) * 0.7f;
    p->col = col;
    DrawStripSub5(sprite->sprno);
    p = Vertex03Work;
    y2 = zy - shift - 10.0f;
    x2 = zx + shift + 10.0f;
    x1 = x + shift + 10.0f;
    y1 = y - shift - 10.0f;
    p->x = x2;
    p->y = y2;
    p->z = z;
    p->u = u1;
    p->v = v1;
    p->col = col;
    p++;
    p->x = x1;
    p->y = y2;
    p->z = z;
    p->u = u0;
    p->v = v1;
    p->col = col;
    p++;
    p->x = x2;
    p->y = y2 + (y1 - y2) * 0.3;
    p->z = z;
    p->u = u1;
    p->v = v1 + (v0 - v1) * 0.3f;
    p->col = col;
    p++;
    p->x = x1;
    p->y = y2 + (y1 - y2) * 0.3;
    p->z = z;
    p->u = u0;
    p->v = v1 + (v0 - v1) * 0.3f;
    p->col = col;
    p++;
    p->x = x2;
    p->y = y2 + (y1 - y2) * 0.7;
    p->z = z;
    p->u = u1;
    p->v = v1 + (v0 - v1) * 0.7f;
    p->col = col;
    DrawStripSub5(sprite->sprno);
    p++;
}

void StripDebug(int sw)
{
    gStripDebug = sw;
}

void gStripDraw_BG(NL_SPR_ARG *sprite)
{
    NLVERTEX03 *p;
    float u1;
    float x;
    float y;
    float z;
    float x2;
    float y2;
    float u0;
    float v0;
    float v1;
    unsigned int col;

    x = sprite->x; y = sprite->y; z = sprite->z;
    x2 = sprite->zm_x; y2 = sprite->zm_y;
    col = (int)(sprite->trnsl * 255.0f);
    col <<= 24;
    col |= Gcol;
    u0 = sprite->u0; u1 = sprite->u1;
    v0 = -sprite->v0; v1 = -sprite->v1;
    p = Vertex03Work;
    p->x = x; p->y = y; p->z = z; p->u = u0; p->v = v0; p->col = col; p++;
    p->x = x2; p->y = y; p->z = z; p->u = u1; p->v = v0; p->col = col; p++;
    p->x = x; p->y = y2; p->z = z; p->u = u0; p->v = v1; p->col = col; p++;
    p->x = x2; p->y = y2; p->z = z; p->u = u1; p->v = v1; p->col = col;
    if (sprite->listType == 0) {
        nlDispStripTexBG(Vertex03Work, sprite->sprno);
    } else if (sprite->listType == 1) {
        nlDispStripTexTrnslBG(Vertex03Work, sprite->sprno);
    } else {
        nlDispStripTexTrnslBG(Vertex03Work, sprite->sprno);
    }
}
