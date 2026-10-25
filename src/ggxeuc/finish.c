#include "generated/finish.h"
#include "generated/finish.h"

void SetBlendingMode(short int mode);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);
extern u_short FrameBufferIdByIdno[144];

void DrawParticle(int x, int y, float scale, float alfa)
{
    static NLVERTEX03 vt[4];
    float fx, fy;
    u_int col;

    fx = x;
    fy = y;
    alfa *= 255.0f;
    scale *= 32.0f;
    col = ((int)alfa << 24) | 0x8080FF;
    vt[0].x = fx - scale;
    vt[0].y = fy - scale;
    vt[1].x = fx - scale;
    vt[1].y = fy + scale;
    vt[2].x = fx + scale;
    vt[2].y = fy - scale;
    vt[3].x = fx + scale;
    vt[3].y = fy + scale;
    vt[0].col = col;
    vt[1].col = col;
    vt[2].col = col;
    vt[3].col = col;
    SetBlendingMode(1);
    nlDispStripTexTrnsl(4, vt, FrameBufferIdByIdno[111] + 4);
}

int Random(void);

void KakuseiFinishParticlePointClear(KAKUSEIPARTICLEWORK *p)
{
    p->x = ((Random() & 0xFF) - 0x80) * 5;
    p->y = ((Random() & 0xFF) - 0x80) * 5;
    if (p->x < 0) {
        p->x -= 100;
    } else {
        p->x += 100;
    }
    if (p->y < 0) {
        p->y -= 100;
    } else {
        p->y += 100;
    }
    p->z = (Random() & 0x7F) * 100 + 10000;
}

void KakuseiFinishParticlePointClear(KAKUSEIPARTICLEWORK *p);

void KakuseiFinishParticleCalc(KAKUSEIPARTICLEWORK *p)
{
    p->z -= 400;
    if (p->z < -2000) {
        KakuseiFinishParticlePointClear(p);
    }
}

int Random(void);

void KakuseiFinishPointClear(KAKUSEILINEWORK *p, int n)
{
    p->z = (Random() & 0xFF) * 50 + 16000;
    p->time = n;
    p->index = 0;
    p->angle = Random() << 11;
    p->color = Random() % 3;
    p->r = (Random() & 0x3FF) + 1000;
    p->flag = Random() & 3;
}

float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);

void KakuseiFinishCalc(KAKUSEILINEWORK *p)
{
    unsigned int index;
    float c;

    p->z -= 800;
    if (p->z < -4000) {
        KakuseiFinishPointClear(p, 0);
    }
    p->index--;
    if (++p->time == 0) {
        KakuseiFinishPointClear(p, 1);
    }
    {
        short int AddAngleTb[4] = { 0x200, 0x100, -0x200, -0x100 };
        KAKUSEIPOINT *point;
        float z;

        index = p->index & 0x1F;
        p->index = index;
        point = &p->point[index];
        p->angle += AddAngleTb[p->flag & 3];
        z = p->z;
        if (z < 0.0f) {
            z = 0.0f;
        }
        c = 512.0 / (z + 512.0);
        point->x = G_Sin(p->angle) * p->r * c + 320.0;
        point->y = G_Cos(p->angle) * p->r * c + 240.0;
        point->offx = G_Cos(p->angle) * 150.0f * c;
        point->offy = -G_Sin(p->angle) * 150.0f * c;
    }
}

int sound_request(int no);

void KakuseiFinishFirstFrame(void)
{
    int i;

    kakuseifinish.time = 0;
    sound_request(0x33);
    sound_request(0x33);
    sound_request(0x33);
    sound_request(0x33);
    sound_request(0x33);
    for (i = 0; i < 24; i++) {
        KakuseiFinishPointClear(&kakuseiwork[i], -i);
        KakuseiFinishCalc(&kakuseiwork[i]);
    }
    for (i = 0; i < 64; i++) {
        KakuseiFinishParticlePointClear(&particlework[i]);
        KakuseiFinishParticleCalc(&particlework[i]);
    }
}

