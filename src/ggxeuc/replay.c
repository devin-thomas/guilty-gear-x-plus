#include "generated/replay.h"
#include "generated/replay.h"
static const short idtb[] = { 1, 2, 3 };
static REP_DELETE_INFO rdinfo;

void rep_file_exist_clr(int i)
{
    int j;

    replay_index = 0;
    for (j = 9; j >= 0; j--) {
        replay_file[i][j] = 0;
    }
}

int MemoryCheck(void);
void MemoryDir(int slot, char *fname, int *p);

void rep_file_exist_job(int port)
{
    static char *namelist[10] = {
#ifdef VERSION_JP
        "BISLPS-20137GGXRP0", "BISLPS-20137GGXRP1", "BISLPS-20137GGXRP2", "BISLPS-20137GGXRP3", "BISLPS-20137GGXRP4",
        "BISLPS-20137GGXRP5", "BISLPS-20137GGXRP6", "BISLPS-20137GGXRP7", "BISLPS-20137GGXRP8", "BISLPS-20137GGXRP9",
#else
        "BKSLPM-64525GGXRP0", "BKSLPM-64525GGXRP1", "BKSLPM-64525GGXRP2", "BKSLPM-64525GGXRP3", "BKSLPM-64525GGXRP4",
        "BKSLPM-64525GGXRP5", "BKSLPM-64525GGXRP6", "BKSLPM-64525GGXRP7", "BKSLPM-64525GGXRP8", "BKSLPM-64525GGXRP9",
#endif
    };

    if (replay_check_flag == -1) {
        replay_index = 0;
        replay_check_flag = 1;
    }
    if (MemoryCheck() > 0) {
        return;
    }
    if (replay_check_flag) {
        replay_check_flag = replay_index / 10 == 0;
    }
    replay_index = replay_index % 10;
    MemoryDir(port, namelist[replay_index], &replay_file[port][replay_index]);
    replay_index++;
}

int rep_file_exist_check(int port, int id)
{
    return replay_file[port][id];
}

extern SYS_COND sc;

void clr_preaction(void)
{
    rp.pre_action = 0;
    sc.replay_first = 0;
}

void InitGameinfo_ByGameOver(int side);
extern GAME_INFOMATION g_info;
extern short UseCharacter1p2p[];
extern int StageNo;

void replay_init(void)
{
    int side;

    for (side = 0; side < 2; side++) {
        g_info.LoadPlayerID[side] = -1;
        UseCharacter1p2p[side] = 0;
        InitGameinfo_ByGameOver(side);
    }
    StageNo = 0;
}

extern SYS_COND sc;

static void replay_local_init(void)
{
    int cnt;

    sc.replay = 0;
    rp.count = 0;
    for (cnt = 0; cnt < 3600; cnt++) {
        rp.rep_dat[cnt] = -1;
        rp.rep_trg[cnt] = -1;
        rp.rep_off[cnt] = -1;
        rp.rep_old[cnt] = -1;
    }
}

extern SYS_COND sc;
extern int BattleStatus;

void replay_status_chk(void)
{
    if (!sc.replay) {
        return;
    }
    if (rp.count >= 3600) {
        return;
    }
    if (rp.count == 0 && BattleStatus >= 4) {
        return;
    }
    if (sc.replay == 1) {
        replay_recording();
    }
}

extern USER_SETTING userinfo;

static short make_replaydat(u_int dat, int side)
{
    short tmp;

    tmp = (dat & userinfo.key[side].punch) != 0;
    if (dat & userinfo.key[side].kick) {
        tmp |= 2;
    }
    if (dat & userinfo.key[side].slash) {
        tmp |= 4;
    }
    if (dat & userinfo.key[side].hslash) {
        tmp |= 8;
    }
    if (dat & userinfo.key[side].fuckin) {
        tmp |= 0x10;
    }
    if (dat & 0x1000) {
        tmp |= 0x20;
    }
    if (dat & 0x4000) {
        tmp |= 0x40;
    }
    if (dat & 0x8000) {
        tmp |= 0x80;
    }
    if (dat & 0x2000) {
        tmp |= 0x100;
    }
    return tmp;
}

extern PAD pads[2];

static void encode_replaydat(int side)
{
    rp.rep_dat[rp.count] = make_replaydat(pads[side].data, side);
    rp.rep_trg[rp.count] = make_replaydat(pads[side].trg, side);
    rp.rep_off[rp.count] = make_replaydat(pads[side].offtrg, side);
    rp.rep_old[rp.count] = make_replaydat(pads[side].old, side);
}

void replay_recording(void)
{
    encode_replaydat(rp.side);
    rp.count++;
}

extern USER_SETTING userinfo;

