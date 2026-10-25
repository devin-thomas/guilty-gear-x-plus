#include "generated/game.h"
void LinkAddrTb(u_int *addr);
void ObjLinkSub(u_int *p, int pid, int cnt);
void ObjFileLink(int addr, int use, int pid);
void ForeStellPlayer(void);
void GameInit(void);
void GameLoad_Skill(void);
void GameLoad(void);
void set_noskill(void);
void Pause_Execute(void);
int pause_normal(int pos);
int pause_gallery(int pos);
int pause_training(int pos);
int pause_training_common(HEADWORK *hw, PARTS *pt, int pos);
void pause_training_common_init(HEADWORK *hw, int flg);
int pause_training_enemy(HEADWORK *hw, PARTS *pt, int pos);
int chk_selectok(int no);
void pause_training_enemy_init(HEADWORK *hw, int flg);
int pause_training_practice(HEADWORK *hw, PARTS *pt, int pos);
void pause_training_practice_init(HEADWORK *hw, int flg);
void disp_enemylevel(HEADWORK *hw);
void disp_cmnguard(HEADWORK *hw);
void GameKill(void);
void ExitDemo_Main(void);
void SwDataConv(void);
void GameMain(void);
void g_main_End(void);
void GameEnd(void);
extern char *Player1File;
extern char *Player2File;
extern char *Player1Skill;
extern char *Player2Skill;
extern int GlobalHitStop;
extern int ExitDemoMode;
extern int BattleStatus;
extern int AfterBattleCounter;
static const short idtb[] = { 1, 2, 3 };
static int an_id[] = { 0xd, 0x4c, 0x4e, 0x53, -1 };
static int ax_id[] = { 5, 0x4d, 0x51, -1 };
static int bk_id[] = { 9, 0x39, 0x3a, -1 };
static int ch_id[] = { 7, 0x45, 0x3b, 0x28, 0x3c, -1 };
static int dz_id[] = { 0x10, 0x24, 0x20, 0x43, -1 };
static int fa_id[] = { 0xa, 0x2b, 0x40, 0x4a, -1 };
static int jm_id[] = { 0xc, 0x2a, 0x26, 0x25, 0x2f, 0x30, 0x27, -1 };
static int jy_id[] = { 0xe, 0x47, 0x3f, 0x55, -1 };
static int ky_id[] = { 2, 0x29, 0x56, -1 };
static int ml_id[] = { 4, 0x34, 0x21, 0x2e, -1 };
static int my_id[] = { 3, 0x3e, 0x48, 0x37, 0x44, 0x46, 0x4b, -1 };
static int po_id[] = { 6, 0x49, 0x3d, -1 };
static int sl_id[] = { 1, 0x2c, 0x42, -1 };
static int ts_id[] = { 0xb, 0x50, 0x4f, 0x54, 0x52, -1 };
static int ve_id[] = { 0xf, 0x38, 0x2d, 0x22, 0x31, 0x35, 0x32, 0x36, -1 };
static int zt_id[] = { 8, 0x41, 0x23, 0x33, -1 };
static int kr_id[] = { 0x11, 0x57, -1 };
static int js_id[] = { 0x12, 0x59, 0x60, -1 };
static int *eff_id[] = { sl_id, ky_id, my_id, ml_id, ax_id, po_id, ch_id, zt_id, bk_id, fa_id, ts_id, jm_id, an_id, jy_id, ve_id, dz_id, kr_id, js_id };
static int effcmn[] = { 0x69, 0x64, 0x68, 0x6a, 0x6b, 0x66, 0x6d, 0x6f, 0x6c, 0x67, 0x61, 0x6e, 0x62, 0x7e, 0x63, 0x65, 0x7f, -1 };
void LoadFrameBufferObject(short int idno);
int SetCellTopAndGclTop(short int side, u_short idno, u_int *cell, u_int *patt);
int SetInstTb(short int side, u_short idno, u_int *inst);
int SetPalTop(short int side, u_short idno, u_int *pal);
void set_PlayDat(int idno, short int *p);
extern unsigned int DebugFlag;
extern unsigned int Pause;
extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];
extern USER_SETTING userinfo;
extern TRAINING_SETTING *tr_info;
extern SURVIVAL_INFOMATION *sv_info;
extern SYS_COND sc;
extern u_int TotalCounter;
extern char *EffectCmnFile;
int IST_Load(int *texadr, int sprid, int palid);
void JobNext(void);
void FontDraw(char *str, int x, int y, float z, int alfa);
extern PAD pads[2];
extern int BattleStatus;
extern CHARANGE_CTRL *gl_info;
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void InstDisp(int sw);
void JobChange(int job);
void Opt_KeyConfig(int sw);
int iodat_chk(int stat);
void nlSetBackColor(int c1, int c2, int c3);
int sound_request(int no);
void debugf(char *format, ...);
int chk_selectok(int no);
int pause_training_common(HEADWORK *hw, PARTS *pt, int pos);
int pause_training_enemy(HEADWORK *hw, PARTS *pt, int pos);
int pause_training_practice(HEADWORK *hw, PARTS *pt, int pos);
void pause_training_common_init(HEADWORK *hw, int flg);
void pause_training_enemy_init(HEADWORK *hw, int flg);
void pause_training_practice_init(HEADWORK *hw, int flg);
void disp_cmnguard(HEADWORK *hw);
void disp_enemylevel(HEADWORK *hw);
extern CHARACTER_WORK *PlayerWork;
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void LinkAddrTb(u_int *addr)
{
    u_int offset;

    offset = (u_int)addr;
    if (*addr < offset) {
        if (*addr == 0xFFFFFFFF) {
            *addr = 0;
            return;
        }
        while (*addr != 0xFFFFFFFF) {
            *addr += offset;
            addr++;
        }
        *addr = 0;
    }
}

void ObjLinkSub(u_int *p, int pid, int cnt)
{
    u_int *pal;
    u_int *inst;

    LinkAddrTb(p);
    if (p[0]) {
        pal = (u_int *)p[3];
        inst = (u_int *)p[2];
        SetCellTopAndGclTop(pid, cnt, (u_int *)p[1], (u_int *)p[0]);
        if (pal) SetPalTop(pid, cnt, pal);
        SetInstTb(pid, cnt, inst);
        if (pid != -2) {
            if (pid == -1) {
                set_PlayDat(0, (short *)inst);
                set_PlayDat(1, (short *)inst);
            } else {
                set_PlayDat(pid, (short *)inst);
            }
        }
        switch (cnt) {
        case 0x21:
        case 0x22:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
        case 0x53:
        case 0x55:
        case 0x56:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x7e:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
            LoadFrameBufferObject(cnt);
            break;
        }
    }
}

