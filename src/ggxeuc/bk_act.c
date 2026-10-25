#include "generated/bk_act.h"

int PlayerGuardDamage(CHARACTER_WORK *offset, int damage, TACTHEADER *ahp);
void RequestGuard(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);

int BKGuardDash(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    if (PlayerGuardDamage(offset, header->damage, header)) {
        return -1;
    }
    RequestGuard(offset, header->flag, header->lvflag, cnt);
    return 1;
}

void RequestGuard(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);

int BKGuardHW3(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    offset->DamParam->FaintPoint = 0;
    header->flag &= ~0x40000;
    header->flag &= ~0x80000;
    RequestGuard(offset, header->flag, header->lvflag, cnt);
    offset->HitStopTime = 0;
    return 1;
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsGyakuHaouSyoko(CHARACTER_WORK *offset);
int IsGyakuSyoryu(CHARACTER_WORK *offset);
int IsGyakuYoga(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int BK_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if ((userinfo.gguse[offset->padid] & 4) && GetTensionVal(offset) >= 5000 && IsGyakuHaouSyoko(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x02080095);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x02080096);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080097);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x02080098);
                return 1;
            }
        }
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x0208005A);
            return 1;
        }
        if (IsGyakuYoga(offset) && IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x0208008E);
            return 1;
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x0208005B);
                return 1;
            }
            if ((userinfo.gguse[offset->padid] & 2) && IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x020800A3);
                return 1;
            }
        }
        if (IsGyakuSyoryu(offset)) {
            if (userinfo.gguse[offset->padid] & 2) {
                if (IsTrgAndOffPunch(offset)) {
                    SetAttackActNo(offset, 0x02080056);
                    return 1;
                }
            }
            if (userinfo.gguse[offset->padid] & 4) {
                if (IsTrgAndOffPunch(offset)) {
                    SetAttackActNo(offset, 0x02080056);
                    return 1;
                }
                if (IsTrgAndOffKick(offset)) {
                    SetAttackActNo(offset, 0x0208005C);
                    return 1;
                }
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080054);
                    return 1;
                }
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x02080059);
            return 1;
        }
        if (IsHadou(offset) && IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x0209009E);
            return 1;
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void BK_ExciteCommnadCheck(CHARACTER_WORK *offset) {
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
extern GAME_INFOMATION g_info;

void BK_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080052);
    } else {
        SetAttackActNo(offset, 0x02080053);
    }
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsGCancel(CHARACTER_WORK *offset);
int IsGyakuHaouSyoko(CHARACTER_WORK *offset);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int BK_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (GetTensionVal(offset) >= 5000 && !(userinfo.gguse[offset->padid] & 4) && IsGyakuHaouSyoko(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                offset->HitStopTime = 0;
                offset->actst |= 0x80;
                SetAttackActNo(offset, 0x02080095);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                offset->HitStopTime = 0;
                offset->actst |= 0x80;
                SetAttackActNo(offset, 0x02080096);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                offset->HitStopTime = 0;
                offset->actst |= 0x80;
                SetAttackActNo(offset, 0x02080097);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                offset->HitStopTime = 0;
                offset->actst |= 0x80;
                SetAttackActNo(offset, 0x02080098);
                return 1;
            }
        }
        if (IsGCancel(offset)) {
            if (!(userinfo.gguse[offset->padid] & 2)) {
                if (IsTrgAndOffPunch(offset)) {
                    offset->HitStopTime = 0;
                    offset->actst |= 0x80;
                    SetAttackActNo(offset, 0x02080056);
                    return 1;
                }
            }
            if (!(userinfo.gguse[offset->padid] & 4)) {
                if (IsTrgAndOffPunch(offset)) {
                    offset->HitStopTime = 0;
                    offset->actst |= 0x80;
                    SetAttackActNo(offset, 0x02080056);
                    return 1;
                }
                if (IsTrgAndOffKick(offset)) {
                    offset->HitStopTime = 0;
                    offset->actst |= 0x80;
                    SetAttackActNo(offset, 0x0208005C);
                    return 1;
                }
                if (IsTrgAndOffSlash(offset)) {
                    offset->HitStopTime = 0;
                    offset->actst |= 0x80;
                    SetAttackActNo(offset, 0x02080054);
                    return 1;
                }
            }
        }
    }
    if (GetTensionVal(offset) >= 5000 && !(offset->actst & 0x10) && IsDouji(offset, 0xF, 2, 2)) {
        if (IsStdRight(offset) || IsSitRight(offset)) {
            offset->actst |= 0x80;
            SetAttackActNo(offset, 0x03080080);
            return 1;
        }
    }
    return 0;
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int BK_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x65, 0x64)) {
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void BKAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x80)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void BKAct_LongHair1(CHARACTER_WORK *offset) {
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

void BKAct_LongHair2(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x51)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void BKAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void BKAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void BKAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x57);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void BKAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x58);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void BKAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x92);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_SHW1_TTT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    if (offset->attackst & 0x80000) {
        if (offset->Mark & 1) {
            EffectCreate_ActNo(offset, 0x3A, 0, 2);
            offset->Mark &= ~1;
            offset->attackst &= ~0x80000;
        }
        if (offset->Mark & 2) {
            EffectCreate_ActNo(offset, 0x3A, 0, 3);
            offset->Mark &= ~2;
            offset->attackst &= ~0x80000;
        }
        if (offset->Mark & 4) {
            EffectCreate_ActNo(offset, 0x3A, 0, 4);
            offset->Mark &= ~4;
            offset->attackst &= ~0x80000;
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_HW1_Tatami(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_HW1_Tatami2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA3);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void BKAct_HW1_STatami(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9E);
    SetLandBreakActno(offset, 0x9F);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_HW1_STatamiLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9F);
    EndtoSit(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void BKAct_HW2_RZ(CHARACTER_WORK *offset) {
    offset->ply->JumpFlag |= 4;
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    SetLandBreakActno(offset, 0x200015);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_StiffLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8F);
    EndtoSit(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern USER_SETTING userinfo;

int BKGuardHW3(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void BKAct_HW3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x56)) {
        if (userinfo.gguse[offset->padid] & 4) {
            offset->GuardSt |= 0x400;
        }
    }
    if (userinfo.gguse[offset->padid] & 4) {
        if (offset->ActTimer <= 32 && (offset->GuardSt & 0x400)) {
            offset->ply->GuardFunc = BKGuardHW3;
        } else {
            offset->GuardSt = 0;
            offset->ply->GuardFunc = 0;
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_HW4_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x54);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_HW5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    offset->actst |= 0x100000;
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int BKGuardDash(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void BKAct_HW6_GuardDash(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8E);
    if (offset->Mark) {
        offset->ply->GuardFunc = BKGuardDash;
        offset->GuardSt = 0x201;
        if (offset->dirflag == 0) {
            if (offset->xspeed > 0) {
                offset->xspeed = -offset->xspeed;
            }
        } else if (offset->dirflag == 1) {
            if (offset->xspeed < 0) {
                offset->xspeed = -offset->xspeed;
            }
        }
    } else {
        offset->GuardSt = 0;
        offset->ply->GuardFunc = 0;
    }
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x60);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x61);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_EntryC_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_EntryC_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5F);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_KakuseiNo2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x63);
    EndtoStand(offset);
}

