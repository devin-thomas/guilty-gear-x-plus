#include "generated/ky_act.h"

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*KYAct_jumpTb[])() = {
    CmnAct_Stand, CmnAct_Sit, CmnAct_Stand2Sit, CmnAct_Sit2Stand, CmnAct_JumpReady,
    CmnAct_ForwardWalk, CmnAct_BackWalk, CmnAct_Jump_Vert, CmnAct_Drop_Vert,
    CmnAct_Jump_Forward, CmnAct_Jump_Back, CmnAct_Jump_Homing, CmnAct_JumpDash_Forward,
    CmnAct_JumpDash_Back, CmnAct_Dash, CmnAct_Reduce, CmnAct_BackStep, CmnAct_StandTurnLeft,
    CmnAct_StandTurnRight, CmnAct_SitTurnLeft, CmnAct_SitTurnRight, CmnAct_KYJumpLand, 0,
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
    CmnAct_ForwardGetUp, CmnAct_Faint, KYAct_RightKick, CmnAct_ForwardDownLand,
    CmnAct_VertDownDrop, CmnAct_VertDownLand, CmnAct_Drop_Back, CmnAct_DownBound,
    CmnAct_ForwardDownBound, CmnAct_VertDownBound, 0, KYAct_LongHair1, KYAct_LongHair2,
    KYAct_WinA, KYAct_WinB, KYAct_WinC, KYAct_HW2_SKYSE, KYAct_EntryA_1, KYAct_EntryA_2,
    KYAct_EntryB_1, KYAct_EntryB_2, KYAct_HW1_StunEdge, KYAct_HW4_SD_1, KYAct_HW4_SD_2,
    KYAct_OVERKILLMODE, KYAct_HW2_SKYSE_HS, 0, KYAct_ITIGEKI, KYAct_HW7_GT, KYAct_HW3_VS,
    KYAct_HW3_VS_Jump, KYAct_HW3_VS_Drop, KYAct_HW3_VS_Land, KYAct_SHW1_RL, KYAct_HW5_CS,
    KYAct_HW5_CS_Jump, KYAct_HW5_CS_Land, KYAct_EntryC_1, KYAct_EntryC_2, KYAct_SHW2_LS,
    KYAct_SHW2_LSJump, KYAct_SHW2_LSLand, KYAct_HW1_StunEdgeLv2, CmnAct_ThrowDamageBySL,
    CmnAct_ThrowDamageByPO, CmnAct_ThrowDamageByVE, CmnAct_ThrowDamageByJM,
    CmnAct_ThrowDamageByML, CmnAct_ThrowDamageBySLEX, KYAct_Throw, KYAct_Throw2, KYAct_Throw3,
    CmnAct_ThrowDamageByZT, CmnAct_ThrowDamageByCH, CmnAct_ThrowDamageByKY,
    CmnAct_ThrowDamageByJY, CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageByMYEX,
    CmnAct_ThrowDamageByPOEX, CmnAct_ThrowDamageByPOEXJump, CmnAct_ThrowDamageByPOEXLand,
    CmnAct_ThrowDamageByZTEX, CmnAct_ThrowDamageByCHEX, CmnAct_ThrowDamageByFA,
    CmnAct_ThrowDamageByFA_NAME, CmnAct_ThrowDamageByDZ, KYAct_HW3_HVS, KYAct_HW3_HVS_Jump,
    KYAct_HW3_SVS, KYAct_HW3_SVS_Jump, CmnAct_SHWDamageByFA1, CmnAct_SHWDamageByFA2, 0,
    KYAct_StiffLand, KYAct_DA1, CmnAct_ThrowDamageByPOEX2Jump,
    CmnAct_ThrowDamageByPOEX2CeilingLand, CmnAct_ThrowDamageByPOEX2Drop,
    CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK, CmnAct_ThrowDamageByAX,
    CmnAct_DieByFA, 0, CmnAct_ThrowDamageByTS, CmnAct_ThrowDamageByAN, CmnAct_DmpByFA,
    CmnAct_AXTenhousekiDamege, CmnAct_DieByJY, CmnAct_CutByJY, CmnAct_ThrowDamageByMYIT,
    CmnAct_ThrowDamageByMYITJump, CmnAct_DmgByANHW3, CmnAct_ITIGEKIDamageByVE, KYAct_LongHair3,
    KYAct_LongHair4, CmnAct_Casket, CmnAct_ItigekiDamegeByBK, CmnAct_DieByTS, KYAct_Lose,
    CmnAct_Die2ByTS, KYAct_HW1_StunEdgeLv2Ex, CmnAct_ThrowDamageByJS, CmnAct_JSVADamege,
    CmnAct_ThrowDamageByKR, CmnAct_ThrowDamageByKYEX, KYAct_ThrowB, KYAct_ThrowB_Miss,
    KYAct_HW3_Add, KYAct_HW5_CSX, KYAct_HW5_CSX_Jump, KYAct_HW5_CSX_Land, KYAct_HW4_SDX_S,
    KYAct_HW4_SDX_S_Hit, KYAct_HW4_SDX_S_Jump, 0,
};

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int KY_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x04080066);
            return 1;
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080062);
                return 1;
            }
            if (!(userinfo.gguse[offset->padid] & 2)) {
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02080087);
                    return 1;
                }
            } else if (GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080087);
                return 1;
            }
        }
        if (IsHadou(offset)) {
            if (!(offset->ply->ObjFlag & 1)) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x0208005A);
                    return 1;
                }
                if (!(userinfo.gguse[offset->padid] & 2)) {
                    if (IsTrgAndOffHighSlash(offset)) {
                        SetAttackActNo(offset, 0x0208006F);
                        return 1;
                    }
                } else if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x020800A9);
                    return 1;
                }
            }
            if (IsTrgAndOffKick(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    SetAttackActNo(offset, 0x020800B4);
                    return 1;
                } else {
                    SetAttackActNo(offset, 0x0208005B);
                    return 1;
                }
            }
        }
        if (IsTatsumaki(offset) && IsTrgAndOffKick(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x020800B1);
                return 1;
            } else {
                SetAttackActNo(offset, 0x02080067);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080089);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080089);
                return 1;
            }
        }
        if (!(offset->ply->ObjFlag & 1) && offset->posy < -5500) {
            if (IsHadou(offset)) {
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x0208005E);
                    return 1;
                }
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080055);
                    return 1;
                }
            }
            if (IsTatsumaki(offset) && (userinfo.gguse[offset->padid] & 4) && IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x020800B2);
                return 1;
            }
        }
    }
    return 0;
}

