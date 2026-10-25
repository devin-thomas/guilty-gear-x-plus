#include "generated/ch_act.h"

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
CHARACTER_WORK *SearchObjectWorkActnowithFuncExec(CHARACTER_WORK *off, short unsigned int idno, int actno, void (*Func)());
COLLISION *Get_GCLEx(CHARACTER_WORK *offset);
int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int EndtoDemo(CHARACTER_WORK *offset);
int EndtoDrop(CHARACTER_WORK *offset);
int EndtoSit(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
int GetPlayerDist(void);
int GetPlayerDisty(void);
int GetPlayerDistyOff(CHARACTER_WORK *offset);
int GetXMove(CHARACTER_WORK *offset, int val);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsSitaSita(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsTrgHighSlash(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int IsYoga(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int LandtoAction(CHARACTER_WORK *offset, int actno);
int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
short int GetTensionVal(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);
void SetWinActFlag(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);
void Tension_Dec(CHARACTER_WORK *offset, int Val);
void set_random_seed_abs(void);
extern USER_SETTING userinfo;
extern GAME_INFOMATION g_info;
int CH_HW4_ThrowCheck(CHARACTER_WORK *offset);
int CHHW8_1ExCommandCheck(CHARACTER_WORK *offset);
int CHHW8_2ExCommandCheck(CHARACTER_WORK *offset);
void CHSKYRANBUSetYSpeed(CHARACTER_WORK *offset, int off);
int CHHWXCommandCheck(CHARACTER_WORK *offset);
int CHHWXEndCheck(CHARACTER_WORK *offset);

#include "generated/cmn_act.h"
#include "generated/throw.h"

void (*CHAct_jumpTb[])() = {
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
    CmnAct_ForwardGetUp, CmnAct_Faint, CHAct_FK_First, CmnAct_ForwardDownLand,
    CmnAct_VertDownDrop, CmnAct_VertDownLand, CmnAct_Drop_Back, CmnAct_DownBound,
    CmnAct_ForwardDownBound, CmnAct_VertDownBound, 0, CHAct_LongHair1, CHAct_LongHair2,
    CHAct_LongHair3, CHAct_LongHair4, CHAct_HW2_BB, CHAct_HW2_BBSky, CHAct_HW2_BBDrop,
    CHAct_HW2_BBLand, CHAct_WinA, CHAct_WinB, CHAct_WinC, CHAct_EntryA_1, CHAct_EntryA_2,
    CHAct_EntryB_1, CHAct_EntryB_2, CHAct_SHW1_SKYRANBU_First, CHAct_SHW1_SKYRANBU_1,
    CHAct_SHW1_SKYRANBU_2, CHAct_SHW1_SKYRANBU_3, CHAct_SHW1_SKYRANBU_4, CHAct_SHW1_SKYRANBU_5,
    CHAct_SHW1_SKYRANBU_6, CHAct_SHW1_SKYRANBU_Land, CHAct_SHW1_SKYRANBU_7, CHAct_HW3_CB,
    CHAct_HW3_CBBack, CHAct_HW2_SBB, CHAct_HW2_SBBSky, CHAct_HW2_SBBDrop, CHAct_HW2_SBBLand,
    CHAct_FK_Sky, CHAct_FK_Land, CHAct_Sakugan, CHAct_HW1_AB, CHAct_HW1_ABBack, CHAct_HW5_TS,
    CHAct_HW5_TSAppear, CHAct_HW6_TL, CHAct_HW6_TLAppear, CHAct_HW7_Meisai, CHAct_Throw,
    CmnAct_ThrowDamageBySL, CmnAct_ThrowDamageByPO, CmnAct_ThrowDamageByVE,
    CmnAct_ThrowDamageByJM, CmnAct_ThrowDamageByML, CmnAct_ThrowDamageBySLEX,
    CmnAct_ThrowDamageByZT, CmnAct_ThrowDamageByCH, CmnAct_ThrowDamageByKY,
    CmnAct_ThrowDamageByJY, CmnAct_ThrowDamageByMY, CmnAct_ThrowDamageByMYEX, CHAct_HW1_SAB,
    CHAct_HW1_SABBack, CmnAct_ThrowDamageByPOEX, CmnAct_ThrowDamageByPOEXJump,
    CmnAct_ThrowDamageByPOEXLand, CHAct_SHW2_RANBU_First, CHAct_SHW2_RANBU_Beat,
    CHAct_SHW2_RANBU_BB, CHAct_SHW2_RANBU_Back, CHAct_HW4_KONOHA, CHAct_HW4_KONOHAJump,
    CHAct_HW4_KONOHALand, CHAct_HW4_KONOHAThrow, CHAct_HW4_KONOHAThrowBack,
    CmnAct_ThrowDamageByCHEX, CmnAct_ThrowDamageByZTEX, CHAct_SHW1_SKYRANBU_Land2,
    CHAct_OVERKILLMODE, CHAct_ITIGEKI, CHAct_ITIGEKIJump, CHAct_ITIGEKIMissDrop,
    CHAct_ITIGEKIMissLand, CHAct_ITIGEKIDrop, CHAct_ITIGEKILand, CmnAct_ThrowDamageByFA,
    CmnAct_ThrowDamageByFA_NAME, CmnAct_ThrowDamageByDZ, CmnAct_SHWDamageByFA1,
    CmnAct_SHWDamageByFA2, CHAct_DA1, CmnAct_ThrowDamageByPOEX2Jump,
    CmnAct_ThrowDamageByPOEX2CeilingLand, CmnAct_ThrowDamageByPOEX2Drop,
    CmnAct_ThrowDamageByPOEX2Land, CmnAct_ThrowDamageByBK, CmnAct_ThrowDamageByAX,
    CmnAct_DieByFA, 0, CmnAct_ThrowDamageByTS, CmnAct_ThrowDamageByAN, CmnAct_DmpByFA,
    CmnAct_AXTenhousekiDamege, CHAct_StiffLand, CmnAct_DieByJY, CHAct_HW8_Sanren_1,
    CHAct_HW8_Sanren_2, CHAct_HW8_Sanren_3, CHAct_HW8_Sanren_3Jump, CHAct_HW8_Sanren_3Land,
    CmnAct_CutByJY, CHAct_HW5_TS2, CHAct_HW5_TS2Appear, CHAct_HW6_TL2, CHAct_HW6_TL2Appear,
    CmnAct_ThrowDamageByMYIT, CmnAct_ThrowDamageByMYITJump, CmnAct_DmgByANHW3,
    CmnAct_ITIGEKIDamageByVE, CmnAct_Casket, CmnAct_ItigekiDamegeByBK, CmnAct_DieByTS,
    CHAct_Lose, CmnAct_Die2ByTS, CmnAct_ThrowDamageByJS, CmnAct_JSVADamege,
    CmnAct_ThrowDamageByKR, CmnAct_ThrowDamageByKYEX, CHAct_HW2X_BB, CHAct_HW2X_BB2,
    CHAct_HW2X_BBSky, CHAct_HW3X_CB, CHAct_HW3X_CB2, CHAct_HW2X_SBB, CHAct_HW2X_SBB2,
    CHAct_HW2X_SBBSky, CHAct_HW1X_AB, CHAct_HW1X_AB2, CHAct_HW1X_SAB, CHAct_HW1X_SAB2, 0,
};

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsSitaSita(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsYoga(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int CH_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x0408005F);
            return 1;
        }
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x0408008A);
            return 1;
        }
        if (IsYoga(offset)) {
            if (IsTrgAndOffHighSlash(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    SetAttackActNo(offset, 0x020800CB);
                    return 1;
                } else {
                    SetAttackActNo(offset, 0x02080068);
                    return 1;
                }
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x0208008E);
                return 1;
            }
        }
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x020800C8);
                return 1;
            } else {
                SetAttackActNo(offset, 0x02080054);
                return 1;
            }
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffSlash(offset) && !(userinfo.gguse[offset->padid] & 4)) {
                SetAttackActNo(offset, 0x020800B1);
                return 1;
            }
            if (IsTrgAndOffPunch(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    SetAttackActNo(offset, 0x020800D0);
                    return 1;
                } else {
                    SetAttackActNo(offset, 0x02080071);
                    return 1;
                }
            }
        }
        if (IsSitaSita(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x02080075);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x020800B9);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x020800B7);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080073);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x020800CD);
                return 1;
            } else {
                SetAttackActNo(offset, 0x0208006A);
                return 1;
            }
        }
        if (IsHadou(offset) && IsTrgAndOffPunch(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x020800D2);
                return 1;
            } else {
                SetAttackActNo(offset, 0x02080085);
                return 1;
            }
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void CH_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
    } else {
        SetAttackActNo(offset, 0x02080052);
    }
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void CH_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080051);
    } else {
        SetAttackActNo(offset, 0x02080053);
    }
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int CH_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0xa2);
}

