#include "generated/jy_act.h"

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsTrgHighSlash(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int JY_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsSyoryu(offset) && IsTrgSlash(offset)) {
            SetAttackActNo(offset, 0x020800A6);
            return 1;
        }
        if (IsHadou(offset)) {
            if (userinfo.gguse[offset->padid] & 4) {
                if (IsTrgSlash(offset)) {
                    offset->Localid = 0x5D;
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
                if (IsTrgPunch(offset)) {
                    offset->Localid = 0x5C;
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
                if (IsTrgKick(offset)) {
                    offset->Localid = 0x62;
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
            } else {
                if (IsTrgSlash(offset)) {
                    offset->Localid = 0x5D;
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
                if (IsTrgPunch(offset)) {
                    offset->Localid = 0x5C;
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
                if (IsTrgKick(offset)) {
                    offset->Localid = 0x62;
                    SetAttackActNo(offset, 0x02080059);
                    return 1;
                }
            }
            if (offset->ply->PersonalWork[1].b[0] < 8 && IsTrgHighSlash(offset)) {
                SetAttackActNo(offset, 0x020800B3);
                return 1;
            }
        }
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x04080078);
            return 1;
        }
        if (IsTatsumaki(offset) && IsTrgPunch(offset) && !(offset->ply->ObjFlag & 1)) {
            SetAttackActNo(offset, 0x02080069);
            return 1;
        }
    }
    if (offset->actst & 0x10) {
        if ((offset->yspeed < 0 || offset->posy < -7500) && IsHadou(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080058);
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);

extern GAME_INFOMATION g_info;

void JY_ExciteCommnadCheck(CHARACTER_WORK *offset) {
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
/* switchdataD_005c9380: no debug record */

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void JY_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    switch (Random() % 5) {
    case 0:
        SetAttackActNo(offset, 0x02080052);
        break;
    case 1:
        SetAttackActNo(offset, 0x02080053);
        break;
    case 2:
        SetAttackActNo(offset, 0x0208009F);
        break;
    case 3:
        SetAttackActNo(offset, 0x020800A0);
        break;
    case 4:
        SetAttackActNo(offset, 0x020800A1);
        break;
    }
}

int IsTrgHighSlash(CHARACTER_WORK *offset);

int JYSP2ExCommandCheck2(CHARACTER_WORK *offset) {
    if (IsTrgHighSlash(offset)) {
        offset->Localid = 0xAB;
    }
    return 0;
}

int IsHoldKick(CHARACTER_WORK *offset);
int IsHoldPunch(CHARACTER_WORK *offset);
int IsHoldSlash(CHARACTER_WORK *offset);
int IsLeft(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
int JYGetIaiLevel(CHARACTER_WORK *offset);
int JYSP2ExCommandCheck2(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int JYSP2ExCommandCheck(CHARACTER_WORK *offset) {
    int lv;

    JYSP2ExCommandCheck2(offset);
    lv = JYGetIaiLevel(offset);
    switch (offset->Localid) {
    case 0x5D:
        if (!IsHoldSlash(offset)) {
            offset->ply->PersonalWork[0].w[0] = 0;
            if (lv == 0) {
                SetAttackActNo(offset, 0x8005D);
            } else if (lv == 1) {
                SetAttackActNo(offset, 0x80067);
            } else {
                SetAttackActNo(offset, 0x80068);
            }
            return 1;
        }
        break;
    case 0x62:
        if (!IsHoldKick(offset)) {
            offset->ply->PersonalWork[0].w[0] = 0;
            if (lv == 0) {
                SetAttackActNo(offset, 0x80062);
            } else if (lv == 1) {
                SetAttackActNo(offset, 0x80065);
            } else {
                SetAttackActNo(offset, 0x80066);
            }
            return 1;
        }
        break;
    case 0x5C:
        if (!IsHoldPunch(offset)) {
            offset->ply->PersonalWork[0].w[0] = 0;
            if (lv == 0) {
                SetAttackActNo(offset, 0x8005C);
            } else if (lv == 1) {
                SetAttackActNo(offset, 0x80063);
            } else {
                SetAttackActNo(offset, 0x80064);
            }
            return 1;
        }
        break;
    default:
        SetAttackActNo(offset, 0xAB);
        return 1;
    }
    if (IsLeft(offset)) {
        SetAttackActNo(offset, 0x61);
        return 1;
    }
    if (IsRight(offset)) {
        SetAttackActNo(offset, 0x5B);
        return 1;
    }
    return 0;
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int JY_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x8C);
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);

int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int JY_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x7C, 0x7B)) {
            return 1;
        }
    }
    return 0;
}

int IsTrgSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int JY_HW7_CmdChk(CHARACTER_WORK *offset) {
    if (IsTrgSlash(offset)) {
        SetAttackActNo(offset, 0x900A8);
    }
    return 0;
}

int JYGetIaiLevel(CHARACTER_WORK *offset) {
    if (offset->ply->PersonalWork[0].w[0] == 0) {
        return 0;
    }
    return offset->ply->PersonalWork[0].w[0] == 1 ? 1 : 2;
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int JYGetIaiLevel(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);

void HW1_LevelChk(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;
    int lv;

    c_arg.flag = 4;
    lv = JYGetIaiLevel(offset);
    if (lv != 0) {
        if (lv == 1) {
            if (!SearchEffectWorkActno(offset, 0x3F, 0x27)) {
                c_arg.actno = 0x27;
                EffectCreate(offset, 0x3F, &c_arg);
            }
        } else {
            if (!SearchEffectWorkActno(offset, 0x3F, 0x28)) {
                c_arg.actno = 0x28;
                EffectCreate(offset, 0x3F, &c_arg);
            }
        }
    }
    if (!SearchEffectWorkActno(offset, 0x3F, 0x29)) {
        EffectCreate_ActNo(offset, 0x3F, 0, 0x29);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void JYAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x8C)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x54);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5F);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8D);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8E);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JYSP2ExCommandCheck2: no debug record */

int JYSP2ExCommandCheck2(CHARACTER_WORK *offset);

void JYAct_SP2_KAM(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    offset->ply->ExCommandFunc = JYSP2ExCommandCheck2;
    if (offset->Localid == 0xAB) {
        EndtoAction(offset, 0xAB);
    } else {
        EndtoAction(offset, 0x5A);
    }
}

void HW1_LevelChk(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JYSP2ExCommandCheck: no debug record */
/* JYSP2ExCommandCheckEX: no debug record */
extern USER_SETTING userinfo;

int JYSP2ExCommandCheck(CHARACTER_WORK *offset);
int JYSP2ExCommandCheckEX(CHARACTER_WORK *offset);
extern int BattleStatus;

void JYAct_SP2_WAT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    if (userinfo.gguse[offset->padid] & 4) {
        offset->ply->ExCommandFunc = JYSP2ExCommandCheckEX;
    } else {
        offset->ply->ExCommandFunc = JYSP2ExCommandCheck;
    }
    if (userinfo.gguse[offset->padid] & 4) {
        if (BattleStatus == 3) {
            SetAttackActNo(offset, 0xAB);
        }
    }
    HW1_LevelChk(offset);
}

