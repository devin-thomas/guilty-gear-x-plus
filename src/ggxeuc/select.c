#include "generated/select.h"

int chk_select_color(int id, int color);
extern short int UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern SYS_COND sc;
char *FileReadSelect(int fid, int chan);
int GetLimitStatus(void);
void JobChange(int job);
void JobNext(void);
void heap_clear(void);
int sound_request(int no);
extern int SubJob;
extern int SubJobCount;
extern USER_SETTING userinfo;
int FileReadCheck(void);
extern PAD pads[2];
void dfree(void *mem);
unsigned int gStripGetFadeColor(unsigned int color);
void nlSetBackColor(int c1, int c2, int c3);
int Cr_IsStartWait(int side);
int Cr_Request_Start(int side);
void SetCPUCharacterFlag(int playerside, int flag);
void chk_select_reset(int side);
extern TRAINING_SETTING *tr_info;
int IST_Load(int *texadr, int sprid, int palid);
void PXF_Free(int id, int pal);
int get_player(void);
void BGM_Request(int no, int loop);
int Cr_IsPlaying(int side);
void FadeInit(void);
short int GetSpriteIdTop(short unsigned int max);
void LinkAddrTb(u_int *addr);
void PXF_SpriteClear(void);
void set_bgmvol(void);
extern int TransSprtID;
extern int StageNo;
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
void EasySpriteDrawNum(int id, int x, int y, int w, float z, int n);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
extern SURVIVAL_INFOMATION *sv_info;
extern story_t story;
void SetPaltbNo(unsigned int id, unsigned int palid);
void EasySpriteDrawTR(int id, int x, int y, float z, float width);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void EasySpriteDrawBustUp(int id, int x, int y, int u0, int u1, int v0, int v1, float z);
void polydraw_Frm(float x1, float y1, float x2, float y2, float w, float z, unsigned int c1, unsigned int c2);
int Random(void);
void FontDraw(char *str, int x, int y, float z, int alfa);
char *G_sprintf(char *buff, char *str, int *valtb);
void dcfont_init(void);
void debugf(char *format, ...);
void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu);
void Read_Playdat(int size);
void Change_ScreenScale(int i);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
int InitSurvivalBattle(void);
int Enter_Stage_Bg(void);
int VsCpuCtrl(void);
extern VS_REPLAY rp;
void pack_allfree(void);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void DispCreditMsg(void);
void sound_alloff(void);
void BGM_Free(void);
void polydraw_FLine(float x1, float y1, float x2, float y2, float z, unsigned int color1);

static const short int idtb[] = { 1, 2, 3 };
static SELPOS selpos[22] = {
    { 283, 4, 0, 10.2f }, { 282, 378, 0, 10.4f }, { 335, 288, 1, 10.4f }, { 289, 87, 0, 10.4f },
    { 229, 176, 0, 10.6f }, { 230, 21, 0, 10.6f }, { 348, 112, 0, 10.2f }, { 270, 247, 0, 10.2f },
    { 335, 35, 1, 10.6f }, { 333, 345, 0, 10.6f }, { 376, 377, 0, 10.8f }, { 203, 272, 0, 10.4f },
    { 217, 96, 0, 10.2f }, { 221, 345, 1, 10.6f }, { 341, 209, 0, 10.6f }, { 171, 400, 1, 10.8f },
    { 400, 330, 1, 10.8f }, { 132, 360, 1, 10.8f }, { 283, 181, 0, 10.8f }, { 146, 191, 0, 10.8f },
    { 420, 191, 0, 10.8f }, { 0, 0, 0, 0.0f },
};
LINEPOINT lp0[6] = {
    { 149, 129, 151, 480 }, { 194, 142, 198, 480 }, { 324, 217, 326, 480 },
    { 415, 140, 416, 480 }, { 462, 192, 463, 480 }, { 502, 345, 504, 480 },
};
LINEPOINT lp1[7] = {
    { 177, 0, 179, 318 }, { 212, 0, 213, 429 }, { 259, 0, 261, 439 }, { 431, 0, 435, 431 },
    { 447, 0, 449, 97 }, { 456, 0, 457, 431 }, { 521, 0, 522, 169 },
};
LINEPOINT lp2[9] = {
    { 348, 28, 514, 29 }, { 143, 101, 365, 103 }, { 323, 169, 546, 171 },
    { 197, 219, 432, 222 }, { 178, 316, 405, 318 }, { 237, 324, 465, 328 },
    { 70, 427, 431, 428 }, { 195, 428, 431, 431 }, { 413, 468, 578, 469 },
};
static SELECTINFO SelInfo[2] = { { 0, 0, 0, -1 }, { 0, 0, 1, -1 } };
char bectl_J[80] = {
    6, 2, 9, 4, 14, 8, 10, 1, 8, 15, 17, 10, 13, 1, 7, 19, 15, 13, 19, 12, 9, 16, 1, 13, 4, 9,
    13, 15, 12, 19, 3, 2, 1, 11, 6, 7, 2, 3, 17, 11, 10, 10, 17, 9, 18, 5, 8, 14, 7, 6, 4, 5,
    18, 12, 2, 16, 19, 7, 5, 3, 18, 14, 14, 6, 10, 3, 18, 11, 17, 12, 14, 16, 5, 4, 15, 8, 0, 0,
    0, 0,
};
char bectl_K[80] = {
    6, 2, 9, 4, 14, 8, 10, 1, 8, 15, 17, 10, 13, 1, 7, 19, 15, 13, 19, 12, 9, 16, 1, 13, 4, 9,
    13, 15, 12, 19, 3, 2, 1, 11, 6, 7, 2, 3, 17, 11, 10, 10, 17, 9, 17, 5, 8, 14, 7, 6, 4, 5,
    17, 12, 2, 16, 19, 7, 5, 3, 17, 14, 14, 6, 3, 10, 14, 11, 17, 12, 14, 16, 5, 4, 15, 8, 0, 0,
    0, 0,
};
char bectl[80] = {
    6, 2, 9, 4, 14, 8, 10, 1, 8, 15, 12, 10, 13, 1, 7, 19, 15, 13, 19, 12, 9, 16, 1, 13, 4, 9,
    13, 15, 12, 19, 3, 2, 1, 11, 6, 7, 2, 3, 11, 11, 10, 10, 16, 9, 3, 5, 8, 14, 7, 6, 4, 5, 16,
    12, 2, 16, 19, 7, 5, 3, 11, 14, 14, 6, 3, 10, 18, 11, 17, 12, 14, 16, 5, 4, 15, 8, 0, 0, 0,
    0,
};
char bectl_T[80] = {
    6, 2, 9, 4, 14, 8, 10, 1, 8, 15, 12, 10, 13, 1, 7, 19, 15, 13, 19, 12, 9, 14, 1, 13, 4, 9,
    13, 15, 12, 19, 3, 2, 1, 11, 6, 7, 2, 3, 11, 11, 10, 10, 14, 9, 3, 5, 8, 14, 7, 6, 4, 5, 11,
    12, 2, 6, 19, 7, 5, 3, 1, 1, 1, 1, 3, 10, 18, 11, 17, 12, 14, 16, 5, 4, 15, 8, 1, 1, 1, 1,
};
char bectl_L[80] = {
    6, 2, 9, 4, 14, 8, 10, 1, 8, 15, 12, 10, 13, 1, 7, 19, 15, 13, 19, 12, 9, 14, 1, 13, 4, 9,
    13, 15, 12, 19, 3, 2, 1, 10, 6, 7, 2, 3, 14, 9, 10, 10, 16, 9, 3, 5, 8, 14, 7, 6, 4, 5, 10,
    12, 2, 6, 19, 7, 5, 3, 1, 1, 1, 1, 3, 10, 18, 11, 17, 12, 14, 16, 5, 4, 15, 8, 1, 1, 1, 1,
};
char LimitFlag[6][19] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};
char VsLimitFlag[6][19] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};
static int randlogcnt = 0;
static int randlog[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
static SelectControl selst;
static int width1;
static int width2;
static int slemode;
static int offset;
static int count;
static int randflag[2];
static short int reloadflag[2];
static char *vector;
static int SelectMax;
static int status;

void disp_hande(int side, int mode)
{
    int offsetx[2][2] = { { 0x40, 0x1E0 }, { 0x40, 0x1C0 } };
    int offsety[2] = { 0x18, 0x1A0 };
    int valtb[2];

    if (!(pads[side].trg & 4) && (pads[side].trg & 8)) {
        if (selst.vital[side] < 2) {
            selst.vital[side] = 8;
        } else {
            selst.vital[side] = selst.vital[side] - 1;
        }
        sound_request(0x37);
    }
    valtb[0] = g_info.vital[side] = selst.vital[side];
    if (mode == 0) {
        FontDraw("VITAL", offsetx[0][side], offsety[mode], 1.0f, 0xFF);
        FontDraw(G_sprintf(0, "%3d", valtb), offsetx[0][side] + 0x40, offsety[mode], 1.0f, 0xFF);
    } else {
        FontDrawDC("VITAL", offsetx[mode][side], offsety[mode], 2.0f, 1, 2);
        FontDrawDC(G_sprintf(0, "%3d", valtb), offsetx[mode][side] + 0x50, offsety[mode], 2.0f, 1, 2);
    }
}

void change_extra_color(void)
{
    int i;
    int extcol;

    for (i = 0; i < 2; i++) {
        if (sc.selcnt[i] >= 15 && g_info.PlayerPalette[i] < 5) {
            extcol = g_info.PlayerPalette[i] + 6;
            if (chk_select_color(UseCharacter1p2p[i], extcol)) {
                g_info.PlayerPalette[i] = extcol;
            }
            if (g_info.GoldMode) {
                if (g_info.PlayerPalette[i] == 9) {
                    g_info.PlayerPalette[i] -= 6;
                } else if (g_info.PlayerPalette[i] == 10) {
                    g_info.PlayerPalette[i] -= 6;
                }
            }
            if ((sc.extcol_flg >> i) & 1) {
                if (chk_select_color(UseCharacter1p2p[i], 5)) {
                    g_info.PlayerPalette[i] = 5;
                }
            }
        }
    }
}

void qselect_init(SELECTINFO *sel)
{
    int i;
    int id;
    int pos[18][2] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 }, { 2, 0 }, { 2, 1 }, { 3, 0 }, { 3, 1 }, { 4, 0 }, { 4, 1 }, { 7, 0 }, { 5, 0 }, { 5, 1 }, { 6, 0 }, { 6, 1 }, { 7, 1 }, { 8, 0 }, { 8, 1 } };

    if (g_info.vital[0] >= 1 && g_info.vital[0] <= 8) {
        selst.vital[0] = g_info.vital[0];
    } else {
        g_info.vital[0] = selst.vital[0] = 8;
    }
    if (g_info.vital[1] >= 1 && g_info.vital[1] <= 8) {
        selst.vital[1] = g_info.vital[1];
    } else {
        g_info.vital[1] = selst.vital[1] = 8;
    }
    PXF_SpriteClear();
    FadeInit();
    dcfont_init();
    for (i = 0; i < 2; i++) {
        if (!g_info.PlayerSelected[i]) {
            id = g_info.SelectPlayerIdno[i];
            sel[i].selected = 0;
            sel[i].color = -1;
            UseCharacter1p2p[i] = 0;
        } else {
            id = UseCharacter1p2p[i];
            sel[i].selected = id;
            sel[i].color = g_info.PlayerPalette[i];
        }
        sel[i].pv = pos[id - 1][0];
        sel[i].ph = pos[id - 1][1];
        sc.selcnt[i] = 0;
    }
    sc.bg = -1;
    sc.selcnt[0] = 0;
    sc.selcnt[1] = 0;
    sc.extcol_flg = 0;
}

