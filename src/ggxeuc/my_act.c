#include "generated/my_act.h"

int EndtoStand(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*MYAct_jumpTb[])() = {
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
    CmnAct_ForwardDownBound, CmnAct_VertDownBound, 0, MYAct_LongHair1, MYAct_LongHair2,
    MYAct_WinC, MYAct_LongHair4, MYAct_EntryC_2, MYAct_Tokusyu1, 0, MYAct_Tokusyu2,
    MYAct_EntryA_2, MYAct_EntryB_2, MYAct_EntryC_1, MYAct_EntryA_1, MYAct_EntryB_1,
    CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageBySL, CmnAct_ThrowDamageByPO,
    CmnAct_ThrowDamageByVE, CmnAct_ThrowDamageByJM, CmnAct_ThrowDamageByML, MYAct_Throw,
    MYAct_LongHair5, 0, MYAct_ITIGEKI, MYAct_Hissatu2_1a, MYAct_Hissatu2_1b, MYAct_Hissatu2_1c,
    MYAct_DA1, MYAct_ThrowB_Miss, MYAct_OVERKILLMODE, MYAct_WinB_1, MYAct_WinB_2,
    MYAct_Hissatu3_1, MYAct_Hissatu3_2, MYAct_Hissatu3_3, MYAct_Hissatu4_1,
    MYAct_HissatuIruka_modori2, MYAct_Hissatu2_1d, MYAct_Hissatu2_Land,
    CmnAct_ThrowDamageBySLEX, CmnAct_ThrowDamageByKY, CmnAct_ThrowDamageByZT,
    CmnAct_ThrowDamageByCH, CmnAct_ThrowDamageByJY, MYAct_ThrowB, MYAct_ThrowB_2,
    CmnAct_ThrowDamageByMYEX, MYAct_Hissatu6_1, MYAct_Hissatu6_2, MYAct_HissatuIruka_modori,
    MYAct_Hissatu6H_1, MYAct_Hissatu6H_2, MYAct_Hissatu5_1, MYAct_Hissatu5_2, MYAct_Hissatu5H_1,
    MYAct_Hissatu5H_2, MYAct_Kakusei_1, MYAct_Kakusei_2, CmnAct_ThrowDamageByZTEX,
    CmnAct_ThrowDamageByPOEX, CmnAct_ThrowDamageByPOEXJump, CmnAct_ThrowDamageByPOEXLand,
    CmnAct_ThrowDamageByCHEX, CmnAct_ThrowDamageByFA, CmnAct_ThrowDamageByFA_NAME,
    CmnAct_ThrowDamageByDZ, CmnAct_SHWDamageByFA1, CmnAct_SHWDamageByFA2,
    CmnAct_ThrowDamageByPOEX2Jump, CmnAct_ThrowDamageByPOEX2CeilingLand,
    CmnAct_ThrowDamageByPOEX2Drop, CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK,
    CmnAct_ThrowDamageByAX, MYAct_Hissatu2_2a, CmnAct_DieByFA, 0, CmnAct_ThrowDamageByTS,
    CmnAct_ThrowDamageByAN, CmnAct_DmpByFA, CmnAct_AXTenhousekiDamege, CmnAct_DieByJY,
    CmnAct_CutByJY, MYAct_ItigekiThrow, MYAct_ItigekiThrowCatch, MYAct_ItigekiThrowJump,
    CmnAct_ThrowDamageByMYIT, CmnAct_ThrowDamageByMYITJump, CmnAct_DmgByANHW3,
    CmnAct_ITIGEKIDamageByVE, MYAct_LongHair3, CmnAct_Casket, CmnAct_ItigekiDamegeByBK,
    CmnAct_DieByTS, MYAct_Lose, CmnAct_Die2ByTS, CmnAct_ThrowDamageByJS, CmnAct_JSVADamege,
    CmnAct_ThrowDamageByKR, CmnAct_ThrowDamageByKYEX, MYAct_Hissatu3x_1, MYAct_Hissatu3x_2,
    MYAct_Hissatu3x_3, MYAct_Hissatu3x_4, MYAct_HissatuEX1_Start, MYAct_HissatuEX1_Sky,
    MYAct_HissatuEX2_Start, MYAct_HissatuEX2_Sky, MYAct_HissatuEXx_End, MYAct_HissatuEX1H_Start,
    MYAct_HissatuEX1H_Sky, MYAct_HissatuEX2H_Start, MYAct_HissatuEX2H_Sky,
    MYAct_HissatuEXxH_End, 0,
};

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDownTAMEUp(CHARACTER_WORK *offset);
int IsGyakuYoga(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsLeftTAMERight(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsYoga(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int MY_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x04080087);
            return 1;
        }
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x02080067);
            return 1;
        }
        if (IsGyakuYoga(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x020800B3);
                return 1;
            }
            SetAttackActNo(offset, 0x0208006F);
            return 1;
        }
        if (!(offset->ply->ObjFlag & 1) && IsYoga(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                offset->ply->PersonalWork[1].w[0] = 0;
                SetAttackActNo(offset, 0x02080072);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                offset->ply->PersonalWork[1].w[0] = 1;
                SetAttackActNo(offset, 0x02080072);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                offset->ply->PersonalWork[1].w[0] = 2;
                SetAttackActNo(offset, 0x02080072);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                offset->ply->PersonalWork[1].w[0] = 3;
                SetAttackActNo(offset, 0x02080072);
                return 1;
            }
        }
        if (IsDownTAMEUp(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x020800B9);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x020800BE);
                    return 1;
                }
            } else {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x0208007E);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02080081);
                    return 1;
                }
            }
        }
        if (IsLeftTAMERight(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x020800B7);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x020800BC);
                    return 1;
                }
            } else {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080083);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02080085);
                    return 1;
                }
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x02080099);
            return 1;
        }
    }
    return 0;
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void MY_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    enemy = GetEnemyPlayer(offset);
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (enemy->idno == 0xE) {
        SetAttackActNo(offset, 0x02080064);
        return;
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
        return;
    }
    SetAttackActNo(offset, 0x020800A9);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void MY_RespectCommnadCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    enemy = GetEnemyPlayer(offset);
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (enemy->idno == 0xE && g_info.PlayerVoice[offset->padid] == 0) {
        SetAttackActNo(offset, 0x02080053);
        return;
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080051);
        return;
    }
    SetAttackActNo(offset, 0x02080053);
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int MY_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    if (GetTensionVal(offset) >= 5000 && !(offset->actst & 0x10) && IsDouji(offset, 0xF, 2, 2)) {
        if (IsStdRight(offset) || IsSitRight(offset)) {
            offset->actst |= 0x80;
            SetAttackActNo(offset, 0x0308006A);
            return 1;
        }
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int MY_InterruptProc(CHARACTER_WORK *offset) {
    SetActNoNext(offset, 0x80080);
    offset->HitStopTime = 0;
    return 0;
}

int MY_InterruptProcEX(CHARACTER_WORK *offset) {
    offset->HitStopTime = 0;
    return 0;
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);

int MYHW5_LandInterruptProc(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    arg.flag = 0x104;
    arg.actno = 0xD;
    EffectCreate(offset, 0x44, &arg);
    return 0;
}

int IsGyakuYoga(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);

int MY_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 0xA2 && offset->Mark) {
        return 0xF;
    }
    if (offset->actst & 0x10) {
        return 0;
    }
    if (offset->actno == 0xA2 || offset->actno == 0xA3) {
        return 0;
    }
    if (IsGyakuYoga(offset) && IsTrgAndOffKick(offset)) {
        return 4;
    }
    return 0;
}

