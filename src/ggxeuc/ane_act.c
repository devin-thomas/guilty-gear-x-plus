#include "generated/ane_act.h"

#include "generated/eff_act.h"

void (*ANNG_Act_jumpTb[])() = {
    ANNGAct_1, ANNGAct_1, ANNGAct_1, ANNGAct_1, ANNGAct_1, ANNGAct_1, ANNGAct_2, ANNGAct_3,
    ANNGAct_Kakusei1, ANNGAct_Kakusei2, ANNG5Act_1, ANNG5Act_2, CMNEFFActWithJumpDef, 0,
};

void (*ANBF_Act_jumpTb[])() = {
    ANBFAct_1, ANBFAct_2, ANBFAct_3, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef,
    ANBFAct_Kakusei3, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef,
    ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef,
    ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef,
    ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef, ANBFAct_WinDef,
    ANBFAct_WinDef, 0,
};

void (*ANUP_Act_jumpTb[])() = { ANUPAct_UnvisualObject, CMNEFFActWithJumpDef, 0 };

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANNGAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        CREATE_ARG c_arg;

        c_arg.flag = 5;
        c_arg.actno = 7;
        c_arg.x = offset->posx;
        c_arg.y = offset->posy;
        EffectCreate(offset, 0x4C, &c_arg);
        offset->actst |= 0x80000000;
    } else {
        SetJumpControl(offset);
        if (offset->posy >= 0) {
            CREATE_ARG c_arg;

            c_arg.flag = 5;
            c_arg.actno = 6;
            c_arg.x = offset->posx;
            c_arg.y = offset->posy;
            EffectCreate(offset, 0x4C, &c_arg);
            offset->actst |= 0x80000000;
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANNGAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    offset->angle = (u_short) offset->angle - 0x2000;
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANNGAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    offset->angle = (u_short) offset->angle + 0x2000;
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANNGAct_Kakusei1(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->Localid = 0xC;
    }
    if (offset->attackst & 0x80000) {
        offset->Localid = 9;
    }
    EndtoAction(offset, offset->Localid);
    SetJumpControl(offset);
    OutSideEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANNGAct_Kakusei2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    offset->dirflag = 0;
    EndtoEliminate(offset);
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANNG5Act_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
        return;
    }
    OutSideEliminate(offset);
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANNG5Act_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
        return;
    }
    if (offset->posy >= 0) {
        offset->actst |= 0x80000000;
    }
    SetJumpControl(offset);
    OutSideEliminate(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OutSideEliminate(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
float nlSin(short unsigned int r);

void ANBFAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->RHitBackx = offset->posy;
        poffset->ply->PersonalWork[0].l &= ~2;
        offset->Localid = 0;
    }
    if (poffset->actst & 0x20) {
        SetActNoNext(offset, 3);
        return;
    }
    if (offset->attackst & 0x880000) {
        SetActNoNext(offset, 3);
        return;
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 1);
        return;
    }
    offset->Localid += 0x100;
    offset->posy = nlSin(offset->Localid) * 1500.0 + offset->RHitBackx;
    EndtoAction(offset, 3);
    SetObjFlag(offset, 1);
    SetJumpControl(offset);
    OutSideEliminate(offset);
}

int IsLand(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void ANBFAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    SetChangeFrameAnime(offset, offset->actno);
    SetJumpControl(offset);
    SetObjFlag(offset, 1);
    SetObjFlag(offset, 2);
    if (poffset->actst & 0x20) {
        SetActNoNext(offset, 3);
        return;
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 3);
        return;
    }
    if (IsLand(offset)) {
        SetActNoNext(offset, 3);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
void OutSideEliminate(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void ANBFAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 3);
        return;
    }
    SetObjFlag(offset, 1);
    SetObjFlag(offset, 2);
    SetJumpControl(offset);
    OutSideEliminate(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void ANBFAct_Kakusei3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetExCollision(CHARACTER_WORK *offset, short int x, short int y, short unsigned int w, short unsigned int h, unsigned int attr);
int TimetoEliminate(CHARACTER_WORK *offset, int time);

void ANUPAct_UnvisualObject(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetExCollision(offset, -50, -400, 100, 400, 1);
    }
    TimetoEliminate(offset, 600);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void ANBFAct_WinDef(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    SetJumpControl(offset);
    EndtoEliminate(offset);
    offset->actst |= 0x100000;
}
