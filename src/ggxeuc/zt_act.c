#include "generated/zt_act.h"

extern GAME_INFOMATION g_info;

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int Random(void);
void set_random_seed_abs(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
int EndtoSit(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
extern PAD GameKey[2];
extern TRAINING_SETTING *tr_info;
extern PAD pads[2];
int IsHoldKick(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
extern short int Jump_GravityByIdno[];
int LandtoAction(CHARACTER_WORK *offset, int actno);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);
int EndtoDemo(CHARACTER_WORK *offset);
void SetWinActFlag(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
void Tension_Dec(CHARACTER_WORK *offset, int Val);
void SetAnten(CHARACTER_WORK *offset, int flag);
int ZTHW6ExCommandCheck(CHARACTER_WORK *offset);
int ZTHW6ExCommandCheck2(CHARACTER_WORK *offset);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);

short int GetTensionVal(CHARACTER_WORK *offset);
int IsFireWheel(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsSitaSita(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int ZT_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsSitaSita(offset)) {
            if (!(offset->ply->ObjFlag & 1) && IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x2080057);
                return 1;
            }
            if (!(offset->ply->ObjFlag & 2) && IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x2080071);
                return 1;
            }
        }
        if (IsFireWheel(offset)) {
            if (GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x408006A);
                return 1;
            }
            if ((offset->ply->ObjFlag & 4) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x408009D);
                return 1;
            }
        }
        if (~offset->ply->ObjFlag & 4) {
            if (offset->ply->PersonalWork[0].ub[1] && IsHadou(offset)) {
                if (IsTrgAndOffPunch(offset)) {
                    if (SetAttackActNo(offset, 0x2080055)) {
                        offset->ply->PersonalWork[0].b[0] = 1;
                    }
                    return 1;
                }
                if (IsTrgAndOffKick(offset)) {
                    if (SetAttackActNo(offset, 0x2080055)) {
                        offset->ply->PersonalWork[0].b[0] = 2;
                    }
                    return 1;
                }
                if (IsTrgAndOffSlash(offset)) {
                    if (SetAttackActNo(offset, 0x2080055)) {
                        offset->ply->PersonalWork[0].b[0] = 3;
                    }
                    return 1;
                }
                if (IsTrgAndOffHighSlash(offset)) {
                    if (SetAttackActNo(offset, 0x2080055)) {
                        offset->ply->PersonalWork[0].b[0] = 0;
                    }
                    return 1;
                }
            }
        } else if (IsHadou(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x2080056);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x2080056);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x2080056);
                return 1;
            }
            if (IsTrgAndOffHighSlash(offset)) {
                SetAttackActNo(offset, 0x2080056);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x2080065);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x2080062);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (GetTensionVal(offset) >= 5000 && IsHadoux2(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x2080058);
                return 1;
            }
        }
        if (userinfo.gguse[offset->padid] & 2) {
            if (IsSitaSita(offset)) {
                if (!(offset->ply->ObjFlag & 1) && IsTrgAndOffSlash(offset)) {
                    SetAttackActNo(offset, 0x20800A1);
                    return 1;
                }
                if (!(offset->ply->ObjFlag & 2) && IsTrgAndOffHighSlash(offset)) {
                    SetAttackActNo(offset, 0x20800A2);
                    return 1;
                }
            }
        }
    }
    return 0;
}

void ZT_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x2080050);
    } else {
        SetAttackActNo(offset, 0x2080051);
    }
}

void ZT_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x2080052);
    } else {
        SetAttackActNo(offset, 0x2080053);
    }
}

int CmnGuardSpecialAttackCheck(CHARACTER_WORK *offset, int actno);

int ZT_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    return CmnGuardSpecialAttackCheck(offset, 0x87);
}

int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);

int ZT_CommandThrowCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsSyoryu(offset) && IsTrgAndOffSlash(offset)) {
            return 5;
        }
    }
    return 0;
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsSyoryu(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);

int ZT_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x5F, 0x5C)) {
            return 1;
        }
        if (IsSyoryu(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x208007C);
                return 1;
            }
        }
    }
    return 0;
}

int IsLeft(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);