void KakuseiFinishCalcMain(void)
{
    int i;

    for (i = 0; i < 24; i++) {
        KakuseiFinishCalc(&kakuseiwork[i]);
    }
    for (i = 0; i < 64; i++) {
        KakuseiFinishParticleCalc(&particlework[i]);
    }
}

void KakuseiFinishParticleDrawSub(KAKUSEIPARTICLEWORK *p)
{
    float z;
    float x;
    float y;
    float c;
    int i;

    z = p->z;
    for (i = 0; i < 5; i++) {
        if (!(z > 10000.0f) && !(z < 0.0f)) {
            c = 512.0 / (z + 512.0);
            x = p->x * c + 320.0f;
            y = p->y * c + 240.0f;
            DrawParticle(x, y, c * ((i + 3) * (i + 3)) / 9.0f, 1.0 / (i + 1.0));
        }
        z += 150.0f;
    }
}

void nlDispStripTrnsl(int num, NLVERTEX00 *vt);

void KakuseiFinishDrawSub(KAKUSEILINEWORK *p)
{
    int count;
    int i;
    NLVERTEX00 vt[6];
    unsigned int alfa;
    unsigned int c0;

    c0 = 0;
    count = p->time;
    if (count > 32) {
        count = 32;
    }
    if (count > 12) {
        count = 12;
    }
    count--;
    for (i = 0; i < count; i++) {
        {
            unsigned int index = (p->index + i) & 0x1F;
            KAKUSEIPOINT *point = &p->point[index];

            vt[0].x = point->x + point->offx;
            vt[2].x = point->x;
            vt[4].x = point->x - point->offx;
            vt[0].y = point->y + point->offy;
            vt[2].y = point->y;
            vt[4].y = point->y - point->offy;
        }
        {
            unsigned int index = (p->index + i + 1) & 0x1F;
            KAKUSEIPOINT *point = &p->point[index];

            vt[1].x = point->x + point->offx;
            vt[3].x = point->x;
            vt[5].x = point->x - point->offx;
            vt[1].y = point->y + point->offy;
            vt[3].y = point->y;
            vt[5].y = point->y - point->offy;
        }
        vt[0].z = 600.0f;
        vt[1].z = 600.0f;
        vt[2].z = 600.0f;
        vt[3].z = 600.0f;
        vt[4].z = 600.0f;
        vt[5].z = 600.0f;
        if (p->color == 0) {
            c0 = 0xFF;
        } else if (p->color == 1) {
            c0 = 0xFF0000;
        } else if (p->color == 2) {
            c0 = 0xFF00;
        }
        alfa = 0;
        if (i) {
            alfa = 220 - i * 20;
        }
        vt[0].col = c0;
        vt[2].col = (alfa << 24) | 0xFFFFFF;
        vt[4].col = c0;
        alfa = 220 - (i + 1) * 20;
        vt[1].col = c0;
        vt[3].col = (alfa << 24) | 0xFFFFFF;
        vt[5].col = c0;
        nlDispStripTrnsl(6, vt);
    }
}

void SetBlendingMode(short int mode);

void KakuseiFinishDrawMain(void)
{
    int i;

    SetBlendingMode(1);
    for (i = 0; i < 24; i++) {
        KakuseiFinishDrawSub(&kakuseiwork[i]);
    }
    for (i = 0; i < 64; i++) {
        KakuseiFinishParticleDrawSub(&particlework[i]);
    }
}

void nlDispStripTrnsl(int num, NLVERTEX00 *vt);

void KakuseiFinishTriangleDraw(int x1, int y1, int x2, int y2, unsigned int color)
{
    NLVERTEX00 vt[3];

    vt[0].z = 620.0f;
    vt[1].z = 620.0f;
    vt[2].z = 620.0f;
    vt[0].x = 320.0f;
    vt[1].x = x1 + 320.0;
    vt[2].x = x2 + 320.0;
    vt[0].y = 240.0f;
    vt[1].y = y1 + 240.0;
    vt[2].y = y2 + 240.0;
    vt[0].col = color;
    vt[1].col = color;
    vt[2].col = color;
    nlDispStripTrnsl(3, vt);
}

