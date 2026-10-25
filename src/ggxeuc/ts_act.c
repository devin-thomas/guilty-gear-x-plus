#include "generated/ts_act.h"

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsYoga(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;

int TS_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsFireWheel(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x04080095);
            return 1;
        }
        if (g_info.boss[offset->padid] == 1) {
            if (IsYoga(offset)) {
                if (IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x02080085);
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x02080089);
                    return 1;
                }
            }
        } else if (IsYoga(offset)) {
            if (IsTrgAndOffSlash(offset) && !(offset->ply->ObjFlag & 1)) {
                SetAttackActNo(offset, 0x02080085);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset) && !(offset->ply->ObjFlag & 8)) {
                SetAttackActNo(offset, 0x02080089);
                return 1;
            }
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffKick(offset) && !(offset->ply->ObjFlag & 2)) {
                SetAttackActNo(offset, 0x02080092);
                return 1;
            }
            if (IsTrgAndOffPunch(offset) && !(offset->ply->ObjFlag & 2)) {
                SetAttackActNo(offset, 0x02080088);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x02080057);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    offset->Localid = 0;
                    SetAttackActNo(offset, 0x0208008F);
                    return 1;
                } else {
                    offset->Localid = 0;
                    SetAttackActNo(offset, 0x0208008E);
                    return 1;
                }
            }
            if (IsTrgAndOffHighSlash(offset) && (userinfo.gguse[offset->padid] & 4)) {
                offset->Localid = 1;
                SetAttackActNo(offset, 0x0208008F);
                return 1;
            }
        }
    }
    return 0;
}

int Random(void);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void TS_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *karasu;

    karasu = SearchObjectWork(offset, 0x52);
    if (karasu) {
        if (g_info.mode & 0x4000) {
            set_random_seed_abs();
        }
        if (offset->actno == 0x50 || offset->actno == 0x51) {
            return;
        }
        if (karasu->actno == 5 || karasu->actno == 6 || karasu->actno == 7 || karasu->actno == 9 || karasu->actno == 0xC) {
            return;
        }
        if (Random() & 1) {
            SetAttackActNo(offset, 0x02080050);
        } else {
            SetAttackActNo(offset, 0x02080051);
        }
    }
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void TS_RespectCommnadCheck(CHARACTER_WORK *offset) {
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
short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsGCancel(CHARACTER_WORK *offset);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int TS_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    if (userinfo.gguse[offset->padid] & 4) {
        if (!(offset->actst & 0x10) && IsGCancel(offset) && IsTrgAndOffPunch(offset)) {
            offset->HitStopTime = 0;
            offset->ply->MutekiTime = 0x18;
            offset->actst |= 0x80;
            SetAttackActNo(offset, 0x02080057);
            return 1;
        }
        if (GetTensionVal(offset) < 5000) {
            return 0;
        }
        if (!(offset->actst & 0x10) && IsDouji(offset, 0xF, 2, 2)) {
            if (IsStdRight(offset) || IsSitRight(offset)) {
                offset->actst |= 0x80;
                SetAttackActNo(offset, 0x03080075);
                return 1;
            }
        }
        return 0;
    }
    return CmnGuardSpecialAttackCheck(offset, 0x75);
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int TS_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x5E, 0x5B)) {
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void TSAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x75)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void TSAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54);
    EndtoDemo(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void TSAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    if (SetChangeFrameAnime(offset, 0x93)) {
        offset->ply->PersonalWork[0].l = 8;
    }
    if (offset->ply->PersonalWork[0].l == 9 && (offset->InstSt & 8)) {
        SetActNoNext(offset, 0x94);
    }
    TimetoAction(offset, 0x94, 0xB4);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void TSAct_WinB2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x94);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void TSAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_EntryB_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_EntryB_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    EndtoStand(offset);
}

void TSAct_ITIGEKI_L1(CHARACTER_WORK *offset) {
}

