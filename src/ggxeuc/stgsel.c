#include "generated/stgsel.h"
void StgSelLoad(void);
void StgSelFree(void);
void StgSelInit(void);
void DrawBG(int id, int x, int y, float z);
void DrawIconSub(int id, int x, int y, int flag);
void DrawIcon(int y3);
void DispChar_st(float x, float y, float z, float t, char c);
void strloop(int fms);
void StgSelMain(void);
void StgSelDC(void);
char *StgSelFile = 0;
static CHWORK chwork[26];

char *FileReadSelect(int fid, int chan);

void StgSelLoad(void)
{
    StgSelFile = FileReadSelect(0xA4, 2);
}

void BGM_Free(void);
void FadeInit(void);
void JobChange(int job);
void SetStageNo(int no);
int VsCpuCtrl(void);
void dfree(void *mem);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void sound_alloff(void);
extern int StageNo;

void StgSelFree(void)
{
    SetStageNo(StageNo);
    FadeInit();
    sound_alloff();
    BGM_Free();
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    dfree(StgSelFile);
    StgSelFile = 0;
    JobChange(VsCpuCtrl());
}

int FileReadCheck(void);
void JobChange(int job);
extern GAME_INFOMATION g_info;
extern int SubJob;
extern int SubJobCount;

void StgSelInit(void)
{
    if (FileReadCheck()) {
        if (!(g_info.mode & 0x900)) {
            JobChange(0x13);
        } else {
            JobChange(0x14);
        }
        SubJob = 0;
        SubJobCount = 0;
    }
}

