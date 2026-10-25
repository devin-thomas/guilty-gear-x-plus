#include "generated/gallery.h"

static const short idtb[] = { 1, 2, 3 };
extern short int UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern u_int TotalCounter;
extern SYS_COND sc;
extern int StageNo;
void *mmalloc(int size, char *name);
void InitGameinfo_ByGameOver(int side);
extern PAD pads[2];
int iodat_chk(int stat);
int sound_request(int no);
extern CHARANGE_CTRL *gl_info;
static CHARANGE charange_01[] = {
    { 257, 2, 8, 8, 0, 0, 0, 0, 16384, 8, 0, -1 },
};
static CHARANGE charange_02[] = {
    { 260, 8, 8, 8, 128, 0, 0, 0, 2960, 8, 0, -1 },
};
static CHARANGE charange_03[] = {
    { 258, 1, 8, 8, 128, 0, 0, 0, 1939, 8, 0, -1 },
};
static CHARANGE charange_04[] = {
    { 257, 261, 8, 8, 0, 0, 0, 0, 36032, 2, 6, -1 },
};
static CHARANGE charange_05[] = {
    { 13, 268, 8, 8, 128, 128, 0, 0, 3874, 16, 0, -1 },
};
static CHARANGE charange_06[] = {
    { 258, 270, 8, 8, 0, 0, 0, 0, 34880, 2, 5, -1 },
};
static CHARANGE charange_07[] = {
    { 268, 267, 8, 8, 0, 0, 0, 0, 34880, 2, 8, -1 },
};
static CHARANGE charange_08[] = {
    { 1, 16, 8, 6, 128, 0, 0, 0, 16, 8, 0, -1 },
};
static CHARANGE charange_09[] = {
    { 7, 14, 6, 8, 64, 64, 0, 0, 16384, 8, 0, -1 },
};
static CHARANGE charange_10[] = {
    { 15, 10, 8, 4, 128, 0, 0, 0, 2050, 1, 0, -1 },
};
static CHARANGE charange_11[] = {
    { 5, 3, 8, 8, 0, 0, 128, 0, 3072, 8, 0, -1 },
};
static CHARANGE charange_12[] = {
    { 265, 269, 8, 8, 128, 0, 0, 128, 34816, 2, 7, -1 },
};
static CHARANGE charange_13[] = {
    { 12, 6, 8, 8, 0, 128, 0, 0, 1824, 8, 0, -1 },
};
static CHARANGE charange_14[] = {
    { 1, 7, 8, 8, 0, 0, 128, 0, 4096, 1, 0, 1 },
};
static CHARANGE charange_15[] = {
    { 11, 4100, 8, 4, 0, 0, 0, 0, 4, 1, 0, -1 },
};
static CHARANGE charange_16[] = {
    { 16, 4097, 8, 2, 64, 0, 0, 128, 32, 8, 0, 9 },
};
static CHARANGE charange_17[] = {
    { 10, 2, 0, 0, 128, 128, 128, 128, 16384, 1, 0, -1 },
};
static CHARANGE charange_18[] = {
    { 14, 6, 8, 4, 0, 128, 0, -64, 16384, 8, 0, 2 },
};
static CHARANGE charange_19[] = {
    { 9, 9, 8, 4, 128, 0, -64, 64, 14340, 1, 0, 3 },
};
static CHARANGE charange_20[] = {
    { 260, 263, 8, 8, 128, 0, 0, 0, 34816, 2, 15, -1 },
};
static CHARANGE charange_21[] = {
    { 3, 8, 8, 6, 0, 128, 0, 128, 768, 8, 0, 7 },
};
static CHARANGE charange_22[] = {
    { 6, 10, 6, 8, 0, 128, -32, 64, 6240, 8, 0, 4 },
};
static CHARANGE charange_23[] = {
    { 2, 15, 8, 4, 128, 0, -64, 128, 10244, 1, 0, 6 },
};
static CHARANGE charange_24[] = {
    { 1, 4098, 8, 8, 0, 128, 128, 64, 2080, 8, 0, -1 },
};
static CHARANGE charange_25[] = {
    { 4099, 4103, 4, 2, 0, 128, 0, 0, 1056, 8, 0, -1 },
};
static CHARANGE charange_26[] = {
    { 4101, 4102, 8, 6, 0, 128, 0, -32, 2112, 8, 0, -1 },
};
static CHARANGE charange_27[] = {
    { 4105, 4107, 8, 8, 128, 128, 0, 0, 4, 1, 0, -1 },
};
static CHARANGE charange_28[] = {
    { 4100, 4104, 8, 8, 0, 128, -8, 16, 4128, 8, 0, -1 },
};
static CHARANGE charange_29[] = {
    { 4098, 4113, 8, 6, 128, 0, 64, 0, 2816, 1, 0, -1 },
};
static CHARANGE charange_30[] = {
    { 4097, 4113, 6, 8, 0, 128, 128, -32, 2080, 8, 0, -1 },
};
static CHARANGE charange_31[] = {
    { 4113, 4114, 8, 8, 0, 0, 32, -16, 2048, 8, 0, -1 },
};
static CHARANGE charange_32[] = {
    { 4097, 4114, 4, 8, 64, 128, 64, -32, 3104, 8, 0, -1 },
};
static CHARANGE_LIST charange_all[] = {
    { "01", 1, 0, 60, -1, charange_01 },
    { "02", 1, 0, 99, -1, charange_02 },
    { "03", 1, 0, 99, -1, charange_03 },
    { "04", 1, 0, -1, 14, charange_04 },
    { "05", 2, 8, 30, -1, charange_05 },
    { "06", 2, 0, -1, 14, charange_06 },
    { "07", 2, 0, -1, 22, charange_07 },
    { "08", 2, 8, 40, -1, charange_08 },
    { "09", 3, 8, 60, -1, charange_09 },
    { "10", 3, 16, -1, -1, charange_10 },
    { "11", 3, 16, 60, -1, charange_11 },
    { "12", 3, 0, -1, 21, charange_12 },
    { "13", 4, 24, 99, -1, charange_13 },
    { "14", 4, 24, -1, -1, charange_14 },
    { "15", 4, 24, -1, -1, charange_15 },
    { "16", 4, 32, 45, -1, charange_16 },
    { "17", 4, 24, -1, -1, charange_17 },
    { "18", 4, 24, 99, -1, charange_18 },
    { "19", 4, 24, -1, -1, charange_19 },
    { "20", 4, 24, -1, 23, charange_20 },
    { "21", 5, 32, 60, -1, charange_21 },
    { "22", 5, 32, 99, -1, charange_22 },
    { "23", 5, 32, -1, -1, charange_23 },
    { "24", 5, 32, 99, -1, charange_24 },
    { "25", 6, 32, 10, -1, charange_25 },
    { "26", 6, 32, 99, -1, charange_26 },
    { "27", 6, 32, -1, -1, charange_27 },
    { "28", 6, 32, 60, -1, charange_28 },
    { "29", 6, 32, -1, -1, charange_29 },
    { "30", 6, 32, 99, -1, charange_30 },
    { "31", 6, 32, 99, -1, charange_31 },
    { "32", 6, 32, 99, -1, charange_32 },
    { "01", -1, -1, 0, -1, charange_01 },
    { "02", -1, -1, 0, -1, charange_01 },
    { "03", -1, -1, 0, -1, charange_01 },
    { "04", -1, -1, 0, -1, charange_01 },
    { "05", -1, -1, 0, -1, charange_01 },
    { "06", -1, -1, 0, -1, charange_01 },
    { "07", -1, -1, 0, -1, charange_01 },
    { "08", -1, -1, 0, -1, charange_01 },
    { "01", -1, 0, 0, -1, charange_01 },
    { "02", -1, 0, 0, -1, charange_01 },
    { "03", -1, 0, 0, -1, charange_01 },
    { "04", -1, 0, 0, -1, charange_01 },
    { "05", -1, 0, 0, -1, charange_01 },
    { "06", -1, 0, 0, -1, charange_01 },
    { "07", -1, 0, 0, -1, charange_01 },
    { "08", -1, 0, 0, -1, charange_01 },
    { "09", -1, 0, 0, -1, charange_01 },
    { "10", -1, 0, 0, -1, charange_01 },
    { "11", -1, 0, 0, -1, charange_01 },
    { "12", -1, 0, 0, -1, charange_01 },
    { "13", -1, 0, 0, -1, charange_01 },
    { "14", -1, 0, 0, -1, charange_01 },
    { "15", -1, 0, 0, -1, charange_01 },
    { "16", -1, 0, 0, -1, charange_01 },
    { "17", -1, 0, 0, -1, charange_01 },
    { "18", -1, 0, 0, -1, charange_01 },
    { "19", -1, 0, 0, -1, charange_01 },
    { "20", -1, 0, 0, -1, charange_01 },
    { "21", -1, 0, 0, -1, charange_01 },
    { "22", -1, 0, 0, -1, charange_01 },
    { "23", -1, 0, 0, -1, charange_01 },
    { "24", -1, 0, 0, -1, charange_01 },
    { "25", -1, 0, 0, -1, charange_01 },
    { "26", -1, 0, 0, -1, charange_01 },
    { "27", -1, 0, 0, -1, charange_01 },
    { "28", -1, 0, 0, -1, charange_01 },
    { "29", -1, 0, 0, -1, charange_01 },
    { "30", -1, 0, 0, -1, charange_01 },
    { "31", -1, 0, 0, -1, charange_01 },
    { "32", -1, 0, 0, -1, charange_01 },
    { "33", -1, 0, 0, -1, charange_01 },
    { "34", -1, 0, 0, -1, charange_01 },
    { "35", -1, 0, 0, -1, charange_01 },
    { "36", -1, 0, 0, -1, charange_01 },
    { "37", -1, 0, 0, -1, charange_01 },
    { "38", -1, 0, 0, -1, charange_01 },
    { "39", -1, 0, 0, -1, charange_01 },
    { "40", -1, 0, 0, -1, charange_01 },
    { "41", -1, 0, 0, -1, charange_01 },
    { "42", -1, 0, 0, -1, charange_01 },
    { "01", -1, 0, -1, -1, charange_01 },
    { "02", -1, 0, -1, -1, charange_01 },
    { "03", -1, 0, -1, -1, charange_01 },
    { "04", -1, 0, -1, -1, charange_01 },
    { "05", -1, 0, -1, -1, charange_01 },
    { "06", -1, 0, -1, -1, charange_01 },
    { "07", -1, 0, -1, -1, charange_01 },
    { "08", -1, 0, -1, -1, charange_01 },
    { "09", -1, 0, -1, -1, charange_01 },
    { "10", -1, 0, -1, -1, charange_01 },
    { "11", -1, 0, -1, -1, charange_01 },
    { "12", -1, 0, -1, -1, charange_01 },
    { "13", -1, 0, -1, -1, charange_01 },
    { "14", -1, 0, -1, -1, charange_01 },
    { "15", -1, 0, -1, -1, charange_01 },
    { "16", -1, 0, -1, -1, charange_01 },
};
extern int SubJob;
extern int SubJobCount;
extern char *ThumbnailFile;
extern char *GalleryFile;
extern int gal_cursor;
extern int gal_nextjob;
extern float gal_zoomsize;
extern int gal_x;
extern int gal_y;
extern int gal_view;
extern int gal_x_hos[8];
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void FadeInit(void);
void InitCancelFlag(void);
void PXF_SpriteClear(void);
void dcfont_init(void);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
char *FileReadSelect(int fid, int chan);
void Init_Game_Info(void);
void JobNext(void);
void heap_clear(void);
void nlSetBackColor(int c1, int c2, int c3);
int FileReadCheck(void);
void FontDraw(char *str, int x, int y, float z, int alfa);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void debugf(char *format, ...);
int RequestWin(int id);
void prac_mark_disp_gallery(void);
extern CHARACTER_WORK *PlayerWork;
extern int BattleStatus;
extern int PLAYER_HITPOINTMAX;
extern unsigned int Pause;
extern int EnterFightJob;
int SetLimitStatus(void);
void Change_ScreenScale(int i);
int Cr_Request_Start(int side);
int Credit_Init(void);
int Enter_Stage_Bg(void);
void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu);
int IsFadeEnd(void);
void JobChange(int job);
void MemoryClose(void);
void Rq_Fadeout(void);
void SetCPUCharacterFlag(int playerside, int flag);
void SetFadeSpeed(int frame);
int iotrg_chk(int stat);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
int IST_Load(int *texadr, int sprid, int palid);
void LinkAddrTb(u_int *addr);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
static void gallery_mode_init(void);
void GalleryInit(void);
void GalleryLoad(void);
static void gallery_mode_disp(HEADWORK *hw, int pos);
void gal_select_mode(void);
void gal_fullscreen_keycheck(void);
void put_thumbnail(void);
void EasySpriteDrawFullScreen(int id, int x, int y, float zm, float trnsl);
void EasySpriteDrawFullScreenEX(int id, int x, int y, float zm);
static void Gallery_Main_Init(char *file, int base);
void GalleryMain(void);
void gallery_init(void);
static void gallery_result_disp(void);
static void gallery_bonus_disp(void);
static void gallery_bonus_set(void);
void gallery_story_set(int no);
static void gallery_fade_bg(void);
static void gallery_mode_exit(void);
void gallery_status_chk(void);
static void gallery_game_init(int no, int pp, int pc);
static void gallery_info_disp(int no);
static int gallery_reset(int frm);
int GalleryRetry_Execute(int frm);
static int gallery_restart(int sw);
static void disp_gallery_retry(HEADWORK *hw, int pos);
static void operate_gallery_retry(int *pos);
void Exit_GalleryRetry(void);
void gallery_retry_select(void);
void gallery_retry_title(void);
void SetFadeEndCommand(int cmd);
void sound_alloff(void);
void gFadeValSet(float val);
void BGM_Free(void);
void objkill_winmrk(void);
void EliminateRoundEnd(void);
void ResultInitial(void);
void font_entry(int side, char *str);
void SetScreenFlag(int flag);
void PlayerParaInit(void);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
void BattleTimeInit(void);
void MaySHPOrg(void);
void MayBack(int a);
void ggx_bgm_start(void);
void BeforeFight_Init(void);
void Rq_Fadein(void);
extern char *ChName[];
extern int winner_mode;
void SetWinnerFlag(void);
void ChaseInit(void);
void KakuseiFinishInit(void);
void init_davol(void);
void ClearScore(int side);
int ReqSongOut(int outtime, int mval);