void ClearPoint3(NL_POINT3 *p, unsigned int cnt);
void FillPoint3Z(NL_POINT3 *p, unsigned int cnt, float z);
void CircleDraw(NL_POINT3 *p, unsigned int c0, unsigned int c1, NL_POINT3 *offp);
void nlSetBackColor(int c1, int c2, int c3);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
extern GAME_INFOMATION g_info;
extern int KakuseiFinish_wave[]; /* STATIC_EXTERN 0x5dfe00: wave[4] */

int KakuseiFinish(void)
{
    KAKUSEIFINISH *p;
    int i;
    NL_POINT3 pp[2];
    NL_POINT3 off;

    p = &kakuseifinish;
    if (g_info.BattleControlFlag & 0x40) {
        g_info.BattleControlFlag = (g_info.BattleControlFlag | 0x80) & ~0x40;
        KakuseiFinishFirstFrame();
        for (i = 3; i >= 0; i--) {
            KakuseiFinish_wave[i] = 0;
        }
    }
    p->time++;
    KakuseiFinishCalcMain();
    if (p->time == 2) {
        KakuseiFinish_wave[0] = 1;
    }
    if (p->time == 6) {
        KakuseiFinish_wave[1] = 1;
    }
    if (p->time == 12) {
        KakuseiFinish_wave[2] = 1;
    }
    if (p->time == 20) {
        KakuseiFinish_wave[3] = 1;
    }
    if (p->time == 30) {
        KakuseiFinish_wave[0] = 1;
    }
    if (p->time == 44) {
        KakuseiFinish_wave[1] = 1;
    }
    if (p->time == 60) {
        KakuseiFinish_wave[2] = 1;
    }
    if (p->time == 80) {
        KakuseiFinish_wave[3] = 1;
    }
    if (p->time < 120) {
        unsigned int color;
        float f;
        float r;

        f = p->time & 0x1F;
        if (p->time & 0x20) {
            f = 32.0f - f;
        }
        f *= 0.03125f;
        color = 0xFF000000 | ((int)(f * 150.0f + 100.0f) << 16) | ((int)(f * 160.0f + 0.0f) << 8);
        nlSetBackColor(color, color, color);
        for (i = 0; i < 4; i++) {
            if (KakuseiFinish_wave[i]) {
                ClearPoint3(pp, 2);
                FillPoint3Z(pp, 2, 610.0f);
                r = (20.0 - KakuseiFinish_wave[i]) * 0.5;
                pp[0].y = 400.0 / r;
                r = (20.0 - (KakuseiFinish_wave[i] - 4)) * 0.5;
                pp[1].y = 400.0 / r;
                off.x = 320.0f;
                off.y = 240.0f;
                CircleDraw(pp, 0xE0B0B0FF, 0xC0C0FF, &off);
                if (KakuseiFinish_wave[i]++ >= 18) {
                    KakuseiFinish_wave[i] = 0;
                }
            }
        }
    }
    if (p->time < 80) {
    } else if (p->time < 120) {
        int color;
        float r;
        float t;

        color = (p->time - 110) / 10.0 * 255.0;
        t = (35.0 - (p->time - 80)) * 0.5;
        if (t < 1.0) {
            t = 1.0f;
        }
        r = 400.0 / t;
        off.x = 320.0f;
        off.y = 240.0f;
        ClearPoint3(pp, 2);
        FillPoint3Z(pp, 2, 610.0f);
        pp[0].y = 0;
        pp[1].y = r;
        CircleDraw(pp, 0xFFFFFFFF, 0xFFFFFFFF, &off);
        if (t >= 1.1) {
            pp[0].y = pp[1].y;
            pp[1].y = r * 1.4;
            CircleDraw(pp, 0xFFFFFFFF, 0xC0C0FF, &off);
        }
        if (color > 0) {
            polydraw_FBox(0, 0, 640.0f, 480.0f, 2.0f, gStripGetFadeColor((color << 24) | 0xFFFFFF));
        }
    } else if (p->time < 150) {
        nlSetBackColor(0, 0, 0);
        polydraw_FBox(0, 0, 640.0f, 480.0f, 2.0f, 0xFFFFFFFF);
        return 0;
    } else if (p->time < 160) {
        unsigned int color;

        color = ((unsigned int)((160 - p->time) / 10.0 * 255.0) << 24) | 0xFFFFFF;
        polydraw_FBox(0, 0, 640.0f, 480.0f, 2.0f, color);
        return 0;
    } else {
        g_info.BattleControlFlag &= ~0xC0;
        return 0;
    }
    KakuseiFinishDrawMain();
    return 1;
}


