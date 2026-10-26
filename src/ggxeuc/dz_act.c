#include "generated/dz_act.h"

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void DZ_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x02080050);
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void DZ_RespectCommnadCheck(CHARACTER_WORK *offset) {
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

int DZ_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x7A);
}

int DZ_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void DZAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x7A)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void DZAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void DZAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void DZAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void DZAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x5AU);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void DZAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x5DU);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void DZAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54U);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void DZAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW1_IRAY(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW1_IRAYGG(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x97);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW2_TAXI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x5F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW3_Sword(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW4_ICE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x57);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW5_BitP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    if (SetChangeFrameAnime(offset, 0x56) != 0) {
        offset->ply->PersonalWork[0].b[0] = 1;
        offset->ply->PersonalWork[0].b[1] = 1;
        offset->ply->PersonalWork[0].b[2] = 1;
        offset->ply->PersonalWork[0].b[3] = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW5_BitK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    if (SetChangeFrameAnime(offset, 0x94) != 0) {
        offset->ply->PersonalWork[0].b[0] = 2;
        offset->ply->PersonalWork[0].b[1] = 2;
        offset->ply->PersonalWork[0].b[2] = 2;
        offset->ply->PersonalWork[0].b[3] = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW5_BitS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    if (SetChangeFrameAnime(offset, 0x95) != 0) {
        offset->ply->PersonalWork[0].b[0] = 5;
        offset->ply->PersonalWork[0].b[1] = 5;
        offset->ply->PersonalWork[0].b[2] = 0;
        offset->ply->PersonalWork[0].b[3] = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_HW5_BitHS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    if (SetChangeFrameAnime(offset, 0x94) != 0) {
        offset->ply->PersonalWork[0].b[0] = 1;
        offset->ply->PersonalWork[0].b[1] = 4;
        offset->ply->PersonalWork[0].b[2] = 0;
        offset->ply->PersonalWork[0].b[3] = 0;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_SHW1_GANMARAY(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x60);
    EndtoAction(offset, 0x81);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_SHW1_GANMARAYEnd(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x81);
    EndtoAction(offset, 0x82);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void DZAct_SHW1_GANMARAYEndSky(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x82);
    SetLandBreakActno(offset, 0x4E);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_SHW2_DI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x61);
    EndtoStand(offset);
    if (offset->attackst & 0x80000) {
        SetActNoNext(offset, 0x62);
        return;
    }
    EndtoStand(offset);
}

void DZ_NecroLaserBoot_Proc(CHARACTER_WORK *offset) {
    offset->Localid |= 0x200;
}

int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetSearchExecProcAddr(void *addr);
/* DZ_NecroLaserBoot_Proc: no debug record */
void DZ_NecroLaserBoot_Proc(CHARACTER_WORK *offset);

void DZAct_SHW2_NEC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    OrScreenFlag(0x4004);
    SetChangeFrameAnime(offset, 0x62);
    if (offset->dirflag == 1) {
        if (offset->posx > 32000) {
            offset->xspeed = -800;
            return;
        }
        offset->xspeed = 0;
    } else {
        if (offset->posx < -32000) {
            offset->xspeed = 800;
            return;
        }
        offset->xspeed = 0;
    }
    if (offset->Mark) {
        SetSearchExecProcAddr(DZ_NecroLaserBoot_Proc);
        SearchCharacterExecute(offset, 0x43, 0x2E, 0x27);
    }
    EndtoStand(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_EntryA_1(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x5B)) {
        c_arg.flag = 0x104;
        c_arg.actno = 0xF;
        EffectCreate(offset, 0x80, &c_arg);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x73);
    EndtoStand(offset);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void PlayerThrowSetSub(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int DZ_InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);

    if (aoffset->ply == 0) {
        offset->GuardSt &= ~3;
        return 0;
    }
    SetActNoNext(offset, 0x9C);
    if (eoffset->actst & 0x10) {
        SetActNoNext(eoffset, 0x2F);
    } else {
        SetActNoNext(eoffset, 0x27);
    }
    offset->HitStopTime = 0;
    offset->Mark &= ~1;
    eoffset->HitStopTime = 60;
    PlayerThrowSetSub(offset);
    return 1;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int DZ_InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void DZAct_SHW2_TENCHI_Wait(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9B);
    if (offset->Mark != 0) {
        offset->ply->GuardFunc = DZ_InterruptProc;
        offset->GuardSt = 0xA03;
    } else {
        offset->GuardSt = 0;
        offset->ply->GuardFunc = 0;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void DZAct_SHW2_TENCHI_Start(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    eoffset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9C);
    eoffset->HitStopTime = 0x3C;
    EndtoAction(offset, 0x9D);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetForceStopTime(short int t1p, short int t2p);

void DZAct_SHW2_TENCHI_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    eoffset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x9D) != 0) {
        SetForceStopTime(-1, -1);
        eoffset->HitStopTime = 0x24;
    }
    EndtoAction(offset, 0x9E);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int sound_request(int no);

void DZAct_SHW2_TENCHI_2(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    eoffset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x9E)) {
        c_arg.flag = 2;
        c_arg.y = -8000;
        c_arg.x = 0;
        EffectCreate_ActNo(offset, 0x77, &c_arg, 1);
        EffectCreate_ActNo(offset, 0x77, &c_arg, 2);
        EffectCreate_ActNo(offset, 0x6F, &c_arg, 9);
        sound_request(0x32);
    }
    if (offset->ply->ObjFlag & 8) {
        eoffset->HitStopTime = 30;
    }
    EndtoAction(offset, 0x9F);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetSearchExecProcAddr(void *addr);
int sound_request(int no);

void DZ_NecroLaserBoot_Proc(CHARACTER_WORK *offset);

void DZAct_SHW2_TENCHI_3(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    SetChangeFrameAnime(offset, 0x9F);
    if (offset->ActTimer == 16 && offset->HitStopTime == 0) {
        c_arg.flag = 2;
        c_arg.y = -8000;
        c_arg.x = 0;
        EffectCreate_ActNo(offset, 0x77, &c_arg, 1);
        EffectCreate_ActNo(offset, 0x77, &c_arg, 2);
        EffectCreate_ActNo(offset, 0x6F, &c_arg, 9);
        sound_request(0x32);
    }
    if (offset->dirflag == 1) {
        if (offset->posx > 32000) {
            offset->xspeed = -3200;
            return;
        }
        offset->xspeed = 0;
    } else {
        if (offset->posx < -32000) {
            offset->xspeed = 3200;
            return;
        }
        offset->xspeed = 0;
    }
    if (offset->Mark) {
        SetSearchExecProcAddr(DZ_NecroLaserBoot_Proc);
        SearchCharacterExecute(offset, 0x43, 0x2E, 0x27);
    }
    EndtoStand(offset);
}

extern USER_SETTING userinfo;
int IsFireWheel(CHARACTER_WORK *offset);
int IsGRAY(void);
#ifdef GGX_PORT
/* DZ_SpecialAttackCheck calls IsGRAY(CHARACTER_WORK *) with no arguments; offset is still in a0.
   This TU declares it (void), so the call goes through a twin in platforms/common/core/abi_glue.c. */
int ggx_port_IsGRAY(CHARACTER_WORK *offset);
#define IsGRAY() ggx_port_IsGRAY(offset)
#endif
int IsGyakuSyoryu(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHard1(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
short int GetTensionVal(CHARACTER_WORK *offset);
int DZ_SpecialAttackCheck(CHARACTER_WORK *offset)
{
    int boss;
    if (!(offset->actst & 0x10)) {
        if (IsGRAY() && GetTensionVal(offset) >= 10000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x02080060);
            return 1;
        }
        if (IsHard1(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffPunch(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x0208009B);
                return 1;
            }
            SetAttackActNo(offset, 0x02080061);
            return 1;
        }
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                SetAttackActNo(offset, 0x02080097);
                return 1;
            }
            SetAttackActNo(offset, 0x0208005E);
            return 1;
        }
        if ((~offset->ply->ObjFlag & 0x10) && IsHadou(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080055);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080057);
                return 1;
            }
        }
        if ((~offset->ply->ObjFlag & 8) && IsGyakuSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x0208005F);
            return 1;
        }
        if ((~offset->ply->ObjFlag & 2) && IsTatsumaki(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x02080056);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x02080094);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080095);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080096);
                return 1;
            }
        }
        if (g_info.boss[offset->padid] == 1) {
            if (IsHadou(offset)) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080055);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02080057);
                    return 1;
                }
            }
            if (IsGyakuSyoryu(offset) && IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x0208005F);
                return 1;
            }
        }
    }
    boss = g_info.boss[offset->padid];
    if (boss == 1) {
        if (IsTrgAndOffPunch(offset)) {
            offset->ply->PersonalWork[0].b[0] = 1;
            offset->ply->PersonalWork[0].b[1] = 1;
            offset->ply->PersonalWork[0].b[2] = 1;
            offset->ply->PersonalWork[0].b[3] = 0;
        } else if (IsTrgAndOffKick(offset)) {
            offset->ply->PersonalWork[0].b[0] = 2;
            offset->ply->PersonalWork[0].b[1] = 2;
            offset->ply->PersonalWork[0].b[2] = 2;
            offset->ply->PersonalWork[0].b[3] = 0;
        } else if (IsTrgAndOffSlash(offset)) {
            offset->ply->PersonalWork[0].b[0] = 5;
            offset->ply->PersonalWork[0].b[1] = 5;
            offset->ply->PersonalWork[0].b[2] = 0;
            offset->ply->PersonalWork[0].b[3] = 0;
        } else if (IsTrgAndOffHighSlash(offset)) {
            offset->ply->PersonalWork[0].b[0] = 1;
            offset->ply->PersonalWork[0].b[1] = 4;
            offset->ply->PersonalWork[0].b[2] = 0;
            offset->ply->PersonalWork[0].b[3] = 0;
        }
    }
    return 0;
}
