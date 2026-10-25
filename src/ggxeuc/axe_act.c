#include "generated/axe_act.h"

#include "generated/eff_act.h"

void (*AXCH_Act_jumpTb[])() = {
    AXCHAct_1, AXCHAct_2, AXCHAct_2, AXCHAct_2, AXCHAct_2, AXCHAct_LC_Itigeki,
    AXCHAct_RasyousenChain, AXCHAct_RasyousenChainAX, AXCHAct_ItigekiGroundChain,
    AXCHAct_ItigekiAttackColli, AXCHAct_ItigekiAttackColli2, AXCHAct_LC_Itigeki2,
    AXCHAct_ItigekiAttackColli3, 0,
};

void (*AXVQ_Act_jumpTb[])() = {
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, AXVQAct_RSS_Shot, AXVQAct_RSS_Parasite, CMNEFFActDef, CMNEFFActDef, AXVQAct_B,
    CMNEFFActDef, AXVQAct_D, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, AXVQAct_11,
    AXVQAct_ExplodeCreate, CMNEFFActDef, CMNEFFActDef, AXVQAct_RSSSatellite, AXVQAct_ITHahen,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, 0,
};

float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
void ParXSpeed(CHARACTER_WORK *offset, float par);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
extern CHARACTER_WORK *PlayerWork;

void SetAXChainSpeed(CHARACTER_WORK *offset) {
    switch (PlayerWork[offset->padid].ply->PersonalWork[0].ub[2]) {
    case 0:
        SetXSpeed(offset, offset->Localid * 150 + 1400);
        SetYSpeed(offset, -100, 0);
        break;
    case 1:
        ParXSpeed(offset, 0.9f);
        offset->gravity = 30;
        break;
    case 2:
        SetXSpeed(offset, -G_Cos(offset->angle) * ((offset->Localid - 10) * 400));
        SetYSpeed(offset, G_Sin(offset->angle) * ((offset->Localid - 10) * 400), 0);
        break;
    case 3:
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 10);
        break;
    case 4:
        if (offset->Localid < 8) {
            SetXSpeed(offset, offset->Localid * 150 + 800);
            SetYSpeed(offset, -1000, 0);
        } else if (offset->Localid < 16) {
            SetXSpeed(offset, offset->Localid * 150 + 800);
            SetYSpeed(offset, 1000, 0);
        } else {
            SetXSpeed(offset, offset->Localid * 150 + 1400);
            SetYSpeed(offset, -100, 0);
        }
        break;
    }
}