int CH_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 0x8f) {
        return 6;
    }
    return 0;
}

int IsHadoux2(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int CH_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x97, 0x96)) {
            return 1;
        }
        if (offset->ply->PersonalWork[1].w[0] == 0) {
            if (IsTatsumaki(offset) && IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x02080077);
                return 1;
            }
        }
    }
    return 0;
}

void CHAct_HW1_AB(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x10;
    SetChangeFrameAnime(offset, 0x71);
    EndtoAction(offset, 0x72);
    if (!(offset->ActTimer & 3) && offset->xspeed != 0 && offset->HitStopTime == 0) {
        arg.flag = 5;
        arg.actno = 1;
        arg.x = offset->posx;
        arg.y = 0;
        EffectCreate(offset, 0x61, &arg);
    }
}

void CHAct_HW1_ABBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x72);
    offset->actst &= ~0x10;
    offset->actst |= 0x400;
    EndtoSit(offset);
}

void CHAct_HW1_SAB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x85);
    offset->ply->JumpFlag |= 4;
    EndtoAction(offset, 0x86);
}

void CHAct_HW1_SABBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x86);
}

void CHAct_StiffLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xaf);
    EndtoSit(offset);
}

void CHAct_HW2_BB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x54);
    EndtoAction(offset, 0x55);
}

