#include "generated/guardp.h"

static const short int idtb[] = { 1, 2, 3 };

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

void PIYODeadProc(CHARACTER_WORK *offset)
{
    CHARACTER_WORK *poffset;
    int index;
    static const unsigned char tb[9] = { 5, 3, 7, 0, 4, 2, 6, 8, 8 };

    poffset = offset->parentWork;
    index = poffset->ply->FaintTime / 32;
    if (poffset->ply->FaintTime == 0 || offset->Localid == tb[index]) {
        SetActNoNext(offset, 1);
    }
}

void PIYODeadProc(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetSearchExecProcAddr(void *addr);

void FaintClear(CHARACTER_WORK *offset)
{
    offset->ply->FaintTime = 0;
    SetSearchExecProcAddr(PIYODeadProc);
    SearchCharacterExecute(offset, 0x6E, 0, 0x26);
}

void FaintClear(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int Random(void);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetSearchExecProcAddr(void *addr);
int g_roundmin(int val, int min);
void PIYODeadProc(CHARACTER_WORK *offset);
extern PAD GameKey[2];
extern short int PDGCGauge_Recovery[];
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;
extern CHARANGE_CTRL *gl_info;
extern int BattleStatus;

void GuardPointExecute(CHARACTER_WORK *offset)
{
    int time;

    if (offset->ply->FaintTime > 0) {
        time = offset->ply->FaintTime / 32;
        if (offset->HitStopTime == 0) {
            if (GameKey[offset->padid].trg & 0xA000) {
                offset->ply->FaintTime -= Random() + 2 & 3;
                if (offset->actno == 0x46) offset->ply->pactst |= 0x400;
            }
            if (GameKey[offset->padid].trg & (offset->ply->PunchKey | offset->ply->KickKey | offset->ply->SlashKey | offset->ply->HighSlashKey)) {
                offset->ply->FaintTime -= Random() + 2 & 3;
                if (offset->actno == 0x46) offset->ply->pactst |= 0x400;
            }
            offset->ply->FaintTime--;
        } else {
            if (GameKey[offset->padid].trg & 0xA000) {
                offset->ply->FaintTime -= Random() + 1 & 1;
            }
            if (GameKey[offset->padid].trg & (offset->ply->PunchKey | offset->ply->KickKey | offset->ply->SlashKey | offset->ply->HighSlashKey)) {
                offset->ply->FaintTime -= Random() + 1 & 1;
            }
        }
        if (offset->ply->FaintTime <= 0) {
            offset->ply->FaintTime = 0;
            offset->ply->FaintPoint = 0;
            SetSearchExecProcAddr(PIYODeadProc);
            SearchCharacterExecute(offset, 0x6E, 0, 0x26);
        }
        if (offset->ply->FaintTime / 32 < time) {
            SetSearchExecProcAddr(PIYODeadProc);
            SearchCharacterExecute(offset, 0x6E, 0, 0x26);
        }
        if (BattleStatus != 2 && offset->actno == 0x46) {
            FaintClear(offset);
        }
    }
    if (offset->actst & 0x20) return;
    {
        int point;

        if (g_info.mode & 0x100) {
            point = tr_info->tcom.ggage;
        } else if ((g_info.mode & 0x8000) && BattleStatus != 1) {
            point = gl_info->gg[offset->padid];
        } else {
            point = 0;
        }
        if (offset->ply->GuardPoint < point) offset->ply->GuardPoint = point;
    }
    if (offset->actst & 0x200) offset->ply->GuardTime = 30;
    if (offset->ply->GuardTime > 0) {
        offset->ply->GuardTime--;
    } else {
        int point;

        if (g_info.mode & 0x100) {
            point = tr_info->tcom.ggage;
        } else if ((g_info.mode & 0x8000) && BattleStatus != 1) {
            point = gl_info->gg[offset->padid];
        } else {
            point = 0;
        }
        offset->ply->GuardPoint = g_roundmin(offset->ply->GuardPoint - 1, point);
        offset->ply->GuardTime = PDGCGauge_Recovery[offset->padid];
    }
    if (offset->ply->FaintPoint > 0) {
        if (GetEnemyPlayer(offset)->ply->OverKillFlag == 1) {
            offset->ply->FaintPoint -= 8;
        } else {
            offset->ply->FaintPoint -= 5;
        }
        if (offset->ply->FaintPoint < 0) offset->ply->FaintPoint = 0;
    }
}

extern short int PDGCGauge_DefPoint[];

void AddGuardPoint(CHARACTER_WORK *offset, int lv, TACTHEADER *ahp)
{
    static char DefGuardLvByATLV[] = { 2, 4, 6, 7 };
    static char GuardCounterLv[] = { 0, 0, 4, 8, 14, 20, 26, 32 };
    int clv;
    int val;

    clv = (ahp->lvflag >> 10) & 7;
    if (clv == 0) clv = DefGuardLvByATLV[lv];
    val = GuardCounterLv[clv];
    val = val * PDGCGauge_DefPoint[offset->padid] / 32;
    if ((~offset->actst & 0x10) && (ahp->flag & 0xC00) != 0xC00) val -= val >> 2;
    if (offset->actst & 0x200) val >>= 1;
    offset->ply->GuardPoint += val;
    if (offset->ply->GuardPoint > 128) offset->ply->GuardPoint = 128;
}

void DecGuardPoint(CHARACTER_WORK *offset, int lvflag)
{
    int Point;
    DAMAGEPARAM *dp;

    Point = lvflag & 3;
    dp = offset->DamParam;
    if (offset->ply->GuardPoint > 0) {
        offset->ply->GuardPoint -= offset->ply->GuardPoint >> 3;
    }
    if (dp->DamageHosei == -1) {
        if (Point == 0) {
            offset->ply->GuardPoint -= 8;
        } else if (Point == 1) {
            offset->ply->GuardPoint -= 7;
        } else {
            offset->ply->GuardPoint -= 6;
        }
    } else {
        offset->ply->GuardPoint -= dp->DamageHosei;
    }
    if (offset->ply->GuardPoint < -128) offset->ply->GuardPoint = -128;
}

int IsCounterHitByGuardCounter(CHARACTER_WORK *offset)
{
    return offset->ply->GuardPoint > 64.0;
}

short int DamageHosei[] = { 256, 200, 152, 112, 80, 48, 32, 16, 8 };
short int DamageHoseiKakusei[] = { 256, 176, 128, 96, 80, 48, 32, 16, 8 };

int GetGuardCounterDamage(CHARACTER_WORK *offset, TACTHEADER *ahp)
{
    int index;
    unsigned int gp;
    unsigned int h;
    unsigned int i;
    short int *dhtb;

    if (ahp && (ahp->flag & 0x4000000)) {
        dhtb = DamageHoseiKakusei;
    } else {
        dhtb = DamageHosei;
    }
    if (offset->ply->GuardPoint > 0) return offset->ply->GuardPoint / 4 + 256;
    if (offset->ply->GuardPoint == 0) return 256;
    gp = ~offset->ply->GuardPoint;
    index = gp / 16;
    h = dhtb[index];
    i = h - dhtb[index + 1];
    return (h * 16 - i * (gp % 16)) / 16;
}

short int GetGuardCounterAddLV(CHARACTER_WORK *offset)
{
    return offset->ply->GuardPoint / 64;
}

int IsFaint(CHARACTER_WORK *offset)
{
    return offset->ply->FaintPoint == -1;
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int IsFaint(CHARACTER_WORK *offset);
int Random(void);
void font_entry(int side, char *str);

void AddFaintPoint(CHARACTER_WORK *offset, int damage)
{
    int pt;
    DAMAGEPARAM *dp;
    int i;
    CREATE_ARG c_arg;

    dp = offset->DamParam;
    if (IsFaint(offset)) return;
    damage = damage * dp->FaintPoint >> 4;
    if (damage < 3) return;
    if (GetEnemyPlayer(offset)->ply->OverKillFlag == 1) damage /= 2;
    pt = damage / 4 + 1;
    pt *= (Random() & 0x1F) + 64;
    if (pt > offset->ply->FaintMax * 100) pt = offset->ply->FaintMax * 100;
    offset->ply->FaintPoint += pt;
    if (offset->ply->FaintPoint / 100 >= offset->ply->FaintMax) {
        font_entry(offset->padid, "FAINT");
        offset->ply->FaintMax += offset->ply->FaintMax >> 2;
        if (offset->ply->FaintMax > 120) offset->ply->FaintMax = 120;
        offset->ply->FaintPoint = -1;
        offset->ply->FaintTime = 256;
        for (i = 0; i < 8; i++) {
            c_arg.flag = 0x20;
            c_arg.Localid = i;
            EffectCreate(offset, 0x6E, &c_arg);
        }
    }
}