int IsGyakuYoga(CHARACTER_WORK *offset);
int IsGyakuYogax2(CHARACTER_WORK *offset);
int IsYogax2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int MY_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsGyakuYogax2, IsTrgAndOffHighSlash };
    static void *functb2[2] = { IsYogax2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0xA2, 0x6C)) {
            return 1;
        }
        if (ItigekiCommandCheck(offset, functb2, 0xA2, 0x6C)) {
            return 1;
        }
        if (IsGyakuYoga(offset) && IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x0208006B);
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void MYAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x6A)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_ThrowB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x7B);
    EndtoAction(offset, 0x7C);
}

void MYAct_ThrowB_2(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x7C);
    SetLandBreakActno(offset, 0x15);
}

void MYAct_ThrowB_Miss(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x6B);
    EndtoStand(offset);
}

void MYAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x63);
    EndtoStand(offset);
}

void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void MYAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void MYAct_LongHair3(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0xA9)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void MYAct_LongHair5(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x64)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void MYAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void MYAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x54);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);

void MYAct_Tokusyu1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);

void MYAct_Tokusyu2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    SetChangeFrameAnime(offset, 0x57);
    SetLandBreakActno(offset, 0x15);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void MYAct_WinB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x6D);
    TimetoAction(offset, 0x6E, 0x5A);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void MYAct_WinB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x6E);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void MYAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void MYAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6C);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void MYAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x66)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int MY_ItigekiThrowCheck(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void MYAct_ItigekiThrow(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = MY_ItigekiThrowCheck;
    if (SetChangeFrameAnime(offset, 0xA2)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetMYITThrowDamageActNoJump(CHARACTER_WORK *offset);

void MYAct_ItigekiThrowCatch(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA3);
    offset->actst |= 0x100000;
    if (EndtoAction(offset, 0xA4) != 0) {
        SetMYITThrowDamageActNoJump(GetEnemyPlayer(offset));
    }
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MYAct_ItigekiThrowJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xA4)) {
        offset->xspeed = -offset->posx / 40;
    }
    if (offset->Mark) {
        if (offset->padid) {
            OrScreenFlag(0x10004);
        } else {
            OrScreenFlag(0x20004);
        }
    } else if (offset->padid) {
        OrScreenFlag(0xD);
    } else {
        OrScreenFlag(0xE);
    }
    offset->actst |= 0x100000;
    SetLandBreakActno(offset, 0x15);
}

