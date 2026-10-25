#include "generated/ml_act.h"

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*MLAct_jumpTb[])() = {
    CmnAct_Stand, CmnAct_Sit, CmnAct_Stand2Sit, CmnAct_Sit2Stand, CmnAct_JumpReady,
    CmnAct_ForwardWalk, CmnAct_BackWalk, CmnAct_Jump_Vert, CmnAct_Drop_Vert,
    CmnAct_Jump_Forward, CmnAct_Jump_Back, CmnAct_Jump_Homing, CmnAct_JumpDash_Forward,
    CmnAct_JumpDash_Back, CmnAct_Dash, CmnAct_Reduce, CmnAct_BackStep, CmnAct_StandTurnLeft,
    CmnAct_StandTurnRight, CmnAct_SitTurnLeft, CmnAct_SitTurnRight, CmnAct_JumpLand,
    CmnAct_Drop_Forward, CmnAct_GetUp, CmnAct_StandPunch, CmnAct_StandKick, CmnAct_StandSlash,
    CmnAct_StandSlashNear, CmnAct_StandHighSlash, CmnAct_SitPunch, CmnAct_SitKick,
    CmnAct_SitSlash, CmnAct_SitHighSlash, CmnAct_JumpPunch, CmnAct_JumpKick, CmnAct_JumpSlash,
    CmnAct_JumpHighSlash, CmnAct_StandDamage, CmnAct_StandDamage2, CmnAct_StandDamage3,
    CmnAct_StandLowDamage, CmnAct_StandLowDamage2, CmnAct_StandLowDamage3, CmnAct_SitDamage,
    CmnAct_SitDamage2, CmnAct_SitDamage3, CmnAct_SkyDamage, CmnAct_SkyDamage2, CmnAct_WallBound,
    CmnAct_Down, CmnAct_DownLand, CmnAct_Down2, CmnAct_Kirimomi, CmnAct_StdGuardV,
    CmnAct_StdGuardV2, CmnAct_StdGuardH, CmnAct_StdGuardH2, CmnAct_SitGuard, CmnAct_SitGuard2,
    CmnAct_SkyGuard, CmnAct_SkyGuard2, CmnAct_Stand2, CmnAct_RightPunch, CmnAct_DustAttack,
    CmnAct_SitRightKick, CmnAct_SuperHSlash, CmnAct_Ukemi, CmnAct_ForwardDown, CmnAct_VertDown,
    CmnAct_ForwardGetUp, CmnAct_Faint, CmnAct_RightKick, CmnAct_ForwardDownLand,
    CmnAct_VertDownDrop, CmnAct_VertDownLand, CmnAct_Drop_Back, CmnAct_DownBound,
    CmnAct_ForwardDownBound, CmnAct_VertDownBound, 0, MLAct_LongHair1, MLAct_LongHair2,
    MLAct_LongHair3, MLAct_LongHair4, MLAct_Throw, MLAct_WinA, MLAct_WinB, MLAct_HW3_1,
    MLAct_HW3_2, MLAct_HW1, MLAct_HW4_IS, MLAct_HW6, MLAct_HW5, MLAct_EntryA_1, MLAct_EntryA_2,
    MLAct_HW2, MLAct_EntryB_1, MLAct_EntryB_2, MLAct_HW2_LOOP, MLAct_HW2_END,
    MLAct_SHW1_FXFirst, MLAct_SHW1_FXJump, MLAct_SHW1_FXDrop, MLAct_SHW1_FXLand, MLAct_DA1,
    CmnAct_ThrowDamageBySL, CmnAct_ThrowDamageByPO, CmnAct_ThrowDamageByVE,
    CmnAct_ThrowDamageByJM, CmnAct_ThrowDamageByML, MLAct_OVERKILLMODE, MLAct_KILL_1_LV2,
    MLAct_ITIGEKIHit, MLAct_ITIGEKI, MLAct_ITIGEKIBack, MLAct_HW1S, CmnAct_ThrowDamageBySLEX,
    MLAct_HW4_ISRide, MLAct_HW4_ISDrop, MLAct_HW4_ISLand, CmnAct_ThrowDamageByZT,
    CmnAct_ThrowDamageByCH, CmnAct_ThrowDamageByKY, CmnAct_ThrowDamageByJY,
    CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageByMYEX, CmnAct_ThrowDamageByPOEX,
    CmnAct_ThrowDamageByPOEXJump, CmnAct_ThrowDamageByPOEXLand, CmnAct_ThrowDamageByZTEX,
    MLAct_EntryC_1, MLAct_EntryC_2, MLAct_EntryC_3, CmnAct_ThrowDamageByCHEX,
    CmnAct_ThrowDamageByFA, CmnAct_ThrowDamageByFA_NAME, CmnAct_ThrowDamageByDZ,
    CmnAct_SHWDamageByFA1, CmnAct_SHWDamageByFA2, CmnAct_ThrowDamageByPOEX2Jump,
    CmnAct_ThrowDamageByPOEX2CeilingLand, CmnAct_ThrowDamageByPOEX2Drop,
    CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK, CmnAct_ThrowDamageByAX,
    CmnAct_DieByFA, 0, CmnAct_ThrowDamageByTS, CmnAct_ThrowDamageByAN, CmnAct_DmpByFA,
    CmnAct_AXTenhousekiDamege, MLAct_HW5H, MLAct_HW5Land, CmnAct_DieByJY, CmnAct_CutByJY,
    MLAct_WinC, CmnAct_ThrowDamageByMYIT, CmnAct_ThrowDamageByMYITJump, CmnAct_DmgByANHW3,
    MLAct_HW5PF, MLAct_HW5PFLand, MLAct_HW7BW, CmnAct_ITIGEKIDamageByVE, MLAct_WinD,
    CmnAct_Casket, CmnAct_ItigekiDamegeByBK, MLAct_SHW2_TT, CmnAct_DieByTS, MLAct_Lose,
    CmnAct_Die2ByTS, CmnAct_ThrowDamageByJS, CmnAct_JSVADamege, CmnAct_ThrowDamageByKR,
    CmnAct_ThrowDamageByKYEX, 0,
};

