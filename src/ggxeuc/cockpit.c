#include "generated/cockpit.h"

void FileRead(int fid, void *adr);
int GetFileSize(int fid);
void *mmalloc(int size, char *name);
extern char *CockpitFile;

void CockpitLoad(void)
{
    if (CockpitFile == 0) {
        CockpitFile = mmalloc(GetFileSize(0x99), "CockpitFile");
        FileRead(0x99, CockpitFile);
    }
}

void LinkAddrTb(u_int *addr);
extern char *CockpitFile;
extern int TransSprtID;

void CockpitLoadInit(void)
{
    LinkAddrTb((u_int *)CockpitFile);
    TransSprtID = 0x140;
}

int IST_Load(int *texadr, int sprid, int palid);
void SetPVRMinSize(short unsigned int x, short unsigned int y);
extern char *CockpitFile;
extern int TransSprtID;

int CockpitImageLoad(int no, int *pal, int sprtid)
{
    int *addr;

    addr = (int *)CockpitFile + no;
    SetPVRMinSize(8, 8);
    IST_Load(addr, TransSprtID + sprtid, -1);
    *pal = -1;
    return TransSprtID + sprtid;
}

int CockpitImageLoad(int no, int *pal, int sprtid);
extern short unsigned int cp_SprtIdx[128];

void CreditFontLoad(int id)
{
    int pal;

    cp_SprtIdx[0x43] = CockpitImageLoad(0x43, &pal, id);
    cp_SprtIdx[0x44] = CockpitImageLoad(0x44, &pal, id + 1);
    cp_SprtIdx[0x45] = CockpitImageLoad(0x45, &pal, id + 2);
}

short int GetSpriteIdTop(short unsigned int max);
int IST_Load(int *texadr, int sprid, int palid);
void LinkAddrTb(u_int *addr);
extern short unsigned int cp_SprtIdx[128];
extern GAME_INFOMATION g_info;
extern char *CockpitFile;
extern int TransSprtID;

void CockpitInit(void)
{
    int *addr;
    short int palidx;
    int i;
    u_short id;

    addr = (int *)CockpitFile;
    i = 0;
    LinkAddrTb((u_int *)addr);
    TransSprtID = GetSpriteIdTop(4);
    for (; *addr; addr++, i++) {
        palidx = -1;
        switch (i) {
        case 0 ... 0x11:
        case 0x2C ... 0x42:
        case 0x47 ... 0x58:
            palidx = -2;
            break;
        }
        if (palidx != -2) {
            id = GetSpriteIdTop(1);
            IST_Load(addr, id, -1);
            cp_SprtIdx[i] = id;
        }
    }
    g_info.BattleControlFlag &= ~2;
}

void PXF_GetSize(int id, int *x, int *y);
void gStripDrawTop(float *v, short int *sz, int id, int dirflag);
extern short unsigned int cp_SprtIdx[128];

void CockpitDraw(int id, int x, int y, float z, int dirflag)
{
    float v[3];
    int xx;
    int yy;
    short int pos[2];
    int sprtid;

    sprtid = cp_SprtIdx[id];
    v[0] = x;
    v[1] = y;
    v[2] = z;
    PXF_GetSize(sprtid, &xx, &yy);
    pos[0] = xx;
    pos[1] = yy;
    gStripDrawTop(v, pos, sprtid, dirflag);
}

void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void CockpitDraw_Scale(int id, int x, int y, float z, float zx, float zy, int attr)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = zx;
    sprite.zm_y = zy;
    sprite.listType = 1;
    sprite.attr = attr;
sprite.u0 = 0;
sprite.u1 = 1.0f;
sprite.v0 = 0;
sprite.v1 = 1.0f;
sprite.trnsl = 1.0f;
    sprite.ang = 0;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void CockpitDraw(int id, int x, int y, float z, int dirflag);
void DrawName(int p);
void FontDraw(char *str, int x, int y, float z, int alfa);
char *G_sprintf_arg1(char *str, int arg1);
int IsCounterHitByGuardCounter(CHARACTER_WORK *offset);
int IsEvent(u_int flag);
void TensionDraw(CHARACTER_WORK *offset, int side);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
extern USER_SETTING userinfo;
extern int cpsenterpos[2];
extern CHARACTER_WORK *PlayerWork;
extern int PLAYER_HITPOINTMAX;
extern short int UseCharacter1p2p[];
extern u_int TotalCounter;

