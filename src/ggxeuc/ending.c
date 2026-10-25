#include "generated/ending.h"
#include "generated/ending.h"

void GetEdFont_UV(char *str, int *ui, int *vi)
{
    int u;
    int v;

    if (*str >= '0' && *str <= '9') {
        u = *str - '0';
        v = 0;
        while (u >= 10) {
            u -= 10;
            v++;
        }
    } else if (*str >= 'A' && *str <= 'Z') {
        u = *str - 'A';
        v = 1;
        while (u >= 10) {
            u -= 10;
            v++;
        }
    } else if (*str >= 'a' && *str <= 'z') {
        u = *str - 'a';
        v = 5;
        while (u >= 10) {
            u -= 10;
            v++;
        }
    } else if (*str == '-') {
        u = 0;
        v = 4;
    } else if (*str == '+') {
        u = 1;
        v = 4;
    } else if (*str == '.') {
        u = 2;
        v = 4;
    } else if (*str == '!') {
        u = 3;
        v = 4;
    } else if (*str == '?') {
        u = 4;
        v = 4;
    } else if (*str == '/') {
        u = 5;
        v = 4;
    } else if (*str == '(') {
        u = 6;
        v = 4;
    } else if (*str == ')') {
        u = 7;
        v = 4;
    } else if (*str == '@') {
        u = 8;
        v = 4;
    } else if (*str == '"') {
        u = 9;
        v = 4;
    } else if (*str == '&') {
        u = 0;
        v = 8;
    } else if (*str == '\'') {
        u = 1;
        v = 8;
    } else if (*str == ',') {
        u = 2;
        v = 8;
    } else {
        u = -1;
        v = -1;
    }
    *ui = u;
    *vi = v;
}

int SetRightPosPr(char *str, int x, int size)
{
    int width;
    int u;
    int v;
    int chars;
    FONT_UV *uv;

    chars = 0;
    uv = 0;
    while (*str) {
        width = 14;
        GetEdFont_UV(str, &u, &v);
        if (u != -1 && v != -1 && u < 10 && v < 9) {
            uv = &fontuv[v][u];
            width = 0;
        }
        if (width == 0) {
            width = uv->u1 - uv->u0 + 2.0f;
        } else {
            width = 14;
        }
        str++;
        chars += width;
    }
    return x - chars;
}

void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);

#ifdef VERSION_JP
#define ENDCREDIT_LINES 689
#define ENDCREDIT_SPEED 1.4236133098602295f
#else
#define ENDCREDIT_LINES 788
#define ENDCREDIT_SPEED 1.6426308155059814f
#endif

int EndingCreditExe(ENDCREDIT_WORK *src, float *scr_y, int pause)
{
    int x;
    int y;
    int i;
    int count;

    count = 0;
    y = 0;
    for (i = 0; i < ENDCREDIT_LINES; i++, src++) {
        if (src->str) {
            y = i * 23 + *scr_y;
            if (y >= -24 && y <= 480) {
switch (src->flag) { case 1: default: x = 80; break; case 3: x = SetRightPosPr(src->str, 0x230, 16); break; case 2: x = SetRightPosPr(src->str, 0, 16) / 2 + 320; break; }
                count = i;
                EndFontDraw(src->str, x, y, 4.0f, 1.0f);
                EndFontDraw(src->str, x + 6, y + 6, 5.0f, 0.7f);
            }
        }
    }
    if (!pause) {
        *scr_y -= ENDCREDIT_SPEED;
    }
    polydraw_GBox(0.0f, 0.0f, 640.0f, 64.0f, 3.0f, 0xFF000000, 0xFF000000, 0, 0);
    polydraw_GBox(0.0f, 416.0f, 640.0f, 480.0f, 3.0f, 0, 0, 0xFF000000, 0xFF000000);
    if (count == ENDCREDIT_LINES - 1) {
        Botom_y = y + 24.0;
    } else {
        Botom_y = 480.0f;
    }
    return count;
}