void CHAct_HW2_BBSky(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x55);
    SetJumpControl(offset);
    EndtoAction(offset, 0x10056);
}

void CHAct_HW2_BBDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x56);
    SetLandBreakActno(offset, 0x57);
}

void CHAct_HW2_BBLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x57);
    EndtoStand(offset);
}

void CHAct_HW2_SBB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x6a);
    EndtoAction(offset, 0x6b);
}

void CHAct_HW2_SBBSky(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x6b);
    SetJumpControl(offset);
    EndtoAction(offset, 0x1006c);
}

void CHAct_HW2_SBBDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x6c);
    SetLandBreakActno(offset, 0x6d);
}

void CHAct_HW2_SBBLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x6d);
    EndtoStand(offset);
}

void CHAct_HW3_CB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x68);
    TimetoAction(offset, 0x69, 60);
    if (offset->Mark) {
        SetActNoNext(offset, 0x69);
    }
}

void CHAct_HW3_CBBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x69);
    EndtoStand(offset);
}

void CHAct_HW4_KONOHA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x8e);
    
}

int CH_HW4_ThrowCheck(CHARACTER_WORK *offset);

void CHAct_HW4_KONOHAJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (offset->Mark) {
        offset->ply->ExCommandFunc = CH_HW4_ThrowCheck;
    }
    SetChangeFrameAnime(offset, 0x8f);
    SetLandBreakActno(offset, 0x90);
}

