#include "generated/po_act.h"

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsLeftTAMERight(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int PO_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x0408006B);
            return 1;
        }
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x04080082);
            return 1;
        }
        if (IsSyoryu(offset) && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x02080072);
            return 1;
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x02080056);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080055);
                return 1;
            }
        }
        if (IsLeftTAMERight(offset) && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x02080078);
            return 1;
        }
        if (IsTatsumaki(offset) && IsTrgAndOffPunch(offset)) {
            SetAttackActNo(offset, 0x02080059);
            return 1;
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void PO_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080050);
    } else {
        SetAttackActNo(offset, 0x020800A0);
    }
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void PO_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080052);
    } else {
        SetAttackActNo(offset, 0x020800A1);
    }
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int PO_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x94);
}

int IsFireWheel(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);

int PO_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10) && IsFireWheel(offset) && IsTrgAndOffPunch(offset)) {
        return 3;
    }
    return 0;
}

int IsFireWheel(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int PO_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && IsTrgAndOffPunch(offset)) {
            SetAttackActNo(offset, 0x0208007A);
            return 1;
        }
        if (ItigekiCommandCheck(offset, functb, 0x96, 0x95)) {
            return 1;
        }
    }
    return 0;
}

int IsGyakuYoga(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int POHW2ExCommandCheck(CHARACTER_WORK *offset) {
    if (IsGyakuYoga(offset) && IsTrgAndOffHighSlash(offset)) {
        offset->Localid = 1;
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void POAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void POAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void POAct_LongHair3(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0xA0) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void POAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0xA1);
    EndtoStand(offset);
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void POAct_EntryA_1(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 0x51)) {
        SetXSpeed(offset, -50000);
        SetYSpeed(offset, -50000, 0);
    } else {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
    }
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void POAct_EntryA_2(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetXSpeed(offset, 2000);
    SetYSpeed(offset, 2000, 0);
    SetChangeFrameAnime(offset, 0x53);
    SetLandBreakActno(offset, 0x68);
}

int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_EntryA_3(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x68);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x69);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6A);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & 0xFFFFFBFF) | 0x40;
    SetChangeFrameAnime(offset, 0x54);
    EndtoStand(offset);
}

int PlayerDamage(CHARACTER_WORK *offset, int dam, TACTHEADER *ahp);
void RequestHit(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);