void SelectInit(void)
{
    SubJob = 0;
    SubJobCount = 0;
    heap_clear();
    if (userinfo.quickconti) {
        qselect_init(SelInfo);
        sound_request(0x56);
        if (GetLimitStatus() == 1) {
            SelectMax = 15;
        } else if (GetLimitStatus() == 2) {
            SelectMax = 16;
        } else if (GetLimitStatus() == 4) {
            SelectMax = 17;
        } else if (GetLimitStatus() == 5) {
            SelectMax = 18;
        } else {
            SelectMax = 14;
        }
        if ((g_info.mode & 0x2000) && SelectMax > 16) {
            SelectMax = 16;
        }
        JobChange(15);
    } else {
        SelectFile = FileReadSelect(0xA2, 2);
        JobNext();
    }
}

void SelectLoad(void)
{
    if (FileReadCheck()) {
        JobNext();
    }
}

int GetSelectColorIndexByKey(int side)
{
    if (pads[side].trg & 0x20) {
        return 0;
    }
    if (pads[side].trg & 0x40) {
        return 1;
    }
    if (pads[side].trg & 0x80) {
        return 2;
    }
    if (pads[side].trg & 0x10) {
        return 3;
    }
    if (pads[side].trg & 0x800) {
        return 4;
    }
    return -1;
}

void SelectEnd(void)
{
    dfree(SelectFile);
    SelectFile = 0;
}

int RandSel(void)
{
    int i;
    int n;
    int idx;

    i = -1;
    if (GetLimitStatus() == 1) {
        idx = 1;
    } else if (GetLimitStatus() == 2) {
        idx = 2;
    } else if (GetLimitStatus() == 3) {
        idx = 3;
    } else if (GetLimitStatus() == 4) {
        idx = 4;
    } else if (GetLimitStatus() == 5) {
        idx = 5;
    } else {
        idx = 0;
    }
    if (g_info.mode & 0x2000) {
        if (idx >= 3) {
            idx = 2;
        }
    }
    while (i == -1) {
        i = Random() % 18;
        if (!VsLimitFlag[idx][i]) {
            i = -1;
        }
        for (n = 0; n < 10; n++) {
            if (i == randlog[n]) {
                i = -1;
            }
        }
    }
randlog[randlogcnt] = i;
    randlogcnt++;
    if (randlogcnt >= 10) {
        randlogcnt = 0;
    }
    return i + 1;
}

int GetCursorMoveIndexByKey(int side)
{
    int ret;

    ret = -1;
    if (pads[side].data & 0x8000) {
        ret = 0;
    } else if (pads[side].data & 0x1000) {
        ret = 1;
    } else if (pads[side].data & 0x2000) {
        ret = 2;
    } else if (pads[side].data & 0x4000) {
        ret = 3;
    }
    if (ret >= 0) {
        if (!(sc.cnt[side] == 0 || (sc.cnt[side] > 16 && !(sc.cnt[side] & 3)))) {
            ret = -1;
        }
        sc.cnt[side]++;
    } else {
        sc.cnt[side] = 0;
    }
    return ret;
}

void SelectBGDraw(void)
{
    unsigned int color;

    if (slemode & 0x100) {
        color = gStripGetFadeColor(0xFF003A46);
    } else if (slemode & 0x200) {
        color = gStripGetFadeColor(0xFF30493E);
    } else if (slemode & 0x2000) {
        color = gStripGetFadeColor(0xFF643232);
    } else {
        color = gStripGetFadeColor(0xFF362C1E);
    }
    nlSetBackColor(color, color, color);
}

