#include "generated/jm_act.h"
int JMHW2ExCommandCheck(CHARACTER_WORK *offset);
int JMHW4CommandCheck(CHARACTER_WORK *offset);
int JMHW4KCommandCheck(CHARACTER_WORK *offset);
void JMkirakiraPut(CHARACTER_WORK *offset);
int JMWinB_KeyHook(CHARACTER_WORK *offset);
int JMBlocking(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

int EnemyThrowCheck(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
short int GetTensionVal(CHARACTER_WORK *offset);
int GetThrowDist(void);
int IsFireWheel(CHARACTER_WORK *offset);
int IsGyakuSyoryu(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsSita(CHARACTER_WORK *offset);
int IsSitaSita(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int JM_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000) {
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x0408005E);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    SetAttackActNo(offset, 0x040800D5);
                    return 1;
                } else {
                    SetAttackActNo(offset, 0x04080085);
                    return 1;
                }
            }
        }
        if (IsGyakuSyoryu(offset) && (userinfo.gguse[offset->padid] & 4) && IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x02080061);
            return 1;
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffKick(offset)) {
                offset->Localid = 2;
                SetAttackActNo(offset, 0x020800BD);
                return 1;
            }
            if ((userinfo.gguse[offset->padid] & 4) && IsTrgAndOffPunch(offset)) {
                if (!EnemyThrowCheck(GetEnemyPlayer(offset)) && GetThrowDist() < 10000) {
                    SetAttackActNo(offset, 0x020800DB);
                    return 1;
                } else {
                    SetAttackActNo(offset, 0x020800DC);
                    return 1;
                }
            }
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffKick(offset)) {
                offset->Localid = 0;
                SetAttackActNo(offset, 0x02080067);
                return 1;
            }
            if (userinfo.gguse[offset->padid] & 4) {
                if (IsTrgAndOffPunch(offset)) {
                    SetAttackActNo(offset, 0x02080074);
                    return 1;
                }
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080062);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x0208007A);
                    return 1;
                }
            } else if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080061);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if ((userinfo.gguse[offset->padid] & 4) && IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x020800C6);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                offset->Localid = 1;
                SetAttackActNo(offset, 0x020800B6);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x0208005C);
                return 1;
            }
        }
        if (IsSitaSita(offset)) {
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x02080056);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080071);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080072);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (offset->posy < -2000 && IsTrgKick(offset)) {
            if (!(userinfo.gguse[offset->padid] & 4)) {
                if (IsHadou(offset)) {
                    offset->Localid = 0;
                    SetAttackActNo(offset, 0x000800CC);
                    return 1;
                }
                if (IsTatsumaki(offset)) {
                    offset->Localid = 1;
                    SetAttackActNo(offset, 0x000800CE);
                    return 1;
                }
                if (IsSyoryu(offset)) {
                    offset->Localid = 2;
                    SetAttackActNo(offset, 0x000800D0);
                    return 1;
                }
            }
            if (IsSita(offset)) {
                SetAttackActNo(offset, 0x000800B3);
                return 1;
            }
        }
    }
    return 0;
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void JM_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x02080050);
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void JM_RespectCommnadCheck(CHARACTER_WORK *offset) {
    SetAttackActNo(offset, 0x02080051);
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int JM_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x63);
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int JM_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (!offset->ply->PersonalWork[1].ub[3]) {
            if (ItigekiCommandCheck(offset, functb, 0x65, 0x64)) {
                return 1;
            }
        }
    }
    return 0;
}

int JM_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 0x5E && offset->Mark) return 14;
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void JMAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JMAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoAction(CHARACTER_WORK *offset, int actno);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetParentPosX(CHARACTER_WORK *offset, short int pos);
void SetParentPosY(CHARACTER_WORK *offset, short int pos);
void SetWinActFlag(CHARACTER_WORK *offset);