void ObjFileLink(int addr, int use, int pid)
{
    u_int *p;
    u_int *IDTb;
    u_int *idlist;

    IDTb = (u_int *)addr;
    if (use >= 0) {
        idlist = (u_int *)eff_id[use];
    } else {
        idlist = (u_int *)effcmn;
    }
    LinkAddrTb((u_int *)addr);
    p = (u_int *)*(u_int *)addr;
    addr = (int)idlist;
    while (p) {
        ObjLinkSub(p, pid, *(u_int *)addr);
        IDTb++;
        addr += 4;
        p = (u_int *)*IDTb;
    }
}

char *FileReadSelect(int fid, int chan);
void Read_Playdat(int size);
void debugf(char *format, ...);
extern short int UseCharacter1p2p[];
extern USER_SETTING userinfo;
void ForeStellPlayer(void)
{
    int f1id;
    int f2id;

    Read_Playdat(0);
    f1id = (UseCharacter1p2p[0] - 1) * 2 + 0xCE;
    f2id = (UseCharacter1p2p[1] - 1) * 2 + 0xCE;
    if (userinfo.gguse[0] & 2) f1id--;
    if (userinfo.gguse[1] & 2) f2id--;
    debugf("Player Load %d %d\n", UseCharacter1p2p[0], UseCharacter1p2p[1]);
    if (f2id < f1id) {
        Player2Skill = FileReadSelect(UseCharacter1p2p[1] + 0xBA, 6);
        Player1Skill = FileReadSelect(UseCharacter1p2p[0] + 0xBA, 5);
        Player2File = FileReadSelect(f2id, 1);
        Player1File = FileReadSelect(f1id, 0);
    } else {
        Player1Skill = FileReadSelect(UseCharacter1p2p[0] + 0xBA, 5);
        Player2Skill = FileReadSelect(UseCharacter1p2p[1] + 0xBA, 6);
        Player1File = FileReadSelect(f1id, 0);
        Player2File = FileReadSelect(f2id, 1);
    }
}

void AllBgWork_Clear(void);
int AllCharacterWorkInitial(void);
void CockpitInit(void);
void CockpitLoadInit(void);
void FadeInit(void);
void ForeStellBG(void);
void ForeStellVoiceLoad(void);
void InstTbInit(void);
void JobNext(void);
void PadInitActuator(int pad);
void Screen_Init(void);
void dcfont_init(void);
void heap_clear(void);
void set_random_seed(void);
void GameInit(void)
{
    heap_clear();
    PadInitActuator(0);
    PadInitActuator(1);
    DebugFlag = 0;
    Pause = 0;
    GlobalHitStop = 0;
    if (g_info.mode & 0x4000) set_random_seed();
    ForeStellPlayer();
    ForeStellBG();
    AllCharacterWorkInitial();
    AllBgWork_Clear();
    Screen_Init();
    ExitDemoMode = 0;
    ForeStellVoiceLoad();
    InstTbInit();
    CockpitLoadInit();
    CockpitInit();
    dcfont_init();
    FadeInit();
    JobNext();
}

int PXF_GetSysSprite(void);
extern DCFONT_INFO fi;
void GameLoad_Skill(void)
{
    u_int *p;
    int n;

    for (n = 0; n < 2; n++) {
        fi.font[n + 5].spno = PXF_GetSysSprite();
    }
    p = (u_int *)Player1Skill;
    LinkAddrTb(p);
    IST_Load((int *)p, fi.font[5].spno, -1);
    p = (u_int *)Player2Skill;
    LinkAddrTb(p);
    IST_Load((int *)p, fi.font[6].spno, -1);
}

void BG_Init(void);
void ChaseInit(void);
void EnterFight_Init(void);
void FadeInit(void);
int FileReadCheck(void);
void G_GraphicInit(void);
void InitExCollisionWork(void);
void KakuseiFinishInit(void);
void PlayerInit(void);
void Poly_Effect_Initial(void);
void ResultInitial(void);
void Rq_Fadein(void);
void dcfont_init(void);
void init_davol(void);
void set_activebtn(int padid);
void set_sevol(void);
void sv_save_enemystatus(int side);
void sv_set_enemy_status(int side);
void GameLoad(void)
{
    if (!FileReadCheck()) {
        if (TotalCounter & 0x20) {
            FontDraw("NOW LOADING", 0x118, 0xF0, 200.0f, 0xFF);
        }
    } else {
        ObjFileLink((int)Player1File, UseCharacter1p2p[0] - 1, 0);
        ObjFileLink((int)Player2File, UseCharacter1p2p[1] - 1, 1);
        ObjFileLink((int)EffectCmnFile, -1, -2);
        BG_Init();
        PlayerInit();
        set_activebtn(0);
        set_activebtn(1);
        Poly_Effect_Initial();
        FadeInit();
        ChaseInit();
        ResultInitial();
        InitExCollisionWork();
        G_GraphicInit();
        KakuseiFinishInit();
        dcfont_init();
        GameLoad_Skill();
        set_sevol();
        init_davol();
        Rq_Fadein();
        BattleStatus = 1;
        EnterFight_Init();
        sc.rannyu = 0;
        sc.pause_clear = 0;
        if (g_info.mode & 0x200) {
            sv_save_enemystatus(sv_info->ply == 0);
            sv_set_enemy_status(sv_info->ply == 0);
        }
        JobNext();
    }
}

void set_noskill(void)
{
    pads[0].trg = 0;
    pads[1].trg = 0;
    if (g_info.mode & 0x100) {
        tr_info->iobkup[0] = 0x62;
        tr_info->iobkup[2] = 0x62;
        tr_info->iobkup[1] = 0x62;
    }
}

