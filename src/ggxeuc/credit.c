#include "generated/credit.h"

void FontDraw(char *str, int x, int y, float z, int alfa);
extern USER_SETTING userinfo;
int strlen(char *s);
void InitGameinfo_ByGameOver(int side);
extern GAME_INFOMATION g_info;
void ClearScore(int side);
extern PAD pads[2];
int g_roundmin(int val, int min);
extern SCOREDATA_BYCHARACTER D_55CFC0[];
char *G_sprintf_arg1(char *str, int arg1);
int NameEntryExecute(NAMEENTRYCONTROL *p, int side);
void HighScore_EntryName(SCOREDATA_BYCHARACTER *p, int idno, int rank, char *name);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
char *strcat(char *d, char *s);
void NameEntryInit(NAMEENTRYCONTROL *p);
int HighScore_Entry(SCOREDATA_BYCHARACTER *p, int idno, unsigned int score);
static const short idtb[] = { 1, 2, 3 };
int Score_GetScorerate(int side);
char *G_sprintf_arg2(char *str, int arg1, int arg2);
char *G_sprintf_arg3(char *str, int arg1, int arg2, int arg3);
extern int JobMode;
extern int winner_mode;
extern SYS_COND sc;
extern CHARANGE_CTRL *gl_info;
extern SURVIVAL_INFOMATION *sv_info;
extern TRAINING_SETTING *tr_info;
extern u_int TotalCounter;
void set_char2num(int num, char *block, int id);
void time_convert(char *buf, u_int time, int id);

int Credit_Init(void) {
    int i;
    for (i = 0; i < 2; i++) {
        cr_ram[i].mode = 0;
        cr_ram[i].time = 0;
        cr_ram[i].gametime = 0;
        cr_ram[i].rank = -1;
        cr_ram[i].totalrank = -1;
        cr_ram[i].charidno = 0;
        cr_ram[i].continueflag = 0;
        InitGameinfo_ByGameOver(i);
    }
    return 0;
}

int Cr_IsStartWait(int side) {
    return cr_ram[side].mode == 0;
}

int Cr_Request_Start(int side) {
    CREDIT_RAM *ram = &cr_ram[side];
    if (ram->mode == 2) ClearScore(side);
    g_info.mode |= 1 << side;
    ram->mode = 1;
    ram->gametime = 0;
    ram->continueflag = 0;
    if (g_info.mode & 0x800) {
        cr_ram[!side].mode = 1;
        cr_ram[!side].gametime = 1;
        cr_ram[!side].continueflag = 1;
    }
    return 0;
}

int Cr_SetContinue(int side) {
    CREDIT_RAM *cr = &cr_ram[side];
    if (cr->mode == 1) {
        cr->mode = 2;
        cr->time = 1200;
        cr->continueflag = 1;
        g_info.PlayerSelected[side] = 0;
        return 1;
    }
    return 0;
}

int Cr_SetContinueTime(int side, int time) {
    CREDIT_RAM *cr = &cr_ram[side];
    if (cr->mode == 2) {
        cr->time = time * 120;
        return 1;
    }
    return 0;
}

int Cr_SetGameOver(int side) {
    CREDIT_RAM *ram = &cr_ram[side];
    NameEntryInit(&ram->nameentry);
    ram->charidno = g_info.SelectPlayerIdno[side];
    if (g_info.mode & 0x2200) {
        ram->rank = 0;
        ram->totalrank = 0;
    } else {
        ram->rank = HighScore_Entry(D_55CFC0, ram->charidno, g_info.score[side]);
        ram->totalrank = HighScore_Entry(D_55CFC0, 0, g_info.score[side]);
    }
    if (ram->rank >= 0 || ram->totalrank >= 0) {
        ram->mode = 5;
        return 0;
    }
 ram->mode = 0; ram->gametime = 0;
    InitGameinfo_ByGameOver(side);
    g_info.mode &= ~(1 << side);
    return 0;
}