void CockpitDraw1p2p(int side)
{
    int life;
    int life2;
    int color;
    int color2;
    CHARACTER_WORK *offset;
    int offu;
    int gdp;

    offset = &PlayerWork[side];
    offu = userinfo.offs_cockpit_upper;
    life = (float)offset->ply->lifelength / PLAYER_HITPOINTMAX * 240.0f;
    life2 = (float)offset->ply->lifelengthdamage / PLAYER_HITPOINTMAX * 240.0f;
    if (life == 240) {
        color = 0x780000FF;
    } else if (life > 120) {
        color = 0x7800FF00;
    } else {
        color = 0x7880FF00;
    }
    color = gStripGetFadeColor(color);
    color2 = gStripGetFadeColor(0xFFFFFFFF);
    if (IsEvent(2)) {
        FontDraw(G_sprintf_arg1("%3d", PLAYER_HITPOINTMAX - PlayerWork[side].ply->lifelength), cpsenterpos[offset->padid] - 0x12, 0x14, 274.0f, 0x80);
    }
    if (side == 0) {
        polydraw_GBox(0x120 - life, offu + 0x19, 288.0f, offu + 0x25, 275.0f, color2, color2, color, color);
        polydraw_FBox(0x120 - life, offu + 0x19, 0x120 - life2, offu + 0x25, 275.0f, gStripGetFadeColor(0x60FF0000));
    } else {
        polydraw_GBox(352.0f, offu + 0x19, life + 0x160, offu + 0x25, 275.0f, color2, color2, color, color);
        polydraw_FBox(life + 0x160, offu + 0x19, life2 + 0x160, offu + 0x25, 275.0f, gStripGetFadeColor(0x60FF0000));
    }
    gdp = (offset->ply->GuardPoint + 0x80) * 0x58 / 256;
    if ((TotalCounter & 2) && IsCounterHitByGuardCounter(offset)) {
        color = gStripGetFadeColor(0xFFFFFFFF);
    } else {
        color = gStripGetFadeColor(0xC0FF0000);
    }
    if (side == 0) {
        CockpitDraw(0x24, 0x20, offu + 0xE, 280.0f, 0);
        CockpitDraw(UseCharacter1p2p[0] + 0x11, 0x20, offu + 0x2A, 280.0f, 0);
        CockpitDraw(0x26, 0xA0, offu + 0x29, 280.0f, 0);
        polydraw_GBox(0x120 - gdp, offu + 0x2C, 288.0f, offu + 0x30, 275.0f, gStripGetFadeColor(0xFFFFFFFF), gStripGetFadeColor(0xFFFFFFFF), color, color);
    } else {
        CockpitDraw(0x24, 0x260, offu + 0xE, 280.0f, 1);
        CockpitDraw(UseCharacter1p2p[1] + 0x11, 0x260, offu + 0x2A, 280.0f, 1);
        CockpitDraw(0x27, 0x160, offu + 0x29, 280.0f, 0);
        polydraw_GBox(352.0f, offu + 0x2C, gdp + 0x160, offu + 0x30, 275.0f, gStripGetFadeColor(0xFFFFFFFF), gStripGetFadeColor(0xFFFFFFFF), color, color);
    }
    TensionDraw(&PlayerWork[side], side);
    DrawName(side);
}

void CockpitDraw1p2p(int side);
void CockpitDrawCmn(void);
void DispCreditMsg(void);
int GetFadeCockpitFlag(void);
int IsDemoBattle(void);
void SetBlendingMode(short int mode);
void gColPush(unsigned int color);
void gCurrentColorClear(void);
extern GAME_INFOMATION g_info;

void CockpitExecute(void)
{
    gCurrentColorClear();
    SetBlendingMode(0);
    if (GetFadeCockpitFlag()) {
        gColPush(0xFFFFFF);
    }
    if (!(g_info.BattleControlFlag & 2) && !IsDemoBattle()) {
        CockpitDraw1p2p(0);
        CockpitDraw1p2p(1);
        DispCreditMsg();
        CockpitDrawCmn();
        gCurrentColorClear();
    }
}