void CHAct_HW4_KONOHALand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x90);
    
}

void CHAct_HW4_KONOHAThrow(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x40;
    if (SetChangeFrameAnime(offset, 0x91)) {
        enemy = offset->ply->Enemy;
        offset->dirflag = !enemy->dirflag;
        offset->posy = enemy->posy - 12000;
        offset->posx = enemy->posx + GetXMove(enemy, -3000);
    }
    EndtoAction(offset, 0x92);
}

void CHAct_HW4_KONOHAThrowBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x92);
    SetLandBreakActno(offset, 0x90);
}

void CHAct_HW5_TS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x73);
    EndtoAction(offset, 0x74);
}

void CHAct_HW5_TSAppear(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    int x, y;

    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0x74)) {
        enemy = offset->ply->Enemy;
        x = abs(enemy->posx - offset->posx) - 8000;
        y = enemy->posy - 20000;
        SetXSpeed(offset, x / 4);
        SetYSpeed(offset, y / 4, 0);
        SetJumpControl(offset);
    }
    EndtoDrop(offset);
}

void CHAct_HW5_TS2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xb7);
    EndtoAction(offset, 0xb8);
}

void CHAct_HW5_TS2Appear(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0xb8)) {
        SetXSpeed(offset, 2000);
        SetYSpeed(offset, -5000, 0);
        SetJumpControl(offset);
    }
    EndtoDrop(offset);
}

void CHAct_HW6_TL(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x75);
    EndtoAction(offset, 0x76);
}

void CHAct_HW6_TLAppear(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x76);
    EndtoStand(offset);
}

void CHAct_HW6_TL2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xb9);
    EndtoAction(offset, 0xba);
}

void CHAct_HW6_TL2Appear(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xba);
    EndtoStand(offset);
}

void CHAct_HW7_Meisai(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x77);
    if (offset->Mark) {
        offset->ply->PersonalWork[1].w[0] = 480;
    }
    EndtoStand(offset);
}

int CHHW8_1ExCommandCheck(CHARACTER_WORK *offset) {
    if (offset->Mark == 0) {
        return 0;
    }
    if (IsHadou(offset) && IsTrgAndOffSlash(offset)) {
        SetAttackActNo(offset, 0x020800B2);
        return 1;
    }
    if (IsHadou(offset) && IsTrgAndOffKick(offset)) {
        SetAttackActNo(offset, 0x020800B3);
        return 1;
    }
    return 0;
}

int CHHW8_2ExCommandCheck(CHARACTER_WORK *offset) {
    if (offset->Mark == 0) {
        return 0;
    }
    if (IsHadou(offset) && IsTrgAndOffKick(offset)) {
        SetAttackActNo(offset, 0x020800B3);
        return 1;
    }
    return 0;
}

void CHAct_HW8_Sanren_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHW8_1ExCommandCheck;
    SetChangeFrameAnime(offset, 177);
    EndtoStand(offset);
}

void CHAct_HW8_Sanren_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHW8_2ExCommandCheck;
    SetChangeFrameAnime(offset, 178);
    EndtoStand(offset);
}

void CHAct_HW8_Sanren_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xb3);
    EndtoAction(offset, 0xb4);
}

void CHAct_HW8_Sanren_3Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xb4);
    SetLandBreakActno(offset, 0xb5);
}

void CHAct_HW8_Sanren_3Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0xb5);
    EndtoStand(offset);
}

void CHAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void CHAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

void CHAct_LongHair3(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x52)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void CHAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

void CHAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x5b);
    
}

void CHAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x5c);
    EndtoStand(offset);
}

void CHAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x5d);
    
}

void CHAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x5e);
    EndtoStand(offset);
}

