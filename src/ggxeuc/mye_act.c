#include "generated/mye_act.h"

#include "generated/eff_act.h"

void (*MYEE_Act_jumpTb[])() = { MYEEAct_0, MYEEAct_1, 0, MYEEAct_3, 0, 0 };

void (*MYIG_Act_jumpTb[])() = {
    MYIGAct_Def, MYIGAct_Def, MYIGAct_Def, MYIGAct_Def, MYIGAct_Def, MYIGAct_Def, MYIGAct_Def,
    MYIGAct_Def, MYIGAct_GAKIWALK, MYIGAct_GAKIFALL, MYIGAct_GAKILAND, MYIGAct_GAKIWIN,
    MYIGAct_Pengin, MYIGAct_april, MYIGAct_PenginLand, CMNEFFActWithJumpDef,
    CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, 0,
    0,
};

void (*MYIB_Act_jumpTb[])() = {
    MYIBAct_ROBO, MYIBAct_BABA1, MYIBAct_BABA2, MYIBAct_BABA3, MYIBAct_BABAWin,
    MYIBAct_BABABackWalk, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, 0,
};

void (*MYIK_Act_jumpTb[])() = {
    MYIKAct_ByHoop, MYIKAct_1, MYIKAct_2, MYIKAct_3, MYIKAct_4, MYIKAct_5, 0, MYIKAct_7, 0,
    MYIKAct_Entry, MYIKAct_Leave, MYIKAct_a, MYIKAct_Entry, MYIKAct_Leave, MYIKAct_ByHoop,
    MYIKAct_ByHoop, 0, 0,
};

void (*MYKU_Act_jumpTb[])() = { MYKUAct_0, MYKUAct_1, 0 };

void (*MYFU_Act_jumpTb[])() = {
    MYFUAct_0, MYFUAct_1, MYFUAct_2, CMNEFFActWithJumpDef, MYFUAct_4, MYFUAct_5, MYFUAct_6,
    MYFUAct_HoopShoot, MYFUAct_HoopShoot, MYFUAct_HoopShoot, MYFUAct_HoopShoot,
    CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, 0,
};

void MYEE_ColliCheck(CHARACTER_WORK *offset) {
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void MYEEAct_0(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 0)) {
        offset->posy -= 0x8ca0;
    }
    SetLandBreakActno(offset, 1);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void MYEEAct_1(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 1)) {
        offset->posy = 0;
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
void MYEEAct_3(CHARACTER_WORK *offset) {
    static int test;

    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 3)) {
        if (offset->dirflag == 0) {
            offset->posx += 20000;
        } else {
            offset->posx -= 20000;
        }
        offset->posy -= 10000;
        SetYSpeed(offset, -1300, 70);
        SetXSpeed(offset, 300);
        offset->Localid = 0;
        test = offset->posy - 4000;
    }
    if (offset->posy > test) {
        SetYSpeed(offset, -1300, 70);
    }
    if (offset->ActTimer > 130 && !(offset->Localid & 1)) {
        SetXSpeed(offset, 0);
    }
    if (offset->ActTimer > 140 && !(offset->Localid & 1)) {
        SetXSpeed(offset, -400);
        SetYSpeed(offset, -1300, 70);
        offset->Localid |= 1;
    }
    if (offset->ActTimer > 180) {
        offset->actst |= 0x80000000;
    }
}

RECTL *GetScreenRect(void);
void ScreenSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIGAct_Def(CHARACTER_WORK *offset) {
    RECTL *rect = GetScreenRect();

    if (SetChangeFrameAnimeSameActno(offset)) {
        if (offset->dirflag == 1) {
            offset->posx = rect->x - 2000;
        } else {
            offset->posx = rect->x + rect->w + 2000;
        }
    }
    if (offset->HitParam) {
        offset->HitParam->DownFlag |= 0x210;
    }
    ScreenSideEliminate(offset);
}