static u_int make_inputdat(short dat, int side)
{
    u_int tmp;

    tmp = 0;
    if (dat & 1) {
        tmp = userinfo.key[side].punch;
    }
    if (dat & 2) {
        tmp |= userinfo.key[side].kick;
    }
    if (dat & 4) {
        tmp |= userinfo.key[side].slash;
    }
    if (dat & 8) {
        tmp |= userinfo.key[side].hslash;
    }
    if (dat & 0x10) {
        tmp |= userinfo.key[side].fuckin;
    }
    if (dat & 0x20) {
        tmp |= 0x1000;
    }
    if (dat & 0x40) {
        tmp |= 0x4000;
    }
    if (dat & 0x80) {
        tmp |= 0x8000;
    }
    if (dat & 0x100) {
        tmp |= 0x2000;
    }
    return tmp;
}

extern PAD pads[2];

static void decode_replaydat(int side)
{
    pads[side].data = make_inputdat(rp.rep_dat[rp.count], side);
    pads[side].trg = make_inputdat(rp.rep_trg[rp.count], side);
    pads[side].offtrg = make_inputdat(rp.rep_off[rp.count], side);
    pads[side].old = make_inputdat(rp.rep_old[rp.count], side);
}

extern PAD pads[2];
extern SYS_COND sc;
extern int BattleStatus;
extern int GlobalHitStop;

void replay_playback(void)
{
    if (sc.replay) {
        if ((rp.count != 0 || BattleStatus < 4) && rp.count < 3600) {
            if (GlobalHitStop <= 0) {
                decode_replaydat(rp.side);
            } else {
                pads[rp.side].data = 0;
                pads[rp.side].trg = 0;
                pads[rp.side].offtrg = 0;
                pads[rp.side].old = 0;
            }
            rp.count++;
        }
    }
}

void MemoryType(int slot, int *type, int *free, int *exist);

static int replay_chk_connect(int port)
{
    int type;

    MemoryType(port, &type, 0, 0);
    return type;
}

void MemoryType(int slot, int *type, int *free, int *exist);

static int replay_chk_connect_all(void)
{
    int i;
    int type;

    for (i = 0; i < 2; i++) {
        MemoryType(i, &type, 0, 0);
        if (type == 4) {
            return 1;
        }
        if (type == 3) {
            return 2;
        }
    }
    return 0;
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);

static void replay_mode_disp(HEADWORK *hw, int pos)
{
    int i;
    int alpha;

    FontDrawDC("RECORDING MODE", 32.0f, 40.0f, 4.0f, 1, 0);
    for (i = 0; i < 4; i++) {
        alpha = (pos == i) ? 1 : 160;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 4.0f, alpha, 1);
    }
}

void nlSetBackColor(int c1, int c2, int c3);

static void replay_fade_bg(void)
{
    static int num = 0;
    static int frame = 0;
    int col[3];
    int bg[3] = { 0xFF400000, 0xFF200000, 0xFF000000 };
    int i;

    for (i = 0; i < 3; i++) {
        col[i] = ((bg[i] & 0xFFFFFF) >> 16) - num - frame;
        if (col[i] < 0) {
            col[i] = 0;
        }
        col[i] = (bg[i] & 0xFF00FFFF) | (col[i] << 16);
    }
    nlSetBackColor(col[0], col[1], col[2]);
    if (frame >= 2) {
        frame = 0;
        num += 8;
    } else {
        frame++;
    }
}

void CreditFontLoad(int id);
void FadeInit(void);
void InitCancelFlag(void);
void PXF_SpriteClear(void);
void dcfont_init(void);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
extern short UseCharacter1p2p[];
extern GAME_INFOMATION g_info;

static void replay_mode_init(void)
{
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    PXF_SpriteClear();
    FadeInit();
    dcfont_init();
    CreditFontLoad(20);
    InitCancelFlag();
    g_info.LoadPlayerID[0] = -1;
    UseCharacter1p2p[0] = 0;
    UseCharacter1p2p[1] = 0;
    g_info.LoadPlayerID[1] = -1;
}

void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void nlSetBackColor(int c1, int c2, int c3);

static void replay_mode_exit(void)
{
    nlSetBackColor(0, 0, 0);
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
}

extern short UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern SYS_COND sc;
extern int StageNo;

void set_replay_info(void)
{
    sc.replay_first = 1;
    rp.id[0] = UseCharacter1p2p[0];
    rp.id[1] = UseCharacter1p2p[1];
    rp.col[0] = g_info.PlayerPalette[0];
    rp.col[1] = g_info.PlayerPalette[1];
    rp.bg = StageNo;
    rp.voice[0] = g_info.PlayerVoice[0];
    rp.voice[1] = g_info.PlayerVoice[1];
    rp.ggmode[0] = userinfo.gguse[0];
    rp.ggmode[1] = userinfo.gguse[1];
}

