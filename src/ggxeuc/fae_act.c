#include "generated/fae_act.h"

void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void FANG_ColliCheck(CHARACTER_WORK *offset) {
    if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
        SetXSpeed(offset, -500);
        SetYSpeed(offset, -500, 50);
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FANGAct_Def(CHARACTER_WORK *offset) {
    int landtb[7] = { 0x00, 0x14, 0x13, 0x12, 0x12, 0x12, 0x15 };

    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
    } else {
        SetObjFlag(offset, 1);
        SetLandBreakActno(offset, landtb[offset->actno]);
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void font_entry(int side, char *str);

extern int PLAYER_HITPOINTMAX;

void FANGAct_Choco(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;
    CHARACTER_WORK *enemy = parent->ply->Enemy;
    CREATE_ARG arg;

    SetChangeFrameAnime(offset, offset->actno);
    if (!(offset->actst & 0x10)) {
        arg.flag = 0x124;
        arg.actno = 0x19;
        arg.Localid = 0xFF;
        if (!(parent->actst & 0x10) && parent->posx >= offset->posx - 4800
            && offset->posx + 4800 >= parent->posx && parent->HitPoint) {
            EffectCreate(parent, 0x2B, &arg);
            SetFlash(1, 0xF00);
            parent->HitPoint += 16;
            if (parent->HitPoint > PLAYER_HITPOINTMAX) {
                parent->HitPoint = PLAYER_HITPOINTMAX;
                font_entry(parent->padid, "MAXIMUM");
            }
            offset->actst |= 0x80000000;
            parent->ply->lifelengthdamage = parent->ply->lifelength = parent->HitPoint;
        } else if (!(enemy->actst & 0x10) && enemy->posx >= offset->posx - 4800
            && offset->posx + 4800 >= enemy->posx && enemy->HitPoint) {
            EffectCreate(enemy, 0x2B, &arg);
            SetFlash(1, 0xF00);
            enemy->HitPoint += 16;
            if (enemy->HitPoint > PLAYER_HITPOINTMAX) {
                enemy->HitPoint = PLAYER_HITPOINTMAX;
                font_entry(enemy->padid, "MAXIMUM");
            }
            offset->actst |= 0x80000000;
            enemy->ply->lifelengthdamage = enemy->ply->lifelength = enemy->HitPoint;
        }
    }
    SetObjFlag(offset, 1);
    EndtoEliminate(offset);
}

void FontDraw(char *str, int x, int y, float z, int alfa);
void ObjWorld2ScreenV(CHARACTER_WORK *offset, NL_POINT3 *p);
void SetBlendingMode(short int mode);

void Print_Build(CHARACTER_WORK *offset) {
    NL_POINT3 p;

    ObjWorld2ScreenV(offset, &p);
    SetBlendingMode(offset->transmode);
    FontDraw("BUILD UP", p.x - 48.0f, p.y, offset->priority, offset->trans);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void Print_Build(CHARACTER_WORK *offset);

void FANGAct_Build(CHARACTER_WORK *offset) {
    offset->graphicfunc = Print_Build;
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void FANG_MiniFA_Throw(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->parentWork = parent->parentWork;
    }
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
        return;
    }
    SetObjFlag(offset, 1);
    SetJumpControl(offset);
    if (offset->yspeed > 0) {
        SetActNoNext(offset, 0x10002);
    }
}

int GetBattleTimeStopFlag(int padid);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void FANG_MiniFA_Fall(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (GetBattleTimeStopFlag(-1)) {
        SetActNoNext(offset, 5);
    } else if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 5);
    } else {
        if (offset->Mark == 0) {
            SetObjFlag(offset, 1);
        }
        SetLandBreakActno(offset, 3);
    }
}