int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
extern USER_SETTING userinfo;

int KY_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (userinfo.gguse[offset->padid] & 4) {
        if (!(offset->actst & 0x10) && IsSyoryu(offset) && IsTrgAndOffPunch(offset)) {
            return 2;
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void KY_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
    } else {
        SetAttackActNo(offset, 0x020800A2);
    }
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void KY_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080051);
    } else {
        SetAttackActNo(offset, 0x020800A3);
    }
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int KY_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x8F);
}

int IsHadoux2(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int KY_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x60, 0x5D)) {
            return 1;
        }
        if (userinfo.gguse[offset->padid] & 4) {
            if (IsSyoryu(offset) && IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x020800AF);
                return 1;
            }
        }
    }
    return 0;
}

int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
extern USER_SETTING userinfo;

int KYHW3ExCommandCheck(CHARACTER_WORK *offset) {
    if ((userinfo.gguse[offset->padid] & 4) && offset->Localid == 0 && IsTatsumaki(offset) && IsTrgAndOffKick(offset)) {
        offset->Localid = 1;
        return 1;
    }
    return 0;
}

int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
extern USER_SETTING userinfo;

int KYHW4ExCommandCheck(CHARACTER_WORK *offset) {
    if ((userinfo.gguse[offset->padid] & 4) && offset->Localid == 0 && IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
        offset->Localid = 1;
        return 1;
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void KYAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void KYAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void KYAct_LongHair3(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0xA2) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void KYAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0xA3);
    EndtoStand(offset);
}