void SelectInputExecute(int side)
{
    int indexoffset;
    int selectcolor;
    int keyside;

    keyside = side;
    side &= 1;
    if (keyside & 0x10000) {
        keyside = side ^ 1;
    } else {
        keyside = side;
    }
    selectcolor = GetSelectColorIndexByKey(keyside);
    if (selectcolor != -1) {
        if (selst.p[side] == 0x13) {
            selst.p[side] = RandSel();
            ReLoadPalette(side);
        }
        if (userinfo.phase >= 3) {
            if (pads[keyside].data & 2) {
                if (pads[keyside].data & 1) {
                    if (selst.p[side] == 2) {
                        userinfo.gguse[side] = 4;
                    } else {
                        userinfo.gguse[side] = 6;
                    }
                } else {
                    userinfo.gguse[side] = 2;
                }
            } else {
                userinfo.gguse[side] = 0;
            }
        }
        if (userinfo.gguse[side] & 4) {
            debugf("Player %d is GGEX Chara\n", side + 1);
        } else if (userinfo.gguse[side] & 2) {
            debugf("Player %d is GG Chara\n", side + 1);
        } else {
            debugf("Player %d is Normal Chara\n", side + 1);
        }
        selst.color[side] = selectcolor;
        selst.flag[side] = 4;
        sound_request(0x39);
        if (slemode & 3) {
            if (selst.p[0] == selst.p[1] && selst.color[0] == selst.color[1]) {
                if (selst.flag[side ? 0 : 1]) {
                    selst.color[side] = 5;
                }
            }
        }
        EntrySelectCharacter(side, selst.p[side], selst.color[side], 0);
    } else if (!randflag[side]) {
        indexoffset = GetCursorMoveIndexByKey(keyside);
        if (indexoffset != selectcolor) {
            sound_request(0x37);
            if (!side) {
                width1 = 0;
            } else {
                width2 = 0;
            }
            selst.p[side] = vector[selst.p[side] * 4 + indexoffset - 4];
            reloadflag[side] = 2;
        }
    }
    if (g_info.mode & 0x800) {
        if (selst.flag[side] != 1) {
            disp_hande(side, 0);
        }
    }
}

int SelectGetOtherFlag(int side)
{
    return side ? 1 : 2;
}

int SelectGetSelfFlag(int side)
{
    return side ? 2 : 1;
}

void SelectExecuteByPlayerSide(int side)
{
    int sd;
    int pos;

    if (!(slemode & SelectGetSelfFlag(side))) {
        if ((pads[side].trg & 0x800) && Cr_IsStartWait(side) && !(slemode & 0xFFFFFFFC)) {
            Cr_Request_Start(side);
            count = 20;
            slemode |= SelectGetSelfFlag(side);
            SetCPUCharacterFlag(side, 0);
        } else if (slemode & 0x4100) {
            SetCPUCharacterFlag(side, 0);
        } else {
            SetCPUCharacterFlag(side, 1);
        }
        if (slemode == (side == 0 ? 0x102 : 0x101) || slemode == (side == 0 ? 0x4002 : 0x4001)) {
            if (selst.flag[side == 0] == 1) {
                SelectInputExecute(side | 0x10000);
            }
        }
    } else {
        if (selst.flag[side] == 0) {
            SelectInputExecute(side);
        } else if (pads[side].trg & 8) {
            sound_request(0x3B);
            g_info.PlayerSelected[side] = 0;
            selst.flag[side] = 0;
            g_info.SelectPlayerIdno[side] = 0;
            UseCharacter1p2p[side] = 0;
            chk_select_reset(side);
        }
    }
    if (g_info.mode & 0x900) {
        if (g_info.mode & 0x100) {
            sd = tr_info->ply;
            pos = (sd == 0);
            if (selst.flag[0] == 0) {
                pos = sd;
            }
        } else {
            pos = side;
            sd = pos;
        }
        if (pads[sd].trg & 4) {
            sc.bg = selst.p[pos];
        }
    }
}

void CharIcon(int sw)
{
    static int i;
    static int max;
    static int fms;
    static int fmsnext;
    static int count;
    static int px;
    static int py;
    static float x;
    static float y0[6] = { 480.0f, 480.0f, 480.0f, 480.0f, 480.0f, 480.0f };
    static float y1[7] = { 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f, 64.0f };
    static int chriconmode;
    int idx;

    idx = GetLimitStatus();
    if (g_info.mode & 0x2000) {
        if (idx >= 3) {
            idx = 2;
        }
    }
    if (sw == 0) {
        status = 0;
        fms = 0;
        x = 0;
        chriconmode = 0;
        return;
    }
    if (chriconmode == 0) {
        if (fms < 0x28) {
            SelectBGDraw();
            if (fms >= 3) {
                for (i = 0; i < 6; i++) {
                    polydraw_FBox(lp0[i].x1, (int)y0[i], lp0[i].x2, lp0[i].y2, 23.0f, 0xFFFFFFFF);
                    y0[i] -= 38.6f;
                    if ((int)y0[i] < lp0[i].y1) {
                        y0[i] = lp0[i].y1;
                    }
                }
            }
            if (fms >= 5) {
                for (i = 0; i < 7; i++) {
                    polydraw_FBox(lp1[i].x1, lp1[i].y1, lp1[i].x2, y1[i], 21.0f, 0xFFFFFFFF);
                    y1[i] += 34.666668f;
                    if ((int)y1[i] > lp1[i].y2) {
                        y1[i] = lp1[i].y2;
                    }
                }
            }
            for (i = 0; i < 9; i++) {
                polydraw_FBox(lp2[i].x1, lp2[i].y1, lp2[i].x2, lp2[i].y2, 19.0f, 0xFFFFFFFF);
            }
            x += 8.533334f;
            if ((int)x > 0x140) {
                x = 320.0f;
            }
            polydraw_FBox(0.0f, 0.0f, 0x140 - (int)x, 480.0f, 15.0f, 0xFF000000);
            polydraw_FBox((int)x + 0x140, 0.0f, 640.0f, 480.0f, 15.0f, 0xFF000000);
            fms++;
        } else {
            chriconmode = 1;
            max = 0x13;
            fms = 0x2D;
            fmsnext = 0x2F;
            count = 0;
        }
    }
    if (chriconmode == 1) {
        if (fms < 0x5A) {
            SelectBGDraw();
            for (i = 0; i < count; i++) {
                px = selpos[i].x;
                py = selpos[i].y;
                if (selpos[i].type) {
                    px += 0x31;
                } else {
                    px += 0x25;
                }
                if (LimitFlag[idx][i]) {
                    EasySpriteDrawTR(i + 0x58, px, py, selpos[i].pry, 1.0f);
                }
            }
            if (++fms == fmsnext) {
                if (++count > max) {
                    count = max;
                }
                fmsnext = fms + 2;
            }
            for (i = 0; i < 6; i++) {
                polydraw_FBox(lp0[i].x1, lp0[i].y1, lp0[i].x2, lp0[i].y2, 23.0f, 0xFFFFFFFF);
            }
            for (i = 0; i < 7; i++) {
                polydraw_FBox(lp1[i].x1, lp1[i].y1, lp1[i].x2, lp1[i].y2, 21.0f, 0xFFFFFFFF);
            }
            for (i = 0; i < 9; i++) {
                polydraw_FBox(lp2[i].x1, lp2[i].y1, lp2[i].x2, lp2[i].y2, 19.0f, 0xFFFFFFFF);
            }
        } else {
            offset = 0;
            width1 = 100;
            width2 = 100;
            chriconmode = 2;
        }
    }
    if (chriconmode == 2) {
        SelectBGDraw();
        if (slemode == 1) {
            x = 100.0f;
        } else if (slemode == 2) {
            x = -100.0f;
        } else if ((slemode & 3) == 3) {
            x = 0;
        } else if (slemode & 0x100) {
            x = 0;
        } else if (slemode & 0x4000) {
            x = 0;
        } else if (slemode & 1) {
            x = 100.0f;
        } else if (slemode & 2) {
            x = -100.0f;
        }
        if (offset < x) {
            offset += 16.0;
            if (x < offset) {
                offset = x;
            }
        } else if (x < offset) {
            offset -= 16.0;
            if (offset < x) {
                offset = x;
            }
        }
        if (x == offset) {
            status = 2;
        }
        for (i = 0; i < count; i++) {
            if ((slemode == 1 && selst.p[0] - 1 != i) || (slemode == 2 && selst.p[1] - 1 != i)
                || ((slemode & 3) && selst.p[0] - 1 != i && selst.p[1] - 1 != i)
                || (slemode == 0x101 && selst.p[0] - 1 != i) || (slemode == 0x102 && selst.p[1] - 1 != i)
                || (slemode == 0x101 && selst.flag[0]) || (slemode == 0x102 && selst.flag[1])
                || (slemode == 0x201 && selst.p[0] - 1 != i) || (slemode == 0x202 && selst.p[1] - 1 != i)
                || (slemode == 0x2001 && selst.p[0] - 1 != i) || (slemode == 0x2002 && selst.p[1] - 1 != i)
                || (slemode == 0x4001 && selst.p[0] - 1 != i) || (slemode == 0x4002 && selst.p[1] - 1 != i)
                || (slemode == 0x4001 && selst.flag[0]) || (slemode == 0x4002 && selst.flag[1])
                || (slemode == 1 && selst.flag[0]) || (slemode == 2 && selst.flag[1])
                || (selst.flag[0] && selst.p[0] == i + 1) || (selst.flag[1] && selst.p[1] == i + 1)
                || ((slemode & 3) && selst.flag[0] && selst.flag[1])
                || (width1 == 100 && width2 == 100)) {
                px = selpos[i].x;
                py = selpos[i].y;
                if (selpos[i].type) {
                    px += 0x31;
                } else {
                    px += 0x25;
                }
                if (LimitFlag[idx][i]) {
                    EasySpriteDrawTR(i + 0x58, px + offset, py, selpos[i].pry, 1.0f);
                }
            }
        }
        if (x != offset) {
            status = 1;
        }
        for (i = 0; i < 6; i++) {
            polydraw_FBox(lp0[i].x1 + offset, lp0[i].y1, lp0[i].x2 + offset, lp0[i].y2, 23.0f, gStripGetFadeColor(0xFFFFFFFF));
        }
        for (i = 0; i < 7; i++) {
            polydraw_FBox(lp1[i].x1 + offset, lp1[i].y1, lp1[i].x2 + offset, lp1[i].y2, 21.0f, gStripGetFadeColor(0xFFFFFFFF));
        }
        for (i = 0; i < 9; i++) {
            polydraw_FBox(lp2[i].x1 + offset, lp2[i].y1, lp2[i].x2 + offset, lp2[i].y2, 19.0f, gStripGetFadeColor(0xFFFFFFFF));
        }
    }
}

