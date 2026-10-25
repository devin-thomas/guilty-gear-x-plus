#include "generated/chase.h"

static const short idtb[] = { 1, 2, 3 };

static int FadeOutVal;
static int ChaseTime;
static float transval;
static int ChaseFlag;
static int trans_bg_id;

int IsChaseFade(void)
{
    return ChaseFlag;
}

float GetChaseTrnsl(int id)
{
    if (ChaseFlag) {
        if (id < trans_bg_id) {
            return 0.0f;
        }
        if (id == trans_bg_id) {
            return transval;
        }
        return 1.0f;
    }
    return 1.0f;
}

void SetChaseTrnsl(int val)
{
    int id;

    id = 0;
    if (val <= 0) {
        transval = 1.0f;
        trans_bg_id = 0;
        ChaseFlag = 0;
        return;
    }
    while (val >= 100) {
        val -= 100;
        id++;
    }
    transval = 1.0 - (float)val / 100.0;
    trans_bg_id = id;
    ChaseFlag = 1;
}

int GetScreenBGY(void);
int IsChase(void);
void SetYClipOff(void);
void SetChaseTrnsl(int val);

void ChaseFadeOutExe(void)
{
    int add;
    int val;

    val = -GetScreenBGY() / 100 - 100;
    if (IsChase()) {
        if (val > 0) {
            ChaseTime = 60;
        }
        if (ChaseTime > 0) {
            add = (val - FadeOutVal) / 5;
            if (add < 6) {
                FadeOutVal += 5;
            } else {
                FadeOutVal += add;
            }
        }
    } else if (ChaseTime > 0) {
        ChaseTime--;
        add = (val - FadeOutVal) / 5;
        if (add < 6) {
            FadeOutVal += 5;
        } else {
            FadeOutVal += add;
        }
        if (FadeOutVal > 400) {
            ChaseTime = 0;
        }
    } else if (FadeOutVal <= 0) {
        FadeOutVal = 0;
    } else if (val < 300) {
        val = -50;
        if (FadeOutVal > val) {
            add = (val - FadeOutVal) / 10;
            if (add == 0) {
                FadeOutVal--;
            } else {
                FadeOutVal += add;
            }
        }
    } else {
        ChaseTime = 10;
    }
    SetChaseTrnsl(FadeOutVal);
    if (FadeOutVal >= 250) {
        SetYClipOff();
    }
}

int IsChaseEffectDraw(void)
{
    if (FadeOutVal <= 0) {
        return 0;
    }
    return FadeOutVal / 100 + 1;
}

void SetBlendingMode(short int mode);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);

void TokorotenDraw(int x, int y, int z, unsigned int color, unsigned int color2)
{
    NLVERTEX00 vertexu[4];
    NLVERTEX00 *vp;

    SetBlendingMode(1);
    vp = vertexu;
    vp->x = x - z / 200;
    vp->y = y;
    vp->z = 300.0f;
    vp->col = color2;
    vp++;
    vp->x = x;
    vp->y = y - z / 100;
    vp->z = 300.0f;
    vp->col = color2;
    vp++;
    vp->x = x;
    vp->y = y;
    vp->z = 300.0f;
    vp->col = color;
    vp++;
    vp->x = x + z / 200;
    vp->y = y;
    vp->z = 300.0f;
    vp->col = color2;
    nlDispStripTrnsl(4, vertexu);
    vp = vertexu;
    vp->x = x - z / 200;
    vp->y = y;
    vp->z = 300.0f;
    vp->col = color2;
    vp++;
    vp->x = x;
    vp->y = y + z / 5;
    vp->z = 300.0f;
    vp->col = color2;
    vp++;
    vp->x = x;
    vp->y = y;
    vp->z = 300.0f;
    vp->col = color;
    vp++;
    vp->x = x + z / 200;
    vp->y = y;
    vp->z = 300.0f;
    vp->col = color2;
    nlDispStripTrnsl(4, vertexu);
    SetBlendingMode(0);
}

int BGWorld2ScreenY(int y);
float GetBGFade(void);
int Random(void);
void TokorotenDraw(int x, int y, int z, unsigned int color, unsigned int color2);
void WaterFallDraw(WATERFALL *wtfp);
void bgprecalc(int scrollrate);
void nlSetBackColor(int c1, int c2, int c3);
int rand(void);
extern CHASE_EFFECT ChaseEffect[64];
extern WATERFALL wtf[6];
extern int basey;
extern int basealfa;
extern GAME_INFOMATION g_info;
extern u_int TotalCounter;