short int GetTensionVal(CHARACTER_WORK *offset);
int IsGeiza(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsRendaSlash(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int ML_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsGeiza(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x04080064);
            return 1;
        }
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x020800A6);
            return 1;
        }
        if (IsHadou(offset) && !(offset->ply->ObjFlag & 1)) {
            if (userinfo.gguse[offset->padid] & 4) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x0209005C);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02090097);
                    return 1;
                }
            } else {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02080073);
                    return 1;
                }
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x0208005A);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x0208005B);
                return 1;
            }
            if (!(userinfo.gguse[offset->padid] & 4) && !(offset->ply->ObjFlag & 1) && IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x020800A1);
                return 1;
            }
        }
        if (IsTrgSlash(offset) && IsRendaSlash(offset)) {
            SetAttackActNo(offset, 0x0208005F);
            return 1;
        }
    }
    if (offset->actst & 0x10) {
        if (IsHadou(offset) && offset->posy < -5500) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x02090057);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x0209009F);
                return 1;
            }
            if ((userinfo.gguse[offset->padid] & 4) && !(offset->ply->ObjFlag & 1)) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x0209005C);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02090097);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void ML_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
    } else {
        SetAttackActNo(offset, 0x02080051);
    }
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void ML_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080052);
    } else {
        SetAttackActNo(offset, 0x02080053);
    }
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int ML_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x68);
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int ML_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x71, 0x6E)) {
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void MLAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void MLAct_LongHair2(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x51)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void MLAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void MLAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void MLAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x55);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void MLAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x56);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void MLAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x9B);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void MLAct_WinD(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xA3);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void MLAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW1S(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x73);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_SHW2_TT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA6);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x5F)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].l = 16;
    } else if (IsTrgSlash(offset)) {
        offset->ply->PersonalWork[0].l += 10;
    }
    EndtoAction(offset, 0x62);
}