static void gallery_mode_init(void)
{
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    PXF_SpriteClear();
    FadeInit();
    dcfont_init();
    CockpitLoadInit();
    CreditFontLoad(20);
    InitCancelFlag();
    g_info.LoadPlayerID[0] = -1;
    UseCharacter1p2p[0] = 0;
    UseCharacter1p2p[1] = 0;
    g_info.LoadPlayerID[1] = -1;
}

void GalleryInit(void)
{
    int i;

    Init_Game_Info();
    gallery_mode_init();
    nlSetBackColor(0, 0, 0);
    gal_cursor = 0;
    gal_nextjob = 0;
    SubJob = 0;
    SubJobCount = 0;
    for (i = 0; i < 128; i++) {
        gl_info->start[i] = userinfo.gstart[i];
        gl_info->check[i] = userinfo.gcheck[i];
    }
    if (gl_info->start[gl_info->no] == 0) {
        gl_info->no = 0;
    }
    heap_clear();
    ThumbnailFile = FileReadSelect(0x98, 1);
    JobNext();
}

void GalleryLoad(void)
{
    if (FileReadCheck()) {
        JobNext();
    } else if (TotalCounter & 0x20) {
        FontDraw("NOW LOADING", 280, 240, 200.0f, 255);
    }
}

static void gallery_mode_disp(HEADWORK *hw, int pos)
{
    int i;
    int alpha;

    FontDrawDC("GALLERY MODE", 32.0f, 40.0f, 4.0f, 1, 0);
    for (i = 0; i < 5; i++) {
        alpha = (pos == i) ? 1 : 160;
        if (i == 1 && gl_info->check[gl_info->no] == 0 && (charange_all[gl_info->no].star != -1 || charange_all[gl_info->no].level != -1)) {
            alpha = 240;
        }
        if (i == 0 && charange_all[gl_info->no].star == -1) {
            alpha = 240;
        }
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 4.0f, alpha, 1);
    }
    gallery_bonus_set();
}