void JMAct_WinA_0(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eff;
    COLLISION *col;

    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoAction(offset, 0x53);
    if (offset->Mark & 1) {
        eff = EffectCreate(offset, 0x30, 0);
        SetActNoNext(eff, 0);
        col = Get_GCLExNo(offset, 0);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
        eff->posy += -48000;
        offset->Mark &= ~1;
    }
    if (offset->Mark & 2) {
        eff = SearchEffectWorkActno(offset, 0x30, 1);
        col = Get_GCLExNo(offset, 0);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
    }
    if (offset->Mark & 4) {
        eff = EffectCreate(offset, 0x30, 0);
        SetActNoNext(eff, 2);
        col = Get_GCLExNo(offset, 1);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
        eff->posy += -48000;
        offset->Mark &= ~4;
    }
    if (offset->Mark & 8) {
        eff = SearchEffectWorkActno(offset, 0x30, 3);
        col = Get_GCLExNo(offset, 1);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
    }
    if (offset->Mark & 0x10) {
        eff = EffectCreate(offset, 0x30, 0);
        SetActNoNext(eff, 4);
        col = Get_GCLExNo(offset, 2);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
        eff->posy += -48000;
        offset->Mark &= ~0x10;
    }
    if (offset->Mark & 0x20) {
        eff = SearchEffectWorkActno(offset, 0x30, 5);
        col = Get_GCLExNo(offset, 2);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
    }
}

CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void JMAct_WinA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    if (SetChangeFrameAnime(offset, 0x53)) {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, -1800, 100);
        SetActNoNext(SearchEffectWorkActno(offset, 0x30, 1), 6);
        SetActNoNext(SearchEffectWorkActno(offset, 0x30, 3), 6);
        SetActNoNext(SearchEffectWorkActno(offset, 0x30, 5), 6);
    }
    SetLandBreakActno(offset, 0x54);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoDemo(CHARACTER_WORK *offset);
COLLISION *Get_GCLEx(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetParentPosX(CHARACTER_WORK *offset, short int pos);
void SetParentPosY(CHARACTER_WORK *offset, short int pos);
void SetWinActFlag(CHARACTER_WORK *offset);

void JMAct_WinA_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eff;
    COLLISION *col;

    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54);
    if (offset->Mark) {
        eff = EffectCreate(offset, 0x30, 0);
        SetActNoNext(eff, 7);
        col = Get_GCLExNo(offset, 0);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y);
        eff->posy += -48000;
        eff = EffectCreate(offset, 0x30, 0);
        SetActNoNext(eff, 8);
        col = Get_GCLEx(offset);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y - 10);
        eff->posy += -51200;
        eff = EffectCreate(offset, 0x30, 0);
        SetActNoNext(eff, 9);
        col = Get_GCLEx(offset);
        SetParentPosX(eff, col->x);
        SetParentPosY(eff, col->y - 20);
        eff->posy += -54400;
        offset->Mark = 0;
    }
    EndtoDemo(offset);
}

int GetPlayerDist(void);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

int JMWinB_KeyHook(CHARACTER_WORK *offset) {
    if (GetPlayerDist() > 14000) {
        SetActNo(offset, 0x00100005);
        return 0;
    }
    return 1;
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void JMAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x5D);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void JMAct_EntryB_1(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 0x75)) {
        SetXSpeed(offset, -50000);
        SetYSpeed(offset, -46000, 0);
    } else {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
    }
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_EntryB_2(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x76);
    SetLandBreakActno(offset, 0x77);
}

int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_EntryB_3(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x77);
    EndtoStand(offset);
}

