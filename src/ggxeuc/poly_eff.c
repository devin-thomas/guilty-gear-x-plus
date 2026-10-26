#include "generated/poly_eff.h"
void *smalloc(int size, char *name);
extern HIBANA_WORK hibanap[256];
extern HIBANA_TEMP hibanatemp[16];
extern NLVERTEX00 *nlvt;
static LIGHTNING_WORK *lwork;
extern int lightningtimer;
HIBANA_WORK *GetWorkPointer(int id);

HIBANA_WORK *GetWorkPointer(int id)
{
    return &hibanap[(id - 1) * 16];
}

float GetObjScale(void);
float ObjWorld2ScreenX(float x);
float ObjWorld2ScreenY(float y);
unsigned int gStripGetFadeColor(unsigned int color);
void nl1DispStripTrnsl(float z);
void nl2DispStripTrnsl(int num, NLVERTEX00 *vt);
void nl3DispStripTrnsl(void);
void SetBlendingMode(short int mode);
extern HIBANA_VERTEXDATA hibana_vertexdata[]; /* STATIC_EXTERN 0x5705a0 */

void HibanaDraw(CHARACTER_WORK *offset)
{
    HIBANA_WORK *base;
    int index;
    float osc;
    NLVERTEX00 *vp;
    int i;
    int num;
    HIBANA_VERTEXDATA *p;
    HIBANA_TEMP *hp;

    base = GetWorkPointer(offset->Localid);
    index = offset->HitBackx;
    osc = GetObjScale();
    for (i = 0; i < 16; i++) {
        index &= 0xF;
        hibanatemp[i].x = ObjWorld2ScreenX(base[index].x);
        hibanatemp[i].y = ObjWorld2ScreenY(base[index].y);
        hibanatemp[i].offx = base[index].offx * osc;
        hibanatemp[i].offy = base[index].offy * osc;
        index--;
    }
    SetBlendingMode(1);
    nl1DispStripTrnsl(offset->priority);
    p = hibana_vertexdata;
    while (p->num) {
        num = p->num;
        vp = nlvt;
        i = num;
        while (i--) {
            hp = &hibanatemp[p->index];
            vp->x = hp->x + hp->offx * p->v;
            vp->y = hp->y + hp->offy * p->v;
            vp->col = gStripGetFadeColor(p->color | 0x80000000);
            p++;
            vp++;
        }
        nl2DispStripTrnsl(num, nlvt);
    }
    nl3DispStripTrnsl();
}

float nlSqrt(float src);

void SetHibanaVertex(CHARACTER_WORK *offset)
{
    HIBANA_WORK *base;
    int prev;
    float tx;
    float ty;
    float f;

    base = GetWorkPointer(offset->Localid);
    prev = offset->HitBackx;
    offset->HitBackx = (prev + 1) & 0xF;
    base[(prev + 1) & 0xF].x = offset->posx;
    base[(prev + 1) & 0xF].y = offset->posy;
    tx = offset->posx - base[prev].x;
    ty = offset->posy - base[prev].y;
    f = tx * tx;
    f += ty * ty;
    f = nlSqrt(f);
    base[(prev + 1) & 0xF].offx = ty / f;
    base[(prev + 1) & 0xF].offy = -tx / f;
}

CHARACTER_WORK *SearchEffectWorkLocalid_IgnorePadid(CHARACTER_WORK *off, short unsigned int idno, int Localid);

int InitHibanaVertex(CHARACTER_WORK *offset)
{
    int i;

    offset->Localid = 0;
    for (i = 1; i < 17; i++) {
        if (SearchEffectWorkLocalid_IgnorePadid(offset, offset->idno, i) == 0) {
            offset->Localid = i;
            return 1;
        }
    }
    return 0;
}

void CellMain(CHARACTER_WORK *offset);
void ObjWorld2ScreenV(CHARACTER_WORK *offset, NL_POINT3 *p);
void nlUnitMatrix(void);
void nlRotateAX(u_short r);
void nlRotateAY(u_short r);
void gCurrentColorSetARBG(unsigned int arbg);
unsigned int gCurrentColorGet(void);
void gCurrentColorClear(void);
u_int RGBConv2(u_int rgb);
float G_Sin(short unsigned int r);
float G_Cos(short unsigned int r);
void nlCalcPoint(NL_POINT3 *p1, NL_POINT3 *p2);
void nlcDispStripTrnsl(int num, NLVERTEX00 *vt);

void PolySkyDashEffectDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[6];
    NL_POINT3 off;
    NLVERTEX00 vt[6];
    int r;
    int i;
    float s;
    float c;
    float r0;
    float r1;
    float r2;
    float z;
    float sv;

    CellMain(offset);
    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(0);
    nlUnitMatrix();
    nlRotateAX(0);
    if (offset->dirflag == 1) {
        nlRotateAY(0xCF00);
    } else {
        nlRotateAY(0x3100);
    }
    gCurrentColorSetARBG(0x80FF);
    vt[0].col = vt[1].col = RGBConv2(gCurrentColorGet());
    gCurrentColorSetARBG(0xD0E0FF);
    vt[2].col = vt[3].col = RGBConv2(gCurrentColorGet() | (offset->trans << 24));
    gCurrentColorSetARBG(0x80FF);
    vt[4].col = vt[5].col = RGBConv2(gCurrentColorGet() | ((offset->trans >> 3) << 24));
    r0 = offset->ActTimer * 3;
    r2 = offset->ActTimer * 5 + 80;
    r1 = r0 + (r2 - r0) * ((float)offset->ActTimer / 20.0);
    p[0].z = p[1].z = r0 * 0.2 + (r0 - 30.0) * 0.2;
    p[2].z = p[3].z = r1 * 0.2 + (r1 - 30.0) * 0.2;
    p[4].z = p[5].z = r2 * 0.2 + (r2 - 30.0) * 0.2;
    for (r = 0; r < 0x10000; r += 0x800) {
        s = G_Sin(r) * off.z;
        c = G_Cos(r) * off.z;
        p[0].x = s * r0;
        p[0].y = c * r0;
        p[2].x = s * r1;
        p[2].y = c * r1;
        p[4].x = s * r2;
        p[4].y = c * r2;
        s = G_Sin(r + 0x800) * off.z;
        c = G_Cos(r + 0x800) * off.z;
        p[1].x = s * r0;
        p[1].y = c * r0;
        p[3].x = s * r1;
        p[3].y = c * r1;
        p[5].x = s * r2;
        p[5].y = c * r2;
        for (i = 0; i < 6; i++) {
            NLVERTEX00 *v;

            nlCalcPoint(&p[i], (NL_POINT3 *)&vt[i]);
            v = &vt[i];
            sv = v->z;
            z = 500.0 / (sv + 500.0);
            v->x *= z;
            v->y *= z;
            v->x += off.x;
            v->y += off.y;
            if (sv < 0.0f) {
                v->z = (float)offset->priority - 8.0;
            } else {
                v->z = (float)offset->priority + 60.0;
            }
        }
        nlcDispStripTrnsl(6, vt);
    }
    gCurrentColorClear();
}

void PolyEffInit(void)
{
    nlvt = smalloc(0x440, "NLVERTEX00");
}