int Cr_IsPlaying(int side);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
int sound_request(int no);
extern story_t story;
void Pause_Execute(void)
{
    static int pos;
    static int alpha = 0xFF;

    if (g_info.Event) return;
    if (g_info.mode & 0x4000) return;
    if ((g_info.mode & 0x2000) && story.is_enter) return;
    if (Pause == 0 && sc.rannyu == 0 && BattleStatus == 2
        && (((pads[0].trg & 0x800) && Cr_IsPlaying(0)) || ((pads[1].trg & 0x800) && Cr_IsPlaying(1)))
        && (!(g_info.mode & 0x100) || tr_info->begin <= 0)) {
        sound_request(0x39);
        Pause = 1;
        pos = 0;
        pads[0].trg = 0;
        pads[1].trg = 0;
        sc.pause_clear = 0;
    } else if (sc.pause_clear & 0xEF) {
        Pause = 1;
    }
    if (Pause >= 1 && Pause <= 2) {
        if (sc.pause_clear) return;
        if (alpha < 0xD0) alpha += 0x20;
        if (alpha > 0xD0) alpha = 0xD0;
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 3.0f, alpha << 24);
        if (g_info.mode & 0x100) {
            pos = pause_training(pos);
        } else if (g_info.mode & 0x8000) {
            pos = pause_gallery(pos);
        } else {
            pos = pause_normal(pos);
        }
        if (Pause == 0) {
            sc.pause_clear = 0;
            set_noskill();
        } else if (g_info.mode & 0x100) {
            if ((pads[0].trg & 0x100) || (pads[1].trg & 0x100)) Pause = 0x80;
        } else {
            if ((pads[0].trg & 0x100) || (pads[1].trg & 0x100)) Pause = 0x80;
        }
    } else if (Pause >= 0x80 && Pause <= 0x8F) {
        if (g_info.mode & 0x100) {
            if ((pads[0].trg & 0x100) || (pads[1].trg & 0x100)) Pause = 1;
        } else {
            if ((pads[0].trg & 0x100) || (pads[1].trg & 0x100)) Pause = 1;
        }
    } else if (Pause == 0x90) {
        Pause = 0x80;
        DebugFlag &= ~8;
        DebugFlag &= ~0x40;
        DebugFlag &= ~0x100;
        alpha = 0;
    } else {
        alpha = 0;
        DebugFlag &= ~8;
        DebugFlag &= ~0x40;
        DebugFlag &= ~0x100;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void InstDisp(int sw);
void JobChange(int job);
void Opt_KeyConfig(int sw);
int iodat_chk(int stat);
void nlSetBackColor(int c1, int c2, int c3);
int sound_request(int no);
int pause_normal(int pos)
{
    HEADWORK hw[4] = {
        { "RETURN", 256.0f, 208.0f, 0, 0, 0 },
        { "KEY CONFIG", 256.0f, 240.0f, 0, 0, 0 },
        { "COMMAND LIST", 256.0f, 272.0f, 0, 0, 0 },
        { "TITLE", 256.0f, 304.0f, 0, 0, 0 },
    };
    int i;
    int alpha;
    int pad;

    if (sc.grpvar[0] == 0) {
        if ((pad = iodat_chk(0x1000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = pos ? pos - 1 : 3;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = (pos != 3) ? pos + 1 : 0;
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        pad = pads[0].trg | pads[1].trg;
        if (pad & 0x840) {
            sc.grpvar[0] = 0;
            sound_request(0x3B);
            Pause = 0;
        } else if (pad & 0x20) {
            sound_request(0x39);
            switch (pos) {
            case 0:
                Pause = 0;
                break;
            case 1:
                Opt_KeyConfig(0);
                sc.grpvar[0] = pos;
                break;
            case 2:
                if ((pads[0].trg | pads[1].trg) & 0x20) {
                    if (pads[0].trg & 0x20) {
                        sc.grpvar[0] = 1;
                    } else {
                        sc.grpvar[0] = pos;
                    }
                    InstDisp(0);
                }
                break;
            case 3:
                if (userinfo.autosave) {
                    nlSetBackColor(0, 0, 0);
                    sc.grpvar[1] = -1;
                    JobChange(0x2D);
                } else {
                    nlSetBackColor(0, 0, 0);
                    JobChange(3);
                }
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            alpha = (i == pos) ? 1 : 0xA0;
            FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
        }
    } else if (pos == 1) {
        Opt_KeyConfig(1);
    } else if (pos == 2) {
        InstDisp(1);
    } else {
        sc.grpvar[0] = 0;
    }
    return pos;
}

int pause_gallery(int pos)
{
    HEADWORK hw[6] = {
        { "RETURN", 224.0f, 176.0f, 0, 0, 0 },
        { "RETRY", 224.0f, 208.0f, 0, 0, 0 },
        { "SELECT", 224.0f, 240.0f, 0, 0, 0 },
        { "KEY CONFIG", 224.0f, 272.0f, 0, 0, 0 },
        { "COMMAND LIST", 224.0f, 304.0f, 0, 0, 0 },
        { "TITLE", 224.0f, 336.0f, 0, 0, 0 },
    };
    int i;
    int alpha;
    int pad;

    if (gl_info->reset) {
        for (i = 0; i < 6; i++) {
            alpha = (i == pos) ? 1 : 0xA0;
            FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
        }
        return pos;
    }
    if (sc.grpvar[0] == 0) {
        if ((pad = iodat_chk(0x1000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = pos ? pos - 1 : 5;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = (pos != 5) ? pos + 1 : 0;
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        pad = pads[0].trg | pads[1].trg;
        if (pad & 0x840) {
            sc.grpvar[0] = 0;
            sound_request(0x3B);
            Pause = 0;
        } else if (pad & 0x20) {
            sound_request(0x39);
            switch (pos) {
            case 0:
                Pause = 0;
                break;
            case 1:
                if (gl_info->reset == 0) gl_info->reset = 1;
                break;
            case 2:
                nlSetBackColor(0, 0, 0);
                JobChange(0x1B);
                Pause = 0;
                break;
            case 3:
                Opt_KeyConfig(0);
                sc.grpvar[0] = 1;
                break;
            case 4:
                if ((pads[0].trg | pads[1].trg) & 0x20) {
                    if (pads[0].trg & 0x20) {
                        sc.grpvar[0] = 1;
                    } else {
                        sc.grpvar[0] = 2;
                    }
                    InstDisp(0);
                }
                break;
            case 5:
                if (userinfo.autosave) {
                    nlSetBackColor(0, 0, 0);
                    sc.grpvar[1] = -1;
                    JobChange(0x2D);
                } else {
                    nlSetBackColor(0, 0, 0);
                    JobChange(3);
                }
                break;
            }
        }
        for (i = 0; i < 6; i++) {
            alpha = (i == pos) ? 1 : 0xA0;
            FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
        }
    } else if (pos == 3) {
        Opt_KeyConfig(1);
    } else if (pos == 4) {
        InstDisp(1);
    } else {
        sc.grpvar[0] = 0;
    }
    return pos;
}

void Tranning_init_prac(void);
int pause_training(int pos)
{
    int (*pause_training_sub[3])() = { pause_training_common, pause_training_enemy, pause_training_practice };
    int i;
    int pad;
    int modeid[3][2] = { { 0, 8 }, { 0, 8 }, { 0, 2 } };
    PARTS mp[3] = {
        { "CONFIG", 32.0f, 104.0f, 0 },
        { "ENEMY SETTING", 176.0f, 104.0f, 0 },
        { "PRACTICE", 440.0f, 104.0f, 0 },
    };
    static HEADWORK hw[3][9] = {
        {
            { "RETURN", 64.0f, 144.0f, 0, 0, 0 },
            { "TENSION MAX", 64.0f, 176.0f, 0, 4, 0 },
            { "GUARD LEVEL", 64.0f, 208.0f, 0, -1, 0 },
            { "DAMAGE DISP", 64.0f, 240.0f, 0, 2, 0 },
            { "CHARACTER SELECT", 64.0f, 272.0f, 0, 0, 0 },
            { "KEY CONFIG", 64.0f, 304.0f, 0, 0, 0 },
            { "COMMAND LIST", 64.0f, 336.0f, 0, 0, 0 },
            { "DEFAULT", 64.0f, 368.0f, 0, 0, 0 },
            { "TITLE", 64.0f, 400.0f, 0, 0, 0 },
        },
        {
            { "RETURN", 64.0f, 144.0f, 0, 0, 0 },
            { "MODE", 64.0f, 176.0f, 0, 3, 0 },
            { "ENEMY STATE", 64.0f, 208.0f, 0, 4, 0 },
            { "GUARD", 64.0f, 240.0f, 0, 2, 0 },
            { "RECOVERY", 64.0f, 272.0f, 0, 5, 0 },
            { "ATTACK MEMORY", 64.0f, 304.0f, 0, 2, 0 },
            { "ENEMY LEVEL", 64.0f, 336.0f, 10, 33, 0 },
            { "LIFE REGAIN", 64.0f, 368.0f, 1, 4, 0 },
            { "DEFAULT", 64.0f, 400.0f, 0, 0, 0 },
        },
        {
            { "RETURN", 64.0f, 144.0f, 0, 0, 0 },
            { "PRACTICE", 64.0f, 176.0f, 0, 1, 0 },
            { "DEFAULT", 64.0f, 208.0f, 0, 0, 0 },
            { "", 64.0f, 240.0f, 0, 0, 0 },
            { "", 64.0f, 272.0f, 0, 0, 0 },
            { "", 64.0f, 304.0f, 0, 0, 0 },
            { "", 64.0f, 336.0f, 0, 0, 0 },
            { "", 64.0f, 368.0f, 0, 0, 0 },
            { "", 64.0f, 400.0f, 0, 0, 0 },
        },
    };
    PARTS pt[25] = {
        { "STAND", 272.0f, 10.0f, 0 },
        { "CROUCH", 352.0f, 10.0f, 0 },
        { "JUMP", 444.0f, 10.0f, 0 },
        { "OFF", 272.0f, 10.0f, 0 },
        { "ON", 328.0f, 10.0f, 0 },
        { "NORMAL", 272.0f, 10.0f, 0 },
        { "MAX", 376.0f, 10.0f, 0 },
        { "OFF", 272.0f, 10.0f, 0 },
        { "FT", 336.0f, 10.0f, 0 },
        { "NT", 384.0f, 10.0f, 0 },
        { "BK", 432.0f, 10.0f, 0 },
        { "RND", 480.0f, 10.0f, 0 },
        { "FREE", 272.0f, 10.0f, 0 },
        { "VSCPU", 344.0f, 10.0f, 0 },
        { "OFF", 272.0f, 10.0f, 0 },
        { "PLAYER", 326.0f, 10.0f, 0 },
        { "ENEMY", 412.0f, 10.0f, 0 },
        { "BOTH", 484.0f, 10.0f, 0 },
        { "STAND", 272.0f, 10.0f, 0 },
        { "CROUCH", 344.0f, 10.0f, 0 },
        { "JUMP", 428.0f, 10.0f, 0 },
        { "ATTACK", 488.0f, 10.0f, 0 },
        { "FREE", 272.0f, 10.0f, 0 },
        { "VSCPU", 344.0f, 10.0f, 0 },
        { "VSPLAYER", 422.0f, 10.0f, 0 },
    };
    int alpha;

    if (tr_info->clear == 0) {
        pause_training_common_init(hw[0], 1);
        pause_training_enemy_init(hw[1], 1);
        pause_training_practice_init(hw[2], 1);
        tr_info->clear = 1;
    }
    if (sc.grpvar[0] == 0) {
        FontDrawDC("TRAINING MENU", 32.0f, 40.0f, 2.0f, 1, 0);
        if (Pause == 1) {
            Pause = 2;
            hw[1][5].pos = 0;
        }
        if (tr_info->cnfside == 2 && pos >= 3) pos = 2;
        pos = pause_training_sub[tr_info->cnfside](hw[tr_info->cnfside], pt, pos);
        for (i = 0; i < 3; i++) {
            alpha = (tr_info->cnfside == i) ? 1 : 0xA0;
            FontDrawDC(mp[i].head, mp[i].xp, mp[i].yp, 2.0f, alpha, 1);
        }
        pad = pads[0].trg | pads[1].trg;
        if ((pad & 4) || (pos == 0 && (pad & 0x8000))) {
            sound_request(0x37);
            if (tr_info->cnfside == 0) {
                tr_info->cnfside = 2;
            } else {
                tr_info->cnfside--;
            }
        }
        pad = pads[0].trg | pads[1].trg;
        if ((pad & 8) || (pos == 0 && (pad & 0x2000))) {
            sound_request(0x37);
            if (tr_info->cnfside == 2) {
                tr_info->cnfside = 0;
            } else {
                tr_info->cnfside++;
            }
        }
        while (chk_selectok(pos)) {
            pos = (pos == 0) ? modeid[tr_info->cnfside][1] : pos - 1;
        }
        if ((pad = iodat_chk(0x1000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                do {
                    pos = (pos == 0) ? modeid[tr_info->cnfside][1] : pos - 1;
                } while (chk_selectok(pos));
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                do {
                    pos = (pos == modeid[tr_info->cnfside][1]) ? modeid[tr_info->cnfside][0] : pos + 1;
                } while (chk_selectok(pos));
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x8000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                if (hw[tr_info->cnfside][pos].maxhead > 0) {
                    int ret;

                    if (tr_info->cnfside == 1 && pos == 6) {
                        ret = 0;
                        sound_request(6);
                    } else {
                        ret = 1;
                        sound_request(0x37);
                    }
                    hw[tr_info->cnfside][pos].pos--;
                    if (tr_info->cnfside == 2 && pos == 1) {
                        if (hw[2][1].pos < 0) hw[2][1].pos = tr_info->pracnomax - 1;
                    } else if (hw[tr_info->cnfside][pos].pos < 0) {
                        if (ret == 0) {
                            hw[tr_info->cnfside][pos].pos = 0;
                        } else {
                            hw[tr_info->cnfside][pos].pos = hw[tr_info->cnfside][pos].maxhead - 1;
                        }
                    }
                } else if (hw[tr_info->cnfside][pos].maxhead == -1) {
                    int num;

                    num = ((pads[0].data | pads[1].data) & 0x20) ? 10 : 1;
                    sound_request(6);
                    hw[tr_info->cnfside][pos].pos -= num;
                    if (hw[tr_info->cnfside][pos].pos < -128) hw[tr_info->cnfside][pos].pos = -128;
                }
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x2000)) != 0) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                if (hw[tr_info->cnfside][pos].maxhead > 0) {
                    int ret;

                    if (tr_info->cnfside == 1 && pos == 6) {
                        sound_request(6);
                        ret = 0;
                    } else {
                        sound_request(0x37);
                        ret = 1;
                    }
                    hw[tr_info->cnfside][pos].pos++;
                    if (tr_info->cnfside == 2 && pos == 1) {
                        if (hw[2][1].pos >= tr_info->pracnomax) hw[2][1].pos = 0;
                    } else if (hw[tr_info->cnfside][pos].pos >= hw[tr_info->cnfside][pos].maxhead) {
                        if (ret == 0) {
                            hw[tr_info->cnfside][pos].pos = hw[tr_info->cnfside][pos].maxhead - 1;
                        } else {
                            hw[tr_info->cnfside][pos].pos = 0;
                        }
                    }
                } else if (hw[tr_info->cnfside][pos].maxhead == -1) {
                    int num;

                    num = ((pads[0].data | pads[1].data) & 0x20) ? 10 : 1;
                    sound_request(6);
                    hw[tr_info->cnfside][pos].pos += num;
                    if (hw[tr_info->cnfside][pos].pos > 128) hw[tr_info->cnfside][pos].pos = 128;
                }
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        if ((pads[0].trg | pads[1].trg) & 0x840) {
            sc.grpvar[0] = 0;
            sound_request(0x3B);
            Pause = 0;
        }
        if (tr_info->stat == 3 || tr_info->stat == 1) {
            hw[2][1].pos = 0;
            Tranning_init_prac();
        }
    } else if (pos == 5) {
        Opt_KeyConfig(1);
    } else if (pos == 6) {
        InstDisp(1);
    } else {
        sc.grpvar[0] = 0;
    }
    return pos;
}

int pause_training_common(HEADWORK *hw, PARTS *pt, int pos)
{
    int i;
    int alpha;
    int j;

    hw[1].p = pt + 14;
    hw[3].p = pt + 3;
    hw[0].p = 0;
    hw[2].p = 0;
    hw[4].p = 0;
    hw[5].p = 0;
    hw[6].p = 0;
    hw[7].p = 0;
    hw[8].p = 0;
    for (i = 0; i < 9; i++) {
        alpha = (i == pos) ? 1 : 0xA0;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
        if (hw[i].p) {
            for (j = 0; j < hw[i].maxhead; j++) {
                alpha = (hw[i].pos == j) ? 0xFF : 0x50;
                FontDraw(hw[i].p[j].head, hw[i].xp + hw[i].p[j].xp, hw[i].yp + hw[i].p[j].yp, 2.0f, alpha);
            }
        }
    }
    switch (pos) {
    case 0:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            Pause = 0;
        }
        break;
    case 1:
        tr_info->tcom.tension = hw[pos].pos;
        if (tr_info->tcom.tension == 0) {
            PlayerWork[0].ply->Tension = 0;
            PlayerWork[1].ply->Tension = 0;
        }
        break;
    case 2:
        tr_info->tcom.ggage = hw[pos].pos;
        break;
    case 3:
        tr_info->tcom.damage_disp = hw[pos].pos;
        break;
    case 4:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            tr_info->stat = 1;
            tr_info->itigeki = 0;
            Pause = 0;
        }
        break;
    case 5:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            sc.grpvar[0] = 1;
            Opt_KeyConfig(0);
        }
        break;
    case 6:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            sc.grpvar[0] = 1;
            InstDisp(0);
        }
        break;
    case 7:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            pause_training_common_init(hw, 0);
        }
        break;
    case 8:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            tr_info->stat = 3;
            Pause = 0;
        }
        break;
    }
    disp_cmnguard(hw + 2);
    return pos;
}

void pause_training_common_init(HEADWORK *hw, int flg)
{
    if (flg == 0) {
        hw[0].pos = 0;
        hw[1].pos = 0;
        hw[2].pos = 0;
        hw[3].pos = 0;
        hw[4].pos = 0;
        hw[5].pos = 0;
        hw[8].pos = 0;
        tr_info->tcom.tension = hw[1].pos;
        tr_info->tcom.ggage = hw[2].pos;
        tr_info->tcom.damage_disp = hw[3].pos;
    } else {
        hw[2].pos = tr_info->tcom.ggage;
        hw[1].pos = tr_info->tcom.tension;
        hw[3].pos = tr_info->tcom.damage_disp;
    }
}

/* chk_selectok's local flg[3][9] initializer sits in .rodata after data of functions not in this
   file yet: copy it from the original's bytes */
typedef struct {
    int flg[3][9];
} chk_selectok_flgtb;
extern chk_selectok_flgtb D_5BB158; /* RODATA_EXTERN 0x5bb158: { {-1 x9}, {-1,-1,0,0,0,0,1,-1,-1}, {-1 x9} } */

int chk_selectok(int no)
{
    chk_selectok_flgtb flg = D_5BB158;

    if (flg.flg[tr_info->cnfside][no] == -1) return 0;
    return flg.flg[tr_info->cnfside][no] != tr_info->tene.mode;
}

void pause_training_enemy_init(HEADWORK *hw, int flg)
{
    if (flg == 0) {
        hw[1].pos = 0;
        tr_info->tene.mode = 0;
        hw[6].pos = 10;
        hw[7].pos = 3;
        hw[0].pos = 0;
        hw[2].pos = 0;
        hw[3].pos = 0;
        hw[4].pos = 0;
        hw[5].pos = 0;
        tr_info->tene.level = hw[6].pos;
        tr_info->tene.type = hw[2].pos;
        tr_info->tene.guard = hw[3].pos;
        tr_info->tene.ukemi = hw[4].pos;
        tr_info->tene.rejene = hw[7].pos;
        tr_info->tene.memflg = hw[5].pos;
        tr_info->tene.resettim = 0;
    } else {
        hw[1].pos = tr_info->tene.mode;
        hw[6].pos = tr_info->tene.level;
        hw[2].pos = tr_info->tene.type;
        hw[3].pos = tr_info->tene.guard;
        hw[4].pos = tr_info->tene.ukemi;
        hw[7].pos = tr_info->tene.rejene;
    }
}

void pause_training_practice_init(HEADWORK *hw, int flg)
{
    hw[1].pos = 0;
    tr_info->pracno = 0;
}

unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FLine(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void polydraw_LBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void set_char2num(int num, char *block, int id);
void disp_enemylevel(HEADWORK *hw)
{
    int col;
    int alpha;
    char buf[8];

    alpha = (tr_info->tene.mode == 1) ? 0xFF : 0x10;
    set_char2num(hw->pos, buf, 0);
    FontDraw(buf, hw->xp + 272.0f + 256.0f + 8.0f, hw->yp + 10.0f, 2.0f, alpha);
    if (tr_info->tene.mode != 1) {
        polydraw_FBox(hw->xp + 272.0f, hw->yp + 10.0f, hw->xp + 272.0f + 256.0f, hw->yp + 24.0f, 1.0f, gStripGetFadeColor(0xC0000010));
    }
    polydraw_LBox(hw->xp + 272.0f, hw->yp + 10.0f, hw->xp + 272.0f + 256.0f, hw->yp + 24.0f, 1.0f, 0xFFFFFFFF);
    col = tr_info->tene.level * 8;
    if (col >= 0xFF) col = 0xFF;
    if (col <= 0x20) col = 0x20;
    col = (col << 16) + col / 4;
    col |= 0xFF000000;
    if (tr_info->tene.level) {
        polydraw_FBox(hw->xp + 272.0f, hw->yp + 10.0f, hw->xp + 272.0f + (float)(tr_info->tene.level * 8) - 1.0f, hw->yp + 24.0f, 2.0f, col);
        polydraw_FLine(hw->xp + 272.0f + (float)(tr_info->tene.level * 8) - 1.0f, hw->yp + 10.0f, hw->xp + 272.0f + (float)(tr_info->tene.level * 8) - 1.0f, hw->yp + 24.0f, 1.0f, 0xFFFF0000);
    }
}

void AllCharacterWorkRelease(void);
void BGM_Free(void);
void VoiceFree(void);
void dfree(void *mem);
void GameKill(void)
{
    AllCharacterWorkRelease();
    if (Player2File) {
        if (Player2File != Player1File) {
            dfree(Player2File);
            dfree(Player2Skill);
        }
        Player2File = 0;
        Player2Skill = 0;
    }
    if (Player1File) {
        dfree(Player1File);
        dfree(Player1Skill);
        Player1File = 0;
        Player1Skill = 0;
    }
    BGM_Free();
    VoiceFree();
}

void SwDataConv(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (pads[i].data & PlayerWork[i].ply->PandK) pads[i].data |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey;
        if (pads[i].data & PlayerWork[i].ply->SandHS) pads[i].data |= PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].data & PlayerWork[i].ply->PKS) pads[i].data |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey;
        if (pads[i].data & PlayerWork[i].ply->PKSHS) pads[i].data |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].trg & PlayerWork[i].ply->PandK) pads[i].trg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey;
        if (pads[i].trg & PlayerWork[i].ply->SandHS) pads[i].trg |= PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].trg & PlayerWork[i].ply->PKS) pads[i].trg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey;
        if (pads[i].trg & PlayerWork[i].ply->PKSHS) pads[i].trg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].offtrg & PlayerWork[i].ply->PandK) pads[i].offtrg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey;
        if (pads[i].offtrg & PlayerWork[i].ply->SandHS) pads[i].offtrg |= PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].offtrg & PlayerWork[i].ply->PKS) pads[i].offtrg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey;
        if (pads[i].offtrg & PlayerWork[i].ply->PKSHS) pads[i].offtrg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].old & PlayerWork[i].ply->PandK) pads[i].old |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey;
        if (pads[i].old & PlayerWork[i].ply->SandHS) pads[i].old |= PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].old & PlayerWork[i].ply->PKS) pads[i].old |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey;
        if (pads[i].old & PlayerWork[i].ply->PKSHS) pads[i].old |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].oldtrg & PlayerWork[i].ply->PandK) pads[i].oldtrg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey;
        if (pads[i].oldtrg & PlayerWork[i].ply->SandHS) pads[i].oldtrg |= PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
        if (pads[i].oldtrg & PlayerWork[i].ply->PKS) pads[i].oldtrg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey;
        if (pads[i].oldtrg & PlayerWork[i].ply->PKSHS) pads[i].oldtrg |= PlayerWork[i].ply->PunchKey | PlayerWork[i].ply->KickKey | PlayerWork[i].ply->SlashKey | PlayerWork[i].ply->HighSlashKey;
    }
}

