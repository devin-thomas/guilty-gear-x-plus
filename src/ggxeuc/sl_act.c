#include "generated/sl_act.h"

extern GAME_INFOMATION g_info;

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int Random(void);
void set_random_seed_abs(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
int EndtoSit(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int SLHW2ExCommandCheck(CHARACTER_WORK *offset);
extern PAD GameKey[2];
extern TRAINING_SETTING *tr_info;
extern PAD pads[2];
int IsHoldKick(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
extern short int Jump_GravityByIdno[];
int LandtoAction(CHARACTER_WORK *offset, int actno);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);
int EndtoDemo(CHARACTER_WORK *offset);
void SetWinActFlag(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
void Tension_Dec(CHARACTER_WORK *offset, int Val);
void SetAnten(CHARACTER_WORK *offset, int flag);

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*SLAct_jumpTb[])() = {
    CmnAct_Stand, CmnAct_Sit, CmnAct_Stand2Sit, CmnAct_Sit2Stand, CmnAct_JumpReady,
    CmnAct_ForwardWalk, CmnAct_BackWalk, CmnAct_Jump_Vert, CmnAct_Drop_Vert,
    CmnAct_Jump_Forward, CmnAct_Jump_Back, CmnAct_Jump_Homing, CmnAct_JumpDash_Forward,
    CmnAct_JumpDash_Back, CmnAct_Dash, CmnAct_Reduce, CmnAct_BackStep, CmnAct_StandTurnLeft,
    CmnAct_StandTurnRight, CmnAct_SitTurnLeft, CmnAct_SitTurnRight, CmnAct_JumpLand, 0,
    CmnAct_GetUp, CmnAct_StandPunch, CmnAct_StandKick, CmnAct_StandSlash, CmnAct_StandSlashNear,
    CmnAct_StandHighSlash, CmnAct_SitPunch, CmnAct_SitKick, CmnAct_SitSlash,
    CmnAct_SitHighSlash, CmnAct_JumpPunch, CmnAct_JumpKick, CmnAct_JumpSlash,
    CmnAct_JumpHighSlash, CmnAct_StandDamage, CmnAct_StandDamage2, CmnAct_StandDamage3,
    CmnAct_StandLowDamage, CmnAct_StandLowDamage2, CmnAct_StandLowDamage3, CmnAct_SitDamage,
    CmnAct_SitDamage2, CmnAct_SitDamage3, CmnAct_SkyDamage, CmnAct_SkyDamage2, CmnAct_WallBound,
    CmnAct_Down, CmnAct_DownLand, CmnAct_Down2, CmnAct_Kirimomi, CmnAct_StdGuardV,
    CmnAct_StdGuardV2, CmnAct_StdGuardH, CmnAct_StdGuardH2, CmnAct_SitGuard, CmnAct_SitGuard2,
    CmnAct_SkyGuard, CmnAct_SkyGuard2, CmnAct_Stand2, CmnAct_RightPunch, CmnAct_DustAttack,
    CmnAct_SitRightKick, CmnAct_SuperHSlash, CmnAct_Ukemi, CmnAct_ForwardDown, CmnAct_VertDown,
    CmnAct_ForwardGetUp, CmnAct_Faint, CmnAct_RightKick, CmnAct_ForwardDownLand,
    CmnAct_VertDownDrop, CmnAct_VertDownLand, 0, CmnAct_DownBound, CmnAct_ForwardDownBound,
    CmnAct_VertDownBound, 0, SLAct_LongHair1, SLAct_LongHair2, SLAct_LongHair3, SLAct_RSWall,
    SLAct_RSAttack, 0, SLAct_Throw, CmnAct_ThrowDamageBySL, SLAct_WinB, SLAct_EntryC_1,
    SLAct_EntryC_2, SLAct_HW4_VR_1, SLAct_HW4_VR_2, SLAct_HW4_VR_Land, SLAct_RSJump,
    SLAct_SHW1_DI, SLAct_SHW2_TR, CmnAct_ThrowDamageByPO, SLAct_HW1_GF, SLAct_DA1, SLAct_HW2_VV,
    SLAct_HW2_VV_Jump, SLAct_HW2_VV_Drop, SLAct_HW2_VV_Land, SLAct_HW2_VV2, SLAct_HW2_VV2_Jump,
    SLAct_HW3_GV, SLAct_HW3_GV_Jump, SLAct_HW3_GV_Drop, SLAct_HW3_GV_Land, SLAct_HW2_SVV,
    SLAct_HW2_SVV_Jump, SLAct_HW2_SVV2, SLAct_HW2_SVV2_Jump, CmnAct_ThrowDamageByVE,
    CmnAct_ThrowDamageByJM, CmnAct_ThrowDamageByML, SLAct_HW21_VVK, SLAct_HW22_DVV,
    SLAct_EntryA_1, SLAct_EntryA_2, SLAct_EntryB_1, SLAct_EntryB_2, SLAct_OVERKILLMODE,
    SLAct_ITIGEKI_L2, SLAct_ITIGEKI_L3, SLAct_ITIGEKI, SLAct_ITIGEKI_Jump, SLAct_HW2_VS,
    SLAct_HW2_VSJump, SLAct_RSDrop, SLAct_RSLand, SLAct_HW4_VRA_1, SLAct_HW4_VRA_2,
    SLAct_HW4_VRA_Land, SLAct_ThrowB, SLAct_ThrowB_Miss, CmnAct_ThrowDamageBySLEX,
    CmnAct_ThrowDamageByZT, CmnAct_ThrowDamageByCH, CmnAct_ThrowDamageByKY,
    CmnAct_ThrowDamageByJY, CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageByMYEX,
    CmnAct_ThrowDamageByPOEX, CmnAct_ThrowDamageByPOEXJump, CmnAct_ThrowDamageByPOEXLand,
    CmnAct_ThrowDamageByZTEX, CmnAct_ThrowDamageByDZ, CmnAct_ThrowDamageByCHEX,
    CmnAct_ThrowDamageByFA, CmnAct_ThrowDamageByFA_NAME, CmnAct_SHWDamageByFA1,
    CmnAct_SHWDamageByFA2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    CmnAct_ThrowDamageByPOEX2Jump, CmnAct_ThrowDamageByPOEX2CeilingLand,
    CmnAct_ThrowDamageByPOEX2Drop, CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK,
    SLAct_SHW1_DIEnd, CmnAct_ThrowDamageByAX, SLAct_HW3_GV2, CmnAct_DieByFA, 0,
    CmnAct_ThrowDamageByTS, CmnAct_ThrowDamageByAN, CmnAct_DmpByFA, CmnAct_AXTenhousekiDamege,
    CmnAct_DieByJY, CmnAct_CutByJY, SLAct_HW4_VRH_1, SLAct_HW4_VRH_2, SLAct_HW4_VRH_Land,
    CmnAct_ThrowDamageByMYIT, CmnAct_ThrowDamageByMYITJump, CmnAct_DmgByANHW3,
    CmnAct_ITIGEKIDamageByVE, SLAct_EntryD_1, SLAct_EntryD_2, SLAct_WinA, SLAct_WinC,
    CmnAct_Casket, CmnAct_ItigekiDamegeByBK, CmnAct_DieByTS, SLAct_Lose, CmnAct_Die2ByTS,
    CmnAct_ThrowDamageByJS, CmnAct_JSVADamege, CmnAct_ThrowDamageByKR, CmnAct_ThrowDamageByKYEX,
    0,
};

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTatsumakix2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int SL_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x4080060);
            return 1;
        }
        if (IsTatsumakix2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x408005F);
            return 1;
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x2080064);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                if (offset->ply->PersonalWork[0].w[0]) {
                    SetAttackActNo(offset, 0x2080080);
                    return 1;
                }
                SetAttackActNo(offset, 0x2080068);
                return 1;
            }
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x2080062);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x208005B);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x208005E);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x208006A);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x208006E);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x2080070);
                return 1;
            }
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x2080084);
                return 1;
            }
        }
    }
    return 0;
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int SL_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x63);
}