void gSpriteDrawWH(G_SPR_ARGXYWH *sprite);
extern short unsigned int cp_SprtIdx[128];

void CpTimeDraw(int x, int y, float z, int n);
char *G_sprintf(char *buff, char *str, int *valtb);
void BeatFontDraw(char *str, int x, int y, float z, int alfa, float t);
extern int BattleTime;
/* .rodata after the TU's ChName strings ("TESTAMENT", "POTEMKIN"), which are not defined here yet */
extern char D_5BFCC0[]; /* RODATA_EXTERN 0x5bfcc0: "BONUS %8d" */

void CockpitDrawCmn(void)
{
    GAME_INFOMATION *p;
    CHARACTER_WORK *offset;
    int offu;
    int i;
    int valtb[2];

    offu = userinfo.offs_cockpit_upper;
    p = &g_info;
    CockpitDraw(0x25, 0x120, offu - 4, 280.0f, 0);
    CpTimeDraw(0x127, offu + 0x2E, 265.0f, BattleTime / p->TimeByCount);
    offset = PlayerWork;
    for (i = 0; i < 2; i++, offset++) {
        if (offset->ply->hitDispTime && offset->ply->HitCount >= 2) {
            valtb[0] = offset->ply->HitCount;
            FontDraw("BEAT", cpsenterpos[offset->padid == 0] - 0x5E, 0x70, 265.0f, 0xFF);
            BeatFontDraw(G_sprintf(0, "%3d", valtb), cpsenterpos[offset->padid == 0] - 0x62, 0x58, 266.0f, 0xFF, offset->ply->hitDispIn);
            if (IsEvent(2)) {
                FontDraw(G_sprintf_arg1(D_5BFCC0, p->combototal[offset->padid == 0]), cpsenterpos[offset->padid == 0] - 0x5E, 0xA0, 264.0f, 0xFF);
            }
        }
    }
}


extern char *ChName[];
extern short int UseCharacter1p2p[];
/* DrawName's local size[] initializer lives in .rodata after the TU's ChName strings,
   which are not defined here yet: copy it from the original's bytes instead */
typedef struct {
    char size[18];
} DrawName_sizetb;
extern DrawName_sizetb D_5BFCD0; /* RODATA_EXTERN 0x5bfcd0: { 3, 2, 3, 6, 3, 8, 5, 4, 6, 5, 9, 3, 4, 6, 5, 5, 5, 7 } */

void DrawName(int p)
{
    int offu;
    DrawName_sizetb size = D_5BFCD0;

    offu = userinfo.offs_cockpit_upper;
    if (p) {
        FontDraw(ChName[UseCharacter1p2p[p] - 1], 0x260 - size.size[UseCharacter1p2p[p] - 1] * 12, offu + 0x40, 265.0f, 0xFF);
    } else {
        FontDraw(ChName[UseCharacter1p2p[0] - 1], 0x20, offu + 0x40, 265.0f, 0xFF);
    }
}


extern u_int TotalCounter;
extern char D_5BFCE8[]; /* RODATA_EXTERN 0x5bfce8: "DEMO PLAY" (follows the TU's ChName strings in .rodata) */

void DrawDemoPlayandLogo(void)
{
    if (TotalCounter & 0x20) {
        FontDraw(D_5BFCE8, 0x10A, 0x120, 1.0f, 0xFF);
    }
}