void gal_select_mode(void)
{
    int pad;
    int i;

    if ((pad = iodat_chk(0x1000)) != 0) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            if (--gal_cursor < 0) {
                gal_cursor = 4;
            }
            if (gal_cursor == 1 && gl_info->check[gl_info->no] == 0 && (charange_all[gl_info->no].star != -1 || charange_all[gl_info->no].level != -1)) {
                gal_cursor = 0;
            }
            if (gal_cursor == 0 && charange_all[gl_info->no].star == -1) {
                gal_cursor = 4;
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x4000)) != 0) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            if (++gal_cursor >= 5) {
                gal_cursor = 0;
            }
            if (gal_cursor == 0 && charange_all[gl_info->no].star == -1) {
                gal_cursor = 1;
            }
            if (gal_cursor == 1 && gl_info->check[gl_info->no] == 0 && (charange_all[gl_info->no].star != -1 || charange_all[gl_info->no].level != -1)) {
                gal_cursor = 2;
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x8000)) != 0) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            for (i = 7; i >= 0; i--) {
                gal_x_hos[i] = i * 4 + 32;
            }
            sound_request(0x37);
            gl_info->no--;
            while (gl_info->start[gl_info->no] == 0) {
                if (--gl_info->no < 0) {
                    gl_info->no = 97;
                }
            }
            if (gl_info->no < 0) {
                gl_info->no = 97;
            }
            if (gal_cursor == 0 && charange_all[gl_info->no].star == -1) {
                gal_cursor = 1;
            }
            if (gal_cursor == 1 && gl_info->check[gl_info->no] == 0 && (charange_all[gl_info->no].star != -1 || charange_all[gl_info->no].level != -1)) {
                gal_cursor = 2;
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x2000)) != 0) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            for (i = 7; i >= 0; i--) {
                gal_x_hos[i] = i * 4 + 32;
            }
            sound_request(0x37);
            gl_info->no++;
            while (gl_info->start[gl_info->no] == 0) {
                if (++gl_info->no >= 98) {
                    gl_info->no = 0;
                }
            }
            if (gl_info->no >= 98) {
                gl_info->no = 0;
            }
            if (gal_cursor == 0 && charange_all[gl_info->no].star == -1) {
                gal_cursor = 1;
            }
            if (gal_cursor == 1 && gl_info->check[gl_info->no] == 0 && (charange_all[gl_info->no].star != -1 || charange_all[gl_info->no].level != -1)) {
                gal_cursor = 2;
            }
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
}

void gal_fullscreen_keycheck(void)
{
    if (iodat_chk(0x1000)) {
        gal_y -= 4;
    }
    if (iodat_chk(0x4000)) {
        gal_y += 4;
    }
    if (iodat_chk(0x8000)) {
        gal_x -= 4;
    }
    if (iodat_chk(0x2000)) {
        gal_x += 4;
    }
    if (iodat_chk(8)) {
        gal_zoomsize += 0.02f;
    }
    if (iodat_chk(4)) {
        gal_zoomsize -= 0.02f;
    }
    if (gal_zoomsize > 2.0f) {
        gal_zoomsize = 2.0f;
    }
    if (gal_zoomsize < 0.5f) {
        gal_zoomsize = 0.5f;
    }
    if (gal_y < 240.0f - gal_zoomsize * 256.0f) {
        gal_y = 240.0f - gal_zoomsize * 256.0f;
    }
    if (gal_y > gal_zoomsize * 256.0f + 240.0f) {
        gal_y = gal_zoomsize * 256.0f + 240.0f;
    }
    if (gal_x < 320.0f - gal_zoomsize * 256.0f) {
        gal_x = 320.0f - gal_zoomsize * 256.0f;
    }
    if (gal_x > gal_zoomsize * 256.0f + 320.0f) {
        gal_x = gal_zoomsize * 256.0f + 320.0f;
    }
    if ((pads[0].trg | pads[1].trg) & 0x20) {
        if (gal_view) {
            gal_view = 0;
        } else {
            gal_view = 1;
        }
    }
    if ((pads[0].trg | pads[1].trg) & 0x40) {
        SubJobCount = 0;
        SubJob++;
    }
}

