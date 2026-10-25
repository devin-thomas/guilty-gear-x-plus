#include "generated/ve_act.h"

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDownTAMEUp45(CHARACTER_WORK *offset);
int IsGeiza(CHARACTER_WORK *offset);
int IsGyakuYoga(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsLeftTAMERight45(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int VE_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsGeiza(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x04080070);
            return 1;
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffKick(offset) && !(userinfo.gguse[offset->padid] & 4)) {
                SetAttackActNo(offset, 0x0208005B);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080061);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080062);
                return 1;
            }
        }
        if (IsHadou(offset) && IsTrgAndOffKick(offset) && (userinfo.gguse[offset->padid] & 4)) {
            SetAttackActNo(offset, 0x02080054);
            return 1;
        }
        if (IsDownTAMEUp45(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080053);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080099);
                return 1;
            }
        }
        if (IsLeftTAMERight45(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080060);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080097);
                return 1;
            }
        }
        if (IsGyakuYoga(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x0208005A);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x02080065);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080066);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080067);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x020900A6);
            return 1;
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02090058);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02090072);
                return 1;
            }
            if (IsTrgAndOffKick(offset) && (userinfo.gguse[offset->padid] & 4)) {
                SetAttackActNo(offset, 0x02090054);
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

void VE_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
    } else {
        SetAttackActNo(offset, 0x0208009B);
    }
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void VE_RespectCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x02080051);
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int VE_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x71);
}

int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);

int VE_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x6C, 0x69)) {
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void VEAct_LongHair1(CHARACTER_WORK *offset) {
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

void VEAct_LongHair3(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x9B)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void VEAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void VEAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x64);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void VEAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void VEAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int GetXMove(CHARACTER_WORK *offset, int val);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);

void SetVEBallTarget(CHARACTER_WORK *offset, int *tb) {
    int i;
    int ano;
    CHARACTER_WORK *offobj;

    ano = 6;
    i = 4;
    while (i--) {
        offobj = SearchObjectWorkActno(offset, 0x22, ano);
        if (offobj) {
            offobj->HitBackx = offset->posx + GetXMove(offset, tb[0]);
            offobj->RHitBackx = offset->posy + tb[1];
        }
        tb += 2;
        ano++;
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetVEBallTarget(CHARACTER_WORK *offset, int *tb);

void VEAct_HW1_BALLTHROW(CHARACTER_WORK *offset) {
    int postb[8] = { 0x2ee0,-0x2710,0x5dc0,-0x3a98,0x8ca0,-0x4e20,0xbb80,-0x61a8 };

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    SetVEBallTarget(offset, postb);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetVEBallTarget(CHARACTER_WORK *offset, int *tb);

void VEAct_HW1_BALLTHROW2(CHARACTER_WORK *offset) {
    int postb[8] = { 0x2710,-0x4650,0x4650,-0x6590,0x6590,-0x4650,0x4650,-0x2710 };

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x65);
    SetVEBallTarget(offset, postb);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetVEBallTarget(CHARACTER_WORK *offset, int *tb);

void VEAct_HW1_BALLTHROW3(CHARACTER_WORK *offset) {
    int postb[8] = { 0x1f40,-0x2710,0x3e80,-0x55f0,0x5dc0,-0x36b0,0x7530,-0x1770 };

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x66);
    SetVEBallTarget(offset, postb);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetVEBallTarget(CHARACTER_WORK *offset, int *tb);

void VEAct_HW1_BALLTHROW4(CHARACTER_WORK *offset) {
    int postb[8] = { 0x2ee0,-0x7530,0x4e20,-0x61a8,0x6d60,-0x4e20,0x8ca0,-0x3a98 };

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    SetVEBallTarget(offset, postb);
    EndtoStand(offset);
}

int IsActEnd(CHARACTER_WORK *offset);
int IsHoldSlash(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;

void VEAct_HW2_TUKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x60) && g_info.boss[offset->padid] != 1) {
        offset->ply->PersonalWork[0].b[1] = 0;
    }
    if (IsActEnd(offset)) {
        if (IsHoldSlash(offset) && offset->ply->PersonalWork[0].b[1] >= 0) {
            offset->ply->PersonalWork[0].b[1]++;
        } else {
            if (g_info.boss[offset->padid] == 1) {
                offset->ply->PersonalWork[0].b[1] = 0;
            }
            SetActNoNext(offset, 0x6D);
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;

void VEAct_HW2_TUKISHOOT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6D);
    if (g_info.boss[offset->padid] == 1) {
        offset->ply->PersonalWork[0].b[1] = 0;
    }
    EndtoStand(offset);
}

int IsActEnd(CHARACTER_WORK *offset);
int IsHoldHighSlash(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;

void VEAct_HW2_TUKI2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x97) && g_info.boss[offset->padid] != 1) {
        offset->ply->PersonalWork[0].b[1] = 0;
    }
    if (IsActEnd(offset)) {
        if (IsHoldHighSlash(offset) && offset->ply->PersonalWork[0].b[1] >= 0) {
            offset->ply->PersonalWork[0].b[1]++;
        } else {
            if (g_info.boss[offset->padid] == 1) {
                offset->ply->PersonalWork[0].b[1] = 0;
            }
            SetActNoNext(offset, 0x98);
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;

void VEAct_HW2_TUKISHOOT2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x98);
    if (g_info.boss[offset->padid] == 1) {
        offset->ply->PersonalWork[0].b[1] = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW3_MASSE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW3_MASSE2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x99);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void VEAct_HW4_SUMMER_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x54);
    SetJumpControl(offset);
    EndtoAction(offset, 0x55);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW4_SUMMER_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (!SetChangeFrameAnime(offset, 0x55) && offset->yspeed > -900) {
        SetActNoNext(offset, 0x10056);
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void VEAct_HW4_SUMMER_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56);
    SetLandBreakActno(offset, 0x57);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW4_SUMMER_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW5_WARP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoAction(offset, 0x68);
}