RECTL *GetScreenRect(void);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
void MYIGAct_GAKIWALK(CHARACTER_WORK *offset) {
    RECTL *rect = GetScreenRect();

    if (SetChangeFrameAnimeSameActno(offset)) {
        if (offset->dirflag == 1) {
            offset->posx = rect->x - 2000;
        } else {
            offset->posx = rect->x + rect->w + 2000;
        }
    }
    TimetoAction(offset, 9, 20);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void MYIGAct_GAKIFALL(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetLandBreakActno(offset, 10);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
void MYIGAct_GAKILAND(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    TimetoAction(offset, 11, 60);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIGAct_GAKIWIN(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void MYIGAct_Pengin(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    SetLandBreakActno(offset, 14);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIGAct_april(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIGAct_PenginLand(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

RECTL *GetScreenRect(void);
void ScreenSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
void MYIBAct_ROBO(CHARACTER_WORK *offset) {
    RECTL *rect = GetScreenRect();

    if (SetChangeFrameAnimeSameActno(offset)) {
        if (offset->dirflag == 1) {
            offset->posx = rect->x - 2000;
        } else {
            offset->posx = rect->x + rect->w + 2000;
        }
    }
    if (offset->HitParam) {
        offset->HitParam->DownFlag |= 0x210;
    }
    ScreenSideEliminate(offset);
}

RECTL *GetScreenRect(void);
void ScreenSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
void MYIBAct_BABA1(CHARACTER_WORK *offset) {
    RECTL *rect = GetScreenRect();

    if (SetChangeFrameAnimeSameActno(offset)) {
        if (offset->dirflag == 1) {
            offset->posx = rect->x - 2000;
        } else {
            offset->posx = rect->x + rect->w + 2000;
        }
    }
    if (offset->HitParam) {
        offset->HitParam->DownFlag |= 0x210;
    }
    TimetoAction(offset, 2, 48);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIBAct_BABA2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 5);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIBAct_BABA3(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    if (offset->HitParam) {
        offset->HitParam->DownFlag |= 0x210;
    }
    EndtoAction(offset, 4);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIBAct_BABAWin(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYIBAct_BABABackWalk(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->dirflag = !offset->dirflag;
    }
    EndtoAction(offset, 3);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
int IsLand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_ByHoop(CHARACTER_WORK *offset) {
    CREATE_ARG arg;
    short i;

    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 5);
        return;
    }
    if (IsLand(offset)) {
        arg.flag = 0x204;
        arg.actno = 2;
        for (i = 0; i < 10; i++) {
            EffectCreate(offset, 0x48, &arg);
        }
        offset->actst |= 0x80000000;
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg, unsigned short actno);
int IsLand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
void MYIKAct_1(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 1)) {
        EffectCreate_ActNo(offset, 0x3e, 0, 3);
        if (offset->dirflag == 0) {
            offset->posx += 20000;
        } else {
            offset->posx -= 20000;
        }
        SetYSpeed(offset, -800, 50);
        SetActNoNext(0, 3);
    }
    if (IsLand(offset)) {
        SetYSpeed(offset, -800, 50);
    }
    if (offset->ActTimer > 125) {
        SetXSpeed(offset, 0);
        SetActNoNext(offset, 0x1000b);
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_2(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 2)) {
        offset->angle = 0;
    }
    if (offset->ActTimer > 50) {
        offset->actst |= 0x80000000;
    }
}

int IsLand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_a(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 11);
    if (IsLand(offset)) {
        SetActNoNext(offset, 0x10002);
    }
    offset->angle += 0x1000;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_3(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 3);
    EndtoAction(offset, 7);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_4(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 4);
    EndtoAction(offset, 7);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_5(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 5);
    EndtoAction(offset, 7);
}

int IsLand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
void MYIKAct_7(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 7)) {
        switch (parent->Localid) {
        case 3:
            SetXSpeed(offset, -2000);
            SetYSpeed(offset, 2000, 50);
            break;
        case 4:
            SetXSpeed(offset, -2000);
            SetYSpeed(offset, -100, 0);
            break;
        case 5:
            SetXSpeed(offset, -2000);
            SetYSpeed(offset, -2000, 10);
            break;
        default:
            SetXSpeed(offset, -2000);
            SetYSpeed(offset, -2000, 10);
            break;
        }
    }
    offset->angle += 0x1000;
    if (offset->ActTimer > 50) {
        offset->actst |= 0x80000000;
    }
    if (IsLand(offset)) {
        offset->actst |= 0x80000000;
    }
}

int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_Entry(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    LandtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void MYIKAct_Leave(CHARACTER_WORK *offset) {
    CREATE_ARG arg;
    int i;

    offset->actst |= 0x100000;
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->posy > -2000) {
        arg.flag = 0x204;
        arg.actno = 2;
        i = 10;
        while (i--) {
            EffectCreate(offset, 0x48, &arg);
        }
    }
    if (offset->posy > 2000) {
        offset->actst |= 0x80000000;
    }
}

RECTL *GetScreenRect(void);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void MYKUAct_0(CHARACTER_WORK *offset) {
    RECTL *rect = GetScreenRect();

    if (SetChangeFrameAnimeSameActno(offset)) {
        if (offset->dirflag == 1) {
            offset->posx = rect->x - 30000;
        } else {
            offset->posx = rect->x + rect->w + 30000;
        }
    }
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    if (offset->posy > 40000) {
        offset->actst |= 0x80000000;
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
int IsLand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYKUAct_1(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 1)) {
        arg.flag = 0x104;
        arg.actno = 6;
        EffectCreate(offset, 0x48, &arg);
    }
    if (IsLand(offset)) {
        arg.flag = 0x104;
        arg.actno = 1;
        EffectCreate(offset, 0x48, &arg);
        offset->Mark &= ~1;
        offset->actst |= 0x80000000;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void MYFUAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void MYFUAct_1(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 1)) {
        offset->posy = 0;
        offset->posx += 2000;
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void MYFUAct_2(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 2);
    LandtoEliminate(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void MYFUAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 3);
    EndtoEliminate(offset);
}

int GetBattleTimeStopFlag(int padid);
int GetXMove(CHARACTER_WORK *offset, int val);
int IsActEnd(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
int IsHoldPunch(CHARACTER_WORK *offset);
int IsHoldKick(CHARACTER_WORK *offset);
int IsHoldSlash(CHARACTER_WORK *offset);
int IsHoldHighSlash(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;
void MYFUAct_4(CHARACTER_WORK *offset) {
    static int (*SwitchHoldTbl[4])(CHARACTER_WORK *) = {
        IsHoldPunch, IsHoldKick, IsHoldSlash, IsHoldHighSlash
    };
    CHARACTER_WORK *parent = offset->parentWork;
    CHARACTER_WORK *enemy = parent->ply->Enemy;

    SetObjFlag(offset, 1);
    offset->CommandFlag = 0xc05f;
    if (SetChangeFrameAnime(offset, 4)) {
        parent->ply->PersonalWork[1].w[0] &= 3;
        if (parent->ply->PersonalWork[1].w[0] == 0) {
            offset->posx = parent->posx + GetXMove(offset, -5000);
        } else if (parent->ply->PersonalWork[1].w[0] == 1) {
            offset->posx = parent->posx + GetXMove(offset, 20000);
        } else if (parent->ply->PersonalWork[1].w[0] == 2) {
            offset->posx = enemy->posx + GetXMove(offset, -30000);
        } else {
            offset->posx = enemy->posx + GetXMove(offset, -20000);
        }
        offset->Localid = 0;
        offset->posy = parent->posy - 32000;
    }
    if (IsActEnd(offset)) {
        if (SwitchHoldTbl[parent->ply->PersonalWork[1].w[0]](parent)) {
            if (offset->Localid++ > 0x5a0) {
                offset->Localid = 0x5a0;
            }
        } else if (g_info.boss[offset->padid] == 1 || offset->Localid >= 0x5a0) {
            SetActNoNext(offset, 10);
        } else if (offset->Localid >= 0x2d0) {
            SetActNoNext(offset, 9);
        } else if (offset->Localid >= 0xf0) {
            SetActNoNext(offset, 8);
        } else {
            SetActNoNext(offset, 7);
        }
    }
    if (parent->actst & 0x20) {
        if (g_info.boss[offset->padid] == 1) {
            SetActNoNext(offset, 10);
        } else {
            SetActNoNext(offset, 5);
        }
    }
    if (GetBattleTimeStopFlag(-1)) {
        SetActNoNext(offset, 5);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void MYFUAct_5(CHARACTER_WORK *offset) {
    SetObjFlag(offset, 1);
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void MYFUAct_6(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posy += 3000;
    }
    EndtoEliminate(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void MYFUAct_HoopShoot(CHARACTER_WORK *offset) {
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 5);
}