u_int RGBConv2(u_int rgb);
float G_Sin(short unsigned int r);
float G_Cos(short unsigned int r);
void nlcDispFan(int num, NLVERTEX00 *vt);
void nlcDispFanTrnsl(int num, NLVERTEX00 *vt);
void nlcDispStrip(int num, NLVERTEX00 *vt);
void nlcDispStripTrnsl(int num, NLVERTEX00 *vt);

void CircleDraw(NL_POINT3 *p, unsigned int c0, unsigned int c1, NL_POINT3 *offp)
{
    NLVERTEX00 *vp;
    u_short angle;
    int i;
    float offx;
    float offy;
    float r1;
    float r2;
    float z;
    float s;
    float c;

    vp = nlvt;
    offx = offp->x;
    offy = offp->y;
    r1 = p->y;
    r2 = p[1].y;
    z = p->z;
    c0 = RGBConv2(c0);
    c1 = RGBConv2(c1);
    vp->z = z;
    if (r1 == 0.0f) {
        vp->col = c0;
        vp->x = offx;
        vp->y = offy;
        vp++;
        vp->col = c1;
        angle = 0;
        for (i = 0; i < 33; i++) {
            s = G_Sin(angle);
            c = G_Cos(angle);
            vp->x = r2 * s + offx;
            vp->y = r2 * c + offy;
            angle += 0x800;
            vp++;
        }
        if ((c0 >> 24) == 0xFF && (c1 >> 24) == 0xFF) {
            nlcDispFan(34, nlvt);
        } else {
            nlcDispFanTrnsl(34, nlvt);
        }
    } else {
        vp++;
        angle = 0;
        for (i = 0; i < 33; i++) {
            s = G_Sin(angle);
            c = G_Cos(angle);
            vp[-1].x = r1 * s + offx;
            vp[-1].y = r1 * c + offy;
            vp[-1].col = c0;
            vp->x = r2 * s + offx;
            vp->y = r2 * c + offy;
            vp->col = c1;
            angle += 0x800;
            vp += 2;
        }
        if ((c0 >> 24) == 0xFF && (c1 >> 24) == 0xFF) {
            nlcDispStrip(66, nlvt);
        } else {
            nlcDispStripTrnsl(66, nlvt);
        }
    }
}

void EllipseDraw(NL_POINT3 *p, unsigned int c0, unsigned int c1, NL_POINT3 *offp)
{
    NLVERTEX00 *vp;
    u_short angle;
    int i;
    float offx;
    float offy;
    float rx1;
    float ry1;
    float rx2;
    float ry2;
    float z;
    float s;
    float c;

    vp = nlvt;
    offx = offp->x;
    offy = offp->y;
    rx1 = p->x;
    ry1 = p->y;
    rx2 = p[1].x;
    ry2 = p[1].y;
    z = p->z;
    c0 = RGBConv2(c0);
    c1 = RGBConv2(c1);
    vp->z = z;
    vp++;
    angle = 0;
    for (i = 0; i < 33; i++) {
        s = G_Sin(angle);
        c = G_Cos(angle);
        vp[-1].x = rx1 * s + offx;
        vp[-1].y = ry1 * c + offy;
        vp[-1].col = c0;
        vp->x = rx2 * s + offx;
        vp->y = ry2 * c + offy;
        vp->col = c1;
        angle += 0x800;
        vp += 2;
    }
    nlcDispStripTrnsl(66, nlvt);
}

void CircleDrawHalf(NL_POINT3 *p, unsigned int c0, unsigned int c1, NL_POINT3 *offp)
{
    NLVERTEX00 *vp;
    float offx;
    float offy;
    float r1;
    float r2;
    float z;

    vp = nlvt;
    offx = offp->x;
    offy = offp->y;
    r1 = p->y;
    r2 = p[1].y;
    z = p->z;
    c0 = RGBConv2(c0);
    c1 = RGBConv2(c1);
    vp->z = z;
    if (r1 == 0.0) {
        vp->x = 0.0f + offx;
        vp->y = 0.0f + offy;
        vp->col = c0;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = r2 + offy;
        vp->col = c1;
        vp++;
        vp->x = r2 * 0.707107f + offx;
        vp->y = r2 * 0.707107f + offy;
        vp++;
        vp->x = r2 + offx;
        vp->y = 0.0f + offy;
        vp++;
        vp->x = r2 * 0.707107f + offx;
        vp->y = -r2 * 0.707107f + offy;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = -r2 + offy;
        vp++;
        vp->x = -r2 * 0.707107f + offx;
        vp->y = -r2 * 0.707107f + offy;
        vp++;
        vp->x = -r2 + offx;
        vp->y = 0.0f + offy;
        vp++;
        vp->x = -r2 * 0.707107f + offx;
        vp->y = r2 * 0.707107f + offy;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = r2 + offy;
        nlcDispFanTrnsl(10, nlvt);
    } else {
        vp->x = 0.0f + offx;
        vp->y = r1 + offy;
        vp->col = c0;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = r2 + offy;
        vp->col = c1;
        vp++;
        vp->x = r1 * 0.707107f + offx;
        vp->y = r1 * 0.707107f + offy;
        vp->col = c0;
        vp++;
        vp->x = r2 * 0.707107f + offx;
        vp->y = r2 * 0.707107f + offy;
        vp->col = c1;
        vp++;
        vp->x = r1 + offx;
        vp->y = 0.0f + offy;
        vp->col = c0;
        vp++;
        vp->x = r2 + offx;
        vp->y = 0.0f + offy;
        vp->col = c1;
        vp++;
        vp->x = r1 * 0.707107f + offx;
        vp->y = r1 * -0.707107f + offy;
        vp->col = c0;
        vp++;
        vp->x = r2 * 0.707107f + offx;
        vp->y = r2 * -0.707107f + offy;
        vp->col = c1;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = -r1 + offy;
        vp->col = c0;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = -r2 + offy;
        vp->col = c1;
        vp++;
        vp->x = r1 * -0.707107f + offx;
        vp->y = r1 * -0.707107f + offy;
        vp->col = c0;
        vp++;
        vp->x = r2 * -0.707107f + offx;
        vp->y = r2 * -0.707107f + offy;
        vp->col = c1;
        vp++;
        vp->x = -r1 + offx;
        vp->y = 0.0f + offy;
        vp->col = c0;
        vp++;
        vp->x = -r2 + offx;
        vp->y = 0.0f + offy;
        vp->col = c1;
        vp++;
        vp->x = r1 * -0.707107f + offx;
        vp->y = r1 * 0.707107f + offy;
        vp->col = c0;
        vp++;
        vp->x = r2 * -0.707107f + offx;
        vp->y = r2 * 0.707107f + offy;
        vp->col = c1;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = r1 + offy;
        vp->col = c0;
        vp++;
        vp->x = 0.0f + offx;
        vp->y = r2 + offy;
        vp->col = c1;
        nlcDispStripTrnsl(18, nlvt);
    }
}

void PXF_GetSize(int id, int *x, int *y);
void nlsDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);