void BGObjCreate(void);
void BG_Execute(void);
void ChaseEffectExe(void);
void ChaseFadeOutExe(void);
void CockpitExecute(void);
void CollisionExecute(void);
void CpObjExecute(void);
int DigestReloadStack(void);
void DrawDemoPlayandLogo(void);
void EffectExecute(void);
void EnterFight_Main(void);
void FlashExecute(void);
void GraphicExecuteRoot(void);
void HitStopTimeExeRoot(void);
int IsDemoBattle(void);
void JSTokorotenExecute(int flag);
void KakuseiBG_Draw(void);
void KakuseiBG_Execute(void);
int MeterColorChange(int col);
void ObjectExecute(void);
void PlayerExecute(void);
void PlayerInfoDisp(void);
void RefereeExe(void);
void ScreenExecute(void);
void TranningStat_chk(void);
void chk_boss_stat(void);
void chk_survival_stat(void);
void font_execute(int size);
void gallery_status_chk(void);
int get_player(void);
void ioexe_play(int side);
void replay_playback(void);
void replay_status_chk(void);
extern story_t story;
extern int JobMode;
extern int JSTokorotenFlag;
extern u_int TotalGameCounter;
extern int GameMain_SlowCount; /* STATIC_EXTERN 0x5cb294: SlowCount (= 0) */

