#include "ggxeuc/objtypes.h"

int GetTensionVal(CHARACTER_WORK *offset);
void Tension_Add(CHARACTER_WORK *offset, int val);

int GetTensionVal(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *ply = offset->ply;
    if (ply->OverKillFlag) return 0;
    else return ply->Tension;
}

int GetPlayerDist(void);

float GetNegativeCoefficient(CHARACTER_WORK *offset) {
    float coef;
    float ret;
    short bal;

    if (GetPlayerDist() < 25000) {
        coef = 0.5f;
    } else if (GetPlayerDist() <= 49999) {
        coef = 1.0f;
    } else {
        coef = 1.5f;
    }
    bal = offset->ply->TensionBalance;
    if (bal < -5000) ret = coef * 3.0;
    else if (bal < -3000) ret = coef * 2.0;
    else if (bal < -1500) ret = coef;
    else if (bal < -500) ret = coef * 0.8;
    else if (bal < 500) ret = coef * 0.6;
    else ret = coef * 0.3;
    return ret;
}

int GetPlayerDist(void);
extern short PDEneTensionRate[];

float GetCoefficient(CHARACTER_WORK *offset) {
    float coef;
    int dist;
    PLAYER_ENTRY *ply;
    short bal;

    dist = GetPlayerDist();
    if (dist <= 49999) {
        coef = 1.0f;
    } else if (dist <= 74999) {
        coef = 0.6f;
    } else {
        coef = 0.4f;
    }
    ply = offset->ply;
    if (ply->TensionPenaltyTime) coef = coef * 0.2;
    bal = ply->TensionBalance;
    if (bal < -5000) coef = coef * 0.25;
    else if (bal < -3000) coef = coef * 0.5;
    else if (bal < -1500) coef = coef * 0.75;
    else if (bal < -500) coef = coef * 0.9;
    else if (bal < 500) ;
    else if (bal < 2000) coef = coef * 1.25;
    else coef = coef * 1.5;
    return coef * ((float)(PDEneTensionRate[offset->padid] + 16) / 16.0);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int GetPlayerDistOff(CHARACTER_WORK *offset);
float GetNegativeCoefficient(CHARACTER_WORK *offset);
float GetCoefficient(CHARACTER_WORK *offset);
int g_roundmin(int val, int max);
extern short PDTensionAdd_FWalk[];
extern short PDTensionAdd_FJump[];
extern short PDTensionAdd_FSDash[];
extern short PDTensionAdd_FDash[];

void Add_TensionVal_atMove(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    float mod;
    float neg;
    float coef;
    short t;
    PLAYER_ENTRY *ply;

    enemy = GetEnemyPlayer(offset);
    if (offset->ply->OverKillFlag) return;

    mod = 1.0f;
    if (enemy->actst & 0x8320) mod = 0.2f;
    neg = GetNegativeCoefficient(offset);
    coef = GetCoefficient(offset);

    switch (offset->actno) {
    case 5:
        ply = offset->ply;
        ply->Tension += PDTensionAdd_FWalk[offset->padid] * coef * mod;
        ply->TensionBalance += coef * 6.0f;
        break;
    case 9:
        if (offset->ActTimer < 6) coef += coef;
        ply = offset->ply;
        ply->Tension += PDTensionAdd_FJump[offset->padid] * coef * mod;
        ply->TensionBalance += coef * 5.0f;
        break;
    case 12:
        if (GetPlayerDistOff(offset) >= -10000) {
            ply = offset->ply;
            ply->Tension += PDTensionAdd_FSDash[offset->padid] * coef * mod;
            ply->TensionBalance += coef * 5.0f;
        }
        break;
    case 14:
        if (GetPlayerDistOff(offset) >= -10000) {
            t = offset->ActTimer;
            if (t > 60) coef = coef * 0.5;
            else if (t <= 30) {
                if (t < 8) coef = coef * 0.6;
            }
            ply = offset->ply;
            ply->Tension += PDTensionAdd_FDash[offset->padid] * coef * mod;
            ply->TensionBalance += coef * 8.0f;
        }
        break;
    default:
        goto next;
    }
    offset->ply->TensionNoTouchTime -= 2;
next:

    coef = 1.0f;
    switch (offset->actno) {
    case 6:
        offset->ply->TensionBalance -= 4;
        offset->ply->NegativeVal += neg * 20.0f;
        offset->ply->TensionMovePenaltyTime = g_roundmin(offset->ply->TensionMovePenaltyTime, 12);
        offset->ply->TensionMovePenaltyTime += 1;
        break;
    case 13:
        if (offset->ActTimer >= 6) coef = 0.3f;
        offset->ply->NegativeVal += neg * 40.0f;
        offset->ply->TensionBalance -= coef * 40.0f;
        offset->ply->TensionMovePenaltyTime = g_roundmin(offset->ply->TensionMovePenaltyTime, 30);
        offset->ply->TensionMovePenaltyTime += 2;
        break;
    case 16:
        if (offset->ActTimer >= 6) coef = 0.3f;
        offset->ply->NegativeVal += neg * 60.0f;
        offset->ply->TensionBalance -= coef * 40.0f;
        offset->ply->TensionMovePenaltyTime = g_roundmin(offset->ply->TensionMovePenaltyTime, 30);
        offset->ply->TensionMovePenaltyTime += 2;
        break;
    case 10:
        if (offset->ActTimer >= 6) coef = 0.3f;
        offset->ply->NegativeVal += neg * 30.0f;
        offset->ply->TensionBalance -= coef * 36.0f;
        offset->ply->TensionMovePenaltyTime = g_roundmin(offset->ply->TensionMovePenaltyTime, 30);
        offset->ply->TensionMovePenaltyTime += 2;
        break;
    }
}

void Add_TensionVal_atAttackHit(CHARACTER_WORK *offset, int val) {
    PLAYER_ENTRY *ply = offset->ply;
    ply->TensionBalance += val / 2;
    ply->TensionNoTouchTime = ply->TensionNoTouchTime / 2 - 300;
}

void Add_TensionVal_atAttackGuard(CHARACTER_WORK *offset, int val) {
    PLAYER_ENTRY *ply = offset->ply;
    ply->TensionNoTouchTime -= 60;
    ply->TensionBalance += val / 4;
}

float GetCoefficient(CHARACTER_WORK *offset);

void Tension_Add(CHARACTER_WORK *offset, int val) {
    if (offset->ply->OverKillFlag) return;
    offset->ply->Tension += val * GetCoefficient(offset);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
float GetCoefficient(CHARACTER_WORK *offset);
int g_round(int val, int max, int min);

void Tension_Add_Enemy(CHARACTER_WORK *offset, int val) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    if (enemy->ply->OverKillFlag) return;
    if (val <= 0) return;
    if (offset->actst & 0x20) {
        if (offset->ply->HitCount >= 16) return;
        val -= val * g_round(offset->ply->HitCount + 4, 15, 5) / 16;
    }
    enemy->ply->Tension += val * GetCoefficient(enemy);
}

void Tension_Add(CHARACTER_WORK *offset, int val);

void Tension_Add_Damage(CHARACTER_WORK *offset, int val, TACTHEADER *hdr) {
    int lv;

    if (hdr == NULL) return;
    if (offset->ply->OverKillFlag) return;
    if (hdr->flag & 4) {
        Tension_Add(offset, val * 6.0);
    } else {
        lv = hdr->lvflag & 3;
        if (lv == 0) Tension_Add(offset, val * 6.0);
        else if (lv == 1) Tension_Add(offset, val * 4.5);
        else Tension_Add(offset, val * 3.5);
    }
}

void Tension_Add_Guard(CHARACTER_WORK *offset, int val) {
    Tension_Add(offset, val * 2);
}

void Tension_Add_JGuard(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *ply = offset->ply;
    if (ply->OverKillFlag) return;
    ply->TensionBalance += 200.0f;
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);

void Tension_Add_ByLongHair(CHARACTER_WORK *offset) {
    Tension_Add(GetEnemyPlayer(offset), 1000);
}

/* first matched by Codex */
int Tension_Cmp(CHARACTER_WORK *offset, int val) {
    return GetTensionVal(offset) >= val;
}

void Tension_Dec(CHARACTER_WORK *offset, int val) {
    PLAYER_ENTRY *ply;
    int time = 0;

    if (val == -1) {
        ply = offset->ply;
        ply->Tension = 0;
        ply->TensionFlag |= 8;
    } else if (val == -2) {
        ply = offset->ply;
        val = 25;
        time = 60;
    } else if (val == -3) {
        ply = offset->ply;
        val = 70;
        time = 60;
    } else if (val == -4) {
        ply = offset->ply;
        val = 5000;
        time = 240;
    } else if (val == -5) {
        ply = offset->ply;
        val = 5000;
        time = 240;
    } else {
        if (val == -6) val = 5000;
        ply = offset->ply;
    }
    ply->Tension -= val;
    if (ply->TensionPenaltyTime < time) ply->TensionPenaltyTime = time;
    if (ply->Tension < 0) ply->Tension = 0;
}

void SetDefaultTension(CHARACTER_WORK *offset) {
    TACTHEADER *hdr = offset->ActHeaderp;

    if (!(hdr->flag & 0x2000)) return;
    if (hdr->flag & 4) {
        if (offset->ply) {
            offset->TensionAdd = 60;
            offset->ply->TensionHissatuAddVal = 0xFF;
        } else {
            offset->TensionAdd = 10;
        }
    } else {
        switch (hdr->lvflag & 3) {
        case 0: offset->TensionAdd = 12; break;
        case 1: offset->TensionAdd = 22; break;
        default: offset->TensionAdd = 32; break;
        }
    }
}

float GetNegativeCoefficient(CHARACTER_WORK *offset);
int g_roundmin(int val, int min);
int g_roundmax(int val, int max);
int GetBattleTimeStopFlag(int no);
void Score_ExecuteScorerate(int padid);
void font_entry(int padid, char *str);
extern int BattleStatus;
extern int TotalCounter;
extern int PLAYER_HITPOINTMAX;

void TensionExecute(CHARACTER_WORK *offset) {
    int dec;

    offset->ply->Tension = g_roundmax(offset->ply->Tension, 10000);
    if (BattleStatus != 2) return;
    if (GetBattleTimeStopFlag(-1)) return;
    Score_ExecuteScorerate(offset->padid);

    if (!(offset->actst & 0x220)) {
        if (offset->ply->OverKillFlag == 1 && !(TotalCounter & 0xF)) {
            if (offset->HitPoint > PLAYER_HITPOINTMAX / 2) dec = 10;
            else if (offset->HitPoint > PLAYER_HITPOINTMAX / 4) dec = 5;
            else if (offset->HitPoint > PLAYER_HITPOINTMAX / 8) dec = 2;
            else dec = 1;
            if (offset->ply->Tension > 0) {
                offset->ply->Tension = g_roundmin(offset->ply->Tension - 160, 0);
            } else if (offset->HitPoint) {
                offset->HitPoint -= dec;
                offset->HitPoint = g_roundmin(offset->HitPoint, 1);
            }
        }
        if (offset->actst & 0x2800) {
            offset->ply->TensionSideTime++;
            if (offset->ply->TensionSideTime > 480) {
                offset->ply->NegativeVal += GetNegativeCoefficient(offset) * 20.0f;
                offset->ply->TensionBalance -= 6;
            } else if (offset->ply->TensionSideTime > 240) {
                offset->ply->TensionBalance -= 3;
            }
            if (offset->ply->TensionSideTime > 960) offset->ply->TensionSideTime = 960;
        } else {
            if (offset->ply->TensionSideTime > 0) offset->ply->TensionSideTime--;
        }
        offset->ply->TensionNoTouchTime++;
        if (offset->ply->TensionNoTouchTime > 1080) {
            offset->ply->NegativeVal += GetNegativeCoefficient(offset) * 15.0f;
            offset->ply->TensionBalance -= 4;
        } else if (offset->ply->TensionNoTouchTime > 360) {
            offset->ply->TensionBalance -= 4;
        }
        if (offset->ply->TensionNoTouchTime > 1800) offset->ply->TensionNoTouchTime = 1800;
        if (offset->ply->TensionNoTouchTime < 0) offset->ply->TensionNoTouchTime = 0;
    }

    if (offset->ply->OverKillFlag == 0) {
        if (offset->ply->NegativeVal > 10000) {
            PLAYER_ENTRY *ply;
            font_entry(offset->padid, "NEGATIVE PENALTY");
            ply = offset->ply;
            ply->TensionPenaltyTime = 600;
            ply->Tension = 0;
            ply->NegativeVal = 0;
            ply->TensionBalance -= ply->TensionBalance / 2;
            ply->TensionSideTime = 0;
            ply->TensionNoTouchTime = 0;
            ply->FaintMax -= ply->FaintMax / 8;
            offset->ply->FaintMax = g_roundmin(offset->ply->FaintMax, 40);
            offset->ply->FaintPoint = g_roundmax(offset->ply->FaintPoint, offset->ply->FaintMax * 0.9 * 100.0);
        }
    } else {
        offset->ply->NegativeVal = g_roundmax(offset->ply->NegativeVal, 6000);
    }
    if (offset->ply->TensionBalance < 0) offset->ply->TensionBalance++;
    if (offset->ply->TensionBalance > 0) offset->ply->TensionBalance--;
    if (offset->ply->TensionBalance < -10000) offset->ply->TensionBalance = -10000;
    if (offset->ply->TensionBalance > 10000) offset->ply->TensionBalance = 10000;
    if ((offset->ply->NegativeVal -= 10) < 0) offset->ply->NegativeVal = 0;
    if (offset->ply->Tension < 0) offset->ply->Tension = 0;
    if (offset->ply->TensionFlag & 8) offset->ply->Tension = 0;
}

void TensionAddExecute(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *ply = offset->ply;
    if (ply->TensionFlag & 2) {
        Tension_Add(offset, ply->TensionHissatuAddVal * 5);
        offset->ply->TensionFlag &= ~3;
    }
}