void CreditTask(int side) {
    CREDIT_RAM *ram = &cr_ram[side];
    int count;
    switch (ram->mode) {
    case 0:
        break;
    case 2:
        GetCreditInfo(side, &count);
        if (count < 8 && (pads[side].trg & (userinfo.key[side].punch | userinfo.key[side].kick | userinfo.key[side].slash | userinfo.key[side].hslash))) {
            ram->time -= ram->time % 120;
            ram->time = g_roundmin(ram->time - 1, 1);
        }
        if (ram->time <= 0 || --ram->time <= 0) {
            Cr_SetGameOver(side);
        }
        break;
    case 1:
        ram->gametime++;
        break;
    }
}

int Cr_IsPlaying(int side) {
    return cr_ram[side].mode == 1;
}

int Cr_IsNameEntry(int side) {
    int mode = cr_ram[side].mode;
    if (mode == 5) return 5;
    if (mode == 2) return 2;
    return 0;
}

int Cr_IsContenue(int side) {
    return cr_ram[side].mode == 2;
}

int CreditExecute(void) {
    int i;
    for (i = 0; i < 2; i++) {
        CreditTask(i);
    }
    return 0;
}

void GetCreditInfo(int side, int *count) {
    CREDIT_RAM *cr = &cr_ram[side];
    *count = (cr->time + 119) / 120;
}

char *GetScoreInfo(int side) {
    GAME_INFOMATION *p = &g_info;
    if (p->MedalMode[side]) {
        return G_sprintf_arg3(" %dP X%2d %9d ", side + 1, Score_GetScorerate(side), p->score[side]);
    }
    return G_sprintf_arg2(" %dP     %9d ", side + 1, p->score[side]);
}

char *Cr_GetCoinMsg(int stat, int count) {
    int Playing;
    if (stat == 2) {
        if (JobMode == 0x20 && (g_info.BattleControlFlag & 4)) return "PLEASE WAIT";
        return G_sprintf_arg1("CONTINUE? %2d", count);
    } else if (stat == 3) {
        return "INSERT COIN(S)";
    } else if (stat == 4) {
        return "INSERT MORE COIN(S)";
    } else if (stat == 0) {
        if (JobMode == 0x20 && (g_info.BattleControlFlag & 4)) {
            if (g_info.mode & 0x400) return "WATCH";
            return "PLEASE WAIT";
        }
        if (g_info.BattleControlFlag & 0x200) return "PLEASE WAIT";
        if ((winner_mode & 6) && (g_info.BattleControlFlag & 4)) return "PLEASE WAIT";
        Playing = 0;
        if (Cr_IsPlaying(0) || Cr_IsPlaying(1)) Playing = 1;
        if (g_info.mode & 0x400) return "WATCH";
        if (Playing) return "PRESS START";
        return "PRESS START 1P OR 2P";
    }
    return "ERROR!";
}

char *Cr_GetCreditMsg(int stat, int side, int creditcnt) {
    return "FREE PLAY";
}

int Cr_CenterFontDraw(char *str, int x, int y) {
    int off = userinfo.offs_cockpit_upper;
    x -= strlen(str) * 6;
    FontDraw(str, x, y + off, 4.0f, 255);
    return x;
}

int Cr_NameEntry(int side, int basex, int y) {
    CREDIT_RAM *ram = &cr_ram[side];
    int x;
    if (!(g_info.mode & 0x2200)) {
        if (ram->mode != 5) return 0;
        if (NameEntryExecute(&ram->nameentry, side)) {
            if (ram->rank >= 0) HighScore_EntryName(D_55CFC0, ram->charidno, ram->rank, ram->nameentry.name);
            if (ram->totalrank >= 0) HighScore_EntryName(D_55CFC0, 0, ram->totalrank, ram->nameentry.name);
 ram->mode = 0; ram->gametime = 0; ram->rank = -1; ram->totalrank = -1;
            InitGameinfo_ByGameOver(side);
            g_info.mode &= ~(1 << side);
        }
        x = Cr_CenterFontDraw(strcat(G_sprintf_arg1("RANK IN ", 0), ram->nameentry.name), basex, y);
        if (ram->nameentry.cursor >= 0) {
            x += (ram->nameentry.cursor + 8) * 12;
            polydraw_FBox(x - 1, y - 2 + userinfo.offs_cockpit_upper, x + 13, y + 18 + userinfo.offs_cockpit_upper, 5.0f, 0x8000ffff);
        }
        return 1;
    }
    return 0;
}