void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu);
void SetCPUCharacterFlag(int playerside, int flag);
void SetStageNo(int no);
extern short UseCharacter1p2p[];
extern GAME_INFOMATION g_info;

void get_replay_info(void)
{
    int i;

    rp.count = 0;
    for (i = 0; i < 2; i++) {
        UseCharacter1p2p[i] = rp.id[i];
        SetCPUCharacterFlag(i, 0);
        EntrySelectCharacter(i, rp.id[i], rp.col[i], 0);
        g_info.PlayerSelected[i] = 1;
        g_info.PlayerVoice[i] = rp.voice[i];
    }
    SetStageNo(rp.bg);
}

int check_replay_info(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (rp.id[i] < 1 || rp.id[i] > 18) {
            return 1;
        }
        if (rp.col[i] < 0 || rp.col[i] > 10) {
            return 1;
        }
        if (rp.voice[i] < 0 || rp.voice[i] > 2) {
            return 1;
        }
        if (rp.ggmode[i] < 0 || rp.ggmode[i] > 7) {
            return 1;
        }
    }
    if (rp.bg < 1 || rp.bg > 12) {
        return 1;
    }
    if (rp.sound < 1 || rp.sound > 36) {
        return 1;
    }
    return 0;
}

void set_replay_sound(int no)
{
    rp.sound = no;
}

int get_replay_sound(void)
{
    return rp.sound;
}

void FontDraw(char *str, int x, int y, float z, int alfa);
int MemoryCheck(void);
void MemoryErrClear(void);
void MemoryFormatAndSave(int slot);
void MemorySave(int slot);
void MemorySetReplayDir(int id, char *data, int size);

int replay_datasave(int sw)
{
    static int mode;
    static int err;

    if (sw) {
        mode = 0;
        err = 0;
        return 0;
    }
    if (mode == 0) {
        if (MemoryCheck() > 0) {
            return 0;
        }
        mode++;
    } else if (mode == 1) {
        if (replay_chk_connect(rdinfo.port) == 3) {
            MemorySetReplayDir(rdinfo.fnum, (char *)&rp, sizeof(VS_REPLAY));
            MemoryFormatAndSave(rdinfo.port);
        } else {
            MemorySetReplayDir(rdinfo.fnum, (char *)&rp, sizeof(VS_REPLAY));
            MemorySave(rdinfo.port);
        }
        mode = 2;
    } else if (mode == 2) {
        err = MemoryCheck();
        if (err == 0) {
            MemoryErrClear();
            return 1;
        }
        if (err < 0) {
            mode = 3;
        }
        if (rdinfo.port == 0) {
            FontDraw("MEMORY CARD SLOT 1", 384, 264, 2.0f, 255);
        } else {
            FontDraw("MEMORY CARD SLOT 2", 384, 264, 2.0f, 255);
        }
        FontDraw("NOW SAVING", 424, 288, 2.0f, 255);
        FontDraw("PLEASE", 392, 328, 2.0f, 255);
        FontDraw("DO NOT REMOVE", 392, 348, 2.0f, 255);
        FontDraw("THE MEMORY CARD", 392, 368, 2.0f, 255);
        FontDraw("            (PS2)", 392, 388, 2.0f, 255);
    } else {
        if (rdinfo.port == 0) {
            FontDraw("MEMORY CARD SLOT 1", 384, 264, 2.0f, 255);
        } else {
            FontDraw("MEMORY CARD SLOT 2", 384, 264, 2.0f, 255);
        }
        if (err == -7) {
            FontDraw("FORMAT ERROR", 424, 288, 2.0f, 255);
        } else {
            FontDraw("SAVE ERROR", 424, 288, 2.0f, 255);
        }
        if (++mode >= 181) {
            return -1;
        }
    }
    return 0;
}

void FontDraw(char *str, int x, int y, float z, int alfa);
int sound_request(int no);
extern PAD pads[2];
extern SYS_COND sc;

int replay_select_savechk(int sw)
{
    int i;
    int alpha;
    int ret;
    PARTS pt[2] = {
        { "YES", 432.0f, 288.0f, 0 },
        { "NO", 512.0f, 288.0f, 0 },
    };

    if (sw) {
        rdinfo.pos = 0;
    }
    FontDraw("REPLAY SAVE?", 416, 216, 2.0f, 255);
    for (i = 0; i < 2; i++) {
        alpha = (rdinfo.pos == i) ? 255 : 80;
        FontDraw(pt[i].head, pt[i].xp, pt[i].yp, 2.0f, alpha);
    }
    if ((pads[0].trg | pads[1].trg) & 0xA000) {
        sound_request(0x37);
        rdinfo.pos ^= 1;
    }
    if ((pads[0].trg | pads[1].trg) & 0x20) {
        sound_request(0x39);
        ret = rdinfo.pos != 0;
        sc.grpvar[1] = ret;
        return 1;
    }
    if ((pads[0].trg | pads[1].trg) & 0x40) {
        sound_request(0x3B);
        sc.grpvar[1] = 1;
        return 1;
    }
    return 0;
}