int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern PAD GameKey[2];

int MYHW2_ExCommandCheck(CHARACTER_WORK *offset) {
    if (offset->ply->PersonalWork[0].ub[0] == 0) {
        return 0;
    }
    if (!(offset->Mark & 2)) {
        return 0;
    }
    if (IsTrgAndOffSlash(offset)) {
        if ((GameKey[offset->padid].data & 0x1000) && (GameKey[offset->padid].data & 0x2000)) {
            offset->ply->PersonalWork[0].w[1] = 5;
        } else if ((GameKey[offset->padid].data & 0x1000) && (GameKey[offset->padid].data & 0x8000)) {
            offset->ply->PersonalWork[0].w[1] = 6;
        } else if ((GameKey[offset->padid].data & 0x4000) && (GameKey[offset->padid].data & 0x2000)) {
            offset->ply->PersonalWork[0].w[1] = 7;
        } else if ((GameKey[offset->padid].data & 0x4000) && (GameKey[offset->padid].data & 0x8000)) {
            offset->ply->PersonalWork[0].w[1] = 8;
        } else if (GameKey[offset->padid].data & 0x1000) {
            offset->ply->PersonalWork[0].w[1] = 1;
        } else if (GameKey[offset->padid].data & 0x4000) {
            offset->ply->PersonalWork[0].w[1] = 2;
        } else if (GameKey[offset->padid].data & 0x2000) {
            offset->ply->PersonalWork[0].w[1] = 3;
        } else if (GameKey[offset->padid].data & 0x8000) {
            offset->ply->PersonalWork[0].w[1] = 4;
        } else {
            return 0;
        }
        SetAttackActNo(offset, 0x74);
        return 1;
    }
    return 0;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu2_1a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x67) != 0) {
        offset->ply->PersonalWork[0].b[0] = 3;
        offset->ply->PersonalWork[0].w[1] = 0;
    }
    EndtoAction(offset, 0x68);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void MYAct_Hissatu2_2a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 0x99) != 0) {
        offset->ply->PersonalWork[0].b[0] = 3;
        offset->ply->PersonalWork[0].w[1] = 0;
    }
    EndtoAction(offset, 0x68);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int MYHW2_ExCommandCheck(CHARACTER_WORK *offset);