void TSAct_ITIGEKI_L2(CHARACTER_WORK *offset) {
}

void TSAct_ITIGEKI_L3(CHARACTER_WORK *offset) {
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x5B);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int IsBattleDecision(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
void SetFlashOut(int fms);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void TSAct_ITIGEKI(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x5E)) {
        offset->ply->OverKillFlag = 2;
        offset->ColliFlag = 0;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (!IsBattleDecision() && (offset->attackst & 0x80000)) {
        int acttb[19] = {
            0x00, 0xC7, 0xA6, 0xAC, 0xA7, 0xAF, 0xAB, 0xC1, 0x9E, 0xA0,
            0xBF, 0x98, 0xC4, 0xA8, 0xAE, 0xA8, 0x91, 0x96, 0x96
        };

        if (offset->dirflag) {
            if (offset->posx + 26000 >= 72000) {
                offset->posx = 46000;
            }
        } else {
            if (offset->posx - 26000 <= -72000) {
                offset->posx = -46000;
            }
        }
        SetActNoNext(eoffset, acttb[eoffset->idno]);
        SetActNoNext(offset, 0x96);
        SetFlash(6, 0xF01);
        SetFlashOut(4);
    }
    EndtoStand(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoAction(CHARACTER_WORK *offset, int actno);
void OrScreenFlag(int flag);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_ITIGEKI_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *karas;

    karas = SearchObjectWork(offset, 0x52);
    OrScreenFlag(0x1000);
    if (SetChangeFrameAnime(offset, 0x96)) {
        CREATE_ARG c_arg;

        c_arg.flag = 5;
        c_arg.actno = 9;
        if (offset->dirflag) {
            c_arg.x = offset->posx + 20000;
        } else {
            c_arg.x = offset->posx - 20000;
        }
        c_arg.y = offset->posy;
        EffectCreate(offset, 0x50, &c_arg);
        offset->ply->PersonalWork[0].l = 10;
        if (karas) {
            SetActNoNext(karas, 10);
        }
    }
    offset->CommandFlag = 0xC05F;
    EndtoAction(offset, 0x97);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_ITIGEKI_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0x97);
    offset->CommandFlag = 0xC05F;
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_HW2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    EndtoAction(offset, 0x5C);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void TSAct_HW2Jump(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    SetLandBreakActno(offset, 0x5D);
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;
extern int BattleStatus;

void TSAct_HW2Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x5D)) {
        offset->Mark = 0;
    }
    if (g_info.boss[offset->padid] == 1) {
        if ((offset->attackst & 0x80000) && offset->Mark == 0) {
            offset->Mark = 1;
            SearchCharacterExecute(offset, 0x4F, 2, 0x17);
            SearchCharacterExecute(offset, 0x4F, 3, 0x17);
            SearchCharacterExecute(offset, 0x4F, 4, 0x17);
            SearchCharacterExecute(offset, 0x4F, 8, 0x17);
            SearchCharacterExecute(offset, 0x4F, 9, 0x17);
            SearchCharacterExecute(offset, 0x4F, 10, 0x17);
            SearchCharacterExecute(offset, 0x4F, 11, 0x17);
            SearchCharacterExecute(offset, 0x4F, 12, 0x17);
            if (BattleStatus < 3) {
                ObjectCreate_ActNo(offset, 0x4F, 0, 9);
            }
        }
    }
    EndtoStand(offset);
}

