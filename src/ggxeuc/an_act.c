#include "generated/an_act.h"

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*ANAct_jumpTb[])() = {
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
    CmnAct_VertDownBound, 0, ANAct_LongHair1, ANAct_LongHair2, ANAct_Stand3, ANAct_WinA,
    ANAct_WinB, ANAct_EntryB_1, ANAct_EntryB_2, ANAct_EntryA_1, ANAct_EntryA_2, ANAct_SHW1,
    ANAct_HW2, ANAct_HW1, ANAct_HW5, ANAct_HW5b, ANAct_HW4G, ANAct_HW4S, ANAct_HW6G,
    ANAct_LongHair3, ANAct_OVERKILLMODE, ANAct_ITIGEKI_Attack, ANAct_ITIGEKI_Drop,
    ANAct_ITIGEKI, ANAct_HW6_0, CmnAct_ThrowDamageBySL, CmnAct_ThrowDamageByPO,
    CmnAct_ThrowDamageByVE, CmnAct_ThrowDamageByJM, CmnAct_ThrowDamageByML,
    CmnAct_ThrowDamageBySLEX, CmnAct_ThrowDamageByKY, ANAct_ITIGEKI_Land,
    CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageByMYEX, CmnAct_ThrowDamageByZT,
    CmnAct_ThrowDamageByZTEX, CmnAct_ThrowDamageByCH, CmnAct_ThrowDamageByJY,
    CmnAct_ThrowDamageByPOEX, CmnAct_ThrowDamageByPOEXJump, CmnAct_ThrowDamageByPOEXLand,
    CmnAct_SHWDamageByFA1, CmnAct_SHWDamageByFA2, CmnAct_ThrowDamageByCHEX,
    CmnAct_ThrowDamageByFA, CmnAct_ThrowDamageByFA_NAME, CmnAct_ThrowDamageByDZ, ANAct_DA1,
    CmnAct_ThrowDamageByPOEX2Jump, CmnAct_ThrowDamageByPOEX2CeilingLand,
    CmnAct_ThrowDamageByPOEX2Drop, CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK,
    CmnAct_ThrowDamageByAX, CmnAct_DieByFA, 0, ANAct_Throw, CmnAct_ThrowDamageByAN,
    CmnAct_ThrowDamageByTS, CmnAct_DmpByFA, CmnAct_AXTenhousekiDamege, CmnAct_DieByJY,
    CmnAct_CutByJY, 0, ANAct_HW6Land, ANAct_HW6Sky_Shot, ANAct_HW6Sky_Drop, ANAct_HW6Sky_Land,
    ANAct_HW5c_0, ANAct_HW5c_1, ANAct_HW5c_2, ANAct_HW5c_3, ANAct_HW5c_4, ANAct_HW5d_0,
    ANAct_HW5d_1, ANAct_HW5d_2, ANAct_HW5d_3, ANAct_HW5d_4, ANAct_HW3_Jump, ANAct_HW3_Hold,
    ANAct_HW3_Land, CmnAct_DmgByANHW3, ANAct_HW3_Vart, CmnAct_ThrowDamageByMYIT,
    CmnAct_ThrowDamageByMYITJump, CmnAct_ITIGEKIDamageByVE, CmnAct_Casket,
    CmnAct_ItigekiDamegeByBK, ANAct_HW4Land, CmnAct_DieByTS, ANAct_Lose, ANAct_HW3_Sub,
    CmnAct_Die2ByTS, ANAct_HW7, ANAct_HW7_Jump, ANAct_HW7_Land, ANAct_HW8,
    CmnAct_ThrowDamageByJS, CmnAct_JSVADamege, CmnAct_ThrowDamageByKR, CmnAct_ThrowDamageByKYEX,
    ANAct_HW5Xc_0, ANAct_HW5Xc_1, ANAct_HW5Xc_2, ANAct_HW5Xc_3, ANAct_HW5Xc_4, ANAct_HW8X,
    ANAct_HW8Xa_0, ANAct_HW8Xa_1, ANAct_HW8Xb_0, ANAct_HW8Xc_Sub, ANAct_HW8Xc_Jump,
    ANAct_HW8Xc_Hold, ANAct_HW8Xc_Land, ANAct_HW8Xc_Vart, 0,
};

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;