void put_thumbnail(void)
{
    int i;
    int j;
    int x_hos;

    if (gl_info->check[gl_info->no] != 0 || (charange_all[gl_info->no].star == -1 && charange_all[gl_info->no].level == -1)) {
        for (i = 0; i < 8; i++) {
            x_hos = 0;
            if (gal_x_hos[i] != 0) {
                for (j = 0; j <= gal_x_hos[i]; j++) {
                    x_hos += j;
                }
                gal_x_hos[i]--;
            }
            if (x_hos > 300) {
                x_hos = 300;
            }
            EasySpriteDrawEX(gl_info->no + 32, x_hos + 352, 80, 1.0f, 5, (8 - i) * 0.125);
        }
    }
    EasySpriteDrawEX(gl_info->no + 130, 70, 320, 1.0f, 5, 0.75f);
}

void EasySpriteDrawFullScreen(int id, int x, int y, float zm, float trnsl)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = 1.0f;
    sprite.zm_x = zm;
    sprite.zm_y = zm;
    sprite.u0 = 0;
    sprite.u1 = 1.0f;
    sprite.v0 = 0;
    sprite.v1 = 1.0f;
    sprite.listType = 1;
    sprite.attr = 10;
    sprite.ang = 0;
    sprite.trnsl = trnsl;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void EasySpriteDrawFullScreenEX(int id, int x, int y, float zm)
{
    int i;
    int j;
    int x_hos;

    if (gl_info->no >= 32 && gl_info->no < 40) {
        EasySpriteDrawEX(gl_info->no + 197, 0, 480, 20.0f, 525, 1.0f);
        polydraw_FBox(0, 0, 640.0f, 480.0f, 19.0f, 0x60000000);
    } else {
        EasySpriteDrawEX(228, 0, 480, 20.0f, 525, 1.0f);
    }
    for (i = 0; i < 8; i++) {
        x_hos = 0;
        if (gal_x_hos[i] != 0) {
            for (j = 0; j <= gal_x_hos[i]; j++) {
                x_hos += j;
            }
            gal_x_hos[i]--;
        }
        EasySpriteDrawFullScreen(237, gal_x, gal_y, gal_zoomsize * (x_hos + 1000) / 1000.0f, (8 - i) * 0.125);
    }
}

static void Gallery_Main_Init(char *file, int base)
{
    int i;

    LinkAddrTb((u_int *)file);
    for (i = 0; ((int *)file)[i] != 0; i++) {
        IST_Load((int *)file + i, base + i, -1);
    }
}