void FontDraw(char *str, int x, int y, float z, int alfa);
char *G_sprintf(char *buff, char *str, int *valtb);
int vm_get_freeblock(int port);
extern char *vmrp_fname[10];

static void disp_save_selectfile(void)
{
    int i;
    int num;
    int alpha;
    char *portname[2] = { "SLOT1", "SLOT2" };
    int val[2];
    int type;

    FontDraw("SAVE REPLAY FILE", 368, 144, 2.0f, 255);
    for (i = 0; i < 2; i++) {
        alpha = (i == rdinfo.port) ? 255 : 80;
        FontDraw(portname[i], 368 + i * 116, 168, 2.0f, alpha);
    }
    num = 0;
    type = replay_chk_connect(rdinfo.port);
    if (type == 4) {
        goto list;
    }
    if (type == 3) {
    list:
        rep_file_exist_job(rdinfo.port);
        if (replay_check_flag <= 0) {
            for (i = 0; i < 10; i++) {
                if (rdinfo.pos == num) {
                    rdinfo.fnum = i;
                    alpha = (replay_check_flag > 0) ? 80 : 255;
                } else {
                    alpha = 80;
                }
                FontDraw(vmrp_fname[i], 400, 192 + num * 24, 2.0f, alpha);
                if (type > 0 && rep_file_exist_check(rdinfo.port, i)) {
                    FontDraw("@", 384, 192 + num * 24, 2.0f, 255);
                }
                num++;
            }
        }
        if (type == 4) {
            val[0] = vm_get_freeblock(rdinfo.port);
            FontDraw(G_sprintf(0, "%4dKB    FREE", val), 424, 432, 2.0f, 255);
        } else {
            FontDraw("UNFORMATTED", 424, 432, 2.0f, 255);
            for (i = 9; i >= 0; i--) {
                replay_file[rdinfo.port][i] = 0;
            }
        }
    } else {
        rep_file_exist_clr(rdinfo.port);
        if (replay_chk_connect(rdinfo.port) == 3) {
            FontDraw("UNFORMATTED", 384, 192, 2.0f, 80);
            for (i = 9; i >= 0; i--) {
                replay_file[rdinfo.port][i] = 0;
            }
        } else {
            replay_check_flag = -1;
            FontDraw("NOT CONNECT", 384, 192, 2.0f, 80);
            num = -1;
        }
    }
    rdinfo.max = num;
}

void MemoryOpen(void);
void MemorySetNoDir(void);
extern SYS_COND sc;

void FontDraw(char *str, int x, int y, float z, int alfa);
int sound_request(int no);
int iodat_chk(int stat);
int vm_get_freeblock(int port);
extern PAD pads[2];
extern SYS_COND sc;