int AN_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x020800B9);
                return 1;
            }
            SetAttackActNo(offset, 0x02080059);
            return 1;
        }
        if (IsSyoryu(offset) && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x020800AA);
            return 1;
        }
        if (IsHadou(offset)) {
            if (g_info.boss[offset->padid] == 1) {
                if (IsTrgAndOffPunch(offset)) {
                    SetAttackActNo(offset, 0x0208005A);
                    return 1;
                }
            } else if (IsTrgAndOffPunch(offset) && !(offset->ply->ObjFlag & 1)) {
                SetAttackActNo(offset, 0x0208005A);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x0208005C);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x020800AF);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    SetAttackActNo(offset, 0x020800B4);
                    return 1;
                }
                SetAttackActNo(offset, 0x020800AC);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x02080066);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        IsHadou(offset);
        if (IsTatsumaki(offset) && IsTrgAndOffPunch(offset)) {
            SetAttackActNo(offset, 0x02080090);
            return 1;
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void AN_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
    } else {
        SetAttackActNo(offset, 0x02080061);
    }
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void AN_RespectCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x02080051);
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;

int ANHW5ExCommandCheck(CHARACTER_WORK *offset) {
    if (IsTrgAndOffSlash(offset) && (offset->attackst & 0x1000)) {
        SetAttackActNo(offset, 0x0208005D);
        return 1;
    }
    if (IsTrgAndOffPunch(offset) && (offset->attackst & 0x1000)) {
        if (userinfo.gguse[offset->padid] & 4) {
            SetAttackActNo(offset, 0x020800AC);
            return 1;
        }
        SetAttackActNo(offset, 0x02080093);
        return 1;
    }
    if (IsTrgAndOffKick(offset) && (offset->attackst & 0x1000)) {
        SetAttackActNo(offset, 0x02080098);
        return 1;
    }
    if (g_info.boss[offset->padid] == 1 && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset) && (offset->attackst & 0x1000)) {
        SetAttackActNo(offset, 0x02080059);
        return 1;
    }
    return 0;
}

int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int ANHW8XExCommandCheck(CHARACTER_WORK *offset) {
    if (IsTrgAndOffPunch(offset) && (offset->attackst & 0x1000)) {
        SetAttackActNo(offset, 0x020800BA);
        return 1;
    }
    if (IsTrgAndOffKick(offset) && (offset->attackst & 0x1000)) {
        SetAttackActNo(offset, 0x020800BD);
        return 1;
    }
    if (IsTrgAndOffSlash(offset) && (offset->attackst & 0x1000)) {
        SetAttackActNo(offset, 0x020800BC);
        return 1;
    }
    return 0;
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int AN_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x7E);
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int AN_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x65, 0x62)) {
            return 1;
        }
    }
    return 0;
}