void ChaseEffectExe(void)
{
    int x;
    int y;
    unsigned int alfa;
    unsigned int r;
    unsigned int g;
    unsigned int w;
    int val;

    if (FadeOutVal >= 100 && !(g_info.BattleControlFlag & 0xC0)) {
    if (FadeOutVal >= 200) {
        val = FadeOutVal - 200;
        if (val > 200) {
            val = 200;
        }
        val = val * GetBGFade();
        if (val) {
            r = val * 200.0 / 200.0;
            g = val * 150.0 / 600.0;
            w = TotalCounter & 0x1F;
            if (TotalCounter & 0x20) {
                w = 0x20 - w;
            }
            g = (g * (w + 0x20)) >> 6;
            r = (g << 8) | (r << 16);
            nlSetBackColor(r, r, r);
        }
    }
    bgprecalc(1000);
    basey -= 5;
    while (basey <= 0) {
        basey += 1200;
    }
    for (val = 0; val < 64; val++) {
        if (ChaseEffect[val].time <= 0) {
            ChaseEffect[val].y = -(rand() % 1000) * 100;
            ChaseEffect[val].x = Random() % 640;
            ChaseEffect[val].z = rand() % 80 * 10 + 300;
            ChaseEffect[val].time = (rand() & 0x3F) + 0x40;
            ChaseEffect[val].createtime = 32;
        } else {
            if (ChaseEffect[val].z > 0x442) {
                ChaseEffect[val].z = 0x442;
            } else if (ChaseEffect[val].z < 300) {
                ChaseEffect[val].z = 300;
            }
            y = BGWorld2ScreenY(ChaseEffect[val].y);
            ChaseEffect[val].y -= ChaseEffect[val].z * 5;
            x = ChaseEffect[val].x;
            if (ChaseEffect[val].createtime-- > 0) {
                alfa = (32 - ChaseEffect[val].createtime) * 4;
            } else {
                alfa = (ChaseEffect[val].time < 32) ? ChaseEffect[val].time * 4 : 0x80;
            }
            alfa = alfa * GetBGFade();
            alfa *= 2;
            alfa--;
            alfa <<= 24;
            TokorotenDraw(x, y, ChaseEffect[val].z, alfa | 0xFFFFFF, alfa | 0x800000);
            ChaseEffect[val].time--;
        }
    }
    if (FadeOutVal > 250) {
        if (FadeOutVal - 350 > 0) {
            basealfa = 100;
        } else {
            basealfa = FadeOutVal - 250;
        }
        basealfa = basealfa * GetBGFade();
        if (basealfa) {
            WaterFallDraw(&wtf[0]);
            WaterFallDraw(&wtf[1]);
            WaterFallDraw(&wtf[2]);
            WaterFallDraw(&wtf[3]);
            WaterFallDraw(&wtf[4]);
            WaterFallDraw(&wtf[5]);
        }
    }
    }
}

extern WATERFALL wtf[6];
extern int JSTokorotenFlag;

void ChaseInit(void)
{
    wtf[0].scrx = 640;
    wtf[0].offx = -8;
    wtf[0].scry = 0;
    wtf[0].offy = 0;
    wtf[1].scrx = 640;
    wtf[1].offx = -8;
    wtf[1].scry = 400;
    wtf[1].offy = 0;
    wtf[2].scrx = 640;
    wtf[2].offx = -8;
    wtf[2].scry = 800;
    wtf[2].offy = 0;
    wtf[3].scrx = 0;
    wtf[3].offx = 8;
    wtf[3].scry = 0;
    wtf[3].offy = 0;
    wtf[4].scrx = 0;
    wtf[4].offx = 8;
    wtf[4].scry = 400;
    wtf[4].offy = 0;
    wtf[5].scrx = 0;
    wtf[5].offx = 8;
    wtf[5].scry = 800;
    wtf[5].offy = 0;
    FadeOutVal = -100;
    ChaseTime = 0;
    transval = 1.0f;
    ChaseFlag = 0;
    trans_bg_id = 0;
    JSTokorotenFlag = 0;
}


void ChaseEnd(void)
{
}

u_int RGBConv2(u_int rgb);
void nlcDispStripTrnsl(int num, NLVERTEX00 *vt);

void WaterFallDraw(WATERFALL *wtfp)
{
    NLVERTEX00 vertexu[64];
    NLVERTEX00 vertexd[64];
    NLVERTEX00 *vp;
    int i;
    int scrx;
    int offx;
    int scry;
    int offy;
    int x;
    int y;
    int h;
    int c1;
    int col;

    SetBlendingMode(1);
    scrx = wtfp->scrx;
    offx = wtfp->offx;
    bgprecalc(1000);
    scry = basey + wtfp->scry + BGWorld2ScreenY(0);
    while (scry > 1200) scry -= 1200;
    scry -= 600;
    offy = -(scry * 2 / 3);
    scry += 240;
    scry <<= 5;
    x = scrx;
    y = scry;
    wtf->height[wtf->index] = (Random() & 0x7F) + 0x40;
    for (i = 0; i < 32; i++, x += offx, y += offy) {
        h = wtf->height[(wtf->index + i) & 0x3F] * (0.7 - i * 0.02);
        col = (0xFF - i * 8) * basealfa / 100;
        c1 = RGBConv2(col << 24 | 0xFFFFFF);
        vp = &vertexu[i * 2];
        vp->x = x;
        vp->y = y / 32;
        vp->z = 320.0f;
        vp->col = c1;
        vp++;
        vp->x = x;
        vp->y = y / 32 + h;
        vp->z = 320.0f;
        vp->col = RGBConv2(col << 24 | 0xFF << 16 | (0xF0 - i * 3) << 8);
        vp = &vertexd[i * 2];
        vp->x = x;
        vp->y = y / 32;
        vp->z = 320.0f;
        vp->col = c1;
        vp++;
        vp->x = x;
        vp->y = y / 32 - h;
        vp->z = 320.0f;
        vp->col = RGBConv2(col << 24 | 0xFF << 16 | (0xF0 - i * 3) << 8);
    }
    nlcDispStripTrnsl(64, vertexu);
    nlcDispStripTrnsl(64, vertexd);
    y = scry;
    c1 = RGBConv2(0xFF0000);
    for (i = 0; i < 32; i++, y += offy) {
        h = wtf->height[(wtf->index + i) & 0x3F] * (1.5 - i * 0.04);
        vp = &vertexu[i * 2];
        vp->y = y / 32 + h;
        vp->col = c1;
        vp = &vertexd[i * 2];
        vp->y = y / 32 - h;
        vp->col = c1;
    }
    nlcDispStripTrnsl(64, vertexu);
    nlcDispStripTrnsl(64, vertexd);
    wtfp->index--;
    wtfp->index &= 0x3F;
    SetBlendingMode(0);
}
