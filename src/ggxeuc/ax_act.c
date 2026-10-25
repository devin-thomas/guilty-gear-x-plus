#include "generated/ax_act.h"

static int axkillchainid;

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*AXAct_jumpTb[])() = {
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
    CmnAct_SitRightKick, AXAct_SuperHSlash_1, CmnAct_Ukemi, CmnAct_ForwardDown, CmnAct_VertDown,
    CmnAct_ForwardGetUp, CmnAct_Faint, CmnAct_RightKick, CmnAct_ForwardDownLand,
    CmnAct_VertDownDrop, CmnAct_VertDownLand, 0, CmnAct_DownBound, CmnAct_ForwardDownBound,
    CmnAct_VertDownBound, 0, AXAct_LongHair1, AXAct_LongHair2, AXAct_WinA, AXAct_WinB,
    AXAct_SuperHSlash_2, AXAct_SuperHSlash_3, AXAct_AXB_1, AXAct_AXB_Land,
    AXAct_Raieisageki_H_Bound, AXAct_Raieisageki_H_Land, 0, 0, CmnAct_ThrowDamageBySL,
    CmnAct_ThrowDamageByPO, CmnAct_ThrowDamageByVE, CmnAct_ThrowDamageByJM,
    CmnAct_ThrowDamageByML, CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageBySLEX,
    CmnAct_ThrowDamageByKY, CmnAct_ThrowDamageByMYEX, CmnAct_ThrowDamageByCH,
    CmnAct_ThrowDamageByZT, CmnAct_ThrowDamageByZTEX, CmnAct_ThrowDamageByJY,
    CmnAct_ThrowDamageByPOEX, CmnAct_ThrowDamageByPOEXJump, CmnAct_ThrowDamageByPOEXLand,
    CmnAct_SHWDamageByFA1, CmnAct_SHWDamageByFA2, CmnAct_ThrowDamageByCHEX,
    CmnAct_ThrowDamageByFA, CmnAct_ThrowDamageByFA_NAME, CmnAct_ThrowDamageByDZ, AXAct_DA1,
    CmnAct_ThrowDamageByPOEX2Jump, CmnAct_ThrowDamageByPOEX2CeilingLand,
    CmnAct_ThrowDamageByPOEX2Drop, CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK,
    AXAct_Throw, CmnAct_ThrowDamageByAX, CmnAct_DieByFA, 0, CmnAct_ThrowDamageByTS,
    CmnAct_ThrowDamageByAN, AXAct_OVERKILLMODE, AXAct_ITIGEKI_L2, AXAct_ITIGEKIHIT,
    AXAct_ITIGEKI, AXAct_Benten_L, AXAct_Benten_H, AXAct_Benten_H_End, AXAct_Benten_AXB,
    AXAct_Benten_AXB_End, CmnAct_DmpByFA, AXAct_Rensen, AXAct_Rensen_End, AXAct_Rensen_Type1,
    AXAct_Rensen_Type2, AXAct_Rasyousen, AXAct_RasyousenBack, AXAct_Tenhouseki,
    AXAct_Tenhouseki_Act, 0, AXAct_Raieisageki_L, AXAct_Raieisageki_L_Sky,
    AXAct_Raieisageki_L_Land, AXAct_Raieisageki_H, AXAct_Raieisageki_H_Sky,
    AXAct_Raieisageki_H_Drop, AXAct_Rasyousen_SD, AXAct_Rasyousen_JumpReady,
    AXAct_Rasyousen_Jump, CmnAct_AXTenhousekiDamege, CmnAct_DieByJY, CmnAct_CutByJY, AXAct_SHW1,
    CmnAct_ThrowDamageByMYIT, CmnAct_ThrowDamageByMYITJump, CmnAct_DmgByANHW3,
    CmnAct_ITIGEKIDamageByVE, AXAct_LongHair3, AXAct_WinC, AXAct_EntryA_1, AXAct_EntryA_2,
    AXAct_EntryB_1, AXAct_EntryB_2, AXAct_EntryC_1, AXAct_EntryC_2, CmnAct_Casket,
    CmnAct_ItigekiDamegeByBK, AXAct_Rasyousen_JumpHit, AXAct_Rasyousen_Drop,
    AXAct_Rasyousen_Land, CmnAct_DieByTS, AXAct_Lose, CmnAct_Die2ByTS, CmnAct_ThrowDamageByJS,
    CmnAct_JSVADamege, CmnAct_ThrowDamageByKR, CmnAct_ThrowDamageByKYEX, AXAct_SHW2,
    AXAct_SHW2_Jump, AXAct_SHW2_Land, 0,
};

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
short int GetTensionVal(CHARACTER_WORK *offset);
int IsGyakuYoga(CHARACTER_WORK *offset);
int IsKaen(CHARACTER_WORK *offset);
int IsLeftTAMERight(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int AX_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (GetTensionVal(offset) >= 5000 && IsKaen(offset) && IsTrgAndOffHighSlash(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x040800B6);
                return 1;
            } else {
                SetAttackActNo(offset, 0x0408009D);
                return 1;
            }
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080082);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080083);
                return 1;
            }
        }
        if (IsLeftTAMERight(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080088);
                return 1;
            }
            if (!(GetEnemyPlayer(offset)->actst & 0x8100) && IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x0208008C);
                return 1;
            }
        }
        if (IsTatsumaki(offset) && IsTrgAndOffPunch(offset)) {
            SetAttackActNo(offset, 0x0208008E);
            return 1;
        }
        if (IsGyakuYoga(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080091);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080094);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (offset->posy < -2000 && IsSyoryu(offset)) {
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080056);
                return 1;
            }
        }
    }
    return 0;
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void AX_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x02080050);
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void AX_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080051);
    } else {
        SetAttackActNo(offset, 0x020800A2);
    }
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int AX_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x72);
}