void FontDraw(char *str, int x, int y, float z, int alfa)
{
    int u;
    int v;
    G_SPR_ARGXYWH *sprite;
    G_SPR_ARGXYWH spritework;

    sprite = &spritework;
sprite->x = x;
sprite->y = y;
sprite->z = z;
sprite->trnsl = alfa;
sprite->attr = 0;
sprite->zm_x = 1.0f;
sprite->zm_y = 1.0f;
    for (; *str; str++, sprite->x += 12.0f) {
        if (*str >= '0' && *str <= '9') {
            u = *str - '0';
            v = 0;
            while (u >= 5) {
                u -= 5;
                v++;
            }
        } else if (*str >= 'A' && *str <= 'Z') {
            u = *str - 'A';
            v = 2;
            while (u >= 5) {
                u -= 5;
                v++;
            }
        } else if (*str == '-') {
            u = 0;
            v = 8;
        } else if (*str == '+') {
            u = 1;
            v = 8;
        } else if (*str == '.') {
            u = 2;
            v = 8;
        } else if (*str == '!') {
            u = 3;
            v = 8;
        } else if (*str == ',') {
            u = 4;
            v = 8;
        } else if (*str == '?') {
            u = 0;
            v = 9;
        } else if (*str == '/') {
            u = 1;
            v = 9;
        } else if (*str == '(') {
            u = 2;
            v = 9;
        } else if (*str == ')') {
            u = 3;
            v = 9;
        } else if (*str == '@') {
            u = 4;
            v = 9;
        } else if (*str == '&') {
            u = 3;
            v = 11;
        } else if (*str == '>') {
            u = 4;
            v = 11;
        } else if (*str == 'a') {
            u = 1;
            v = 11;
        } else if (*str == 'b') {
            u = 3;
            v = 10;
        } else if (*str == 'c') {
            u = 2;
            v = 11;
        } else if (*str == 'd') {
            u = 0;
            v = 10;
        } else if (*str == 'e') {
            u = 4;
            v = 8;
        } else if (*str == 'f') {
            u = 1;
            v = 10;
        } else if (*str == 'g') {
            u = 4;
            v = 10;
        } else if (*str == 'h') {
            u = 2;
            v = 10;
        } else if (*str == 'i') {
            u = 0;
            v = 11;
        } else if (*str == 'w') {
            u = 1;
            v = 7;
        } else if (*str == 'x') {
            u = 2;
            v = 7;
        } else if (*str == 'y') {
            u = 3;
            v = 7;
        } else if (*str == 'z') {
            u = 4;
            v = 7;
        } else {
            continue;
        }
        sprite->sprno = cp_SprtIdx[v / 4 + 0x43];
        v &= 3;
        v *= 16;
        sprite->u0 = u * 12;
        sprite->v0 = v;
        sprite->u1 = u * 12 + 12;
        sprite->v1 = v + 16;
        gSpriteDrawWH(sprite);
    }
}

void gSpriteDrawWH(G_SPR_ARGXYWH *sprite);
extern short unsigned int cp_SprtIdx[128];

void BeatFontDraw(char *str, int x, int y, float z, int alfa, float t)
{
    int u;
    int v;
    G_SPR_ARGXYWH *sprite;
    G_SPR_ARGXYWH spritework;

    sprite = &spritework;
    sprite->sprno = cp_SprtIdx[0x46];
    sprite->x = x;
    sprite->z = z;
    sprite->trnsl = alfa;
    sprite->y = y;
    sprite->attr = 0;
    sprite->zm_x = 1.0f;
    sprite->zm_y = 1.0f;
    for (; *str; str++, sprite->x += 64.0f) {
        if (*str >= '0' && *str <= '9') {
            u = *str - '0';
            v = 0;
            while (u >= 4) {
                u -= 4;
                v++;
            }
            sprite->zm_y = 1.0f;
            sprite->attr = 0;
            sprite->u0 = u * 64;
            sprite->u1 = u * 64 + 64;
            sprite->v0 = v * 80 + (1.0f - t) * 80.0f;
            sprite->v1 = v * 80 + 80;
            sprite->y = y + (1.0f - t) * 80.0f;
            gSpriteDrawWH(sprite);
            if (t < 1.0f) {
                sprite->attr = 1;
                sprite->zm_y = 2.0f;
                sprite->v0 = v * 80;
                sprite->v1 = v * 80 + (1.0f - t) * 80.0f;
                sprite->y -= (1.0f - t) * 2.0f * 80.0f;
                gSpriteDrawWH(sprite);
            }
        }
    }
}

int SetRightPos(char *str, int x, int size)
{
    int chars;

    chars = 0;
    while (*str) {
        str++;
        chars++;
    }
    return x - size * chars;
}

extern FONT_WORK font_work[2][16];

void font_entry(int side, char *str)
{
    char *dst;
    int i;

    if (side < 0) {
        for (side = 0; side < 2; side++) {
            for (i = 0; i < 16; i++) {
                font_work[side][i].status = 0;
            }
        }
        return;
    }
    for (i = 0; i < 16; i++) {
        if (font_work[side][i].status == 0) {
font_work[side][i].y = 0xA0;
font_work[side][i].time = 0x78;
font_work[side][i].status = 1;
font_work[side][i].x = 0;
font_work[side][i].acc = 0;
            dst = font_work[side][i].str;
            while (*str) {
                *dst++ = *str++;
            }
            *dst = 0;
            return;
        }
    }
}