int IsHadou(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;

int JMHW2ExCommandCheck(CHARACTER_WORK *offset) {
    if (userinfo.gguse[offset->padid] & 4) {
        offset->ply->PersonalWork[0].ub[1] = 0;
    }
    if (offset->ply->PersonalWork[0].ub[1] && offset->actnonext == -1) {
        if (IsTrgKick(offset)) {
            if (g_info.boss[offset->padid] == 1) {
                if (IsSyoryu(offset)) {
                    offset->Localid = 2;
                    offset->ply->PersonalWork[0].ub[1]--;
                    SetAttackActNo(offset, 0x000800C2);
                    return 1;
                }
                if (IsHadou(offset)) {
                    offset->Localid = 0;
                    offset->ply->PersonalWork[0].ub[1]--;
                    SetAttackActNo(offset, 0x0008006C);
                    return 1;
                }
                if (IsTatsumaki(offset)) {
                    offset->Localid = 1;
                    offset->ply->PersonalWork[0].ub[1]--;
                    SetAttackActNo(offset, 0x000800BB);
                    return 1;
                }
            } else {
                if (offset->Localid != 2 && IsSyoryu(offset)) {
                    offset->Localid = 2;
                    offset->ply->PersonalWork[0].ub[1]--;
                    SetAttackActNo(offset, 0x000800C2);
                    return 1;
                }
                if (offset->Localid != 0 && IsHadou(offset)) {
                    offset->Localid = 0;
                    offset->ply->PersonalWork[0].ub[1]--;
                    SetAttackActNo(offset, 0x0008006C);
                    return 1;
                }
                if (offset->Localid != 1 && IsTatsumaki(offset)) {
                    offset->Localid = 1;
                    offset->ply->PersonalWork[0].ub[1]--;
                    SetAttackActNo(offset, 0x000800BB);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int JMHW4CommandCheck(CHARACTER_WORK *offset) {
    if (userinfo.gguse[offset->padid] & 4) {
        SetActNoNext(offset, 0x000800C7);
        return 1;
    }
    if (IsTrgAndOffPunch(offset)) {
        SetActNoNext(offset, 0x000800C6);
        return 1;
    }
    if (IsTrgAndOffKick(offset)) {
        SetActNoNext(offset, 0x000800C7);
        return 1;
    }
    if (IsTrgSlash(offset)) {
        SetActNoNext(offset, 0x000800C8);
        return 1;
    }
    if (IsTrgAndOffHighSlash(offset)) {
        SetActNoNext(offset, 0x0008007A);
        return 1;
    }
    return 0;
}

int IsHadou(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int JMHW4KCommandCheck(CHARACTER_WORK *offset) {
    if (IsTrgKick(offset)) {
        if (IsHadou(offset)) {
            offset->ply->PersonalWork[0].ub[1] = 1;
            offset->Localid = 0;
            SetAttackActNo(offset, 0x0008006C);
            return 1;
        }
        if (IsTatsumaki(offset)) {
            offset->ply->PersonalWork[0].ub[1] = 1;
            offset->Localid = 1;
            SetAttackActNo(offset, 0x000800BB);
            return 1;
        }
        if (IsSyoryu(offset)) {
            offset->ply->PersonalWork[0].ub[1] = 1;
            offset->Localid = 2;
            SetAttackActNo(offset, 0x000800C2);
            return 1;
        }
    }
    return 0;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_DKFirst(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    EndtoAction(offset, 0x69);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_FKFirst(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB6);
    EndtoAction(offset, 0xB8);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_TKFirst(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBD);
    EndtoAction(offset, 0xBF);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);

void JMkirakiraPut(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    if (!(offset->ActTimer & 1) && offset->HitStopTime == 0) {
        arg.flag = 0x200;
        EffectCreate_ActNo(offset, 0x26, &arg, 4);
    }
}

void JMkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_DK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[0].ub[2] ? 0x68 : 0x69)) {
        offset->ply->PersonalWork[0].ub[1] = 2;
        if (offset->ply->PersonalWork[0].ub[2]) {
            offset->ply->PersonalWork[0].ub[2]--;
        }
    }
    if (offset->AnimeNo == 0x69) {
        SetLandBreakActno(offset, 0x6A);
    } else {
        SetLandBreakActno(offset, 0x6E);
    }
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    JMkirakiraPut(offset);
}

void JMkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_SDK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[0].ub[2] ? 0xCB : 0xCC)) {
        offset->ply->PersonalWork[0].ub[1] = 2;
        if (offset->ply->PersonalWork[0].ub[2]) {
            offset->ply->PersonalWork[0].ub[2]--;
        }
    }
    if (offset->AnimeNo == 0xCC) {
        SetLandBreakActno(offset, 0x6A);
    } else {
        SetLandBreakActno(offset, 0x6E);
    }
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    JMkirakiraPut(offset);
}

void JMkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_VDK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[0].ub[2] ? 0x6B : 0x6C)) {
        if (offset->ply->PersonalWork[0].ub[2]) {
            offset->ply->PersonalWork[0].ub[2]--;
        }
    }
    if (offset->AnimeNo == 0x6C) {
        SetLandBreakActno(offset, 0x6D);
    } else {
        SetLandBreakActno(offset, 0x6F);
    }
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    JMkirakiraPut(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_FK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[1].ub[1] ? 0xB7 : 0xB8)) {
        offset->ply->PersonalWork[0].ub[1] = 2;
        if (offset->ply->PersonalWork[1].ub[1]) {
            offset->ply->PersonalWork[1].ub[1]--;
        }
    }
    SetLandBreakActno(offset, 0xB9);
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_SFK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[1].ub[1] ? 0xCD : 0xCE)) {
        offset->ply->PersonalWork[0].ub[1] = 2;
        if (offset->ply->PersonalWork[1].ub[1]) {
            offset->ply->PersonalWork[1].ub[1]--;
        }
    }
    SetLandBreakActno(offset, 0xB9);
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_VFK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[1].ub[1] ? 0xBA : 0xBB)) {
        if (offset->ply->PersonalWork[1].ub[1]) {
            offset->ply->PersonalWork[1].ub[1]--;
        }
    }
    SetLandBreakActno(offset, 0xBC);
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
}

void JMkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_TK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[1].ub[0] ? 0xBE : 0xBF)) {
        offset->ply->PersonalWork[0].ub[1] = 2;
        if (offset->ply->PersonalWork[1].ub[0]) {
            offset->ply->PersonalWork[1].ub[0]--;
        }
    }
    SetLandBreakActno(offset, 0xC0);
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    JMkirakiraPut(offset);
}

void JMkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_STK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[1].ub[0] ? 0xCF : 0xD0)) {
        offset->ply->PersonalWork[0].ub[1] = 2;
        if (offset->ply->PersonalWork[1].ub[0]) {
            offset->ply->PersonalWork[1].ub[0]--;
        }
    }
    SetLandBreakActno(offset, 0xC0);
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    JMkirakiraPut(offset);
}

void JMkirakiraPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
/* JMHW2ExCommandCheck: no debug record */

void JMAct_HW2_VTK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, offset->ply->PersonalWork[1].ub[0] ? 0xC1 : 0xC2)) {
        if (offset->ply->PersonalWork[1].ub[0]) {
            offset->ply->PersonalWork[1].ub[0]--;
        }
    }
    SetLandBreakActno(offset, 0xC3);
    if (offset->attackst & 0x1000) {
        offset->ply->ExCommandFunc = JMHW2ExCommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    JMkirakiraPut(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_DKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6A);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_FKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB9);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_TKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC0);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_MAXDKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6E);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_VDKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6D);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_MAXVDKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_VFKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBC);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW2_VTKLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC3);
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_HW7_Kick(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB3);
    if (offset->attackst & 0x80000) {
        SetActNoNext(offset, 0x000100B4);
    }
    SetLandBreakActno(offset, 0xB5);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_HW7_KickHit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB4);
    SetLandBreakActno(offset, 0xB5);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW7_KickLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xB5);
    EndtoStand(offset);
}