int GetBattleTimeStopFlag(int padid);
void OutSideEliminate(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void FANGAct_MiniFA(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (GetBattleTimeStopFlag(-1)) {
        SetActNoNext(offset, 5);
    } else if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 5);
    } else {
        OutSideEliminate(offset);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
float nlSin(short unsigned int r);

void FANG_MiniFA_Ballon(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->parentWork = parent->parentWork;
        offset->HitBackx = offset->posx;
    }
    offset->posx = nlSin(offset->Localid) * 60.0f * 100.0f + offset->HitBackx;
    offset->Localid += 0x200;
    SetJumpControl(offset);
    EndtoEliminate(offset);
    if (offset->posy < -60000) {
        offset->actst |= 0x80000000;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void FANG_MiniFADie(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    if (offset->posy > 40000) {
        offset->actst |= 0x80000000;
    }
    EndtoEliminate(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void FANGAct_Hammer(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 0x1001AU);
    }
    offset->angle -= 0x2000;
    SetObjFlag(offset, 1U);
    SetLandBreakActno(offset, 0x18U);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void FANGAct_HammerEnd(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->angle -= 0x2000;
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int GetBattleTimeStopFlag(int padid);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void FANGAct_Meteo(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1U);
    SetJumpControl(offset);
    if (GetBattleTimeStopFlag(-1) != 0) {
        offset->actst |= 0x80000000;
    }
    EndtoEliminate(offset);
}

RECTL *GetScreenRect(void);
int GetXMove(CHARACTER_WORK *offset, int val);
int LandtoEliminate(CHARACTER_WORK *offset);
int Random(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

extern RECTL ScreenObj;

void FANGAct_MeteoFall(CHARACTER_WORK *offset) {
    RECTL *rect;

    if (SetChangeFrameAnimeSameActno(offset)) {
        rect = GetScreenRect();
        offset->posy = ScreenObj.y - 50000;
        offset->posy -= (Random() & 0x3FF) * 100;
        offset->posx = rect->x + rect->w / 2 + GetXMove(offset, (200 - (Random() & 0x1FF) - (Random() & 0xFF)) * 100);
    }
    SetObjFlag(offset, 1);
    SetJumpControl(offset);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 0x1001D);
    }
    LandtoEliminate(offset);
}

int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void FANGAct_MeteoBreak(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    SetJumpControl(offset);
    LandtoEliminate(offset);
}

int IsLand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void FANGAct_Egg(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno)) {
        SetLandBreakActno(offset, -1);
    } else {
        if (IsLand(offset)) {
            offset->actst |= 0x80000000;
        }
        if (offset->posy > 0) {
            offset->actst |= 0x80000000;
        }
        if (offset->attackst & 0x80000) {
            SetActNoNext(offset, 8);
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int GetBattleTimeStopFlag(int padid);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void FANGAct_Dyna(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent;
    CHARACTER_WORK *enemy;
    CREATE_ARG arg;

    SetChangeFrameAnime(offset, offset->actno);
    if (offset->Mark && !GetBattleTimeStopFlag(-1)) {
        parent = offset->parentWork;
        enemy = parent->ply->Enemy;
        arg.flag = 1;
        arg.x = offset->posx;
        arg.y = offset->posy;
        if (!SearchEffectWorkActno(enemy, 0x51, 8)) {
            ObjectCreate_ActNo(parent, 0x2B, &arg, 0x1B);
            ObjectCreate_ActNo(enemy, 0x2B, &arg, 0x1B);
        }
    }
    SetObjFlag(offset, 1);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetParentPosEx(CHARACTER_WORK *offset);

void FALHAct(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    EndtoEliminate(offset);
    SetParentPosEx(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

extern RECTL ScreenObj;

void FASHWBoxAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    SetChangeFrameAnime(offset, 6);
    offset->posx = ScreenObj.x - 26000 + offset->Localid * 16000;
    offset->dirflag = 0;
    if (parent->ply->PersonalWork[0].ub[1]) {
        if (parent->ply->PersonalWork[0].ub[1] - 1 == offset->Localid) {
            SetActNoNext(offset, 7);
        } else {
            SetActNoNext(offset, 8);
        }
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

extern RECTL ScreenObj;

void FASHWBoxAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 7);
    offset->posx = ScreenObj.x - 26000 + offset->Localid * 16000;
    offset->dirflag = 0;
    EndtoAction(offset, 0xA);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

extern RECTL ScreenObj;

void FASHWBoxAct_3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;
    CHARACTER_WORK *box;
    CREATE_ARG arg;

    box = SearchEffectWorkActno(offset, 0x40, 4);
    if (!box) {
        box = SearchEffectWorkActno(offset, 0x40, 5);
    }
    if (SetChangeFrameAnime(offset, 0xA)) {
        arg.flag = 0x104;
        if (parent->ply->PersonalWork[0].ub[2] == 3) {
            arg.actno = 4;
        } else {
            arg.actno = 5;
        }
        EffectCreate(offset, 0x40, &arg);
    }
    offset->posx = ScreenObj.x - 26000 + offset->Localid * 16000;
    offset->dirflag = 0;
    if (box) {
        box->posx = offset->posx;
    }
    TimetoAction(offset, 0xD, 120);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void FASHWEngAct(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 4);
    SetJumpControl(offset);
    if (offset->yspeed >= 0) {
        offset->yspeed = 0;
        offset->gravity = 0;
    }
    TimetoAction(offset, 0xB, 0x78);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void FASHWDetAct(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 5);
    SetJumpControl(offset);
    if (offset->yspeed >= 0) {
        offset->yspeed = 0;
        offset->gravity = 0;
    }
    TimetoAction(offset, 0xC, 0x78);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FALHAct_Metta(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent;

    SetChangeFrameAnime(offset, 9);
    parent = offset->parentWork;
    if (parent->actno != 0x80 && parent->actno != 0x7A && parent->actno != 0xCD && parent->actno != 0xCE) {
        offset->actst |= 0x80000000;
    }
    if (offset->ActTimer >= 64 || parent->ActTimer < 4) {
        offset->actst |= 0x80000000;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Que(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    offset->dirflag = 0;
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
COLLISION *Get_GCLEx(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Bed(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;
    CHARACTER_WORK *enemy = parent->ply->Enemy;
    COLLISION *col;

    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->posx = 0;
        offset->posy = 0;
    }
    EndtoEliminate(offset);
    col = Get_GCLEx(offset);
    if (col) {
        enemy->parentx = col->x;
        if (offset->dirflag == 1) {
            enemy->posx = offset->posx - parent->parentx * 100;
        } else {
            enemy->posx = offset->posx + parent->parentx * 100;
        }
        enemy->parenty = col->y;
        enemy->posy = offset->posy + enemy->parenty * 100;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Efe(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno) != 0) {
        offset->posx = 0;
        offset->posy = 0;
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Self(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = ((CHARACTER_WORK *)offset->parentWork)->ply->Enemy;

    SetChangeFrameAnime(offset, offset->actno);
    enemy->ply->pactst |= 0x200;
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Bom(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = ((CHARACTER_WORK *)offset->parentWork)->ply->Enemy;

    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x80000) {
        enemy->ply->pactst |= 0x100;
        SetActNoNext(enemy, 0x32);
    }
    EndtoEliminate(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Swc(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->dirflag) {
            offset->posx = -69300;
        } else {
            offset->posx = 69300;
        }
        offset->posy = 0;
    }
    if (parent->Mark == 5) {
        offset->actst |= 0x80000000;
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void FANGAct_Atomic(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->posx = 0;
        offset->posy = 0;
    }
    if (parent->Mark == 1) {
        offset->actst |= 0x80000000;
    }
}

void ReversColor(void);

void ReversDraw(CHARACTER_WORK *offset) {
    ReversColor();
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ReversDraw(CHARACTER_WORK *offset);

void FANGAct_NegaPogi(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    offset->graphicfunc = ReversDraw;
    SetChangeFrameAnime(offset, offset->actno);
    if (parent->Mark != 1) {
        offset->actst |= 0x80000000;
    }
    EndtoEliminate(offset);
}