int IsTrgAndOffPunch(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int AN_GuardMotionAttackCheck(CHARACTER_WORK *offset) {
    if (offset->HitStopTime) {
        if (IsTrgAndOffPunch(offset)) {
            SetActNoNext(offset, 0x5E);
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void ANAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x7E)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_Stand3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x101;
    offset->actst &= 0xFFFFEFFF;
    SetChangeFrameAnime(offset, 0x52);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void ANAct_LongHair1(CHARACTER_WORK *offset) {
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

void ANAct_LongHair3(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x61)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void ANAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoDemo(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void ANAct_WinA(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;
    COLLISION *ex;
    int i;

    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    if (offset->Mark) {
        c_arg.flag = 0x25;
        for (i = 0; i < 15; i++) {
            ex = Get_GCLExNo(offset, i);
            c_arg.Localid = i;
            c_arg.actno = i + 9;
            c_arg.x = offset->posx + ex->x * 100;
            c_arg.y = offset->posy + ex->y * 100;
            EffectCreate(offset, 0x4E, &c_arg);
        }
        offset->Mark = 0;
        offset->actst |= 0x10;
    }
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void ANAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void ANAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_SHW1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoStand(offset);
    if (offset->Mark & 1) {
        offset->ply->PersonalWork[0].l |= 2;
        offset->Mark &= 0xFE;
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int ANHW5ExCommandCheck(CHARACTER_WORK *offset);

void ANAct_HW5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = ANHW5ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x5C)) {
        offset->ColliFlag = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW5b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW5c_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x93);
    EndtoAction(offset, 0x30094);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5c_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x94);
    EndtoAction(offset, 0x20095);
    SetJumpControl(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5c_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x95);
    SetJumpControl(offset);
    EndtoAction(offset, 0x30096);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW5c_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x96);
    SetLandBreakActno(offset, 0x200097);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5c_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x97);
    SetJumpControl(offset);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW5Xc_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB4);
    EndtoAction(offset, 0x300B5);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5Xc_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB5);
    EndtoAction(offset, 0x200B6);
    SetJumpControl(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5Xc_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB6);
    SetJumpControl(offset);
    EndtoAction(offset, 0x300B7);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW5Xc_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB7);
    SetLandBreakActno(offset, 0x2000B8);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5Xc_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB8);
    SetJumpControl(offset);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW5d_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x98);
    EndtoAction(offset, 0x30099);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5d_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x99);
    EndtoAction(offset, 0x2009A);
    SetJumpControl(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5d_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9A);
    SetJumpControl(offset);
    EndtoAction(offset, 0x3009B);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW5d_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9B);
    SetLandBreakActno(offset, 0x20009C);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW5d_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9C);
    SetJumpControl(offset);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int ANHW5ExCommandCheck(CHARACTER_WORK *offset);