void GameMain(void)
{
    int Doing;
    int i;

    if (IsDemoBattle()) {
        ExitDemo_Main();
        if (JobMode != 0x20) return;
        DrawDemoPlayandLogo();
    } else {
        Pause_Execute();
    }
    Doing = 0;
    if (!(Pause >= 1 && Pause <= 2) && !(Pause >= 0x80 && Pause <= 0x8F)) {
        if (DebugFlag & 1) {
            GameMain_SlowCount--;
            if (GameMain_SlowCount <= 0) {
                GameMain_SlowCount = 4;
                Doing = 1;
            }
        } else {
            Doing = 1;
        }
    }
    if (Doing) {
        SwDataConv();
        if (GlobalHitStop <= 0) {
            TotalGameCounter++;
            ScreenExecute();
            PlayerExecute();
            BGObjCreate();
            ObjectExecute();
            EffectExecute();
            ChaseFadeOutExe();
            HitStopTimeExeRoot();
        } else {
            if ((g_info.mode & 0x4000) && sc.replay == 2) {
                replay_playback();
                ioexe_play(get_player());
            }
            GlobalHitStop--;
        }
        KakuseiBG_Execute();
        CpObjExecute();
    }
    PlayerInfoDisp();
    DigestReloadStack();
    GraphicExecuteRoot();
    MeterColorChange(0xFFFF00);
    BG_Execute();
    MeterColorChange(0xFF);
    CockpitExecute();
    DigestReloadStack();
    FlashExecute();
    KakuseiBG_Draw();
    ChaseEffectExe();
    JSTokorotenExecute(JSTokorotenFlag);
    MeterColorChange(0);
    if (Doing) CollisionExecute();
    font_execute(1);
    if (Doing) RefereeExe();
    if (g_info.mode & 0x200) {
        g_info.boss[0] = 0;
        g_info.boss[1] = 0;
    } else if (g_info.mode & 0x8000) {
        if (gl_info->pow != -1) {
            g_info.boss[gl_info->ply == 0] = 1;
        } else {
            g_info.boss[gl_info->ply == 0] = 0;
        }
        g_info.boss[gl_info->ply] = 0;
    } else if (g_info.mode & 0x2000) {
        for (i = 0; i < 2; i++) {
            if (story.is_boss[i]) {
                g_info.boss[i] = 1;
            } else {
                g_info.boss[i] = 0;
            }
        }
    } else {
        for (i = 0; i < 2; i++) {
            if (g_info.PlayerPalette[i] == 9) {
                g_info.boss[i] = 1;
            } else if (g_info.PlayerPalette[i] == 10) {
                g_info.boss[i] = 2;
            } else {
                g_info.boss[i] = 0;
            }
        }
    }
    if (g_info.boss[0] || g_info.boss[1]) chk_boss_stat();
    if (g_info.mode & 0x100) {
        TranningStat_chk();
    } else if (g_info.mode & 0x200) {
        chk_survival_stat();
    } else if (g_info.mode & 0x4000) {
        replay_status_chk();
    } else if (g_info.mode & 0x8000) {
        gallery_status_chk();
    }
    sc.damage[0] = 0;
    sc.damage[1] = 0;
    EnterFight_Main();
    if (JobMode != 0x20) GameKill();
}