void MYAct_Hissatu2_1b(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x68);
    offset->ply->ExCommandFunc = MYHW2_ExCommandCheck;
    EndtoAction(offset, 0x69);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void MYAct_Hissatu2_1c(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x69) != 0) {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0xC8, 0x96);
        offset->Localid = 0;
    }
    SetLandBreakActno(offset, 0x75);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
int MYHW2_ExCommandCheck(CHARACTER_WORK *offset);

void MYAct_Hissatu2_1d(CHARACTER_WORK *offset) {
    static const int speedtb[16] = {
        0, -1500, 0, 1500, 1500, 0, -1500, 0,
        1060, -1060, -1060, -1060, 1060, 1060, -1060, 1060,
    };

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x74)) {
        offset->ply->PersonalWork[0].ub[0]--;
    }
    if (offset->Mark & 1) {
        int i = offset->ply->PersonalWork[0].w[1] - 1;

        SetXSpeed(offset, speedtb[i << 1]);
        SetYSpeed(offset, speedtb[(i << 1) + 1], 0);
    }
    offset->ply->ExCommandFunc = MYHW2_ExCommandCheck;
    EndtoAction(offset, 0x69);
    SetLandBreakActno(offset, 0x75);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu2_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x75);
    EndtoStand(offset);
}

int IsActEnd(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6F);
    if (IsActEnd(offset)) {
        if (offset->attackst & 0x80000) {
            SetActNoNext(offset, 0x70);
        } else {
            SetActNoNext(offset, 0x71);
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x70);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x71);
    EndtoStand(offset);
}

void FontDraw(char *str, int x, int y, float z, int alfa);
char *G_sprintf_arg1(char *str, int arg1);

void MYHissatu3LevelDraw(int padid, int level) {
    int x;

    x = padid ? 0x226 : 0x5A;
    FontDraw(G_sprintf_arg1("POWER %d", level), x - 0x30, 0x1A4, 65.0f, 0xFF);
}

int IsActEnd(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
void MYHissatu3LevelDraw(int padid, int level);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3x_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xB3)) {
        offset->ply->PersonalWork[0].w[1] = 0;
        offset->Localid = 0;
    }
    if (IsTrgAndOffHighSlash(offset)) {
        if (++offset->Localid >= 25) {
            offset->Localid = 24;
        }
        offset->ply->PersonalWork[0].w[1] = offset->Localid;
    }
    MYHissatu3LevelDraw(offset->padid, offset->ply->PersonalWork[0].w[1] / 3 + 1);
    if (IsActEnd(offset)) {
        if (offset->attackst & 0x80000) {
            SetActNoNext(offset, 0xB4);
        } else {
            SetActNoNext(offset, 0x71);
        }
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void MYHissatu3LevelDraw(int padid, int level);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3x_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xB4)) {
        offset->Localid -= 3;
    }
    MYHissatu3LevelDraw(offset->padid, offset->ply->PersonalWork[0].w[1] / 3 + 1);
    if (offset->Localid >= 0) {
        EndtoAction(offset, 0xB5);
        return;
    }
    EndtoAction(offset, 0xB6);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void MYHissatu3LevelDraw(int padid, int level);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3x_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xB5)) {
        offset->Localid -= 3;
    }
    MYHissatu3LevelDraw(offset->padid, offset->ply->PersonalWork[0].w[1] / 3 + 1);
    if (offset->Localid >= 0) {
        EndtoAction(offset, 0xB5);
        return;
    }
    EndtoAction(offset, 0xB6);
}