void ANAct_HW8(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = ANHW5ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0xAF)) {
        offset->ColliFlag = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int ANHW8XExCommandCheck(CHARACTER_WORK *offset);

void ANAct_HW8X(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = ANHW8XExCommandCheck;
    if (SetChangeFrameAnime(offset, 0xB9)) {
        offset->ColliFlag = 0;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW8Xa_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBA);
    EndtoAction(offset, 0x300BB);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW8Xa_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBB);
    SetLandBreakActno(offset, 0xA7);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW8Xb_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBC);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW4G(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoAction(offset, 0x3005F);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW4S(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5F);
    SetLandBreakActno(offset, 0xA7);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW4Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA7);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW6_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x66);
    EndtoAction(offset, 0x30060);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW6G(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x60);
    SetJumpControl(offset);
    SetLandBreakActno(offset, 0x20008F);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW6Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8F);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW6Sky_Shot(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x90);
    SetLandBreakActno(offset, 0x200092);
    EndtoAction(offset, 0x30091);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW6Sky_Drop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x91);
    SetLandBreakActno(offset, 0x200092);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANAct_HW6Sky_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x92);
    SetJumpControl(offset);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW7(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAC);
    EndtoAction(offset, 0x300AD);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW7_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAD);
    SetLandBreakActno(offset, 0x2000AE);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW7_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAE);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x62);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void ANAct_ITIGEKI(CHARACTER_WORK *offset) {
    if (offset->padid == 0) {
        OrScreenFlag(0x140);
    } else {
        OrScreenFlag(0x240);
    }
    offset->CommandFlag = 0xC05F;
    offset->ply->pactst |= 0x1000;
    if (SetChangeFrameAnime(offset, 0x65)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoAction(offset, 0x63);
}

RECTL *GetScreenRect(void);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_ITIGEKI_Attack(CHARACTER_WORK *offset) {
    RECTL *p = GetScreenRect();

    if (offset->padid == 0) {
        OrScreenFlag(0x140);
    } else {
        OrScreenFlag(0x240);
    }
    offset->CommandFlag = 0xC05F;
    offset->ply->pactst |= 0x1000;
    if (SetChangeFrameAnime(offset, 0x63)) {
        if (offset->dirflag == 1) {
            offset->posx = p->x - 32000;
        } else {
            offset->posx = p->x + p->w + 32000;
        }
    }
    if (offset->dirflag == 0) {
        if (offset->posx < p->x - 152000) {
            SetActNoNext(offset, 0x64);
        }
    } else {
        if (offset->posx > p->x + p->w + 152000) {
            SetActNoNext(offset, 0x64);
        }
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
RECTL *GetScreenRect(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_ITIGEKI_Drop(CHARACTER_WORK *offset) {
    RECTL *p = GetScreenRect();

    offset->CommandFlag = 0xC05F;
    offset->ply->pactst |= 0x1000;
    if (SetChangeFrameAnime(offset, 0x64)) {
        offset->dirflag = !offset->dirflag;
        if (offset->dirflag == 1) {
            offset->posx = p->x - 32000;
        } else {
            offset->posx = p->x + p->w + 32000;
        }
    } else if (offset->ply->EnemyTouch >= 6) {
        SetActNoNext(offset, 0x6E);
        return;
    }
    EndtoAction(offset, 0x6E);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_ITIGEKI_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6E);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x400) | 0x40;
    SetChangeFrameAnime(offset, 0x87);
    EndtoStand(offset);
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

void AN_HW3_HitInterruptProc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    int acttb[19] = {
        0x00, 0xBF, 0xA0, 0xA7, 0x9E, 0xA0, 0xA6, 0xBD, 0x99, 0x9B,
        0xB7, 0x8A, 0xAF, 0xA0, 0xA2, 0xA0, 0x8D, 0x92, 0x92
    };

    SetActNo(offset, 0x9E);
    SetActNoNext(enemy, acttb[enemy->idno]);
    offset->HitStopTime = 0;
    enemy->HitStopTime = 0;
    offset->ply->inertia = 0;
    enemy->ply->inertia = 0;
    offset->actst |= 0x4000;
    enemy->actst |= 0x4000;
    offset->ply->JumpFlag &= ~2;
    enemy->ply->JumpFlag &= ~2;
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

void AN_HW8Xc_HitInterruptProc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    int acttb[19] = {
        0x00, 0xBF, 0xA0, 0xA7, 0x9E, 0xA0, 0xA6, 0xBD, 0x99, 0x9B,
        0xB7, 0x8A, 0xAF, 0xA0, 0xA2, 0xA0, 0x8D, 0x92, 0x92
    };

    SetActNo(offset, 0xBF);
    SetActNoNext(enemy, acttb[enemy->idno]);
    offset->HitStopTime = 0;
    enemy->HitStopTime = 0;
    offset->ply->inertia = 0;
    enemy->ply->inertia = 0;
    offset->actst |= 0x4000;
    enemy->actst |= 0x4000;
    offset->ply->JumpFlag &= ~2;
    enemy->ply->JumpFlag &= ~2;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW3_Sub(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAA);
    EndtoAction(offset, 0x21009D);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void AN_HW3_HitInterruptProc(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW3_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->AtkHitInterrupt = AN_HW3_HitInterruptProc;
    SetChangeFrameAnime(offset, 0x9D);
    SetLandBreakActno(offset, 0x20009F);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW3_Hold(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9E);
    EndtoAction(offset, 0x2000A1);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW3_Vart(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA1);
    SetLandBreakActno(offset, 0x20009F);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW3_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9F);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW8Xc_Sub(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBD);
    EndtoAction(offset, 0x2100BE);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void AN_HW8Xc_HitInterruptProc(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW8Xc_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->AtkHitInterrupt = AN_HW8Xc_HitInterruptProc;
    SetChangeFrameAnime(offset, 0xBE);
    SetLandBreakActno(offset, 0x2000C0);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW8Xc_Hold(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBF);
    EndtoAction(offset, 0x2000C1);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void ANAct_HW8Xc_Vart(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC1);
    SetLandBreakActno(offset, 0x2000C0);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANAct_HW8Xc_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC0);
    EndtoStand(offset);
}