int POPowerArmor(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    if (header->flag & 0x04000000) {
        return 0;
    }
    offset->DamParam->FaintPoint = 0;
    header->flag &= ~0x40000;
    header->flag &= ~0x80000;
    if (PlayerDamage(offset, header->damage >> 1, header)) {
        return 0;
    }
    RequestHit(offset, header->flag, header->lvflag, cnt);
    offset->GuardSt &= ~0x400;
    return 1;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetThrowDist(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int POPowerArmor(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void POAct_HW1_CHARGE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->GuardFunc = POPowerArmor;
    if (SetChangeFrameAnime(offset, 0x78)) {
        offset->GuardSt |= 0x400;
    }
    if (offset->Mark && GetThrowDist() < 17000) {
        SetActNoNext(offset, 0x08000079);
    }
    EndtoAction(offset, 0x08000079);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int POPowerArmor(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void POAct_HW1_ATTACK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->GuardFunc = POPowerArmor;
    SetChangeFrameAnime(offset, 0x79);
}

COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);

void POTAIKU_SetEnemyPos(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;
    COLLISION *colli;

    eoffset = offset->ply->Enemy;
    colli = Get_GCLExNo(offset, 1);
    if (offset->Mark == 0 && colli != 0) {
        eoffset->actst |= 0x400000;
        eoffset->yspeed = -250;
        eoffset->gravity = 25;
        eoffset->ply->DownTimer = -1;
        eoffset->CommandFlag &= ~0x800;
        eoffset->xspeed = 0;
        if (offset->dirflag == 1) {
            eoffset->posx = offset->posx - colli->x * 100;
        } else {
            eoffset->posx = offset->posx + colli->x * 100;
        }
        eoffset->posy = offset->posy + (colli->y + 160) * 100;
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void POTAIKU_SetEnemyPos(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW2_TAIKU(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x72);
    if (offset->attackst & 0x80000) {
        POTAIKU_SetEnemyPos(offset);
        EndtoAction(offset, 0x74);
    } else {
        EndtoAction(offset, 0x73);
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW2_TAIKUMiss(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x73);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
short int GetTensionVal(CHARACTER_WORK *offset);
void POTAIKU_SetEnemyPos(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int POHW2ExCommandCheck(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;

void POAct_HW2_TAIKU1ST(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = POHW2ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x74)) {
        offset->Localid = 0;
    }
    POTAIKU_SetEnemyPos(offset);
    if (offset->Localid) {
        EndtoAction(offset, 0x76);
    } else if (g_info.boss[offset->padid] == 1) {
        if (GetTensionVal(offset) >= 5000) {
            EndtoAction(offset, 0x04080082);
        } else {
            EndtoAction(offset, 0x75);
        }
    } else {
        EndtoAction(offset, 0x75);
    }
}

void POTAIKU_SetEnemyPos(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW2_TAIKU2ND(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x75);
    if (offset->Mark == 0) {
        POTAIKU_SetEnemyPos(offset);
    }
}

void POTAIKU_SetEnemyPos(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW2_TAIKU2NDMOVE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x76);
    POTAIKU_SetEnemyPos(offset);
}

void POTAIKU_SetEnemyPos(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW2_TAIKU2NDDERIVE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x77);
    POTAIKU_SetEnemyPos(offset);
}

void SetPOEXThrowDamageActNoLand(CHARACTER_WORK *offset);

int PO_PB_LandInterruptProc(CHARACTER_WORK *offset) {
    SetPOEXThrowDamageActNoLand(offset->ply->Enemy);
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW3_PB_Miss(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7A);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetPOEXThrowDamageActNoJump(CHARACTER_WORK *offset);

void POAct_HW3_PB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (offset->padid == 0) {
        OrScreenFlag(0xD);
    } else {
        OrScreenFlag(0xE);
    }
    SetChangeFrameAnime(offset, 0x7B);
    offset->actst |= 0x100000;
    if (EndtoAction(offset, 0x7C)) {
        SetPOEXThrowDamageActNoJump(offset->ply->Enemy);
    }
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int PO_PB_LandInterruptProc(CHARACTER_WORK *offset);

void POAct_HW3_PB_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(4);
    SetChangeFrameAnime(offset, 0x7C);
    offset->actst |= 0x100000;
    SetLandBreakActno(offset, 0x7D);
    offset->ply->LandInterrupt = PO_PB_LandInterruptProc;
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HW3_PB_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7D);
    if (offset->Mark && offset->ActTimer % 6 == 0) {
        EffectCreate_ActNo(offset, 0x70, 0, 1);
    }
    offset->actst |= 0x100000;
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_Hissatu_5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HISSATU_6_a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56);
    EndtoAction(offset, 0x57);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void POAct_HISSATU_6_b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    SetLandBreakActno(offset, 0x10058);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HISSATU_6_c(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HISSATU_6_a2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoAction(offset, 0x5A);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void POAct_HISSATU_6_b2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    SetLandBreakActno(offset, 0x1005B);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_HISSATU_6_c2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW1_SPB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x82);
}

void PlayerThrowSetSub(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetPOEX2ThrowDamageActNo(CHARACTER_WORK *offset);

void PO_SPB_HitInterruptProc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset = offset->ply->Enemy;

    SetPOEX2ThrowDamageActNo(eoffset);
    SetActNo(offset, 0x86);
    offset->HitStopTime = 0;
    eoffset->HitStopTime = 0;
    PlayerThrowSetSub(offset);
}

void SetPOEX2ThrowDamageActNoLand(CHARACTER_WORK *offset);

int PO_SPB_LandInterruptProc(CHARACTER_WORK *offset) {
    SetPOEX2ThrowDamageActNoLand(offset->ply->Enemy);
    return 0;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void PO_SPB_HitInterruptProc(CHARACTER_WORK *offset);

void POAct_SHW1_SPBJump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x83);
    SetJumpControl(offset);
    EndtoAction(offset, 0x10084);
    offset->ply->AtkHitInterrupt = PO_SPB_HitInterruptProc;
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void POAct_SHW1_SPBMissDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x84);
    SetLandBreakActno(offset, 0x85);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW1_SPBMissLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x85);
}