void SL_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x2080050);
    } else {
        SetAttackActNo(offset, 0x2080051);
    }
}

void SL_RespectCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x2080052);
}

int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);

int SL_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsSyoryu(offset) && IsTrgAndOffKick(offset)) {
            return 1;
        }
    }
    return 0;
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);

int SL_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x7E, 0x7B)) {
            return 1;
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x2080088);
                return 1;
            }
        }
    }
    return 0;
}

int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);

int SLHW2ExCommandCheck(CHARACTER_WORK *offset) {
    if (offset->Localid != 0) {
        return 0;
    }
    if (IsTatsumaki(offset) && IsTrgAndOffKick(offset)) {
        offset->Localid = 1;
        return 1;
    }
    return 0;
}

void SLAct_HW2_VV(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0x64)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x30065);
}

void SLAct_HW2_VV_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = SLHW2ExCommandCheck;
    if (!SetChangeFrameAnime(offset, 0x65)) {
        YSpeedtoAction(offset, 0x30066, -500);
    }
    SetJumpControl(offset);
}

void SLAct_HW2_VV_Drop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = SLHW2ExCommandCheck;
    SetChangeFrameAnime(offset, 0x66);
    if (offset->Mark) {
        if (offset->Localid == 1) {
            SetAttackActNo(offset, 0x20B0075);
        } else if (offset->Localid == 2) {
            SetActNoNext(offset, 0x30076);
        }
    }
    SetLandBreakActno(offset, 0x30067);
}