void Ebbs(int cnt)
{
    int i;
    static int status;
    static int flg;
    static int fms;
    static float y1;
    static float y2;
    static float hight;
    static float x1[4];
    static float x2[4];

    if (cnt == 0) {
x1[0] = 0;
        x2[0] = 0;
        x1[1] = 216.0f;
        x1[2] = 432.0f;
        x1[3] = 648.0f;
        x2[1] = 216.0f;
        x2[2] = 432.0f;
        x2[3] = 648.0f;
        status = 0;
        flg = 0;
        fms = 0;
        y1 = x1[0];
        y2 = x1[0];
        hight = x1[0];
    }
    switch (status) {
    case 0:
        y1 = 48.0f;
        hight = 10.0f;
        y2 = 428.0f;
        status = 1;
        flg = !flg;
        fms = 0;
        break;
    case 1:
        for (i = 0; i < 4; i++) {
            if (flg) {
                EasySpriteDrawEX(0x72, x1[i], 0x3A, 13.0f, 0xD, 1.0f);
                EasySpriteDrawEX(0x72, x2[i], 0x1B6, 13.0f, 0x1F, 1.0f);
            } else {
                EasySpriteDrawEX(0x72, x1[i], 0x1B6, 13.0f, 0xD, 1.0f);
                EasySpriteDrawEX(0x72, x2[i], 0x3A, 13.0f, 0x1F, 1.0f);
            }
            x1[i] -= 1.44;
            if (x1[i] <= -216.0f) {
                x1[i] = 648.0f;
            }
            x2[i] += 1.44;
            if (x2[i] >= 864.0f) {
                x2[i] = 0;
            }
        }
        if (++fms == 392) {
            status = 2;
        }
        break;
    case 2:
        hight -= 1.25;
        if (hight <= 0.0) {
            hight = 0;
            status = 3;
        }
        break;
    case 3:
        y1 += 15.833333333333334;
        y2 -= 15.833333333333334;
        if (y1 >= 428.0f || y2 <= 48.0f) {
            y1 = 428.0f;
            status = 4;
            y2 = 48.0f;
        }
        break;
    case 4:
        hight += 1.25;
        if (hight >= 0.0f) {
            status = 0;
            hight = 10.0f;
        }
        break;
    }
    for (i = 0; i < 640; i += 4) {
        polydraw_FBox(i, y1 - hight, i + 2, y1 - hight + 4.0f, 13.0f, 0xFFFFFFFF);
        polydraw_FBox(i + 2, y1 + hight, i + 4, y1 + hight + 4.0f, 13.0f, 0xFFFFFFFF);
        polydraw_FBox(i, y2 - hight, i + 2, y2 - hight + 4.0f, 13.0f, 0xFFFFFFFF);
        polydraw_FBox(i + 2, y2 + hight, i + 4, y2 + hight + 4.0f, 13.0f, 0xFFFFFFFF);
    }
}

void ReLoadPalette(int side)
{
    int palid[2] = { 32, 48 };
    int spid[2] = { 0, 1 };
    int *p;

    p = (int *)SelectFile;
    if (reloadflag[side] == 1) {
        PXF_Free(spid[side] + 0x20, palid[side]);
        IST_Load(&p[selst.p[side] - 1], spid[side] + 0x20, -1);
    }
    reloadflag[side]--;
}

void select_push_chk(void)
{
    int i;
    int ret;
    int pad;

    for (i = 0; i < 2; i++) {
        if (UseCharacter1p2p[i]) {
            if (sc.selcnt[i] != -1 && sc.selcnt[i] < 15) {
                if (g_info.mode & 0x100) {
                    pad = (tr_info->ply == i) ? i : !i;
                } else if (g_info.mode & 0x4000) {
                    pad = (get_player() == i) ? i : !i;
                } else {
                    pad = i;
                }
                if (pads[pad].data & 0x8F0) {
                    if (++sc.selcnt[i] >= 15) {
                        ret = 0;
                        if (pads[pad].data & 0x20) {
                            ret++;
                        }
                        if (pads[pad].data & 0x40) {
                            ret++;
                        }
                        if (pads[pad].data & 0x80) {
                            ret++;
                        }
                        if (pads[pad].data & 0x10) {
                            ret++;
                        }
                        if (pads[pad].data & 0x800) {
                            ret++;
                        }
                        if (ret >= 2) {
                            sc.extcol_flg |= 1 << i;
                        }
                    }
                } else if (sc.selcnt[i] > 0) {
                    sc.selcnt[i] = -1;
                } else {
                    sc.selcnt[i] = 0;
                }
            }
        } else {
            sc.selcnt[i] = 0;
            sc.extcol_flg &= ~(1 << i);
        }
    }
}