int replay_save_operate(int sw)
{
    static int ret;
    static int alpha;
    static int pad;
    static int mode;
    static int pos = 0;

    if (sw) {
        rdinfo.port = 0;
        rdinfo.fnum = 0;
        rdinfo.pos = 0;
        rdinfo.max = 0;
        ret = -1;
        replay_check_flag = 1;
        mode = 0;
        return 0;
    }
    if (mode == 0) {
        if (ret == -1) {
            if (replay_check_flag > 0) {
                disp_save_selectfile();
                return 0;
            }
            if ((pad = iodat_chk(0x1000)) != 0) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    sound_request(0x37);
                    if (--rdinfo.pos < 0) {
                        rdinfo.pos = 9;
                    }
                }
                sc.cnt[pad - 1]++;
            } else if ((pad = iodat_chk(0x4000)) != 0) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    sound_request(0x37);
                    if (++rdinfo.pos >= 10) {
                        rdinfo.pos = 0;
                    }
                }
                sc.cnt[pad - 1]++;
            } else if ((pad = iodat_chk(0x8000)) != 0) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    replay_index = 0;
                    replay_check_flag = 1;
                    sound_request(0x37);
                    if (--rdinfo.port < 0) {
                        rdinfo.port = 1;
                    }
                    rdinfo.fnum = 0;
                    rdinfo.pos = 0;
                }
                sc.cnt[pad - 1]++;
            } else if ((pad = iodat_chk(0x2000)) != 0) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    replay_index = 0;
                    replay_check_flag = 1;
                    sound_request(0x37);
                    if (++rdinfo.port >= 2) {
                        rdinfo.port = 0;
                    }
                    rdinfo.fnum = 0;
                    rdinfo.pos = 0;
                }
                sc.cnt[pad - 1]++;
            } else {
                sc.cnt[0] = 0;
                sc.cnt[1] = 0;
            }
            disp_save_selectfile();
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                if (replay_chk_connect(rdinfo.port) == 3) {
                    sound_request(0x39);
                    mode = 3;
                    pos = 0;
                }
            }
            if (((pads[0].trg | pads[1].trg) & 0x20) && rdinfo.max > 0) {
                if (replay_chk_connect(rdinfo.port) == 3) {
                    mode = 3;
                    pos = 0;
                } else if (vm_get_freeblock(rdinfo.port) < 96 && !rep_file_exist_check(rdinfo.port, rdinfo.fnum)) {
                    sound_request(0x39);
                    mode = 1;
                    ret = -1;
                } else if (!rep_file_exist_check(rdinfo.port, rdinfo.fnum)) {
                    sound_request(0x39);
                    ret = 0;
                    sc.grpvar[1] = ret;
                    return 1;
                } else {
                    sound_request(0x39);
                    pos = 0;
                    mode = 2;
                }
            }
            if ((pads[0].trg | pads[1].trg) & 0x40) {
                sound_request(0x3B);
                ret = 1;
                sc.grpvar[1] = ret;
                return 1;
            }
            return 0;
        } else {
            sc.grpvar[1] = ret;
            return 1;
        }
    } else if (mode == 1) {
        if (rdinfo.port == 0) {
            FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
        } else {
            FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
        }
        FontDraw("SHORT OF MEMORY!!", 384, 216, 2.0f, 255);
        FontDraw("96KB IS NEEDED", 384, 336, 2.0f, 255);
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            mode = 0;
        }
    } else if (mode == 2) {
        if (rdinfo.port == 0) {
            FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
        } else {
            FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
        }
        FontDraw("FILE OVERWRITE?", 400, 216, 2.0f, 255);
        alpha = pos ? 80 : 255;
        FontDraw("YES", 432, 288, 2.0f, alpha);
        alpha = pos ? 255 : 80;
        FontDraw("NO", 512, 288, 2.0f, alpha);
        if (vm_get_freeblock(rdinfo.port) < 0) {
            mode = 0;
        } else {
            if ((pads[0].trg | pads[1].trg) & 0xA000) {
                sound_request(0x37);
                pos ^= 1;
            }
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                sound_request(0x39);
                if (pos == 0) {
                    ret = 0;
                }
                mode = 0;
            } else if ((pads[0].trg | pads[1].trg) & 0x40) {
                mode = 0;
                ret = 1;
            }
        }
    } else if (mode == 3) {
        if (rdinfo.port == 0) {
            FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
        } else {
            FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
        }
        FontDraw("FORMAT NOW?", 400, 216, 2.0f, 255);
        alpha = pos ? 80 : 255;
        FontDraw("NO", 432, 288, 2.0f, alpha);
        alpha = pos ? 255 : 80;
        FontDraw("YES", 512, 288, 2.0f, alpha);
        if (vm_get_freeblock(rdinfo.port) != -1) {
            mode = 0;
        } else {
            if ((pads[0].trg | pads[1].trg) & 0xA000) {
                sound_request(0x37);
                pos ^= 1;
            }
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                sound_request(0x39);
                if (pos) {
                    ret = 0;
                }
                mode = 0;
            } else if ((pads[0].trg | pads[1].trg) & 0x40) {
                mode = 0;
            }
        }
    }
    return 0;
}


void ReplaySaveInit(void)
{
    MemorySetNoDir();
    MemoryOpen();
    ReplaySaveMode = 0;
    sc.grpvar[1] = 0;
}

void MemoryClose(void);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
extern SYS_COND sc;

int ReplaySaveMain(void)
{
    int err1;
    int err2;

    if (ReplaySaveMode == 0) {
        err1 = replay_chk_connect(0);
        err2 = replay_chk_connect(1);
        if (err1 == 0 || err2 == 0) {
            return 0;
        }
        replay_select_savechk(1);
        ReplaySaveMode = 1;
    } else if (ReplaySaveMode == 1) {
        if (replay_select_savechk(0)) {
            if (sc.grpvar[1]) {
                return 1;
            }
            sc.grpvar[1] = 0;
            replay_save_operate(1);
            ReplaySaveMode = 2;
        }
    } else if (ReplaySaveMode == 2) {
        if (replay_save_operate(0)) {
            if (sc.grpvar[1]) {
                MemoryClose();
                return 1;
            }
            replay_datasave(1);
            ReplaySaveMode = 3;
        }
    } else if (ReplaySaveMode == 3) {
        err1 = replay_datasave(0);
        if (err1 > 0) {
            MemoryClose();
            return 1;
        }
        if (err1 < 0) {
            ReplaySaveInit();
            replay_save_operate(1);
            ReplaySaveMode = 2;
            return 0;
        }
    }
    polydraw_FBox(368.0f, 144.0f, 600.0f, 456.0f, 8.0f, gStripGetFadeColor(0xC01F1F1F));
    return 0;
}