void CHSKYRANBUSetYSpeed(CHARACTER_WORK *offset, int off) {
    int y = GetPlayerDistyOff(offset) + off;
    int yspeed = (float)y / (float)GetPlayerDist() * 6000.0f;

    if (yspeed > -300) {
        yspeed = -300;
    }
    if (yspeed < -1000) {
        yspeed = -1000;
    }
    SetYSpeed(offset, yspeed, 0);
}

void CHAct_SHW1_SKYRANBU_First(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0x5f)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0x60);
}

void CHAct_SHW1_SKYRANBU_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x60)) {
        SetXSpeed(offset, 6000);
        CHSKYRANBUSetYSpeed(offset, 0);
    }
    offset->actst |= 0x100000;
    OrScreenFlag(0x40);
    JumpControl(offset);
    if (IsActTimeOver(offset, 3) && (offset->actst & 0x2800)) {
        SetActNoNext(offset, 0x61);
    }
}

void CHAct_SHW1_SKYRANBU_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x61)) {
        SetXSpeed(offset, -6000);
        CHSKYRANBUSetYSpeed(offset, 0);
    }
    offset->actst |= 0x100000;
    OrScreenFlag(0x40);
    JumpControl(offset);
    if (IsActTimeOver(offset, 3) && (offset->actst & 0x2800)) {
        if (offset->Localid) {
            SetActNoNext(offset, 0x62);
        } else {
            offset->Localid = 1;
            SetActNoNext(offset, 0x60);
        }
    }
}

void CHAct_SHW1_SKYRANBU_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x62)) {
        SetXSpeed(offset, 6000);
        CHSKYRANBUSetYSpeed(offset, 0);
    }
    offset->actst |= 0x100000;
    OrScreenFlag(0x40);
    JumpControl(offset);
    if (IsActTimeOver(offset, 3) && (offset->actst & 0x2800)) {
        if (offset->attackst & 0x1000) {
            SetActNoNext(offset, 0x67);
        } else {
            SetActNoNext(offset, 0x65);
        }
    }
}

void CHAct_SHW1_SKYRANBU_7(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x67)) {
        SetXSpeed(offset, -6000);
        CHSKYRANBUSetYSpeed(offset, -20000);
    }
    offset->actst |= 0x100000;
    OrScreenFlag(0x40);
    JumpControl(offset);
    if (GetPlayerDist() < 6000) {
        SetActNoNext(offset, 0x63);
    }
    if (IsActTimeOver(offset, 3) && (offset->actst & 0x2800)) {
        SetActNoNext(offset, 0x65);
    }
}

void CHAct_SHW1_SKYRANBU_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (offset->padid == 0) {
        OrScreenFlag(0x2d);
    } else {
        OrScreenFlag(0x2e);
    }
    SetAnten(offset, 1);
    SetChangeFrameAnime(offset, 0x63);
    offset->actst |= 0x100000;
    JumpControl(offset);
    EndtoAction(offset, 0x64);
}

void CHAct_SHW1_SKYRANBU_5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (offset->padid == 0) {
        OrScreenFlag(0x1025);
    } else {
        OrScreenFlag(0x1026);
    }
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x64)) {
        offset->ColliFlag = 0;
    }
    offset->actst |= 0x100000;
    JumpControl(offset);
    LandtoAction(offset, 0x66);
}

void CHAct_SHW1_SKYRANBU_6(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0x65)) {
        offset->gravity = 100;
    }
    JumpControl(offset);
    LandtoAction(offset, 0x95);
}

void CHAct_SHW1_SKYRANBU_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 1);
    if (offset->padid == 0) {
        OrScreenFlag(0x1025);
    } else {
        OrScreenFlag(0x1026);
    }
    SetChangeFrameAnime(offset, 0x66);
}

void CHAct_SHW1_SKYRANBU_Land2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x95);
}

void CHAct_SHW2_RANBU_First(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x8a);
    if (userinfo.gguse[offset->padid] & 4) {
        SetAnten(offset, 1);
    }
    if (GetPlayerDist() < 10000 && GetPlayerDisty() < 20000) {
        EndtoAction(offset, 0x8b);
    }
    TimetoAction(offset, 0x8d, 30);
}