void SLAct_HW2_VV_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    EndtoSit(offset);
}

void SLAct_HW2_VV2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0x68)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x30069);
}

void SLAct_HW2_VV2_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = SLHW2ExCommandCheck;
    if (!SetChangeFrameAnime(offset, 0x69)) {
        YSpeedtoAction(offset, 0x30066, -700);
    }
    SetJumpControl(offset);
}

void SLAct_HW2_VS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0x80)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x30081);
}

void SLAct_HW2_VSJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = SLHW2ExCommandCheck;
    if (!SetChangeFrameAnime(offset, 0x81)) {
        YSpeedtoAction(offset, 0x30066, -400);
    }
    SetJumpControl(offset);
}

void SLAct_HW2_SVV(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0x6E)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x3006F);
}

void SLAct_HW2_SVV_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = SLHW2ExCommandCheck;
    if (!SetChangeFrameAnime(offset, 0x6F)) {
        YSpeedtoAction(offset, 0x30066, -700);
    }
    SetJumpControl(offset);
}

void SLAct_HW2_SVV2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->ply->ExCommandFunc = SLHW2ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x70)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x30071);
}

void SLAct_HW2_SVV2_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (!SetChangeFrameAnime(offset, 0x71)) {
        YSpeedtoAction(offset, 0x30066, -700);
    }
    SetJumpControl(offset);
}

void SLAct_HW21_VVK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x75)) {
        if (offset->yspeed > -400) {
            offset->yspeed = -400;
        } else {
            offset->yspeed = offset->yspeed / 2;
        }
    }
    SetLandBreakActno(offset, 0x30067);
}

void SLAct_HW22_DVV(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x76)) {
        offset->Localid = 0;
    }
    if (offset->Mark) {
        YSpeedtoAction(offset, 0x30066, -700);
    }
    SetJumpControl(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
extern PAD GameKey[2];

void SLAct_HW3_GV(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *ply;

    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0x6A) != 0) {
        offset->ply->PersonalWork[0].w[1] = 0;
    }
    if (offset->HitStopTime != 0) {
        if (GameKey[offset->padid].trg & 0xA000) {
            offset->ply->PersonalWork[0].w[1] += 1;
        }
        ply = offset->ply;
        if (GameKey[offset->padid].trg & (ply->PunchKey | ply->KickKey | ply->SlashKey | ply->HighSlashKey)) {
            ply->PersonalWork[0].w[1] += 1;
        }
    } else {
        if (GameKey[offset->padid].trg & 0xA000) {
            offset->ply->PersonalWork[0].w[1] += 6;
        }
        ply = offset->ply;
        if (GameKey[offset->padid].trg & (ply->PunchKey | ply->KickKey | ply->SlashKey | ply->HighSlashKey)) {
            ply->PersonalWork[0].w[1] += 6;
        }
    }
    if (offset->ply->PersonalWork[0].w[1] > 120) {
        offset->ply->PersonalWork[0].w[1] = 120;
    }
    TimetoAction(offset, 0x300B1, offset->ply->PersonalWork[0].w[1] / 6 + 20);
}

void SLAct_HW3_GV2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0xB1);
    EndtoAction(offset, 0x3006B);
}

void SLAct_HW3_GV_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (!SetChangeFrameAnime(offset, 0x6B)) {
        YSpeedtoAction(offset, 0x3006C, -500);
    }
    SetJumpControl(offset);
}

void SLAct_HW3_GV_Drop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6C);
    SetLandBreakActno(offset, 0x3006D);
}

void SLAct_HW3_GV_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6D);
    EndtoSit(offset);
}

void SLAct_HW4_VR_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->JumpFlag &= ~2;
    SetChangeFrameAnime(offset, 0x5B);
    if ((g_info.mode & 0x100) && tr_info->tene.memflg) {
        if (!(pads[offset->padid].old & offset->ply->KickKey) && !(pads[offset->padid].data & offset->ply->KickKey)) {
            offset->Mark = 1;
        }
    } else {
        if (!IsHoldKick(offset)) {
            offset->Mark = 1;
        }
    }
    if (offset->Mark) {
        EndtoAction(offset, 0x5C);
    } else {
        EndtoAction(offset, 0xBB);
    }
}

void SLAct_HW4_VR_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    SetLandBreakActno(offset, 0x2005D);
}