int IsTrgSlash(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void MLAct_HW2_LOOP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62);
    if (IsTrgSlash(offset)) {
        offset->ply->PersonalWork[0].l += 5;
    }
    if (offset->ply->PersonalWork[0].l > offset->ActTimer + 30) {
        offset->ply->PersonalWork[0].l = offset->ActTimer + 30;
    }
    if (offset->ply->PersonalWork[0].l > 120) {
        offset->ply->PersonalWork[0].l = 120;
    }
    TimetoAction(offset, 0x63, offset->ply->PersonalWork[0].l);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW2_END(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x63)) {
        offset->Localid = 1;
    }
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MLAct_HW3_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x57)) {
        offset->ColliFlag = 0;
    }
    SetLandBreakActno(offset, 0x58);
    if ((offset->attackst & 0x1000) && (~offset->attackst & 0x80000)) {
        SetActNoNext(offset, 0x58);
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MLAct_HW3_2(CHARACTER_WORK *offset) {
    offset->ColliFlag = 0;
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
    SetLandBreakActno(offset, 0x15);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW4_IS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    EndtoAction(offset, 0x75);
}

int mlstageside(CHARACTER_WORK *offset) {
    if (offset->dirflag == 0 && offset->posx < 20000) {
        return 1;
    }
    if (offset->dirflag == 1 && offset->posx > -20000) {
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int mlstageside(CHARACTER_WORK *offset);

void MLAct_HW4_ISRide(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x75)) {
        offset->ply->EnemyTouch = 0;
    }
    SetJumpControl(offset);
    if ((offset->attackst & 0x1000) || ((offset->actst & 0x2800) && mlstageside(offset) && offset->Mark) || offset->ply->EnemyTouch > 10) {
        SetActNoNext(offset, 0x76);
    }
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW4_ISDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x76);
    SetLandBreakActno(offset, 0x77);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW4_ISLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x77);
    EndtoStand(offset);
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    SetLandBreakActno(offset, 0x98);
}

int IsTrgHighSlash(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int sound_request(int no);
extern PAD GameKey[2];

void MLAct_HW5H(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x97)) {
        offset->ply->PersonalWork[1].ub[0] = 0;
        offset->ply->PersonalWork[1].ub[1] = 0;
        offset->ply->PersonalWork[1].ub[2] = 0;
        offset->ply->PersonalWork[1].ub[3] = 0;
    }
    if (offset->Mark && (IsTrgSlash(offset) || IsTrgHighSlash(offset))) {
        u_char dir;

        if ((GameKey[offset->padid].data & 0x1000) && (GameKey[offset->padid].data & 0x2000)) {
            dir = 5;
        } else if ((GameKey[offset->padid].data & 0x1000) && (GameKey[offset->padid].data & 0x8000)) {
            dir = 6;
        } else if ((GameKey[offset->padid].data & 0x4000) && (GameKey[offset->padid].data & 0x2000)) {
            dir = 7;
        } else if ((GameKey[offset->padid].data & 0x4000) && (GameKey[offset->padid].data & 0x8000)) {
            dir = 8;
        } else if (GameKey[offset->padid].data & 0x1000) {
            dir = 1;
        } else if (GameKey[offset->padid].data & 0x4000) {
            dir = 2;
        } else if (GameKey[offset->padid].data & 0x2000) {
            dir = 3;
        } else if (GameKey[offset->padid].data & 0x8000) {
            dir = 4;
        } else {
            return;
        }
        if (offset->ply->PersonalWork[1].ub[0] == 0) {
            offset->ply->PersonalWork[1].ub[0] = dir;
        } else if (offset->ply->PersonalWork[1].ub[1] == 0) {
            offset->ply->PersonalWork[1].ub[1] = dir;
        } else if (offset->ply->PersonalWork[1].ub[2] == 0) {
            offset->ply->PersonalWork[1].ub[2] = dir;
        } else if (offset->ply->PersonalWork[1].ub[3] == 0) {
            offset->ply->PersonalWork[1].ub[3] = dir;
        } else {
            return;
        }
        sound_request(0x39);
    }
    SetLandBreakActno(offset, 0x98);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW5Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x98);
    EndtoSit(offset);
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW5PF(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9F);
    SetLandBreakActno(offset, 0xA0);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW5PFLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA0);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_HW6(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoStand(offset);
}