void BGM_Free(void);
void BG_End(void);
void ChaseEnd(void);
int FTO_Exe(int cmd, int no);
void KakuseiFinishEnd(void);
void VoiceFree(void);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void sound_alloff(void);
void g_main_End(void)
{
    sound_alloff();
    nlSetBackColor(0, 0, 0);
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    ChaseEnd();
    BG_End();
    FTO_Exe(7, 0);
    BGM_Free();
    VoiceFree();
    KakuseiFinishEnd();
}

int pause_training_enemy(HEADWORK *hw, PARTS *pt, int pos)
{
    int i;
    int alpha;
    int j;
    HEADWORK *lv;

    hw[1].p = pt + 22;
    hw[2].p = pt + 18;
    hw[0].p = 0;
    hw[6].p = 0;
    hw[4].p = pt + 7;
    hw[3].p = pt + 3;
    hw[7].p = pt + 14;
    lv = &hw[6];
    hw[5].p = pt + 3;
    hw[8].p = 0;
    for (i = 0; i < 9; i++) {
        alpha = (i == pos) ? 1 : 0xA0;
        alpha = chk_selectok(i) ? 0xF0 : alpha;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
        if (hw[i].p) {
            for (j = 0; j < hw[i].maxhead; j++) {
                alpha = (hw[i].pos == j) ? 0xFF : 0x50;
                alpha = chk_selectok(i) ? 0x10 : alpha;
                FontDraw(hw[i].p[j].head, hw[i].xp + hw[i].p[j].xp, hw[i].yp + hw[i].p[j].yp, 2.0f, alpha);
            }
        }
    }
    switch (pos) {
    case 0:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            Pause = 0;
        }
        break;
    case 1:
        tr_info->tene.mode = hw[pos].pos;
        break;
    case 6:
        tr_info->tene.level = hw[pos].pos;
        break;
    case 2:
        tr_info->tene.type = hw[pos].pos;
        break;
    case 3:
        tr_info->tene.guard = hw[pos].pos;
        break;
    case 4:
        tr_info->tene.ukemi = hw[pos].pos;
        break;
    case 7:
        tr_info->tene.rejene = hw[pos].pos;
        break;
    case 5:
        tr_info->tene.memflg = hw[pos].pos;
        tr_info->tene.resettim = 0;
        break;
    case 8:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            pause_training_enemy_init(hw, 0);
        }
        break;
    }
    disp_enemylevel(lv);
    return pos;
}