void disp_tag(int side)
{
    TAG_WORK tw[2] = {
        { 0, 0x5C, 0, 0x74, 0xFF0000FF, 0x40FFFFFF },
        { 0x42, 0x5C, 0x2A, 0x74, 0xFFFF0000, 0x40FFFFFF },
    };
    int px;
    int py;
    int offs;
    float pry;
    static short int flag[2] = { 0, 0 };

    pry = 9.0f;
    if (selst.flag[side]) {
        pry = 11.0f;
    }
    if (selpos[selst.p[side] - 1].type) {
        px = selpos[selst.p[side] - 1].x + tw[side].side_xoff;
        py = selpos[selst.p[side] - 1].y + tw[side].side_yoff;
        if (flag[side] || selst.flag[side]) {
            if (selst.flag[side]) {
                pry = 11.0f;
            }
            flag[side] = 0;
            polydraw_Frm(offset + selpos[selst.p[side] - 1].x, selpos[selst.p[side] - 1].y, offset + selpos[selst.p[side] - 1].x + 0x62, selpos[selst.p[side] - 1].y + 0x4A, 4.0f, (int)pry, tw[side].color1, tw[side].color2);
        } else {
            flag[side] = 1;
            polydraw_Frm(offset + selpos[selst.p[side] - 1].x, selpos[selst.p[side] - 1].y, offset + selpos[selst.p[side] - 1].x + 0x62, selpos[selst.p[side] - 1].y + 0x4A, 4.0f, (int)pry, tw[side].color2, tw[side].color1);
        }
    } else {
        px = selpos[selst.p[side] - 1].x + tw[side].length_xoff;
        py = selpos[selst.p[side] - 1].y + tw[side].length_yoff;
        if (flag[side] || selst.flag[side]) {
            if (selst.flag[side]) {
                pry = 11.0f;
            }
            flag[side] = 0;
            polydraw_Frm(offset + selpos[selst.p[side] - 1].x, selpos[selst.p[side] - 1].y, offset + selpos[selst.p[side] - 1].x + 0x4A, selpos[selst.p[side] - 1].y + 0x62, 4.0f, (int)pry, tw[side].color1, tw[side].color2);
        } else {
            flag[side] = 1;
            polydraw_Frm(offset + selpos[selst.p[side] - 1].x, selpos[selst.p[side] - 1].y, offset + selpos[selst.p[side] - 1].x + 0x4A, selpos[selst.p[side] - 1].y + 0x62, 4.0f, (int)pry, tw[side].color2, tw[side].color1);
        }
    }
    offs = userinfo.offs_cockpit_lower ? userinfo.offs_cockpit_lower + 12 : userinfo.offs_cockpit_lower;
    if (side == 0) {
        EasySpriteDrawEX(0x6C, px + offset, py, 7.0f, 0x20D, 1.0f);
        EasySpriteDrawEX(0x70, 0, offs + 0x1E2, 5.0f, 0x20D, 1.0f);
    } else {
        EasySpriteDrawEX(0x6D, px + offset, py, 7.0f, 0x20D, 1.0f);
        EasySpriteDrawEX(0x71, 0x220, offs + 0x1E2, 5.0f, 0x20D, 1.0f);
    }
}

void disp_bgtag(void)
{
    TAG_WORK tw = { 0, 0x4C, 0, 0x64, 0xFFFFFF00, 0x40FFFFFF };
    int px;
    int py;
    static short int flag = 0;

    if (sc.bg < 0) {
        return;
    }
    if (selpos[sc.bg - 1].type) {
        px = selpos[sc.bg - 1].x + tw.side_xoff;
        py = selpos[sc.bg - 1].y + tw.side_yoff;
        if (flag) {
            flag = 0;
            polydraw_Frm(offset + selpos[sc.bg - 1].x, selpos[sc.bg - 1].y, offset + selpos[sc.bg - 1].x + 0x62, selpos[sc.bg - 1].y + 0x4A, 4.0f, 9.0f, tw.color1, tw.color2);
        } else {
            flag = 1;
            polydraw_Frm(offset + selpos[sc.bg - 1].x, selpos[sc.bg - 1].y, offset + selpos[sc.bg - 1].x + 0x62, selpos[sc.bg - 1].y + 0x4A, 4.0f, 9.0f, tw.color2, tw.color1);
        }
    } else {
        px = selpos[sc.bg - 1].x + tw.length_xoff;
        py = selpos[sc.bg - 1].y + tw.length_yoff;
        if (flag) {
            flag = 0;
            polydraw_Frm(offset + selpos[sc.bg - 1].x, selpos[sc.bg - 1].y, offset + selpos[sc.bg - 1].x + 0x4A, selpos[sc.bg - 1].y + 0x62, 4.0f, 9.0f, tw.color1, tw.color2);
        } else {
            flag = 1;
            polydraw_Frm(offset + selpos[sc.bg - 1].x, selpos[sc.bg - 1].y, offset + selpos[sc.bg - 1].x + 0x4A, selpos[sc.bg - 1].y + 0x62, 4.0f, 9.0f, tw.color2, tw.color1);
        }
    }
    EasySpriteDrawEX(0x73, px, py, 7.0f, 0x20D, 1.0f);
}

void disp_bust(int side, int xp)
{
    BUST_WORK bw[2] = { { 32, 160, 512, 0 }, { 33, 480, 512, 0 } };
    int BustUp_U0[18] = { 0 };
    int BustUp_U1[18] = { 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512 };
    int BustUp_V0[18] = { 0 };
    int BustUp_V1[18] = { 512, 512, 512, 500, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512 };

    bw[0].plyid = selst.p[0] - 1;
    bw[1].plyid = selst.p[1] - 1;
    if (selst.p[side] == 19) {
        return;
    }
    if (reloadflag[side]) {
        ReLoadPalette(side);
        return;
    }
    EasySpriteDrawBustUp(bw[side].spid, bw[side].xp, bw[side].yp, BustUp_U0[bw[side].plyid], BustUp_U1[bw[side].plyid], BustUp_V0[bw[side].plyid], BustUp_V1[bw[side].plyid], 17.0f);
    EasySpriteDrawEX(selst.p[side] + 0x31, xp, 0x1A4, 3.0f, 0xD, 1.0f);
    polydraw_FBox(xp, 0.0f, xp + 0x30, 480.0f, 15.0f, 0xCC000000);
}

int chk_selectplayer(void)
{
    int ret;

    ret = 0;
    if (slemode == 1 && selst.flag[0]) {
        ret = 1;
    } else if (slemode == 2 && selst.flag[1]) {
        ret = 1;
    } else if ((slemode & 3) && selst.flag[0] && selst.flag[1]) {
        ret = 1;
    } else if ((slemode & 0x100) && selst.flag[0] && selst.flag[1]) {
        ret = 1;
    } else if (slemode == 0x201 && selst.flag[0]) {
        ret = 1;
    } else if (slemode == 0x202 && selst.flag[1]) {
        ret = 1;
    } else if ((slemode & 0x4000) && selst.flag[0] && selst.flag[1]) {
        ret = 1;
    } else if (slemode == 0x2001 && selst.flag[0]) {
        ret = 1;
    } else if (slemode == 0x2002 && selst.flag[1]) {
        ret = 1;
    }
    return ret;
}

void disp_icon(int side)
{
    ICON_WORK iw[2] = { { 0x20, 0 }, { 0x30, 0 } };
    int px;
    int py;
    float pry;

    iw[0].width = width1;
    iw[1].width = width2;
    px = selpos[selst.p[side] - 1].x;
    py = selpos[selst.p[side] - 1].y;
    if (selpos[selst.p[side] - 1].type) {
        px += 0x31;
    } else {
        px += 0x25;
    }
    pry = 9.0f;
    if (selst.flag[side]) {
        pry = 11.0f;
    }
    if (selst.p[side] != 19) {
        if (selst.flag[side] == 0 && reloadflag[side] == 0) {
            SetPaltbNo(selst.p[side] + 0x43, iw[side].palno);
            EasySpriteDrawTR(selst.p[side] + 0x43, px + offset, selpos[selst.p[side] - 1].y, pry, iw[side].width / 100.0);
        }
    } else {
        EasySpriteDrawTR(0x6A, px + offset, selpos[selst.p[side] - 1].y, pry, iw[side].width / 100.0);
    }
    if (selst.flag[side] >= 2) {
        selst.flag[side]--;
        if (selpos[selst.p[side] - 1].type) {
            polydraw_FBox(px + offset - 0x31, py, px + offset + 0x31, py + 0x48, 3.0f, 0xFFFFFFFF);
        } else {
            polydraw_FBox(px + offset - 0x25, py, px + offset + 0x25, py + 0x62, 3.0f, 0xFFFFFFFF);
        }
    }
}