int ZTHW6ExCommandCheck(CHARACTER_WORK *offset) {
    if (!IsHoldKick(offset)) {
        SetAttackActNo(offset, 0x67);
        return 1;
    }
    if (IsLeft(offset)) {
        SetAttackActNo(offset, 0x69);
        return 1;
    }
    if (IsRight(offset)) {
        SetAttackActNo(offset, 0x68);
        return 1;
    }
    return 0;
}

int ZTHW6ExCommandCheck2(CHARACTER_WORK *offset) {
    if (!IsHoldKick(offset)) {
        SetAttackActNo(offset, 0x67);
        return 1;
    }
    return 0;
}

void ZTAct_HW3Fall(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, 0x58);
    if (IsActTimeOver(offset, 50)) {
        if (offset->dirflag == 1 && offset->posx > 0) {
            if (offset->actst & 0x2800) {
                SetAttackActNo(offset, 0x59);
            }
        } else if (offset->dirflag == 0 && offset->posx < 0) {
            if (offset->actst & 0x2800) {
                SetAttackActNo(offset, 0x59);
            }
        }
    }
    TimetoAction(offset, 0x59, 0xF0);
}

void ZTAct_HW3Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59U);
    SetLandBreakActno(offset, 0x15U);
}

void ZTAct_HW1_ED(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55U);
    EndtoStand(offset);
}

void ZTAct_HW1_EDBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x56U);
    EndtoStand(offset);
}

void ZTAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x5AU);
    EndtoDemo(offset);
}

void ZTAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x5BU);
    EndtoDemo(offset);
}

void ZTAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

void ZTAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50U) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void ZTAct_LongHair2(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x51U) != 0) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

void ZTAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x52U);
    EndtoStand(offset);
}

void ZTAct_LongHair4(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x53U);
    EndtoStand(offset);
}

void ZTAct_HW4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62U);
    EndtoStand(offset);
}

void ZTAct_HW2_DR(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    EndtoStand(offset);
}

void ZTAct_HW2_DR2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x71);
    EndtoStand(offset);
}

void ZTAct_SKY_DR(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA1);
    SetLandBreakActno(offset, 0xA3);
}

void ZTAct_SKY_DR2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA2);
    SetLandBreakActno(offset, 0xA3);
}

void ZTAct_SKY_DRLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xA3);
    EndtoSit(offset);
}

void ZTAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= 0xFFFFFBFF;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x5C);
    EndtoStand(offset);
}

void ZTAct_ITIGEKI_L2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    EndtoAction(offset, 0x5F);
}

void ZTAct_ITIGEKI_L3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoAction(offset, 0x5F);
}

COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);

void ZTItigeki_SetEnemyPos(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    COLLISION *colli;

    colli = Get_GCLExNo(offset, 0);
    if (colli) {
        enemy->xspeed = 0;
        enemy->yspeed = 0;
        enemy->actst |= 0x400000;
        if (offset->dirflag == 1) {
            enemy->posx = offset->posx - colli->x * 100;
        } else {
            enemy->posx = offset->posx + colli->x * 100;
        }
        enemy->posy = offset->posy + (colli->y + 120) * 100;
    }
}

void ZTItigeki_SetEnemyPos(CHARACTER_WORK *offset);

void ZTAct_ITIGEKI(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x5F)) {
        offset->ply->OverKillFlag = 2;
        offset->ColliFlag = 0;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        ZTItigeki_SetEnemyPos(offset);
        SetActNoNext(offset, 0x60);
        return;
    }
    EndtoStand(offset);
}

void ZTItigeki_SetEnemyPos(CHARACTER_WORK *offset);

void ZTAct_ITIGEKI_Face(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    ZTItigeki_SetEnemyPos(offset);
    SetChangeFrameAnime(offset, 0x60);
    EndtoStand(offset);
}