extern int BootControl;
int GetPassBattleTime(void);
int IsFadeEnd(void);
void JobChange(int job);
int ReqSongMute(int outtime, int intime, int mutetime, int mval);
void Rq_Fadeout(void);
void SetFadeSpeed(int frame);
void sound_alloff(void);
extern int PLAYER_HITPOINTMAX;

void ExitDemo_Main(void)
{
    if (ExitDemoMode == 0) {
        if (GetPassBattleTime() < 0x709) {
            if ((pads[0].trg | pads[1].trg) & 0x800) {
                ExitDemoMode = 1;
                SetFadeSpeed(20);
                Rq_Fadeout();
                ReqSongMute(1, 10, 240, 5);
            } else {
                if (!(PlayerWork[0].HitPoint < PLAYER_HITPOINTMAX / 4 || PlayerWork[1].HitPoint < PLAYER_HITPOINTMAX / 4)) {
                    return;
                }
                BootControl |= 2;
                ExitDemoMode = 1;
                SetFadeSpeed(20);
                Rq_Fadeout();
                ReqSongMute(1, 10, 240, 5);
            }
        } else {
            BootControl |= 2;
            ExitDemoMode = 1;
            SetFadeSpeed(20);
            Rq_Fadeout();
            ReqSongMute(1, 10, 240, 5);
        }
    } else if (IsFadeEnd()) {
        sound_alloff();
        GameKill();
        JobChange(3);
    }
}