void Select_Main_Init(void)
{
    int *p;
    int i;
    char *vectrtb[5] = { bectl_L, bectl_T, bectl, bectl_K, bectl_J };
    int idx;

    p = (int *)SelectFile;
    if (GetLimitStatus() == 1) {
        idx = 1;
    } else if (GetLimitStatus() == 2) {
        idx = 2;
    } else if (GetLimitStatus() == 4) {
        idx = 3;
    } else if (GetLimitStatus() == 5) {
        idx = 4;
    } else {
        idx = 0;
    }
    if ((g_info.mode & 0x2000) && idx >= 3) {
        idx = 2;
    }
    if (g_info.vital[0] >= 1 && g_info.vital[0] <= 8) {
        selst.vital[0] = g_info.vital[0];
    } else {
        selst.vital[0] = 8;
        g_info.vital[0] = 8;
    }
    if (g_info.vital[1] >= 1 && g_info.vital[1] <= 8) {
        selst.vital[1] = g_info.vital[1];
    } else {
        selst.vital[1] = 8;
        g_info.vital[1] = 8;
    }
    PXF_SpriteClear();
    LinkAddrTb((u_int *)p);
    TransSprtID = GetSpriteIdTop(4);
    for (i = 0; p[i]; i++) {
        IST_Load(&p[i], i + 0x20, -1);
    }
    FadeInit();
    vector = vectrtb[idx];
    slemode = 0;
    for (i = 0; i < 2; i++) {
        reloadflag[i] = 1;
        randflag[i] = 0;
        if (g_info.PlayerSelected[i] == 0) {
            selst.flag[i] = 0;
            selst.p[i] = g_info.SelectPlayerIdno[i];
            UseCharacter1p2p[i] = 0;
        } else {
            selst.flag[i] = 4;
            selst.p[i] = UseCharacter1p2p[i];
        }
        if (Cr_IsPlaying(i) && ((g_info.mode >> i) & 1)) {
            SetCPUCharacterFlag(i, 0);
        } else {
            SetCPUCharacterFlag(i, 1);
        }
        slemode = g_info.mode;
    }
    BGM_Request(0x17, 1);
    set_bgmvol();
    sc.extcol_flg = 0;
    sc.selcnt[0] = 0;
    sc.selcnt[1] = 0;
    sc.bg = -1;
    if (!g_info.PlayerSelected[0] || g_info.LoadPlayerID[0] == -1) {
        g_info.LoadPlayerID[0] = -1;
    }
    if (!g_info.PlayerSelected[1] || g_info.LoadPlayerID[1] == -1) {
        g_info.LoadPlayerID[1] = -1;
    }
    StageNo = 0;
}

void disp_player1p2p(int side, int xp)
{
    if ((slemode & SelectGetSelfFlag(side)) ||
        ((slemode & 0x4100) && (slemode & SelectGetOtherFlag(side)) && selst.flag[side == 0])) {
        disp_icon(side);
        disp_tag(side);
        disp_bust(side, xp);
    }
}

void disp_selecttimer(int count)
{
    TIMER_WORK tw[2] = { { 4, 380, 75, 380, 16 }, { 428, 380, 499, 380, 16 } };
    int id;

    if (!(g_info.mode & ~3)) {
        id = (slemode == 2);
        EasySpriteDrawEX(0x6F, tw[id].count_xoff + 0x30, tw[id].count_yoff + 0x20, 7.0f, 0x20D, 1.0f);
        EasySpriteDrawNum(0x6E, tw[id].num_xoff + 0x30, tw[id].num_yoff + 0x1C, tw[id].space, 3.0f, count);
    }
}

