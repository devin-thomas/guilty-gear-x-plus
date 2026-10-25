#include "generated/vsctrl.h"

static const short idtb[] = { 1, 2, 3 };

extern SYS_COND sc;
extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];
extern USER_SETTING userinfo;
int Random(void);
void SetStageNo(int no);

int Enter_Stage_Bg(void)
{
    GAME_INFOMATION *gp;
    int side;
    int bg;
    char StageTable[18] = { 8, 8, 5, 3, 1, 6, 2, 7, 2, 6, 9, 4, 2, 5, 7, 10, 11, 12 };

    gp = &g_info;
    side = gp->Player;
    if (gp->mode & 0x900) {
        bg = sc.bg;
        if (bg <= 0) {
            bg = UseCharacter1p2p[side ? 0 : 1];
        } else if (bg == 0x13) {
            switch (userinfo.phase) {
            case 5:
                bg = Random() % 18 + 1;
                break;
            case 4:
                bg = Random() % 17 + 1;
                break;
            case 2:
            case 3:
                bg = Random() % 16 + 1;
                break;
            case 1:
                bg = Random() % 15 + 1;
                break;
            default:
                bg = Random() % 14 + 1;
                break;
            }
        }
        SetStageNo(StageTable[bg - 1]);
    } else if (gp->mode & 0x4000) {
        SetStageNo(StageTable[UseCharacter1p2p[side ? 0 : 1] - 1]);
    } else {
        SetStageNo(StageTable[UseCharacter1p2p[side ? 0 : 1] - 1]);
    }
    return 0;
}


void DispVsCtrlFlag(void) {
}

extern int LimitStatus;

extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];
extern USER_SETTING userinfo;

int ResetEnemyEntryFlag(void)
{
    GAME_INFOMATION *gp;
    int i;
    int side;
    int count;

    gp = &g_info;
    side = gp->Player;
    count = gp->BattleCnt[side];
    UseCharacter1p2p[side ? 0 : 1] = gp->BattleHistory[side][count];
    i = 0;
    if (UseCharacter1p2p[side] == gp->BattleHistory[side][count]) {
        while (gp->PlayerPalette[side] == i) {
            i++;
        }
    }
    if (g_info.GoldMode == 1) {
        i = 9;
    } else if (g_info.GoldMode == 2) {
        i = 10;
    }
    if (g_info.GGEXMode & 2) {
        userinfo.gguse[!side] = 2;
    } else if (g_info.GGEXMode & 4) {
        if (UseCharacter1p2p[side ? 0 : 1] == 2) {
            userinfo.gguse[!side] = 4;
        } else {
            userinfo.gguse[!side] = 6;
        }
    } else {
        userinfo.gguse[!side] = 0;
    }
    EntrySelectCharacter(!side, UseCharacter1p2p[side ? 0 : 1], i, 1);
    Enter_Stage_Bg();
    gp->VsCpuCtrlFlag[side] &= ~0x10;
    return 0;
}


void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu);
int RandSel(void);
int get_select_color(int id);
void Change_ScreenScale(int flag);
void SetCPUCharacterFlag(int playerside, int flag);

