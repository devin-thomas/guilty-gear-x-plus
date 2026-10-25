#include "generated/mle_act.h"

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
int EndtoEliminate(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
void SetJumpControl(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
void SetNoAttack(CHARACTER_WORK *offset);
int IsHitStop(CHARACTER_WORK *offset);
int IsActEnd(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int Random(void);
void SetBlendingMode(short int mode);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
extern USER_SETTING userinfo;

void (*MLRG_Act_jumpTb[])() = {
    MLRGAct_0, MLRGAct_1, MLRGAct_2, MLRGAct_0S, MLRGAct_1S, MLRGAct_2S, MLRGAct_0, MLRGAct_1,
    MLRGAct_2, MLRGAct_0S, MLRGAct_1S, MLRGAct_2S, MLRASENAct, MLSBOWL, MLSBOWLMove,
    MLSBOWLBreak, MLRGAct_0K, MLRGAct_1K, MLRGAct_2K, MLRGAct_0K2, MLRGAct_1K2, MLRGAct_2K2,
    MLRGAct_0K3, MLRGAct_1K3, MLRGAct_2K3, 0,
};

void (*MLKR_Act_jumpTb[])() = {
    MLKRAct_0, MLKRAct_0, MLKRAct_0, MLKRAct_0, MLKRAct_LandCircle0, MLKRAct_Pillar, 0,
    MLKRAct_ERR, MLKRAct_ERR, MLKRAct_ERR, MLKRAct_ERR, MLKRAct_ERR, MLKRAct_ERR, MLKRAct_ERR,
    MLKRAct_ERR, 0, MLKRAct_Circle0, MLKRAct_Circle1, MLKRAct_Circle2, MLKRAct_Circle3,
    MLKRAct_Circle4, MLKRAct_Circle5, MLKRAct_HW4, MLKRAct_KILL, 0,
};

void (*MLRS_Act_jumpTb[])() = {
    MLRSAct_0, MLRSAct_1, MLRSAct_2, MLRSAct_3, MLKRAct_1, MLKRAct_1, MLKRAct_1, MLKRAct_1,
    MLKRAct_21, MLKRAct_22, MLKRAct_23, MLKRAct_24, 0,
};

int SetActNo(CHARACTER_WORK *offset, unsigned int no);

void MLRG_ColliCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 1) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 2);
    } else if (offset->actno == 17) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 18);
    } else if (offset->actno == 20) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 21);
    } else if (offset->actno == 23) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 24);
    } else if (offset->actno == 4) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 5);
    } else if (offset->actno == 7) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 8);
    } else if (offset->actno == 10) {
        if (*(unsigned int *)&offset->ColliFlag & 0xffff00) SetActNo(offset, 11);
    }
}

void MLRGAct_0(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1);
    EndtoAction(offset, 1);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    if (!SetChangeFrameAnimeSameActno(offset)) {
        TimetoAction(offset, 2, offset->Mark);
    }
    SetObjFlag(offset, 1);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    offset->InstSt |= 0x4000;
    SetNoAttack(offset);
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_0S(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1);
    EndtoAction(offset, 4);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_1S(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    if (!SetChangeFrameAnimeSameActno(offset)) {
        TimetoAction(offset, 5, offset->Mark);
    }
    SetObjFlag(offset, 1);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_2S(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    offset->InstSt |= 0x4000;
    SetNoAttack(offset);
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_0K(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1);
    EndtoAction(offset, 17);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_1K(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Mark = 20;
    }
    SetObjFlag(offset, 1);
    if (offset->ActTimer > offset->Mark) {
        SetActNoNext(offset, 18);
    }
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_2K(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    offset->InstSt |= 0x4000;
    SetNoAttack(offset);
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_0K2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1);
    EndtoAction(offset, 20);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_1K2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Mark = 20;
    }
    SetObjFlag(offset, 1);
    if (offset->ActTimer > offset->Mark) {
        SetActNoNext(offset, 21);
    }
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_2K2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    offset->InstSt |= 0x4000;
    SetNoAttack(offset);
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_0K3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1);
    EndtoAction(offset, 23);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_1K3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Mark = 20;
    }
    SetObjFlag(offset, 1);
    if (offset->ActTimer > offset->Mark) {
        SetActNoNext(offset, 24);
    }
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void MLRGAct_2K3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    offset->InstSt |= 0x4000;
    SetNoAttack(offset);
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    if (userinfo.gguse[parent->padid] & 4) {
        if (parent->actno == 0x5c) {
            offset->posx = parent->posx;
        }
    }
}

