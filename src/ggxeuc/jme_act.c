#include "generated/jme_act.h"
void JMHinokoPut(CHARACTER_WORK *offset);
void JMFEHinokoPut(CHARACTER_WORK *offset);
void JMCpObjFunc(CHARACTER_WORK *offset);
void JMSSDraw(CHARACTER_WORK *offset);

void JMHS_ColliCheck(CHARACTER_WORK *offset) {
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMHSAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMHSAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMHSAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);

void JMHinokoPut(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    if (offset->HitStopTime == 0) {
        arg.flag = 0x200;
        EffectCreate_ActNo(offset, 0x26, &arg, 4);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
void JMHinokoPut(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMFKAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    JMHinokoPut(offset);
    EndtoEliminate(offset);
}

void JMHinokoPut(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void JMFKAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    JMHinokoPut(offset);
    if (offset->parentFlag & 0x60) {
        SetActNoNext(offset, 2);
    }
    TimetoAction(offset, 2, 3);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void JMHinokoPut(CHARACTER_WORK *offset);
void ObjIndependence(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JMFKAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    JMHinokoPut(offset);
    ObjIndependence(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void RandMove(CHARACTER_WORK *offset, unsigned int val);
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
float nlCos(short unsigned int r);
float nlSin(short unsigned int r);

void JMFKAct_4(CHARACTER_WORK *offset) {
    u_short r;

    if (SetChangeFrameAnime(offset, 4)) {
        r = (Random() & 0xFF) << 8;
        SetXSpeed(offset, nlCos(r) * 40.0f);
        SetYSpeed(offset, nlSin(r) * 40.0f, 0);
        SetLandBreakActno(offset, -1);
        RandMove(offset, 0x10);
    }
    EndtoEliminate(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int ParentDamagetoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMTMAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    ParentDamagetoAction(offset, 6);
    EndtoAction(offset, 6);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void ObjIndependence(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMTMAct_1(CHARACTER_WORK *offset) {
    offset->InstSt |= 0x4000;
    SetChangeFrameAnimeSameActno(offset);
    ObjIndependence(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int Random(void);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
float nlCos(short unsigned int r);
float nlSin(short unsigned int r);

void JMTMAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;
    u_short r;

    SetJumpControl(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        r = (Random() & 0xFF) << 8;
        SetXSpeed(offset, nlSin(r) * (parent->scale / 4));
        SetYSpeed(offset, nlCos(r) * (parent->scale / 4), 0);
        offset->posx -= offset->xspeed * 30;
        offset->posy -= offset->yspeed * 30;
    }
    EndtoEliminate(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMDSAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->posy += 1600;
    if (offset->posy >= offset->parenty * 100) {
        offset->posy = offset->parenty * 100;
        SetActNoNext(offset, offset->actno + 1);
    }
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMDSAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMDSAct_Jump(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->posy -= 2400;
    if (offset->posy <= -64000) {
        offset->actst |= 0x80000000;
    }
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMDSAct_2ndFall(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->posy += 1600;
    if (offset->posy >= offset->parenty * 100) {
        offset->posy = offset->parenty * 100;
        SetActNoNext(offset, offset->actno + 3);
    }
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMDSAct_End_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

void JumpControl(CHARACTER_WORK *offset);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void JMDSAct_End_2(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetYSpeed(offset, -160, 80);
    }
    JumpControl(offset);
    if (offset->posy > offset->parenty * 100) {
        SetYSpeed(offset, 0, 0);
        SetAnimeNo(offset, offset->actno);
        offset->posy = offset->parenty * 100;
    }
}

void JumpControl(CHARACTER_WORK *offset);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void JMDSAct_End_3(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetYSpeed(offset, -320, 80);
    }
    JumpControl(offset);
    if (offset->posy > offset->parenty * 100) {
        SetYSpeed(offset, 0, 0);
        SetAnimeNo(offset, offset->actno);
        offset->posy = offset->parenty * 100;
    }
}

void GraphicExecute(CHARACTER_WORK *offset);

extern GAME_INFOMATION g_info;

void JMCpObjFunc(CHARACTER_WORK *offset) {
    if (!(g_info.BattleControlFlag & 2)) {
        GraphicExecute(offset);
    }
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
/* JMCpObjFunc: no debug record */

extern USER_SETTING userinfo;

void JMDSAct_Icon00(CHARACTER_WORK *offset) {
    int dir = offset->padid ? 1 : -1;
    CHARACTER_WORK *parent = GetParentPlayer(offset);
    int lower = userinfo.offs_cockpit_lower;
    int no;
    int x;

    offset->graphicfunc = JMCpObjFunc;
    SetChangeFrameAnimeSameActno(offset);
    if (offset->actno >= 0x16) {
        x = 0x60;
        no = offset->actno - 0x16;
        if (parent->ply->PersonalWork[1].ub[0] < no + 1) {
            offset->actst |= 0x80000000;
        }
    } else if (offset->actno >= 0x13) {
        x = 0x40;
        no = offset->actno - 0x13;
        if (parent->ply->PersonalWork[1].ub[1] < no + 1) {
            offset->actst |= 0x80000000;
        }
    } else if (offset->actno >= 0x10) {
        x = 0x20;
        no = offset->actno - 0x10;
        if (parent->ply->PersonalWork[0].ub[2] < no + 1) {
            offset->actst |= 0x80000000;
        }
    } else {
        x = 0;
        no = offset->actno - 0xD;
        if (parent->ply->PersonalWork[1].ub[2] < no + 1) {
            offset->actst |= 0x80000000;
        }
    }
    offset->SpriteFlag |= 1;
    offset->posy = (lower - (no * 8 - 440)) * 100;
    offset->posx = (dir * (300 - x) + 320) * 100;
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);

void JMFEHinokoPut(CHARACTER_WORK *offset) {
    CREATE_ARG arg;

    if (!(offset->ActTimer & 7) && offset->HitStopTime == 0) {
        arg.flag = 0x200;
        EffectCreate_ActNo(offset, 0x2F, &arg, 8);
    }
}

int IsHitStop(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void JMFEAct_0(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->scale = 25;
        offset->scaleY = 1000;
        offset->Mark = 0;
    }
    SetJumpControl(offset);
    if (IsHitStop(offset)) {
        return;
    }
    if (offset->Mark == 0) {
        offset->scaleY += 100;
        if (offset->scaleY > 2000) {
            offset->Localid = 0;
            offset->Mark = 1;
        }
    } else if (offset->Mark == 1) {
        offset->Localid++;
        if (offset->Localid >= 5) {
            offset->Localid = 0;
            offset->Mark = 2;
        }
    } else if (offset->Mark == 2) {
        offset->scaleY = offset->scaleY - (offset->scaleY - 1000) * 0.15;
        if (offset->scaleY < 1010) {
            offset->scaleY = 1000;
        }
        offset->scale += offset->Localid / 2;
        if (offset->Localid >= 30) {
            offset->Localid += 10;
        } else {
            offset->Localid += 1;
        }
        if (offset->scale > 1000) {
            offset->scale = 1000;
            SetActNoNext(offset, 6);
        }
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void JMFEHinokoPut(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void JMFEAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoAction(offset, 7);
    JMFEHinokoPut(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMFEAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMFEAct_A(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (offset->Mark & 2) {
        offset->actst |= 0x80000000;
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMGDAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = GetParentPlayer(offset);

    SetChangeFrameAnimeSameActno(offset);
    if (parent->actst & 0x4020) {
        SetActNoNext(offset, 3);
    }
    EndtoAction(offset, 3);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
/* xtb: no debug record */
/* ytb: no debug record */

void JMFNAct(CHARACTER_WORK *offset) {
    static int xtb[8] = { 160, 160, 160, 160, 480, 480, 480, 480 };
    static int ytb[8] = { 90, 190, 290, 390, 90, 190, 290, 390 };
    CHARACTER_WORK *parent = offset->parentWork;
    float rate;

    offset->SpriteFlag |= 1;
    SetChangeFrameAnimeSameActno(offset);
    if (offset->ActTimer < 10) {
        offset->scale = 3000;
        offset->posx = 32000;
        offset->posy = 24000;
        offset->trans = 0xFF;
    } else if (offset->ActTimer < 30) {
        rate = (offset->ActTimer - 10.0f) / 20.0f;
        offset->scale = 3000 - (offset->ActTimer - 10) * 100;
        offset->posx = ((xtb[offset->actno - 3] - 320) * rate + 320.0f) * 100.0f;
        offset->posy = ((ytb[offset->actno - 3] - 240) * rate + 240.0f) * 100.0f;
    } else {
        offset->scale = 1000;
        offset->posx = xtb[offset->actno - 3] * 100;
        offset->posy = ytb[offset->actno - 3] * 100;
    }
    if (parent->actno != 0x89 && parent->actno != 0x66) {
        if (offset->trans < 25) {
            offset->actst |= 0x80000000;
        }
        offset->trans -= 50;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int Random(void);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void JMKRAct_Yuge(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->trans = 0xFF;
        offset->posx += ((Random() & 0x3F) - 0x20) * 100;
        offset->posy += ((Random() & 7) - 4) * 100;
    }
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JMBGAct(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    offset->SpriteFlag |= 1;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->trans = 0xFF;
    }
    offset->posx = 32000;
    offset->posy = 24000;
    if (parent->actno != 0x89 && parent->actno != 0x66) {
        if (offset->trans < 25) {
            offset->actst |= 0x80000000;
        }
        offset->trans -= 50;
    }
}

void polydraw_FBoxOPQ(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void JMSSDraw(CHARACTER_WORK *offset) {
    polydraw_FBoxOPQ(0.0f, 0.0f, 640.0f, offset->posy, 4.0f, 0);
    polydraw_FBoxOPQ(0.0f, 480.0f, 640.0f, 480 - offset->posy, 4.0f, 0);
}

/* JMSSDraw: no debug record */
extern GAME_INFOMATION g_info;

extern GAME_INFOMATION g_info;

void JMSSAct(CHARACTER_WORK *offset) {
    offset->graphicfunc = JMSSDraw;
    offset->SpriteFlag |= 1;
    if (((CHARACTER_WORK *)offset->parentWork)->actno == 0x89 || ((CHARACTER_WORK *)offset->parentWork)->actno == 0x66) {
        g_info.BattleControlFlag |= 2;
        offset->posy = 40;
    } else {
        g_info.BattleControlFlag &= ~2;
        offset->posy -= 4;
        if (offset->posy <= 0) {
            offset->actst |= 0x80000000;
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int LandtoEliminate(CHARACTER_WORK *offset);
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
float nlCos(short unsigned int r);

void JMKRAct_0D(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    if (SetChangeFrameAnime(offset, 0xD)) {
        if (offset->dirflag) {
            offset->xspeed = -((Random() & 0xFF) + 600);
        } else {
            offset->xspeed = (Random() & 0xFF) + 600;
        }
        SetYSpeed(offset, nlCos((Random() & 0xFF) << 8) * 500.0f, 0);
    }
    EndtoEliminate(offset);
    LandtoEliminate(offset);
}