void HW1_LevelChk(CHARACTER_WORK *offset);
int IsHoldKick(CHARACTER_WORK *offset);
int IsHoldPunch(CHARACTER_WORK *offset);
int IsHoldSlash(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JYSP2ExCommandCheckEX2: no debug record */
extern USER_SETTING userinfo;

int JYSP2ExCommandCheckEX2(CHARACTER_WORK *offset);
extern int BattleStatus;

void JYAct_SP2_FWLK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    if (userinfo.gguse[offset->padid] & 4) {
        offset->ply->ExCommandFunc = JYSP2ExCommandCheckEX2;
    }
    if (!IsRight(offset)) {
        SetActNoNext(offset, 0x5A);
    }
    if (!(userinfo.gguse[offset->padid] & 4)) {
        if (!IsHoldSlash(offset) & !IsHoldKick(offset) & !IsHoldPunch(offset)) {
            SetActNoNext(offset, 0x5A);
        }
    }
    if (IsTrgAndOffHighSlash(offset)) {
        SetActNoNext(offset, 0xAB);
    }
    HW1_LevelChk(offset);
    if (userinfo.gguse[offset->padid] & 4) {
        if (BattleStatus == 3) {
            SetAttackActNo(offset, 0xAB);
        }
    }
}

void HW1_LevelChk(CHARACTER_WORK *offset);
int IsHoldKick(CHARACTER_WORK *offset);
int IsHoldPunch(CHARACTER_WORK *offset);
int IsHoldSlash(CHARACTER_WORK *offset);
int IsLeft(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JYSP2ExCommandCheckEX2: no debug record */
extern USER_SETTING userinfo;

int JYSP2ExCommandCheckEX2(CHARACTER_WORK *offset);
extern int BattleStatus;

void JYAct_SP2_BWLK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x61);
    if (userinfo.gguse[offset->padid] & 4) {
        offset->ply->ExCommandFunc = JYSP2ExCommandCheckEX2;
    }
    if (!IsLeft(offset)) {
        SetActNoNext(offset, 0x5A);
    }
    if (!(userinfo.gguse[offset->padid] & 4)) {
        if (!IsHoldSlash(offset) & !IsHoldKick(offset) & !IsHoldPunch(offset)) {
            SetActNoNext(offset, 0x5A);
        }
    }
    if (IsTrgAndOffHighSlash(offset)) {
        SetActNoNext(offset, 0xAB);
    }
    HW1_LevelChk(offset);
    if (userinfo.gguse[offset->padid] & 4) {
        if (BattleStatus == 3) {
            SetAttackActNo(offset, 0xAB);
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_SP2_RTN(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x60);
    offset->ply->ExCommandFunc = 0;
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_SP2_Cancel(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAB);
    offset->ply->ExCommandFunc = 0;
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW1_UP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW1_UP1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x63);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW1_UP2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x64);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW2_MD(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW2_MD1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x65);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW2_MD2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x66);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW3_DO(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW3_DO1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JYAct_HW3_DO2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x68);
    if (userinfo.gguse[offset->padid] & 4) {
        EndtoAction(offset, 0x5A);
    } else {
        EndtoAction(offset, 0x60);
    }
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void JYAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x56);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void JYAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x57);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JYAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void JYAct_LongHair1(CHARACTER_WORK *offset) {
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

void JYAct_LongHair2(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x51)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JYAct_Respect1(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JYAct_Respect2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JYAct_Respect3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x9F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JYAct_Respect4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0xA0);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JYAct_Respect5(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0xA1);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JYAct_HW5_DB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x58);
    SetLandBreakActno(offset, 0x2000B2);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW5_DBLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0xB2);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW8_Coin(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    if (SetChangeFrameAnime(offset, 0xB3)) {
        offset->ply->PersonalWork[1].b[0]++;
    }
    EndtoStand(offset);
}

void OrScreenFlag(int flag);
void ScreenExecute(void);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetFlash(char time, short unsigned int flag);

int JY_SHW_InterruptProc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = offset->ply->Enemy;
    eoffset->ply->DownTimer = 0x78;
    SetFlash(0x10, 0xF01);
    offset->actst |= 0x4000;
    eoffset->actst |= 0x4020;
    SetActNo(offset, 0x7A);
    SetActNo(eoffset, 0x400028);
    eoffset->posy = 0;
    eoffset->actst &= ~0x10;
    eoffset->posx = offset->posx + 0x36B0;
    eoffset->HitStopTime = offset->HitStopTime;
    offset->HitStopTime = 0;
    OrScreenFlag(0x1000);
    ScreenExecute();
    return 1;
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JY_SHW_InterruptProc: no debug record */

