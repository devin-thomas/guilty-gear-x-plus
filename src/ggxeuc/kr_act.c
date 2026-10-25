#include "generated/kr_act.h"

short int GetTensionVal(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHadoux2(CHARACTER_WORK *offset);
int IsKRK(CHARACTER_WORK *offset);
int IsRendaSlash(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int KR_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsHadoux2(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x0408005E);
            return 1;
        }
        if (IsKRK(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x04080066);
            return 1;
        }
        if (IsHadou(offset)) {
            if (IsTrgAndOffPunch(offset)) {
                SetAttackActNo(offset, 0x0208005C);
                return 1;
            }
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x0208005D);
                return 1;
            }
        }
        if (IsTatsumaki(offset)) {
            if (IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080062);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x0208005F);
                return 1;
            }
        }
        if (IsTrgSlash(offset) && IsRendaSlash(offset)) {
            SetAttackActNo(offset, 0x02080064);
            return 1;
        }
    }
    return 0;
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void KR_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    SetAttackActNo(offset, 0x02080050);
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void KR_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    SetAttackActNo(offset, 0x02080051);
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int KR_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    if (GetTensionVal(offset) >= 5000 && !(offset->actst & 0x10) && IsDouji(offset, 0xF, 2, 2)) {
        if (IsStdRight(offset) || IsSitRight(offset)) {
            offset->actst |= 0x80;
            SetAttackActNo(offset, 0x0308006A);
            return 1;
        }
    }
    return 0;
}

int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);
int Random(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int KR_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x57, 0x55)) {
            return 1;
        }
        if (IsTatsumaki(offset) && IsTrgAndOffPunch(offset)) {
            if (Random() & 3) {
                SetAttackActNo(offset, 0x02080067);
                return 1;
            } else {
                SetAttackActNo(offset, 0x02080069);
                return 1;
            }
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);

void KRAct_LongHair1(CHARACTER_WORK *offset) {
    int i;
    int dir;
    CREATE_ARG c_arg;

    SetExciteCommnadFlag(offset);
    if (SetChangeFrameAnime(offset, 0x50) != 0) {
        Tension_Add_ByLongHair(offset);
        offset->ply->PersonalWork[1].ub[1] = 0;
        if (offset->ply->PersonalWork[1].ub[0]) {
            offset->ply->PersonalWork[1].ub[1] = 1;
        }
    }
    c_arg.flag = 2;
    c_arg.x = 0x1900;
    c_arg.y = -0x1F40;
    dir = offset->dirflag == 1;
    if (!offset->ply->PersonalWork[1].ub[1]) {
        for (i = 0; i < 4; i++) {
            if (offset->ActTimer == i * 12 + 6 && offset->HitStopTime == 0) {
                if (i != 1) {
                    ObjectCreate_ActNo(offset, 0x57, &c_arg, dir * 15 + 12 + i * 3);
                } else if (!(Random() & 0xF)) {
                    ObjectCreate_ActNo(offset, 0x57, &c_arg, dir * 15 + 24);
                } else {
                    ObjectCreate_ActNo(offset, 0x57, &c_arg, dir * 15 + 15);
                }
            }
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void KRAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_EntryA_1(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x52)) {
        c_arg.flag = 2;
        c_arg.x = 0;
        c_arg.y = 0;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 0x2C);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_EntryA_2(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    SetChangeFrameAnime(offset, 0x53);
    if (offset->ActTimer == 12) {
        offset->Mark = 1;
    }
    if (offset->ActTimer == 32) {
        c_arg.flag = 2;
        c_arg.x = 0;
        c_arg.y = -50000;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 0x2B);
    }
    offset->CommandFlag = 0xC05F;
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_EntryB_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0x9B);
    offset->CommandFlag = 0xC05F;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_EntryB_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0x9C);
    offset->CommandFlag = 0xC05F;
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_EntryB_1x(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0x9E);
    offset->CommandFlag = 0xC05F;
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void KRAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x56);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int IsTrgPunch(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

void KRAct_SAKE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x67);
    if (offset->ActTimer >= 9 && offset->ActTimer < 21) {
        if (IsTrgPunch(offset)) {
            SetAttackActNo(offset, 0x80068);
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_SAKE_Attack(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x68);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int sound_request(int no);

void KRAct_SAKE_Failed(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x69)) {
        c_arg.flag = 5;
        c_arg.x = offset->posx;
        c_arg.y = offset->posy - 10000;
        c_arg.actno = 0;
        EffectCreate(offset, 0x6F, &c_arg);
        sound_request(0x1D);
    }
    EndtoStand(offset);
}