void SetAXChainSpeed(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetParentPosEx(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void AXCHAct_1(CHARACTER_WORK *offset) {
    if (!SetChangeFrameAnimeSameActno(offset) && offset->posy >= 0) {
        if (offset->yspeed > 300) {
            SetYSpeed(offset, -offset->yspeed / 2, 50);
        }
    }
    if (offset->Localid) {
        SetAXChainSpeed(offset);
    } else {
        SetParentPosEx(offset);
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
    }
    SetJumpControl(offset);
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void SetAXChainSpeed(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void AXCHAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    SetAXChainSpeed(offset);
    SetJumpControl(offset);
    if (offset->attackst & 0x80000) {
        poffset->attackst |= 0x80000;
    }
}

float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
int nlArcTan2(int x, int y);
extern CHARACTER_WORK *PlayerWork;

void AXCHCalc(CHARACTER_WORK *offset, CHARACTER_WORK *next) {
    float x;
    float y;
    u_short angle;

    if (PlayerWork[offset->padid].ply->PersonalWork[0].ub[2] == 2) {
        if (offset->Localid == 0) {
            offset->angle += 0x680;
        }
        next->angle = offset->angle;
        if (offset->dirflag == 1) {
            angle = offset->angle;
        } else {
            angle = -offset->angle;
        }
    } else {
        x = -(float)(next->posx - offset->posx);
        y = -(float)(next->posy - offset->posy);
        angle = nlArcTan2(x, y);
    }
    offset->angle = (offset->dirflag == 1) ? angle : -angle;
    next->posx = offset->posx + -G_Sin(angle) * 16.0f * 100.0f;
    next->posy = offset->posy + -G_Cos(angle) * 16.0f * 100.0f;
}

int nlArcTan2(int x, int y);

void AXCHCalc2(CHARACTER_WORK *offset, CHARACTER_WORK *target) {
    float x;
    float y;
    u_short angle;

    x = -(float)(target->posx - offset->posx);
    y = -(float)(-90000 - offset->posy);
    angle = nlArcTan2(x, y);
    offset->angle = (offset->dirflag == 1) ? angle : -angle;
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
int IsHitStop(CHARACTER_WORK *offset);
extern CHARACTER_WORK *PlayerWork;

void AXCHCalcHook(CHARACTER_WORK *offset, CHARACTER_WORK *next) {
    u_short angle;
    CHARACTER_WORK *object;
    CREATE_ARG c_arg;

    if (PlayerWork[offset->padid].ply->PersonalWork[0].ub[2] == 0 || PlayerWork[offset->padid].ply->PersonalWork[0].ub[2] == 4) {
        next->angle = -0x4000;
    } else {
        next->angle = offset->angle;
    }
    if (offset->dirflag == 1) {
        angle = offset->angle;
    } else {
        angle = -offset->angle;
    }
    next->posx = offset->posx + -G_Sin(angle) * 16.0f * 100.0f;
    next->posy = offset->posy + -G_Cos(angle) * 16.0f * 100.0f;
    if (!IsHitStop(next) && !(next->attackst & 0x800)) {
        c_arg.flag = 2;
        c_arg.x = -G_Sin(next->angle) * 6400.0f;
        c_arg.y = -G_Cos(next->angle) * 6400.0f;
        if (next->actno == 3) {
            object = EffectCreate_ActNo(next, 0x51, &c_arg, 6);
        } else if (PlayerWork[offset->padid].ply->PersonalWork[0].ub[2] == 2) {
            object = EffectCreate_ActNo(next, 0x51, &c_arg, 2);
        } else {
            object = EffectCreate_ActNo(next, 0x51, &c_arg, 1);
        }
        if (object) {
            object->angle = next->angle;
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
RECTL *GetScreenRect(void);
int Random(void);
void RenewalCollision(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void AXCHAct_LC_Itigeki(CHARACTER_WORK *offset) {
    RECTL *p;
    u_short angle;
    CHARACTER_WORK *eoffset;
    int flag;
    int offx;
    CHARACTER_WORK *offobj;

    eoffset = GetEnemyPlayer(offset);
    p = GetScreenRect();
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = (Random() & 0x3F00) - 0x6000;
        if (offset->dirflag == 1) {
            angle = offset->angle;
        } else {
            angle = -offset->angle;
        }
        if (offset->dirflag == 1) {
            offset->posx = p->x;
        } else {
            offset->posx = p->x + p->w;
        }
        offx = offset->posx - eoffset->posx;
        offset->posy = eoffset->posy - 15000;
        offset->posy += G_Cos(angle) * offx / G_Sin(angle);
    }
    if (offset->dirflag == 1) {
        angle = offset->angle;
    } else {
        angle = -offset->angle;
    }
    flag = 0;
    if (offset->dirflag == 1) {
        if (p->x + p->w + 3200 < offset->posx) {
            flag = 1;
        }
    } else {
        if (offset->posx < p->x - 3200) {
            flag = 1;
        }
    }
    if (!flag) {
        offset->xspeed = -G_Sin(angle) * 48.0f * 100.0f;
        offset->yspeed = -G_Cos(angle) * 48.0f * 100.0f;
    } else {
        offset->xspeed = 0;
        offset->yspeed = 0;
    }
    if (offset->Localid == 0) {
        if (((offset->posx - eoffset->posx) >= 0 ? (offset->posx - eoffset->posx) : (eoffset->posx - offset->posx)) < 3200) {
            offobj = SearchObjectWorkActno(offset, 0x4D, 9);
            if (offobj) {
                offobj->posx = offset->posx;
                offobj->posy = offset->posy;
                RenewalCollision(offobj);
            }
            offset->Localid = 1;
        }
    }
    EndtoEliminate(offset);
    SetJumpControl(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
RECTL *GetScreenRect(void);
int Random(void);
void RenewalCollision(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void AXCHAct_LC_Itigeki2(CHARACTER_WORK *offset) {
    RECTL *p;
    u_short angle;
    CHARACTER_WORK *eoffset;
    int flag;
    int offx;
    CHARACTER_WORK *offobj;

    eoffset = GetEnemyPlayer(offset);
    p = GetScreenRect();
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = (Random() & 0x3F00) - 0x6000;
        if (offset->dirflag == 1) {
            angle = offset->angle;
        } else {
            angle = -offset->angle;
        }
        if (offset->dirflag == 1) {
            offset->posx = p->x;
        } else {
            offset->posx = p->x + p->w;
        }
        offx = offset->posx - eoffset->posx;
        offset->posy = eoffset->posy - 20000;
        offset->posy += G_Cos(angle) * offx / G_Sin(angle);
    }
    if (offset->dirflag == 1) {
        angle = offset->angle;
    } else {
        angle = -offset->angle;
    }
    flag = 0;
    if (offset->dirflag == 1) {
        if (p->x + p->w + 3200 < offset->posx) {
            flag = 1;
        }
    } else {
        if (offset->posx < p->x - 3200) {
            flag = 1;
        }
    }
    if (!flag) {
        offset->xspeed = -G_Sin(angle) * 48.0f * 100.0f;
        offset->yspeed = -G_Cos(angle) * 48.0f * 100.0f;
    } else {
        offset->xspeed = 0;
        offset->yspeed = 0;
    }
    if (offset->Localid == 0) {
        if (((offset->posx - eoffset->posx) >= 0 ? (offset->posx - eoffset->posx) : (eoffset->posx - offset->posx)) < 3200) {
            offobj = SearchObjectWorkActno(offset, 0x4D, 9);
            if (offobj) {
                offobj->posx = offset->posx;
                offobj->posy = offset->posy;
                RenewalCollision(offobj);
            }
            offset->Localid = 1;
        }
    }
    EndtoEliminate(offset);
    SetJumpControl(offset);
}

extern unsigned char PG_offySkyTbByIdno[19];  /* data 0x5702d8 eff_act.c */

void LinkParentCenter(CHARACTER_WORK *offset, CHARACTER_WORK *poffset) {
    offset->posy = poffset->posy - (PG_offySkyTbByIdno[poffset->idno] * 0x64);
    offset->posx = poffset->posx;
}

void AXCHCalc2(CHARACTER_WORK *offset, CHARACTER_WORK *target);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void LinkParentCenter(CHARACTER_WORK *offset, CHARACTER_WORK *poffset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void AXCHAct_RasyousenChainAX(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s1;
    CHARACTER_WORK *temp_s2;

    temp_s2 = GetEnemyPlayer(offset);
    temp_s1 = GetParentPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (SetParentPosExNo(offset, 0) == 0) {
        LinkParentCenter(offset, temp_s1);
    }
    AXCHCalc2(offset, temp_s2);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void LinkParentCenter(CHARACTER_WORK *offset, CHARACTER_WORK *poffset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
int g_roundmin(int val, int min);

void AXCHAct_RasyousenChain(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s1;
    CHARACTER_WORK *temp_s2;

    temp_s1 = GetEnemyPlayer(offset);
    temp_s2 = GetParentPlayer(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetYSpeed(temp_s1, -0x258, 0x28);
    }
    LinkParentCenter(offset, temp_s1);
    SetJumpControl(offset);
    if (temp_s2->posy >= -0xC7) {
        if (temp_s2->yspeed > 0) {
            temp_s1->posy = g_roundmin(temp_s1->posy - 0x7D0, -0x3E80);
            SetYSpeed(temp_s1, 0, 0);
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetExCollision(CHARACTER_WORK *offset, short int x, short int y, short unsigned int w, short unsigned int h, unsigned int attr);

void AXCHAct_ItigekiGroundChain(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    SetExCollision(offset, -0x1A9, -2, 0x352, 4, 1);
    if (offset->attackst & 0x80000) {
        poffset->attackst |= 0x80000;
    }
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetExCollision(CHARACTER_WORK *offset, short int x, short int y, short unsigned int w, short unsigned int h, unsigned int attr);

void AXCHAct_ItigekiAttackColli(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetExCollision(offset, -0x32, -0x64, 0x64, 200, 1);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXCHAct_ItigekiAttackColli2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s0;

    temp_s0 = GetEnemyPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    temp_s0->xspeed = 0;
    temp_s0->yspeed = 0;
    temp_s0->gravity = 0;
    temp_s0->actst |= 0x400000;
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void LinkParentCenter(CHARACTER_WORK *offset, CHARACTER_WORK *poffset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetExCollision(CHARACTER_WORK *offset, short int x, short int y, short unsigned int w, short unsigned int h, unsigned int attr);

void AXCHAct_ItigekiAttackColli3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    LinkParentCenter(offset, eoffset);
    SetExCollision(offset, -0x46, -0x46, 0x8C, 0x8C, 1);
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
int IsHitStop(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXVQAct_B(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = -0x6000;
    }
    if (!IsHitStop(offset)) {
        c_arg.flag = 0x1002;
        offset->angle += 0x800;
        c_arg.x = -G_Sin(offset->angle) * 15000.0f;
        c_arg.y = -G_Cos(offset->angle) * 15000.0f;
        c_arg.angle = offset->angle;
        EffectCreate_ActNo(offset, 0x51, &c_arg, 0xA);
        if (offset->ActTimer & 1) {
            c_arg.flag = 2;
            EffectCreate_ActNo(offset, 0x51, &c_arg, 0xE);
        }
    }
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
int IsHitStop(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXVQAct_D(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = -0x6000;
    }
    if (!IsHitStop(offset)) {
        c_arg.flag = 0x1002;
        offset->angle -= 0x800;
        c_arg.x = -G_Sin(offset->angle) * 15000.0f;
        c_arg.y = -G_Cos(offset->angle) * 15000.0f;
        c_arg.angle = offset->angle;
        EffectCreate_ActNo(offset, 0x51, &c_arg, 0xC);
        if (offset->ActTimer & 1) {
            c_arg.flag = 2;
            EffectCreate_ActNo(offset, 0x51, &c_arg, 0xE);
        }
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXVQAct_11(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = (unsigned short) offset->angle - 0x8000;
    }
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void OutSideEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXVQAct_RSS_Shot(CHARACTER_WORK *offset) {
    CHARACTER_WORK *temp_s1;
    CHARACTER_WORK *temp_v0;
    unsigned int temp_v1;

    temp_s1 = GetParentPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    OutSideEliminate(offset);
    temp_v1 = offset->attackst;
    if (temp_v1 & 0x80000) {
        if (!(temp_s1->actst & 0x4020)) {
            temp_v0 = GetParentPlayer(offset);
            temp_v0->attackst |= 0x80000;
            EffectCreate_ActNo(temp_v0, 0x51, 0, 8);
        }
        offset->actst |= 0x80000000;
    } else if (temp_v1 & 0x1000) {
        EffectCreate_ActNo(offset, 0x51, 0, 9);
        offset->actst |= 0x80000000;
    }
    EndtoEliminate(offset);
    if ((int) offset->actst < 0) {
        SearchCharacterExecute(offset, 0x51, 0x15, 0x16);
    }
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void LinkParentCenter(CHARACTER_WORK *offset, CHARACTER_WORK *poffset);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int TimetoEliminate(CHARACTER_WORK *offset, int time);

void AXVQAct_RSS_Parasite(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;
    CHARACTER_WORK *poffset;

    eoffset = GetEnemyPlayer(offset);
    poffset = GetParentPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    LinkParentCenter(offset, eoffset);
    eoffset->actst |= 0x400000;
    eoffset->ply->DownTimer = 0xF0;
    eoffset->xspeed = 0;
    eoffset->CommandFlag &= ~0x800;
    TimetoEliminate(offset, 0xF0);
    if (poffset->actst & 0x20) {
        offset->actst |= 0x80000000;
    }
    if ((int) offset->actst < 0) {
        SearchCharacterExecute(offset, 0x51, 0x15, 0x16);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void LinkParentCenter(CHARACTER_WORK *offset, CHARACTER_WORK *poffset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetExCollision(CHARACTER_WORK *offset, short int x, short int y, short unsigned int w, short unsigned int h, unsigned int attr);

void AXVQAct_ExplodeCreate(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    LinkParentCenter(offset, eoffset);
    SetExCollision(offset, -0xA, -0xA, 0x14, 0x14, 1);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
int GetXMove(CHARACTER_WORK *offset, int val);
int IsHitStop(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void AXVQAct_RSSSatellite(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    SetChangeFrameAnimeSameActno(offset);
    if (!(offset->actst & 0x80000000)) {
        offset->posx = poffset->posx + GetXMove(offset, G_Sin((offset->Localid * 256) & 0xFF00) * 30.0f * 100.0f);
        offset->posy = poffset->posy + G_Cos((offset->Localid * 256) & 0xFF00) * 30.0f * 100.0f;
    }
    if (!IsHitStop(offset)) {
        offset->Localid += 0x10;
    }
    EndtoEliminate(offset);
}

int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void AXVQAct_ITHahen(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    LandtoEliminate(offset);
}
