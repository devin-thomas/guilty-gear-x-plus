#include "generated/fa_act.h"

int EnemyThrowCheck(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
short int GetTensionVal(CHARACTER_WORK *offset);
int GetThrowDist(void);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsYoga(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern USER_SETTING userinfo;

int FA_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x04080069);
                return 1;
            }
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x040800C8);
                return 1;
            }
        }
        if (IsYoga(offset) && IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x02080079);
            return 1;
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffPunch(offset) && !(offset->ply->ObjFlag & 1)) {
                SetAttackActNo(offset, 0x02080060);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080056);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffHighSlash(offset)) {
                if (userinfo.gguse[offset->padid] & 4) {
                    SetAttackActNo(offset, 0x020800CD);
                    return 1;
                }
                if (!EnemyThrowCheck(GetEnemyPlayer(offset)) && GetThrowDist() < 10000) {
                    SetAttackActNo(offset, 0x0208007A);
                    return 1;
                }
                SetAttackActNo(offset, 0x020800BE);
                return 1;
            }
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x0208007B);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x020800A8);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x0208007D);
                return 1;
            }
        }
    }
    if (offset->actst & 0x10) {
        if (IsHadou(offset) && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x0008005E);
            return 1;
        }
    }
    return 0;
}

int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void FA_ExciteCommnadCheck(CHARACTER_WORK *offset) {
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

void FA_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    if (Random() & 1) {
        SetAttackActNo(offset, 0x02080051);
    } else {
        SetAttackActNo(offset, 0x020800BA);
    }
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int FA_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    if (GetTensionVal(offset) >= 5000 && !(offset->actst & 0x10) && IsDouji(offset, 0xF, 2, 2)) {
        if (IsStdRight(offset) || IsSitRight(offset)) {
            offset->actst |= 0x80;
            SetAttackActNo(offset, 0x0308009C);
            return 1;
        }
    }
    return 0;
}

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);

int FA_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x66, 0x63)) {
            return 1;
        }
    }
    return 0;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsLeft(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int FAHW1CommandCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;

    if (IsLeft(offset) && !(enemy->actst & 0x110)) {
        SetAttackActNo(offset, 0x80081);
        return 0;
    }
    EndtoAction(offset, 0x83);
    return 0;
}

int IsHadou(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);

int FAHW1cCommandCheck(CHARACTER_WORK *offset) {
    if (IsHadou(offset) && IsTrgPunch(offset)) {
        offset->ply->PersonalWork[0].b[0] = 1;
    }
    return 0;
}

int IsHadou(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);

int FAHW1dCommandCheck(CHARACTER_WORK *offset) {
    if (IsHadou(offset) && IsTrgPunch(offset)) {
        offset->ply->PersonalWork[0].b[0] = 1;
    }
    return 0;
}

int IsHadou(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);

int FAHW1fCommandCheck(CHARACTER_WORK *offset) {
    if (IsHadou(offset) && IsTrgPunch(offset)) {
        offset->ply->PersonalWork[0].b[0] = 1;
    }
    return 0;
}

int IsBackStep(CHARACTER_WORK *offset);
int IsDash(CHARACTER_WORK *offset);
int IsDown(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
extern int BattleStatus;

int FAHW6CommandCheck(CHARACTER_WORK *offset) {
    if (BattleStatus == 2) {
        if (IsBackStep(offset)) {
            SetAttackActNo(offset, 0x020800A6);
            return 0;
        }
        if (IsDash(offset)) {
            SetAttackActNo(offset, 0x020800A5);
            return 0;
        }
        if (IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x02080059);
            return 0;
        }
        if (IsTrgAndOffPunch(offset)) {
            SetAttackActNo(offset, 0x020800C5);
            return 0;
        }
        if (IsTrgAndOffKick(offset)) {
            SetAttackActNo(offset, 0x020800C6);
            return 0;
        }
    }
    if (IsDown(offset)) {
        SetActNo(offset, 0xA7);
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void FAAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x9C)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void FAAct_LongHair1(CHARACTER_WORK *offset) {
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

void FAAct_LongHair4(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x52)) {
        Tension_Add_ByLongHair(offset);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void FAAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void FAAct_LongHair3(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0xBA);
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void FAAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x53);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void FAAct_AFR_Win(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xAC);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetWinActFlag(CHARACTER_WORK *offset);

void FAAct_WinB(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    OrScreenFlag(0x2000);
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x9A);
    SetJumpControl(offset);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void FAAct_WinC(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0xBB);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void FAAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x54);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_EntryA_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void FAAct_EntryB_1(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, offset->dirflag ? 0xC1 : 0x57)) {
        SetYSpeed(offset, -0x109A0, 0);
    } else {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
    }
}