void CircleDrawTex(NL_POINT3 *p, unsigned int c0, NL_POINT3 *offp, int index)
{
    NLVERTEX03 vt[66];
    NLVERTEX03 *vp;
    u_short angle;
    int i;
    float offx;
    float offy;
    float r1;
    float r2;
    float z;
    float s;
    float c;
    int sx;
    int sy;
    float sxf;
    float syf;

    offx = offp->x;
    offy = offp->y;
    r1 = p->y;
    r2 = p[1].y;
    z = p->z;
    PXF_GetSize(index, &sx, &sy);
    sxf = sx;
    vt[0].z = z;
    syf = sy;
    vp = vt;
    for (i = 0, angle = 0; i < 33; i++) {
        s = G_Sin(angle);
        c = G_Cos(angle);
        vp->x = r1 * s + offx;
        vp->y = r1 * c + offy;
        vp->col = c0;
        if (i & 1) {
            vp->u = 0.0f;
        } else {
            vp->u = sxf;
        }
        vp->v = syf;
        vp++;
        vp->x = r2 * s + offx;
        vp->y = r2 * c + offy;
        vp->col = c0;
        if (i & 1) {
            vp->u = 0.0f;
        } else {
            vp->u = sxf;
        }
        vp->v = 0.0f;
        vp++;
        angle += 0x800;
    }
    nlsDispStripTexTrnsl(66, vt, index);
}

void ClearPoint3(NL_POINT3 *p, unsigned int cnt)
{
    while (cnt--) {
        p->x = 0.0f;
        p->y = 0.0f;
        p->z = 0.0f;
        p++;
    }
}

void FillPoint3Z(NL_POINT3 *p, unsigned int cnt, float z)
{
    p->z = z;
}

void HamonEffectDraw(CHARACTER_WORK *offset)
{
    int trs;
    NL_POINT3 p[2];
    NL_POINT3 off;
    float sc;

    sc = GetObjScale();
    trs = 255 - offset->ActTimer * 255 / 32;
    off.x = ObjWorld2ScreenX(offset->posx);
    off.y = ObjWorld2ScreenY(offset->posy);
    p[1].x = (offset->ActTimer * 12.0f + 60.0f) * sc;
    p[1].y = (offset->ActTimer * 3.0f + 15.0f) * sc;
    p[0].x = p[1].x * 0.7f;
    p[0].y = p[1].y * 0.7f;
    p[0].z = p[1].z = 200.0f;
    SetBlendingMode(0);
    EllipseDraw(p, gStripGetFadeColor(((trs / 4) << 24) | 0xFF0000), gStripGetFadeColor((trs << 24) | 0xFFFF00), &off);
}

void UkemiEffectDraw(CHARACTER_WORK *offset)
{
    int trs;
    NL_POINT3 p[2];
    NL_POINT3 off;

    trs = 255 - offset->ActTimer * 255 / 32;
    off.x = ObjWorld2ScreenX(offset->posx);
    off.y = ObjWorld2ScreenY(offset->posy);
    p[1].y = offset->ActTimer * 6.0f + 60.0f;
    p[0].y = p[1].y * 0.95f;
    p[0].x = p[1].x = 0.0f;
    p[0].z = p[1].z = 10.0f;
    CircleDraw(p, gStripGetFadeColor(0xFF), gStripGetFadeColor((trs << 24) | 0xFFFFFF), &off);
    p[0].y = p[1].y + 2.0f;
    CircleDraw(p, gStripGetFadeColor(0xFFFFFF), gStripGetFadeColor((trs << 24) | 0xFFFFFF), &off);
}

void PerfectGuardEffectDraw2(CHARACTER_WORK *offset)
{
    float x;
    float y;
    float ox;
    float oy;
    float R;
    int k;
    unsigned int colortb[4];
    u_int c0;
    u_int c1;
    int angle;

    colortb[0] = RGBConv2(0xFFFFFF);
    colortb[1] = RGBConv2(0xC0FFFFFF);
    colortb[2] = RGBConv2(0x60FFFFFF);
    colortb[3] = RGBConv2(0xFFFFFF);
    angle = (((int)(offset->attackst & 0xF) + 2) << 16) / 40;
    x = ObjWorld2ScreenX(offset->posx);
    y = ObjWorld2ScreenY(offset->posy);
    R = offset->HitBackx * GetObjScale();
    SetBlendingMode(0);
    if (offset->Localid > 0) {
        c0 = RGBConv2(((offset->Localid * 8 + 0x7F) << 24) | 0xFFFFFF);
        c1 = RGBConv2(((offset->Localid * 8 + 0x7F) << 24) | 0xFFFFFF);
        for (k = 0; k < 17; k++) {
            ox = R * G_Cos(k << 11);
            oy = R * G_Sin(k << 11);
            if (offset->dirflag == 0) {
                ox = -ox;
            }
            nlvt[k * 2].x = x + ox;
            nlvt[k * 2].y = y - oy;
            nlvt[k * 2].col = c0;
            nlvt[k * 2 + 1].x = x + ox;
            nlvt[k * 2 + 1].y = y + oy;
            nlvt[k * 2 + 1].col = c1;
        }
        nlvt->z = 10.0f;
        nlcDispStripTrnsl(34, nlvt);
    } else {
        for (k = 0; k < 4; k++) {
            ox = R * G_Cos(angle + k * 0x666);
            oy = R * G_Sin(angle + k * 0x666);
            if (offset->dirflag == 0) {
                ox = -ox;
            }
            nlvt[k * 2].col = colortb[k];
            nlvt[k * 2].x = x + ox;
            nlvt[k * 2].y = y - oy;
            nlvt[k * 2 + 1].x = x + ox;
            nlvt[k * 2 + 1].y = y + oy;
            nlvt[k * 2 + 1].col = colortb[k];
        }
        nlvt->z = 10.0f;
        nlcDispStripTrnsl(8, nlvt);
    }
}

void PerfectGuardEffectDraw(CHARACTER_WORK *offset)
{
    float R;
    int i;
    int j;
    int k;
    NL_POINT3 p;
    NL_POINT3 off;
    int c1;
    unsigned int col[16] = {
        0x0020FF00, 0x0000FF00, 0x0000FF20, 0x0000FF40, 0x0000FF60, 0x0000FF80, 0x0000FF60, 0x0000FF40,
        0x0000FF20, 0x0000FF00, 0x0020FF00, 0x0040FF00, 0x0060FF00, 0x0080FF00, 0x0060FF00, 0x0040FF00,
    };

    SetBlendingMode(1);
    ObjWorld2ScreenV(offset, &off);
    R = offset->HitBackx * off.z;
    for (i = 0; i < 12; i++) {
        nlUnitMatrix();
        nlRotateAX((offset->ActTimer + i) * 512);
        nlRotateAY((offset->ActTimer + i) * 0x500);
        if (offset->Localid > 0) {
            if (i == 11) {
                c1 = RGBConv2(0xE0FFFFFF);
            } else {
                c1 = RGBConv2(((i * 12) << 24) | 0xFFFFFF);
            }
        } else if (i == 11) {
            c1 = RGBConv2(0xE060FF60);
        } else {
            c1 = 0;
        }
        for (j = i & 1, k = 0; j < 66; j += 2, k++) {
            p.x = R * G_Cos(k << 11);
            p.y = R * G_Sin(k << 11);
            p.z = 0.0f;
            nlCalcPoint(&p, (NL_POINT3 *)&nlvt[j]);
            nlvt[j].x += off.x;
            nlvt[j].y += off.y;
            if (c1) {
                nlvt[j].col = c1;
            } else {
                nlvt[j].col = RGBConv2(((i * 6) << 24) | col[(i + k) & 0xF]);
            }
            nlvt[j].z = (offset->priority - 8.0f) + nlvt[j].z / 10.0f;
        }
        if (i) {
            nlcDispStripTrnsl(66, nlvt);
        }
    }
}

void PerfectGuardDisaappearEffectDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;

    ObjWorld2ScreenV(offset, &off);
    p[0].y = (float)(offset->ActTimer * (offset->HitBackx / 20) + offset->HitBackx) * off.z;
    if (offset->ActTimer < 4) {
        p[1].y = p[0].y * (offset->ActTimer * 15 + 35) / 100.0f;
    } else {
        p[1].y = p[0].y * 95.0f / 100.0f;
    }
    SetBlendingMode(0);
    p[0].x = p[1].x = 0.0f;
    p[0].z = p[1].z = 12.0f;
    CircleDraw(p, gStripGetFadeColor(((192 - offset->ActTimer * 12) << 24) | 0x60FF60), gStripGetFadeColor(0xFFFFFF), &off);
}

int g_roundmax(int val, int max);

void LightningCenterEffectDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;
    unsigned int alfa;

    alfa = g_roundmax((unsigned int)((16 - offset->ActTimer) * 400.0f * 0.0625f), 255);
    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    ClearPoint3(p, 2);
    FillPoint3Z(p, 2, offset->priority - 3.0f);
    p[0].y = 0.0f;
    p[1].y = off.z * 60.0f;
    CircleDrawHalf(p, gStripGetFadeColor((alfa << 24) | 0xFFFFFF), gStripGetFadeColor((alfa << 24) | 0xFFFFFF), &off);
    p[0].y = p[1].y;
    p[1].y = off.z * 120.0f;
    CircleDrawHalf(p, gStripGetFadeColor((alfa << 24) | 0xFFFFFF), gStripGetFadeColor(0xFF), &off);
}

void Poly_Effect_Initial(void)
{
    lwork = smalloc(0xE00, "lwork");
}

void nlRotateAZ(u_short r);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);

void HitEff172Draw(CHARACTER_WORK *offset)
{
    static NLVERTEX03 vt[10] = {
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.125f, 0.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.125f, 1.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 0.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.25f, 1.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.375f, 0.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.375f, 1.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0, 0 },
    };
    NLVERTEX03 *vp;
    NL_POINT3 p[10];
    NL_POINT3 *pp;
    NL_POINT3 off;
    float t1;
    float t2;
    int col;
    int i;

    vp = vt;
    CellMain(offset);
    ObjWorld2ScreenV(offset, &off);
    col = gStripGetFadeColor((offset->trans << 24) | 0xFFFFFF);
    t1 = off.z * 160.0f * offset->scaleY / 1000.0f;
    t2 = off.z * 32.0f * offset->scaleY / 1000.0f;
    p[0].x = t1 * 0.1f;
    p[0].y = t1;
    p[1].x = t2 * 0.1f;
    p[1].y = t2;
    p[2].x = t1 * 0.1f * 0.5f;
    p[2].y = t1;
    p[3].x = t2 * 0.1f * 0.5f;
    p[3].y = t2;
    p[4].x = 0.0f;
    p[4].y = t1;
    p[5].x = 0.0f;
    p[5].y = t2;
    p[6].x = -t1 * 0.1f * 0.5f;
    p[6].y = t1;
    p[7].x = -t2 * 0.1f * 0.5f;
    p[7].y = t2;
    p[8].x = -t1 * 0.1f;
    p[8].y = t1;
    p[9].x = -t2 * 0.1f;
    p[9].y = t2;
    nlUnitMatrix();
    nlRotateAZ(offset->angle);
    pp = p;
    for (i = 0; i < 10; i++, pp++) {
        pp->z = 0.0f;
        nlCalcPoint(pp, (NL_POINT3 *)vp);
        vp->x += off.x;
        vp->y += off.y;
        vp->z = offset->priority;
        vp->col = col;
        vp++;
    }
    SetBlendingMode(1);
    nlDispStripTexTrnsl(10, vt, offset->SpriteId);
}

void nlRotateAZ(u_short r);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);

void HitEff172Draw2(CHARACTER_WORK *offset)
{
    NLVERTEX03 vt[4];
    NLVERTEX03 *vp;
    NL_POINT3 p[4];
    NL_POINT3 *pp;
    NL_POINT3 off;
    float t1;
    float t2;
    int trs;
    int i;
    int t;

    CellMain(offset);
    ObjWorld2ScreenV(offset, &off);
    t = offset->ActTimer;
    trs = 255 - (t * 128) / 20;
    trs <<= 24;
    if (t <= 20) {
        float s = off.z * offset->scale / 1000.0f;

        t2 = t * 40 + 150;
        t1 = t * 2 + 20;
        t1 *= s;
        t2 *= s;
        p[2].y = p[0].y = off.z * 10.0f;
        p[3].y = p[1].y = off.z * -10.0f;
        p[1].x = p[0].x = t1;
        p[3].x = p[2].x = t2;
    } else {
        t1 = (t - 10) * 0.8f + 1.0f;
        t2 = t1 * t1;
        t2 = 1.0f / t2;
        t1 = t1 * ((t - 10) + 12.0f) * off.z;
        off.z *= offset->scale;
        p[3].x = p[2].x = off.z * 3.0f;
        p[0].y = p[2].y = t1;
        p[3].y = p[1].y = -t1;
        p[1].x = p[0].x = (2.99f - t2 * 2.7f) * off.z;
    }
    nlUnitMatrix();
    nlRotateAZ(offset->angle);
    vp = vt;
    pp = p;
    for (i = 0; i < 4; i++, pp++) {
        pp->z = 0.0f;
        nlCalcPoint(pp, (NL_POINT3 *)vp);
        vp->x += off.x;
        vp->y += off.y;
        vp++;
    }
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = offset->priority - 2.0f;
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = trs | 0xFFFFFF;
    vt[0].u = vt[2].u = 0.0f;
    vt[1].u = vt[3].u = 0.75f;
    vt[0].v = vt[1].v = 1.0f;
    vt[2].v = vt[3].v = 0.25f;
    SetBlendingMode(1);
    nlDispStripTexTrnsl(4, vt, offset->FBSpriteIdTop);
    SetBlendingMode(0);
}

void CircleDrawTex(NL_POINT3 *p, unsigned int c0, NL_POINT3 *offp, int index);

void HitEff173Draw(CHARACTER_WORK *offset)
{
    float sc;
    NL_POINT3 p[2];
    NL_POINT3 off;
    unsigned int alfa;
    static unsigned char alfatb[16] = { 255, 255, 255, 240, 225, 220, 170, 140, 100, 60, 30, 10, 0, 0, 0, 0 };

    sc = GetObjScale();
    alfa = alfatb[offset->ActTimer];
    off.x = ObjWorld2ScreenX(offset->posx);
    off.y = ObjWorld2ScreenY(offset->posy);
    p[0].x = p[1].x = 0.0f;
    p[0].z = p[1].z = offset->priority - 2.0f;
    SetBlendingMode(1);
    p[0].y = 0.0f;
    p[1].y = (offset->ActTimer * 2 + 30.0f) * sc;
    CircleDraw(p, gStripGetFadeColor((alfa << 24) | 0xFFFFFF), gStripGetFadeColor((alfa << 24) | 0xFFFFFF), &off);
    p[0].y = p[1].y;
    p[1].y += sc * 70.0f;
    CircleDrawTex(p, gStripGetFadeColor((alfa << 24) | 0xFFFFFF), &off, offset->FBSpriteIdTop);
    SetBlendingMode(0);
}

