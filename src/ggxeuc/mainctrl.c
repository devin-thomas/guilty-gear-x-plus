#include "generated/mainctrl.h"

static const short idtb[] = { 1, 2, 3 };

void InitDemoBattle(void);
void JobChange(int job);

void WatchInit(void) {
    InitDemoBattle();
    JobChange(0x15);
}

extern USER_SETTING userinfo;
extern SYS_COND sc;

int IsEvent(u_int flag) {
    return (((userinfo.phase | sc.kakusi) << 8) & flag) != 0;
}

extern GAME_INFOMATION g_info;

void SetCPUCharacterFlag(int playerside, int flag) {
    u_int v;

    g_info.isCPU[playerside] = flag;
    v = *(u_int *)&g_info.CharTransFlag & 0xFFFF0000;
    if (v == 0x1000000) {
        g_info.Player = 0;
    }
    if (v == 0x10000) {
        g_info.Player = 1;
    }
}

extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];
void JobChange(int job);

void VsInit(void) {
    int i;
    u_char s;

    for (i = 0; i < 2; i++) {
        s = g_info.SelectPlayerIdno[i];
        g_info.LoadPlayerID[i] = -1;
        UseCharacter1p2p[i] = 0;
        InitGameinfo_ByGameOver(i);
        g_info.SelectPlayerIdno[i] = s;
    }
    JobChange(0xC);
}

extern GAME_INFOMATION g_info;
void ClearScore(int side);

void InitGameinfo_ByGameOver(int side) {
    int i;

    g_info.VsCpuCtrlFlag[side] = 0;
    g_info.BattleCnt[side] = 0;
    g_info.SelectPlayerIdno[side] = side + 1;
    g_info.PlayerSelected[side] = 0;
    g_info.BattleTime[side] = 0;
    g_info.MedalMode[side] = 0;
    SetCPUCharacterFlag(side, 1);
    ClearScore(side);
    for (i = 15; i >= 0; i--) {
        g_info.BattleHistory[side][i] = 0;
    }
    g_info.PlayerPalette[side] = 0;
    g_info.PlayerVoice[side] = side;
}

extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];
int Random(void);

int InitPlayerVoice(int side) {
    int v;

    v = Random() % 3;
    if (UseCharacter1p2p[side] == UseCharacter1p2p[side == 0]) {
        if (v == g_info.PlayerVoice[side == 0]) {
            v = ((Random() & 1) ? v + 1 : v + 2) % 3;
        }
    }
    return v;
}

extern SYS_COND sc;
extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];

void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu) {
    GAME_INFOMATION *gp;

    gp = &g_info;
    if (sc.conti[playerside] && sc.conti[playerside] != idno) {
        sc.change[playerside] = 1;
    }
    UseCharacter1p2p[playerside] = idno;
    if (!iscpu) {
        gp->PlayerSelected[playerside] = 1;
        gp->SelectPlayerIdno[playerside] = idno;
    }
    gp->PlayerPalette[playerside] = palette;
    debugf("Player %d Palette No %d\n", playerside + 1, palette);
    gp->PlayerVoice[playerside] = InitPlayerVoice(playerside);
}

extern GAME_INFOMATION g_info;

int GetCPUCharacterFlag(void) {
    GAME_INFOMATION *p = &g_info;

    if (p->isCPU[0] && p->isCPU[1]) {
        return 3;
    }
    if (!p->isCPU[0] && p->isCPU[1]) {
        p->Player = 0;
        return 0;
    }
    if (p->isCPU[0] && !p->isCPU[1]) {
        p->Player = 1;
        return 1;
    }
    if (!p->isCPU[0] && !p->isCPU[1]) {
        return 2;
    }
    return 0;
}

extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern SYS_COND sc;
extern short int UseCharacter1p2p[];
extern int StageNo;
extern int RoundNumTb[];
extern int TimeLimittb[];
extern int PLAYER_HITPOINTMAX;
extern int PLAYER_HITPOINTMAXtbByIdno[4];
void InitScore(void);
void PXF_PaletteClear(void);

void Init_Game_Info(void) {
    GAME_INFOMATION *p;
    int i;

    p = &g_info;
    p->CPURoundMax = RoundNumTb[userinfo.roundnum];
    p->VSRoundMax = RoundNumTb[userinfo.roundnum];
    p->Damage = userinfo.damage[0];
    p->TimeByCount = TimeLimittb[1];
    p->Event = 0;
    p->Difficulty = userinfo.glevel;
    PLAYER_HITPOINTMAX = PLAYER_HITPOINTMAXtbByIdno[userinfo.damage[0]];
    p->Winnerid = 1;
    p->Flash = 0;
    for (i = 0; i < 2; i++) {
        p->LoadPlayerID[i] = -1;
        UseCharacter1p2p[i] = 0;
    }
    InitScore();
    g_info.mode = 0;
    g_info.BstartTime[0] = 0;
    g_info.BstartTime[1] = 0;
    StageNo = 0;
    sc.conti[0] = 0;
    sc.conti[1] = 0;
    sc.cnt[0] = 0;
    sc.cnt[1] = 0;
    sc.change[0] = 0;
    sc.change[1] = 0;
    PXF_PaletteClear();
}