void GalleryMain(void)
{
    HEADWORK hw[5] = {
        { "CHALLENGE", 80.0f, 88.0f, 0, 0, 0 },
        { "VIEW", 80.0f, 120.0f, 0, 0, 0 },
        { "RESULT", 80.0f, 152.0f, 0, 0, 0 },
        { "BONUS", 80.0f, 184.0f, 0, 0, 0 },
        { "TITLE", 80.0f, 216.0f, 0, 0, 0 },
    };
    int pad;
    int pp = 0;
    int pc = 0;
    int i;

    switch (SubJob) {
    case 0:
        Gallery_Main_Init(ThumbnailFile, 32);
        SubJobCount = 0;
        SubJob++;
        for (i = 7; i >= 0; i--) {
            gal_x_hos[i] = i * 4 + 32;
        }
        break;
    case 1:
        gallery_mode_disp(hw, gal_cursor);
        polydraw_GBox(0, 0, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        gal_select_mode();
        put_thumbnail();
        gallery_info_disp(gl_info->no);
        if ((pad = iotrg_chk(0x20)) != 0) {
            sound_request(0x39);
            switch (gal_cursor) {
            case 0:
                SubJob = 20;
                SubJobCount = 0;
                debugf("Gallery Start Mode JOB_GAME_I\n");
                Credit_Init();
                if (pad == 1) {
                    pp = 0;
                    pc = 1;
                } else if (pad == 2) {
                    pp = 1;
                    pc = 0;
                }
                g_info.mode = (1 << pp) | 0x8000;
                g_info.ModeFlag = 1;
                g_info.Player = pp;
                gallery_game_init(gl_info->no, pp, pc);
                UseCharacter1p2p[pp] = gl_info->id[pp];
                UseCharacter1p2p[pc] = gl_info->id[pc];
                EntrySelectCharacter(pp, gl_info->id[pp], 0, 0);
                if (gl_info->pow != -1) {
                    EntrySelectCharacter(pc, gl_info->id[pc], 9, 0);
                } else if (gl_info->id[pp] == gl_info->id[pc]) {
                    EntrySelectCharacter(pc, gl_info->id[pc], 5, 0);
                } else {
                    EntrySelectCharacter(pc, gl_info->id[pc], 0, 0);
                }
                SetCPUCharacterFlag(pp, 0);
                if (gl_info->deku == 0) {
                    SetCPUCharacterFlag(pc, 1);
                } else {
                    SetCPUCharacterFlag(pc, 0);
                }
                g_info.PlayerSelected[pp] = 1;
                g_info.PlayerSelected[pc] = 1;
                Cr_Request_Start(pp);
                Change_ScreenScale(1);
                Enter_Stage_Bg();
                sc.rannyu = 0;
                break;
            case 1:
                SubJob++;
                break;
            case 2:
                SubJobCount = 0;
                SubJob = 30;
                break;
            case 3:
                SubJobCount = 0;
                SubJob = 40;
                break;
            case 4:
                if (userinfo.autosave) {
                    gal_nextjob = 45;
                } else {
                    gal_nextjob = 3;
                }
                SubJobCount = 0;
                SubJob = 10;
                break;
            }
        } else if ((pads[0].trg | pads[1].trg) & 0x40) {
            sound_request(0x3B);
            if (userinfo.autosave) {
                gal_nextjob = 45;
            } else {
                gal_nextjob = 3;
            }
            SubJobCount = 0;
            SubJob = 10;
        }
        break;
    case 2:
        if (SubJobCount == 0) {
            gallery_mode_disp(hw, gal_cursor);
            put_thumbnail();
            gallery_fade_bg();
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJobCount++;
            gal_x = 320;
            gal_y = 240;
            gal_zoomsize = 1.0f;
            gal_view = 1;
        } else if (!IsFadeEnd()) {
            gallery_mode_disp(hw, gal_cursor);
            put_thumbnail();
            gallery_fade_bg();
        } else {
            GalleryFile = FileReadSelect(gl_info->no + 0x36, 0);
            SubJob++;
        }
        break;
    case 3:
        if (FileReadCheck()) {
            Gallery_Main_Init(GalleryFile, 0xED);
            for (i = 7; i >= 0; i--) {
                gal_x_hos[i] = i * 4 + 32;
            }
            SubJob++;
        }
        break;
    case 4:
        gal_fullscreen_keycheck();
        EasySpriteDrawFullScreenEX(0xED, gal_x, gal_y, gal_zoomsize);
        if (gal_view) {
            polydraw_FBox(0, 312.0f, 640.0f, 468.0f, 1.0f, 0xA0000000);
            if (gl_info->no == 3 || gl_info->no == 11 || gl_info->no == 19) {
                polydraw_FBox(0, 16.0f, 640.0f, 56.0f, 1.0f, 0xA0000000);
            }
            EasySpriteDrawEX(0xEE, 70, 304, 1.0f, 5, 0.75f);
            FontDraw("w-COMMENT ON/OFF  x-END  hbdf-MOVE", 112, 416, 1.0f, 255);
            FontDraw("R1-ZOOM IN  L1-ZOOM OUT", 112, 434, 1.0f, 255);
            if (gl_info->no == 3) {
                FontDraw("PASSWORD FOR GGP2 - KDOKBNZY -", 136, 32, 1.0f, 255);
            } else if (gl_info->no == 11) {
                FontDraw("PASSWORD FOR GGP2 - ADROPOUT -", 136, 32, 1.0f, 255);
            } else if (gl_info->no == 19) {
                FontDraw("PASSWORD FOR GGP2 - BLACKMAS -", 136, 32, 1.0f, 255);
            }
        }
        break;
    case 5:
        if (SubJobCount == 0) {
            EasySpriteDrawFullScreenEX(0xED, gal_x, gal_y, gal_zoomsize);
            gallery_fade_bg();
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJobCount++;
        } else if (!IsFadeEnd()) {
            EasySpriteDrawFullScreenEX(0xED, gal_x, gal_y, gal_zoomsize);
            gallery_fade_bg();
        } else {
            SubJob = 1;
            SubJobCount = 0;
            for (i = 7; i >= 0; i--) {
                gal_x_hos[i] = 0;
            }
        }
        break;
    case 10:
        if (SubJobCount == 0) {
            gallery_mode_disp(hw, gal_cursor);
            gallery_fade_bg();
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJobCount++;
        } else if (IsFadeEnd()) {
            if (gal_nextjob == 45) {
                sc.grpvar[1] = -1;
            }
            JobChange(gal_nextjob);
            gallery_fade_bg();
        } else {
            gallery_mode_disp(hw, gal_cursor);
            gallery_fade_bg();
        }
        break;
    case 20:
        if (SubJobCount == 0) {
            gallery_mode_disp(hw, gal_cursor);
            gallery_fade_bg();
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJobCount++;
        } else if (!IsFadeEnd()) {
            gallery_mode_disp(hw, gal_cursor);
            gallery_fade_bg();
        } else {
#ifndef VERSION_JP
            MemoryClose();
#endif
            JobChange(21);
            gallery_mode_exit();
        }
        break;
    case 30:
        gallery_result_disp();
        polydraw_GBox(0, 0, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        if ((pads[0].trg | pads[1].trg) & 0x40) {
            SubJob = 1;
            SubJobCount = 0;
            for (i = 7; i >= 0; i--) {
                gal_x_hos[i] = 0;
            }
        }
        break;
    case 40:
        gallery_bonus_disp();
        polydraw_GBox(0, 0, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        if ((pads[0].trg | pads[1].trg) & 0x40) {
            SubJob = 1;
            SubJobCount = 0;
            for (i = 7; i >= 0; i--) {
                gal_x_hos[i] = 0;
            }
        }
        break;
    }
}

void gallery_init(void)
{
    int side;

    if (gl_info == 0) {
        gl_info = mmalloc(sizeof(CHARANGE_CTRL), "gl_info");
    }
    for (side = 0; side < 2; side++) {
        g_info.LoadPlayerID[side] = -1;
        UseCharacter1p2p[side] = 0;
        InitGameinfo_ByGameOver(side);
    }
    StageNo = 0;
}

static void gallery_result_disp(void)
{
    HEADWORK hw[6] = {
        { "EASY", 64.0f, 96.0f, 0, 0, 0 },
        { "NORMAL", 64.0f, 136.0f, 0, 0, 0 },
        { "HARD", 64.0f, 176.0f, 0, 0, 0 },
        { "PETIT", 64.0f, 216.0f, 0, 0, 0 },
        { "STORY", 64.0f, 256.0f, 0, 0, 0 },
        { "ENDING", 64.0f, 376.0f, 0, 0, 0 },
    };
    HEADWORK hw2[5] = {
        { "EASY", 64.0f, 96.0f, 0, 0, 0 },
        { "NORMAL", 64.0f, 144.0f, 0, 0, 0 },
        { "HARD", 64.0f, 192.0f, 0, 0, 0 },
        { "STORY", 64.0f, 240.0f, 0, 0, 0 },
        { "ENDING", 64.0f, 384.0f, 0, 0, 0 },
    };
    int i;
    int j;
    int alpha;

    FontDrawDC("GALLERY MODE", 32.0f, 40.0f, 4.0f, 1, 0);
    if (gl_info->start[24] != 0) {
        for (i = 0; i < 6; i++) {
            FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 4.0f, 1, 1);
            if (i == 4) {
                for (j = 0; j < 42; j++) {
                    if (gl_info->check[(i + 1) * 8 + j]) alpha = 0xff; else alpha = 0x50;
                    FontDraw(charange_all[(i + 1) * 8 + j].no, hw[i].xp + 144.0f + (j % 10) * 36, hw[i].yp + (j / 10) * 22 + 8.0f, 4.0f, alpha);
                }
            } else if (i == 5) {
for (j = 0; j < 16; j++) { if (gl_info->check[i * 16 + j + 2]) alpha = 0xff; else alpha = 0x50; FontDraw(charange_all[i * 16 + j + 2].no, hw[i].xp + 144.0f + (j % 10) * 36, hw[i].yp + (j / 10) * 22 + 8.0f, 4.0f, alpha); }
            } else {
                for (j = 0; j < 8; j++) {
                    if (gl_info->check[i * 8 + j]) alpha = 0xff; else alpha = 0x50;
                    FontDraw(charange_all[i * 8 + j].no, hw[i].xp + 144.0f + j * 36, hw[i].yp + 8.0f, 4.0f, alpha);
                }
            }
        }
    } else {
        for (i = 0; i < 5; i++) {
            FontDrawDC(hw2[i].head, hw2[i].xp, hw2[i].yp, 4.0f, 1, 1);
            if (i == 3) {
                for (j = 0; j < 42; j++) {
                    if (gl_info->check[(i + 2) * 8 + j]) alpha = 0xff; else alpha = 0x50;
                    FontDraw(charange_all[(i + 2) * 8 + j].no, hw2[i].xp + 144.0f + (j % 10) * 36, hw2[i].yp + (j / 10) * 22 + 8.0f, 4.0f, alpha);
                }
            } else if (i == 4) {
for (j = 0; j < 16; j++) { if (gl_info->check[(i + 1) * 16 + j + 2]) alpha = 0xff; else alpha = 0x50; FontDraw(charange_all[(i + 1) * 16 + j + 2].no, hw2[i].xp + 144.0f + (j % 10) * 36, hw2[i].yp + (j / 10) * 22 + 8.0f, 4.0f, alpha); }
            } else {
                for (j = 0; j < 8; j++) {
                    if (gl_info->check[i * 8 + j]) alpha = 0xff; else alpha = 0x50;
                    FontDraw(charange_all[i * 8 + j].no, hw2[i].xp + 144.0f + j * 36, hw2[i].yp + 8.0f, 4.0f, alpha);
                }
            }
        }
    }
}

static void gallery_bonus_disp(void)
{
    int i;
    int alpha;
    int sum = 0;
    int num[6] = { 1, 3, 7, 12, 18, 24 };
    HEADWORK hw[6] = {
        { "01", 64.0f, 96.0f, 0, 0, 0 },
        { "03", 64.0f, 144.0f, 0, 0, 0 },
        { "07", 64.0f, 192.0f, 0, 0, 0 },
        { "12", 64.0f, 240.0f, 0, 0, 0 },
        { "18", 64.0f, 288.0f, 0, 0, 0 },
        { "24", 64.0f, 336.0f, 0, 0, 0 },
    };
    HEADWORK hw2[6] = {
        { "TESTAMENT", 128.0f, 96.0f, 0, 0, 0 },
        { "DIZZY", 128.0f, 144.0f, 0, 0, 0 },
        { "GG MODE", 128.0f, 192.0f, 0, 0, 0 },
        { "KLIFF", 128.0f, 240.0f, 0, 0, 0 },
        { "JUSTICE", 128.0f, 288.0f, 0, 0, 0 },
        { "ALL COLOR", 128.0f, 336.0f, 0, 0, 0 },
    };

    FontDrawDC("GALLERY MODE", 32.0f, 40.0f, 4.0f, 1, 0);
    for (i = 0; i < 24; i++) {
        if (gl_info->check[i]) sum++;
    }
    for (i = 0; i < 6; i++) {
        if (sum >= num[i]) alpha = 1; else alpha = 0xa0;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 4.0f, alpha, 1);
        if (sum >= num[i]) {
            FontDrawDC(hw2[i].head, hw2[i].xp, hw2[i].yp, 4.0f, alpha, 1);
        }
    }
}

void gallery_bonus_set(void) {
    int i;
    int j;
    int sum;
    int num[6] = { 1, 3, 7, 12, 18, 24 };
    sum = 0;
    for (i = 0; i < 24; i++) {
        if (gl_info->check[i]) sum++;
    }
    for (i = 0; i < 6; i++) {
        if (sum < num[i]) continue;
        switch (i) {
        case 0:
            if (userinfo.phase < 1) userinfo.phase = 1;
            SetLimitStatus();
            break;
        case 1:
            if (userinfo.phase < 2) userinfo.phase = 2;
            SetLimitStatus();
            break;
        case 2:
            if (userinfo.phase < 3) userinfo.phase = 3;
            SetLimitStatus();
            break;
        case 3:
            if (userinfo.phase < 4) userinfo.phase = 4;
            SetLimitStatus();
            break;
        case 4:
            if (userinfo.phase < 5) userinfo.phase = 5;
            SetLimitStatus();
            break;
        case 5:
            for (j = 0; j < 18; j++) {
                userinfo.usecol[j] = 0x7ff;
            }
            break;
        }
    }
    SetLimitStatus();
}

void gallery_story_set(int no) {
    int i;
    int j;
    int sum;
    int num[4] = { 1, 16, 32, 42 };
    userinfo.gcheck[no + 40] = 1;
    sum = 0;
    for (i = 0; i < 42; i++) {
        if (userinfo.gcheck[40 + i]) sum++;
    }
    for (i = 0; i < 4; i++) {
        if (sum < num[i]) continue;
        switch (i) {
        case 0:
            if (userinfo.phase < 2) userinfo.phase = 2;
            SetLimitStatus();
            break;
        case 1:
            if (userinfo.phase < 3) userinfo.phase = 3;
            SetLimitStatus();
            break;
        case 2:
            if (userinfo.phase < 5) userinfo.phase = 5;
            SetLimitStatus();
            for (j = 0; j < 18; j++) {
                userinfo.usecol[j] |= 0x200;
            }
            break;
        case 3:
            for (j = 0; j < 18; j++) { userinfo.usecol[j] = 0x7ff; }
            break;
        }
    }
    SetLimitStatus();
}

void gallery_fade_bg(void) {
    static int num = 0;
    static int frame = 0;
    int bg[3];
    int i;
    int col[3] = { 0xff400000, 0xff200000, 0xff000000 };
    for (i = 0; i < 3; i++) {
        bg[i] = ((col[i] & 0xffffff) >> 16) - num - frame;
        if (bg[i] < 0) bg[i] = 0;
        bg[i] = (col[i] & 0xff00ffff) | (bg[i] << 16);
    }
    nlSetBackColor(bg[0], bg[1], bg[2]);
    if (frame >= 2) {
        frame = 0;
        num += 8;
    } else {
        frame++;
    }
}

static void gallery_mode_exit(void)
{
    nlSetBackColor(0, 0, 0);
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
}

void gallery_status_chk(void)
{
    int i;

    if (PlayerWork[gl_info->ply].ply->CPU) {
        PlayerWork[gl_info->ply].ply->CPU = 0;
    }
    if (!(gl_info->ply ? &PlayerWork[0] : &PlayerWork[1])->ply->CPU && !gl_info->deku) {
        (gl_info->ply ? &PlayerWork[0] : &PlayerWork[1])->ply->CPU = 1;
    } else if ((gl_info->ply ? &PlayerWork[0] : &PlayerWork[1])->ply->CPU && gl_info->deku) {
        (gl_info->ply ? &PlayerWork[0] : &PlayerWork[1])->ply->CPU = 0;
    }
    if (sc.grpvar[0] == 0) {
        prac_mark_disp_gallery();
    }
    if (Pause == 0) {
        for (i = 0; i < 2; i++) {
            if (gl_info->undead[i]) {
                if (PlayerWork[i].HitPoint == 0) {
                    PlayerWork[i].HitPoint = 1;
                }
                if (!(PlayerWork[i].actst & 0x20) && PlayerWork[i].HitPoint < PLAYER_HITPOINTMAX) {
                    PlayerWork[i].HitPoint += 4;
                }
                if (PlayerWork[i].HitPoint > PLAYER_HITPOINTMAX) {
                    PlayerWork[i].HitPoint = PLAYER_HITPOINTMAX;
                }
            } else if (gl_info->rejene[i] && PlayerWork[i].HitPoint > 0) {
                if (BattleStatus == 2 && Pause == 0 && TotalCounter % 10 == 0 && PlayerWork[i].HitPoint < PLAYER_HITPOINTMAX && !(PlayerWork[i].actst & 0x20)) {
                    PlayerWork[i].HitPoint++;
                }
            }
            if (gl_info->poison[i]) {
                PlayerWork[i].ply->Poison = 0x40;
            }
            if (BattleStatus == 1) {
                if (gl_info->tg[i] && EnterFightJob < 10) {
                    PlayerWork[i].ply->Tension = gl_info->tg[i];
                }
            }
            if (gl_info->tension[i] > 0) {
                if (PlayerWork[i].ply->OverKillFlag == 2) {
                    PlayerWork[i].ply->OverKillFlag = 0;
                }
                PlayerWork[i].ply->Tension = gl_info->tg[i];
            } else if (gl_info->tension[i] == -1) {
                PlayerWork[i].ply->OverKillFlag = 2;
            }
            if (i == gl_info->ply) {
                if (gl_info->close[i][0]) {
                    PlayerWork[i].ply->ArtsSeal = 0x40;
                }
                if (gl_info->close[i][1]) {
                    PlayerWork[i].ply->JumpSeal = 0x40;
                }
                if (gl_info->clear & 2) {
                    if ((i ? &PlayerWork[0] : &PlayerWork[1])->ply->HitCount >= gl_info->num) {
                        RequestWin(i == 0);
                    }
                }
            }
        }
    }
    if (gl_info->reset) {
        gl_info->reset = gallery_reset(gl_info->reset);
    }
}

static void gallery_game_init(int no, int pp, int pc)
{
    CHARANGE *charange_temp;

    gl_info->ply = pp;
    charange_temp = charange_all[no].charange;
    gl_info->id[pp] = charange_temp->pid & 0xFF;
    gl_info->id[pc] = charange_temp->cid & 0xFF;
    if (charange_temp->pid & 0x1000) {
        gl_info->ggmode[pp] = 2;
    } else {
        gl_info->ggmode[pp] = 0;
    }
    if (charange_temp->cid & 0x1000) {
        gl_info->ggmode[pc] = 2;
    } else {
        gl_info->ggmode[pc] = 0;
    }
    if (charange_temp->pid & 0x100) {
        gl_info->undead[pp] = 1;
    } else {
        gl_info->undead[pp] = 0;
    }
    if (charange_temp->cid & 0x100) {
        gl_info->undead[pc] = 1;
    } else {
        gl_info->undead[pc] = 0;
    }
    if (charange_temp->pvital) {
        gl_info->vital[pp] = charange_temp->pvital * PLAYER_HITPOINTMAX / 8;
    } else {
        gl_info->vital[pp] = 1;
    }
    if (charange_temp->cvital) {
        gl_info->vital[pc] = charange_temp->cvital * PLAYER_HITPOINTMAX / 8;
    } else {
        gl_info->vital[pc] = 1;
    }
    if (charange_temp->ptg) {
        gl_info->tg[pp] = charange_temp->ptg * 10000 / 128;
    } else {
        gl_info->tg[pp] = 0;
    }
    if (charange_temp->ctg) {
        gl_info->tg[pc] = charange_temp->ctg * 10000 / 128;
    } else {
        gl_info->tg[pc] = 0;
    }
    if (charange_temp->pgg) {
        gl_info->gg[pp] = charange_temp->pgg;
    } else {
        gl_info->gg[pp] = 0;
    }
    if (charange_temp->cgg) {
        gl_info->gg[pc] = charange_temp->cgg;
    } else {
        gl_info->gg[pc] = 0;
    }
    if (charange_temp->state & 1) {
        gl_info->rejene[pp] = 1;
    } else {
        gl_info->rejene[pp] = 0;
    }
    if (charange_temp->state & 2) {
        gl_info->rejene[pc] = 1;
    } else {
        gl_info->rejene[pc] = 0;
    }
    if (charange_temp->state & 4) {
        gl_info->poison[pp] = 1;
    } else {
        gl_info->poison[pp] = 0;
    }
    if (charange_temp->state & 8) {
        gl_info->poison[pc] = 1;
    } else {
        gl_info->poison[pc] = 0;
    }
    if (charange_temp->state & 0x10) {
        gl_info->tension[pp] = 10000;
    } else if (charange_temp->state & 0x40) {
        gl_info->tension[pp] = -1;
    } else {
        gl_info->tension[pp] = 0;
    }
    if (charange_temp->state & 0x20) {
        gl_info->tension[pc] = 10000;
    } else if (charange_temp->state & 0x80) {
        gl_info->tension[pc] = -1;
    } else {
        gl_info->tension[pc] = 0;
    }
    gl_info->attack[pp][0] = 1;
    gl_info->attack[pp][1] = 1;
    gl_info->attack[pp][2] = 1;
    gl_info->attack[pp][3] = 1;
    if (charange_temp->state & 0x100) {
        gl_info->attack[pc][0] = 0;
    } else {
        gl_info->attack[pc][0] = 1;
    }
    if (charange_temp->state & 0x200) {
        gl_info->attack[pc][1] = 0;
    } else {
        gl_info->attack[pc][1] = 1;
    }
    if (charange_temp->state & 0x400) {
        gl_info->attack[pc][2] = 0;
    } else {
        gl_info->attack[pc][2] = 1;
    }
    if (charange_temp->state & 0x800) {
        gl_info->attack[pc][3] = 0;
    } else {
        gl_info->attack[pc][3] = 1;
    }
    gl_info->close[pc][0] = 0;
    gl_info->close[pc][1] = 0;
    if (charange_temp->state & 0x1000) {
        gl_info->close[pp][0] = 1;
    } else {
        gl_info->close[pp][0] = 0;
    }
    if (charange_temp->state & 0x2000) {
        gl_info->close[pp][1] = 1;
    } else {
        gl_info->close[pp][1] = 0;
    }
    if (charange_temp->state & 0x8000) {
        gl_info->deku = 1;
    } else {
        gl_info->deku = 0;
    }
    gl_info->clear = charange_temp->clear;
    gl_info->num = charange_temp->num;
    gl_info->pow = charange_temp->pow;
    gl_info->level = charange_all[no].level;
    gl_info->time = charange_all[no].time;
    gl_info->combo = charange_all[no].combo;
    gl_info->reset = 0;
    gl_info->win = 0;
}

static void gallery_info_disp(int no)
{
    CHARANGE *charange_temp;

    charange_temp = charange_all[no].charange;
    if (charange_all[no].star != -1) {
        FontDraw("    NO", 0x60, 0x100, 1.0f, 0xff);
        FontDraw(charange_all[no].no, 0xc0, 0x100, 1.0f, 0xff);
        FontDraw("PLAYER", 0x60, 0x114, 1.0f, 0xff);
        FontDraw(ChName[(u_char)charange_temp->pid - 1], 0xc0, 0x114, 1.0f, 0xff);
        FontDraw(" ENEMY", 0x60, 0x128, 1.0f, 0xff);
        FontDraw(ChName[(u_char)charange_temp->cid - 1], 0xc0, 0x128, 1.0f, 0xff);
        FontDraw(" LEVEL", 0x60, 0x13c, 1.0f, 0xff);
        if (gl_info->no >= 0 && gl_info->no < 8) {
            FontDraw("EASY", 0xc0, 0x13c, 1.0f, 0xff);
        } else if (gl_info->no >= 8 && gl_info->no < 16) {
            FontDraw("NORMAL", 0xc0, 0x13c, 1.0f, 0xff);
        } else if (gl_info->no >= 16 && gl_info->no < 24) {
            FontDraw("HARD", 0xc0, 0x13c, 1.0f, 0xff);
        } else if (gl_info->no >= 24 && gl_info->no < 32) {
            FontDraw("PETIT", 0xc0, 0x13c, 1.0f, 0xff);
        } else {
            FontDraw("UNKNOWN", 0xc0, 0x13c, 1.0f, 0xff);
        }
    } else if (charange_all[no].level == -1) {
        FontDraw("USER GALLERY", 0x60, 0x100, 1.0f, 0xff);
        FontDraw("    NO", 0x60, 0x128, 1.0f, 0xff);
        FontDraw(charange_all[no].no, 0xc0, 0x128, 1.0f, 0xff);
    } else if (charange_all[no].time == -1) {
        FontDraw("ENDING GALLERY", 0x60, 0x100, 1.0f, 0xff);
        FontDraw("    NO", 0x60, 0x128, 1.0f, 0xff);
        FontDraw(charange_all[no].no, 0xc0, 0x128, 1.0f, 0xff);
    } else {
        FontDraw("STORY GALLERY", 0x60, 0x100, 1.0f, 0xff);
        FontDraw("    NO", 0x60, 0x128, 1.0f, 0xff);
        FontDraw(charange_all[no].no, 0xc0, 0x128, 1.0f, 0xff);
    }
    no = 0; /* dead store: keeps the FontDraw tails as jal+b, no sibcalls */
}

static int gallery_reset(int frm)
{
    static int mode;

    if (frm == 1) {
        g_info.BattleControlFlag |= 4;
        FadeInit();
        SetWinnerFlag();
        mode = 30;
        winner_mode = (winner_mode | 1) & ~6;
        PlayerWork[0].Localid = 0;
        PlayerWork[1].Localid = 0;
        PlayerWork[0].ply->JumpFlag = 0;
        PlayerWork[1].ply->JumpFlag = 0;
        SetFadeSpeed(20);
        SetFadeEndCommand(0);
        Rq_Fadeout();
        return 2;
    }
    if (frm == 2) {
        sound_alloff();
        ReqSongOut(10, 0);
        return 3;
    }
    if (frm == 3) {
        if (!IsFadeEnd()) return 3;
        g_info.Player = gl_info->ply;
        ChaseInit();
        KakuseiFinishInit();
        nlSetBackColor(0, 0, 0);
        sound_alloff();
        init_davol();
        if (mode == 32) {
            gallery_restart(0);
            return 4;
        }
        return 5;
    }
    if (frm == 4) {
        if (!gallery_restart(1)) return 4;
    } else if (frm != 5) {
        return 0;
    }
        BGM_Free();
        ClearScore(0);
        ClearScore(1);
        JobChange(mode);
        g_info.BattleControlFlag &= ~4;
        return 0;
}

int GalleryRetry_Execute(int frm)
{
    static int alpha;
    static int pos;
    static int mode;
    HEADWORK hw[3] = {
        { "RETRY", 256.0f, 192.0f, 0, 0, 0 },
        { "SELECT", 256.0f, 232.0f, 0, 0, 0 },
        { "TITLE", 256.0f, 272.0f, 0, 0, 0 },
    };

    if (frm == 0) {
        g_info.BattleControlFlag |= 4;
        FadeInit();
        SetWinnerFlag();
        pos = 0;
        alpha = 0;
        return 1;
    }
    if (frm == 1) {
        if (alpha < 0xd0) {
            alpha += 0x20;
            if (alpha > 0xd0) alpha = 0xd0;
            polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 3.0f, alpha << 24);
            if (alpha == 0xd0) return 2;
            return 1;
        }
        return 2;
    }
    if (frm == 2) {
        disp_gallery_retry(hw, pos);
        if (!((pads[0].trg | pads[1].trg) & 0x820)) {
            operate_gallery_retry(&pos);
            return 2;
        }
        sound_request(0x39);
        switch (pos) {
        case 0:
            mode = 30;
            break;
        case 1:
            mode = 27;
            break;
        case 2:
            if (userinfo.autosave) mode = 45; else mode = 3;
            break;
        }
        winner_mode = (winner_mode | 1) & ~6;
        PlayerWork[0].Localid = 0;
        PlayerWork[1].Localid = 0;
        SetFadeSpeed(20);
        SetFadeEndCommand(0);
        Rq_Fadeout();
        return frm + 1;
    }
    if (frm == 3) {
        disp_gallery_retry(hw, pos);
        sound_alloff();
        ReqSongOut(10, 0);
        return 4;
    }
    if (frm == 4) {
        if (!IsFadeEnd()) {
            disp_gallery_retry(hw, pos);
            return 4;
        }
        nlSetBackColor(0, 0, 0);
        sound_alloff();
        init_davol();
        if (mode == 32) {
            gallery_restart(0);
            return 5;
        }
        return 6;
    }
    if (frm == 5) {
        if (!gallery_restart(1)) return 5;
    } else if (frm != 6) {
        return 0;
    }
    BGM_Free();
    ClearScore(0);
    ClearScore(1);
    if (mode == 45) sc.grpvar[1] = -1;
    JobChange(mode);
    g_info.BattleControlFlag &= ~4;
    return 0;
}