void CellMain(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
void ObjWorld2ScreenV(CHARACTER_WORK *offset, NL_POINT3 *p);
void gCurrentColorClear(void);
unsigned int gCurrentColorGet(void);
void nlCalcPoint(NL_POINT3 *p1, NL_POINT3 *p2);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);
void nlRotateAX(u_short r);
void nlRotateAZ(u_short r);
void nlUnitMatrix(void);




void MLRASENDraw(CHARACTER_WORK *offset) {
    NL_POINT3 p[4];
    NL_POINT3 pos;
    NLVERTEX03 vt[4];
    float r1, r2, s, c, z, f;
    int ang, i;
    NL_POINT3 *pp;
    NLVERTEX03 *vp;

    ang = 0;
    CellMain(offset);
    ObjWorld2ScreenV(offset, &pos);
    SetBlendingMode(offset->transmode);
    nlUnitMatrix();
    nlRotateAX(0xc000);
    nlRotateAZ(offset->ActTimer * 0x800);
    vt[0].col = vt[1].col = gCurrentColorGet() | (offset->trans << 24);
    vt[2].col = vt[3].col = gCurrentColorGet() | (offset->trans << 24);
    r1 = offset->ActTimer * 6 + 50;
    r2 = offset->ActTimer * 6 + 65;
    p[0].z = p[1].z = r1 * 0.2 + (r1 - 30.0) * 0.2;
    p[2].z = p[3].z = r2 * 0.2 + (r2 - 30.0) * 0.2;
    for (ang = 0; ang < 0x10000; ang += 0x800) {

        s = G_Sin(ang) * pos.z;
        c = G_Cos(ang) * pos.z;
        p[0].x = s * r1;
        p[2].x = s * r2;
        p[0].y = c * r1;
        p[2].y = c * r2;
        p[0].z = ang / 1536;
        p[2].z = ang / 1536 + 16;
        s = G_Sin((ang + 0x800)) * pos.z;
        c = G_Cos((ang + 0x800)) * pos.z;
        p[1].x = s * r1;
        p[3].x = s * r2;
        p[1].y = c * r1;
        p[3].y = c * r2;
        p[1].z = (ang + 0x800) / 1536;
        p[3].z = (ang + 0x800) / 1536 + 16;
        pp = p;
        vp = vt;
        for (i = 0; i < 4; i++) {
            nlCalcPoint(pp, (NL_POINT3 *)vp);
            z = vp->z;
            f = 500.0 / (z + 500.0);
            vp->x *= f;
            vp->y *= f;
            if (offset->dirflag != 1) vp->x = -vp->x;
            vp->x += pos.x;
            vp->y += pos.y;
            if (z < 0.0f) vp->z = (float)offset->priority - 8.0;
            else vp->z = (float)offset->priority + 60.0;
            vp++;
            pp++;
        }
        vt[0].u = vt[2].u = ang * (1.0f / 32768.0f);
        vt[1].u = vt[3].u = (ang + 0x800) * (1.0f / 32768.0f);
        if (ang & 0x8000) {
            vt[0].v = vt[1].v = 0.5f;
            vt[2].v = vt[3].v = 0.0f;
        } else {
            vt[0].v = vt[1].v = 1.0f;
            vt[2].v = vt[3].v = 0.5f;
        }
        nlDispStripTexTrnsl(4, vt, offset->SpriteId);
    }
    gCurrentColorClear();
}

void MLRASENDraw(CHARACTER_WORK *offset);

void MLRASENAct(CHARACTER_WORK *offset) {
    offset->graphicfunc = MLRASENDraw;
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void MLSBOWL(CHARACTER_WORK *offset) {
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 14);
    SetJumpControl(offset);
    if (!IsHitStop(offset)) {
        offset->angle += 0x800;
    }
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void MLSBOWLMove(CHARACTER_WORK *offset) {
    static const int speedtb[18] = {
        0, 0, 0, -1000, 0, 1000, 1000, 0, -1000, 0,
        707, -707, -707, -707, 707, 707, -707, 707,
    };
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    u_char dir;

    SetObjFlag(offset, 1);
    if (offset->Localid == 0) dir = 3;
    else if (offset->Localid == 1) dir = parent->ply->PersonalWork[1].ub[0];
    else if (offset->Localid == 2) dir = parent->ply->PersonalWork[1].ub[1];
    else if (offset->Localid == 3) dir = parent->ply->PersonalWork[1].ub[2];
    else if (offset->Localid == 4) dir = parent->ply->PersonalWork[1].ub[3];
    else dir = 0;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Localid++;
        SetXSpeed(offset, speedtb[dir * 2]);
        SetYSpeed(offset, speedtb[dir * 2 + 1], 0);
    }
    if (offset->Localid >= 5 || dir == 0) EndtoAction(offset, 15);
    else EndtoAction(offset, 14);
    SetLandBreakActno(offset, 15);
    if (!IsHitStop(offset)) {
        offset->angle += 0x800;
    }
}