int EndtoStand(CHARACTER_WORK *offset);
void MYHissatu3LevelDraw(int padid, int level);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu3x_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB6);
    MYHissatu3LevelDraw(offset->padid, offset->ply->PersonalWork[0].w[1] / 3 + 1);
    EndtoStand(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu4_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x72);
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu6_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x7E)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x7F);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* MYHW5_LandInterruptProc: no debug record */
/* MY_InterruptProc: no debug record */

int MYHW5_LandInterruptProc(CHARACTER_WORK *offset);
int MY_InterruptProc(CHARACTER_WORK *offset);

void MYAct_Hissatu6_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7F);
    offset->ply->AtkHitInterrupt = MY_InterruptProc;
    offset->ply->LandInterrupt = MYHW5_LandInterruptProc;
    offset->ply->AtkGuardInterrupt = MY_InterruptProc;
    SetLandBreakActno(offset, 0x15);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Hissatu6H_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x81)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x82);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* MYHW5_LandInterruptProc: no debug record */
/* MY_InterruptProc: no debug record */

int MYHW5_LandInterruptProc(CHARACTER_WORK *offset);
int MY_InterruptProc(CHARACTER_WORK *offset);

void MYAct_Hissatu6H_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x82);
    offset->ply->AtkHitInterrupt = MY_InterruptProc;
    offset->ply->LandInterrupt = MYHW5_LandInterruptProc;
    offset->ply->AtkGuardInterrupt = MY_InterruptProc;
    SetLandBreakActno(offset, 0x15);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void MYAct_Hissatu5_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 0x83)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x84);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* MY_InterruptProc: no debug record */

int MY_InterruptProc(CHARACTER_WORK *offset);

void MYAct_Hissatu5_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x84);
    offset->ply->AtkHitInterrupt = MY_InterruptProc;
    offset->ply->AtkGuardInterrupt = MY_InterruptProc;
    SetLandBreakActno(offset, 0x15);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void MYAct_Hissatu5H_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 0x85);
    EndtoAction(offset, 0x86);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* MY_InterruptProc: no debug record */

int MY_InterruptProc(CHARACTER_WORK *offset);

void MYAct_Hissatu5H_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x86);
    offset->ply->AtkHitInterrupt = MY_InterruptProc;
    offset->ply->AtkGuardInterrupt = MY_InterruptProc;
    SetLandBreakActno(offset, 0x15);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MYAct_HissatuIruka_modori(CHARACTER_WORK *offset) {
    CREATE_ARG arg;
    short no;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x80)) {
        arg.flag = 0x104;
        switch (offset->Localid) {
        case 1:
            no = 3;
            break;
        case 2:
            no = 4;
            break;
        case 3:
            no = 5;
            break;
        case 4:
            goto end;
        default:
            goto create;
        }
        arg.actno = offset->Localid = no;
    create:
        EffectCreate(offset, 0x44, &arg);
    }
end:
    SetLandBreakActno(offset, 0x73);
}

int EndtoSit(CHARACTER_WORK *offset);
void RqLndEff(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuIruka_modori2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x73)) {
        RqLndEff(offset);
    }
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuEX1_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB7);
    EndtoAction(offset, 0xB8);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MYAct_HissatuEX1_Sky(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB8);
    SetLandBreakActno(offset, 0xBB);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuEX2_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB9);
    EndtoAction(offset, 0xBA);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MYAct_HissatuEX2_Sky(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBA);
    SetLandBreakActno(offset, 0xBB);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuEXx_End(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBB);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuEX1H_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBC);
    EndtoAction(offset, 0xBD);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MYAct_HissatuEX1H_Sky(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBD);
    SetLandBreakActno(offset, 0xC0);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuEX2H_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBE);
    EndtoAction(offset, 0xBF);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MYAct_HissatuEX2H_Sky(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBF);
    SetLandBreakActno(offset, 0xC0);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_HissatuEXxH_End(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC0);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void MYAct_Kakusei_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x87);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void MYAct_Kakusei_2(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x88)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x80);
}