void CmnAct_StdAtk_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void KYAct_RightKick(CHARACTER_WORK *offset) {
    CmnAct_StdAtk_ToStand(offset, 0x47);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void KYAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void KYAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void KYAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_SHW1_RL(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetAnten(offset, 1);
    SetChangeFrameAnime(offset, 0x66);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_SHW2_LS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x6C);
    EndtoAction(offset, 0x2006D);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_SHW2_LSJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x6D);
    SetLandBreakActno(offset, 0x2006E);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_SHW2_LSLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x6E);
    EndtoStand(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_StiffLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x400;
    SetChangeFrameAnime(offset, 0x8E);
    EndtoSit(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW1_StunEdge(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x5A);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW1_StunEdgeLv2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x6F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW1_StunEdgeLv2Ex(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0xA9);
    EndtoStand(offset);
}

int EndtoDrop(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_HW2_SKYSE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->JumpFlag |= 4;
    SetChangeFrameAnime(offset, 0x55);
    SetLandBreakActno(offset, 0x15);
    EndtoDrop(offset);
}

int EndtoDrop(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_HW2_SKYSE_HS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->JumpFlag |= 4;
    SetChangeFrameAnime(offset, 0x5E);
    SetLandBreakActno(offset, 0x15);
    EndtoDrop(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW3_VS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x62);
    EndtoAction(offset, 0x30063);
}

int KYHW3ExCommandCheck(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
extern USER_SETTING userinfo;

void KYAct_HW3_VS_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = KYHW3ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x63)) {
        offset->Localid = 0;
    } else if (offset->Localid && (userinfo.gguse[offset->padid] & 4)) {
        YSpeedtoAction(offset, 0x020B00B0, -500);
    } else {
        YSpeedtoAction(offset, 0x30064, -500);
    }
    SetJumpControl(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW3_HVS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x87);
    EndtoAction(offset, 0x30088);
}

int KYHW3ExCommandCheck(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
extern USER_SETTING userinfo;

void KYAct_HW3_HVS_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = KYHW3ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x88)) {
        offset->Localid = 0;
    } else if (offset->Localid && (userinfo.gguse[offset->padid] & 4)) {
        YSpeedtoAction(offset, 0x020B00B0, -500);
    } else {
        YSpeedtoAction(offset, 0x30064, -500);
    }
    SetJumpControl(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW3_SVS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x89);
    EndtoAction(offset, 0x3008A);
}

int KYHW3ExCommandCheck(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
extern USER_SETTING userinfo;

void KYAct_HW3_SVS_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = KYHW3ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x8A)) {
        offset->Localid = 0;
    } else if (offset->Localid && (userinfo.gguse[offset->padid] & 4)) {
        YSpeedtoAction(offset, 0x020B00B0, -500);
    } else {
        YSpeedtoAction(offset, 0x30064, -500);
    }
    SetJumpControl(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_HW3_VS_Drop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x64);
    SetLandBreakActno(offset, 0x30065);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW3_VS_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x65);
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void KYAct_HW3_Add(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xB0) != 0) {
        offset->Localid = 0;
    }
    SetJumpControl(offset);
    EndtoAction(offset, 0x30064);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW4_SDX_S_Hit(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    offset->CommandFlag = 0xC05F;
    eoffset = offset->ply->Enemy;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0xB5)) {
        if (eoffset->posx < offset->posx) {
            offset->dirflag = 0;
        } else {
            offset->dirflag = 1;
        }
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x300B6);
}

int KYHW3ExCommandCheck(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
extern USER_SETTING userinfo;

void KYAct_HW4_SDX_S_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = KYHW3ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0xB6)) {
        offset->Localid = 0;
    } else if (offset->Localid && (userinfo.gguse[offset->padid] & 4)) {
        YSpeedtoAction(offset, 0x020B00B0, -500);
    } else {
        YSpeedtoAction(offset, 0x30064, -500);
    }
    SetJumpControl(offset);
}

int IsActEnd(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW4_SD_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x5B);
    if (IsActEnd(offset) != 0) {
        SetActNoNext(offset, 0x2005C);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);

void KYAct_HW4_SD_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (SetChangeFrameAnime(offset, 0x5C) != 0) {
        SetXSpeed(offset, 0);
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW4_SDX_S(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0xB4);
    if (offset->Mark) {
        offset->actst &= ~0x100000;
    } else {
        offset->actst |= 0x100000;
    }
    EndtoAction(offset, 0x200B5);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW5_CS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x67);
    EndtoAction(offset, 0x68);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_HW5_CS_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x68);
    SetLandBreakActno(offset, 0x69);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW5_CS_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x400;
    SetChangeFrameAnime(offset, 0x69);
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW5_CSX(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0xB1);
    EndtoAction(offset, 0xB2);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_HW5_CSX_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB2);
    SetLandBreakActno(offset, 0xB3);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW5_CSX_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x400;
    SetChangeFrameAnime(offset, 0xB3);
    EndtoSit(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_HW7_GT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x61);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6A);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6B);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x5D);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void KYAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x60)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x76);
    EndtoAction(offset, 0x77);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KYAct_Throw2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x77);
    SetLandBreakActno(offset, 0x78);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_Throw3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x78);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_ThrowB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0xAE);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KYAct_ThrowB_Miss(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0xAF);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void KYAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x8F)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}