void CircleDrawTex(NL_POINT3 *p, unsigned int c0, NL_POINT3 *offp, int index);

void PXF_GetSize(int id, int *x, int *y);
void nlsDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);

void HitEff174Draw(CHARACTER_WORK *offset)
{
    NLVERTEX03 vt[4];
    NLVERTEX03 *vp;
    NL_POINT3 p[4];
    NL_POINT3 *pp;
    float x;
    float y;
    float sc1;
    float t1;
    float t2;
    int trs;
    int i;
    int sx;
    int sy;
    float sxf;
    float syf;

    PXF_GetSize(offset->FBSpriteIdTop, &sx, &sy);
    sxf = sx;
    syf = sy;
    sc1 = GetObjScale();
    trs = 255 - offset->ActTimer * 24;
    trs <<= 24;
    t2 = offset->ActTimer * 0.25f + 2.0f;
    t1 = t2 * 0.1f * (sc1 * offset->scale);
    t2 = t2 * (sc1 * offset->scale);
    p[2].y = p[0].y = sc1 * 3.0f;
    p[3].y = p[1].y = sc1 * -3.0f;
    p[1].x = p[0].x = t1;
    p[3].x = p[2].x = t2;
    nlUnitMatrix();
    nlRotateAZ((offset->Localid & 0xFF) << 8);
    x = ObjWorld2ScreenX(offset->posx);
    y = ObjWorld2ScreenY(offset->posy);
    vp = vt;
    pp = p;
    for (i = 0; i < 4; i++, pp++) {
        pp->z = 0.0f;
        nlCalcPoint(pp, (NL_POINT3 *)vp);
        vp->x += x;
        vp->y += y;
        vp++;
    }
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = offset->priority - 2.0f;
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = gStripGetFadeColor((trs << 24) | 0xFFFFFF);
    vt[0].u = vt[2].u = 0.0f;
    vt[1].u = vt[3].u = sxf * 0.75f;
    vt[0].v = vt[1].v = syf;
    vt[2].v = vt[3].v = syf * 0.25f;
    SetBlendingMode(1);
    nlsDispStripTexTrnsl(4, vt, offset->FBSpriteIdTop);
    SetBlendingMode(0);
}

void HitEff175Draw(CHARACTER_WORK *offset)
{
    float sc;
    NL_POINT3 p[2];
    NL_POINT3 off;
    unsigned int alfa;
    static unsigned char alfatb[16] = { 255, 255, 255, 240, 225, 220, 170, 140, 100, 60, 30, 10, 0, 0, 0, 0 };

    sc = GetObjScale();
    alfa = alfatb[offset->ActTimer];
    off.x = ObjWorld2ScreenX(offset->posx);
    off.y = ObjWorld2ScreenY(offset->posy);
    p[0].x = p[1].x = 0.0f;
    p[0].z = p[1].z = offset->priority - 2.0f;
    SetBlendingMode(1);
    p[0].y = 0.0f;
    p[1].y = (offset->ActTimer * 2.0f + 25.0f) * sc;
    CircleDraw(p, gStripGetFadeColor((alfa << 24) | 0xFFFFFF), gStripGetFadeColor((alfa << 24) | 0xFFFFFF), &off);
    p[0].y = p[1].y;
    p[1].y += sc * 40.0f;
    CircleDrawTex(p, gStripGetFadeColor((alfa << 24) | 0xFFFFFF), &off, offset->FBSpriteIdTop);
    SetBlendingMode(0);
}

void HitEffSlashSplashDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;

    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    ClearPoint3(p, 2);
    FillPoint3Z(p, 2, offset->priority + 1.0f);
    p[0].y = 0.0f;
    p[1].y = off.z * 2.0f;
    CircleDrawHalf(p, gStripGetFadeColor((offset->trans << 24) | 0xFF8080), gStripGetFadeColor((offset->trans << 24) | 0xFF0000), &off);
    p[0].y = p[1].y;
    p[1].y = off.z * 10.0f;
    CircleDrawHalf(p, gStripGetFadeColor(((offset->trans >> 1) << 24) | 0xFF0000), gStripGetFadeColor(0xFF0000), &off);
}

void HitEffHandFightSplashDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;

    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(0);
    ClearPoint3(p, 2);
    FillPoint3Z(p, 2, offset->priority + 1.0f);
    p[0].y = 0.0f;
    p[1].y = off.z;
    CircleDrawHalf(p, gStripGetFadeColor((offset->trans << 24) | 0xFFFFFF), gStripGetFadeColor((offset->trans << 24) | 0xFFFFFF), &off);
    p[0].y = p[1].y;
    p[1].y = off.z * 10.0f;
    CircleDrawHalf(p, gStripGetFadeColor(((offset->trans >> 1) << 24) | 0xFFFFFF), gStripGetFadeColor(0xFFFFFF), &off);
}

void HitEffKakuseiSplashDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;

    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    ClearPoint3(p, 2);
    FillPoint3Z(p, 2, offset->priority + 1.0f);
    p[0].y = 0.0f;
    p[1].y = off.z * 5.0f;
    CircleDrawHalf(p, gStripGetFadeColor((offset->trans << 24) | 0xC0C0FF), gStripGetFadeColor(((int)(offset->trans * 0.6f) << 24) | 0xC0C0FF), &off);
    p[0].y = p[1].y;
    p[1].y = off.z * 22.0f;
    CircleDrawHalf(p, gStripGetFadeColor(((int)(offset->trans * 0.5f) << 24) | 0xC0C0FF), gStripGetFadeColor(0xC0C0FF), &off);
}

void HitEffKakuseiRingDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[4];
    NL_POINT3 off;
    NLVERTEX03 vt[4];
    int oi;
    int r;
    int i;
    float s;
    float c;
    float z;
    float sv;

    oi = offset->ActTimer & 0x1F;
    {
        float sxf = 255.0f;

        CellMain(offset);
        ObjWorld2ScreenV(offset, &off);
        SetBlendingMode(1);
        nlUnitMatrix();
        nlRotateAX(offset->HitBackx);
        if (offset->dirflag == 1) {
            nlRotateAY(-offset->RHitBackx);
        } else {
            nlRotateAY(offset->RHitBackx);
        }
        p[0].z = 0.0f;
        p[2].z = 0.0f;
        p[1].z = 0.0f;
        p[3].z = 0.0f;
        vt[0].v = vt[2].v = p[0].z;
        vt[1].v = vt[3].v = sxf;
        vt[0].u = vt[1].u = (oi / 32.0f + p[0].z) * sxf;
        vt[2].u = vt[3].u = oi / 32.0f * sxf + sxf;
    }
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = gCurrentColorGet() | (offset->trans << 24);
    for (r = 0; r < 0x10000; r += 0x800) {
        s = G_Sin(r) * (offset->scale / 1000.0f) * off.z;
        c = G_Cos(r) * (offset->scale / 1000.0f) * off.z;
        p[0].x = s * 100.0f;
        p[1].x = s * 200.0f;
        p[0].y = c * 100.0f;
        p[1].y = c * 200.0f;
        s = G_Sin(r + 0x800) * (offset->scale / 1000.0f) * off.z;
        c = G_Cos(r + 0x800) * (offset->scale / 1000.0f) * off.z;
        p[2].x = s * 100.0f;
        p[3].x = s * 200.0f;
        p[2].y = c * 100.0f;
        p[3].y = c * 200.0f;
        for (i = 0; i < 4; i++) {
            NLVERTEX03 *v;

            nlCalcPoint(&p[i], (NL_POINT3 *)&vt[i]);
            v = &vt[i];
            sv = v->z;
            z = 400.0f / (sv + 500.0f);
            v->x *= z;
            v->y *= z;
            v->x += off.x;
            v->y += off.y;
            if (sv < 0.0f) {
                v->z = offset->priority - 8.0f;
            } else {
                v->z = offset->priority + 60.0f;
            }
        }
        nlsDispStripTexTrnsl(4, vt, offset->SpriteId);
    }
}

void HitEffRomanCancelDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[4];
    NL_POINT3 off;
    int r;
    int i;
    float s;
    float c;
    float z;
    float sv;

    CellMain(offset);
    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(0);
    nlUnitMatrix();
    nlRotateAX(offset->HitBackx);
    if (offset->dirflag == 1) {
        nlRotateAY(-offset->RHitBackx);
    } else {
        nlRotateAY(offset->RHitBackx);
    }
    p[0].z = 0.0f;
    p[2].z = 0.0f;
    p[1].z = 0.0f;
    p[3].z = 0.0f;
    nlvt[0].col = nlvt[1].col = nlvt[2].col = nlvt[3].col = RGBConv2(gStripGetFadeColor((offset->trans << 24) | 0xFF0000));
    for (r = 0; r < 0x10000; r += 0x800) {
        s = G_Sin(r) * (offset->scale / 1000.0f) * off.z;
        c = G_Cos(r) * (offset->scale / 1000.0f) * off.z;
        p[0].x = s * 130.0f;
        p[1].x = s * 150.0f;
        p[0].y = c * 130.0f;
        p[1].y = c * 150.0f;
        s = G_Sin(r + 0x800) * (offset->scale / 1000.0f) * off.z;
        c = G_Cos(r + 0x800) * (offset->scale / 1000.0f) * off.z;
        p[2].x = s * 130.0f;
        p[3].x = s * 150.0f;
        p[2].y = c * 130.0f;
        p[3].y = c * 150.0f;
        for (i = 0; i < 4; i++) {
            nlCalcPoint(&p[i], (NL_POINT3 *)&nlvt[i]);
            sv = nlvt[i].z;
            z = 400.0f / (sv + 500.0f);
            nlvt[i].x *= z;
            nlvt[i].y *= z;
            nlvt[i].x += off.x;
            nlvt[i].y += off.y;
            if (sv < 0.0f) {
                nlvt[i].z = offset->priority - 8.0f;
            } else {
                nlvt[i].z = offset->priority + 60.0f;
            }
        }
        nlcDispStripTrnsl(4, nlvt);
    }
}

void HitEffSDashSplashDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;

    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    ClearPoint3(p, 2);
    FillPoint3Z(p, 2, offset->priority + 1.0f);
    p[0].y = 0.0f;
    p[1].y = off.z * 1.5f;
    CircleDrawHalf(p, gStripGetFadeColor((offset->trans << 24) | 0xC0C0FF), gStripGetFadeColor(((int)(offset->trans * 0.5f) << 24) | 0x8080FF), &off);
    p[0].y = p[1].y;
    p[1].y = off.z * 10.0f;
    CircleDrawHalf(p, gStripGetFadeColor(((int)(offset->trans * 0.5f) << 24) | 0x4040FF), gStripGetFadeColor(0xFF), &off);
}

void KakuseiEffectDraw2(CHARACTER_WORK *offset)
{
    NL_POINT3 off;
    float x;
    float y;
    float t;
    float w;
    int i;
    u_int c0;
    u_int c1;

    w = 0.1f - offset->ActTimer * 0.1f / 20.0f;
    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    y = 80.0f;
    t = -8.0f;
    x = y;
    if (offset->dirflag) {
        x = -y;
    }
    x = x * off.z;
    y = off.z * y;
    for (i = 31; i >= 0; i--) {
        nlvt[i].z = offset->priority;
    }
    c0 = RGBConv2(gStripGetFadeColor(0xC06060F0));
    c1 = RGBConv2(0xFFFFFFFF);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].col = c0;
        nlvt[i * 2 + 1].col = c1;
    }
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) + w * y;
        nlvt[i * 2].y = off.y + y * (t + i) - w * x;
        nlvt[i * 2 + 1].x = off.x + x * (t + i);
        nlvt[i * 2 + 1].y = off.y + y * (t + i);
    }
    nlcDispStripTrnsl(32, nlvt);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) - w * y;
        nlvt[i * 2].y = off.y + y * (t + i) + w * x;
        nlvt[i * 2 + 1].x = off.x + x * (t + i);
        nlvt[i * 2 + 1].y = off.y + y * (t + i);
    }
    nlcDispStripTrnsl(32, nlvt);
    c0 = RGBConv2(gStripGetFadeColor(0x8080F0));
    c1 = RGBConv2(gStripGetFadeColor(0x808080F0));
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].col = c0;
        nlvt[i * 2 + 1].col = c1;
    }
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) + w * y * 4.0f;
        nlvt[i * 2].y = off.y + y * (t + i) - w * x * 4.0f;
        nlvt[i * 2 + 1].x = off.x + x * (t + i) + w * y;
        nlvt[i * 2 + 1].y = off.y + y * (t + i) - w * x;
    }
    nlcDispStripTrnsl(32, nlvt);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) - w * y * 4.0f;
        nlvt[i * 2].y = off.y + y * (t + i) + w * x * 4.0f;
        nlvt[i * 2 + 1].x = off.x + x * (t + i) - w * y;
        nlvt[i * 2 + 1].y = off.y + y * (t + i) + w * x;
    }
    nlcDispStripTrnsl(32, nlvt);
}

void KakuseiEffectDraw3(CHARACTER_WORK *offset)
{
    int i;
    int n;
    NL_POINT3 p[2];
    NL_POINT3 off;
    unsigned int alfa;

    alfa = 250;
    n = 0;
    i = offset->ActTimer;
    while (i--) {
        if (n >= 20) {
            if (n < 28) {
                alfa = alfa * 0.975f;
            } else {
                alfa = alfa * 0.8f;
            }
        }
        n++;
    }
    ObjWorld2ScreenV(offset, &off);
    p[0].x = p[1].x = 0.0f;
    p[0].z = p[1].z = offset->priority - 1.0f;
    SetBlendingMode(1);

    p[1].y = (offset->ActTimer * 3 + 30.0f) * off.z;
    p[0].y = 0.0f;
#ifdef GGX_PORT
    /* The EE computes the first colour before the second argument updates alfa. */
    {
        u_int c1 = gStripGetFadeColor(alfa << 24 | 0xFFFFFF);
        u_int c2 = gStripGetFadeColor(alfa = alfa << 24 | 0xC0C0FF);

        CircleDraw(p, c1, c2, &off);
    }
#else
    CircleDraw(p, gStripGetFadeColor(alfa << 24 | 0xFFFFFF), gStripGetFadeColor(alfa = alfa << 24 | 0xC0C0FF), &off);
#endif
    p[0].y = p[1].y;
    p[1].y *= 1.7f;
    CircleDraw(p, gStripGetFadeColor(alfa), gStripGetFadeColor(0x8080FF), &off);
}