void SelectMain(void)
{
    static int rp_side;
    static int x;
    static int fms;
    static int color;
    static int x1;
    static int x2;
    static int chold1;
    static int chold2;
    static u_char a1;
    static u_char a2;
    static u_char a3;
    static u_char r;
    static u_char g;
    static u_char b;
    int BustUp_U0[18] = { 0 };
    int BustUp_U1[18] = { 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512 };
    int BustUp_V0[18] = { 0 };
    int BustUp_V1[18] = { 512, 512, 512, 500, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512 };

    if (SubJob == 0) {
        if (SubJobCount == 0) {
            pack_allfree();
            SubJobCount++;
        }
        if (SubJobCount == 1) {
            SubJobCount = 2;
        }
        if (SubJobCount == 2) {
            Select_Main_Init();
            CharIcon(0);
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 1) {
        status = 0;
        CockpitLoadInit();
        CreditFontLoad(0x12C);
        SubJob++;
    } else if (SubJob == 2) {
        if (status == 2) {
            count = 0x280;
            ReLoadPalette(0);
            ReLoadPalette(1);
            SubJob++;
        }
    } else if (SubJob == 3) {
        if (SubJobCount < 0x15) {
            DispCreditMsg();
            if (slemode & SelectGetSelfFlag(0)) {
                EasySpriteDrawBustUp(0x20, count + 0xA0, 0x200, BustUp_U0[selst.p[0] - 1], BustUp_U1[selst.p[0] - 1], BustUp_V0[selst.p[0] - 1], BustUp_V1[selst.p[0] - 1], 17.0f);
                EasySpriteDrawEX(selst.p[0] + 0x31, 5, 0x3C - count, 3.0f, 0xD, 1.0f);
            }
            if (slemode & SelectGetSelfFlag(1)) {
                EasySpriteDrawBustUp(0x21, -count + 0x1E0, 0x200, BustUp_U0[selst.p[1] - 1], BustUp_U1[selst.p[1] - 1], BustUp_V0[selst.p[1] - 1], BustUp_V1[selst.p[1] - 1], 17.0f);
                EasySpriteDrawEX(selst.p[1] + 0x31, 0x24B, count, 3.0f, 0xD, 1.0f);
            }
            SubJobCount++;
            count -= 0x20;
            if (count < 0) {
                count = 0;
            }
        } else {
            a3 = 3;
            count = 0x14;
            a2 = 1;
            x1 = 5;
            x2 = 0x24B;
            width1 = 100;
            width2 = 100;
            a1 = 0;
            fms = 0;
            reloadflag[0] = 0;
            reloadflag[1] = 0;
            sound_request(0x56);
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 4) {
        Read_Playdat(0x80);
        DispCreditMsg();
        if (width1 < 100) {
            width1 += 7;
        }
        if (width1 > 100) {
            width1 = 100;
        }
        if (width2 < 100) {
            width2 += 7;
        }
        if (width2 > 100) {
            width2 = 100;
        }
r++;
        g += 2;
        b += 3;
        color = 0xFF000000 | (r << 16) | (g << 8) | b;
        polydraw_FLine(x, 396.0f, x + 0x140, 396.0f, 21.0f, color);
        x += 10;
        if (x > 0x280) {
            x = -0x168;
            count--;
        }
        disp_selecttimer(count);
        rp_side = rp.side;
        if (selst.p[0] == selst.p[1] && (slemode & 3)) {
            if ((!(g_info.mode & 0x100) || selst.flag[tr_info->ply]) && (!(g_info.mode & 0x4000) || selst.flag[rp_side])
                && (!(g_info.mode & 0x2000) || selst.flag[story.self_side])) {
                width1 = 100;
                width2 = 100;
            }
        }
        if (fms >= 7) {
            fms = 0;
            a1++;
            a2++;
            a3++;
            if (a1 >= 6) {
                a1 = 0;
            }
            if (a2 >= 6) {
                a2 = 0;
            }
            if (a3 >= 6) {
                a3 = 0;
            }
        } else {
            fms++;
        }
        disp_player1p2p(0, x1);
        disp_player1p2p(1, x2);
        disp_bgtag();
        if (chold2 != selst.p[1]) {
            chold2 = selst.p[1];
            x2 = 0x280;
        }
        if (chold1 != selst.p[0]) {
            chold1 = selst.p[0];
            x1 = -0x30;
        }
        if (x1 < 5) {
            x1 += 4;
            if (x1 > 5) {
                x1 = 5;
            }
        }
        if (x2 > 0x24B) {
            x2 -= 4;
            if (x2 < 0x24B) {
                x2 = 0x24B;
            }
        }
        if (!(slemode & ~3) && count < 0) {
            pads[0].trg |= 0x20;
            pads[1].trg |= 0x20;
        }
        if (chk_selectplayer()) {
            SubJobCount = 0;
            SubJob++;
            Change_ScreenScale(1);
            g_info.BattleControlFlag |= 0x200;
            select_push_chk();
        } else {
            SelectExecuteByPlayerSide(0);
            SelectExecuteByPlayerSide(1);
            Ebbs(SubJobCount);
            SubJobCount++;
            select_push_chk();
        }
    } else if (SubJob == 5) {
        select_push_chk();
        if (SubJobCount == 0) {
            r = 0;
            color = 0xFFFFFF;
        }
        if (SubJobCount < 10) {
            Read_Playdat(0x80);
            DispCreditMsg();
            polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 25.0f, color);
            color = ((char)r << 24) | 0xFFFFFF;
            if (SubJobCount < 5) {
                r = r + 0x33;
            } else {
                r = r - 0x33;
            }
            if ((slemode & 1) || (g_info.mode & 0x4100)) {
                EasySpriteDrawBustUp(0x20, 0xA0, 0x200, BustUp_U0[selst.p[0] - 1], BustUp_U1[selst.p[0] - 1], BustUp_V0[selst.p[0] - 1], BustUp_V1[selst.p[0] - 1], 17.0f);
                EasySpriteDrawEX(selst.p[0] + 0x31, 5, 0x1A4, 27.0f, 0xD, 1.0f);
            }
            if ((slemode & 2) || (g_info.mode & 0x4100)) {
                EasySpriteDrawBustUp(0x21, 0x1E0, 0x200, BustUp_U0[selst.p[1] - 1], BustUp_U1[selst.p[1] - 1], BustUp_V0[selst.p[1] - 1], BustUp_V1[selst.p[1] - 1], 17.0f);
                EasySpriteDrawEX(selst.p[1] + 0x31, 0x24B, 0x1A4, 25.0f, 0xD, 1.0f);
            }
            SubJobCount++;
        } else {
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 6) {
        select_push_chk();
        if (SubJobCount < 0x320) {
            Read_Playdat(0x80);
            DispCreditMsg();
            if ((slemode & 1) || (g_info.mode & 0x4100)) {
                EasySpriteDrawBustUp(0x20, SubJobCount + 0xA0, 0x200, BustUp_U0[selst.p[0] - 1], BustUp_U1[selst.p[0] - 1], BustUp_V0[selst.p[0] - 1], BustUp_V1[selst.p[0] - 1], 17.0f);
                EasySpriteDrawEX(selst.p[0] + 0x31, 0, 0x1C0 - SubJobCount, 27.0f, 0x20D, 1.0f);
            }
            if ((slemode & 2) || (g_info.mode & 0x4100)) {
                EasySpriteDrawBustUp(0x21, -SubJobCount + 0x1E0, 0x200, BustUp_U0[selst.p[1] - 1], BustUp_U1[selst.p[1] - 1], BustUp_V0[selst.p[1] - 1], BustUp_V1[selst.p[1] - 1], 17.0f);
                EasySpriteDrawEX(selst.p[1] + 0x31, 0x24B, SubJobCount + 0x20, 3.0f, 5, 1.0f);
            }
            SubJobCount += 0x28;
        } else {
            x = 0;
            SetFadeSpeed(0x14);
            Rq_Fadeout();
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 7) {
        select_push_chk();
        if (!IsFadeEnd()) {
            DispCreditMsg();
        } else {
            polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 1.0f, 0xFF000000);
            SubJob++;
        }
    } else if (SubJob == 8) {
        select_push_chk();
        nlSetBackColor(0, 0, 0);
        g_info.BattleControlFlag &= ~0x200;
        sound_alloff();
        BGM_Free();
        nlFreeCommonTextureAll();
        nlFreePaletteAll();
        change_extra_color();
        if (g_info.mode & 0x100) {
            Enter_Stage_Bg();
            tr_info->stat = 0;
            JobChange(0x1E);
        } else if (g_info.mode & 0x200) {
            JobChange(0x15);
            InitSurvivalBattle();
        } else if (g_info.mode & 0x4000) {
            Enter_Stage_Bg();
            JobChange(0x1E);
        } else if (g_info.mode & 0x2000) {
            JobChange(0x31);
        } else {
            JobChange(VsCpuCtrl());
        }
        sc.rannyu = 0;
        SelectEnd();
    }
    if (SubJob >= 1 && SubJob <= 7) {
        CharIcon(1);
    }
}

void qsel_disp_name(CHARADAT *cdat, int max)
{
    int i;

    FontDrawDC("CHARACTER SELECT", 32.0f, 40.0f, 2.0f, 1, 0);
    for (i = 0; i < max; i++) {
        FontDrawDC(cdat[i].name, cdat[i].xp, cdat[i].yp, 2.0f, 1, 1);
    }
}

void qsel_operate(int side, SELECTINFO *sel, CHARADAT *cdat)
{
    int qsel_vmax[2][5] = { { 7, 8, 8, 9, 9 }, { 7, 7, 8, 8, 9 } };
    int qsel_pos[2][9] = { { 0, 2, 4, 6, 8, 10, 12, 14, 16 }, { 1, 3, 5, 7, 9, 11, 13, 15, 17 } };
    int se;
    int ret;
    int pad;
    int max;

    se = -1;
    if (GetLimitStatus() == 1) {
        max = 1;
    } else if (GetLimitStatus() == 2) {
        max = 2;
    } else if (GetLimitStatus() == 4) {
        max = 3;
    } else if (GetLimitStatus() == 5) {
        max = 4;
    } else {
        max = 0;
    }
    if (g_info.mode & 0x2000) {
        if (max >= 3) {
            max = 2;
        }
    }
pad = (((g_info.mode & 0x100) && tr_info->ply != side) || ((g_info.mode & 0x4000) && get_player() != side)) ? !side : side;
    if (sel[side].selected == 0) {
        if (pads[pad].data & 0x1000) {
            if (sc.cnt[pad] == 0 || (sc.cnt[pad] > 16 && !(sc.cnt[pad] & 3))) {
                se = 0x37;
                if (--sel[side].pv < 0) {
                    sel[side].pv = qsel_vmax[sel[side].ph][max] - 1;
                }
            }
            sc.cnt[pad]++;
        } else if (pads[pad].data & 0x4000) {
            if (sc.cnt[pad] == 0 || (sc.cnt[pad] > 16 && !(sc.cnt[pad] & 3))) {
                se = 0x37;
                if (++sel[side].pv >= qsel_vmax[sel[side].ph][max]) {
                    sel[side].pv = 0;
                }
            }
            sc.cnt[pad]++;
        } else if (pads[pad].data & 0x8000) {
            if (sc.cnt[pad] == 0 || (sc.cnt[pad] > 16 && !(sc.cnt[pad] & 3))) {
                se = 0x37;
                if (qsel_vmax[sel[side].ph ? 0 : 1][max] >= sel[side].pv + 1) {
                    sel[side].ph ^= 1;
                } else {
                    se = -1;
                }
            }
            sc.cnt[pad]++;
        } else if (pads[pad].data & 0x2000) {
            if (sc.cnt[pad] == 0 || (sc.cnt[pad] > 16 && !(sc.cnt[pad] & 3))) {
                se = 0x37;
                if (qsel_vmax[sel[side].ph ? 0 : 1][max] >= sel[side].pv + 1) {
                    sel[side].ph ^= 1;
                } else {
                    se = -1;
                }
            }
            sc.cnt[pad]++;
        } else {
            sc.cnt[pad] = 0;
        }
    }
    if ((g_info.mode & 0x800) && sel[side].selected == 0) {
        disp_hande(side, 1);
    }
    ret = GetSelectColorIndexByKey(pad);
    if (sel[side].selected == 0) {
        if (ret != -1) {
            sel[side].color = ret;
            sel[side].selected = cdat[qsel_pos[sel[side].ph][sel[side].pv]].no;
            se = 0x39;
            if (userinfo.phase >= 3) {
                if (pads[pad].data & 2) {
                    if (pads[pad].data & 1) {
                        if (sel[side].selected == 2) {
                            userinfo.gguse[side] = 4;
                        } else {
                            userinfo.gguse[side] = 6;
                        }
                    } else {
                        userinfo.gguse[side] = 2;
                    }
                } else {
                    userinfo.gguse[side] = 0;
                }
            }
            if (userinfo.gguse[side] & 4) {
                debugf("Player %d is GGEX Chara\n", side + 1);
            } else if (userinfo.gguse[side] & 2) {
                debugf("Player %d is GG Chara\n", side + 1);
            } else {
                debugf("Player %d is Normal Chara\n", side + 1);
            }
            if (sel[side ? 0 : 1].selected == sel[side].selected && sel[side ? 0 : 1].color == ret) {
                sel[side].color = 5;
            }
            EntrySelectCharacter(side, sel[side].selected, sel[side].color, 0);
            pads[side].trg = 0;
        }
    } else if (pads[side].trg & 8) {
        sel[side].selected = 0;
        g_info.PlayerSelected[side] = 0;
        UseCharacter1p2p[side] = 0;
        g_info.SelectPlayerIdno[side] = 0;
        se = 0x3B;
        chk_select_reset(side);
    }
    if (se > 0) {
        sound_request(se);
    }
    if (pads[pad].trg & 4) {
        sc.bg = cdat[qsel_pos[sel[side].ph][sel[side].pv]].no;
    }
}

void qsel_disp_cursol(int side, SELECTINFO *sel)
{
    int x;
    int y;
    int alpha;
    char *cur[2] = { "1P", "2P" };

    if (((g_info.mode >> side) & 1) || (g_info.mode & 0x4100)) {
        x = sel[side].ph ? 0x152 : 0x52;
        if ((g_info.mode & 1) && side && sel[0].pv == sel[1].pv && sel[0].ph == sel[1].ph) {
            x -= 0x28;
        } else if ((g_info.mode & 0x4100) && (g_info.mode & 2) && !side && sel[1].pv == sel[0].pv && sel[1].ph == sel[0].ph) {
            x -= 0x28;
        }
        y = sel[side].pv * 32 + 0x70;
        alpha = sel[side].selected ? 0xA0 : 1;
        FontDrawDC(cur[side], x, y, 3.0f, alpha, 2);
    }
}

int chk_selectplayer_quick(SELECTINFO *sel)
{
    int ret;

    ret = 0;
    if (g_info.mode == 1 && sel[0].selected) {
        ret = 1;
    } else if (g_info.mode == 2 && sel[1].selected) {
        ret = 1;
    } else if ((g_info.mode & 3) && sel[0].selected && sel[1].selected) {
        ret = 1;
    } else if ((g_info.mode & 0x100) && sel[0].selected && sel[1].selected) {
        ret = 1;
    } else if ((g_info.mode & 0x200) && sel[sv_info->ply].selected) {
        ret = 1;
    } else if ((g_info.mode & 0x4000) && sel[0].selected && sel[1].selected) {
        ret = 1;
    } else if ((g_info.mode & 0x2000) && sel[story.self_side].selected) {
        ret = 1;
    }
    return ret;
}

void fade_bg(void)
{
    static int num = 0;
    static int frame = 0;
    int bg[3];
    int i;
    int col[3] = { 0xFF004000, 0xFF002000, 0xFF000000 };

    for (i = 0; i < 3; i++) {
        bg[i] = ((col[i] & 0xFFFFFF) >> 8) - num - frame;
        if (bg[i] < 0) {
            bg[i] = 0;
        }
        bg[i] = (col[i] & 0xFFFF00FF) | (bg[i] << 8);
    }
    nlSetBackColor(bg[0], bg[1], bg[2]);
    if (frame >= 2) {
        frame = 0;
        num += 8;
    } else {
        frame++;
    }
}

void QuickSelectMain(void)
{
    CHARADAT cdat[18] = {
        { "SOL", 1, 128, 112 }, { "KY", 2, 384, 112 }, { "MAY", 3, 128, 144 }, { "MILLIA", 4, 384, 144 },
        { "AXL", 5, 128, 176 }, { "POTEMKIN", 6, 384, 176 }, { "CHIPP", 7, 128, 208 }, { "ZATO", 8, 384, 208 },
        { "BAIKEN", 9, 128, 240 }, { "FAUST", 10, 384, 240 }, { "JAM", 12, 128, 272 }, { "ANJI", 13, 384, 272 },
        { "JOHNNY", 14, 128, 304 }, { "VENOM", 15, 384, 304 }, { "TESTAMENT", 11, 128, 336 }, { "DIZZY", 16, 384, 336 },
        { "KLIFF", 17, 128, 368 }, { "JUSTICE", 18, 384, 368 },
    };
    int i;

    if (SubJob == 0) {
        Read_Playdat(0x80);
        nlSetBackColor(0xFF004000, 0xFF002000, 0xFF000000);
        qsel_disp_name(cdat, SelectMax);
        for (i = 0; i < 2; i++) {
            if ((g_info.mode >> i) & 1) {
                SetCPUCharacterFlag(i, 0);
                qsel_operate(i, SelInfo, cdat);
                qsel_disp_cursol(i, SelInfo);
            } else if (!(g_info.mode & 0x4100)) {
                if ((pads[i].trg & 0x800) && Cr_IsStartWait(i) && !(g_info.mode & 0x2200)) {
                    Cr_Request_Start(i);
                    SetCPUCharacterFlag(i, 0);
                } else {
                    SetCPUCharacterFlag(i, 1);
                }
            } else {
                SetCPUCharacterFlag(i, 0);
                if (SelInfo[i ? 0 : 1].selected) {
                    qsel_operate(i, SelInfo, cdat);
                    qsel_disp_cursol(i, SelInfo);
                }
            }
        }
        if (chk_selectplayer_quick(SelInfo)) {
            Change_ScreenScale(1);
            SetFadeSpeed(0x14);
            Rq_Fadeout();
            goto next;
        }
    } else if (SubJob == 1) {
        if (IsFadeEnd()) {
            nlSetBackColor(0, 0, 0);
            nlFreeCommonTextureAll();
            nlFreePaletteAll();
        next:
            SubJob++;
        } else {
            fade_bg();
            qsel_disp_name(cdat, SelectMax);
            qsel_disp_cursol(0, SelInfo);
            qsel_disp_cursol(1, SelInfo);
        }
    } else if (SubJob == 2) {
        change_extra_color();
        if (g_info.mode & 0x100) {
            Enter_Stage_Bg();
            tr_info->stat = 0;
            JobChange(0x1E);
        } else if (g_info.mode & 0x200) {
            JobChange(0x1E);
            InitSurvivalBattle();
        } else if (g_info.mode & 0x4000) {
            Enter_Stage_Bg();
            JobChange(0x1E);
        } else if (g_info.mode & 0x2000) {
            JobChange(0x31);
        } else {
            JobChange(VsCpuCtrl());
        }
        sc.rannyu = 0;
        SelectEnd();
    }
    select_push_chk();
}