void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetPOEX2ThrowDamageActNoCeilingLand(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;

void POAct_SHW1_SPBHold(CHARACTER_WORK *offset) {
    if (g_info.boss[offset->padid] == 1) {
        if (offset->padid == 0) {
            OrScreenFlag(5);
        } else {
            OrScreenFlag(6);
        }
    }
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x86);
    SetJumpControl(offset);
    OrScreenFlag(4);
    if (offset->posy < -65000) {
        offset->yspeed = 0;
        offset->gravity = 0;
        SetActNoNext(offset, 0x87);
        SetPOEX2ThrowDamageActNoCeilingLand(offset->ply->Enemy);
    }
    offset->actst |= 0x100000;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetPOEX2ThrowDamageActNoDrop(CHARACTER_WORK *offset);

void POAct_SHW1_SPBHoldCeilingLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x87);
    OrScreenFlag(4);
    if (EndtoAction(offset, 0x88)) {
        SetPOEX2ThrowDamageActNoDrop(offset->ply->Enemy);
    }
    offset->actst |= 0x100000;
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int PO_SPB_LandInterruptProc(CHARACTER_WORK *offset);

void POAct_SHW1_SPBHoldDrop(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x88);
    OrScreenFlag(4);
    SetLandBreakActno(offset, 0x8F);
    offset->ply->LandInterrupt = PO_SPB_LandInterruptProc;
    offset->actst |= 0x100000;
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW1_SPBHoldLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8F);
    if (offset->Mark && offset->ActTimer % 6 == 0) {
        EffectCreate_ActNo(offset, 0x70, 0, 1);
    }
}

int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsYogax2(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

int POSHW2ExCommandCheck(CHARACTER_WORK *offset) {
    if (offset->ply->PersonalWork[0].ub[0] && offset->Mark) {
        SetActNo(offset, 0x6D);
    }
    if (IsYogax2(offset) && IsTrgAndOffPunch(offset)) {
        offset->ply->PersonalWork[0].ub[0] = 1;
        if (offset->Mark) {
            SetActNo(offset, 0x6D);
        }
    }
    return 0;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int POSHW2ExCommandCheck(CHARACTER_WORK *offset);

void POAct_SHW2_Punch1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    offset->ply->ExCommandFunc = POSHW2ExCommandCheck;
    if (SetChangeFrameAnime(offset, 0x6B)) {
        offset->ply->PersonalWork[0].ub[0] = 0;
    }
    EndtoAction(offset, 0x6C);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW2_Punch1Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6C);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW2_Punch2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6D);
    EndtoAction(offset, 0x6E);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW2_Punch3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6E);
    EndtoAction(offset, 0x6F);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_SHW2_Punch4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x6F);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void POAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x70);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void POAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x71);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void POAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xA8);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void POAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void POAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x94)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x95);
    EndtoStand(offset);
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void SetPOITIGEKIPunch(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x97);
    offset->ply->MutekiTime = 0xFF;
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetPOITIGEKIPunch(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void POAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x96)) {
        offset->ply->OverKillFlag = 2;
        offset->ply->AtkHitInterrupt = SetPOITIGEKIPunch;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int GetXMove(CHARACTER_WORK *offset, int val);
int IsActEnd(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void POAct_ITIGEKIPunch(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    offset->CommandFlag = 0xC05F;
    eoffset = offset->ply->Enemy;
    if (!IsActEnd(offset)) {
        OrScreenFlag(0x400C);
        SetAnten(offset, 0);
    }
    SetChangeFrameAnime(offset, 0x97);
    if (offset->Mark) {
        eoffset->gravity = 0;
        eoffset->posy = -103000;
        eoffset->yspeed = 4000;
        eoffset->posx = offset->posx + GetXMove(offset, 40000);
        eoffset->xspeed = 0;
        eoffset->ply->DownTimer = -1;
    }
    EndtoDemo(offset);
    offset->actst |= 0x40000000;
}