void KakuseiEffectDraw4(CHARACTER_WORK *offset)
{
    NL_POINT3 p[2];
    NL_POINT3 off;
    unsigned int alfa;

    alfa = 120 - offset->ActTimer * 110 / 40;
    ObjWorld2ScreenV(offset, &off);
    p[0].x = p[1].x = 0.0f;
    p[0].z = p[1].z = offset->priority - 2.0f;
    SetBlendingMode(1);

    p[0].y = (offset->ActTimer * 5.0f + 130.0f) * off.z;
    p[1].y = p[0].y * 0.85f;
    CircleDraw(p, gStripGetFadeColor(alfa << 24 | 0xCFCFFF), gStripGetFadeColor(0x8080FF), &off);
    p[1].y = p[0].y * 1.2f;
    CircleDraw(p, gStripGetFadeColor(alfa << 24 | 0xCFCFFF), gStripGetFadeColor(0x8080FF), &off);
}

void KakuseiEffectDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 off;
    float x;
    float y;
    float t;
    u_int c0;
    u_int c1;
    static float tb[16] = { 0.0f, 0.2f, 0.4f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
    int i;

    t = 10 - offset->ActTimer;
    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    x = y = 80.0f;
    if (offset->dirflag) {
        x = -x;
    }
    x = x * off.z;
    y = off.z * y;
    for (i = 0; i < 32; i++) {
        nlvt[i].z = offset->priority;
    }
    c0 = RGBConv2(gStripGetFadeColor(0xFF4040F0));
    c1 = RGBConv2(0xFFFFFFFF);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].col = c0;
        nlvt[i * 2 + 1].col = c1;
    }
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) + tb[i] * y;
        nlvt[i * 2].y = off.y + y * (t + i) - tb[i] * x;
        nlvt[i * 2 + 1].x = off.x + x * (t + i);
        nlvt[i * 2 + 1].y = off.y + y * (t + i);
    }
    nlcDispStrip(32, nlvt);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) - tb[i] * y;
        nlvt[i * 2].y = off.y + y * (t + i) + tb[i] * x;
        nlvt[i * 2 + 1].x = off.x + x * (t + i);
        nlvt[i * 2 + 1].y = off.y + y * (t + i);
    }
    nlcDispStrip(32, nlvt);
    for (i = 31; i >= 0; i--) {
        nlvt[i].z = offset->priority + 1.0f;
    }
    c0 = RGBConv2(gStripGetFadeColor(0xFF8080F0));
    c1 = RGBConv2(gStripGetFadeColor(0x808080F0));
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].col = c0;
        nlvt[i * 2 + 1].col = c1;
    }
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) + tb[i] * y * 2.0f;
        nlvt[i * 2].y = off.y + y * (t + i) - tb[i] * x * 2.0f;
        nlvt[i * 2 + 1].x = off.x + x * (t + i) + tb[i] * y;
        nlvt[i * 2 + 1].y = off.y + y * (t + i) - tb[i] * x;
    }
    nlcDispStripTrnsl(32, nlvt);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) - tb[i] * y * 2.0f;
        nlvt[i * 2].y = off.y + y * (t + i) + tb[i] * x * 2.0f;
        nlvt[i * 2 + 1].x = off.x + x * (t + i) - tb[i] * y;
        nlvt[i * 2 + 1].y = off.y + y * (t + i) + tb[i] * x;
    }
    nlcDispStripTrnsl(32, nlvt);
    x = y = -80.0f;
    if (offset->dirflag) {
        x = -x;
    }
    x = x * off.z;
    y = off.z * y;
    for (i = 0; i < 32; i++) {
        nlvt[i].z = offset->priority;
    }
    c0 = RGBConv2(gStripGetFadeColor(0xFF6060F0));
    c1 = RGBConv2(0xFFFFFFFF);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].col = c0;
        nlvt[i * 2 + 1].col = c1;
    }
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) + tb[i] * y;
        nlvt[i * 2].y = off.y + y * (t + i) - tb[i] * x;
        nlvt[i * 2 + 1].x = off.x + x * (t + i);
        nlvt[i * 2 + 1].y = off.y + y * (t + i);
    }
    nlcDispStrip(32, nlvt);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) - tb[i] * y;
        nlvt[i * 2].y = off.y + y * (t + i) + tb[i] * x;
        nlvt[i * 2 + 1].x = off.x + x * (t + i);
        nlvt[i * 2 + 1].y = off.y + y * (t + i);
    }
    nlcDispStrip(32, nlvt);
    c0 = RGBConv2(gStripGetFadeColor(0x8080F0));
    c1 = RGBConv2(gStripGetFadeColor(0x808080F0));
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].col = c0;
        nlvt[i * 2 + 1].col = c1;
    }
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) + tb[i] * y * 2.0f;
        nlvt[i * 2].y = off.y + y * (t + i) - tb[i] * x * 2.0f;
        nlvt[i * 2 + 1].x = off.x + x * (t + i) + tb[i] * y;
        nlvt[i * 2 + 1].y = off.y + y * (t + i) - tb[i] * x;
    }
    nlcDispStripTrnsl(32, nlvt);
    for (i = 0; i < 16; i++) {
        nlvt[i * 2].x = off.x + x * (t + i) - tb[i] * y * 2.0f;
        nlvt[i * 2].y = off.y + y * (t + i) + tb[i] * x * 2.0f;
        nlvt[i * 2 + 1].x = off.x + x * (t + i) - tb[i] * y;
        nlvt[i * 2 + 1].y = off.y + y * (t + i) + tb[i] * x;
    }
    nlcDispStripTrnsl(32, nlvt);
}

