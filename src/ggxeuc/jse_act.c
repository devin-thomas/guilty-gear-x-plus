#include "generated/jse_act.h"

int SetActNo(CHARACTER_WORK *offset, unsigned int no);

void JSEX_ColliCheck(CHARACTER_WORK *offset) {
    if (offset->actno >= 6 && offset->actno <= 7 && (*(unsigned int *)&offset->ColliFlag & 0xFFFF00)) {
        SetActNo(offset, 8);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetPriorityMain(CHARACTER_WORK *offset, int PriAdd, int flag);

void JSEFAct_MoveEXColli(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    COLLISION *colli;
    int Localid;
    int Localst;

    poffset = GetParentPlayer(offset);
    Localid = offset->Localid & 0xF;
    Localst = offset->Localid & ~0xF;
    SetChangeFrameAnimeSameActno(offset);
    switch (Localid) {
    case 0:
    case 1:
        if (Localst & 0x20) {
            colli = Get_GCLExNo(poffset, Localid + 1);
        } else {
            colli = Get_GCLExNo(poffset, Localid);
        }
        if (Localst & 0x10) {
            SetPriorityMain(offset, 0x37, 1);
        } else {
            SetPriorityMain(offset, -10, 1);
        }
        break;
    case 2:
    case 3:
        if (Localst & 0x20) {
            colli = Get_GCLExNo(poffset, Localid - 1);
        } else {
            colli = Get_GCLExNo(poffset, Localid);
        }
        if (Localst & 0x10) {
            SetPriorityMain(offset, 0x37, 1);
        } else {
            SetPriorityMain(offset, -10, 1);
        }
        if (poffset->dirflag == 1) {
            offset->dirflag = 0;
        } else {
            offset->dirflag = 1;
        }
        break;
    default:
        colli = Get_GCLExNo(poffset, 0);
        SetPriorityMain(offset, 0x37, 1);
        break;
    }
    if (colli) {
        if (poffset->dirflag == 1) {
            offset->posx = poffset->posx - colli->x * 100;
        } else {
            offset->posx = poffset->posx + colli->x * 100;
        }
        offset->posy = poffset->posy + colli->y * 100;
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetPriorityMain(CHARACTER_WORK *offset, int PriAdd, int flag);

void JSEFAct_Hissatu4_Start(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    COLLISION *colli;
    int index;

    poffset = GetParentPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    index = 0;
    if (offset->ActTimer >= 6) {
        index = (offset->ActTimer <= 7) ? 1 : 2;
    }
    colli = Get_GCLExNo(poffset, index);
    if (colli) {
        if (poffset->dirflag == 1) {
            offset->posx = poffset->posx - colli->x * 100;
        } else {
            offset->posx = poffset->posx + colli->x * 100;
        }
        offset->posy = poffset->posy + colli->y * 100;
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetPriorityMain(CHARACTER_WORK *offset, int PriAdd, int flag);

void JSEFAct_IRay_Beam(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    COLLISION *colli;

    poffset = GetParentPlayer(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = -0x6000;
        offset->scale = 12000;
        offset->scaleY = 1;
    }
    if (offset->HitStopTime == 0) {
        colli = Get_GCLExNo(poffset, 0);
        SetPriorityMain(offset, -10, 1);
        if (colli) {
            if (poffset->dirflag == 1) {
                offset->posx = poffset->posx - colli->x * 100;
            } else {
                offset->posx = poffset->posx + colli->x * 100;
            }
            offset->posy = poffset->posy + colli->y * 100;
        }
        if (offset->ActTimer < 16) {
            offset->angle = -0x6000;
            offset->scaleY += 100;
        } else if (offset->angle != 0x4000) {
            offset->angle += 0x400;
        } else {
            offset->scaleY -= 100;
            if (offset->scaleY <= 0) {
                offset->scaleY = 1;
            }
        }
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetPriorityMain(CHARACTER_WORK *offset, int PriAdd, int flag);

void JSEFAct_IRay_Colli(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    COLLISION *colli;
    int angleflag;
    short unsigned int angle;
    int xlength;
    int ylength;

    angleflag = 0;
    poffset = GetParentPlayer(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = -0x3000;
    }
    if (offset->HitStopTime == 0) {
        if (offset->angle == 0x6000) {
            offset->actst |= 0x80000000;
        }
        colli = Get_GCLExNo(poffset, 0);
        SetPriorityMain(offset, -10, 1);
        if (colli) {
            if (poffset->dirflag == 1) {
                offset->posx = poffset->posx - colli->x * 100;
            } else {
                offset->posx = poffset->posx + colli->x * 100;
            }
            offset->posy = poffset->posy + colli->y * 100;
        }
        if (offset->ActTimer < 16) {
            offset->angle = -0x6000;
        } else {
            offset->angle += 0x400;
        }
        angle = offset->angle;
        if (angle >= 0x8000 && angle < 0xC000) {
            angleflag = 0;
            angle = 0xC000 - angle;
        } else if (angle > 0xBFFF) {
            angleflag = 1;
            angle = angle + 0x4000;
        } else if (angle < 0x7FFF) {
            if (angle > 0x4000) {
                angle = 0x4000;
            }
            angleflag = 2;
            angle = 0x4000 - angle;
        }
        if (offset->Localid == 0) {
            offset->scale = 1500;
        } else {
            offset->scale = 1000;
        }
        xlength = offset->Localid * 20000 * G_Sin(angle);
        ylength = offset->Localid * 20000 * G_Cos(angle);
        if (offset->dirflag == 1) {
            if (angleflag == 0) {
                offset->posx -= xlength;
                offset->posy += ylength;
            } else if (angleflag == 1) {
                offset->posx += xlength;
                offset->posy += ylength;
            } else if (angleflag == 2) {
                offset->posx += xlength;
                offset->posy -= ylength;
            }
        } else {
            if (angleflag == 0) {
                offset->posx += xlength;
                offset->posy += ylength;
            } else if (angleflag == 1) {
                offset->posx -= xlength;
                offset->posy += ylength;
            } else if (angleflag == 2) {
                offset->posx -= xlength;
                offset->posy -= ylength;
            }
        }
        if (offset->angle == 0x6000) {
            offset->actst |= 0x80000000;
        }
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void JSEFAct_Hissatu4_Effect(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void JSEXAct_ITHahen(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    LandtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
void OutSideEliminate(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void JSEFAct_Hissatu5_Start(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    SetLandBreakActno(offset, 8);
    SetObjFlag(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->Mark && IsTrgAndOffPunch(poffset) && poffset->HitStopTime == 0) {
        SetActNo(offset, 8);
    }
    OutSideEliminate(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void JSEFAct_Hissatu5_Burst(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetObjFlag(offset, 1);
    if (SetChangeFrameAnimeSameActno(offset)) {
        if (g_info.mode & 0x4000) {
            set_random_seed_abs();
        }
    }
    EndtoEliminate(offset);
}