void MLSBOWLBreak(CHARACTER_WORK *offset) {
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    SetJumpControl(offset);
    if (!IsHitStop(offset)) {
        offset->angle += 0x400;
    }
}

void MLKR_ColliCheck(CHARACTER_WORK *offset) {
}

void MLKRAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (!IsHitStop(offset)) {
        offset->angle += 0x1000;
    }
    EndtoEliminate(offset);
}

void MLKRAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoEliminate(offset);
    if (parent->actno == 0x71) {
        if (offset->attackst & 0x80000) {
            parent->attackst |= 0x80000;
            parent->ply->MutekiTime = 30;
        }
        parent->Mark = 0;
    }
}

void MLKRAct_ERR(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void MLKRAct_21(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = (CHARACTER_WORK *)((CHARACTER_WORK *)offset->parentWork)->ply->Enemy;
    SetJumpControl(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        enemy->gravity = 0;
        enemy->yspeed = 0;
        offset->angle = 0x2000;
        SetXSpeed(offset, 4000);
        SetYSpeed(offset, 4000, 0);
        offset->posx = enemy->posx - offset->xspeed * 15;
        offset->posy = enemy->posy - offset->yspeed * 15 - 10000;
    }
    enemy->ply->DownTimer = 20;
    if (IsActTimeOver(offset, 15)) {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
        SetActNoNext(offset, 10);
    }
}

void MLKRAct_22(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = (CHARACTER_WORK *)((CHARACTER_WORK *)offset->parentWork)->ply->Enemy;
    SetJumpControl(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        enemy->gravity = 0;
        enemy->yspeed = 0;
        offset->angle = -0x2000;
        SetXSpeed(offset, -4000);
        SetYSpeed(offset, 4000, 0);
        offset->posx = enemy->posx - offset->xspeed * 15;
        offset->posy = enemy->posy - offset->yspeed * 15 - 10000;
    }
    if (IsActTimeOver(offset, 15)) {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
        SetActNoNext(offset, 11);
    }
}

void MLKRAct_23(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = (CHARACTER_WORK *)((CHARACTER_WORK *)offset->parentWork)->ply->Enemy;
    CREATE_ARG arg;
    CHARACTER_WORK *w;
    int i, a, b;

    if (SetChangeFrameAnimeSameActno(offset)) {
        arg.flag = 0x20;
        for (a = 0, i = 0; i < 4; a += 0x800, i++) {
            for (b = a; b < a + 0x10000; b += 0x1000) {
                arg.Localid = b;
                w = EffectCreate_ActNo(offset, 0x34, &arg, 0x17);
                if (w) w->HitBackx = i;
            }
        }
    }
    enemy->ply->DownTimer = 20;
    EndtoEliminate(offset);
}

void MLKRAct_24(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void MLKRAct_Circle0(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = (CHARACTER_WORK *)((CHARACTER_WORK *)offset->parentWork)->ply->Enemy;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posx = enemy->posx;
        offset->posy = 0;
    }
    EndtoAction(offset, 17);
}

CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
void ParentFlagSuccession(CHARACTER_WORK *poffset, CHARACTER_WORK *offset);

void MLKRAct_Circle1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *w;

    SetChangeFrameAnimeSameActno(offset);
    if (offset->HitStopTime == 0) {
        switch (offset->ActTimer & 7) {
        case 0:
            w = ObjectCreate_ActNo(offset, 0x2e, 0, 6);
            if (w) {
                w->posx += (Random() % 100 - 50) * 200;
                ParentFlagSuccession(offset, w);
            }
            break;
        case 2:
            w = EffectCreate_ActNo(offset, 0x2e, 0, 4);
            if (w) {
                w->posx += (Random() % 100 - 50) * 200;
                ParentFlagSuccession(offset, w);
            }
            break;
        case 4:
            w = ObjectCreate_ActNo(offset, 0x2e, 0, 7);
            if (w) {
                w->posx += (Random() % 100 - 50) * 200;
                ParentFlagSuccession(offset, w);
            }
            break;
        case 6:
            w = EffectCreate_ActNo(offset, 0x2e, 0, 5);
            if (w) {
                w->posx += (Random() % 100 - 50) * 200;
                ParentFlagSuccession(offset, w);
            }
            break;
        }
    }
    if (IsActTimeOver(offset, 60)) {
        EndtoAction(offset, 18);
    }
}

void MLKRAct_Circle2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void MLKRAct_Circle3(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 20);
}

void MLKRAct_Circle4(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (IsActTimeOver(offset, 60)) {
        EndtoAction(offset, 21);
    }
}