void CHAct_SHW2_RANBU_Beat(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x8b);
    if (userinfo.gguse[offset->padid] & 4) {
        SetAnten(offset, 1);
        offset->Localid = 1;
        EndtoAction(offset, 0x60);
    } else {
        EndtoAction(offset, 0x8c);
    }
}

void CHAct_SHW2_RANBU_BB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x8c);
    SetLandBreakActno(offset, 0x57);
}

void CHAct_SHW2_RANBU_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x8d);
    
}

void CHAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetWinActFlag(offset);
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 0x58);
    EndtoDemo(offset);
}

void CHAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x59);
    EndtoDemo(offset);
}

void CHAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x5a);
    EndtoDemo(offset);
}

void CHAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

void CHAct_FK_First(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x47);
    EndtoAction(offset, 0x6e);
}

void CHAct_FK_Sky(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x6e);
    SetLandBreakActno(offset, 0x6f);
}

void CHAct_FK_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, 0x6f);
    EndtoSit(offset);
}

void CHAct_Sakugan(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    if (SetChangeFrameAnime(offset, 0x70)) {
        offset->ply->inertia = 0;
        offset->xspeed /= 2;
    }
    SetLandBreakActno(offset, 0x15);
}

void CHAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x78);
    EndtoStand(offset);
}

void CHAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst &= ~0x400;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x96);
    EndtoStand(offset);
}

extern TRAINING_SETTING *tr_info;

void CHAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x97)) {
        offset->ply->OverKillFlag = 2;
        offset->ColliFlag = 0;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoAction(offset, 0x98);
}

static void CHITIGEKIClearAttack(CHARACTER_WORK *offset) {
    offset->actst |= 0x40;
}

static void CHITIGEKISetNext(CHARACTER_WORK *offset) {
    SetActNoNext(offset, 13);
}

static void CHITIGEKI_SetEnemyPos(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    int d;

    enemy->actst |= 0x400000;
    d = offset->posx - enemy->posx;
    if (abs(d) < 500) {
        enemy->posx = offset->posx;
        enemy->xspeed = 0;
    } else {
        if (d < 0) {
            enemy->xspeed = -500;
        } else {
            enemy->xspeed = 500;
        }
    }
    d = offset->posy - enemy->posy;
    if (abs(d) < 500) {
        enemy->posy = offset->posy;
        enemy->yspeed = 0;
    } else {
        if (d < 0) {
            enemy->yspeed = -500;
        } else {
            enemy->yspeed = 500;
        }
    }
    enemy->HitStopTime = 0;
    enemy->ply->DownTimer = -1;
    enemy->gravity = 0;
}

void CHITIGEKIClearAttack(CHARACTER_WORK *offset);
void CHITIGEKISetNext(CHARACTER_WORK *offset);
void CHITIGEKI_SetEnemyPos(CHARACTER_WORK *offset);

void CHAct_ITIGEKIJump(CHARACTER_WORK *offset) {
    COLLISION *gcl;
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x98)) {
        offset->actst |= 0x100000;
    }
    JumpControl(offset);
    gcl = Get_GCLEx(offset);
    if (gcl) {
        if (offset->dirflag == 1) {
            offset->ply->PersonalWork[2].l = offset->posx - gcl->x * 100;
        } else {
            offset->ply->PersonalWork[2].l = offset->posx + gcl->x * 100;
        }
        offset->ply->PersonalWork[3].l = offset->posy + gcl->y * 100;
    }
    if (!SearchObjectWorkActno(offset, 0x3b, 7) && !SearchObjectWorkActno(offset, 0x3b, 13)) {
        if (offset->attackst & 0x80000) {
            enemy = GetEnemyPlayer(offset);
            if (enemy->xspeed == 0 && enemy->yspeed == 0) {
                EndtoAction(offset, 0x9b);
            }
        } else {
            EndtoAction(offset, 0x99);
        }
    }
    if (offset->attackst & 0x80000) {
        CHITIGEKI_SetEnemyPos(offset);
        SearchObjectWorkActnowithFuncExec(offset, 0x3b, 7, CHITIGEKIClearAttack);
    } else {
        if (IsActTimeOver(offset, 190)) {
            SearchObjectWorkActnowithFuncExec(offset, 0x3b, 7, CHITIGEKIClearAttack);
        }
        if (IsActTimeOver(offset, 192)) {
            SearchObjectWorkActnowithFuncExec(offset, 0x3b, 7, CHITIGEKISetNext);
        }
    }
}