void font_execute(int size)
{
    int side;
    int i;
    int j;
    int hit;
    int alfa;
    static int xbyside[2] = { 80, 400 };
    int fontsize;

    alfa = 0;
    fontsize = (size + 1) * 8;
    for (side = 0; side < 2; side++) {
        for (i = 0; i < 16; i++) {
            if (font_work[side][i].status == 0) {
                continue;
            }
            if (--font_work[side][i].time <= 0) {
                if (font_work[side][i].status == 1) {
                    font_work[side][i].status = 2;
                    font_work[side][i].time = 0x20;
                } else {
                    font_work[side][i].status = 0;
                    continue;
                }
            }
            hit = 0;
            if (font_work[side][i].status != 2) {
                for (j = 0; j < 16; j++) {
                    if (font_work[side][j].status == 1 && font_work[side][j].y < font_work[side][i].y) {
                        if (font_work[side][j].y + fontsize >= font_work[side][i].y) {
                            font_work[side][i].y = font_work[side][j].y + fontsize;
                            font_work[side][i].acc = 0;
                            hit = 1;
                        }
                    }
                }
                if (hit) {
                    goto draw;
                }
            }
            {
                if (font_work[side][i].status == 2) {
                    if (side == 0) {
                        font_work[side][i].x -= 8;
                    } else {
                        font_work[side][i].x += 8;
                    }
                    alfa = font_work[side][i].time * 8;
                } else {
                    font_work[side][i].y -= ++font_work[side][i].acc / 4;
                }
            }
        draw:
            if (font_work[side][i].status == 1) {
                alfa = 0x100 - font_work[side][i].time * 2;
            }
            if (font_work[side][i].y < 0) {
                font_work[side][i].y = 0;
            }
            FontDraw(font_work[side][i].str, xbyside[side] + font_work[side][i].x, font_work[side][i].y + 0xA0, 265.0f, alfa);
        }
    }
}

void gSpriteDrawPS(NL_SPR_ARG *sprite);
void SetBlendingMode(short int mode);

void CpTimeDraw(int x, int y, float z, int n)
{
    NL_SPR_ARG sprite;
    int one;
    int ten;

    sprite.sprno = cp_SprtIdx[0x2A];
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.listType = 1;
    sprite.attr = 13;
    sprite.trnsl = 1.0f;
    sprite.ang = 0;
    if ((unsigned int)n < 100) {
        sprite.zm_x = 24.0f;
        sprite.zm_y = 36.0f;
        sprite.v0 = 0;
        sprite.v1 = 36.0f;
        ten = n / 10;
        one = n;
        while (one >= 10) {
            one -= 10;
        }
        sprite.u0 = ten * 24.0f;
        sprite.u1 = (ten + 1) * 24.0f;
        SetBlendingMode(0);
        gSpriteDrawPS(&sprite);
        sprite.x += 24.0f;
        sprite.u0 = one * 24.0f;
        sprite.u1 = (one + 1) * 24.0f;
        gSpriteDrawPS(&sprite);
    } else {
        sprite.zm_x = 48.0f;
        sprite.zm_y = 28.0f;
        sprite.v0 = 36.0f;
        sprite.v1 = 64.0f;
        sprite.x += 1.0f;
        sprite.y -= 4.0f;
        sprite.u0 = 0;
        sprite.u1 = 48.0f;
        gSpriteDrawPS(&sprite);
    }
}