void gCurrentColorClear(void);
void gCurrentColorSetARBG(unsigned int arbg);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void DrawSammy(NL_SPR_ARG *p)
{
    p->x = 320.0f;
    p->trnsl = 1.0f;
    p->z = 4.0f;
    gSpriteDraw_NaomiLibVersion(p);
    p->trnsl = 0.7f;
    p->x += 6.0f;
    p->y += 6.0f;
    p->z += 1.0;
    gCurrentColorSetARBG(0xFF777777);
    gSpriteDraw_NaomiLibVersion(p);
    gCurrentColorClear();
}

int FileReadCheck(void);
char *FileReadSelect(int fid, int chan);
void JobNext(void);
extern char *OpeningFile;
extern int SubJob;
extern int SubJobCount;

void EndingCreditInit(void)
{
    if (!OpeningFile) {
        OpeningFile = FileReadSelect(0xA1, 0);
    }
    if (!EndingFile) {
        EndingFile = FileReadSelect(0xB9, 1);
    }
    if (!EdFontFile) {
        EdFontFile = FileReadSelect(0xBA, 2);
    }
    if (FileReadCheck()) {
        SubJob = 0;
        SubJobCount = 0;
        JobNext();
    }
}

void BGM_Request(int no, int loop);
int ChkCancel_Start(void);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void DispWinnerCredit(void);
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
void FadeInit(void);
int IST_Load(int *texadr, int sprid, int palid);
int IsFadeEnd(void);
void JobChange(int job);
void LinkAddrTb(u_int *addr);
void PXF_Free(int id, int pal);
int ReqSongMute(int outtime, int intime, int mutetime, int mval);
void Rq_Fadein(void);
void Rq_Fadeout(void);
void dfree(void *mem);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern char *OpeningFile;
extern int SubJob;
extern int SubJobCount;
extern int final;

#ifdef VERSION_JP
#define ENDCREDIT_LAST 688
#define ENDCREDIT_POSY 15824.0f
#else
#define ENDCREDIT_LAST 787
#define ENDCREDIT_POSY 18101.0f
#endif