int IsTrgHighSlash(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int EndtoStand(CHARACTER_WORK *offset);
int sound_request(int no);
extern PAD GameKey[2];

void MLAct_HW7BW(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xA1)) {
        offset->ply->PersonalWork[1].ub[0] = 0;
        offset->ply->PersonalWork[1].ub[1] = 0;
        offset->ply->PersonalWork[1].ub[2] = 0;
        offset->ply->PersonalWork[1].ub[3] = 0;
    }
    if (offset->Mark && IsTrgHighSlash(offset)) {
        u_char dir;

        if ((GameKey[offset->padid].data & 0x1000) && (GameKey[offset->padid].data & 0x2000)) {
            dir = 5;
        } else if ((GameKey[offset->padid].data & 0x1000) && (GameKey[offset->padid].data & 0x8000)) {
            dir = 6;
        } else if ((GameKey[offset->padid].data & 0x4000) && (GameKey[offset->padid].data & 0x2000)) {
            dir = 7;
        } else if ((GameKey[offset->padid].data & 0x4000) && (GameKey[offset->padid].data & 0x8000)) {
            dir = 8;
        } else if (GameKey[offset->padid].data & 0x1000) {
            dir = 1;
        } else if (GameKey[offset->padid].data & 0x4000) {
            dir = 2;
        } else if (GameKey[offset->padid].data & 0x2000) {
            dir = 3;
        } else if (GameKey[offset->padid].data & 0x8000) {
            dir = 4;
        } else {
            return;
        }
        if (offset->ply->PersonalWork[1].ub[0] == 0) {
            offset->ply->PersonalWork[1].ub[0] = dir;
        } else if (offset->ply->PersonalWork[1].ub[1] == 0) {
            offset->ply->PersonalWork[1].ub[1] = dir;
        } else if (offset->ply->PersonalWork[1].ub[2] == 0) {
            offset->ply->PersonalWork[1].ub[2] = dir;
        } else if (offset->ply->PersonalWork[1].ub[3] == 0) {
            offset->ply->PersonalWork[1].ub[3] = dir;
        } else {
            return;
        }
        sound_request(0x39);
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_SHW1_FXFirst(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x64);
    EndtoAction(offset, 0x65);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int Random(void);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void MLkirakiraPut(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (offset->HitStopTime == 0) {
        arg.flag = 0x200;
        EffectCreate_ActNo(offset, 0x34, &arg, Random() % 3 + 1);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void MLkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void MLAct_SHW1_FXJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x65);
    SetJumpControl(offset);
    EndtoAction(offset, 0x66);
    MLkirakiraPut(offset);
    MLkirakiraPut(offset);
    MLkirakiraPut(offset);
}

void MLkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MLAct_SHW1_FXDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x66);
    SetLandBreakActno(offset, 0x200067);
    MLkirakiraPut(offset);
    MLkirakiraPut(offset);
    MLkirakiraPut(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_SHW1_FXLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x60);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x61);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void MLAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x68)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x54);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6E);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_KILL_1_LV2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_ITIGEKIHit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x70);
    EndtoStand(offset);
}

int IsActTimeOver(CHARACTER_WORK *offset, int time);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void MLAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x71)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (IsActTimeOver(offset, 90) && offset->Mark) {
        if (offset->attackst & 0x80000) {
            offset->ply->MutekiTime = 0xFF;
            SetActNoNext(offset, 0x70);
        } else {
            SetActNoNext(offset, 0x72);
        }
    }
    offset->Mark = 1;
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_ITIGEKIBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x72);
    EndtoStand(offset);
}

int GetXMove(CHARACTER_WORK *offset, int val);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x82)) {
        offset->posx = GetXMove(offset, -8000);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MLAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x83);
    EndtoAction(offset, 0x84);
}

void ReqGuardEffect(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);

int MLAct_EntryC_Damage(CHARACTER_WORK *offset, TACTHEADER *header, int cnt) {
    ReqGuardEffect(offset, 0x2000, 0, 0);
    return 1;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int MLAct_EntryC_Damage(CHARACTER_WORK *offset, TACTHEADER *header, int cnt);

void MLAct_EntryC_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x84)) {
        offset->ply->DamageInterrupt = MLAct_EntryC_Damage;
    }
    SetJumpControl(offset);
    if ((offset->InstSt & 0xC8) && offset->posy == 0) {
        SetActNoNext(offset, 0);
    }
}