int gallery_restart(int sw) {
    static int mode;
    if (sw == 0) {
        mode = 0;
        objkill_winmrk();
        EliminateRoundEnd();
        ResultInitial();
        font_entry(-1, 0);
        SetScreenFlag(0);
        PlayerParaInit();
        SetActNo(PlayerWork, 0);
        SetActNo(PlayerWork + 1, 0);
        SetAnimeNo(PlayerWork, 0);
        SetAnimeNo(PlayerWork + 1, 0);
        BattleTimeInit();
        nlSetBackColor(0, 0, 0);
        if (StageNo == 5) {
            MaySHPOrg();
            MayBack(1);
        }
        SetFadeEndCommand(1);
        SetFadeSpeed(15);
        Rq_Fadein();
        return 0;
    }
    if (mode == 0) {
        if (IsFadeEnd()) mode = 1;
    } else {
        if (mode++ < 11) return 0;
        ggx_bgm_start();
        BeforeFight_Init();
        return 1;
    }
    return 0;
}

void disp_gallery_retry(HEADWORK *hw, int pos) {
    int i;
    int alpha;
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 3.0f, 0xd0000000);
    for (i = 0; i < 3; i++) {
        alpha = (pos == i) ? 1 : 0xa0;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
    }
}

void operate_gallery_retry(int *pos) {
    int pad;
    if ((pad = iodat_chk(0x1000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            *pos = *pos ? *pos - 1 : 2;
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x4000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            *pos = (*pos == 2) ? 0 : *pos + 1;
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
}

void Exit_GalleryRetry(void) {
    SetFadeSpeed(20);
    SetFadeEndCommand(0);
    Rq_Fadeout();
    sound_alloff();
    while (!IsFadeEnd()) {
    }
}

void gallery_retry_select(void) {
    gFadeValSet(1.0f);
    sound_alloff();
    BGM_Free();
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    g_info.BattleControlFlag &= ~4;
    JobChange(3);
}

void gallery_retry_title(void) {
    gFadeValSet(1.0f);
    sound_alloff();
    BGM_Free();
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    g_info.BattleControlFlag &= ~4;
    JobChange(3);
}