int EndtoDrop(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void KRAct_DUST2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 0x80;
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 0x5B)) {
        offset->ply->FaintPoint += 0x400;
    }
    EndtoDrop(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Hissatu1_Lv1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Hissatu1_Lv2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5D);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Hissatu1_Lv3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Hissatu2_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5F);
    EndtoAction(offset, 0x60);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void RqJmpEff(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Hissatu2_Sky(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x10;
    if (SetChangeFrameAnime(offset, 0x60)) {
        RqJmpEff(offset);
    }
    EndtoAction(offset, 0x61);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void ReqCmnSE_JumpLand(CHARACTER_WORK *offset);
void RqLndEff(CHARACTER_WORK *offset);

void KRAct_Hissatu2_Land(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x10;
    if (SetChangeFrameAnime(offset, 0x61)) {
        RqLndEff(offset);
        ReqCmnSE_JumpLand(offset);
    }
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void RqJmpEff(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void KRAct_Hissatu3_Start(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x62)) {
        RqJmpEff(offset);
    }
    EndtoAction(offset, 0x63);
}

CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KRAct_Hissatu3_Sky(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x63);
    if (!(offset->ActTimer & 3) && offset->HitStopTime == 0) {
        c_arg.flag = 1;
        c_arg.x = offset->posx;
        c_arg.y = 0;
        ObjectCreate_ActNo(offset, 0x61, &c_arg, 1);
    }
    SetLandBreakActno(offset, 0x15);
}

int IsTrgSlash(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void KRAct_Hissatu4_Loop(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x64)) {
        offset->ply->PersonalWork[0].l = 16;
    }
    if (!(offset->ActTimer & 7) && offset->HitStopTime == 0) {
        c_arg.flag = 2;
        c_arg.x = -0x1900;
        c_arg.y = 0;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 3);
        c_arg.flag = 0x82;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 3);
    }
    if (IsTrgSlash(offset)) {
        offset->ply->PersonalWork[0].l += 15;
    }
    if (offset->ActTimer + 30 < offset->ply->PersonalWork[0].l) {
        offset->ply->PersonalWork[0].l = offset->ActTimer + 30;
    }
    if (offset->ply->PersonalWork[0].l > 300) {
        offset->ply->PersonalWork[0].l = 300;
    }
    TimetoAction(offset, 0x65, offset->ply->PersonalWork[0].l);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_Hissatu4_End(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x65);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void KRAct_Kakusei_1(CHARACTER_WORK *offset) {
    int x_temp;
    int i;
    CREATE_ARG c_arg;

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x66);
    if (offset->ActTimer > 60 && offset->ActTimer < 150 && !(offset->ActTimer & 3) && offset->HitStopTime == 0) {
        if (g_info.mode & 0x4000) {
            set_random_seed_abs();
            for (i = 0; i <= (offset->ActTimer - 60) / 4; i++) {
                Random();
            }
        }
        x_temp = (Random() % 320) * 100 + 9600;
        c_arg.flag = 2;
        c_arg.x = x_temp;
        c_arg.y = 0;
        if (x_temp > 24960) {
            ObjectCreate_ActNo(offset, 0x57, &c_arg, 5);
        } else if (x_temp < 16640 || (Random() & 4)) {
            ObjectCreate_ActNo(offset, 0x57, &c_arg, 4);
        } else {
            ObjectCreate_ActNo(offset, 0x57, &c_arg, 5);
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void KRAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x6A)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KRAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

void DeleteEffectByItigeki(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;
extern int BattleStatus;

void KRAct_ITIGEKI(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x57)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if ((offset->attackst & 0x80000) && BattleStatus == 2) {
        DeleteEffectByItigeki(offset);
        SetFlash(1, 0xF01);
        if (offset->dirflag == 1 && offset->posx > eoffset->posx) {
            offset->dirflag = 0;
        }
        if (offset->dirflag == 0 && offset->posx < eoffset->posx) {
            offset->dirflag = 1;
        }
        eoffset->ply->MutekiTime = 8;
        SetActNoNext(offset, 0x59);
    } else {
        EndtoAction(offset, 0x58);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void KRAct_ITIGEKI2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;
    CREATE_ARG c_arg;

    eoffset = GetEnemyPlayer(offset);
    if (SetChangeFrameAnime(offset, 0x59)) {
        if (eoffset->actst & 0x10) {
            SetYSpeed(eoffset, -1, 0);
        }
        eoffset->HitStopTime = offset->HitStopTime;
        offset->ply->MutekiTime = 0xFF;
        SearchCharacterExecute(offset, 0, 0, 0x90);
        SearchCharacterExecute(offset, 0, 0, 0x91);
        SearchCharacterExecute(eoffset, 0, 0, 0x90);
        SearchCharacterExecute(eoffset, 0, 0, 0x91);
    }
    if (offset->ActTimer == 8) {
        c_arg.flag = 1;
        c_arg.x = eoffset->posx;
        c_arg.y = -102400;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 6);
    }
    if (offset->ActTimer == 68) {
        c_arg.flag = 1;
        c_arg.x = eoffset->posx;
        c_arg.y = 51200;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 7);
    }
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    EndtoAction(offset, 0x5A);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_ITIGEKI3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x5A);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void KRAct_ITIGEKI_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x58);
    EndtoStand(offset);
}