int JY_SHW_InterruptProc(CHARACTER_WORK *offset);

void JYAct_SHW1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x78)) {
        offset->ColliFlag = 0;
    }
    offset->ply->AtkHitInterrupt = JY_SHW_InterruptProc;
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void JYAct_SHW3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    OrScreenFlag(0x1000);
    SetAnten(offset, 0);
    if (offset->dirflag == 0) {
        offset->dirflag = 1;
        eoffset->dirflag = 0;
    }
    eoffset->ply->DownTimer = 0x78;
    eoffset->CommandFlag &= ~0x800;
    if (SetChangeFrameAnime(offset, 0x7A)) {
        eoffset->yspeed = 0;
        eoffset->xspeed = 0;
        eoffset->actst |= 0x400000;
        eoffset->posx = offset->posx + 0x36B0;
        eoffset->actst &= ~0x10;
        eoffset->posy = 0;
    }
    eoffset->yspeed = 0;
    eoffset->xspeed = 0;
    offset->actst |= 0x4000;
    eoffset->posx = offset->posx + 0x36B0;
    eoffset->posy = 0;
    eoffset->actst &= ~0x10;
    eoffset->actst |= 0x404020;
    if (EndtoAction(offset, 0xB0)) {
        SetXSpeed(eoffset, 0);
        SetYSpeed(eoffset, -0x708, 0xD2);
        SetActNoNext(eoffset, 0x30031);
        eoffset->SpriteFlag |= 0x1000;
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_SHW4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB0);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW4_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x69);
    EndtoAction(offset, 0x6A);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW4_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x6A);
    EndtoAction(offset, 0x6B);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW4_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x6B);
    EndtoStand(offset);
}

void JYAct_HW6(CHARACTER_WORK *offset) {
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x76);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x7B);
    EndtoStand(offset);
}

void DeleteEffectByItigeki(CHARACTER_WORK *offset);
int IsActEnd(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void JYAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x7C)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        SetFlash(1, 0xF01);
        SetActNoNext(offset, 0x7F);
        DeleteEffectByItigeki(offset);
    }
    if (IsActEnd(offset)) {
        if (!SearchObjectWorkActno(offset, 0x3F, 0xE)) {
            if (++offset->Mark >= 5) {
                SetActNoNext(offset, 0x7E);
            }
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_KILL_RTN(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7E);
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
/* D_5C9398: no debug record */

int JYDamInt_KILL2(CHARACTER_WORK *offset) {
    int acttb[19] = {
        0x00, 0xB9, 0x9D, 0xA1, 0x9A, 0x9C, 0xA3, 0xB6, 0x96, 0x94,
        0xB4, 0x84, 0xAC, 0x8D, 0x9C, 0x9D, 0x8A, 0x8F, 0x8F
    };

    SetActNoNext(offset, acttb[offset->idno]);
    offset->DamColliFlag = 0;
    return 1;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JYDamInt_KILL2: no debug record */

int JYDamInt_KILL2(CHARACTER_WORK *offset);

void JYAct_KILL2(CHARACTER_WORK *offset) {
    SetAnten(offset, 0);
    offset->CommandFlag = 0xC05F;
    offset->posx = -12000;
    offset->actst |= 0x4000;
    OrScreenFlag(0x1000);
    offset->dirflag = 1;
    if (SetChangeFrameAnime(offset, 0x7F)) {
        offset->HitParam->DamInt = JYDamInt_KILL2;
    }
    EndtoAction(offset, 0x7E);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW7_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA6);
    EndtoAction(offset, 0xA7);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JY_HW7_CmdChk: no debug record */

int JY_HW7_CmdChk(CHARACTER_WORK *offset);

void JYAct_HW7_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA7);
    offset->ply->ExCommandFunc = JY_HW7_CmdChk;
    EndtoAction(offset, 0x100AD);
    SetLandBreakActno(offset, 0x2000AC);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void JYAct_HW7_S(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA8);
    SetJumpControl(offset);
    EndtoAction(offset, 0x100A9);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JYAct_HW7_3a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA9);
    SetLandBreakActno(offset, 0x2100AA);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW7_4a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAA);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JYAct_HW7_3b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAD);
    SetLandBreakActno(offset, 0x2100AC);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYAct_HW7_4b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xAC);
    EndtoStand(offset);
}