void DispCreditMsg(void) {
    int i;
    int basex;
    int y;
    int n;
    char buf[16];
    if (Cr_IsPlaying(0) && Cr_IsPlaying(1)) {
        n = g_info.WinCount;
        if (JobMode != 0x20) return;
        for (basex = 160, i = 0; i < 2; i++, basex += 320) {
            if (!(g_info.mode & 0x800)) {
                if (n && g_info.Player == i) {
                    Cr_CenterFontDraw(G_sprintf_arg1("%3dWIN", n), basex, 2);
                } else {
                    Cr_CenterFontDraw("CHALLENGER", basex, 2);
                }
            } else if (g_info.wincnt[i]) {
                Cr_CenterFontDraw(G_sprintf_arg1("%3dWIN", g_info.wincnt[i]), basex, 2);
            } else {
                Cr_CenterFontDraw("CHALLENGER", basex, 2);
            }
        }
    } else if (g_info.mode & 0x2000) {
        if (JobMode != 0x20) return;
        y = userinfo.offs_cockpit_upper + 2;
        FontDraw("STORY MODE", 96, y, 16.0f, 255);
        FontDraw("STORY MODE", 416, y, 16.0f, 255);
    } else if (g_info.mode & 0x200) {
        if (JobMode != 0x20) return;
        y = userinfo.offs_cockpit_upper + 2;
        for (basex = 48, i = 0; i < 2; i++, basex += 320) {
            if (sv_info->ply != i) continue;
            FontDraw("LEVEL:", basex, y, 16.0f, 255);
            set_char2num(sv_info->sv_ext.survival_level, buf, 0);
            FontDraw(buf, basex + 64, y, 16.0f, 255);
            time_convert(buf, sv_info->sv_ext.survival_totaltime, 0);
            FontDraw(buf, basex + 116, y, 16.0f, 255);
            disp_koron(basex + 116, y);
        }
    } else {
        if (JobMode != 0x20) return;
        for (basex = 160, i = 0; i < 2; i++, basex += 320) {
            if ((g_info.mode & 0x100) && tr_info->ply != i) continue;
            if ((g_info.mode & 0x200) && sv_info->ply != i) continue;
            if ((g_info.mode & 0x8000) && gl_info->ply != i) continue;
            if (g_info.mode & 0x4000) {
                if (sc.replay == 1) {
                    Cr_CenterFontDraw("RECORDING", basex, 2);
                } else if (sc.replay == 2) {
                    Cr_CenterFontDraw("PLAYBACK", basex, 2);
                }
            } else if (Cr_IsPlaying(i)) {
                Cr_CenterFontDraw(GetScoreInfo(i), basex, 2);
            } else if (Cr_NameEntry(i, basex, 2)) {
            } else if ((TotalCounter & 0x40) || cr_ram[i].mode == 2) {
                Cr_CenterFontDraw(Cr_GetCoinMsg(cr_ram[i].mode, cr_ram[i].time / 120), basex, 2);
            } else if (!(g_info.mode & 0x400)) {
                Cr_CenterFontDraw(Cr_GetCreditMsg(cr_ram[i].mode, i, 1), basex, 2);
            }
        }
    }
}

void disp_koron(int x, int y) {
    disp_koron_one(x + 24, y);
    disp_koron_one(x + 60, y);
}

void disp_koron_one(int x, int y) {
    FontDraw(".", x, y, 16.0f, 255);
    FontDraw(".", x, y - 8, 16.0f, 255);
}

void DispWinnerCredit(void) {
    int i;
    int basex;
    if (g_info.mode & 0x2200) return;
    for (i = 0, basex = 160; i < 2; i++, basex += 320) {
        if (Cr_NameEntry(i, basex, 2)) continue;
        if (Cr_IsContenue(i)) Cr_CenterFontDraw(GetScoreInfo(i), basex, 2);
    }
}

void CreditClr(void) {
}

int Cr_IsCreditEnough(int side) {
    return 1;
}

int Cr_GetCrditCnt(int side) {
    return 1;
}