void AddXSpeed(CHARACTER_WORK *offset, short int xspeed);
void DirectionDirectChange(CHARACTER_WORK *offset);
int IsDown(CHARACTER_WORK *offset);
int IsHitStop(CHARACTER_WORK *offset);
int IsLeft(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
int IsUp(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void ZTAct_Fly(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC13F;
    if (SetChangeFrameAnime(offset, 0x61)) {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, -800, 70);
    }
    if (!IsHitStop(offset)) {
        if (IsRight(offset)) {
            AddXSpeed(offset, 70);
        }
        if (IsLeft(offset)) {
            AddXSpeed(offset, -70);
        }
        if (IsUp(offset)) {
            offset->yspeed -= 40;
        }
        if (IsDown(offset)) {
            offset->yspeed += 100;
        }
        if (offset->posy < -50000) {
            offset->yspeed -= 25;
        } else if (offset->posy < -40000) {
            offset->yspeed -= 40;
        } else {
            offset->yspeed -= 55;
        }
        offset->yspeed -= offset->yspeed / 25;
        offset->xspeed -= offset->xspeed / 15;
        if (abs(offset->xspeed) < 15) {
            offset->xspeed = 0;
        }
    }
    DirectionDirectChange(offset);
    SetLandBreakActno(offset, 0x200015);
    TimetoAction(offset, 0x10008, 100);
}

void ZTAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x63);
}

void ZTAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x64);
    EndtoStand(offset);
}

void ZTAct_HW6_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x65) != 0) {
        offset->ply->PersonalWork[0].w[1] = 0;
    }
    EndtoAction(offset, 0x66);
}

void ZTAct_HW6_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = ZTHW6ExCommandCheck;
    offset->actst |= 0x100000;
    SetChangeFrameAnime(offset, 0x66);
    offset->ply->PersonalWork[0].uw[1]++;
    if (offset->ply->PersonalWork[0].uw[1] > 0x78) {
        SetAttackActNo(offset, 0x67);
    }
}

int IsRight(CHARACTER_WORK *offset);

void ZTAct_HW6_F(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = ZTHW6ExCommandCheck2;
    SetChangeFrameAnime(offset, 0x68);
    offset->actst |= 0x100000;
    if (!IsRight(offset)) {
        SetActNoNext(offset, 0x66);
    }
    offset->ply->PersonalWork[0].uw[1]++;
    if (offset->ply->PersonalWork[0].uw[1] > 0x78) {
        SetAttackActNo(offset, 0x67);
    }
}

int IsLeft(CHARACTER_WORK *offset);

void ZTAct_HW6_B(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->ExCommandFunc = ZTHW6ExCommandCheck2;
    SetChangeFrameAnime(offset, 0x69);
    offset->actst |= 0x100000;
    if (!IsLeft(offset)) {
        SetActNoNext(offset, 0x66);
    }
    offset->ply->PersonalWork[0].uw[1]++;
    if (offset->ply->PersonalWork[0].uw[1] > 0x78) {
        SetAttackActNo(offset, 0x67);
    }
}

void ZTAct_HW6_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    EndtoStand(offset);
}

void ZTAct_SHW(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6A);
    EndtoStand(offset);
}

void ZTAct_SHW3_ED(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x9D);
    EndtoStand(offset);
}

void ZTAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & 0xFFFFFBFF) | 0x40;
    SetChangeFrameAnime(offset, 0x54);
    EndtoStand(offset);
}

void ZTAct_ThrowEX(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x7B);
    if (offset->Mark) {
        c_arg.flag = 0x100;
        EffectCreate_ActNo(offset, 0x33, &c_arg, 1);
        EffectCreate_ActNo(offset, 0x33, &c_arg, 2);
    }
}

void ZTAct_ThrowEXMiss(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x7C);
}

void ZTAct_ThrowEXBack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x7D);
}

int GetXMove(CHARACTER_WORK *offset, int val);

void ZTAct_EntryC_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    if (SetChangeFrameAnime(offset, 0x7F)) {
        offset->posx = GetXMove(offset, -0x1F40);
    }
    if (enemy->Mark) {
        SetActNoNext(offset, 0x80);
    }
}

void ZTAct_EntryC_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetChangeFrameAnime(offset, 0x80);
    if (enemy->Mark & 2) {
        SetActNoNext(offset, 0x94);
    }
}

void ZTAct_EntryC_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x94);
    EndtoStand(offset);
}

void ZTAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= 0xFFFFFBFF;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= 0xFFFFFDFF;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x87) != 0) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}