int IsTrgSlash(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);

int JYSP2ExCommandCheckEX2(CHARACTER_WORK *offset) {
    int lv;

    JYSP2ExCommandCheck2(offset);
    lv = JYGetIaiLevel(offset);
    switch (offset->Localid) {
    case 0x5C:
    case 0x5D:
    case 0x62:
        break;
    case 0x7F:
    default:
        SetAttackActNo(offset, 0xAB);
        return 1;
    }
    if (IsTrgSlash(offset)) {
        if (offset->ply->PersonalWork[0].w[0] > 0) {
            offset->ply->PersonalWork[0].w[0]--;
        }
        if (lv == 0) {
            SetAttackActNo(offset, 0x8005D);
        } else {
            SetAttackActNo(offset, 0x80067);
        }
        return 1;
    }
    if (IsTrgKick(offset)) {
        if (offset->ply->PersonalWork[0].w[0] > 0) {
            offset->ply->PersonalWork[0].w[0]--;
        }
        if (lv == 0) {
            SetAttackActNo(offset, 0x80062);
        } else {
            SetAttackActNo(offset, 0x80065);
        }
        return 1;
    }
    if (IsTrgPunch(offset)) {
        if (offset->ply->PersonalWork[0].w[0] > 0) {
            offset->ply->PersonalWork[0].w[0]--;
        }
        if (lv == 0) {
            SetAttackActNo(offset, 0x8005C);
        } else {
            SetAttackActNo(offset, 0x80063);
        }
        return 1;
    }
    return 0;
}

int IsTrgSlash(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);

int JYSP2ExCommandCheckEX(CHARACTER_WORK *offset) {
    int lv;

    JYSP2ExCommandCheck2(offset);
    lv = JYGetIaiLevel(offset);
    switch (offset->Localid) {
    case 0x5C:
    case 0x5D:
    case 0x62:
        break;
    case 0x7F:
    default:
        SetAttackActNo(offset, 0xAB);
        return 1;
    }
    if (IsTrgSlash(offset)) {
        if (offset->ply->PersonalWork[0].w[0] > 0) {
            offset->ply->PersonalWork[0].w[0]--;
        }
        if (lv == 0) {
            SetAttackActNo(offset, 0x8005D);
        } else {
            SetAttackActNo(offset, 0x80067);
        }
        return 1;
    }
    if (IsTrgKick(offset)) {
        if (offset->ply->PersonalWork[0].w[0] > 0) {
            offset->ply->PersonalWork[0].w[0]--;
        }
        if (lv == 0) {
            SetAttackActNo(offset, 0x80062);
        } else {
            SetAttackActNo(offset, 0x80065);
        }
        return 1;
    }
    if (IsTrgPunch(offset)) {
        if (offset->ply->PersonalWork[0].w[0] > 0) {
            offset->ply->PersonalWork[0].w[0]--;
        }
        if (lv == 0) {
            SetAttackActNo(offset, 0x8005C);
        } else {
            SetAttackActNo(offset, 0x80063);
        }
        return 1;
    }
    if (IsLeft(offset)) {
        SetAttackActNo(offset, 0x61);
        return 1;
    }
    if (IsRight(offset)) {
        SetAttackActNo(offset, 0x5B);
        return 1;
    }
    return 0;
}