void CHAct_ITIGEKIMissDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x99);
    SetLandBreakActno(offset, 0x9a);
}

void CHAct_ITIGEKIMissLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x9a);
}

void CHAct_ITIGEKIDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 0);
    offset->actst |= 0x100000;
    SetChangeFrameAnime(offset, 0x9b);
    SetLandBreakActno(offset, 0x9c);
}

void CHAct_ITIGEKILand(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    offset->CommandFlag = 0xc05f;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x9c);
    if (offset->Mark) {
        arg.flag = 1;
        arg.x = offset->ply->PersonalWork[2].l;
        arg.y = offset->ply->PersonalWork[3].l;
        ObjectCreate_ActNo(offset, 0x3b, &arg, 12);
        offset->Mark = 0;
    }
}

void CHAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0xa2)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int CHHWXCommandCheck(CHARACTER_WORK *offset) {
    if (offset->ply->PersonalWork[0].ub[1] == 0) {
        return 0;
    }
    if (!(offset->actst & 0x10)) {
        if (IsTrgPunch(offset)) {
            if (offset->Localid == 0) {
                offset->ply->PersonalWork[0].ub[1]--;
            }
            offset->Localid = 1;
            return 1;
        }
        if (IsTrgSlash(offset)) {
            if (offset->Localid == 0) {
                offset->ply->PersonalWork[0].ub[1]--;
            }
            offset->Localid = 2;
            return 1;
        }
        if (IsTrgHighSlash(offset)) {
            if (offset->Localid == 0) {
                offset->ply->PersonalWork[0].ub[1]--;
            }
            offset->Localid = 3;
            return 1;
        }
    } else {
        if (IsTrgPunch(offset)) {
            if (offset->Localid == 0) {
                offset->ply->PersonalWork[0].ub[1]--;
            }
            offset->Localid = 4;
            return 1;
        }
        if (IsTrgSlash(offset)) {
            if (offset->Localid == 0) {
                offset->ply->PersonalWork[0].ub[1]--;
            }
            offset->Localid = 5;
            return 1;
        }
    }
    return 0;
}

int CHHWXEndCheck(CHARACTER_WORK *offset) {
    switch (offset->Localid) {
    case 0:
        break;
    case 1:
        return 0xd1;
    case 2:
        return 0xc9;
    case 3:
        return 0xcc;
    case 4:
        return 0xd3;
    case 5:
        return 0xce;
    }
    return 0;
}

void CHAct_HW1X_AB(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    CREATE_ARG arg;

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    if (SetChangeFrameAnime(offset, 0xd0)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].b[1] = 2;
    }
    if (CHHWXEndCheck(offset)) {
        if (EndtoAction(offset, CHHWXEndCheck(offset))) {
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        }
    } else {
        EndtoAction(offset, 0x72);
    }
    if (!(offset->ActTimer & 3) && offset->xspeed != 0 && offset->HitStopTime == 0) {
        arg.flag = 5;
        arg.actno = 1;
        arg.x = offset->posx;
        arg.y = 0;
        EffectCreate(offset, 0x61, &arg);
    }
}