int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);

int AX_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x81, 0x7E)) {
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void AXAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x72)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void AXAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void AXAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void AXAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0xA2);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void AXAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void AXAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void AXAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xA3);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);

void AXAct_SuperHSlash_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetNormalBegin(offset);
    SetChangeFrameAnime(offset, 0x41);
    EndtoAction(offset, 0x54);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_SuperHSlash_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x54);
    SetLandBreakActno(offset, 0x55);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_SuperHSlash_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA4);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA5);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA6);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA7);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA8);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA9);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & 0xFFFFFBFF) | 0x40;
    SetChangeFrameAnime(offset, 0x78);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x7E);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_ITIGEKI_L2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x7F);
    EndtoAction(offset, 0x81);
}

int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_ITIGEKIHIT(CHARACTER_WORK *offset) {
    OrScreenFlag(4);
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x80);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void AXAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x81)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        SetAttackActNo(offset, 0x80);
    }
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_AXB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56);
    SetLandBreakActno(offset, 0x57);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_AXB_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Benten_L(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x82);
    EndtoStand(offset);
}

int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int AXBenAXBCommandCheck(CHARACTER_WORK *offset) {
    if ((offset->Mark & 1) && (IsSyoryu(offset))) {
        if (IsTrgAndOffHighSlash(offset)) {
            offset->Localid = 1;
        }
    }
    return 0;
}

int AXBenAXBCommandCheck(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Benten_H(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x83)) {
        offset->Localid = 0;
    }
    offset->ply->ExCommandFunc = AXBenAXBCommandCheck;
    if ((offset->Mark & 2) && offset->Localid) {
        SetActNoNext(offset, 0x10085);
        return;
    }
    EndtoAction(offset, 0x10084);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Benten_H_End(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x84);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_Benten_AXB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x85);
    SetLandBreakActno(offset, 0x86);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Benten_AXB_End(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x86);
    EndtoStand(offset);
}

int IsActEnd(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern PAD GameKey[2];

void AXAct_Rensen(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x88);
    if (IsActEnd(offset)) {
        if (~GameKey[offset->padid].data & 0x8000) {
            if (GameKey[offset->padid].data & 0x1000) {
                SetActNoNext(offset, 0x8008A);
            }
            if (GameKey[offset->padid].data & 0x4000) {
                SetActNoNext(offset, 0x8008B);
            }
        }
        SetActNoNext(offset, 0x89);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rensen_End(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x89);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rensen_Type1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8A);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rensen_Type2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8B);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rasyousen(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8C);
    if (offset->attackst & 0x80000) {
        EndtoAction(offset, 0x97);
        return;
    }
    EndtoAction(offset, 0x8D);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_RasyousenBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8D);
    EndtoStand(offset);
}

void SetAXChainKill_id(int id) {
    axkillchainid = id;
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);

void AXChainKill_Proc(CHARACTER_WORK *offset) {
    if (offset->Localid <= axkillchainid) {
        if (offset->Localid == axkillchainid) {
            offset->Localid = 0;
            offset->parentWork = GetParentPlayer(offset);
        } else {
            offset->actst |= 0x80000000;
        }
    }
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void PlayerThrowSetSub(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int AX_InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);
    short unsigned int ActNoTbl[19] = {
        0x0, 0xB7, 0x9B, 0x9F, 0x96, 0x9A, 0x9F, 0xAE, 0x93, 0x91,
        0xB1, 0x82, 0xAA, 0x8B, 0x9A, 0x9A, 0x88, 0x8D, 0x8D
    };

    if (aoffset->ply == 0) {
        offset->GuardSt &= ~3;
        return 0;
    }
    SetActNoNext(offset, 0x8F);
    SetActNoNext(eoffset, ActNoTbl[eoffset->idno]);
    offset->HitStopTime = 60;
    offset->Mark &= ~1;
    eoffset->HitStopTime = 60;
    PlayerThrowSetSub(offset);
    return 1;
}