void MLKRAct_Circle5(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void MLKRAct_LandCircle0(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posy = 0;
    }
    if (IsActTimeOver(offset, 60)) {
        offset->actst |= 0x80000000;
    }
}

float GetObjScale(void);
float ObjWorld2ScreenX(float x);
float ObjWorld2ScreenY(float y);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);

void MLKRAct_PillarDraw(CHARACTER_WORK *offset) {
    NL_POINT3 p;
    NLVERTEX00 vt[10];
    float w, h, scale, z;
    u_char a;

    w = offset->Localid;
    h = offset->attackst;
    a = offset->trans;
    scale = GetObjScale();
    p.x = ObjWorld2ScreenX(offset->posx);
    p.y = ObjWorld2ScreenY(offset->posy);
    z = offset->priority;
    vt[0].x = vt[1].x = scale * w + p.x;
    vt[2].x = vt[3].x = scale * w * 10.0f * 0.0625f + p.x;
    vt[4].x = vt[5].x = p.x + 0.0f;
    vt[6].x = vt[7].x = -scale * w * 10.0f * 0.0625f + p.x;
    vt[8].x = vt[9].x = -scale * w + p.x;
    vt[0].y = vt[2].y = vt[4].y = vt[6].y = vt[8].y = p.y;
    vt[1].y = vt[3].y = vt[5].y = vt[7].y = vt[9].y = -h * scale + p.y;
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = vt[4].z = vt[5].z = vt[6].z = vt[7].z = vt[8].z = vt[9].z = z;
    vt[0].col = vt[1].col = vt[8].col = vt[9].col = 0x8040ff;
    vt[2].col = vt[3].col = vt[6].col = vt[7].col = (a << 24) | 0xe0c0ff;
    vt[4].col = vt[5].col = (a << 24) | 0xffffff;
    SetBlendingMode(1);
    nlDispStripTrnsl(10, vt);
    SetBlendingMode(0);
}

void MLKRAct_PillarDraw(CHARACTER_WORK *offset);

void MLKRAct_Pillar(CHARACTER_WORK *offset) {
    static u_char size[11] = { 8, 14, 22, 32, 44, 58, 74, 92, 112, 134, 160 };
    int t;

    offset->graphicfunc = MLKRAct_PillarDraw;
    SetChangeFrameAnimeSameActno(offset);
    if (IsActTimeOver(offset, 80)) {
        offset->actst |= 0x80000000;
    } else if (IsActTimeOver(offset, 70)) {
        offset->trans -= 10;
        offset->Localid /= 2;
    } else if (IsActTimeOver(offset, 20)) {
        offset->Localid = 160;
        offset->trans = 0xff;
    } else if ((t = IsActTimeOver(offset, 10))) {
        offset->Localid = size[t];
        offset->attackst = 1000;
    } else {
        offset->Localid = 4;
        offset->attackst += 100;
    }
}

void MLKRAct_HW4(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, Random() & 3)) {
        SetXSpeed(offset, (Random() & 0xff) + 400);
        SetYSpeed(offset, -(Random() & 0x3ff), 0);
    }
    SetJumpControl(offset);
    offset->xspeed -= offset->xspeed / 10.0;
    offset->yspeed -= offset->yspeed / 10.0;
    EndtoEliminate(offset);
}

float nlSin(short unsigned int r);
float nlCos(short unsigned int r);

void MLKRAct_KILL(CHARACTER_WORK *offset) {
    int spd[4] = { 1800, 2000, 2400, 2600 };
    float dv[4] = { 8.0f, 10.0f, 12.0f, 14.0f };

    if (SetChangeFrameAnime(offset, 23)) {
        SetXSpeed(offset, nlSin(offset->Localid) * spd[offset->HitBackx]);
        SetYSpeed(offset, nlCos(offset->Localid) * spd[offset->HitBackx], 0);
    }
    offset->xspeed -= offset->xspeed / dv[offset->HitBackx];
    offset->yspeed -= offset->yspeed / dv[offset->HitBackx];
    offset->posy += offset->yspeed;
    EndtoEliminate(offset);
}

void MLRS_ColliCheck(CHARACTER_WORK *offset) {
}

void MLRSAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 1);
}

void MLRSAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    if (parent->actno != 0x62) {
        offset->actst |= 0x80000000;
    }
    if (IsActEnd(offset)) {
        if (parent->Mark >= 2) {
            SetActNoNext(offset, 2);
        }
    }
}

void MLRSAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = (CHARACTER_WORK *)offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    if (parent->actno != 0x62) {
        offset->actst |= 0x80000000;
    }
    if (IsActEnd(offset)) {
        if (parent->Mark < 3) {
            SetActNoNext(offset, 1);
        }
    }
}

void MLRSAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 8) {
        offset->actst |= 0x80000000;
    }
}