void InitDemoBattle(void)
{
    int id[2];
    char StageTable[18] = { 8, 8, 5, 3, 1, 6, 2, 7, 2, 6, 9, 4, 2, 5, 7, 10, 11, 12 };
    int side;
    int col[2];

    side = Random() & 1;
    id[0] = RandSel();
    id[1] = RandSel();
    Change_ScreenScale(1);
    if (g_info.mode & 0x400) {
        col[0] = get_select_color(id[0]);
        do {
            col[1] = get_select_color(id[1]);
        } while (id[0] == id[1] && col[0] == col[1]);
        if (g_info.GoldMode == 1) {
            col[1] = col[0] = 9;
        } else if (g_info.GoldMode == 2) {
            col[1] = col[0] = 10;
        }
        if (g_info.GGEXMode & 2) {
            userinfo.gguse[1] = userinfo.gguse[0] = 2;
        } else if (g_info.GGEXMode & 4) {
            if (id[0] == 2) {
                userinfo.gguse[0] = 4;
            } else {
                userinfo.gguse[0] = 6;
            }
            if (id[1] == 2) {
                userinfo.gguse[1] = 4;
            } else {
                userinfo.gguse[1] = 6;
            }
        } else {
            userinfo.gguse[0] = 0;
        }
        EntrySelectCharacter(0, id[0], col[0], 1);
        EntrySelectCharacter(1, id[1], col[1], 1);
        if (id[0] == id[1] && g_info.PlayerVoice[0] == g_info.PlayerVoice[1]) {
            g_info.PlayerVoice[0] = (g_info.PlayerVoice[0] + 1) % 3;
        }
    } else {
        EntrySelectCharacter(0, id[0], 0, 1);
        EntrySelectCharacter(1, id[1], id[0] == id[1], 1);
        if (id[0] == id[1] && g_info.PlayerVoice[0] == g_info.PlayerVoice[1]) {
            g_info.PlayerVoice[0] = (g_info.PlayerVoice[0] + 1) % 3;
        }
    }
    SetStageNo(StageTable[UseCharacter1p2p[side] - 1]);
    SetCPUCharacterFlag(0, 1);
    SetCPUCharacterFlag(1, 1);
    SetVsFlag();
}


int GetLimitStatus(void) {
    return LimitStatus;
}

extern int CancelFlag;

void InitCancelFlag(void) {
    CancelFlag = 0;
}

extern int CancelFlag;

int IsCancelFlag(void) {
    return CancelFlag;
}

extern int CancelFlag;

int SetCancelFlag(int flag) {
    CancelFlag = flag;
    return flag;
}

extern GAME_INFOMATION g_info;

int IsDemoBattle(void) {
    return g_info.Demo;
}

extern GAME_INFOMATION g_info;

int IsEnemyEntry(void) {
    return g_info.VsCpuCtrlFlag[g_info.Player] & 2;
}

extern GAME_INFOMATION g_info;

int IsPr(void) {
    return g_info.VsCpuCtrlFlag[g_info.Player] & 1;
}

extern GAME_INFOMATION g_info;

int IsStageDisp(void) {
    return g_info.VsCpuCtrlFlag[g_info.Player] & 8;
}

extern GAME_INFOMATION g_info;

int IsVs(void) {
    return g_info.VsCpuCtrlFlag[g_info.Player] & 4;
}

extern GAME_INFOMATION g_info;

void ClrEnemyEntryFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] &= ~2;
}

extern GAME_INFOMATION g_info;

void ClrPrFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] &= ~1;
}

extern GAME_INFOMATION g_info;

void ClrStageDispFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] &= ~8;
}

extern GAME_INFOMATION g_info;

void ClrVSFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] &= ~4;
}

extern GAME_INFOMATION g_info;

int IsResetEnemyEntry(void) {
    return (g_info.VsCpuCtrlFlag[g_info.Player] & 0x10) == 0;
}

extern GAME_INFOMATION g_info;

void SetPrFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] |= 1;
}

extern GAME_INFOMATION g_info;

void SetStageDispFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] |= 8;
}

extern GAME_INFOMATION g_info;

void SetVsFlag(void) {
    g_info.VsCpuCtrlFlag[g_info.Player] |= 4;
    g_info.VsCpuCtrlFlag[g_info.Player] |= 0x10;
}

extern GAME_INFOMATION g_info;

void WinCountExe(void) {
    GAME_INFOMATION *gp = &g_info;

    if (gp->WinnerSide == gp->Player) {
        gp->WinCount++;
    } else {
        gp->WinCount = 1;
    }
}

extern GAME_INFOMATION g_info;

void VsCpuInit(void) {
    g_info.VsCpuCtrlFlag[0] = 0;
    g_info.VsCpuCtrlFlag[1] = 0;
    g_info.WinCount = 0;
}

extern PAD pads[2];

int ChkCancel(void) {
    int flag;

    flag = 0;
    if (pads[0].trg & 0x822) {
        flag = 1;
    }
    if (pads[1].trg & 0x822) {
        flag = 1;
    }
    return flag;
}

extern PAD pads[2];
int Cr_IsPlaying(int side);

int ChkCancel_Start(void) {
    int flag;

    flag = 0;
    if (pads[0].trg & 0x800) {
        flag = Cr_IsPlaying(0) != 0;
    }
    if (pads[1].trg & 0x800) {
        if (Cr_IsPlaying(1)) {
            flag = 1;
        }
    }
    return flag;
}