int AX_InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Tenhouseki(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8E);
    if (offset->Mark) {
        offset->ply->GuardFunc = AX_InterruptProc;
        offset->GuardSt = 0xA01;
    } else {
        offset->GuardSt = 0;
        offset->ply->GuardFunc = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Tenhouseki_Act(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x4000;
    SetChangeFrameAnime(offset, 0x8F);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Raieisageki_L(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x91);
    EndtoAction(offset, 0x92);
}

void AddXSpeed(CHARACTER_WORK *offset, short int xspeed);
int IsLeft(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_Raieisageki_L_Sky(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x92);
    if (offset->Mark) {
        if (IsRight(offset)) {
            AddXSpeed(offset, (short) offset->Mark);
        }
        if (IsLeft(offset)) {
            AddXSpeed(offset, -(short) offset->Mark);
        }
    }
    SetLandBreakActno(offset, 0x93);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Raieisageki_L_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x93);
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Raieisageki_H(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x94);
    EndtoAction(offset, 0x95);
}

void AddXSpeed(CHARACTER_WORK *offset, short int xspeed);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsLeft(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void AXAct_Raieisageki_H_Sky(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x95);
    if (offset->padid) {
        OrScreenFlag(0x10000);
    } else {
        OrScreenFlag(0x20000);
    }
    if (offset->Mark == 0) {
        if (offset->posy < -120000) {
            SetYSpeed(offset, 0, 0);
            offset->posy = -120000;
        }
    } else {
        SetYSpeed(offset, 0, 0);
        if (IsRight(offset)) {
            AddXSpeed(offset, offset->Mark);
        }
        if (IsLeft(offset)) {
            AddXSpeed(offset, -offset->Mark);
        }
    }
    EndtoAction(offset, 0x96);
}

void AddXSpeed(CHARACTER_WORK *offset, short int xspeed);
int IsLeft(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_Raieisageki_H_Drop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x96);
    if (offset->padid) {
        OrScreenFlag(0x10000);
    } else {
        OrScreenFlag(0x20000);
    }
    if (IsRight(offset)) {
        AddXSpeed(offset, (short) offset->Mark);
    }
    if (IsLeft(offset)) {
        AddXSpeed(offset, -(short) offset->Mark);
    }
    SetLandBreakActno(offset, 0x58);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
void ReqCmnSE_DownLand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetGroundShake(int lv);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_Raieisageki_H_Bound(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x58)) {
        EffectCreate(offset, 0x6C, 0);
        SetGroundShake(2);
        ReqCmnSE_DownLand(offset);
    }
    SetLandBreakActno(offset, 0x59);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoStand(CHARACTER_WORK *offset);
void ReqCmnSE_DownLand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetGroundShake(int lv);

void AXAct_Raieisageki_H_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x59)) {
        EffectCreate(offset, 0x6C, 0);
        SetGroundShake(1);
        ReqCmnSE_DownLand(offset);
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rasyousen_SD(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s0;
    int temp_a0;
    int temp_v0;
    int temp_v1;

    temp_s0 = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x4004);
    SetChangeFrameAnime(offset, 0x97);
    temp_a0 = temp_s0->posx;
    temp_v1 = offset->posx;
    temp_v0 = temp_v1 - temp_a0;
    if (temp_v0 >= 0) {
        if (temp_v0 >= 0xBB8) {

        } else {
            goto block_4;
        }
    } else if ((temp_a0 - temp_v1) < 0xBB8) {
block_4:
        SetActNoNext(offset, 0x98);
    }
    EndtoAction(offset, 0x98);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rasyousen_JumpReady(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s1;

    temp_s1 = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x4004);
    if (SetChangeFrameAnime(offset, 0x98)) {
        offset->posx = temp_s1->posx;
    }
    EndtoAction(offset, 0x99);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void AXAct_Rasyousen_Jump(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s0;

    temp_s0 = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x4004);
    SetChangeFrameAnime(offset, 0x99);
    SetJumpControl(offset);
    EndtoAction(offset, 0xAD);
    if ((offset->posy + offset->yspeed) < temp_s0->posy) {
        SetActNoNext(offset, 0xAC);
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void AXAct_Rasyousen_JumpHit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x4004);
    if (SetChangeFrameAnime(offset, 0xAC)) {
        EffectCreate_ActNo(offset, 0x4D, 0, 6);
    }
    if (offset->posy < -70000) {
        SetActNoNext(offset, 0xAD);
    }
    SetJumpControl(offset);
    EndtoAction(offset, 0xAD);
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_Rasyousen_Drop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x4004);
    SetChangeFrameAnime(offset, 0xAD);
    SetLandBreakActno(offset, 0xAE);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_Rasyousen_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAE);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_SHW1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9D);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_SHW2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB6);
    EndtoAction(offset, 0x040800B7);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void AXAct_SHW2_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB7);
    SetJumpControl(offset);
    SetLandBreakActno(offset, 0x57);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void AXAct_SHW2_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB8);
    EndtoStand(offset);
}