float KakuseiBG_GetFadeVal(void)
{
    return KakuseiControl.fade;
}

int Random(void);

void KakuseiBG_Clear(void)
{
    int i;

    for (i = 0; i < 240; i++) {
        KakuseiControl.table[i * 2] = 0;
        KakuseiControl.table[i * 2 + 1] = (Random() & 0x3F) + 32;
        KakuseiControl.table[i * 2 + 1] = KakuseiControl.table[i * 2 + 1] * (((i - 120 >= 0) ? i - 120 : 120 - i) / 120.0 + 0.3);
    }
    for (i = 0; i < 16; i++) {
        KakuseiControl.nearline[i].speed = (Random() & 0x3F) + 48;
        KakuseiControl.nearline[i].x = 0;
        KakuseiControl.nearline[i].y = Random() & 0xFF;
    }
}

void dfree(void *mem);

extern GAME_INFOMATION g_info;

void KakuseiBG_Execute(void)
{
    int i;
    GAME_INFOMATION *gp;

    gp = &g_info;
    if (!(0 < KakuseiControl.fade) && !(gp->BattleControlFlag & 0x20)) {
        return;
    }
    if (KakuseiControl.fade == 0) {
        KakuseiBG_Clear();
    }
    if ((gp->BattleControlFlag & 0xE0) != 0x20) {
        KakuseiControl.fade -= 0.1f;
        if (KakuseiControl.fade <= 0) {
            KakuseiControl.fade = 0;
            return;
        }
    } else {
        gp->BattleControlFlag &= ~0x20;
        KakuseiControl.fade += 0.1f;
        if (1.0f < KakuseiControl.fade) {
            KakuseiControl.fade = 1.0f;
        }
    }
    for (i = 0; i < 240; i++) {
        KakuseiControl.table[i * 2] += KakuseiControl.table[i * 2 + 1];
        if (KakuseiControl.table[i * 2] > 0x500) {
            KakuseiControl.table[i * 2] -= 0x280;
        }
    }
    for (i = 0; i < 16; i++) {
        KakuseiControl.nearline[i].x += KakuseiControl.nearline[i].speed;
        if (KakuseiControl.nearline[i].x > 0x500) {
            KakuseiControl.nearline[i].x = 0;
        }
    }
}


void KakuseiFinishEnd(void)
{
    int i;

    if (kakuseiworkalloc) {
        for (i = 0; i < 24; i++) {
            dfree(kakuseiwork[i].point);
            kakuseiwork[i].point = 0;
        }
        kakuseiworkalloc = 0;
    }
    if (KakuseiControl.table) {
        dfree(KakuseiControl.table);
        KakuseiControl.table = 0;
    }
    if (KakuseiControl.nearline) {
        dfree(KakuseiControl.nearline);
        KakuseiControl.nearline = 0;
    }
}

void *smalloc(int size, char *name);
extern GAME_INFOMATION g_info;
/* .rodata strings that follow KakuseiBG_Draw's literals, which are not in this file yet */
extern char D_5C5EF8[]; /* RODATA_EXTERN 0x5c5ef8: "kakuseiwork" */
extern char D_5C5F08[]; /* RODATA_EXTERN 0x5c5f08: "nearline" */