void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
float nlSin(short unsigned int r);

void FAAct_EntryB_2(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->dirflag ? 0xC2 : 0x58)) {
        offset->Localid = offset->posx;
    }
    SetLandBreakActno(offset, 0x6E);
    offset->posx = offset->Localid + nlSin(offset->HitBackx) * 12000.0f;
    if (offset->dirflag) {
        offset->HitBackx -= 0x180;
    } else {
        offset->HitBackx += 0x180;
    }
}

int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_EntryB_3(CHARACTER_WORK *offset) {
    OrScreenFlag(0xE0);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, offset->dirflag ? 0xC3 : 0x6E)) {
        if (offset->dirflag) {
            offset->posx = -14400;
        } else {
            offset->posx = 14400;
        }
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW6(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x10;
    SetChangeFrameAnime(offset, 0x56U);
    EndtoAction(offset, 0xA4);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW5_IROIRONAGE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x60U);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW2_IROIRONAGE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC8U);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW7_GMW1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59U);
    EndtoAction(offset, 0x5A);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void FAAct_HW7_GMW2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5AU);
    SetLandBreakActno(offset, 0xFFFFFFFFU);
    TimetoAction(offset, 0x5B, 0x28);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FAAct_HW7_GMW3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5BU);
    SetLandBreakActno(offset, 0x20005CU);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW7_GMWLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5CU);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FAAct_HW7_SGMW(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5EU);
    SetLandBreakActno(offset, 0x20005FU);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW7_SGMWLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5FU);
    EndtoStand(offset);
}

void CmnAct_SitForwardWalk(CHARACTER_WORK *offset);

void FAAct_SitForwardWalk(CHARACTER_WORK *offset) {
    CmnAct_SitForwardWalk(offset);
}

void CmnAct_SitBackWalk(CHARACTER_WORK *offset);

void FAAct_SitBackWalk(CHARACTER_WORK *offset) {
    CmnAct_SitBackWalk(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x63U);
    EndtoStand(offset);
}