short int GetTensionVal(CHARACTER_WORK *offset);
void polydraw_FBoxOPQ(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void TensionDraw(CHARACTER_WORK *offset, int side)
{
    static short int basepos[2] = { 54, 587 };
    int base = basepos[side];
    int val = GetTensionVal(offset);
    register int k1 asm("$4") = 0xC1, k2 asm("$6") = 10000, m0 asm("$3"); /* FAKEMATCH: pins 193 to a0 and 10000 to a2; m0 is a code-free v1 placeholder. See FAKEMATCHES.md */
    register int tb asm("$5") = offset->ply->TensionBarLength * k1; /* FAKEMATCH: pins the lh/mult value to a1 */
    int ten = tb / k2; /* FAKEMATCH: natural C is offset->ply->TensionBarLength * 0xC1 / 10000 */
    int a = (side == 0) ? 1 : -1;
    unsigned int color;
    float b;
    int i;
    static float cnt[2];
    int offl;

    __asm__("" : "=r"(m0)); /* FAKEMATCH: empty asm, v1 busy from here... */
    __asm__("" : "+r"(a) : "r"(m0)); /* FAKEMATCH: ...to here; emits no code */
    if (side) b = 1.0f; /* FAKEMATCH: dead branch, a block split that fences sched1 only; no code survives */
    offl = userinfo.offs_cockpit_lower;
    if (offset->ply->OverKillFlag == 1) {
        if (side == 0) {
            CockpitDraw(0x2B, 0x1C, offl + 0x1C0, 49.0f, 0);
        } else {
            CockpitDraw(0x2B, 0x266, offl + 0x1C0, 49.0f, 1);
        }
        color = gStripGetFadeColor(0xFFFF0000);
        polydraw_FBoxOPQ(base, offl + 0x1CC, a * ten + base, offl + 0x1D8, 50.0f, color);
        return;
    }
    if (offset->ply->OverKillFlag != 0) {
        return;
    }
    if (offset->ply->NegativeVal > 6000) {
        FontDraw("DANGER", cpsenterpos[side] - 0x24, offl + 0x1C0, 49.0f, 0xFF);
    }
    if (side == 0) {
        CockpitDraw(0x28, 0x10, offl + 0x1C0, 60.0f, 0);
    } else {
        CockpitDraw(0x29, 0x172, offl + 0x1C0, 60.0f, 0);
    }
    if (offset->ply->TensionBalance < -10000) b = 0.05f;
    else if (offset->ply->TensionBalance < -6000) b = 0.15f;
    else if (offset->ply->TensionBalance < -3000) b = 0.3f;
    else if (offset->ply->TensionBalance < -1500) b = 0.6f;
    else if (offset->ply->TensionBalance < 3000) b = 1.0f;
    else b = 2.0f;
    cnt[side] -= b;
    if (ten == 0) {
        cnt[side] = 0;
    }
    for (i = 0; i < 0xC1; i += 8) {
        int h;
        int index;
        float hl;

        if (i < 0x30) {
            h = 0x1D2;
        } else if (i < 0x60) {
            h = 0x1D0;
        } else if (i <= 0x8F) {
            h = 0x1CE;
        } else {
            h = 0x1CC;
        }
        index = cnt[side] + (float)(i / 8);
        if (index & 0x10) {
            index = 0x10 - (index & 0xF);
        } else {
            index &= 0xF;
        }
        hl = (float)index * (1.0f - (float)offset->ply->NegativeVal / 10000.0f);
        if (val < 5000) color = gStripGetFadeColor(0xFF000000 | ((u_int)(hl * 50.0f * 0.0625f + 30.0f) << 16) | ((u_int)(hl * 50.0f * 0.0625f + 30.0f) << 8) | (u_int)(hl * 125.0f * 0.0625f + 105.0f));
        else if (val < 10000) color = gStripGetFadeColor(0xFF000000 | ((u_int)(hl * 100.0f * 0.0625f + 155.0f) << 16) | ((u_int)(hl * 80.0f * 0.0625f + 55.0f) << 8) | (u_int)(hl * 80.0f * 0.0625f + 55.0f));
        else color = gStripGetFadeColor(0xFF000000 | ((u_int)(hl * 100.0f * 0.0625f + 155.0f) << 16) | ((u_int)(hl * 100.0f * 0.0625f + 120.0f) << 8) | (u_int)(hl * 50.0f * 0.0625f + 55.0f));
        if (ten - i >= 8) {
            polydraw_FBoxOPQ(a * i + base, h + offl, a * (i + 8) + base, offl + 0x1D8, 50.0f, color);
        } else if (ten - i > 0) {
            polydraw_FBoxOPQ(a * i + base, h + offl, a * ten + base, offl + 0x1D8, 50.0f, color);
        } else {
            break;
        }
    }
}