void SLAct_HW4_VR_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    EndtoSit(offset);
}

void SLAct_HW4_VRH_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->JumpFlag &= ~2;
    SetChangeFrameAnime(offset, 0xBA);
    EndtoAction(offset, 0xBB);
}

void SLAct_HW4_VRH_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBB);
    SetLandBreakActno(offset, 0x200BC);
}

void SLAct_HW4_VRH_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBC);
    EndtoSit(offset);
}

void SLAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x400) | 0x40;
    SetChangeFrameAnime(offset, 0x56);
    EndtoStand(offset);
}

void SLAct_ThrowB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x400) | 0x40;
    SetChangeFrameAnime(offset, 0x87);
    EndtoStand(offset);
}

void SLAct_ThrowB_Miss(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x400) | 0x40;
    SetChangeFrameAnime(offset, 0x88);
    EndtoStand(offset);
}

void SLAct_RSJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->ply->JumpFlag &= ~2;
    OrScreenFlag(0x40);
    SetChangeFrameAnime(offset, 0x5E);
    SetLandBreakActno(offset, 0x15);
    if ((offset->actst & 0x2800) && offset->posy < -4000) {
        SetActNoNext(offset, 0x30053);
    }
}

void SLAct_RSWall(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    OrScreenFlag(0x40);
    SetXSpeed(offset, 0);
    SetChangeFrameAnime(offset, 0x54);
    EndtoAction(offset, 0x54);
}

void SLAct_RSAttack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (offset->posy < -1999) {
        offset->yspeed = offset->posy / -20;
    } else {
        offset->yspeed = offset->posy / -40;
    }
    if (offset->yspeed < 50) {
        offset->yspeed = 50;
    }
    if (SetChangeFrameAnime(offset, 0x55)) {
        offset->yspeed = 0;
        offset->gravity = 0;
        offset->ply->EnemyTouch = 0;
    }
    SetLandBreakActno(offset, 0x15);
    if ((offset->attackst & 0x1000) || ((offset->actst & 0x2800) && offset->ActTimer >= 6) || offset->ply->EnemyTouch > 10) {
        SetXSpeed(offset, 300);
        offset->gravity = Jump_GravityByIdno[offset->padid];
        SetActNoNext(offset, 0x20082);
    }
}

void SLAct_RSDrop(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x82);
    LandtoAction(offset, 0x83);
}

void SLAct_RSLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x83);
    EndtoStand(offset);
}

void SLAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void SLAct_LongHair2(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x51)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void SLAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

void SLAct_SHW1_DI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x5F);
    offset->ply->PersonalWork[0].w[0] = 360;
    EndtoStand(offset);
}

void SLAct_SHW1_DIEnd(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0xAF);
    EndtoStand(offset);
}

void SLAct_SHW2_TR(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x60);
    EndtoStand(offset);
}

void SLAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xC3);
    EndtoDemo(offset);
}

void SLAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x58);
    EndtoDemo(offset);
}

void SLAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xC4);
    EndtoDemo(offset);
}

void SLAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

void SLAct_HW1_GF(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x62);
    EndtoStand(offset);
}

void SLAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x63)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

void SLAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x77);
}

void SLAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x78);
    EndtoStand(offset);
}

void SLAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x79);
}

void SLAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7A);
    EndtoStand(offset);
}

void SLAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
}

void SLAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    EndtoStand(offset);
}

void SLAct_EntryD_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC1);
}

void SLAct_EntryD_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC2);
    EndtoStand(offset);
}

void SLAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x7B);
    EndtoStand(offset);
}

void SLAct_ITIGEKI_L2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x7C)) {
        Tension_Dec(offset, -1);
    }
    EndtoAction(offset, 0x7E);
}

void SLAct_ITIGEKI_L3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x7D)) {
        Tension_Dec(offset, -1);
    }
    EndtoAction(offset, 0x7E);
}

void SLAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x7E)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoAction(offset, 0x7F);
}

void SLAct_ITIGEKI_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    SetChangeFrameAnime(offset, 0x7F);
    SetLandBreakActno(offset, 0x15);
}

void SLAct_HW4_VRA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->JumpFlag &= ~2;
    SetChangeFrameAnime(offset, 0x84);
    EndtoAction(offset, 0x20085);
}

void SLAct_HW4_VRA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x85);
    SetLandBreakActno(offset, 0x20086);
}

void SLAct_HW4_VRA_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x86);
    offset->actst |= 0x400;
    EndtoSit(offset);
}

int IsDragonInstall(CHARACTER_WORK *offset) {
    if (offset->idno == 1 && offset->ply->PersonalWork[0].w[0] > 0) {
        return 1;
    }
    return 0;
}