int EndtoDrop(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchBall(CHARACTER_WORK *off);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void VEAct_HW5_APPEAR(CHARACTER_WORK *offset) {
    CHARACTER_WORK *off;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x68)) {
        off = SearchBall(offset);
        if (off) {
            offset->posx = off->posx;
            offset->posy = off->posy;
        } else {
            offset->posy -= 30000;
        }
    }
    SetJumpControl(offset);
    EndtoDrop(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW6_TG_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x61);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetThrowDist(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW6_TG_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62);
    if (!EndtoAction(offset, 0x63) && offset->Mark && GetThrowDist() < 18000) {
        SetActNoNext(offset, 0x63);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW6_TG_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x63);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void VEAct_HW7_DIVE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
    SetLandBreakActno(offset, 0x74);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void VEAct_HW7_DIVE_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    SetLandBreakActno(offset, 0x73);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void VEAct_HW7_DIVE2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x72);
    SetLandBreakActno(offset, 0x73);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW7_DIVE2_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x73);
    EndtoSit(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_HW7_DIVE_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x74);
    EndtoSit(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5F);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6E);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x69);
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int VE_ITIGEKI_InterruptProc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    short unsigned int VEITIGEKIDamageNoTb[19] = {
        0x00, 0xC0, 0xA1, 0xA8, 0xA2, 0xA1, 0xA7, 0xBE, 0x9A, 0x9C,
        0xB8, 0x8B, 0xB0, 0xA4, 0xA3, 0xA2, 0x8E, 0x93, 0x93,
    };

    SetActNoNext(enemy, VEITIGEKIDamageNoTb[enemy->idno]);
    enemy->ply->MutekiTime = 10;
    enemy->Localid = 0;
    enemy->DamColliFlag = 0;
    enemy->actst |= 0x20000;
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int VE_ITIGEKI_InterruptProc(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void VEAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->AtkHitInterrupt = VE_ITIGEKI_InterruptProc;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x6C)) {
        offset->ColliFlag = 0;
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        if (!offset->Mark) {
            SetActNoNext(offset, 0xA1);
        }
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsActEnd(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_ITIGEKI2(CHARACTER_WORK *offset) {
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0xA1);
    if (IsActEnd(offset)) {
        if (offset->dirflag == 1) {
            if (offset->posx > 32000) {
                SetActNoNext(offset, 0x6A);
                return;
            }
        } else {
            if (offset->posx < -32000) {
                SetActNoNext(offset, 0x6A);
                return;
            }
        }
    }
    EndtoAction(offset, 0x6B);
}

void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void VEAct_ITIGEKI_BackStep(CHARACTER_WORK *offset) {
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6A);
    SetLandBreakActno(offset, 0x6B);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int GetXMove(CHARACTER_WORK *offset, int val);
int IsActEnd(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_ITIGEKI_Wait(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x6B)) {
        GetEnemyPlayer(offset)->ply->DownTimer = -1;
        offset->ply->PersonalWork[1].w[0] = (offset->posx + GetXMove(offset, 40000)) / 100;
        offset->ply->PersonalWork[1].w[1] = (offset->posy - 15000) / 100;
    }
    if (IsActEnd(offset)) {
        eoffset = GetEnemyPlayer(offset);
        if (eoffset->xspeed == 0 && eoffset->yspeed == 0 && eoffset->Localid) {
            SetActNoNext(offset, 0xA5);
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_ITIGEKI_Shot(CHARACTER_WORK *offset) {
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0xA5);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_SHW1_SHOOT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x70);
    EndtoStand(offset);
}

void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
extern GAME_INFOMATION g_info;

void VEAct_SHW2_SKYSHOT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0xA6)) {
        if (g_info.boss[offset->padid] == 1) {
            offset->ply->PersonalWork[0].b[1] = 0x30;
        } else {
            offset->ply->PersonalWork[0].b[1] = 0;
        }
    }
    if (g_info.boss[offset->padid] == 1) {
        if (offset->yspeed < 0) {
            offset->ply->PersonalWork[0].b[1] = 0x30;
        } else {
            offset->ply->PersonalWork[0].b[1] = 0;
        }
    }
    SetLandBreakActno(offset, 0xA7);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_SHW2_SKYSHOTLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA7);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void VEAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x71)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void VEAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x400) | 0x40;
    SetChangeFrameAnime(offset, 0x75);
    EndtoStand(offset);
}
