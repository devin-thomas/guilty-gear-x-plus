#include "generated/plcolchk.h"

static const short idtb[] = { 1, 2, 3 };

int privatese_request(CHARACTER_WORK *offset, int no);
int sound_request(int no);

void ReqHitSE(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag)
{
    static unsigned char DEFHITSE_NIKU[4] = { 0x19, 0x1A, 0x1A, 0x1A };
    static unsigned char DEFHITSE_KIRI[4] = { 0x1C, 0x1C, 0x1D, 0x1D };
    static unsigned char DEFHITSE_TUKI[4] = { 0x1E, 0x1E, 0x1D, 0x1D };
    unsigned short lv;

    lv = lvflag & 3;
    if (offset->DamParam && offset->DamParam->HitSE.param.flag[0] != 0) {
        if (offset->DamParam->HitSE.param.flag[0] & 1) {
            sound_request(offset->DamParam->HitSE.param.no[0]);
        } else if (offset->DamParam->HitSE.param.flag[0] & 2) {
            privatese_request(offset->ply->Enemy, offset->DamParam->HitSE.param.no[0]);
        }
        if (offset->DamParam->HitSE.param.flag[1] != 0) {
            if (offset->DamParam->HitSE.param.flag[1] & 1) {
                sound_request(offset->DamParam->HitSE.param.no[1]);
            } else if (offset->DamParam->HitSE.param.flag[1] & 2) {
                privatese_request(offset->ply->Enemy, offset->DamParam->HitSE.param.no[1]);
            }
        }
        return;
    }
    if (flag & 0x40000) {
        sound_request(0x1F);
    }
    if (flag & 0x80000) {
        sound_request(0x20);
    }
    if (flag & 4) {
        sound_request(0x1B);
    } else if (flag & 1) {
        sound_request(DEFHITSE_NIKU[lv]);
    } else if (flag & 2) {
        sound_request(DEFHITSE_TUKI[lv]);
    } else {
        sound_request(DEFHITSE_KIRI[lv]);
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);

void ReqGuardSE(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag)
{
    static unsigned char DEFGUARDSE_NIKU[4] = { 0x13, 0x14, 0x14, 0x14 };
    static unsigned char DEFGUARDSE_KIRI[4] = { 0x15, 0x15, 0x16, 0x16 };
    unsigned short lv;

    lv = (lvflag >> 2) & 3;
    if (offset->DamParam && offset->DamParam->GuardSE.param.flag[0] != 0) {
        if (offset->DamParam->GuardSE.param.flag[0] & 1) {
            sound_request(offset->DamParam->GuardSE.param.no[0]);
        } else if (offset->DamParam->GuardSE.param.flag[0] & 2) {
            privatese_request(offset->ply->Enemy, offset->DamParam->GuardSE.param.no[0]);
        }
        if (offset->DamParam->GuardSE.param.flag[1] != 0) {
            if (offset->DamParam->GuardSE.param.flag[1] & 1) {
                sound_request(offset->DamParam->GuardSE.param.no[1]);
            } else if (offset->DamParam->GuardSE.param.flag[1] & 2) {
                privatese_request(offset->ply->Enemy, offset->DamParam->GuardSE.param.no[1]);
            }
        }
        return;
    }
    if (flag & 4) {
        sound_request(0x18);
    }
    if (flag & 1) {
        sound_request(DEFGUARDSE_NIKU[lv]);
    } else {
        sound_request(DEFGUARDSE_KIRI[lv]);
    }
}


void SetDownTimer(CHARACTER_WORK *offset, short int DefaultTime, short int SetTime)
{
    int shift;
    int time;

    shift = 0;
    if (offset->ply->DownFlag & 4) {
        shift = 1;
    }
    time = DefaultTime << shift;
    if (SetTime != -1) {
        time = SetTime << shift;
    }
    if (offset->ply->DamageTime >= 840) {
        time = time * 0.4f;
    } else if (offset->ply->DamageTime >= 600) {
        time = time * 0.6f;
    } else if (offset->ply->DamageTime >= 420) {
        time = time * 0.8f;
    } else if (offset->ply->DamageTime >= 300) {
        time = time * 0.9f;
    } else {
        time = time * 1.0f;
    }
    offset->ply->DownTimer = time;
}


void HitMarkPut(CHARACTER_WORK *offset, unsigned short idno, unsigned short actno, int cnt)
{
    int i;
    CREATE_ARG c_arg;

    if (offset->DamColliAddr[cnt] == 0) {
        return;
    }
    if (actno) {
        c_arg.flag = 5;
        c_arg.actno = actno;
    } else {
        c_arg.flag = 1;
    }
    c_arg.x = offset->DamColliPos[cnt].x;
    c_arg.y = offset->DamColliPos[cnt].y;
    if (idno == 0x72) {
        for (i = 2; i >= 0; i--) {
            EffectCreate(offset, idno, &c_arg);
        }
        c_arg.flag = 5;
        c_arg.actno = 1;
        for (i = 2; i >= 0; i--) {
            EffectCreate(offset, idno, &c_arg);
        }
    } else {
        EffectCreate(offset, idno, &c_arg);
    }
}

void ReqCmnSE_PerfectGuard(CHARACTER_WORK *offset);

void ReqGuardEffect(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag, int cnt)
{
    unsigned short lv;

    lv = (lvflag >> 2) & 3;
    if (offset->GuardSt & 8) {
        ReqCmnSE_PerfectGuard(offset);
        HitMarkPut(offset, 0x63, 0, cnt);
        HitMarkPut(offset, 0x63, 0, cnt);
        HitMarkPut(offset, 0x63, 0, cnt);
        HitMarkPut(offset, 0x63, 0, cnt);
        HitMarkPut(offset, 0x63, 0, cnt);
        HitMarkPut(offset, 0x63, 0, cnt);
        HitMarkPut(offset, 0x63, 1, cnt);
        HitMarkPut(offset, 0x63, 1, cnt);
        HitMarkPut(offset, 0x63, 1, cnt);
        HitMarkPut(offset, 0x63, 1, cnt);
        HitMarkPut(offset, 0x63, 2, cnt);
        HitMarkPut(offset, 0x63, 2, cnt);
        HitMarkPut(offset, 0x63, 2, cnt);
        HitMarkPut(offset, 0x63, 3, cnt);
        HitMarkPut(offset, 0x63, 3, cnt);
        return;
    }
    if (flag & 4) {
        HitMarkPut(offset, 0x64, 0, cnt);
    } else if (flag & 1) {
        if (lv == 0) {
            HitMarkPut(offset, 0x69, 0, cnt);
        } else if (lv == 1) {
            HitMarkPut(offset, 0x68, 0, cnt);
        } else {
            HitMarkPut(offset, 0x68, 0, cnt);
        }
    } else {
        HitMarkPut(offset, 0x68, 0, cnt);
        HitMarkPut(offset, 0x72, 0, cnt);
    }
}

void ReqHitEffectSlashH(CHARACTER_WORK *offset, int cnt)
{
    HitMarkPut(offset, 0x6F, 0, cnt);
}

void ReqHitEffectSlashV(CHARACTER_WORK *offset, int cnt)
{
    HitMarkPut(offset, 0x6F, 1, cnt);
}

int IsSpJudjeFlag(unsigned int src, unsigned int dst);
void SetFlash(char time, unsigned short flag);
extern GAME_INFOMATION g_info;
extern int EffectLiveCount;

void ReqHitEffect(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag, int cnt)
{
    unsigned short lv;

    lv = lvflag & 3;
    if (IsSpJudjeFlag(flag, 0x08000000) || IsSpJudjeFlag(flag, 0x10000000)) {
        return;
    }
    if (flag & 0x40000) {
        offset->SpriteFlag |= 0x1000;
    }
    if (flag & 0x80000) {
        offset->SpriteFlag |= 0x2000;
    }
    if ((flag & 0x04000000) && g_info.flash_span_count == 0) {
        SetFlash(1, 0xF01);
        g_info.flash_span_count = 10;
    }
    if (flag & 4) {
        int i;
        if (!(flag & 0x80000000)) {
            HitMarkPut(offset, 0x6F, 3, cnt);
            i = 3;
            while (i--) {
                HitMarkPut(offset, 0x6F, 2, cnt);
            }
            i = 3;
            while (i--) {
                HitMarkPut(offset, 0x6F, 10, cnt);
            }
            if (EffectLiveCount < 0x35) {
                i = 3;
                while (i--) {
                    HitMarkPut(offset, 0x6F, 2, cnt);
                }
                i = 3;
                while (i--) {
                    HitMarkPut(offset, 0x6F, 10, cnt);
                }
            }
        } else {
            HitMarkPut(offset, 0x6F, 5, cnt);
            i = 2;
            while (i--) {
                HitMarkPut(offset, 0x6F, 2, cnt);
            }
            i = 2;
            while (i--) {
                HitMarkPut(offset, 0x6F, 10, cnt);
            }
        }
    } else if (flag & 1) {
        int i;
        if (lv == 0) {
            HitMarkPut(offset, 0x66, 0, cnt);
        } else if (lv == 1) {
            HitMarkPut(offset, 0x6D, 0, cnt);
        } else {
            HitMarkPut(offset, 0x6D, 0, cnt);
        }
        i = 4;
        while (i--) {
            HitMarkPut(offset, 0x6F, 7, cnt);
        }
        if (EffectLiveCount < 0x35) {
            i = 4;
            while (i--) {
                HitMarkPut(offset, 0x6F, 7, cnt);
            }
        }
    } else {
        int i;
        if (flag & 2) {
            ReqHitEffectSlashH(offset, cnt);
            HitMarkPut(offset, 0x67, 12, cnt);
            HitMarkPut(offset, 0x67, 12, cnt);
        } else if (flag & 0x20) {
            ReqHitEffectSlashV(offset, cnt);
            if (lv == 0) {
                HitMarkPut(offset, 0x67, 6, cnt);
                HitMarkPut(offset, 0x67, 6, cnt);
            } else if (lv == 1) {
                HitMarkPut(offset, 0x67, 6, cnt);
                HitMarkPut(offset, 0x67, 7, cnt);
                HitMarkPut(offset, 0x67, 7, cnt);
            } else {
                HitMarkPut(offset, 0x67, 6, cnt);
                HitMarkPut(offset, 0x67, 7, cnt);
                HitMarkPut(offset, 0x67, 8, cnt);
            }
        } else {
            ReqHitEffectSlashH(offset, cnt);
            if (lv == 0) {
                HitMarkPut(offset, 0x67, 3, cnt);
                HitMarkPut(offset, 0x67, 3, cnt);
            } else if (lv == 1) {
                HitMarkPut(offset, 0x67, 3, cnt);
                HitMarkPut(offset, 0x67, 4, cnt);
                HitMarkPut(offset, 0x67, 4, cnt);
            } else {
                HitMarkPut(offset, 0x67, 3, cnt);
                HitMarkPut(offset, 0x67, 4, cnt);
                HitMarkPut(offset, 0x67, 5, cnt);
            }
        }
        i = 6;
        while (i--) {
            HitMarkPut(offset, 0x6F, 6, cnt);
        }
        if (EffectLiveCount < 0x35) {
            i = 6;
            while (i--) {
                HitMarkPut(offset, 0x6F, 6, cnt);
            }
        }
    }
}

void MedalPut(CHARACTER_WORK *offset, COLLIPOS *colli, TACTHEADER *header);
void ReqCmnVoice_Damage(CHARACTER_WORK *offset);

void RequestHit(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag, int cnt)
{
    if (flag & 0x40) {
        return;
    }
    MedalPut(offset, &offset->DamColliPos[cnt], &offset->DamActHeader[cnt]);
    ReqHitEffect(offset, flag, lvflag, cnt);
    ReqHitSE(offset, flag, lvflag);
    ReqCmnVoice_Damage(offset);
}

void ReqCmnVoice_Guard(CHARACTER_WORK *offset);

void RequestGuard(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag, int cnt)
{
    ReqGuardEffect(offset, flag, lvflag, cnt);
    ReqGuardSE(offset, flag, lvflag);
    ReqCmnVoice_Guard(offset);
}

void SetHitBackXSpeed(CHARACTER_WORK *offset, short xspeed);

int SetHitBack(CHARACTER_WORK *offset, CHARACTER_WORK *aoff, int speed)
{
    if (aoff->idno < 0x20
        || (aoff->parentWork && (aoff->parentFlag & 0x80)
            && ((CHARACTER_WORK *)aoff->parentWork)->idno < 0x20)) {
        SetHitBackXSpeed(offset, speed);
        return 1;
    }
    return 0;
}

void Add_TensionVal_atAttackGuard(CHARACTER_WORK *offset, int damage);
void CPUEM_Guard(CHARACTER_WORK *offset);
int GetPlayerDist(void);
void SetADHitBackXSpeed(CHARACTER_WORK *offset, short xspeed);
void SetSkyDashCount(CHARACTER_WORK *offset);
void SetSkyJumpCount(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void Tension_Add_JGuard(CHARACTER_WORK *offset);

void SetGuardParam(CHARACTER_WORK *offset, unsigned short lvflag, TACTHEADER *ahp)
{
    unsigned short lv;
    unsigned short off;
    int speed;
    CHARACTER_WORK *aoff;
    static unsigned char PGTimeTb[] = { 0x0A, 0x0E, 0x13, 0x18, 0x0E, 0x14, 0x1A, 0x18 };
    static unsigned char TimeTb[] = { 0x08, 0x0C, 0x11, 0x14, 0x0C, 0x10, 0x16, 0x14 };
    static unsigned char JGTimeTb[] = { 0x06, 0x09, 0x0D, 0x14, 0x08, 0x0C, 0x10, 0x14 };
    static short SpeedTb[] = { -1100, -1400, -2000, -2000, -2000, -2000, -2000, -2000 };

    lv = (lvflag >> 2) & 3;
    off = (offset->actst & 0x10) ? 4 : 0;
    offset->GuardSt |= 0x20;
    offset->ply->ShakeTime = 12;
    CPUEM_Guard(offset);
    if (offset->GuardSt & 8) {
        offset->AnimeTimeMax = PGTimeTb[lv + off];
    } else if (offset->ply->JustGuardTime >= 1 && offset->ply->JustGuardTime <= 8) {
        Tension_Add_JGuard(offset);
        offset->ply->JustGuardTime = -1;
        offset->AnimeTimeMax = JGTimeTb[lv + off];
        offset->SpriteFlag |= 0x800000;
        offset->ply->JGWhiteTime = 5;
    } else {
        offset->SpriteFlag &= ~0x800000;
        offset->AnimeTimeMax = TimeTb[lv + off];
    }
    speed = SpeedTb[lv];
    if (ahp->flag & 0x20000) {
        if (ahp->flag & 0x10000) {
            speed = speed * 1.5f;
        } else {
            speed = 0;
        }
    } else if (ahp->flag & 0x10000) {
        speed = speed * 0.5f;
    }
    SetXSpeed(offset, speed);
    aoff = offset->DamColliAddr[0];
    if (!(offset->GuardSt & 8)) {
        SetHitBack(offset, aoff, speed);
    } else {
        if (SetHitBack(offset, aoff, SpeedTb[lv])) {
            int x = (GetPlayerDist() <= 21999) ? 900 : 500;
            if (offset->actst & 0x800) {
                x = x * 1.3f;
            }
            SetADHitBackXSpeed(offset->ply->Enemy, x);
        }
        offset->GuardSt |= 0x10;
    }
    Add_TensionVal_atAttackGuard(offset->ply->Enemy, ahp->damage);
    offset->actst |= 0x200;
    SetSkyDashCount(offset);
    SetSkyJumpCount(offset);
}

void Add_TensionVal_atAttackHit(CHARACTER_WORK *offset, int damage);
void CPUEM_Damage(CHARACTER_WORK *offset);
void DecGuardPoint(CHARACTER_WORK *offset, int lvflag);
void HitDispRequest(CHARACTER_WORK *offset, TACTHEADER *ahp);
void PlayerPoisonClear(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);

void SetHitParam(CHARACTER_WORK *offset, int st, TACTHEADER *ahp)
{
    unsigned short lv;
    unsigned short lvflag;
    unsigned short *tb;
    static unsigned short StandTb[] = { 0x25, 0x26, 0x27, 0x27 };
    static unsigned short StandLowTb[] = { 0x28, 0x29, 0x2A, 0x2A };
    static unsigned short SitTb[] = { 0x2B, 0x2C, 0x2D, 0x2D };
    static unsigned short SkyTb[] = { 0x31, 0x31, 0x31, 0x31 };
    static short SpeedTb[] = { -1100, -1400, -2000, -2000 };

    lv = ahp->lvflag & 3;
    lvflag = ahp->lvflag;
    offset->GuardSt &= ~8;
    HitDispRequest(offset, ahp);
    CPUEM_Damage(offset);
    if (st != 3) {
        int speed;
        if (st == 0) {
            if (-offset->DamColliPos[0].y < 10000) {
                tb = StandLowTb;
            } else {
                tb = StandTb;
            }
        } else if (st == 1) {
            tb = SitTb;
        } else {
            int time;
            time = lv * 4 + 10;
            if (offset->ply->DustTime) {
                time = lv * 4 + 20;
            }
            SetDownTimer(offset, time, offset->DamParam->DownUkemiTime);
            tb = SkyTb;
        }
        offset->ply->ShakeTime = 12;
        SetActNo(offset, tb[lv] | 0x400000);
        speed = SpeedTb[lv];
        if (ahp->flag & 0x20000) {
            if (ahp->flag & 0x10000) {
                speed = speed * 1.5f;
            } else {
                speed = 0;
            }
        } else if (ahp->flag & 0x10000) {
            speed = speed / 2;
        }
        SetXSpeed(offset, speed);
        SetHitBack(offset, offset->DamColliAddr[0], speed);
    }
    Add_TensionVal_atAttackHit(offset->ply->Enemy, ahp->damage);
    PlayerPoisonClear(offset->ply->Enemy);
    offset->ply->DownFlag &= ~0x213;
    offset->ply->DownBoundDamage = 0;
    offset->actst |= 0x20;
    offset->ply->JumpFlag &= ~2;
    DecGuardPoint(offset, lvflag);
    if (offset->idno == 7) {
        offset->ply->PersonalWork[1].w[0] = 0;
    }
}

void font_entry(int side, char *str);
int g_roundmin(int val, int min);
int sound_request(int no);

void SetHitStop(CHARACTER_WORK *offset, unsigned int flag, unsigned short lvflag, int type)
{
    unsigned short lv;
    static char tb[] = { 0x16, 0x1A, 0x1C, 0x1C };
    static char ctb[] = { 0x16, 0x22, 0x34, 0x30 };

    if (flag & 0x200) {
        type = 0;
    }
    lv = lvflag & 3;
    if (flag & 0x100000) {
        offset->HitStopTime = g_roundmin(offset->HitStopTime, 0);
        return;
    }
    if (offset->GuardSt & 0x200) {
        if (offset->GuardSt & 3) {
            type = 0;
        }
    }
    if (offset->GuardSt & 0x400) {
        type = 0;
    }
    if (type && (offset->ply->DownFlag & 4)) {
        if (flag & 0x200000) {
            offset->HitStopTime = g_roundmin(offset->HitStopTime, ctb[lv] / 2 & 0xFE);
        } else {
            offset->HitStopTime = g_roundmin(offset->HitStopTime, ctb[lv]);
        }
        if (lv) {
            sound_request(0x5D);
            sound_request(0x5D);
            sound_request(0x5D);
            sound_request(0x5D);
            sound_request(0x5D);
            font_entry(offset->padid == 0, "COUNTER HIT");
        }
    } else {
        if (flag & 0x200000) {
            offset->HitStopTime = g_roundmin(offset->HitStopTime, tb[lv] / 2 & 0xFE);
        } else {
            offset->HitStopTime = g_roundmin(offset->HitStopTime, tb[lv]);
        }
    }
}

int CounterCheck(CHARACTER_WORK *offset, unsigned short lvflag)
{
    return 0;
}

extern short PDDefPoint[];
extern short PDEneAttackRate[];
extern short PDKonjo[];
extern int PLAYER_HITPOINTMAX;

int GetDefDamage(CHARACTER_WORK *offset)
{
    int d;
    int h;
    static unsigned char KonjoTb[48] = {
        255, 255, 255, 232, 192, 148, 124, 100,
        255, 255, 240, 224, 176, 144, 120, 100,
        255, 255, 232, 208, 168, 140, 116, 96,
        255, 240, 224, 200, 164, 136, 112, 92,
        255, 240, 224, 200, 160, 128, 100, 92,
        255, 232, 212, 184, 148, 120, 96, 88,
    };
    int index;

    d = PDDefPoint[offset->padid] + 256;
    index = PDKonjo[offset->padid] * 8;
    if (offset->HitPoint < PLAYER_HITPOINTMAX * 3 / 64) {
        index += 7;
    } else if (offset->HitPoint < PLAYER_HITPOINTMAX * 6 / 64) {
        index += 6;
    } else if (offset->HitPoint < PLAYER_HITPOINTMAX * 10 / 64) {
        index += 5;
    } else if (offset->HitPoint < PLAYER_HITPOINTMAX * 14 / 64) {
        index += 4;
    } else if (offset->HitPoint < PLAYER_HITPOINTMAX * 20 / 64) {
        index += 3;
    } else if (offset->HitPoint < PLAYER_HITPOINTMAX * 27 / 64) {
        index += 2;
    } else if (offset->HitPoint < PLAYER_HITPOINTMAX * 36 / 64) {
        index += 1;
    }
    d = d * (PDEneAttackRate[offset->padid == 0] + 16) / 16;
    h = d * KonjoTb[index];
    return h / 256;
}

void AddFaintPoint(CHARACTER_WORK *offset, int damage);
int GetBattleTimeStopFlag(int padid);
int GetGuardCounterDamage(CHARACTER_WORK *offset, TACTHEADER *ahp);
int IsBattleDecision(void);
void RequestItigeki(void);
int RequestWin(int id);
void Tension_Add_Damage(CHARACTER_WORK *offset, int dam, TACTHEADER *ahp);
int boss_damage_hosei(CHARACTER_WORK *offset, int damage);
void lets_purupuru(CHARACTER_WORK *offset, int damage);
int sv_damage_hosei(CHARACTER_WORK *offset, int damage);
extern short UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern SYS_COND sc;
extern TRAINING_SETTING *tr_info;
extern SURVIVAL_INFOMATION *sv_info;
extern CHARANGE_CTRL *gl_info;
extern int PLAYER_HITPOINTMAX;

int PlayerDamage(CHARACTER_WORK *offset, int dam, TACTHEADER *ahp)
{
    int damage;

    damage = dam;
    if ((g_info.mode & 0x8000) && offset->ply->CPU) {
        if (!gl_info->attack[gl_info->ply == 0][0]) {
            if (!ahp) {
                damage = 0;
            } else if (!(ahp->flag & 0x6000004)) {
                damage = 0;
            }
        }
        if (!gl_info->attack[gl_info->ply == 0][1] && ahp) {
            if ((ahp->flag & 0x6000004) == 4) {
                damage = 0;
            }
        }
        if (!gl_info->attack[gl_info->ply == 0][2] && ahp) {
            if ((ahp->flag & 0x6000000) == 0x4000000) {
                damage = 0;
            }
        }
    }
    if (!damage) {
        return 0;
    }
    if (UseCharacter1p2p[offset->padid == 0] == 11 && GetBattleTimeStopFlag(offset->padid == 0)
        && (g_info.BattleControlFlag & 1)) {
        return 0;
    }
    damage = damage * GetGuardCounterDamage(offset, ahp) / 256;
    damage = damage * GetDefDamage(offset) / 256;
    if (offset->actst & 0x2000000) {
        damage /= 2;
    }
    if (offset->ply->DefenceDown) {
        damage = damage * 1.5f;
    }
    if (damage == 0) {
        damage++;
    }
    if (g_info.mode & 0x200) {
        damage = sv_damage_hosei(offset, damage);
    } else {
        damage = boss_damage_hosei(offset, damage);
    }
    lets_purupuru(offset, damage);
    if (!IsBattleDecision()) {
        if (damage >= offset->HitPoint) {
            int b;
            if (offset->HitPoint > PLAYER_HITPOINTMAX / 4) {
                b = PLAYER_HITPOINTMAX / 5;
            } else if (offset->HitPoint > PLAYER_HITPOINTMAX / 6) {
                b = PLAYER_HITPOINTMAX / 8;
            } else {
                b = 0;
            }
            if (b) {
                int a = offset->HitPoint - b;
                damage -= (damage - a) / 2;
            }
        }
        offset->HitPoint -= damage;
        if (ahp && (ahp->flag & 0x1000000)) {
            if (offset->HitPoint - damage <= 0) {
                offset->HitPoint = 1;
            }
        } else if ((g_info.mode & 0x8000) && gl_info->undead[offset->padid]) {
            if (offset->HitPoint - damage <= 0) {
                offset->HitPoint = 1;
            }
        }
        Tension_Add_Damage(offset, dam, ahp);
        if (ahp && (ahp->flag & 0x2000000)) {
            offset->HitPoint = 0;
            RequestItigeki();
            if (g_info.mode & 0x100) {
                tr_info->itigeki = 3;
            }
        }
    }
    sc.damage[offset->padid] = damage;
    if (offset->HitPoint <= 0) {
        if (!IsBattleDecision() && ahp && (ahp->flag & 0x4000000)
            && (!(g_info.mode & 0x100) || !tr_info->tene.rejene)) {
            if ((g_info.mode & 0x200) && offset->padid != sv_info->ply) {
                sv_info->sv_ext.survival_exp = sv_info->sv_ext.survival_exp + sv_info->sv_ext.survival_level + 100;
            }
            g_info.BattleControlFlag |= 0x40;
        }
        RequestWin(offset->padid);
        offset->HitPoint = 0;
        return 1;
    }
    if (!(g_info.mode & 0x8000) || !gl_info->deku) {
        AddFaintPoint(offset, damage);
    }
    return 0;
}

void AddGuardPoint(CHARACTER_WORK *offset, int lv, TACTHEADER *ahp);
int IsBattleDecision(void);
int RequestWin(int id);
void Tension_Add_Guard(CHARACTER_WORK *offset, int damage);
void Tension_Dec(CHARACTER_WORK *offset, int Val);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;
extern CHARANGE_CTRL *gl_info;

int PlayerGuardDamage(CHARACTER_WORK *offset, int damage, TACTHEADER *ahp)
{
    unsigned int lv;
    DAMAGEPARAM *dp;

    dp = offset->DamParam;
    lv = (ahp->lvflag >> 2) & 3;
    if ((g_info.mode & 0x8000) && offset->ply->CPU) {
        if (!gl_info->attack[gl_info->ply == 0][0]) {
            if (!ahp) {
                damage = 0;
            } else if (!(ahp->flag & 0x6000004)) {
                damage = 0;
            }
        }
        if (!gl_info->attack[gl_info->ply == 0][1] && ahp) {
            if ((ahp->flag & 0x6000004) == 4) {
                damage = 0;
            }
        }
        if (!gl_info->attack[gl_info->ply == 0][2] && ahp) {
            if ((ahp->flag & 0x6000000) == 0x4000000) {
                damage = 0;
            }
        }
    }
    if (!(offset->GuardSt & 8)) {
        AddGuardPoint(offset, lv, ahp);
    }
    if (!(ahp->flag & 4)) {
        if (offset->GuardSt & 8) {
            Tension_Dec(offset, (damage + 10) * 25);
        } else {
            Tension_Add_Guard(offset, damage / 3);
        }
        return 0;
    }
    if (ahp->flag & 0x400000) {
        return 0;
    }
    if (!damage) {
        return 0;
    }
    damage = damage * dp->Kezuri >> 7;
    if (!damage) {
        damage = 1;
    }
    if (offset->GuardSt & 8) {
        Tension_Dec(offset, (damage + 3) * 80);
        return 0;
    }
    Tension_Add_Guard(offset, damage * 15);
    if (IsBattleDecision()) {
        return 0;
    }
    if (offset->HitPoint < 2) {
        if ((g_info.mode & 0x8000) && gl_info->undead[offset->padid]) {
            offset->HitPoint = 1;
        } else {
            if ((ahp->flag & 0x4000000) && (!(g_info.mode & 0x100) || !tr_info->tene.rejene)) {
                g_info.BattleControlFlag |= 0x40;
            }
            RequestWin(offset->padid);
            offset->HitPoint = 0;
        }
        return 1;
    }
    offset->HitPoint -= damage;
    if (offset->HitPoint < 2) {
        offset->HitPoint = 1;
    }
    return 0;
}

short yspeedsub(CHARACTER_WORK *offset, short y)
{
    int hc;

    hc = offset->ply->HitCount - 5;
    if (hc < 0) {
        hc = 0;
    }
    if (hc >= 30) {
        hc = 30;
    }
    return y - y * hc / 60;
}

void DirectionEnemyReverse(CHARACTER_WORK *offset);
void DirectionEnemySame(CHARACTER_WORK *offset);

void SetDownDir(CHARACTER_WORK *offset, unsigned int DownFlag)
{
    if (DownFlag & 0x1000) {
        DirectionEnemyReverse(offset);
    }
    if (DownFlag & 0x800) {
        DirectionEnemySame(offset);
    }
}

int GetCenterX(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
int g_roundmax(int val, int max);

void SetDownPower(CHARACTER_WORK *offset, short Defaultx, short Defaulty, short Defaultg, short Defaulthitback, DAMAGEPARAM *dp)
{
    unsigned int ahflag;
    int d;

    if (dp) {
        ahflag = offset->DamActHeader[0].flag;
        if (dp->DownY != -1) {
            offset->yspeed += yspeedsub(offset, dp->DownY);
        } else {
            offset->yspeed += yspeedsub(offset, Defaulty);
        }
        if (dp->DownGrav != -1) {
            offset->gravity = dp->DownGrav;
        } else {
            offset->gravity = Defaultg;
        }
        if (ahflag & 0x40000000) {
            offset->posy = g_roundmax(offset->posy, -3000);
        }
        if ((ahflag & 8) && offset->yspeed < 0 && (d = GetCenterX(offset)) != 0) {
            offset->xspeed = d / (-(offset->yspeed * 2) / offset->gravity);
        } else if (dp->DownX == -1) {
            SetXSpeed(offset, Defaultx);
        } else {
            SetXSpeed(offset, -dp->DownX);
        }
        offset->ply->DownFlag &= ~0x400;
        offset->ply->DownFlag |= dp->DownFlag;
    } else {
        offset->yspeed += yspeedsub(offset, Defaulty);
        offset->gravity = Defaultg;
        SetXSpeed(offset, Defaultx);
    }
}

int AN_GuardMotionAttackCheck(CHARACTER_WORK *offset);

void CPUEM_Hit(CHARACTER_WORK *offset);
void Tension_Add(CHARACTER_WORK *offset, int Val);
void Tension_Add_Enemy(CHARACTER_WORK *offset, int Val);
void ReqCmnSE_Sousai(CHARACTER_WORK *offset);
int IsCounterHitByGuardCounter(CHARACTER_WORK *offset);
void EliminateTransfaObject(CHARACTER_WORK *off, unsigned char padid);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, unsigned short idno, int actno, unsigned int flag);
void FaintClear(CHARACTER_WORK *offset);
void DirectionDirectChange(CHARACTER_WORK *offset);
int g_round(int val, int max, int min);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
extern short PDDef_Gravity[];

void PlayerColliCheck(CHARACTER_WORK *offset)
{
    CHARACTER_WORK *aoff;
    unsigned int ahflag;
    unsigned short ahlvflag;
    unsigned char ahdamage;
    int cnt;
    TACTHEADER *ahp;

    offset->GuardSt &= ~0x20;
    switch (offset->HitColliFlag) {
    case -10:
        offset->ActHitCount++;
        offset->ColliFlag = offset->HitColliFlag;
        offset->actst |= 0x40;
        offset->attackst |= 0x1000;
        offset->HitStopTime = 0x20;
        Tension_Add(offset, 500);
        offset->ply->sousaitime = 26;
        if (offset->padid == 0) {
            ReqCmnSE_Sousai(offset);
        }
        break;
    case 0:
        break;
    default:
        CPUEM_Hit(offset);
        offset->ActHitCount++;
        offset->ColliFlag = offset->HitColliFlag;
        offset->actst |= 0x40;
        SetHitStop(offset, offset->ActHeader.flag, offset->ActHeader.lvflag, 0);
        if (offset->HitColliFlag == -1) {
            offset->attackst |= 0x80000;
            if (!offset->DamColliFlag && !(offset->actst & 0x20) && offset->ply->AtkHitInterrupt) {
                ((void (*)(CHARACTER_WORK *))offset->ply->AtkHitInterrupt)(offset);
            }
        } else {
            if (!offset->DamColliFlag && !(offset->actst & 0x20) && offset->ply->AtkGuardInterrupt) {
                ((void (*)(CHARACTER_WORK *))offset->ply->AtkHitInterrupt)(offset);
            }
        }
        offset->attackst |= 0x1000;
        if (offset->attackst & 1) {
            if (!(offset->attackst & 0x400)) {
                offset->attackst |= 0x20;
            }
            if ((offset->actst & 0x10) && offset->yspeed > 0) {
                offset->yspeed -= offset->yspeed / 3;
            }
        }
        if (offset->attackst & 0x80) {
            offset->attackst |= 0x100;
        }
        break;
    }
    if (offset->DamColliFlag) {
        EliminateTransfaObject(offset, offset->padid);
        SearchCharacterExecute(offset, 0x4D, 0, 0x16);
        SearchCharacterExecute(offset, 0x4D, 0, 0x13);
        if (offset->actno == 0x46) {
            FaintClear(offset);
        }
        offset->ply->maintain = 0;
        offset->ply->inertia = 0;
        offset->ply->inertiaflag = 0;
        offset->GuardSt &= ~0x10;
        offset->SpriteFlag &= ~0x3200;
        offset->CommandFlag &= ~0x800;
        offset->ply->pactst &= ~3;
        offset->ply->JumpFlag &= ~4;
        for (cnt = 0; cnt < 3 && (aoff = offset->DamColliAddr[cnt]) != 0; cnt++) {
            ahp = &offset->DamActHeader[cnt];
            ahflag = ahp->flag;
            ahlvflag = ahp->lvflag;
            ahdamage = ahp->damage;
            if (offset->GuardSt & 1) {
                offset->actst &= ~0x400;
            }
            if (offset->GuardSt & 2) {
                offset->actst |= 0x400;
            }
            if (!(offset->GuardSt & 8) && !(ahflag & 0x1000)) {
                offset->GuardSt &= ~4;
            }
            if (!(ahflag & 0x400)) {
                offset->GuardSt &= ~1;
            }
            if (!(ahflag & 0x800)) {
                offset->GuardSt &= ~2;
            }
            DirectionDirectChange(offset);
            if (offset->DamColliFlag == 10 && !(ahflag & 0x80)) {
                break;
            }
            offset->ColliFlag = offset->DamColliFlag;
            if (!(offset->DamParam->DownFlag & 8)) {
                offset->ply->DownFlag &= ~4;
            }
            if (offset->GuardSt & 7) {
                Tension_Add_Enemy(offset, aoff->TensionAdd * 6);
            } else {
                Tension_Add_Enemy(offset, aoff->TensionAdd * 12);
                if (IsCounterHitByGuardCounter(offset)) {
                    offset->ply->DownFlag |= 4;
                }
            }
            if (offset->ActHeader.flag & 0x2000) {
                offset->ply->DownFlag |= 4;
            }
            if ((ahflag & 0x8000) && !(offset->ply->DownFlag & 4)) {
                offset->ply->DownFlag |= 0x20;
                offset->DamParam->DamageHosei = 30;
            }
            SetHitStop(offset, ahflag, ahlvflag, 1);
            if (offset->ply->DamageInterrupt
                && ((int (*)(CHARACTER_WORK *, TACTHEADER *, int))offset->ply->DamageInterrupt)(offset, ahp, cnt)) {
                goto next;
            }
            if (offset->posy < 0) {
                offset->actst |= 0x10;
            }
            if (offset->DamParam->DownFlag & 0x100) {
                unsigned short FASHWDamageNoTb2[19] = {
                    0x00, 0x99, 0x8C, 0x92, 0x8A, 0x6D, 0x93, 0xA1, 0x86, 0x7B,
                    0x95, 0x74, 0x9B, 0x79, 0x8B, 0x8B, 0x79, 0x81, 0x81,
                };

                PlayerDamage(offset, ahdamage, ahp);
                HitDispRequest(offset, ahp);
                RequestHit(offset, ahflag, ahlvflag, cnt);
                SetActNo(offset, FASHWDamageNoTb2[offset->idno]);
            } else if (offset->actst & 0x100) {
                if (offset->DamParam->DamInt
                    && ((int (*)(CHARACTER_WORK *, TACTHEADER *, CHARACTER_WORK *, int))offset->DamParam->DamInt)(offset, ahp, aoff, cnt)) {
                    goto next;
                }
                {
                    int yspeed;

                    PlayerDamage(offset, ahdamage >> 1, ahp);
                    SetHitParam(offset, 2, ahp);
                    offset->ply->DownTimer = 8;
                    SetDownDir(offset, offset->DamParam->DownFlag);
                    yspeed = -400;
                    if (offset->actst & 0x10) {
                        yspeed = offset->yspeed;
                    }
                    offset->yspeed = 0;
                    SetDownPower(offset, -600, yspeed, PDDef_Gravity[offset->padid], -600, 0);
                }
                RequestHit(offset, ahflag, ahlvflag, cnt);
            } else if (offset->actst & 0x10) {
                if (offset->GuardSt & 4) {
                    int flag;
                    int basey;

                    flag = offset->actst & 0x200;
                    if (PlayerGuardDamage(offset, ahdamage, ahp)) {
                        goto guardbreak;
                    }
                    SetActNo(offset, 0x3C);
                    SetGuardParam(offset, ahlvflag, ahp);
                    if (flag) {
                        if (offset->yspeed < 0) {
                            offset->yspeed = offset->yspeed * 0.2f;
                        } else {
                            offset->yspeed = offset->yspeed * 0.6f;
                        }
                        offset->yspeed = g_round(offset->yspeed, 900, -400);
                    } else {
                        offset->yspeed = offset->yspeed * 0.4f;
                        offset->yspeed = g_round(offset->yspeed, -300, -600);
                    }
                    if (!(ahlvflag & 3)) {
                        basey = -(ahdamage * 2) - 200;
                    } else {
                        basey = ahdamage * -5 - 400;
                    }
                    SetDownDir(offset, offset->DamParam->DownFlag);
                    SetDownPower(offset, -900, basey, PDDef_Gravity[offset->padid] * 0.7, -600, 0);
                    RequestGuard(offset, ahflag, ahlvflag, cnt);
                } else {
                    if (offset->DamParam->DamInt
                        && ((int (*)(CHARACTER_WORK *, TACTHEADER *, CHARACTER_WORK *, int))offset->DamParam->DamInt)(offset, ahp, aoff, cnt)) {
                        goto next;
                    }
                    if (CounterCheck(offset, ahlvflag)) {
                        goto counter;
                    }
                    if (offset->ply->DustTime) {
                        PlayerDamage(offset, ahdamage, ahp);
                        SetHitParam(offset, 2, ahp);
                        SetDownTimer(offset, 0x30, -1);
                        SetActNo(offset, 0x410034);
                        SetXSpeed(offset, -500);
                    } else if (ahflag & 0x80) {
                        PlayerDamage(offset, ahdamage, ahp);
                        SetHitParam(offset, 2, ahp);
                        if (offset->yspeed > 0) {
                            offset->yspeed = 0;
                        } else if (offset->ply->DustTime) {
                            offset->yspeed -= offset->yspeed / 3;
                        } else {
                            offset->yspeed /= 3;
                        }
                        SetActNo(offset, 0x410044);
                        SetDownDir(offset, offset->DamParam->DownFlag);
                        SetDownPower(offset, -200, -1600, PDDef_Gravity[offset->padid], -300, offset->DamParam);
                    } else {
                        PlayerDamage(offset, ahdamage, ahp);
                        SetHitParam(offset, 2, ahp);
                        if (offset->yspeed > 0) {
                            offset->yspeed = 0;
                        } else if (offset->ply->DustTime) {
                            offset->yspeed -= offset->yspeed / 3;
                        } else {
                            offset->yspeed /= 3;
                        }
                        SetDownDir(offset, offset->DamParam->DownFlag);
                        {
                            short g = PDDef_Gravity[offset->padid];
                            SetDownPower(offset, -600, ahdamage * -5 - 1200, g, -600, offset->DamParam);
                        }
                    }
                    RequestHit(offset, ahflag, ahlvflag, cnt);
                }
            } else {
                if (offset->ply->GuardFunc) {
                    int ret;

                    if (offset->GuardSt & 3) {
                        ret = ((int (*)(CHARACTER_WORK *, TACTHEADER *, CHARACTER_WORK *, int))offset->ply->GuardFunc)(offset, ahp, aoff, cnt);
                        if (ret == -1) {
                            goto guardbreak;
                        }
                        if (ret) {
                            goto next;
                        }
                    }
                    if (offset->ply->GuardFunc && (offset->GuardSt & 0x400)) {
                        ret = ((int (*)(CHARACTER_WORK *, TACTHEADER *, CHARACTER_WORK *, int))offset->ply->GuardFunc)(offset, ahp, aoff, cnt);
                        if (ret == -1) {
                            goto guardbreak;
                        }
                        if (ret) {
                            goto next;
                        }
                    }
                }
                if (offset->GuardSt & 1) {
                    if (PlayerGuardDamage(offset, ahdamage, ahp)) {
                        goto guardbreak;
                    }
                    if (ahflag & 0x20) {
                        SetActNo(offset, 0x38);
                    } else {
                        SetActNo(offset, 0x36);
                    }
                    SetGuardParam(offset, ahlvflag, ahp);
                    RequestGuard(offset, ahflag, ahlvflag, cnt);
                } else if (offset->GuardSt & 2) {
                    if (PlayerGuardDamage(offset, ahdamage, ahp)) {
                        goto guardbreak;
                    }
                    SetActNo(offset, 0x3A);
                    SetGuardParam(offset, ahlvflag, ahp);
                    RequestGuard(offset, ahflag, ahlvflag, cnt);
                } else {
                    if (offset->DamParam->DamInt
                        && ((int (*)(CHARACTER_WORK *, TACTHEADER *, CHARACTER_WORK *, int))offset->DamParam->DamInt)(offset, ahp, aoff, cnt)) {
                        goto next;
                    }
                    if (ahflag & 0x200) {
                    counter:
                        PlayerDamage(offset, ahdamage, ahp);
                        SetHitParam(offset, 2, ahp);
                        SetDownTimer(offset, 0x30, offset->DamParam->DownUkemiTime);
                        SetActNo(offset, 0x400034);
                        offset->yspeed = 0;
                        SetDownDir(offset, offset->DamParam->DownFlag);
                        SetDownPower(offset, -500, -1400, 0, -600, offset->DamParam);
                        offset->ply->DustTime = 30;
                        RequestHit(offset, ahflag, ahlvflag, cnt);
                    } else if (CounterCheck(offset, ahlvflag)) {
                        goto counter;
                    } else if (ahflag & 0x10) {
                        PlayerDamage(offset, ahdamage, ahp);
                        SetHitParam(offset, 2, ahp);
                        SetDownTimer(offset, 0x14, offset->DamParam->DownUkemiTime);
                        SetActNo(offset, 0x410043);
                        offset->yspeed = 0;
                        SetDownDir(offset, offset->DamParam->DownFlag);
                        SetDownPower(offset, -300, -800, PDDef_Gravity[offset->padid], -300, offset->DamParam);
                        RequestHit(offset, ahflag, ahlvflag, cnt);
                    } else if (ahflag & 0x80) {
                        PlayerDamage(offset, ahdamage, ahp);
                        SetHitParam(offset, 2, ahp);
                        SetDownTimer(offset, 0x1C, offset->DamParam->DownUkemiTime);
                        SetActNo(offset, 0x410044);
                        offset->yspeed = 0;
                        SetDownDir(offset, offset->DamParam->DownFlag);
                        SetDownPower(offset, -200, -2000, PDDef_Gravity[offset->padid], -300, offset->DamParam);
                        RequestHit(offset, ahflag, ahlvflag, cnt);
                    } else if (ahflag & 0x100) {
                        PlayerDamage(offset, ahdamage, ahp);
                    guardbreak:
                        offset->GuardSt &= ~0xF;
                        SetHitParam(offset, 2, ahp);
                        SetDownTimer(offset, 0x1C, offset->DamParam->DownUkemiTime);
                        SetActNo(offset, 0x410031);
                        offset->yspeed = 0;
                        SetDownDir(offset, offset->DamParam->DownFlag);
                        SetDownPower(offset, -400, -1000, PDDef_Gravity[offset->padid], -600, offset->DamParam);
                        RequestHit(offset, ahflag, ahlvflag, cnt);
                    } else if (offset->actst & 0x400) {
                        if (PlayerDamage(offset, ahdamage, ahp)) {
                            goto guardbreak;
                        }
                        SetHitParam(offset, 1, ahp);
                        RequestHit(offset, ahflag, ahlvflag, cnt);
                    } else {
                        if (PlayerDamage(offset, ahdamage, ahp)) {
                            goto guardbreak;
                        }
                        SetHitParam(offset, 0, ahp);
                        RequestHit(offset, ahflag, ahlvflag, cnt);
                    }
                }
            }
        next:
            if (offset->DamParam->DownFlag & 0x4000) {
                offset->actst |= 0x2000000;
            }
        }
    }
    offset->HitColliFlag = 0;
    offset->DamColliFlag = 0;
}


int CmnGuardAct(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt)
{
    offset->attackst &= ~0x100000;
    if (offset->idno == 13) {
        offset->ply->ExCommandFunc = AN_GuardMotionAttackCheck;
    }
    if (PlayerGuardDamage(offset, header->damage, header)) {
        return -1;
    }
    RequestGuard(offset, header->flag, header->lvflag, cnt);
    return 1;
}