void disp_cmnguard(HEADWORK *hw)
{
    int col;
    char buf[8];

    set_char2num(hw->pos, buf, 0);
    FontDraw(buf, hw->xp + 272.0f + 257.0f + 8.0f, hw->yp + 10.0f, 2.0f, 0xFF);
    col = (hw->pos < 0) ? 0xFF : 0x50;
    FontDraw("-", hw->xp + 272.0f + 48.0f, hw->yp + 10.0f, 2.0f, col);
    col = (hw->pos >= 0) ? 0xFF : 0x50;
    FontDraw("+", hw->xp + 272.0f + 257.0f - 56.0f, hw->yp + 10.0f, 2.0f, col);
    polydraw_LBox(hw->xp + 272.0f, hw->yp + 10.0f, hw->xp + 272.0f + 257.0f, hw->yp + 24.0f, 1.0f, 0xFFFFFFFF);
    polydraw_FLine(hw->xp + 272.0f + 128.0f, hw->yp + 10.0f, hw->xp + 272.0f + 128.0f, hw->yp + 24.0f, 1.0f, 0xFFFFFFFF);
    col = tr_info->tcom.ggage + 128;
    if (col >= 0xFF) col = 0xFF;
    if (col <= 0x20) col = 0x20;
    col = (col << 16) + col / 4;
    col |= 0xFF000000;
    polydraw_FBox(hw->xp + 272.0f, hw->yp + 10.0f, hw->xp + 272.0f + (float)(tr_info->tcom.ggage + 128), hw->yp + 24.0f, 3.0f, col);
    polydraw_FLine(hw->xp + 272.0f + (float)(tr_info->tcom.ggage + 128), hw->yp + 10.0f, hw->xp + 272.0f + (float)(tr_info->tcom.ggage + 128), hw->yp + 24.0f, 3.0f, 0xFFFF0000);
}

extern PRACALL prac_all[];
void prac_mark_disp(void);

int pause_training_practice(HEADWORK *hw, PARTS *pt, int pos)
{
    int i;
    int alpha;
    PRACLIST *prac_list_temp;
    CHARACTER_WORK *offset;
    int j;

    offset = &PlayerWork[tr_info->ply];
    hw[0].p = 0;
    hw[1].p = 0;
    hw[2].p = 0;
    for (i = 0; i < 3; i++) {
        alpha = (i == pos) ? 1 : 0xA0;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
        if (i == 1) {
            if (hw[1].pos) {
                alpha = 0xFF;
                prac_mark_disp();
            } else {
                alpha = 0x50;
            }
            prac_list_temp = prac_all[offset->idno].list;
            FontDraw(prac_list_temp[hw[i].pos].name, hw[i].xp + 272.0f, hw[i].yp + 10.0f, 2.0f, alpha);
        } else if (hw[i].p) {
            for (j = 0; j < hw[i].maxhead; j++) {
                alpha = (hw[i].pos == j) ? 0xFF : 0x50;
                FontDraw(hw[i].p[j].head, hw[i].xp + hw[i].p[j].xp, hw[i].yp + hw[i].p[j].yp, 2.0f, alpha);
            }
        }
    }
    switch (pos) {
    case 0:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            Pause = 0;
        }
        break;
    case 1:
        tr_info->pracno = hw[pos].pos;
        break;
    case 2:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            pause_training_practice_init(hw, 0);
        }
        break;
    }
    return pos;
}

extern int intrapt;
extern story_t story;
extern int BattleTime;
int Credit_Init(void);
int InitSurvivalBattle(void);
int init_story_battle(void);
/* .rodata strings after chk_selectok's initializer (itself a RODATA_EXTERN) */
extern char D_5BB1C8[]; /* RODATA_EXTERN 0x5bb1c8: "***** story.is_win = TRUE *****\n" */
extern char D_5BB1F0[]; /* RODATA_EXTERN 0x5bb1f0: "***** story.is_win = FALSE *****\n" */

void GameEnd(void)
{
    GAME_INFOMATION *gp;

    gp = &g_info;
    g_main_End();
    if (intrapt) {
        JobChange(0xC);
        intrapt = 0;
    } else if (gp->ModeFlag) {
        if (gp->mode & 0x200) {
            if (sv_info->ply == gp->WinnerSide) {
                if (userinfo.quickconti) {
                    JobChange(0x1E);
                } else {
                    JobChange(0x15);
                }
                if (InitSurvivalBattle()) {
                    JobChange(0x26);
                }
            } else {
                JobChange(0x26);
            }
        } else if (gp->mode & 0x400) {
            JobChange(0x18);
        } else if (gp->mode & 0x4000) {
            Credit_Init();
            JobChange(0x19);
        } else if (gp->mode & 0x8000) {
            Credit_Init();
            JobChange(0x1B);
        } else if (gp->mode & 0x2000) {
            if (story.self_side == gp->WinnerSide) {
                JobChange(0x31);
                story.is_win = 1;
                if (BattleTime / gp->TimeByCount) {
                    story.is_zorome = (BattleTime / gp->TimeByCount % 10 == BattleTime / gp->TimeByCount / 10 % 10);
                } else {
                    story.is_zorome = 0;
                }
                debugf(D_5BB1C8);
                if (init_story_battle()) {
                    JobChange(6);
                }
            } else {
                JobChange(0x31);
                story.nlose++;
                story.is_win = 0;
                debugf(D_5BB1F0);
                if (init_story_battle()) {
                    JobChange(6);
                }
            }
        } else {
            JobChange(0x28);
        }
    } else {
        JobChange(6);
    }
    GameKill();
}