extern int CancelFlag;
extern int rannyuuflag;
void SetFadeSpeed(int frame);
void Rq_Fadeout(void);
int IsFadeEnd(void);

int IsCancel(void) {
    if (CancelFlag == 0 && rannyuuflag == 0) {
        CancelFlag = ChkCancel();
        if (CancelFlag == 0) {
            return 0;
        }
        SetFadeSpeed(10);
        Rq_Fadeout();
    }
    if (CancelFlag) {
        if (rannyuuflag) {
            return 0;
        }
        if (IsFadeEnd()) {
            return 1;
        }
    }
    return 0;
}

extern int LimitStatus;
extern USER_SETTING userinfo;
int IsEvent(u_int flag);

int SetLimitStatus(void) {
    if (IsEvent(1)) {
        LimitStatus = 0;
    } else if (userinfo.phase == 5) {
        LimitStatus = userinfo.phase;
    } else if (userinfo.phase == 4) {
        LimitStatus = userinfo.phase;
    } else if (userinfo.phase == 3) {
        LimitStatus = 2;
    } else if (userinfo.phase == 2) {
        LimitStatus = userinfo.phase;
    } else if (userinfo.phase == 1) {
        LimitStatus = userinfo.phase;
    } else {
        LimitStatus = 0;
    }
    return LimitStatus;
}

int GetCPUCharacterFlag(void);
void SetCPUCharacterFlag(int playerside, int flag);
int Cr_SetContinue(int side);

int WinnerNexstep(void) {
    GAME_INFOMATION *gp;

    gp = &g_info;
    if ((GetCPUCharacterFlag() == 0 && gp->WinnerSide == 0) || (GetCPUCharacterFlag() == 1 && gp->WinnerSide == 1)) {
        ClrVSFlag();
        ClrEnemyEntryFlag();
        ClrStageDispFlag();
        gp->BattleCnt[gp->Player]++;
        g_info.BstartTime[0] = g_info.BattleTime[0];
        g_info.BstartTime[1] = g_info.BattleTime[1];
        return 1;
    }
    if (GetCPUCharacterFlag() != 2) {
        return 0;
    }
    WinCountExe();
    if (!(g_info.mode & 0x8800)) {
        Cr_SetContinue(gp->WinnerSide == 0);
        SetCPUCharacterFlag(gp->WinnerSide == 0, 1);
    } else {
        gp->Player = gp->WinnerSide;
    }
    ClrVSFlag();
    return 1;
}

int IsEvent(u_int flag);
extern int BootControl;
void SetEnemyEntryFlag(void);
void ProfileLoad(void);
void StgSelLoad(void);
int GetCPUCharacterFlag(void);

int VsCpuCtrl(void)
{
    GAME_INFOMATION *gp;
    int flag;

    gp = &g_info;
    if (IsEvent(1) & 1) {
        if (gp->BattleCnt[gp->Player] >= 8) {
            BootControl |= 4;
            return 6;
        }
    } else {
        if (gp->BattleCnt[gp->Player] >= 10) {
            BootControl |= 4;
            return 6;
        }
    }
    if (GetCPUCharacterFlag() == 2) {
        Enter_Stage_Bg();
        SetVsFlag();
        if (g_info.mode & 0x800) {
            return 0x1E;
        }
        return 0x15;
    }
    if (userinfo.quickconti) {
        Enter_Stage_Bg();
        if (!IsResetEnemyEntry() && gp->BattleHistory[gp->Player][gp->BattleCnt[gp->Player]]) {
            ResetEnemyEntryFlag();
            return 0x1E;
        }
        if (IsEnemyEntry() && UseCharacter1p2p[gp->Player ? 0 : 1]) {
            return 0x1E;
        }
        SetEnemyEntryFlag();
        return 0x1E;
    }
    if (!IsPr()) {
        ProfileLoad();
        StgSelLoad();
        SetPrFlag();
        return 0x10;
    }
    if (!IsEnemyEntry()) {
        SetEnemyEntryFlag();
    } else if (!IsResetEnemyEntry()) {
        ResetEnemyEntryFlag();
    }
    if (!IsStageDisp()) {
        StgSelLoad();
        SetStageDispFlag();
        return 0x12;
    }
    if (!IsVs()) {
        SetVsFlag();
        return 0x15;
    }
    return 6;
}