void HitEffItigekiReadyDraw(CHARACTER_WORK *offset)
{
    NL_POINT3 p[6];
    NL_POINT3 off;
    int j;
    float s;
    int i;

    CellMain(offset);
    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(1);
    nlUnitMatrix();
    nlRotateAX(0x4200);
    nlRotateAZ(offset->angle);
    s = off.z;
    p[0].x = s * 6.0f;
    p[1].y = p[0].y = s * 150.0f;
    p[1].x = s * -6.0f;
    p[2].z = 0.0f;
    p[3].z = 0.0f;
    p[0].z = 0.0f;
    p[1].z = 0.0f;
    nlvt[0].col = nlvt[4].col = nlvt[1].col = nlvt[5].col = RGBConv2(gStripGetFadeColor(0xFF0000));
    nlvt[2].col = nlvt[3].col = RGBConv2(gStripGetFadeColor((offset->trans << 24) | 0xFF0000));
    for (i = 0; i < 2; i++) {
        float z;
        float sv;

        nlCalcPoint(&p[i], (NL_POINT3 *)&nlvt[i]);
        sv = nlvt[i].z;
        z = 1024.0f / (sv + 1024.0f);
        nlvt[i].x *= z;
        nlvt[i].y *= z;
        nlvt[i].x += off.x;
        nlvt[i].y += off.y;
        if (sv < 0.0f) {
            nlvt[i].z = offset->priority - 8.0f;
        } else {
            nlvt[i].z = offset->priority + 60.0f;
        }
        nlvt[i + 4].y = nlvt[i].y - offset->scale * z;
        nlvt[i + 2].y = nlvt[i].y - offset->scale * (offset->scaleY / 1000.0f) * z;
        nlvt[i + 2].x = nlvt[i + 4].x = nlvt[i].x;
        nlvt[i + 2].z = nlvt[i + 4].z = nlvt[i].z;
    }
    nlcDispStripTrnsl(6, nlvt);
    for (j = 0; j < 2; j++) {
        if (j == 0) {
            p[0].x = s * 6.0f;
            p[0].z = 0.0f;
            p[1].y = p[0].y = s * 150.0f;
            p[1].x = s * 12.0f;
            p[1].z = 0.0f;
        } else {
            p[0].x = s * -6.0f;
            p[0].z = 0.0f;
            p[1].y = p[0].y = s * 150.0f;
            p[1].x = s * -12.0f;
            p[1].z = 0.0f;
        }
        nlvt[0].col = nlvt[1].col = nlvt[3].col = RGBConv2(gStripGetFadeColor(0xFF0000));
        nlvt[2].col = RGBConv2(gStripGetFadeColor((offset->trans << 24) | 0xFF0000));
        for (i = 0; i < 2; i++) {
            float z;
            float sv;

            nlCalcPoint(&p[i], (NL_POINT3 *)&nlvt[i]);
            sv = nlvt[i].z;
            z = 1024.0f / (sv + 1024.0f);
            nlvt[i].x *= z;
            nlvt[i].y *= z;
            nlvt[i].x += off.x;
            nlvt[i].y += off.y;
            if (sv < 0.0f) {
                nlvt[i].z = offset->priority - 8.0f;
            } else {
                nlvt[i].z = offset->priority + 60.0f;
            }
            if (i != 0) {
                nlvt[i].y -= offset->scale * (offset->scaleY / 1000.0f) * z;
            } else {
                nlvt[i + 3].y = nlvt[i].y - offset->scale * z;
                nlvt[i + 2].y = nlvt[i].y - offset->scale * (offset->scaleY / 1000.0f) * z;
                nlvt[i + 2].x = nlvt[i + 3].x = nlvt[i].x;
                nlvt[i + 2].z = nlvt[i + 3].z = nlvt[i].z;
            }
        }
        nlcDispStripTrnsl(4, nlvt);
    }
}

extern GAME_INFOMATION g_info;
void set_random_seed_abs(void);
int Random(void);
int rand(void);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);

void LightningEffectDraw(CHARACTER_WORK *offset)
{
    float x;
    float y;
    float sin;
    float cos;
    int i;
    int j;
    int angle;
    int n;
    unsigned char pt;
    int timer; /* not in the stabs: holds ActTimer in s7 across the Mark block */
    unsigned int alfa[5];
    static unsigned int colorTb[5] = { 0x20, 0x3030FF, 0xFFFFFF, 0x3030FF, 0x20 };

    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }

    timer = offset->ActTimer;
    alfa[0] = alfa[4] = 0;
    alfa[1] = alfa[3] = (16 - timer) * 120.0f * 0.0625f;
    alfa[2] = (16 - timer) * 255.0f * 0.0625f;

    if (offset->Mark == 0) {
        offset->Mark = 1;
        lightningtimer = 0;
        for (i = 0; i < 128; i++) lwork[i].parent = 255;
        x = ObjWorld2ScreenX(offset->posx);
        y = ObjWorld2ScreenY(offset->posy);
        angle = (Random() & 0xFF) << 8;
        for (i = 1; i < 10; i++) {
            lwork[i].parent = 0x8000;
            lwork[i].x[2] = lwork[i].x[0] = lwork[i].x[1] = lwork[i].x[3] = lwork[i].x[4] = x;
            lwork[i].y[2] = lwork[i].y[0] = lwork[i].y[1] = lwork[i].y[3] = lwork[i].y[4] = y;
            angle = lwork[i].angle = (angle + (long)((Random() & 0xFF) + 0x3000)) << 5; /* (long) is 64-bit on EE: blocks fold reassociation */
        }

        for (i = 1; i < 128; i++) {
            if (lwork[i].parent & 0x8000) {
                lwork[i].parent &= 0x7FFF;
                n = (rand() & 0xF) ? 1 : 2;
                while (n--) {
                    for (j = i; j < 128; j++) {
                        if (lwork[j].parent == 255) {
                            lwork[j].parent = i | 0x8000;
                            angle = ((Random() & 0xFF) << 5) - 0x1000;
                            lwork[j].r = (Random() & 0x3F) + 32;
                            angle = lwork[j].angle = angle + lwork[i].angle;
                            sin = G_Sin(angle);
                            cos = G_Cos(angle);
                            lwork[j].y[2] = sin * lwork[j].r + lwork[i].y[2];
                            lwork[j].x[2] = cos * lwork[j].r + lwork[i].x[2];
                            break;
                        }
                    }
                    if (i == 128) goto end;
                }
            }
        }
    }

end:
    if (lightningtimer != timer) {
        lightningtimer = timer;
        for (i = 1; i < 128; i++) {
            if ((pt = lwork[i].parent & 0x7F)) {
                angle = lwork[i].angle += (Random() & 0xFF) * 16 - 0x800;
                j = Random() & 3;
                if (j) lwork[i].r += j - 2;
                sin = G_Sin(angle);
                cos = G_Cos(angle);
                lwork[i].y[2] = sin * lwork[i].r + lwork[pt].y[2];
                lwork[i].x[2] = cos * lwork[i].r + lwork[pt].x[2];
                lwork[i].y[1] = cos * 2.5f + lwork[i].y[2];
                lwork[i].x[1] = -sin * 2.5f + lwork[i].x[2];
                lwork[i].y[3] = -cos * 2.5f + lwork[i].y[2];
                lwork[i].x[3] = sin * 2.5f + lwork[i].x[2];
                lwork[i].y[0] = cos * 10.0f + lwork[i].y[2];
                lwork[i].x[0] = -sin * 10.0f + lwork[i].x[2];
                lwork[i].y[4] = -cos * 10.0f + lwork[i].y[2];
                lwork[i].x[4] = sin * 10.0f + lwork[i].x[2];
            }
        }
    }

    SetBlendingMode(1);
    for (i = 1; i < 128; i++) {
        if ((pt = lwork[i].parent & 0x7F)) {
            for (j = 0; j < 5; j++) {
                nlvt[j * 2].x = lwork[pt].x[j]; nlvt[j * 2].y = lwork[pt].y[j]; nlvt[j * 2].z = 10.0f;
                nlvt[j * 2].col = (alfa[j] << 24) | colorTb[j];

                nlvt[j * 2 + 1].x = lwork[i].x[j]; nlvt[j * 2 + 1].y = lwork[i].y[j]; nlvt[j * 2 + 1].z = 10.0f;
                nlvt[j * 2 + 1].col = (alfa[j] << 24) | colorTb[j];
            }
            nlDispStripTrnsl(10, nlvt);
        }
    }
}