void HitMarkPut(CHARACTER_WORK *offset, short unsigned int idno, short unsigned int actno, int cnt);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

int JMBlocking(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    HitMarkPut(offset, 0x2F, 0xB, cnt);
    SetActNo(offset, 0x0020008F);
    offset->SpriteFlag |= 0x200;
    offset->ply->whiteouttime = 5;
    offset->ply->PersonalWork[1].ub[3] = 30;
    return 1;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JMBlocking: no debug record */

void JMAct_HW3_HARAI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (offset->Mark) {
        offset->GuardSt = 0;
        offset->ply->GuardFunc = 0;
    } else {
        offset->ply->GuardFunc = JMBlocking;
        offset->GuardSt = 0x1201;
    }
    SetChangeFrameAnime(offset, 0x5C);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Add(CHARACTER_WORK *offset, int Val);

void JMAct_HW3_HARAISuccess(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC11F;
    if (SetChangeFrameAnime(offset, 0x8F)) {
        Tension_Add(offset, 400);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JMHW4CommandCheck: no debug record */

void JMAct_HW4_WA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x61);
    if (offset->Mark) {
        offset->ply->ExCommandFunc = JMHW4CommandCheck;
    } else {
        offset->ply->ExCommandFunc = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int GetPlayerDistOff(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JMAct_HW4_WAV(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x7A) && (userinfo.gguse[offset->padid] & 4)) {
        PLAYER_ENTRY *ply = offset->ply;
        ply->MutekiTime = 6;
        ply->pactst |= 2;
        offset->actst |= 0x20000;
    }
    if (offset->Mark & 2) {
        if (GetPlayerDistOff(offset) < 0) {
            SetActNoNext(offset, 0x7B);
        }
    }
    if (offset->Mark) {
        if (GetPlayerDistOff(offset) < -8000) {
            SetActNoNext(offset, 0x7B);
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

void JMAct_HW4_WAVH(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x7B)) {
        if (userinfo.gguse[offset->padid] & 4) {
            offset->ply->pactst |= 2;
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW4_P(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC6);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JMHW4KCommandCheck: no debug record */

void JMAct_HW4_K(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC7);
    offset->ply->ExCommandFunc = JMHW4KCommandCheck;
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW4_S(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC8);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW4_WAPowerUP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW4_HS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC9);
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW5_SAKE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW6_TAMEDK(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x56)) {
        offset->ply->PersonalWork[0].ub[0] = 0;
    }
    if (EndtoStand(offset) && !offset->Mark) {
        offset->Mark = 1;
        arg.flag = 4;
        offset->ply->PersonalWork[0].ub[2]++;
        if (offset->ply->PersonalWork[0].ub[2] == 1) {
            arg.actno = 0x10;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[0].ub[2] == 2) {
            arg.actno = 0x11;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[0].ub[2] == 3) {
            arg.actno = 0x12;
            EffectCreate(offset, 0x30, &arg);
        } else {
            offset->ply->PersonalWork[0].ub[2] = 3;
        }
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW6_TAMEFK(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x71);
    if (EndtoStand(offset) && !offset->Mark) {
        offset->Mark = 1;
        arg.flag = 4;
        offset->ply->PersonalWork[1].ub[1]++;
        if (offset->ply->PersonalWork[1].ub[1] == 1) {
            arg.actno = 0x13;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[1].ub[1] == 2) {
            arg.actno = 0x14;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[1].ub[1] == 3) {
            arg.actno = 0x15;
            EffectCreate(offset, 0x30, &arg);
        } else {
            offset->ply->PersonalWork[1].ub[1] = 3;
        }
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW6_TAMETK(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x72);
    if (EndtoStand(offset) && !offset->Mark) {
        offset->Mark = 1;
        arg.flag = 4;
        offset->ply->PersonalWork[1].ub[0]++;
        if (offset->ply->PersonalWork[1].ub[0] == 1) {
            arg.actno = 0x16;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[1].ub[0] == 2) {
            arg.actno = 0x17;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[1].ub[0] == 3) {
            arg.actno = 0x18;
            EffectCreate(offset, 0x30, &arg);
        } else {
            offset->ply->PersonalWork[1].ub[0] = 3;
        }
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW6_TAMEHS(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    if (EndtoStand(offset) && !offset->Mark) {
        offset->Mark = 1;
        arg.flag = 4;
        offset->ply->PersonalWork[1].ub[2]++;
        if (offset->ply->PersonalWork[1].ub[2] == 1) {
            arg.actno = 0xD;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[1].ub[2] == 2) {
            arg.actno = 0xE;
            EffectCreate(offset, 0x30, &arg);
        } else if (offset->ply->PersonalWork[1].ub[2] == 3) {
            arg.actno = 0xF;
            EffectCreate(offset, 0x30, &arg);
        } else {
            offset->ply->PersonalWork[1].ub[2] = 3;
        }
    }
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HW6_MAXTUKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x74);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);

void JMAct_SuperHSlash_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetNormalBegin(offset);
    SetChangeFrameAnime(offset, 0x41);
    EndtoAction(offset, 0x5A);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_SuperHSlash_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    SetLandBreakActno(offset, 0x5B);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SuperHSlash_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW1_HAKKEI_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW1_HAKKEI_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW1_HAKKEI_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x60);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW2_RANBU_1(CHARACTER_WORK *offset) {
    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x85);
    if (offset->attackst & 0x80000) {
        SetActNoNext(offset, 0x87);
    } else {
        EndtoAction(offset, 0x86);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW2_RANBU_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x86);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW2_RANBU_3(CHARACTER_WORK *offset) {
    if (offset->padid == 0) {
        OrScreenFlag(0x2D);
    } else {
        OrScreenFlag(0x2E);
    }
    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x87);
    EndtoAction(offset, 0x88);
}

void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_SHW2_RANBU_4(CHARACTER_WORK *offset) {
    if (offset->padid == 0) {
        OrScreenFlag(0x2D);
    } else {
        OrScreenFlag(0x2E);
    }
    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x88);
    if (offset->attackst & 0x80000) {
        SetActNoNext(offset, 0xA9);
    } else {
        SetLandBreakActno(offset, 0x15);
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_SHW2_RANBU_5(CHARACTER_WORK *offset) {
    if (offset->padid == 0) {
        OrScreenFlag(0x2D);
    } else {
        OrScreenFlag(0x2E);
    }
    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA9);
    SetLandBreakActno(offset, 0x15);
    if ((offset->attackst & 0x80000) && !offset->Mark) {
        EffectCreate_ActNo(offset, 0x2F, 0, 5);
        offset->Mark |= 1;
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW3_Start(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xD5)) {
        offset->Mark = 0;
    }
    if (offset->attackst & 0x80000) {
        offset->Mark = 1;
    }
    if (offset->Mark) {
        if (EndtoAction(offset, 0xD7)) {
            if (offset->posx < enemy->posx) {
                offset->dirflag = 1;
            } else {
                offset->dirflag = 0;
            }
        }
    } else {
        EndtoAction(offset, 0xD6);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW3_Failed(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xD6);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW3_First(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xD7)) {
        offset->Mark = 0;
        offset->Localid = 0;
        if (offset->posx < enemy->posx) {
            offset->dirflag = 1;
        } else {
            offset->dirflag = 0;
        }
    }
    if (offset->attackst & 0x80000) {
        offset->Mark = 1;
    }
    if (offset->Mark) {
        if (EndtoAction(offset, 0xD8)) {
            if (offset->posx < enemy->posx) {
                offset->dirflag = 1;
            } else {
                offset->dirflag = 0;
            }
        }
    } else {
        EndtoAction(offset, 0xD6);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW3_Second(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xD8)) {
        offset->Mark = 0;
        if (offset->posx < enemy->posx) {
            offset->dirflag = 1;
        } else {
            offset->dirflag = 0;
        }
        offset->Localid++;
    }
    if (offset->attackst & 0x80000) {
        offset->Mark = 1;
    }
    if (offset->Mark) {
        if (offset->Localid >= 8) {
            SetActNoNext(offset, 0x88);
            if (enemy->posx > offset->posx) {
                offset->dirflag = 1;
                offset->posx = enemy->posx - 5000;
            } else {
                offset->dirflag = 0;
                offset->posx = enemy->posx + 5000;
            }
        } else if (EndtoAction(offset, 0xD8)) {
            if (offset->posx < enemy->posx) {
                offset->dirflag = 1;
            } else {
                offset->dirflag = 0;
            }
        }
    } else {
        EndtoAction(offset, 0xD6);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_SHW3_Third(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    if (offset->padid == 0) {
        OrScreenFlag(0x2D);
    } else {
        OrScreenFlag(0x2E);
    }
    SetAnten(offset, 1);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xD9)) {
        if (enemy->posx > offset->posx) {
            offset->dirflag = 1;
        } else {
            offset->dirflag = 0;
        }
    }
    if (EndtoAction(offset, 0x88)) {
        if (offset->posx < enemy->posx) {
            offset->dirflag = 1;
        } else {
            offset->dirflag = 0;
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void JMAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x63)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    SetChangeFrameAnime(offset, 0x64);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_ITIGEKI_LV2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x78);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_ITIGEKI_LV3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x79);
    EndtoStand(offset);
}

void DeleteEffectByItigeki(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void JMAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x65)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        DeleteEffectByItigeki(offset);
        SetFlash(1, 0xF01);
        offset->ply->MutekiTime = 0xFF;
        SetActNoNext(offset, 0x66);
    }
    EndtoAction(offset, 0x73);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);

void JMAct_ITIGEKI2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x40);
    SetFlash(1, 0xF00);
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x66)) {
        offset->posx = 32000;
        offset->SpriteFlag |= 1;
        EffectCreate_ActNo(offset, 0x27, 0, 0xC);
    }
    offset->posy = 44000;
    enemy->xspeed = 0;
    enemy->yspeed = 0;
    enemy->gravity = 0;
    enemy->posy = -10000;
    enemy->ply->DownTimer = 20;
    enemy->actst &= ~8;
    if (EndtoAction(offset, 0x89)) {
        enemy->yspeed = -200;
        enemy->gravity = 10;
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_ITIGEKI_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x73)) {
        offset->SpriteFlag &= ~1;
    }
    EndtoStand(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_ITIGEKI_DK(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x100000;
    if (SetChangeFrameAnime(offset, 0x89)) {
        OrScreenFlag(0x1000);
        offset->SpriteFlag &= ~1;
        SetFlash(10, 0xF01);
        offset->posy = -40000;
        enemy->posy = -40000;
        offset->posx = 0;
        enemy->yspeed = -200;
        enemy->gravity = 10;
        enemy->posx = 0;
        enemy->xspeed = 0;
        offset->actst |= 8;
        enemy->actst |= 8;
        EffectCreate_ActNo(offset, 0x27, 0, 0xB);
    }
    SetAnten(offset, 1);
    SetLandBreakActno(offset, 0x15);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    SetChangeFrameAnime(offset, 0x7C);
    EndtoAction(offset, 0x7D);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_Throw2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7D);
    SetLandBreakActno(offset, 0x7E);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_Throw3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7E);
    EndtoAction(offset, 0x7F);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JMAct_Throw4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7F);
    SetLandBreakActno(offset, 0x15);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HWX_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xDA);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HWX_Hit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xDB);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMAct_HWX_Failed(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xDC);
    EndtoStand(offset);
}