extern PAD pads[2];
extern SYS_COND sc;
extern int BattleStatus;
int Cr_IsContenue(int side);
int Cr_IsStartWait(int side);
int Cr_Request_Start(int side);
void ClearScore(int side);
int RefereeWorkStop(void);
void IntIrq(void);

int IsRannyuu(void) {
    int i;

    if (IsDemoBattle() || (g_info.mode & 0xAF00)) {
        return 0;
    }
    if (g_info.BattleControlFlag & 4) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        if ((pads[i].trg & 0x800) && ((Cr_IsStartWait(i) && !((g_info.mode >> i) & 1)) || (Cr_IsContenue(i) && ((g_info.mode >> i) & 1)))) {
            Cr_Request_Start(i);
            sc.rannyu = i + 1;
            ClearScore(i);
            BattleStatus = 1;
            RefereeWorkStop();
            g_info.PlayerSelected[i] = 0;
            IntIrq();
            return i + 1;
        }
    }
    return 0;
}

int GetLimitStatus(void);
extern char VsLimitFlag[6][19];

void SetEnemyEntryFlag(void)
{
    GAME_INFOMATION *gp = &g_info;
    int i;
    int side = gp->Player;
    int count = gp->BattleCnt[side];
    char BattleSchedule[18][10] = {
        { 0, 0, 0, 0, 0, 0, 0, 2, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 14, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 8, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 4, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 3, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 8, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 11, 16 }
    };

    UseCharacter1p2p[side ? 0 : 1] = BattleSchedule[UseCharacter1p2p[side] - 1][count];
    while (UseCharacter1p2p[side ? 0 : 1] == 0) {
        if (g_info.GGEXMode || g_info.GoldMode) {
            if (userinfo.phase == 5) {
                UseCharacter1p2p[side ? 0 : 1] = Random() % 18 + 1;
            } else if (userinfo.phase == 4) {
                UseCharacter1p2p[side ? 0 : 1] = Random() % 17 + 1;
            } else {
                UseCharacter1p2p[side ? 0 : 1] = (Random() & 0xF) + 1;
            }
        } else {
            UseCharacter1p2p[side ? 0 : 1] = (Random() & 0xF) + 1;
        }
        if (VsLimitFlag[GetLimitStatus()][UseCharacter1p2p[side ? 0 : 1] - 1] == 0) {
            UseCharacter1p2p[side ? 0 : 1] = 0;
        }
        for (i = 0; i < 10; i++) {
            if (gp->BattleHistory[side][i] == UseCharacter1p2p[side ? 0 : 1]) {
                UseCharacter1p2p[side ? 0 : 1] = 0;
            } else if (BattleSchedule[UseCharacter1p2p[side] - 1][i] == UseCharacter1p2p[side ? 0 : 1]) {
                UseCharacter1p2p[side ? 0 : 1] = 0;
            }
        }
    }
    gp->BattleHistory[side][count] = UseCharacter1p2p[side ? 0 : 1];
    i = 0;
    if (UseCharacter1p2p[side] == gp->BattleHistory[side][count]) {
        while (gp->PlayerPalette[side] == i) {
            i++;
        }
    }
    if (g_info.GoldMode == 1) {
        i = 9;
    } else if (g_info.GoldMode == 2) {
        i = 10;
    }
    if (g_info.GGEXMode & 2) {
        userinfo.gguse[!side] = 2;
    } else if (g_info.GGEXMode & 4) {
        if (UseCharacter1p2p[side ? 0 : 1] == 2) {
            userinfo.gguse[!side] = 4;
        } else {
            userinfo.gguse[!side] = 6;
        }
    } else {
        userinfo.gguse[!side] = 0;
    }
    EntrySelectCharacter(!side, UseCharacter1p2p[side ? 0 : 1], i, 1);
    Enter_Stage_Bg();
    gp->VsCpuCtrlFlag[side] |= 2;
    gp->VsCpuCtrlFlag[side] &= ~0x10;
}