void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void DrawBG(int id, int x, int y, float z)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.zm_x = 0.625f;
    sprite.zm_y = 0.390625f;
    sprite.ang = 0;
    sprite.u0 = 0.0f;
    sprite.u1 = 0.625f;
    sprite.z = z;
    sprite.x = x;
    sprite.y = y;
    sprite.v1 = 0.390625f;
    sprite.v0 = 0.0f;
    sprite.trnsl = 1.0f;
    sprite.listType = 0;
    sprite.attr = 5;
    gSpriteDraw_NaomiLibVersion(&sprite);
    sprite.v0 = 0.5f;
    sprite.v1 = 0.890625f;
    sprite.x += 320.0;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void DrawIconSub(int id, int x, int y, int flag)
{
    NL_SPR_ARG sprite;

    if (flag) {
        sprite.sprno = 20;
    } else {
        sprite.sprno = id + 38;
    }
    sprite.ang = 0;
    sprite.u0 = 0.0f;
    sprite.u1 = 0.5625f;
    sprite.v0 = 0.0f;
    sprite.v1 = 0.421875f;
    sprite.listType = 0;
    sprite.zm_x = 0.5625f;
    sprite.zm_y = 0.421875f;
    sprite.z = 9.0f;
    sprite.x = x;
    sprite.y = y - 74.0f;
    sprite.trnsl = 1.0f;
    sprite.attr = 0x20D;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

extern GAME_INFOMATION g_info;

void DrawIcon(int y3)
{
    int i;
    int x;
    int y;
    GAME_INFOMATION *gp = &g_info;

    for (i = 0; i <= gp->BattleCnt[gp->Player]; i++) {
        x = (i < 5) ? 0x28 : 0x20F;
        y = (i % 5) * 62 + 216 - y3;
        if (i == gp->BattleCnt[gp->Player]) {
            DrawIconSub(gp->BattleHistory[gp->Player][i] - 1, x, y, 1);
        } else {
            DrawIconSub(gp->BattleHistory[gp->Player][i] - 1, x, y, 0);
        }
    }
}

void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void DispChar_st(float x, float y, float z, float t, char c)
{
    NL_SPR_ARG sprite;
    float u0;
    float u1;
    float v0;
    float v1;

    sprite.sprno = 60;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = 0.125f;
    sprite.zm_y = 0.25f;
    sprite.trnsl = t;
    sprite.attr = 13;
    sprite.ang = 0;
    sprite.listType = 0;
    if (c < 'A' || c > 'Z') {
        u0 = v0 = 0.0f;
    } else {
        if (c < 'I') {
            v0 = 0.0f;
            u0 = (c - 'A') * 16.0;
        } else if (c < 'Q') {
            v0 = 16.0f;
            u0 = (c - 'I') * 16.0;
        } else if (c < 'Y') {
            v0 = 32.0f;
            u0 = (c - 'Q') * 16.0;
        } else {
            v0 = 48.0f;
            u0 = (c - 'Y') * 16.0;
        }
    }
    u1 = u0 + 16.0;
    v1 = v0 + 16.0;
    sprite.u0 = u0 * 0.0078125;
    sprite.u1 = u1 * 0.0078125;
    sprite.v0 = v0 * 0.015625;
    sprite.v1 = v1 * 0.015625;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void debugf(char *format);

void StgSelDC(void)
{
    debugf("KOKONIKITA?\n");
}

extern USER_SETTING userinfo;
extern int rannyuuflag;
void pack_allfree(void);
void sound_load_Common(void);
void BGM_Request(int no, int loop);
void set_bgmvol(void);
void Read_Playdat(int size);
void LinkAddrTb(u_int *addr);
int IST_Load(int *texadr, int sprid, int palid);
void PXF_Free(int id, int pal);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void *dmalloc(int size, char *name);
void InitCancelFlag(void);
int IsRannyuu(void);
int IsCancel(void);
int IsCancelFlag(void);
int IsFadeEnd(void);
void DispCreditMsg(void);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
unsigned int gStripGetFadeColor(unsigned int color);
void nlSprPut(NL_SPR_ARG *spr);
int sound_request(int no);
void SetFadeSpeed(int frame);
void Rq_Fadein(void);
void Rq_Fadeout(void);
void EnterFight_Main(void);

#define PFBOX(q, w_, zz, c) polydraw_FBox((int)(q).x, (int)(q).y, (int)((q).x + (w_)), (int)((q).y + (q).h), zz, c)

void StgSelMain(void)
{
    u_int *p;
    int i;
    int id;
    int bgm;
    static int stg;
    static int stgold;
    static int wait;
    static NL_SPR_ARG *sprite;
    static POLYGN *poly;
    static FUJI *film;
    static float x5;
    static float x6;
    static float y3;
    static int count;
    int offu;
    int offl;
    u_int color_w;
    u_int color_b;
    GAME_INFOMATION *gp;
    int fms;
    int x;

    gp = &g_info;
    offu = userinfo.offs_cockpit_upper;
    offl = userinfo.offs_cockpit_lower;
    if (SubJob == 0) {
        if (gp->BattleCnt[gp->Player] == 8) {
            bgm = 0x1d;
        } else if (gp->BattleCnt[gp->Player] == 9) {
            bgm = 0x1e;
        } else {
            bgm = 0x18;
        }
        pack_allfree();
        sound_load_Common();
        BGM_Request(bgm, 1);
        set_bgmvol();
        SubJob++;
    } else if (SubJob == 1) {
        i = 0;
        Read_Playdat(0x100);
        p = (u_int *)StgSelFile;
        LinkAddrTb(p);
        FadeInit();
        while (p[i]) {
            IST_Load((int *)&p[i], i, -1);
            i++;
        }
        id = gp->BattleHistory[gp->Player][gp->BattleCnt[gp->Player]];
        PXF_Free(20, 6);
        IST_Load((int *)&p[id + 19], 20, -1);
        CockpitLoadInit();
        CreditFontLoad(20);
        sprite = dmalloc(0x2d0, "StgSpr");
        poly = dmalloc(0xe0, "StgPoly");
        film = dmalloc(0x28, "StgFilm");
        for (i = 0; i < 9; i++) {
            sprite[i].zm_x = 1.0f;
            sprite[i].zm_y = 1.0f;
            sprite[i].ang = 0;
            sprite[i].u0 = 0.0f;
            sprite[i].u1 = 1.0f;
            sprite[i].v0 = 0.0f;
            sprite[i].v1 = 1.0f;
            sprite[i].listType = 1;
            sprite[i].trnsl = 0.0f;
            sprite[i].attr = 13;
        }
        for (i = 0; i < 10; i++) {
            film[i].y = (float)i * 55.0;
        }
        for (i = 0; i < 12; i++) {
            poly[i].w = 640;
            poly[i].h = 2;
            poly[i].x = 0.0f;
        }

        poly[0].y = -37.0f;
        poly[1].y = -31.0f;
        poly[2].y = -24.0f;
        poly[3].y = -14.0f;
        poly[4].y = 0.0f;

        poly[5].y = 480.0f;
        poly[6].y = 494.0f;
        poly[7].y = 504.0f;
        poly[8].y = 511.0f;
        poly[9].y = 517.0f;

        poly[10].h = 25;
        poly[10].x = -640.0f;
        poly[10].y = offu;

        poly[11].h = 25;
        poly[11].x = 640.0f;
        poly[11].y = offl + 455;

        if (StageNo <= 0) {
            StageNo = 1;
        }
        stg = StageNo - 1;
        sprite[0].sprno = StageNo + 60;
        sprite[0].x = 320 - logowidth[stg];
        sprite[0].y = 272.0f;
        sprite[0].z = 13.0f;
        sprite[0].u0 = 0.0f;
        sprite[0].u1 = 1.0f;
        sprite[0].zm_x = 1.0f;

        sprite[5].sprno = 14;
        sprite[5].x = 0.0f;
        sprite[5].y = 50.0f;
        sprite[5].z = 21.0f;
        sprite[5].attr = 13;
        sprite[5].trnsl = 1.0f;
        sprite[5].v0 = 0.0625f;
        sprite[5].v1 = 1.0f;
        sprite[5].zm_y = 1.0f;
        sprite[5].u0 = 0.0f;
        sprite[5].u1 = 1.0f;
        sprite[5].zm_x = 1.0f;
        sprite[5].listType = 0;

        sprite[6].sprno = 14;
        sprite[6].x = 0.0f;
        sprite[6].y = 50.0f;
        sprite[6].z = 21.0f;
        sprite[6].attr = 13;
        sprite[6].trnsl = 1.0f;
        sprite[6].v0 = 0.0625f;
        sprite[6].v1 = 1.0f;
        sprite[6].zm_y = 1.0f;
        sprite[6].u0 = 0.0f;
        sprite[6].u1 = 1.0f;
        sprite[6].zm_x = 1.0f;
        sprite[6].listType = 0;

        sprite[7].sprno = 15;
        sprite[7].x = 0.0f;
        sprite[7].y = 50.0f;
        sprite[7].z = 21.0f;
        sprite[7].attr = 13;
        sprite[7].trnsl = 1.0f;
        sprite[7].v0 = 0.0f;
        sprite[7].v1 = 0.9375f;
        sprite[7].zm_y = 1.0f;
        sprite[7].u0 = 0.0f;
        sprite[7].u1 = 1.0f;
        sprite[7].zm_x = 1.0f;
        sprite[7].listType = 0;

        sprite[8].sprno = 15;
        sprite[8].x = 0.0f;
        sprite[8].y = 50.0f;
        sprite[8].z = 21.0f;
        sprite[8].attr = 13;
        sprite[8].trnsl = 1.0f;
        sprite[8].v0 = 0.0f;
        sprite[8].v1 = 0.9375f;
        sprite[8].zm_y = 1.0f;
        sprite[8].u0 = 0.0f;
        sprite[8].u1 = 1.0f;
        sprite[8].zm_x = 1.0f;
        sprite[8].listType = 0;

        sprite[5].x = -640.0f;
        sprite[7].x = 640.0f;
        sprite[5].y = 240.0f;
        sprite[7].y = 260.0f;
        SubJobCount = 0;
        wait = 0;
        rannyuuflag = 0;
        InitCancelFlag();
        SubJob++;
    } else if (SubJob == 2) {
        Read_Playdat(0x80);
        rannyuuflag |= IsRannyuu();
        if (IsCancel()) {
            goto cancel;
        }
        DispCreditMsg();
        if (SubJobCount < 15) {
            sprite[5].x += 45.714285714285715;
            sprite[7].x -= 45.714285714285715;
        } else if (SubJobCount >= 15 && SubJobCount < 76) {
            sprite[5].x = 0.0f;
            sprite[7].x = 0.0f;
        } else if (SubJobCount >= 77 && SubJobCount < 91) {
            sprite[5].y -= 6.25;
            sprite[7].y += 6.25;
        } else if (SubJobCount >= 91) {
            sprite[5].y = 140.0f;
            sprite[7].y = 360.0f;
        }
        if (SubJobCount >= 21 && SubJobCount < 29) {
            for (i = 0; i < 5; i++) {
                poly[i].y += 10.875;
            }
            for (i = 0; i < 5; i++) {
                poly[i + 5].y -= 10.875;
            }
        } else if (SubJobCount >= 30) {
            poly[0].y = 50.0f;
            poly[1].y = 56.0f;
            poly[2].y = 63.0f;
            poly[3].y = 73.0f;
            poly[4].y = 87.0f;
            poly[5].y = 391.0f;
            poly[6].y = 405.0f;
            poly[7].y = 415.0f;
            poly[8].y = 422.0f;
            poly[9].y = 428.0f;
        }
        if (SubJobCount >= 45 && SubJobCount < 60) {
            sprite[0].trnsl += 0.0625;
        } else if (SubJobCount >= 61) {
            sprite[0].trnsl = 1.0f;
        }
        if (SubJobCount >= 31 && SubJobCount < 45) {
            poly[10].x += 45.714285714285715;
            poly[11].x -= 45.714285714285715;
        } else if (SubJobCount >= 46) {
            poly[10].x = 0.0f;
            poly[11].x = 0.0f;
        }
        color_w = gStripGetFadeColor(0xffffffff);
        color_b = gStripGetFadeColor(0xff000838);
        if (SubJobCount >= 20) {
            for (i = 0; i < 10; i++) {
                PFBOX(poly[i], poly[i].w, 19.0f, color_w);
            }
        }
        if (SubJobCount >= 31) {
            PFBOX(poly[10], poly[10].w, 19.0f, color_b);
            PFBOX(poly[11], poly[11].w, 19.0f, color_b);
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        if (SubJobCount >= 21) {
            DrawBG(stg + 1, 0, 140, 27.0f);
        }
        if (SubJobCount >= 45) {
            gSpriteDraw_NaomiLibVersion(&sprite[0]);
        }
        polydraw_FBox(0.0f, 87.0f, 640.0f, sprite[5].y - 20.0f, 25.0f, 0xff000000);
        polydraw_FBox(0.0f, sprite[7].y - 10.0f, 640.0f, 391.0f, 25.0f, 0xff000000);
        SubJobCount++;
        if (SubJobCount >= 96) {
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 3) {
        if (SubJobCount == 0) {
            sprite[1].x = 320.0f;
            sprite[1].y = 240.0f;
            sprite[2].x = 320.0f;
            sprite[2].y = 240.0f;
            sprite[3].sprno = 59;
            sprite[3].x = 0.0f;
            sprite[3].y = 50.0f;
            sprite[3].z = 23.0f;
            sprite[3].trnsl = 1.0f;
            sprite[1].sprno = 18;
            sprite[1].z = 15.0f;
            sprite[2].sprno = 19;
            sprite[2].z = 17.0f;
            sprite[1].attr = 0x20a;
            sprite[2].attr = 0x20a;
            sprite[3].attr = 5;
            sprite[3].v0 = (sprite[5].y - 50.0f) / 191.0;
            sprite[3].zm_y = (sprite[5].y - 50.0f) / 191.0;
            sprite[4].sprno = 59;
            sprite[4].z = 23.0f;
            sprite[4].trnsl = 1.0f;
            sprite[3].listType = 0;
            sprite[4].x = 0.0f;
            sprite[4].y = 428.0f;
            sprite[4].attr = 13;
            sprite[4].v0 = (428.0f - sprite[7].y) / 191.0;
            sprite[4].zm_y = (428.0f - sprite[7].y) / 191.0;
            poly[12].x = 0.0f;
            poly[12].y = 240.0f;
            poly[13].y = 240.0f;
            poly[13].x = 320.0f;
            y3 = 550.0f;
            x5 = 20.0f;
            x6 = 507.0f;
            sprite[6].x = 640.0f;
            poly[12].w = 320;
            poly[12].h = 1;
            poly[13].h = 1;
            poly[13].w = 320;
            sprite[6].y = sprite[5].y;
            sprite[8].y = sprite[7].y;
            sprite[4].listType = 0;
            sprite[8].x = -640.0f;
            count = 0;
        }
        Read_Playdat(0x80);
        fms = SubJobCount + 96;
        rannyuuflag |= IsRannyuu();
        if (IsCancel()) {
            goto cancel;
        }
        DispCreditMsg();
        sprite[5].x -= 2.909090909090909;
        sprite[6].x -= 2.909090909090909;
        sprite[7].x += 2.909090909090909;
        sprite[8].x += 2.909090909090909;
        if (sprite[5].x <= -640.0f) {
            sprite[5].x = 636.0f;
        }
        if (sprite[6].x <= -640.0f) {
            sprite[6].x = 636.0f;
        }
        if (sprite[7].x >= 640.0f) {
            sprite[7].x = -636.0f;
        }
        if (sprite[8].x >= 640.0f) {
            sprite[8].x = -636.0f;
        }
        sprite[6].y = sprite[5].y;
        sprite[8].y = sprite[7].y;
        sprite[1].trnsl += 0.03125;
        sprite[2].trnsl += 0.01875;
        sprite[1].ang += 60;
        sprite[2].ang += 60;
        if (fms & 1) {
            sprite[3].attr = sndtb[count & 3] | 5;
            sprite[4].attr = sndtb[count & 3] | 13;
            count++;
        }
        y3 -= 17.1875;
        if (fms >= 106) {
            poly[12].x -= 20.0;
            poly[13].x += 20.0;
        }
        nlSprPut(&sprite[1]);
        nlSprPut(&sprite[2]);
        color_w = gStripGetFadeColor(0xffffffff);
        color_b = gStripGetFadeColor(0xff000838);
        for (i = 0; i < 10; i++) {
            PFBOX(poly[i], poly[i].w, 19.0f, color_w);
        }
        PFBOX(poly[10], poly[10].w, 19.0f, color_b);
        PFBOX(poly[11], poly[11].w, 19.0f, color_b);
        if (fms >= 105) {
            PFBOX(poly[12], poly[12].w, 13.0f, gStripGetFadeColor(0xffc02030));
            PFBOX(poly[13], poly[11].w, 19.0f, gStripGetFadeColor(0xffc02030));
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[6]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        gSpriteDraw_NaomiLibVersion(&sprite[8]);
        DrawBG(stg + 1, 0, 140, 27.0f);
        gSpriteDraw_NaomiLibVersion(&sprite[3]);
        gSpriteDraw_NaomiLibVersion(&sprite[4]);
        gSpriteDraw_NaomiLibVersion(&sprite[0]);
        for (i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {
                id = 56;
            } else {
                id = 57;
            }
            EasySpriteDrawEX(id, x5, (int)film[i].y - y3, 11.0f, 13, 0.6f);
            EasySpriteDrawEX(id, x6, (int)film[i].y - y3, 11.0f, 13, 0.6f);
            if (i == 4 || i == 9) {
                id = 56;
            } else {
                id = 57;
            }
            EasySpriteDrawEX(id, (int)x5 + 80, (int)film[i].y - y3, 11.0f, 29, 0.6f);
            EasySpriteDrawEX(id, (int)x6 + 80, (int)film[i].y - y3, 11.0f, 29, 0.6f);
        }
        polydraw_FBox(x5 + 20.0f, 0.0f, x5 + 92.0f, film[9].y - y3, 11.0f, gStripGetFadeColor(0x99000000));
        polydraw_FBox(x6 + 20.0f, 0.0f, x6 + 92.0f, film[9].y - y3, 11.0f, gStripGetFadeColor(0x99000000));
        DrawIcon(y3);
        SubJobCount++;
        if (SubJobCount >= 32) {
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 4) {
        if (SubJobCount == 0) {
            sprite[0].trnsl = 1.0f;
            sprite[1].trnsl = 0.6f;
            sound_request(0x54);
        }
        Read_Playdat(0x80);
        if (SubJobCount == 140) {
            sound_request(0x55);
        }
        if (IsCancel()) {
            goto cancel;
        }
        DispCreditMsg();
        sprite[5].x -= 2.909090909090909;
        sprite[6].x -= 2.909090909090909;
        sprite[7].x += 2.909090909090909;
        sprite[8].x += 2.909090909090909;
        sprite[6].y = sprite[5].y;
        sprite[8].y = sprite[7].y;
        if (sprite[5].x <= -640.0f) {
            sprite[5].x = 636.0f;
        }
        if (sprite[6].x <= -640.0f) {
            sprite[6].x = 636.0f;
        }
        if (sprite[7].x >= 640.0f) {
            sprite[7].x = -636.0f;
        }
        if (sprite[8].x >= 640.0f) {
            sprite[8].x = -636.0f;
        }
        sprite[1].ang += 60;
        sprite[2].ang += 60;
        if (SubJobCount & 1) {
            sprite[3].v0 = (191.0 - (sprite[5].y - 50.0) + 65.0 + (float)(SubJobCount & 3)) / 256.0;
            sprite[3].zm_y = (sprite[5].y - 50.0) / 256.0;
            sprite[4].v0 = (191.0 - (428.0 - sprite[7].y) + 65.0 + (float)(SubJobCount & 3)) / 256.0;
            sprite[4].zm_y = (428.0 - sprite[7].y) / 191.0;
            sprite[4].attr = sndtb[count & 3] | 13;
            sprite[3].attr = sndtb[count & 3] | 5;
            count++;
        }
        for (i = 0; i < 10; i++) {
            film[i].y += 6.875;
            if (film[i].y >= 550.0f) {
                film[i].y = 0.0f;
            }
        }
        if (stg != stgold) {
            stgold = stg;
        }
        if (SubJobCount >= 300) {
            SubJobCount = 0;
            SubJob++;
            return;
        }
        rannyuuflag |= IsRannyuu();
        nlSprPut(&sprite[1]);
        nlSprPut(&sprite[2]);
        color_w = gStripGetFadeColor(0xffffffff);
        color_b = gStripGetFadeColor(0xff000838);
        for (i = 0; i < 10; i++) {
            PFBOX(poly[i], poly[i].w, 19.0f, color_w);
        }
        PFBOX(poly[10], poly[10].w, 19.0f, color_b);
        PFBOX(poly[11], poly[11].w, 19.0f, color_b);
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[6]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        gSpriteDraw_NaomiLibVersion(&sprite[8]);
        DrawBG(stg + 1, 0, 140, 27.0f);
        gSpriteDraw_NaomiLibVersion(&sprite[3]);
        gSpriteDraw_NaomiLibVersion(&sprite[4]);
        gSpriteDraw_NaomiLibVersion(&sprite[0]);
        for (i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {
                id = 56;
            } else {
                id = 57;
            }
            EasySpriteDrawEX(id, x5, film[i].y, 11.0f, 13, 0.6f);
            EasySpriteDrawEX(id, x6, film[i].y, 11.0f, 13, 0.6f);
            if (i == 4 || i == 9) {
                id = 56;
            } else {
                id = 57;
            }
            EasySpriteDrawEX(id, (int)x5 + 80, film[i].y, 11.0f, 29, 0.6f);
            EasySpriteDrawEX(id, (int)x6 + 80, film[i].y, 11.0f, 29, 0.6f);
        }
        polydraw_FBox(x5 + 20.0f, 0.0f, x5 + 92.0f, 480.0f, 11.0f, gStripGetFadeColor(0x99000000));
        polydraw_FBox(x6 + 20.0f, 0.0f, x6 + 92.0f, 480.0f, 11.0f, gStripGetFadeColor(0x99000000));
        DrawIcon(0);
        if (SubJobCount & 1) {
            x = ((g_info.BattleCnt[g_info.Player] < 5) ? x5 : x6) + 20.0f;
            EasySpriteDrawEX(17, x, (g_info.BattleCnt[g_info.Player] % 5) * 62 + 86, 5.0f, 13, 0.8f);
        }
        strloop(SubJobCount);
        SubJobCount++;
    } else if (SubJob == 5) {
        if (SubJobCount == 0) {
            sprite[5].x = 0.0f;
            stgold = stg;
            sprite[7].x = 0.0f;
            SubJobCount = 276;
        }
        Read_Playdat(0x80);
        rannyuuflag |= IsRannyuu();
        if (IsCancel()) {
            goto cancel;
        }
        if (SubJobCount & 1) {
            sprite[4].attr = sndtb[count & 3] | 13;
            sprite[3].attr = sndtb[count & 3] | 5;
            count++;
        }
        sprite[3].v0 = (191.0 - (sprite[5].y - 50.0)) / 191.0;
        sprite[3].zm_y = (sprite[5].y - 50.0) / 191.0;
        sprite[4].v0 = (191.0 - (428.0 - sprite[7].y)) / 191.0;
        sprite[4].zm_y = (428.0 - (sprite[7].y - 10.0)) / 191.0;
        if (SubJobCount < 280) {
            x5 -= 28.0;
            x6 += 28.0;
        } else if (SubJobCount >= 281) {
            x5 = -120.0f;
            x6 = 641.0f;
        }
        if (SubJobCount < 288) {
            sprite[5].y += 8.333333333333334;
            sprite[7].y -= 8.333333333333334;
        } else if (SubJobCount >= 289 && SubJobCount < 296) {
            sprite[5].y = 240.0f;
            sprite[7].y = 260.0f;
        } else if (SubJobCount >= 297) {
            sprite[5].y -= 40.0;
            sprite[7].y += 40.0;
        }
        nlSprPut(&sprite[1]);
        nlSprPut(&sprite[2]);
        color_w = gStripGetFadeColor(0xffffffff);
        color_b = gStripGetFadeColor(0xff000838);
        if (SubJobCount >= 20) {
            for (i = 0; i < 10; i++) {
                PFBOX(poly[i], poly[i].w, 19.0f, color_w);
            }
        }
        if (SubJobCount >= 31) {
            PFBOX(poly[10], poly[10].w, 19.0f, color_b);
            PFBOX(poly[11], poly[11].w, 19.0f, color_b);
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        DrawBG(stg + 1, 0, 140, 27.0f);
        gSpriteDraw_NaomiLibVersion(&sprite[3]);
        gSpriteDraw_NaomiLibVersion(&sprite[4]);
        for (i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {
                id = 56;
            } else {
                id = 57;
            }
            EasySpriteDrawEX(id, x5, film[i].y, 11.0f, 13, 0.6f);
            EasySpriteDrawEX(id, x6, film[i].y, 11.0f, 13, 0.6f);
            if (i == 4 || i == 9) {
                id = 56;
            } else {
                id = 57;
            }
            EasySpriteDrawEX(id, (int)x5 + 80, film[i].y, 11.0f, 29, 0.6f);
            EasySpriteDrawEX(id, (int)x6 + 80, film[i].y, 11.0f, 29, 0.6f);
        }
        polydraw_FBox(x5 + 20.0f, 0.0f, x5 + 92.0f, 480.0f, 11.0f, gStripGetFadeColor(0x99000000));
        polydraw_FBox(x6 + 20.0f, 0.0f, x6 + 92.0f, 480.0f, 11.0f, gStripGetFadeColor(0x99000000));
        if (SubJobCount >= 297) {
            polydraw_FBox(0.0f, sprite[5].y, 640.0f, sprite[7].y - 20.0f, 3.0f, 0xff000000);
        }
        SubJobCount++;
        if (rannyuuflag && SubJobCount >= 308) {
            SubJobCount = 307;
        }
        if (SubJobCount >= 308) {
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 6) {
        if (SubJobCount == 0) {
            if (IsCancelFlag()) {
                goto cancel;
            }
            SetFadeSpeed(20);
            Rq_Fadein();
            sound_request(0x57);
            SubJobCount++;
        }
        if (!IsFadeEnd()) {
            Read_Playdat(0x80);
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            DispCreditMsg();
            return;
        }
        if (SubJobCount < 21) {
            Read_Playdat(0x80);
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            DispCreditMsg();
            SubJobCount++;
        } else {
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJobCount = 0;
            SubJob++;
        }
    } else {
        if (!IsFadeEnd()) {
            Read_Playdat(0x80);
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            DispCreditMsg();
            return;
        }
    cancel:
        StgSelFree();
        dfree(sprite);
        dfree(poly);
        dfree(film);
        sprite = 0;
        poly = 0;
        film = 0;
        return;
    }
    if (rannyuuflag) {
        EnterFight_Main();
    }
}

void strloop(int fms)
{
    int i;
    static int nxfms;
    static int limit;
    static int mode;
    static int count;
    int offu;
    int offl;
    static float trnsl;
    float z;
    char chartb[26] = "WHEELOFFORTUNEREVOLVINGNOW";
    char flgtb[26] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    float xttb[26] = {
        146.0f, 158.0f, 170.0f, 182.0f, 194.0f, 218.0f, 230.0f, 254.0f, 266.0f, 278.0f, 290.0f, 302.0f, 314.0f,
        326.0f, 350.0f, 362.0f, 374.0f, 386.0f, 398.0f, 410.0f, 422.0f, 434.0f, 446.0f, 470.0f, 482.0f, 494.0f,
    };
    short fmstb[26] = { 5, 7, 9, 11, 13, 8, 10, 18, 20, 22, 24, 26, 28, 30, 16, 14, 12, 10, 8, 6, 4, 2, 0, 25, 23, 21 };

    if (fms == 0) {
        for (i = 0; i < 26; i++) {
            chwork[i].c = chartb[i];
            chwork[i].fms = fmstb[i] * 2;
            chwork[i].flg = flgtb[i];
            if (flgtb[i]) {
                chwork[i].speed = (640.0f - xttb[i]) / 40.0;
            } else {
                chwork[i].speed = xttb[i] / 40.0;
            }
            if (flgtb[i]) {
                chwork[i].x = 640.0f;
            } else {
                chwork[i].x = -16.0f;
            }
            chwork[i].mode = 0;
        }
        trnsl = 1.0f;
        mode = 0;
        count = 0;
    }
    for (i = 0; i < 26; i++) {
        switch (mode) {
        case 0:
            trnsl = 1.0f;
            if (fms >= chwork[i].fms) {
                if (chwork[i].flg) {
                    chwork[i].x -= chwork[i].speed;
                } else {
                    chwork[i].x += chwork[i].speed;
                }
            }
            if (fms >= chwork[i].fms + 40) {
                chwork[i].x = xttb[i];
                chwork[i].mode = 1;
            }
            break;
        case 1:
            if (fms >= 121 && fms < 140) {
                trnsl -= 1.0f / 520.0f;
            } else if (fms >= 121 && fms < 152) {
                trnsl += 1.0f / 312.0f;
            } else if (fms >= 153) {
                trnsl = 1.0f;
                chwork[i].mode = 2;
            }
            break;
        case 2:
            if (312.0f < xttb[i]) {
                if (chwork[i].mode != 3) {
                    chwork[i].x -= (float)((xttb[i] - 312.0) / 20.0);
                    if (chwork[i].x <= 312.0) {
                        chwork[i].x = 312.0f;
                        chwork[i].mode = 3;
                    }
                }
            } else {
                if (chwork[i].mode != 3) {
                    chwork[i].x += (float)((312.0 - xttb[i]) / 20.0);
                    if (chwork[i].x >= 312.0) {
                        chwork[i].x = 312.0f;
                        chwork[i].mode = 3;
                    }
                }
            }
            break;
        case 3:
            if (fms >= 189) {
                chwork[i].mode = 4;
            }
            chwork[i].x = 312.0f;
            nxfms = fms + 1;
            limit = 0;
            break;
        case 4:
            if (chwork[i].flg) {
                if (i >= 25 - limit) {
                    chwork[i].x += 16.0f;
                    if (640.0f < chwork[i].x) {
                        chwork[i].x = 640.0f;
                        chwork[i].mode = 5;
                    }
                }
            } else {
                if (i <= limit) {
                    chwork[i].x -= 16.0f;
                    if (chwork[i].x < -16.0f) {
                        chwork[i].x = -16.0f;
                        chwork[i].mode = 5;
                    }
                }
            }
            if (nxfms + 4 < fms) {
                nxfms = fms + 1;
                limit++;
                if (limit > 26) {
                    limit = 26;
                }
            }
            break;
        case 5:
            for (i = 0; i < 26; i++) {
                chwork[i].c = chartb[i];
                chwork[i].fms = fmstb[i] * 2;
                chwork[i].flg = flgtb[i];
                if (flgtb[i]) {
                    chwork[i].speed = (640.0f - xttb[i]) / 40.0;
                } else {
                    chwork[i].speed = xttb[i] / 40.0;
                }
                if (flgtb[i]) {
                    chwork[i].x = 640.0f;
                } else {
                    chwork[i].x = -16.0f;
                }
                chwork[i].mode = 0;
            }
            trnsl = 1.0f;
            mode = 0;
            count = 0;
            chwork[i].mode = 0;
            break;
        }
        count += chwork[i].mode;
    }
    if (count == 0) {
        mode = 0;
    } else if (count == 26) {
        mode = 1;
    } else if (count == 52) {
        mode = 2;
    } else if (count == 78) {
        mode = 3;
    } else if (count == 104) {
        mode = 4;
    } else if (count == 130) {
        mode = 5;
    }
    count = 0;
    fms = 0;
    z = 17.1f;
    offu = userinfo.offs_cockpit_upper;
    offl = userinfo.offs_cockpit_lower;
    for (i = 0; i < 26; i++) {
        DispChar_st(chwork[i].x, offu + 20, z, trnsl, chwork[i].c);
        DispChar_st(chwork[i].x, offl + 476, z, trnsl, chwork[i].c);
        z += 0.01;
    }
}