int get_player(void)
{
    return rp.side;
}

int Random(void);
void RandomInit(int num);
extern SYS_COND sc;

void set_random_seed(void)
{
    if (sc.replay == 1) {
        rp.seed = Random();
        RandomInit(rp.seed);
    } else if (sc.replay == 2) {
        RandomInit(rp.seed);
    }
}

void RandomInit(int num);

void set_random_seed_abs(void)
{
    RandomInit(rp.seed);
}

int iodat_chk(int stat);
int sound_request(int no);
extern SYS_COND sc;


static void disp_replayfile(void)
{
    int i;
    int num;
    int ret;
    int alpha;
    char *portname[2] = { "SLOT1", "SLOT2" };
    int val[2];

    polydraw_FBox(368.0f, 144.0f, 600.0f, 456.0f, 8.0f, gStripGetFadeColor(0xC01F1F1F));
    FontDraw("SELECT REPLAY FILE", 368, 144, 2.0f, 255);
    for (i = 0; i < 2; i++) {
        alpha = (i == rdinfo.port) ? 255 : 80;
        FontDraw(portname[i], 368 + i * 116, 168, 2.0f, alpha);
    }
    num = 0;
    if (replay_chk_connect(rdinfo.port) == 4) {
        rep_file_exist_job(rdinfo.port);
        if (replay_check_flag <= 0) {
            for (i = 0; i < 10; i++) {
                if (rep_file_exist_check(rdinfo.port, i) > 0) {
                    alpha = 80;
                    if (rdinfo.pos == num) {
                        rdinfo.fnum = i;
                        alpha = 255;
                    }
                    FontDraw(vmrp_fname[i], 384, 192 + num * 24, 2.0f, alpha);
                    num++;
                }
            }
            if (num == 0) {
                FontDraw("NO DATA", 384, 192, 2.0f, 80);
            }
            ret = vm_get_freeblock(rdinfo.port);
            if (ret < 0) {
                ret = 0;
            }
            val[0] = ret;
            FontDraw(G_sprintf(0, "%4dKB    FREE", val), 424, 432, 2.0f, 255);
        }
    } else {
        rep_file_exist_clr(rdinfo.port);
        if (replay_chk_connect(rdinfo.port) == 3) {
            FontDraw("UNFORMATTED", 384, 192, 2.0f, 80);
            replay_check_flag = -1;
        } else {
            FontDraw("NOT CONNECT", 384, 192, 2.0f, 80);
            replay_check_flag = -1;
        }
        num = -1;
    }
    rdinfo.max = num;
}