void RequestHit(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
extern short unsigned int FADmpNoTb[0];

int FADamInt_Die(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    offset->actst |= 0x4020;
    SetActNoNext(offset, FADmpNoTb[offset->idno] | 0x400000);
    RequestHit(offset, header->flag, header->lvflag, 0);
    offset->DamColliFlag = 0;
    return 1;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
extern short unsigned int FADieNoTb[0];

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;

void FAAct_ITIGEKI(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    if (SetChangeFrameAnime(offset, 0x66)) {
        offset->ply->OverKillFlag = 2;
        offset->ColliFlag = 0;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (offset->attackst & 0x80000) {
        SetFlash(1, 0xF01);
        offset->ply->MutekiTime = 0xFF;
        SetActNoNext(offset, 0x68);
        SetActNoNext(enemy, FADieNoTb[enemy->idno]);
    }
    EndtoAction(offset, 0x67);
}

void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
/* FADamInt_Die: no debug record */

int FADamInt_Die(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void FAAct_ITIGEKI_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    OrScreenFlag(0x1000);
    SetAnten(offset, 0);
    offset->actst |= 1;
    enemy->actst |= 0x4000;
    if (SetChangeFrameAnime(offset, 0x68)) {
        offset->posy = -65000;
        offset->HitParam->DamInt = FADamInt_Die;
    }
    if (offset->attackst & 0x80000) {
        SetActNoNext(offset, 0x64);
    }
    offset->posx = enemy->posx;
    SetJumpControl(offset);
}

void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

extern u_int TotalCounter;

void FAAct_ITIGEKI_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    OrScreenFlag(0x1000);
    enemy->actst |= 0x4000;
    SetChangeFrameAnime(offset, 0x64);
    if (TotalCounter & 3) {
        offset->angle += 0x4000;
    }
    SetLandBreakActno(offset, 0x65);
    offset->actst |= 0x4000;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_ITIGEKI_3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    enemy->yspeed = 0;
    enemy->actst |= 0x404000;
    SetChangeFrameAnime(offset, 0x65);
    EndtoAction(offset, 0xA1);
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreateA(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_ITIGEKI_4(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    CREATE_ARG arg;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    enemy->yspeed = 0;
    enemy->actst |= 0x400000;
    SetChangeFrameAnime(offset, 0xA1);
    if (offset->Mark == 1) {
        offset->Mark = 2;
        arg.flag = 5;
        arg.actno = 0xB;
        arg.x = offset->posx;
        arg.y = offset->posy;
        ObjectCreateA(enemy, 0x2B, &arg);
        arg.x = enemy->posx;
        arg.y = enemy->posy;
        arg.actno = 0xA;
        ObjectCreateA(offset, 0x2B, &arg);
        offset->actst &= ~0x4000;
        enemy->actst &= ~0x4000;
    }
    if (offset->Mark == 0) {
        offset->actst |= 0x4000;
        enemy->actst |= 0x4000;
    }
    offset->CommandFlag &= ~0x800;
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_KILL_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67U);
    EndtoStand(offset);
}

void OrScreenFlag(int flag);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetFlash(char time, short unsigned int flag);

extern int BattleStatus;

int FA_SHW_InterruptProc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    u_short acttb[19] = {
        0x00, 0x98, 0x8B, 0x91, 0x89, 0x6C, 0x92, 0xA0, 0x85, 0x7A,
        0x94, 0x73, 0x9A, 0x78, 0x8A, 0x8A, 0x78, 0x80, 0x80,
    };

    if (BattleStatus != 2) {
        return 0;
    }
    if (enemy->actst & 0x10000) {
        return 0;
    }
    if (enemy->actst & 0x100) {
        enemy->ply->GuardPoint -= 0x40;
        if (enemy->ply->GuardPoint < -0x80) {
            enemy->ply->GuardPoint = -0x80;
        }
    }
    SetActNo(enemy, acttb[enemy->idno]);
    enemy->yspeed = 0;
    enemy->HitStopTime = 0;
    enemy->DamColliFlag = 0;
    enemy->xspeed = 0;
    enemy->actst |= 0x4020;
    OrScreenFlag(0x1000);
    SetFlash(6, 0xF01);
    SetActNo(offset, 0x89);
    OrScreenFlag(0x1000);
    offset->HitStopTime = 0;
    offset->actst |= 0x4000;
    return 1;
}

int FA_SHW_InterruptProc(CHARACTER_WORK *offset);

void FAActSHWCheck(CHARACTER_WORK *offset) {
    offset->ply->AtkHitInterrupt = FA_SHW_InterruptProc;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void FAActSHWCheck(CHARACTER_WORK *offset);
int Random(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void set_random_seed_abs(void);

extern GAME_INFOMATION g_info;

void FAAct_SHW1_1(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x69)) {
        offset->ColliFlag = 0;
        offset->ply->PersonalWork[0].b[2] = Random() & 3;
    }
    FAActSHWCheck(offset);
    if ((offset->attackst & 0x1000) && !(offset->actst & 0x4000)) {
        SetActNoNext(offset, 0x6C);
    }
    EndtoAction(offset, 0x6C);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
int Random(void);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void set_random_seed_abs(void);

extern GAME_INFOMATION g_info;
extern RECTL ScreenObj;

void FAAct_SHW1_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = offset->ply->Enemy;
    int acttb[4] = { 0x6A, 0x98, 0x99, 0x9B };
    CREATE_ARG arg;
    int i;

    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    OrScreenFlag(0x1000);
    if (SetChangeFrameAnime(offset, 0x89)) {
        offset->ply->PersonalWork[0].b[1] = 0;
        arg.flag = 0x25;
        arg.actno = 6;
        arg.y = -20000;
        arg.x = ScreenObj.x - 0x55F0;
        for (i = 0; i < 4; i++) {
            arg.Localid = i;
            EffectCreate(offset, 0x40, &arg);
            arg.x += 16000;
        }
    } else {
        if (offset->ActTimer > 120) {
            offset->ply->PersonalWork[0].b[1] = (Random() & 3) + 1;
            offset->ply->PersonalWork[0].b[2] = 1;
        }
        if (offset->ply->PersonalWork[0].ub[1]) {
            EndtoAction(offset, acttb[offset->ply->PersonalWork[0].ub[2]]);
        } else {
            if (IsTrgAndOffPunch(enemy)) {
                offset->ply->PersonalWork[0].b[1] = 1;
            }
            if (IsTrgAndOffKick(enemy)) {
                offset->ply->PersonalWork[0].b[1] = 2;
            }
            if (IsTrgAndOffSlash(enemy)) {
                offset->ply->PersonalWork[0].b[1] = 3;
            }
            if (IsTrgAndOffHighSlash(enemy)) {
                offset->ply->PersonalWork[0].b[1] = 4;
            }
        }
    }
    offset->actst |= 0x4000;
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);

void MakeNega(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    arg.flag = 0x104;
    arg.actno = 6;
    EffectCreate(offset, 0x4A, &arg);
    offset->attackst &= ~0x80000;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void MakeNega(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW1_3a(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    OrScreenFlag(0x1000);
    if (SetChangeFrameAnime(offset, 0x6A)) {
        offset->HitParam->DownFlag |= 0x100;
    }
    offset->actst |= 0x4000;
    enemy->xspeed = 0;
    enemy->yspeed = 0;
    enemy->actst |= 0x4000;
    if (offset->attackst & 0x80000) {
        MakeNega(offset);
    }
    EndtoAction(offset, 0x6B);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void MakeNega(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW1_3b(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    OrScreenFlag(0x1000);
    if (SetChangeFrameAnime(offset, 0x98)) {
        offset->HitParam->DownFlag |= 0x100;
    }
    offset->actst |= 0x4000;
    enemy->xspeed = 0;
    enemy->yspeed = 0;
    enemy->actst |= 0x4000;
    if (offset->attackst & 0x80000) {
        MakeNega(offset);
    }
    EndtoAction(offset, 0x6B);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void MakeNega(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW1_3c(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    OrScreenFlag(0x1000);
    if (SetChangeFrameAnime(offset, 0x99)) {
        offset->HitParam->DownFlag |= 0x100;
    }
    offset->actst |= 0x4000;
    enemy->yspeed = 0;
    enemy->xspeed = 0;
    enemy->actst |= 0x4000;
    if (offset->attackst & 0x80000) {
        MakeNega(offset);
    }
    EndtoAction(offset, 0x6B);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *ObjectCreateA(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
void OrScreenFlag(int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW1_3d(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    CREATE_ARG arg;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    SetAnten(offset, 0);
    OrScreenFlag(0x1000);
    SetChangeFrameAnime(offset, 0x9B);
    if (offset->Mark) {
        arg.flag = 5;
        arg.actno = 9;
        arg.x = offset->posx;
        arg.y = offset->posy;
        ObjectCreateA(enemy, 0x2B, &arg);
        SetActNoNext(enemy, 0);
        enemy->actst &= ~0x10;
        offset->actst &= ~0x4000;
    } else {
        offset->actst |= 0x4000;
    }
    EndtoAction(offset, 0);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW1_RTNa(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6BU);
    EndtoAction(offset, 0x6C);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SHW1_RTNb(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x6CU);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int FAHW1CommandCheck(CHARACTER_WORK *offset);

void FAAct_HW1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    COLLISION *col;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x79)) {
        offset->ply->PersonalWork[0].b[0] = 0;
    }
    if (offset->attackst & 0x80000) {
        enemy = offset->ply->Enemy;
        if (!(enemy->actst & 0x110)) {
            offset->ply->ExCommandFunc = FAHW1CommandCheck;
            col = Get_GCLExNo(offset, 1);
            enemy->yspeed = -1400;
            enemy->xspeed = 0;
            enemy->actst |= 0x400000;
            enemy->posy = 0;
            if (col) {
                if (offset->dirflag == 1) {
                    enemy->posx = offset->posx - col->x * 100;
                } else {
                    enemy->posx = offset->posx + col->x * 100;
                }
            }
        } else {
            EndtoAction(offset, 0x83);
        }
    } else {
        EndtoAction(offset, 0x83);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW1b(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x83)) {
        offset->ply->PersonalWork[0].b[0] = 0;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int FAHW1cCommandCheck(CHARACTER_WORK *offset);

void FAAct_HW1c(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    offset->ply->ExCommandFunc = FAHW1cCommandCheck;
    if (!(enemy->actst & 0x10)) {
        enemy->xspeed = 0;
        enemy->yspeed = 0;
        enemy->posy = 0;
        if (offset->Mark == 0) {
            enemy->actst |= 0x400000;
        }
    }
    if (SetChangeFrameAnime(offset, 0x81)) {
        offset->ply->PersonalWork[0].ub[0] = 0;
    }
    if (offset->ply->PersonalWork[0].ub[0]) {
        EndtoAction(offset, 0x80082);
    } else {
        EndtoStand(offset);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

int FAHW1dCommandCheck(CHARACTER_WORK *offset);

void FAAct_HW1d(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    if (SetChangeFrameAnime(offset, 0x82)) {
        offset->ply->PersonalWork[0].ub[0] = 0;
    } else if (offset->attackst & 0x80000) {
        if (enemy->yspeed < 0) {
            offset->ply->ExCommandFunc = FAHW1dCommandCheck;
            if (offset->ply->PersonalWork[0].ub[0] && enemy->posy <= -20000) {
                SetActNoNext(offset, 0x80085);
            }
        } else {
            offset->ply->ExCommandFunc = 0;
            SetActNoNext(offset, 0x84);
        }
    } else {
        offset->ply->ExCommandFunc = 0;
        EndtoStand(offset);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW1e(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x84)) {
        offset->ply->PersonalWork[0].b[0] = 0;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

int FAHW1fCommandCheck(CHARACTER_WORK *offset);

void FAAct_HW1f(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x85)) {
        offset->ply->PersonalWork[0].ub[0] = 0;
    } else {
        offset->ply->ExCommandFunc = FAHW1fCommandCheck;
        if (offset->yspeed > 0) {
            if (offset->ply->PersonalWork[0].ub[0]) {
                offset->ply->ExCommandFunc = 0;
                EndtoAction(offset, 0x80086);
            } else {
                offset->ply->ExCommandFunc = 0;
                EndtoAction(offset, 8);
            }
        }
    }
    SetJumpControl(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void FAAct_HW1g(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x86)) {
        offset->ply->PersonalWork[0].b[0] = 0;
    }
    SetJumpControl(offset);
    EndtoAction(offset, 8);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW9(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x7A)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].l = 2;
    } else if (IsTrgKick(offset)) {
        offset->Localid++;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsActEnd(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW9_LOOP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x80);
    if (IsTrgSlash(offset)) {
        offset->Localid++;
    }
    if (IsActEnd(offset)) {
        if (offset->Localid) {
            if (offset->Localid >= 3) {
                offset->Localid = 2;
            } else {
                offset->Localid = 0;
            }
            EndtoAction(offset, 0x80);
        } else if (offset->ply->PersonalWork[0].l > 0) {
            if (EndtoAction(offset, 0x80)) {
                offset->ply->PersonalWork[0].l--;
            }
        } else {
            EndtoAction(offset, 0);
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW9X(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0xCD)) {
        offset->Localid = 0;
        offset->ply->PersonalWork[0].l = 2;
    } else if (IsTrgKick(offset)) {
        offset->Localid++;
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsActEnd(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW9X_LOOP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xCE);
    if (IsTrgSlash(offset)) {
        offset->Localid++;
    }
    if (IsActEnd(offset)) {
        if (offset->Localid) {
            if (offset->Localid >= 3) {
                offset->Localid = 2;
            } else {
                offset->Localid = 0;
            }
            EndtoAction(offset, 0xCE);
        } else if (offset->ply->PersonalWork[0].l > 0) {
            if (EndtoAction(offset, 0xCE)) {
                offset->ply->PersonalWork[0].l--;
            }
        } else {
            EndtoAction(offset, 0);
        }
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetXMove(CHARACTER_WORK *offset, int val);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW8a1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    if (SetChangeFrameAnime(offset, 0x7B)) {
        offset->ply->PersonalWork[1].l = enemy->posx;
    }
    if (EndtoAction(offset, 0x7C)) {
        offset->posx = offset->ply->PersonalWork[1].l - GetXMove(offset, 20000);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetXMove(CHARACTER_WORK *offset, int val);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW8b1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    if (SetChangeFrameAnime(offset, 0x7B)) {
        offset->ply->PersonalWork[1].l = enemy->posx;
    }
    if (EndtoAction(offset, 0x7C)) {
        offset->posx = offset->ply->PersonalWork[1].l - GetXMove(offset, -20000);
        if (offset->Mark == 0) {
            offset->Mark = 1;
            offset->dirflag = !offset->dirflag;
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW8a2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7CU);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW8c1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    offset->CommandFlag = 0xC05F;
    enemy = offset->ply->Enemy;
    if (SetChangeFrameAnime(offset, 0x7D)) {
        offset->ply->PersonalWork[1].l = enemy->posx;
    }
    if (EndtoAction(offset, 0x7E)) {
        offset->posy = -65000;
        offset->posx = offset->ply->PersonalWork[1].l;
    }
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FAAct_HW8c2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7EU);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 0x7FU);
    }
    SetLandBreakActno(offset, 0x7FU);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FAAct_HW8c3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x7FU);
    SetLandBreakActno(offset, 0xC7U);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW8cLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC7U);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);

void FAAct_SP1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    SetChangeFrameAnime(offset, 0x6DU);
    offset->ply->SkyDashCorrect = 0;
    offset->ply->maintain = 0;
    SetLandBreakActno(offset, 0xBDU);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_SP1Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xBDU);
    EndtoAction(offset, 0);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int g_roundmin(int val, int min);

void FAAct_Throw(CHARACTER_WORK *offset) {
    static short posyTbByidno[19] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x8E);
    EndtoAction(offset, 0x8F);
    if (offset->Mark) {
        offset->posy += -(posyTbByidno[offset->idno] * 100) / 5;
        offset->posy = g_roundmin(offset->posy, -posyTbByidno[offset->idno] * 100);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFAThrowNAMEDamageActNo(CHARACTER_WORK *offset);

void FAAct_ThrowNAME(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x8FU);
    if (offset->Mark != 0) {
        SetFAThrowNAMEDamageActNo((CHARACTER_WORK *) offset->ply->Enemy);
        offset->Mark = 0;
    }
    EndtoAction(offset, 0x90);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FAAct_ThrowJUMP(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x90U);
    SetLandBreakActno(offset, 0x91U);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_ThrowKETSU(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x91);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

int FAHW6CommandCheck(CHARACTER_WORK *offset);
extern int BattleStatus;

void FAAct_HW6_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x10;
    SetChangeFrameAnime(offset, 0xA4);
    offset->ply->ExCommandFunc = FAHW6CommandCheck;
    TimetoAction(offset, 0xA7, 180);
    if (BattleStatus == 3) {
        SetActNoNext(offset, 0xA7);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW6_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x10;
    SetChangeFrameAnime(offset, 0xA5U);
    EndtoAction(offset, 0xA4);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW6_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x10;
    SetChangeFrameAnime(offset, 0xA6U);
    EndtoAction(offset, 0xA4);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW6_5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= 0xFFFFFFEF;
    SetChangeFrameAnime(offset, 0xA7U);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW6_KubiNobashi(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC5U);
    EndtoAction(offset, 0xA4);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW6_Flower(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0xC6U);
    EndtoAction(offset, 0xA4);
}

int Random(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

extern SURVIVAL_INFOMATION *sv_info;

void SetActFANG(CHARACTER_WORK *poffset, CHARACTER_WORK *offset) {
    int acttb[16] = { 2, 2, 3, 3, 5, 6, 6, 6, 1, 1, 1, 1, 4, 4, 4, 4 };

    if (((g_info.mode & 0x200) && sv_info->sv_ext.battle && poffset->padid == (sv_info->ply == 0))
        || g_info.boss[offset->padid] == 1) {
        SetActNoNext(offset, 5);
    } else {
        SetActNoNext(offset, acttb[Random() & 0xF]);
    }
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FAAct_HW9Sippai(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= 0xFFFFFFEF;
    SetChangeFrameAnime(offset, 0xBEU);
    EndtoStand(offset);
}