void KakuseiFinishInit(void)
{
    int i;

    for (i = 0; i < 24; i++) {
        kakuseiwork[i].point = smalloc(0x100, D_5C5EF8);
    }
    KakuseiControl.fade = 0;
    kakuseiworkalloc = 1;
    KakuseiControl.table = smalloc(0x424, "table");
    KakuseiControl.nearline = smalloc(0xE4, D_5C5F08);
    g_info.BattleControlFlag &= ~0xF1;
}

void gCurrentColorClear(void);
void gCurrentColorSet(float val);
void nlDispStrip(int num, NLVERTEX00 *vt);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);
void nlSetBackColor(int c1, int c2, int c3);
unsigned int gStripGetFadeColor(unsigned int color);

void KakuseiBG_Draw(void)
{
    int i;
    NLVERTEX00 vt[4];
    NLVERTEX00 *vp;
    unsigned int colortable[13] = {
        0xFF000040, 0xFF000060, 0xFF000080, 0xFF1800A0, 0xFF3000C0, 0xFF4800E0, 0xFF6000FF,
        0xFF7810FF, 0xFF9020FF, 0xFFA830FF, 0xFFC040FF, 0xFFD850FF, 0xFFF060FF,
    };

    if (KakuseiControl.fade <= 0.1f) {
        return;
    }
    gCurrentColorSet(KakuseiControl.fade);
    nlSetBackColor(0, 0, 0);
    vt[0].z = 710.0f;
    vp = &vt[1];
    vp->z = 710.0f;
    vp = &vt[2];
    vp->z = 710.0f;
    vp = &vt[3];
    vp->z = 710.0f;
    for (i = 0; i < 240; i++) {
        int index;
        int x;

        index = KakuseiControl.table[i * 2 + 1] >> 3;
        vt[0].y = i * 2;
        vt[0].col = gStripGetFadeColor(colortable[index]);
        vp = &vt[1];
        vp->y = i * 2;
        vp->col = 0xFF000000;
        vp = &vt[2];
        vp->y = i * 2 + 1;
        vp->col = gStripGetFadeColor(colortable[index]);
        vp = &vt[3];
        vp->y = i * 2 + 1;
        vp->col = 0xFF000000;
        for (x = KakuseiControl.table[i * 2]; x > 0; x -= KakuseiControl.table[i * 2 + 1] * 14) {
            float w;

            w = KakuseiControl.table[i * 2 + 1] * 7;
            vt[0].x = x;
            vt[1].x = x - w;
            vt[2].x = x;
            vt[3].x = x - w;
            nlDispStrip(3, vt);
        }
    }
    vt[0].z = 701.0f;
    vt[0].col = gStripGetFadeColor(0xC0FFFFFF);
    vp = &vt[1];
    vp->z = 701.0f;
    vp->col = gStripGetFadeColor(0x80FFFFFF);
    vp = &vt[2];
    vp->z = 701.0f;
    vp->col = gStripGetFadeColor(0xC0FFFFFF);
    vp = &vt[3];
    vp->z = 701.0f;
    vp->col = gStripGetFadeColor(0x80FFFFFF);
    for (i = 0; i < 16; i++) {
        float w;
        float x;

        x = KakuseiControl.nearline[i].x;
        w = KakuseiControl.nearline[i].speed * 4;
        vt[0].y = KakuseiControl.nearline[i].y * 2;
        vt[0].x = x;
        vp = &vt[1];
        vp->y = KakuseiControl.nearline[i].y * 2;
        vp->x = x - w;
        vp = &vt[2];
        vp->y = KakuseiControl.nearline[i].y * 2 + 2;
        vp->x = x;
        vp = &vt[3];
        vp->y = KakuseiControl.nearline[i].y * 2 + 2;
        vp->x = x - w;
        nlDispStripTrnsl(4, vt);
    }
    gCurrentColorClear();
}