void CHAct_HW1X_AB2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    CREATE_ARG arg;

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    if (SetChangeFrameAnime(offset, 0xd1)) {
        offset->Localid = 0;
    }
    if (CHHWXEndCheck(offset)) {
        if (EndtoAction(offset, CHHWXEndCheck(offset))) {
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        }
    } else {
        EndtoAction(offset, 0x72);
    }
    if (!(offset->ActTimer & 3) && offset->xspeed != 0 && offset->HitStopTime == 0) {
        arg.flag = 5;
        arg.actno = 1;
        arg.x = offset->posx;
        arg.y = 0;
        EffectCreate(offset, 0x61, &arg);
    }
}

void CHAct_HW1X_SAB(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    if (SetChangeFrameAnime(offset, 0xd2)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].b[1] = 2;
    }
    offset->ply->JumpFlag |= 4;
    if (CHHWXEndCheck(offset)) {
        if (EndtoAction(offset, CHHWXEndCheck(offset))) {
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        }
    } else {
        EndtoAction(offset, 0x86);
    }
}

void CHAct_HW1X_SAB2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    if (SetChangeFrameAnime(offset, 0xd3)) {
        offset->Localid = 0;
    }
    offset->ply->JumpFlag |= 4;
    if (CHHWXEndCheck(offset)) {
        if (EndtoAction(offset, CHHWXEndCheck(offset))) {
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        }
    } else {
        EndtoAction(offset, 0x86);
    }
}

void CHAct_HW2X_BB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0xc8)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].b[1] = 2;
    }
    EndtoAction(offset, 0xca);
}

void CHAct_HW2X_BB2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0xc9)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0xca);
}

void CHAct_HW2X_BBSky(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    SetChangeFrameAnime(offset, 0xca);
    SetJumpControl(offset);
    if (CHHWXEndCheck(offset)) {
        if (EndtoAction(offset, CHHWXEndCheck(offset))) {
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        }
    } else {
        EndtoAction(offset, 0x10056);
    }
}

void CHAct_HW2X_SBB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0xcd)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].b[1] = 2;
    }
    EndtoAction(offset, 0xcf);
}

void CHAct_HW2X_SBB2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 0xce)) {
        offset->Localid = 0;
    }
    EndtoAction(offset, 0xcf);
}

void CHAct_HW2X_SBBSky(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    SetChangeFrameAnime(offset, 0xcf);
    SetJumpControl(offset);
    if (CHHWXEndCheck(offset)) {
        if (EndtoAction(offset, CHHWXEndCheck(offset))) {
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        }
    } else {
        EndtoAction(offset, 0x1006c);
    }
}

void CHAct_HW3X_CB(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    if (SetChangeFrameAnime(offset, 0xcb)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].b[1] = 2;
    }
    TimetoAction(offset, 0x69, 60);
    if (offset->Mark && CHHWXEndCheck(offset)) {
        SetActNoNext(offset, CHHWXEndCheck(offset));
        if (offset->dirflag == 1 && offset->posx > enemy->posx) {
            offset->dirflag = 0;
        }
        if (offset->dirflag == 0 && offset->posx < enemy->posx) {
            offset->dirflag = 1;
        }
    }
}

void CHAct_HW3X_CB2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->ply->ExCommandFunc = CHHWXCommandCheck;
    if (SetChangeFrameAnime(offset, 0xcc)) {
        offset->Localid = 0;
    }
    TimetoAction(offset, 0x69, 60);
    if (offset->Mark) {
        if (CHHWXEndCheck(offset)) {
            SetActNoNext(offset, CHHWXEndCheck(offset));
            if (offset->dirflag == 1 && offset->posx > enemy->posx) {
                offset->dirflag = 0;
            }
            if (offset->dirflag == 0 && offset->posx < enemy->posx) {
                offset->dirflag = 1;
            }
        } else if (offset->ply->PersonalWork[0].ub[1] == 0) {
            SetActNoNext(offset, 0x69);
        }
    }
}