void EndingCreditMain(void)
{
    static float scr_y;
    static float trnsl;
    int i;
    static int cancel;
    static int pause;
    static int count;
    static int *eddata;
    static int *bgdata;
    static int posy;
    static NL_SPR_ARG sammy;
    int bgmtb[19] = { 0, 31, 31, 31, 32, 31, 32, 31, 33, 32, 32, 33, 31, 31, 32, 33, 32, 34, 34 };
    unsigned int *p;

    if (SubJob == 0) {
        i = 0;
        trnsl = 1.0f;
        scr_y = 480.0f;
        cancel = 0;
        eddata = (int *)EndingFile;
        LinkAddrTb((u_int *)eddata);
        bgdata = (int *)OpeningFile;
        LinkAddrTb((u_int *)bgdata);
        sammy.y = 0.0f;
        sammy.sprno = 0x30;
        sammy.x = 320.0f;
        sammy.z = 4.0f;
        sammy.zm_x = 0.5625f;
        sammy.zm_y = 0.1875f;
        sammy.u0 = 0.3203125f;
        sammy.u1 = 0.8828125f;
        sammy.v0 = 0.8125f;
        sammy.listType = 1;
        sammy.attr = 0x206;
        sammy.v1 = 1.0f;
        sammy.ang = 0;
        sammy.trnsl = 1.0f;
        LinkAddrTb((u_int *)EdFontFile);
        p = (unsigned int *)EdFontFile;
        while (p[i]) {
            IST_Load((int *)&p[i], i + 0x30, -1);
            i++;
        }
        PXF_Free(0x31, 0x31);
        IST_Load(&bgdata[139], 0x31, -1);
        if (g_info.Winnerid < 17) {
            userinfo.gcheck[g_info.Winnerid + 0x51] = 1;
            IST_Load(&eddata[g_info.Winnerid - 1], 0, -1);
        }
        CockpitLoadInit();
        CreditFontLoad(0x28);
        g_info.BattleControlFlag |= 4;
        BGM_Request(bgmtb[g_info.Winnerid], 0);
        FadeInit();
        Rq_Fadein();
        pause = 0;
        SubJob++;
    }
    if (SubJob == 1) {
        if (cancel && IsFadeEnd()) {
            SubJob += 2;
            return;
        }
        count = EndingCreditExe(endcredit_data, &scr_y, pause);
        posy = scr_y + ENDCREDIT_POSY;
        if (count == ENDCREDIT_LAST && posy <= 192) {
            SubJob++;
            sammy.y = Botom_y;
            DrawSammy(&sammy);
            return;
        }
        DispWinnerCredit();
        if (count == ENDCREDIT_LAST) {
            trnsl -= 0.05;
            if (trnsl < 0.0f) {
                trnsl = 0.0f;
            }
        }
        if (g_info.Winnerid < 17) {
            EasySpriteDrawEX(0, 10, 320, 20.0f, 0x20D, trnsl);
        }
        EasySpriteDrawEX(0x31, 0, 480, 23.0f, 0x20D, trnsl);
        if (g_info.Winnerid < 17) {
            polydraw_GBox(30.0f, 100.0f, 350.0f, 340.0f, 22.0f, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000);
            polydraw_GBox(10.0f, 80.0f, 330.0f, 320.0f, 21.0f, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000);
        }
        sammy.y = Botom_y;
        DrawSammy(&sammy);
        if (ChkCancel_Start() && !cancel) {
            cancel = 1;
            Rq_Fadeout();
            ReqSongMute(1, 10, 240, 5);
        }
    } else if (SubJob == 2) {
        if (SubJobCount > 240 && IsFadeEnd()) {
            SubJob++;
            return;
        }
        if (ChkCancel_Start() && !cancel) {
            cancel = 1;
            Rq_Fadeout();
            ReqSongMute(1, 10, 240, 5);
        }
        if (cancel && IsFadeEnd()) {
            SubJob++;
            return;
        }
        EndingCreditExe(endcredit_data, &scr_y, 1);
        DispWinnerCredit();
        if (SubJobCount == 240) {
            ReqSongMute(1, 10, 240, 5);
            Rq_Fadeout();
        }
        sammy.y = Botom_y;
        DrawSammy(&sammy);
        SubJobCount++;
    } else if (SubJob == 3) {
        final = 2;
        g_info.BattleControlFlag &= ~4;
        dfree(EndingFile);
        EndingFile = 0;
        dfree(EdFontFile);
        EdFontFile = 0;
        JobChange(0x28);
    }
}

void EndFontDraw(char *str, int x, int y, float z, float trnsl)
{
    int u;
    int v;
    NL_SPR_ARG sprite;
    FONT_UV *uv;
    float width;

    uv = 0;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.trnsl = trnsl;
    sprite.ang = 0;
    while (*str) {
        width = 14.0f;
        GetEdFont_UV(str, &u, &v);
        if (u != -1 && v != -1 && u < 10 && v < 9) {
            width = 0.0f;
            sprite.sprno = 0x30;
            uv = &fontuv[v][u];
            sprite.zm_y = 0.09375f;
            sprite.zm_x = (uv->u1 - uv->u0) * (1.0 / 256.0);
            v = v * 24.0;
            sprite.u0 = uv->u0 * (1.0 / 256.0);
            sprite.v0 = v * (1.0 / 256.0);
            sprite.u1 = uv->u1 * (1.0 / 256.0);
            sprite.v1 = (v + 24.0) * (1.0 / 256.0);
            if (trnsl == 1.0) {
                gCurrentColorSetARBG(0xFFFFFFFF);
            } else {
                gCurrentColorSetARBG(0xFF777777);
            }
            gSpriteDraw_NaomiLibVersion(&sprite);
            gCurrentColorClear();
        }
        str++;
        if (width == 0.0f) {
            width = uv->u1 - uv->u0 + 2.0f;
        } else {
            width = 14.0f;
        }
        sprite.x += width;
    }
}