void replay_select_file(void)
{
    int pad;

    if ((pad = iodat_chk(0x1000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            if (rdinfo.max > 1) {
                sound_request(0x37);
            }
            if (--rdinfo.pos < 0) {
                rdinfo.pos = rdinfo.max - 1;
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x4000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            if (rdinfo.max > 1) {
                sound_request(0x37);
            }
            if (++rdinfo.pos >= rdinfo.max) {
                rdinfo.pos = 0;
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x8000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            replay_index = 0;
            replay_check_flag = 1;
            sound_request(0x37);
            if (--rdinfo.port < 0) {
                rdinfo.port = 1;
            }
            rdinfo.fnum = 0;
            rdinfo.pos = 0;
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x2000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            replay_index = 0;
            replay_check_flag = 1;
            sound_request(0x37);
            if (++rdinfo.port >= 2) {
                rdinfo.port = 0;
            }
            rdinfo.fnum = 0;
            rdinfo.pos = 0;
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
}

int get_ggmode(int side)
{
    return rp.ggmode[side];
}

int iodat_chk(int stat);
int sound_request(int no);
extern SYS_COND sc;

void rep_select_mode(void)
{
    int pad;

    if ((pad = iodat_chk(0x1000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            if (--rep_cursor < 0) {
                rep_cursor = 3;
            }
            if (!replay_chk_connect_all()) {
                if (rep_cursor == 2) {
                    rep_cursor = 1;
                }
                if (rep_cursor == 1 && !sc.replay_first) {
                    rep_cursor = 0;
                }
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x4000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            if (++rep_cursor >= 4) {
                rep_cursor = 0;
            }
            if (!replay_chk_connect_all()) {
                if (rep_cursor == 1 && !sc.replay_first) {
                    rep_cursor = 3;
                }
                if (rep_cursor == 2) {
                    rep_cursor = 3;
                }
            }
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
}

int rep_id_get(int port, int id)
{
    int i;

    for (i = 0; i < 10; i++) {
        if (rep_file_exist_check(rdinfo.port, i) > 0) {
            if (id-- == 0) {
                return i;
            }
        }
    }
    return 0;
}

void Init_Game_Info(void);
void JobNext(void);
void MemoryOpen(void);
extern int SubJob;

void ReplayInit(void)
{
    Init_Game_Info();
    replay_mode_init();
    rep_cursor = 0;
    rep_nextjob = 0;
    SubJob = 0;
    MemoryOpen();
    rep_file_exist_clr(0);
    rep_file_exist_clr(1);
    replay_enable = 0;
    JobNext();
}

void FontDraw(char *str, int x, int y, float z, int alfa);
int sound_request(int no);
int iotrg_chk(int stat);
int Cr_Request_Start(int side);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
void Change_ScreenScale(int i);
void JobChange(int job);
int IsFadeEnd(void);
void Rq_Fadeout(void);
void SetFadeSpeed(int frame);
int MemoryCheck(void);
void MemoryErrClear(void);
void MemoryLoad(int slot);
void MemoryDelete(int slot);
void MemoryOpen(void);
void MemoryClose(void);
void MemorySetReplayDir(int id, char *data, int size);
extern PAD pads[2];
extern SYS_COND sc;
extern GAME_INFOMATION g_info;
extern int SubJob;
extern int SubJobCount;


void ReplayMain(void)
{
    HEADWORK hw[4] = {
        { "RECORDING", 112.0f, 144.0f, 0, 0, 0 },
        { "REPLAY", 112.0f, 176.0f, 0, 0, 0 },
        { "DATA DELETE", 112.0f, 208.0f, 0, 0, 0 },
        { "TITLE", 112.0f, 240.0f, 0, 0, 0 },
    };
    int pad;
    int ret;

    switch (SubJob) {
    case 0:
        replay_mode_disp(hw, rep_cursor);
        polydraw_GBox(0.0f, 0.0f, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        rep_select_mode();
        if ((pad = iotrg_chk(0x20))) {
            rp.side = pad - 1;
            sound_request(0x39);
            switch (rep_cursor) {
            case 0:
                replay_init();
                replay_local_init();
                rp.count = 0;
                g_info.mode = (1 << rp.side) | 0x4000;
                g_info.ModeFlag = 1;
                Cr_Request_Start(pad - 1);
                rep_nextjob = 12;
                SubJob = 10;
                sc.replay = 1;
                sc.replay_first = 0;
                SubJobCount = 0;
                break;
            case 1:
                if (replay_chk_connect_all()) {
                    SubJob = 1;
                    sc.grpvar[1] = 0;
                    SubJobCount = 0;
                } else if (sc.replay_first) {
                    replay_init();
                    replay_enable = 1;
                }
                if (replay_enable) {
                    g_info.mode = (1 << rp.side) | 0x4000;
                    g_info.ModeFlag = 1;
                    Cr_Request_Start(rp.side);
                    get_replay_info();
                    rep_nextjob = 30;
                    sc.replay = 2;
                    SubJob = 10;
                    SubJobCount = 0;
                }
                break;
            case 2:
                SubJobCount = 0;
                SubJob = 2;
                break;
            case 3:
                rep_nextjob = 3;
                SubJob = 10;
                SubJobCount = 0;
                break;
            }
        } else if ((pads[0].trg | pads[1].trg) & 0x40) {
            sound_request(0x3B);
            rep_nextjob = 3;
            SubJob = 10;
            SubJobCount = 0;
        }
        break;
    case 1:
        replay_mode_disp(hw, rep_cursor);
        polydraw_GBox(0.0f, 0.0f, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        if (SubJobCount == 0) {
            rdinfo.port = 0;
            rdinfo.fnum = 0;
            rdinfo.pos = 0;
            rdinfo.max = 0;
            replay_check_flag = 1;
            SubJobCount = 1;
            break;
        }
        if (replay_check_flag > 0) {
            disp_replayfile();
            break;
        }
        replay_select_file();
        disp_replayfile();
        if (((pads[0].trg | pads[1].trg) & 0x20) && rdinfo.max > 0) {
            sound_request(0x39);
            replay_init();
            replay_local_init();
            sc.replay_first = 0;
            SubJob = 3;
            replay_enable = 0;
            SubJobCount = 0;
        } else if ((pads[0].trg | pads[1].trg) & 0x40) {
            sc.grpvar[1] = 1;
            sound_request(0x3B);
            replay_enable = 0;
            SubJob = 0;
        } else {
            replay_enable = 1;
        }
        break;
    case 2:
        replay_mode_disp(hw, rep_cursor);
        polydraw_GBox(0.0f, 0.0f, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        if (SubJobCount == 0) {
            rdinfo.port = 0;
            rdinfo.fnum = 0;
            rdinfo.pos = 0;
            rdinfo.max = 0;
            replay_check_flag = 1;
            SubJobCount = 1;
            break;
        }
        if (replay_check_flag > 0) {
            disp_replayfile();
            break;
        }
        replay_select_file();
        disp_replayfile();
        if (((pads[0].trg | pads[1].trg) & 0x20) && rdinfo.max > 0) {
            sound_request(0x39);
            SubJobCount = 0;
            SubJob = 4;
        } else if ((pads[0].trg | pads[1].trg) & 0x40) {
            sound_request(0x3B);
            SubJob = 0;
            SubJobCount = 0;
        }
        break;
    case 3:
        replay_mode_disp(hw, rep_cursor);
        polydraw_GBox(0.0f, 0.0f, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        if (SubJobCount == 0) {
            MemorySetReplayDir(rep_id_get(rdinfo.port, rdinfo.pos), (char *)&rp, sizeof(VS_REPLAY));
            MemoryLoad(rdinfo.port);
            SubJobCount++;
            break;
        }
        ret = MemoryCheck();
        if (ret < 0) {
            if (++SubJobCount >= 181) {
                MemoryErrClear();
                SubJob = 1;
                replay_init();
                MemoryOpen();
                break;
            }
            if (rdinfo.port == 0) {
                FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
            } else {
                FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
            }
            FontDraw("LOAD ERROR", 416, 216, 2.0f, 255);
            if (ret == -1) {
                FontDraw("DATA IS CORRUPTED", 416, 240, 2.0f, 255);
            }
        } else if (ret == 0) {
            if (check_replay_info()) {
                if (++SubJobCount >= 181) {
                    MemoryErrClear();
                    SubJob = 1;
                    replay_init();
                    MemoryOpen();
                    break;
                }
                if (rdinfo.port == 0) {
                    FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
                } else {
                    FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
                }
                FontDraw("LOAD ERROR", 416, 216, 2.0f, 255);
                FontDraw("DATA IS CORRUPTED", 416, 240, 2.0f, 255);
            } else {
                SubJob = 0;
                replay_init();
                g_info.mode = (1 << rp.side) | 0x4000;
                g_info.ModeFlag = 1;
                Cr_Request_Start(rp.side);
                get_replay_info();
                Change_ScreenScale(1);
                rep_nextjob = 30;
                sc.replay = 2;
                SubJob = 10;
            }
        } else {
            if (rdinfo.port == 0) {
                FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
            } else {
                FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
            }
            FontDraw("LOADING", 416, 216, 2.0f, 255);
        }
        break;
    case 4:
        replay_mode_disp(hw, rep_cursor);
        polydraw_GBox(0.0f, 0.0f, 640.0f, 480.0f, 100.0f, 0xFF400000, 0xFF200000, 0xFF200000, 0xFF000000);
        if (SubJobCount == 0) {
            MemorySetReplayDir(rep_id_get(rdinfo.port, rdinfo.pos), (char *)&rp, sizeof(VS_REPLAY));
            MemoryDelete(rdinfo.port);
            SubJobCount++;
            break;
        }
        ret = MemoryCheck();
        if (ret < 0) {
            if (++SubJobCount >= 181) {
                MemoryErrClear();
                SubJobCount = 0;
                SubJob = 2;
                replay_init();
                MemoryOpen();
                break;
            }
            if (rdinfo.port == 0) {
                FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
            } else {
                FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
            }
            FontDraw("DELETE ERROR", 416, 216, 2.0f, 255);
        } else if (ret == 0) {
            if (++SubJobCount >= 181) {
                rep_file_exist_clr(0);
                rep_file_exist_clr(1);
                MemoryErrClear();
                SubJobCount = 0;
                SubJob = 2;
                replay_init();
                MemoryOpen();
                break;
            }
            if (rdinfo.port == 0) {
                FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
            } else {
                FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
            }
            FontDraw("DELETE COMPLETE", 416, 216, 2.0f, 255);
        } else {
            if (rdinfo.port == 0) {
                FontDraw("MEMORY CARD SLOT 1", 384, 192, 2.0f, 255);
            } else {
                FontDraw("MEMORY CARD SLOT 2", 384, 192, 2.0f, 255);
            }
            FontDraw("DELETING", 416, 216, 2.0f, 255);
        }
        break;
    case 10:
        if (SubJobCount == 0) {
            replay_mode_disp(hw, rep_cursor);
            replay_fade_bg();
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJobCount++;
        } else if (!IsFadeEnd()) {
            replay_mode_disp(hw, rep_cursor);
            replay_fade_bg();
        } else {
            MemoryClose();
            JobChange(rep_nextjob);
            replay_mode_exit();
        }
        break;
    }
}