int BKDamInt_JumpSeal(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    offset->ply->JumpSeal = 0xF0;
    return 0;
}

int BKDamInt_ArtsSeal(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    offset->ply->ArtsSeal = 0xF0;
    return 0;
}

int BKDamInt_AtkDown(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    offset->ply->AttackDown = 0xF0;
    return 0;
}

int BKDamInt_DefDown(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    offset->ply->DefenceDown = 0xF0;
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int BKDamInt_JumpSeal(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void BKAct_SHW2_Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x95);
    if (offset->HitParam) {
        offset->HitParam->DamInt = BKDamInt_JumpSeal;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int BKDamInt_ArtsSeal(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void BKAct_SHW2_Arts(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x96);
    if (offset->HitParam) {
        offset->HitParam->DamInt = BKDamInt_ArtsSeal;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int BKDamInt_AtkDown(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void BKAct_SHW2_Attack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x97);
    if (offset->HitParam) {
        offset->HitParam->DamInt = BKDamInt_AtkDown;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int BKDamInt_DefDown(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void BKAct_SHW2_Defense(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x98);
    if (offset->HitParam) {
        offset->HitParam->DamInt = BKDamInt_DefDown;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x64);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void Rq_Fadeout(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFadeCockpitFlag(int val);
void SetFadeEndCommand(int val);
void SetFadeSpeed(int frame);

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void BKAct_ITIGEKI_DASH(CHARACTER_WORK *offset) {
    short unsigned int act_tbl[19] = {
        0x00, 0xC6, 0xA5, 0xAB, 0xA5, 0xAB, 0xAA, 0xC0, 0x9C, 0x8D,
        0xBC, 0x8D, 0xB2, 0xA6, 0xA5, 0xA4, 0x90, 0x95, 0x95
    };
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x65)) {
        offset->ply->OverKillFlag = 2;
        offset->ply->PersonalWork[0].b[0] = offset->dirflag;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        SetFadeSpeed(30);
        SetFadeEndCommand(0);
        Rq_Fadeout();
        SetFadeCockpitFlag(1);
        offset->ply->MutekiTime = 0xFF;
        eoffset->ply->MutekiTime = 60;
        eoffset->actst |= 0x20000;
        offset->Mark = 1;
        offset->HitStopTime = 60;
        offset->attackst &= ~0x80000;
        eoffset->HitStopTime = 60;
        SetActNoNext(offset, 0x67);
        SetActNoNext(eoffset, act_tbl[eoffset->idno]);
    }
    if (!offset->Mark) {
        if (offset->actst & 0x2800) {
            if (offset->dirflag == 1 && offset->posx > 0) {
                SetAttackActNo(offset, 0x66);
            }
            if (offset->dirflag == 0 && offset->posx < 0) {
                SetAttackActNo(offset, 0x66);
            }
        }
        if (offset->attackst & 0x1000) {
            SetActNoNext(offset, 0x66);
        }
    }
    EndtoStand(offset);
}

void DeleteEffectByItigeki(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_ITIGEKI(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x67)) {
        DeleteEffectByItigeki(offset);
        if (offset->ply->PersonalWork[0].ub[0]) {
            if (eoffset->posx + 30000 > 50000) {
                offset->posx = 50000;
                eoffset->posx = 20000;
            } else {
                offset->posx = eoffset->posx + 30000;
            }
        } else {
            if (eoffset->posx - 30000 < -50000) {
                offset->posx = -50000;
                eoffset->posx = -20000;
            } else {
                offset->posx = eoffset->posx - 30000;
            }
        }
        offset->posy = eoffset->posy = 0;
        eoffset->ply->DownTimer = -1;
        eoffset->actst &= ~0x10;
        eoffset->CommandFlag &= ~0x800;
        OrScreenFlag(0x41000);
        offset->dirflag = offset->ply->PersonalWork[0].ub[0];
        eoffset->dirflag = offset->dirflag == 0;
    } else {
        OrScreenFlag(0xC0);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_ITIGEKI_MISS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x66);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x400) | 0x40;
    SetChangeFrameAnime(offset, 0x85);
    EndtoStand(offset);
}