void TSAct_HW3(CHARACTER_WORK *offset) {
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void TSAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50)) {
        Tension_Add_ByLongHair(offset);
        offset->ply->PersonalWork[0].l = 3;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void TSAct_LongHair2(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x51)) {
        Tension_Add_ByLongHair(offset);
        offset->ply->PersonalWork[0].l = 3;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void TSAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void TSAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
extern GAME_INFOMATION g_info;
extern int BattleStatus;

void TSAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    if (SetChangeFrameAnime(offset, 0x7E)) {
        offset->Mark = 0;
    }
    if (g_info.boss[offset->padid] == 1 && offset->ActTimer == 0x38 && offset->HitStopTime == 0) {
        if (offset->Mark == 0) {
            offset->Mark = 1;
            SearchCharacterExecute(offset, 0x4F, 2, 0x17);
            SearchCharacterExecute(offset, 0x4F, 3, 0x17);
            SearchCharacterExecute(offset, 0x4F, 4, 0x17);
            SearchCharacterExecute(offset, 0x4F, 8, 0x17);
            SearchCharacterExecute(offset, 0x4F, 9, 0x17);
            SearchCharacterExecute(offset, 0x4F, 10, 0x17);
            SearchCharacterExecute(offset, 0x4F, 11, 0x17);
            SearchCharacterExecute(offset, 0x4F, 12, 0x17);
            if (BattleStatus < 3) {
                ObjectCreate_ActNo(offset, 0x4F, 0, 9);
            }
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_HW6(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x85);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_HW6H(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x89);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_HW7a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x88);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_HW7b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x92);
    EndtoStand(offset);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
extern GAME_INFOMATION g_info;

int TS_HW1InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->HitStopTime = 0;
    SetActNoNext(offset, 0x8F);
    offset->Mark &= ~1;
    eoffset->HitStopTime = 0x1A;
    if (g_info.boss[offset->padid] == 1) {
        eoffset->HitStopTime = 0x34;
    }
    return 1;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TS_HW1InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void TSAct_HW1a(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8E);
    if (offset->Mark & 1) {
        offset->ply->GuardFunc = TS_HW1InterruptProc;
        offset->GuardSt = 0x1203;
    } else {
        offset->GuardSt = 0;
        offset->ply->GuardFunc = 0;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_HW1b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x8F);
    EndtoAction(offset, 0x90);
}

int TSDamInt_PoisonHit(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    offset->ply->Poison = 0xF0;
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int GetXMove(CHARACTER_WORK *offset, int val);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TSDamInt_PoisonHit(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);
extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern int BattleStatus;

void TSAct_HW1c(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x90)) {
        if (userinfo.gguse[offset->padid] & 4) {
            offset->HitStopTime = 0x10;
            if (offset->Localid) {
                offset->Localid = 0;
            } else {
                offset->dirflag = !offset->dirflag;
            }
            offset->posx = eoffset->posx + GetXMove(offset, -7500);
        } else {
            offset->dirflag = !offset->dirflag;
            offset->posx = eoffset->posx + GetXMove(offset, -10000);
        }
        offset->Mark = 0;
    }
    if (offset->HitParam) {
        offset->HitParam->DamInt = TSDamInt_PoisonHit;
    }
    if (g_info.boss[offset->padid] == 1) {
        if ((offset->attackst & 0x80000) && offset->Mark == 0) {
            offset->Mark = 1;
            SearchCharacterExecute(offset, 0x4F, 2, 0x17);
            SearchCharacterExecute(offset, 0x4F, 3, 0x17);
            SearchCharacterExecute(offset, 0x4F, 4, 0x17);
            SearchCharacterExecute(offset, 0x4F, 8, 0x17);
            SearchCharacterExecute(offset, 0x4F, 9, 0x17);
            SearchCharacterExecute(offset, 0x4F, 10, 0x17);
            SearchCharacterExecute(offset, 0x4F, 11, 0x17);
            SearchCharacterExecute(offset, 0x4F, 12, 0x17);
            if (BattleStatus < 3) {
                ObjectCreate_ActNo(offset, 0x4F, 0, 9);
            }
        }
    }
    EndtoStand(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_Stand2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x101;
    offset->actst &= ~0x1000;
    offset->actst |= 0x40000;
    if (SetChangeFrameAnime(offset, 0x91)) {
        offset->ply->PersonalWork[0].l = 2;
    }
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSAct_SHW_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x95);
    EndtoStand(offset);
}
