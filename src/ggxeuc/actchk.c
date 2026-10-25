#include "generated/actchk.h"

static const short int idtb[] = { 1, 2, 3 };

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int GetXMove(CHARACTER_WORK *offset, int val);
int IsDead(CHARACTER_WORK *offset);
int IsFaint(CHARACTER_WORK *offset);
int IsUkemi(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetSkyDashCount(CHARACTER_WORK *offset);
void SetSkyJumpCount(CHARACTER_WORK *offset);
void font_entry(int side, char *str);
extern PAD GameKey[2];
extern short int Jump_GravityByIdno[];
extern short int Jump_YSpeedByIdno[];

int UkemiCheck(CHARACTER_WORK *offset)
{
    CREATE_ARG c_arg;

    if (IsFaint(offset)) return 0;
    if (IsDead(offset)) return 0;
    if (offset->yspeed > 500 && offset->posy >= -9999 && !(GameKey[offset->padid].data & 0xA000)) return 0;
    if (offset->actst & 0x4000) return 0;
    if (IsUkemi(offset)) {
        font_entry(offset->padid, "RECOVERY");
        c_arg.flag = 1;
        c_arg.x = offset->posx;
        c_arg.y = offset->posy - 6000;
        EffectCreate(offset, 0x70, &c_arg);
        if (GameKey[offset->padid].data & 0x2000) {
            offset->yspeed = Jump_YSpeedByIdno[offset->padid];
            offset->xspeed /= 2;
            if (offset->actst & 0x800) {
                offset->xspeed += GetXMove(offset, 1300);
            } else {
                offset->xspeed += GetXMove(offset, 800);
            }
            offset->ply->MutekiTime = 16;
        } else if (GameKey[offset->padid].data & 0x8000) {
            offset->xspeed /= 2;
            offset->yspeed = Jump_YSpeedByIdno[offset->padid] / 2;
            offset->xspeed += GetXMove(offset, -600);
            offset->ply->MutekiTime = 14;
        } else {
            offset->yspeed += Jump_YSpeedByIdno[offset->padid] / 2;
            offset->ply->MutekiTime = 16;
        }
        offset->gravity = Jump_GravityByIdno[offset->padid];
        offset->SpriteFlag = (offset->SpriteFlag & ~0x3000) | 0x200;
        offset->ply->whiteouttime = 6;
        offset->actst &= 0xFDFFFEFF;
        SetSkyDashCount(offset);
        SetSkyJumpCount(offset);
        offset->ply->DownTimer = -1;
        SetActNo(offset, 0x10042);
        return 1;
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int IsKirimomi(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void SetLR(CHARACTER_WORK *offset);
void SetSkyDashCount(CHARACTER_WORK *offset);
void SetSkyJumpCount(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
extern PAD GameKey[2];
extern short int HighJump_GravityByIdno[];
extern short int HighJump_YSpeedByIdno[];

int HomingJumpCancelCheck(CHARACTER_WORK *offset)
{
    if (offset->ply->JumpSeal) return 0;
    if (IsKirimomi(offset)) {
        if (!(GameKey[offset->padid].data & 0x1000)) return 0;
        if (!(offset->actst & 0x10)) {
            DirectionDirectChange(offset);
            SetLR(offset);
        }
        SetXSpeed(offset, 600);
        offset->yspeed = HighJump_YSpeedByIdno[offset->padid] * 0.5;
        offset->gravity = HighJump_GravityByIdno[offset->padid] / 2;
        SetSkyDashCount(offset);
        SetSkyJumpCount(offset);
        offset->ply->JumpFlag = 1;
        offset->ply->JumpDir = 0x21;
        offset->ply->HomingJumpDir = offset->dirflag;
        offset->ply->inertia = 0;
        SetAttackActNo(offset, 0x8000B);
        return 1;
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int IsHighJump(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void SetLR(CHARACTER_WORK *offset);
void SetSkyDashCount(CHARACTER_WORK *offset);
void SetSkyJumpCount(CHARACTER_WORK *offset);
int get_player(void);
extern PAD GameKey[2];
extern short int HighJump_GravityByIdno[];
extern short int HighJump_YSpeedByIdno[];
extern short int Jump_GravityByIdno[];
extern short int Jump_YSpeedByIdno[];
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

int JumpCheck(CHARACTER_WORK *offset)
{
    offset->ply->JumpFlag &= 0xF8;
    if ((g_info.mode & 0x4000) && get_player() != offset->padid) return 0;
    if (offset->ply->JumpSeal) return 0;
    if (IsHighJump(offset)) {
        offset->ply->maintain = 0;
        offset->ply->SkyDashCorrect = 0;
        if (!(offset->actst & 0x10)) {
            DirectionDirectChange(offset);
            SetLR(offset);
        }
        offset->yspeed = HighJump_YSpeedByIdno[offset->padid];
        offset->gravity = HighJump_GravityByIdno[offset->padid];
        SetSkyDashCount(offset);
        if (GameKey[offset->padid].data & 0x2000) {
            offset->ply->JumpFlag = 1;
            offset->actnoSv = 9;
            offset->ply->inertia /= 2;
            offset->ply->JumpDir = 5;
        } else if (GameKey[offset->padid].data & 0x8000) {
            offset->ply->JumpFlag = 1;
            offset->actnoSv = 10;
            offset->ply->inertia = 0;
            offset->ply->JumpDir = 6;
        } else {
            offset->ply->JumpFlag = 1;
            offset->actnoSv = 7;
            offset->ply->inertia /= 2;
            offset->ply->JumpDir = 4;
        }
        SetAttackActNo(offset, 4);
        return 1;
    }
    if (GameKey[offset->padid].data & 0x1000) {
    offset->ply->maintain = 0;
    offset->ply->SkyDashCorrect = 0;
    if (!(offset->actst & 0x10)) {
        DirectionDirectChange(offset);
        SetLR(offset);
    }
    offset->yspeed = Jump_YSpeedByIdno[offset->padid];
    offset->gravity = Jump_GravityByIdno[offset->padid];
    SetSkyDashCount(offset);
    SetSkyJumpCount(offset);
    if (!(g_info.mode & 0x100) || offset->padid == tr_info->ply || tr_info->tene.mode == 1
        || tr_info->tene.mode == 2 || tr_info->tene.memflg >= 2) {
        if (GameKey[offset->padid].data & 0x2000) {
            offset->actnoSv = 9;
            offset->ply->inertia /= 2;
            offset->ply->JumpDir = 1;
        } else if (GameKey[offset->padid].data & 0x8000) {
            offset->actnoSv = 10;
            offset->ply->inertia = 0;
            offset->ply->JumpDir = 2;
        } else {
            offset->actnoSv = 7;
            offset->ply->inertia /= 2;
            offset->ply->JumpDir = 0;
        }
    } else {
        offset->actnoSv = 7;
        offset->ply->inertia /= 2;
        offset->ply->JumpDir = 0;
    }
    SetAttackActNo(offset, 4);
    return 1;
    }
    return 0;
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern PAD GameKey[2];
extern short int Jump_GravityByIdno[];
extern short int Jump_YSpeedByIdno[];
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

int SkyJumpCheck(CHARACTER_WORK *offset)
{
    if (offset->ply->JumpCount <= 0) return 0;
    if (offset->ply->JumpSeal) return 0;
    if (offset->yspeed < 0 && offset->posy >= -6999) return 0;
    if (GameKey[offset->padid].trg & 0x1000) {
        if (offset->idno == 8) {
            SetAttackActNo(offset, 0x61);
        } else {
            offset->ply->JumpFlag = 1;
            offset->yspeed = Jump_YSpeedByIdno[offset->padid] * 0.875;
            offset->gravity = Jump_GravityByIdno[offset->padid];
            if (!(g_info.mode & 0x100) || offset->padid == tr_info->ply || tr_info->tene.mode == 1
                || tr_info->tene.mode == 2 || tr_info->tene.memflg >= 2) {
                if (GameKey[offset->padid].data & 0x2000) {
                    offset->ply->JumpDir = 0x11;
                    SetAttackActNo(offset, 9);
                } else if (GameKey[offset->padid].data & 0x8000) {
                    offset->ply->JumpDir = 0x12;
                    SetAttackActNo(offset, 10);
                } else {
                    offset->ply->JumpDir = 0x10;
                    SetAttackActNo(offset, 7);
                }
            } else {
                offset->ply->JumpDir = 0x10;
                SetAttackActNo(offset, 7);
            }
        }
        offset->ply->JumpCount--;
        offset->ply->SkyDashCount--;
        return 1;
    }
    return 0;
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
extern PAD GameKey[2];

int SitCheck(CHARACTER_WORK *offset)
{
    if (GameKey[offset->padid].data & 0x4000) {
        if (offset->idno == 10 && (GameKey[offset->padid].data & 0xA000)) return 0;
        if (offset->actst & 0x1400) {
            SetActNo(offset, 1);
        } else {
            SetActNo(offset, 2);
        }
        return 1;
    }
    return 0;
}

unsigned int DirectDirectionKeyGet(CHARACTER_WORK *offset);
void DirectionDirectChange(CHARACTER_WORK *offset);
int GetCPUFlag(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

int ForwardWalkCheck(CHARACTER_WORK *offset)
{
    if (GetCPUFlag(offset) && GetCPUFlag(offset) != 2) return 0;
    if ((DirectDirectionKeyGet(offset) & 0x6000) == 0x2000) {
        DirectionDirectChange(offset);
        SetActNo(offset, 5);
        return 1;
    }
    return 0;
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
extern PAD GameKey[2];

int SitForwardWalkCheck(CHARACTER_WORK *offset)
{
    if (offset->idno != 10) return 0;
    if ((GameKey[offset->padid].data & 0x2000) && (GameKey[offset->padid].data & 0x4000)) {
        SetActNo(offset, 0x61);
        return 1;
    }
    return 0;
}

unsigned int DirectDirectionKeyGet(CHARACTER_WORK *offset);
void DirectionDirectChange(CHARACTER_WORK *offset);
int GetCPUFlag(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int get_dekustat(CHARACTER_WORK *offset);

int BackWalkCheck(CHARACTER_WORK *offset)
{
    unsigned int key = DirectDirectionKeyGet(offset);

    if (!get_dekustat(offset)) return 0;
    if (GetCPUFlag(offset) && GetCPUFlag(offset) != 3) return 0;
    if ((key & 0xC000) == 0x8000) {
        DirectionDirectChange(offset);
        SetActNo(offset, 6);
        return 1;
    }
    return 0;
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int get_dekustat(CHARACTER_WORK *offset);
extern PAD GameKey[2];

int SitBackWalkCheck(CHARACTER_WORK *offset)
{
    if (get_dekustat(offset)) {
        if (offset->idno != 10) return 0;
        if ((GameKey[offset->padid].data & 0x8000) && (GameKey[offset->padid].data & 0x4000)) {
            SetActNo(offset, 0x62);
            return 1;
        }
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int GuardCheck(CHARACTER_WORK *offset, int flag);
void SetLR(CHARACTER_WORK *offset);

int TurnGuardCheck(CHARACTER_WORK *offset, int flag)
{
    short unsigned int dirflagsv;

    dirflagsv = offset->dirflag;
    if (!dirflagsv) {
        offset->dirflag = 1;
    } else {
        offset->dirflag = 0;
    }
    SetLR(offset);
    if (GuardCheck(offset, 0)) {
        DirectionDirectChange(offset);
        SetLR(offset);
        return 1;
    }
    offset->dirflag = dirflagsv;
    return 0;
}

unsigned int GetOtherAttackFlag(CHARACTER_WORK *offset);
CHARACTER_WORK *GetOtherPlayer(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int TurnGuardCheck(CHARACTER_WORK *offset, int flag);

int TurnCheck(CHARACTER_WORK *offset)
{
    int flag;
    CHARACTER_WORK *other;

    other = GetOtherPlayer(offset);
    flag = GetOtherAttackFlag(offset);
    if (offset->dirflag == 0 && offset->posx < other->posx) {
        if (flag && TurnGuardCheck(offset, flag)) return 1;
        if (offset->actno == 0 || offset->actno == 5 || offset->actno == 6
            || (offset->actno == 0x52 && offset->idno == 13)
            || (offset->actno == 0x91 && offset->idno == 11) || offset->actno == 0x3D) {
            SetActNo(offset, 0x12);
            return 1;
        }
        if (offset->actno == 1) {
            SetActNo(offset, 0x14);
            return 1;
        }
    }
    if (offset->dirflag == 1 && other->posx < offset->posx) {
        if (flag && TurnGuardCheck(offset, flag)) return 1;
        if (offset->actno == 0 || offset->actno == 5 || offset->actno == 6
            || (offset->actno == 0x52 && offset->idno == 13)
            || (offset->actno == 0x91 && offset->idno == 11) || offset->actno == 0x3D) {
            SetActNo(offset, 0x11);
            return 1;
        }
        if (offset->actno == 1) {
            SetActNo(offset, 0x13);
            return 1;
        }
    }
    return 0;
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);

int FaintCheck(CHARACTER_WORK *offset)
{
    if (offset->ply->FaintPoint == -1) {
        SetActNo(offset, 0x46);
        return 1;
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
unsigned int GetOtherAttackFlag(CHARACTER_WORK *offset);
void PerfectGuardEnableCheck(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetGuardSt(CHARACTER_WORK *offset, int pgflag);
void SetLR(CHARACTER_WORK *offset);
extern int BattleStatus;

int GuardCheck(CHARACTER_WORK *offset, int flag)
{
    unsigned int gf;

    gf = 0;
    if (!(offset->GuardSt & 0x200)) {
        if (offset->ply->maintain) return 0;
        if (flag) {
            gf = ~offset->CommandFlag & 0x100;
            offset->CommandFlag |= 0x100;
        }
        if ((offset->actst & 0x80000) && offset->HitStopTime) return 0;
        PerfectGuardEnableCheck(offset);
        if (flag) {
            SetGuardSt(offset, 2);
        } else {
            SetGuardSt(offset, 3);
        }
        offset->CommandFlag &= ~gf;
        if (offset->actst & 0x200) return 1;
        if (BattleStatus != 2) {
            offset->GuardSt &= 0xFFF0;
            return 0;
        }
        if (offset->GuardSt & 0xF) {
            if (offset->GuardSt & 8) {
                if (offset->GuardSt & 4) {
                    SetActNo(offset, 0x15003B);
                } else {
                    DirectionDirectChange(offset);
                    SetLR(offset);
                    if (offset->GuardSt & 2) {
                        SetActNo(offset, 0x140039);
                    } else {
                        SetActNo(offset, 0x140035);
                    }
                }
                return 1;
            }
            if (flag) return 0;
            flag = GetOtherAttackFlag(offset);
            if (!flag) return 0;
            if (!(offset->GuardSt & 4)) {
                if (offset->GuardSt & 2) {
                    SetActNo(offset, 0x140039);
                } else if (flag & 0x100) {
                    SetActNo(offset, 0x140037);
                } else {
                    SetActNo(offset, 0x140035);
                }
            }
            return 1;
        }
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int GetPlayerDist(void);
int IsDown(CHARACTER_WORK *offset);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int IsTrgHighSlash(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void SetLR(CHARACTER_WORK *offset);
extern PAD GameKey[2];
extern short int NearSlash_DistByIdno[];
extern unsigned int NormalAttackDisableTableByIdno[];

int CmnAttackCheck(CHARACTER_WORK *offset, unsigned int orgflag)
{
    unsigned int flag;
    unsigned int actflag;

    actflag = ~orgflag ? 0x800000 : 0;
    flag = orgflag & NormalAttackDisableTableByIdno[offset->idno];
    if (offset->actst & 0x10) {
        if (IsTrgPunch(offset) && (flag & 0x1000)) {
            SetAttackActNo(offset, actflag | 0xB0021);
            return 1;
        }
        if (offset->idno == 7 && IsTrgKick(offset) && (flag & 0x80000) && IsDown(offset)) {
            SetAttackActNo(offset, actflag | 0xB0070);
            return 1;
        }
        if (offset->idno == 10 && IsTrgKick(offset) && (flag & 0x80000) && IsDown(offset)) {
            SetAttackActNo(offset, actflag | 0xB006D);
            return 1;
        }
        if (IsTrgKick(offset) && (flag & 0x2000)) {
            SetAttackActNo(offset, actflag | 0xB0022);
            return 1;
        }
        if (IsTrgSlash(offset) && (flag & 0x4000)) {
            SetAttackActNo(offset, actflag | 0xB0023);
            return 1;
        }
        if (offset->idno == 3 && IsTrgHighSlash(offset) && (flag & 0x80000) && IsDown(offset)) {
            SetAttackActNo(offset, actflag | 0xB0057);
            return 1;
        }
        if (IsTrgHighSlash(offset) && (flag & 0x8000)) {
            SetAttackActNo(offset, actflag | 0xB0024);
            return 1;
        }
    }
    if (GameKey[offset->padid].data & 0x4000) {
        if (IsTrgPunch(offset) && (flag & 0x80)) {
            SetAttackActNo(offset, actflag | 0x8001D);
            goto done;
        }
        if (offset->idno == 3 && IsTrgKick(offset) && (flag & 0x100000) && IsSitRight(offset)) {
            SetAttackActNo(offset, actflag | 0x80055);
            goto done;
        }
        if (IsTrgKick(offset) && (flag & 0x100)) {
            SetAttackActNo(offset, actflag | 0x8001E);
            goto done;
        }
        if (IsTrgSlash(offset) && (flag & 0x400)) {
            SetAttackActNo(offset, actflag | 0x8001F);
            goto done;
        }
        if (IsTrgHighSlash(offset) && (flag & 0x800)) {
            SetAttackActNo(offset, actflag | 0x80020);
            goto done;
        }
    } else {
        if (IsTrgPunch(offset) && (flag & 2) && IsStdRight(offset)) {
            SetAttackActNo(offset, actflag | 0x8003E);
            goto done;
        }
        if (IsTrgPunch(offset) && (flag & 1)) {
            SetAttackActNo(offset, actflag | 0x80018);
            goto done;
        }
        if (IsTrgKick(offset) && (flag & 0x20000) && IsStdRight(offset)) {
            SetAttackActNo(offset, actflag | 0x80047);
            goto done;
        }
        if (IsTrgKick(offset) && (flag & 4)) {
            SetAttackActNo(offset, actflag | 0x80019);
            goto done;
        }
        if (IsTrgSlash(offset) && (flag & 0x10000) && ~orgflag) {
            SetAttackActNo(offset, actflag | 0x8001A);
            goto done;
        }
        if (NearSlash_DistByIdno[offset->idno] > GetPlayerDist()) {
            if (IsTrgSlash(offset) && (flag & 0x10)) {
                SetAttackActNo(offset, actflag | 0x8001B);
                goto done;
            }
        } else {  /*X*/
            if (IsTrgSlash(offset) && (flag & 8)) {
                SetAttackActNo(offset, actflag | 0x8001A);
                goto done;
            }
        }
        if (IsTrgHighSlash(offset) && (flag & 0x40) && IsStdRight(offset)) {
            SetAttackActNo(offset, actflag | 0x80041);
            goto done;
        }
        if (IsTrgHighSlash(offset) && (flag & 0x20)) {
            SetAttackActNo(offset, actflag | 0x8001C);
            goto done;
        }
    }
    return 0;
done:
    DirectionDirectChange(offset);
    SetLR(offset);
    return 1;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int IsDustAt(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void SetLR(CHARACTER_WORK *offset);
extern PAD GameKey[2];
extern unsigned int NormalAttackDisableTableByIdno[];

int CmnDustAttackCheck(CHARACTER_WORK *offset, unsigned int orgflag)
{
    unsigned int flag;

    flag = orgflag & NormalAttackDisableTableByIdno[offset->idno];
    if (offset->actst & 0x10) return 0;
    if (GameKey[offset->padid].data & 0x4000) {
        if (IsDustAt(offset) && (flag & 0x200)) {
            DirectionDirectChange(offset);
            SetLR(offset);
            SetAttackActNo(offset, 0x80040);
            return 1;
        }
    } else {
        if (IsDustAt(offset) && (flag & 0x40000)) {
            DirectionDirectChange(offset);
            SetLR(offset);
            SetAttackActNo(offset, 0x8003F);
            return 1;
        }
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int IsDash(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int get_player(void);
extern GAME_INFOMATION g_info;

int DashCheck(CHARACTER_WORK *offset)
{
    if ((g_info.mode & 0x4000) && get_player() != offset->padid) return 0;
    if (offset->idno == 6 && g_info.boss[offset->padid] != 1) return 0;
    if (IsDash(offset)) {
        DirectionDirectChange(offset);
        SetActNo(offset, 0xE);
        return 1;
    }
    return 0;
}

int IsBackStep(CHARACTER_WORK *offset);
int IsDash(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int get_player(void);
extern short int SkyDash_Height[];
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;

int SkyDashCheck(CHARACTER_WORK *offset)
{
    if ((g_info.mode & 0x4000) && get_player() != offset->padid) return 0;
    if (offset->idno == 6 && g_info.boss[offset->padid] != 1) return 0;
    if (offset->ply->SkyDashCount > 0) {
        if (!(userinfo.gguse[offset->padid] & 1)) {
            if (offset->yspeed < 0) {
                if (offset->posy > -SkyDash_Height[offset->padid]) return 0;
            } else if (offset->posy >= -3999) {
                return 0;
            }
        }
        if (!(offset->actst & 0x10)) return 0;
        if (IsDash(offset)) {
            SetActNo(offset, 0x1000C);
            offset->ply->inertia /= 2;
            offset->ply->SkyDashCount--;
            offset->ply->JumpCount--;
            return 1;
        }
        if (IsBackStep(offset)) {
            SetActNo(offset, 0x1000D);
            offset->ply->inertia /= 2;
            offset->ply->SkyDashCount--;
            offset->ply->JumpCount--;
            return 1;
        }
    }
    return 0;
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int IsBackStep(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

int BackStepCheck(CHARACTER_WORK *offset)
{
    if (offset->ply->BackStepInvalid) return 0;
    if (IsBackStep(offset)) {
        DirectionDirectChange(offset);
        SetActNo(offset, 0x10);
        return 1;
    }
    return 0;
}

int IsTrgFuckin();
void SL_ExciteCommnadCheck(CHARACTER_WORK *offset);
void KY_ExciteCommnadCheck(CHARACTER_WORK *offset);
void MY_ExciteCommnadCheck(CHARACTER_WORK *offset);
void ML_ExciteCommnadCheck(CHARACTER_WORK *offset);
void AX_ExciteCommnadCheck(CHARACTER_WORK *offset);
void PO_ExciteCommnadCheck(CHARACTER_WORK *offset);
void CH_ExciteCommnadCheck(CHARACTER_WORK *offset);
void ZT_ExciteCommnadCheck(CHARACTER_WORK *offset);
void BK_ExciteCommnadCheck(CHARACTER_WORK *offset);
void FA_ExciteCommnadCheck(CHARACTER_WORK *offset);
void TS_ExciteCommnadCheck(CHARACTER_WORK *offset);
void JM_ExciteCommnadCheck(CHARACTER_WORK *offset);
void AN_ExciteCommnadCheck(CHARACTER_WORK *offset);
void JY_ExciteCommnadCheck(CHARACTER_WORK *offset);
void VE_ExciteCommnadCheck(CHARACTER_WORK *offset);
void DZ_ExciteCommnadCheck(CHARACTER_WORK *offset);
void KR_ExciteCommnadCheck(CHARACTER_WORK *offset);
void JS_ExciteCommnadCheck(CHARACTER_WORK *offset);

int ExciteCommnadCheck(CHARACTER_WORK *offset)
{
    void (*Func)();
    static void *functb[19] = {
        0,
        SL_ExciteCommnadCheck,
        KY_ExciteCommnadCheck,
        MY_ExciteCommnadCheck,
        ML_ExciteCommnadCheck,
        AX_ExciteCommnadCheck,
        PO_ExciteCommnadCheck,
        CH_ExciteCommnadCheck,
        ZT_ExciteCommnadCheck,
        BK_ExciteCommnadCheck,
        FA_ExciteCommnadCheck,
        TS_ExciteCommnadCheck,
        JM_ExciteCommnadCheck,
        AN_ExciteCommnadCheck,
        JY_ExciteCommnadCheck,
        VE_ExciteCommnadCheck,
        DZ_ExciteCommnadCheck,
        KR_ExciteCommnadCheck,
        JS_ExciteCommnadCheck,
    };

    if (offset->actst & 0x10) return 0;
    if (IsTrgFuckin()) {
        Func = functb[offset->idno];
        Func(offset);
        return 1;
    }
    return 0;
}

int IsTrgFuckin();
int IsStdRight(CHARACTER_WORK *offset);
void SL_RespectCommnadCheck(CHARACTER_WORK *offset);
void KY_RespectCommnadCheck(CHARACTER_WORK *offset);
void MY_RespectCommnadCheck(CHARACTER_WORK *offset);
void ML_RespectCommnadCheck(CHARACTER_WORK *offset);
void AX_RespectCommnadCheck(CHARACTER_WORK *offset);
void PO_RespectCommnadCheck(CHARACTER_WORK *offset);
void CH_RespectCommnadCheck(CHARACTER_WORK *offset);
void ZT_RespectCommnadCheck(CHARACTER_WORK *offset);
void BK_RespectCommnadCheck(CHARACTER_WORK *offset);
void FA_RespectCommnadCheck(CHARACTER_WORK *offset);
void TS_RespectCommnadCheck(CHARACTER_WORK *offset);
void JM_RespectCommnadCheck(CHARACTER_WORK *offset);
void AN_RespectCommnadCheck(CHARACTER_WORK *offset);
void JY_RespectCommnadCheck(CHARACTER_WORK *offset);
void VE_RespectCommnadCheck(CHARACTER_WORK *offset);
void DZ_RespectCommnadCheck(CHARACTER_WORK *offset);
void KR_RespectCommnadCheck(CHARACTER_WORK *offset);
void JS_RespectCommnadCheck(CHARACTER_WORK *offset);

int RespectCommnadCheck(CHARACTER_WORK *offset)
{
    void (*Func)();
    static void *functb[19] = {
        0,
        SL_RespectCommnadCheck,
        KY_RespectCommnadCheck,
        MY_RespectCommnadCheck,
        ML_RespectCommnadCheck,
        AX_RespectCommnadCheck,
        PO_RespectCommnadCheck,
        CH_RespectCommnadCheck,
        ZT_RespectCommnadCheck,
        BK_RespectCommnadCheck,
        FA_RespectCommnadCheck,
        TS_RespectCommnadCheck,
        JM_RespectCommnadCheck,
        AN_RespectCommnadCheck,
        JY_RespectCommnadCheck,
        VE_RespectCommnadCheck,
        DZ_RespectCommnadCheck,
        KR_RespectCommnadCheck,
        JS_RespectCommnadCheck,
    };

    if (offset->actst & 0x10) return 0;
    if (!IsTrgFuckin()) return 0;
    if (IsStdRight(offset)) {
        Func = functb[offset->idno];
        Func(offset);
        return 1;
    }
    return 0;
}

int ExciteCommnadCheck(CHARACTER_WORK *offset);
void font_entry(int side, char *str);
int SL_SpecialAttackCheck(CHARACTER_WORK *offset);
int KY_SpecialAttackCheck(CHARACTER_WORK *offset);
int MY_SpecialAttackCheck(CHARACTER_WORK *offset);
int ML_SpecialAttackCheck(CHARACTER_WORK *offset);
int AX_SpecialAttackCheck(CHARACTER_WORK *offset);
int PO_SpecialAttackCheck(CHARACTER_WORK *offset);
int CH_SpecialAttackCheck(CHARACTER_WORK *offset);
int ZT_SpecialAttackCheck(CHARACTER_WORK *offset);
int BK_SpecialAttackCheck(CHARACTER_WORK *offset);
int FA_SpecialAttackCheck(CHARACTER_WORK *offset);
int TS_SpecialAttackCheck(CHARACTER_WORK *offset);
int JM_SpecialAttackCheck(CHARACTER_WORK *offset);
int AN_SpecialAttackCheck(CHARACTER_WORK *offset);
int JY_SpecialAttackCheck(CHARACTER_WORK *offset);
int VE_SpecialAttackCheck(CHARACTER_WORK *offset);
int DZ_SpecialAttackCheck(CHARACTER_WORK *offset);
int KR_SpecialAttackCheck(CHARACTER_WORK *offset);
int JS_SpecialAttackCheck(CHARACTER_WORK *offset);

int SpecialAttackCheck(CHARACTER_WORK *offset)
{
    int (*Func)();
    static void *functb[19] = {
        0,
        SL_SpecialAttackCheck,
        KY_SpecialAttackCheck,
        MY_SpecialAttackCheck,
        ML_SpecialAttackCheck,
        AX_SpecialAttackCheck,
        PO_SpecialAttackCheck,
        CH_SpecialAttackCheck,
        ZT_SpecialAttackCheck,
        BK_SpecialAttackCheck,
        FA_SpecialAttackCheck,
        TS_SpecialAttackCheck,
        JM_SpecialAttackCheck,
        AN_SpecialAttackCheck,
        JY_SpecialAttackCheck,
        VE_SpecialAttackCheck,
        DZ_SpecialAttackCheck,
        KR_SpecialAttackCheck,
        JS_SpecialAttackCheck,
    };
    int ret;

    if (!offset->ply->ArtsSeal) {
        Func = functb[offset->idno];
        ret = Func();
        if (ret) {
            if (offset->actst & 0x8000) {
                font_entry(offset->padid, "REVERSAL");
            }
            return ret;
        }
    }
    return ExciteCommnadCheck(offset);
}

int RespectCommnadCheck(CHARACTER_WORK *offset);
int SL_NoCancelAttackCheck(CHARACTER_WORK *offset);
int KY_NoCancelAttackCheck(CHARACTER_WORK *offset);
int MY_NoCancelAttackCheck(CHARACTER_WORK *offset);
int ML_NoCancelAttackCheck(CHARACTER_WORK *offset);
int AX_NoCancelAttackCheck(CHARACTER_WORK *offset);
int PO_NoCancelAttackCheck(CHARACTER_WORK *offset);
int CH_NoCancelAttackCheck(CHARACTER_WORK *offset);
int ZT_NoCancelAttackCheck(CHARACTER_WORK *offset);
int BK_NoCancelAttackCheck(CHARACTER_WORK *offset);
int FA_NoCancelAttackCheck(CHARACTER_WORK *offset);
int TS_NoCancelAttackCheck(CHARACTER_WORK *offset);
int JM_NoCancelAttackCheck(CHARACTER_WORK *offset);
int AN_NoCancelAttackCheck(CHARACTER_WORK *offset);
int JY_NoCancelAttackCheck(CHARACTER_WORK *offset);
int VE_NoCancelAttackCheck(CHARACTER_WORK *offset);
int DZ_NoCancelAttackCheck(CHARACTER_WORK *offset);
int KR_NoCancelAttackCheck(CHARACTER_WORK *offset);
int JS_NoCancelAttackCheck(CHARACTER_WORK *offset);

int NoCancelAttackCheck(CHARACTER_WORK *offset)
{
    int (*Func)();
    void *functb[19] = {
        0,
        SL_NoCancelAttackCheck,
        KY_NoCancelAttackCheck,
        MY_NoCancelAttackCheck,
        ML_NoCancelAttackCheck,
        AX_NoCancelAttackCheck,
        PO_NoCancelAttackCheck,
        CH_NoCancelAttackCheck,
        ZT_NoCancelAttackCheck,
        BK_NoCancelAttackCheck,
        FA_NoCancelAttackCheck,
        TS_NoCancelAttackCheck,
        JM_NoCancelAttackCheck,
        AN_NoCancelAttackCheck,
        JY_NoCancelAttackCheck,
        VE_NoCancelAttackCheck,
        DZ_NoCancelAttackCheck,
        KR_NoCancelAttackCheck,
        JS_NoCancelAttackCheck,
    };

    if (!offset->ply->ArtsSeal) {
        Func = functb[offset->idno];
        if (Func(offset)) return 1;
    }
    if (RespectCommnadCheck(offset)) return 1;
    return 0;
}

int SL_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int KY_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int MY_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int ML_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int AX_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int PO_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int CH_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int ZT_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int BK_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int FA_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int TS_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int JM_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int AN_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int JY_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int VE_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int DZ_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int KR_GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int JS_GuardSpecialAttackCheck(CHARACTER_WORK *offset);

int GuardSpecialAttackCheck(CHARACTER_WORK *offset)
{
    if (offset->ply->ArtsSeal) return 0;
    switch (offset->idno) {
    default:
        return 0;
    case 1:
        return SL_GuardSpecialAttackCheck(offset);
    case 2:
        return KY_GuardSpecialAttackCheck(offset);
    case 3:
        return MY_GuardSpecialAttackCheck(offset);
    case 4:
        return ML_GuardSpecialAttackCheck(offset);
    case 5:
        return AX_GuardSpecialAttackCheck(offset);
    case 6:
        return PO_GuardSpecialAttackCheck(offset);
    case 7:
        return CH_GuardSpecialAttackCheck(offset);
    case 8:
        return ZT_GuardSpecialAttackCheck(offset);
    case 9:
        return BK_GuardSpecialAttackCheck(offset);
    case 10:
        return FA_GuardSpecialAttackCheck(offset);
    case 11:
        return TS_GuardSpecialAttackCheck(offset);
    case 12:
        return JM_GuardSpecialAttackCheck(offset);
    case 13:
        return AN_GuardSpecialAttackCheck(offset);
    case 14:
        return JY_GuardSpecialAttackCheck(offset);
    case 15:
        return VE_GuardSpecialAttackCheck(offset);
    case 16:
        return DZ_GuardSpecialAttackCheck(offset);
    case 17:
        return KR_GuardSpecialAttackCheck(offset);
    case 18:
        return JS_GuardSpecialAttackCheck(offset);
    }
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int sound_request(int no);

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno)
{
    if (offset->ply->ArtsSeal) return 0;
    if (GetTensionVal(offset) < 5000) return 0;
    if (offset->actst & 0x10) return 0;
    if (!IsDouji(offset, 0xF, 2, 2)) return 0;
    if (IsStdRight(offset) || IsSitRight(offset)) {
        offset->actst |= 0x80;
        sound_request(0x31);
        SetAttackActNo(offset, actno | 0x3080000);
        return 1;
    }
    return 0;
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
short int GetTensionVal(CHARACTER_WORK *offset);
int IsROMANCHANSEL(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void Tension_Dec(CHARACTER_WORK *offset, int Val);
int g_roundmin(int val, int min);
int sound_request(int no);
extern short int Jump_GravityByIdno[];
extern int BattleStatus;

int ROMANCHANSELCheck(CHARACTER_WORK *offset)
{
    int ret;

    if (offset->ply->ArtsSeal) return 0;
    if (BattleStatus != 2) return 0;
    if (GetTensionVal(offset) < 5000) return 0;
    if (IsROMANCHANSEL(offset)) {
    if (offset->actst & 0x10) {
        ret = SetAttackActNo(offset, 0x44010008);
        offset->gravity = g_roundmin(offset->gravity, 50);
    } else {
        SetXSpeed(offset, 0);
        ret = SetAttackActNo(offset, 0x44010000);
    }
    if (ret) {
    sound_request(0x5C);
    sound_request(0x5C);
    sound_request(0x5C);
    sound_request(0x5C);
    sound_request(0x5C);
    offset->SpriteFlag |= 0x200;
    offset->ply->whiteouttime = 5;
    offset->gravity = Jump_GravityByIdno[offset->padid];
    Tension_Dec(offset, -5);
    EffectCreate_ActNo(offset, 0x6F, 0, 0xD);
    EffectCreate_ActNo(offset, 0x6F, 0, 0xD);
    EffectCreate_ActNo(offset, 0x6F, 0, 0xD);
    return 1;
    }
    }
    return 0;
}

int get_dekustat(CHARACTER_WORK *offset);
int StandCheck(CHARACTER_WORK *offset)
{
    u_int st, a, b;
    if (!get_dekustat(offset)) {
        if (GameKey[offset->padid].data & 0x5000) return 0;
        st = offset->actst;
        a = st & 0x400;
        b = st & 0x1000;
        if (!a || b) {
            SetActNo(offset, 0);
        } else {
            SetActNo(offset, 3);
        }
        return 1;
    } else {
        if (GameKey[offset->padid].data & 0xF000) return 0;
        st = offset->actst;
        a = st & 0x400;
        b = st & 0x1000;
        if (!a || b) {
            SetActNo(offset, 0);
        } else {
            SetActNo(offset, 3);
        }
    }
    return 1;
}
