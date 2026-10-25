#include "generated/sle_act.h"

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int EndtoEliminate(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
void SetSearchExecProcAddr(void *addr);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SLGF_NoAttack_Proc(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
void ObjIndependence(CHARACTER_WORK *offset);
int IsMaxHit(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetParentPosEx(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int LandtoEliminate(CHARACTER_WORK *offset);

#include "generated/eff_act.h"

void (*SLTR_Act_jumpTb[])() = {
    SLTRAct_0, SLTRAct_1, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, SLGFAct_1, SLGFAct_2,
    SLGFAct_3, CMNEFFActDef, SLGFAct_4, CMNEFFActDef, CMNEFFActDef, SLVVAct_1, SLVVAct_1,
    SLVVAct_2, SLVVAct_3, CMNEFFActDef, SLVIVVAct_1, SLVIVVAct_1, SLVIVVAct_2, SLVIVVAct_3,
    SLGVEFAct_1, SLGFEFAct_1, CMNEFFActDef, CMNEFFActWithJumpDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, 0,
};

void (*SLSW_Act_jumpTb[])() = { SLSWAct_00, SLSWAct_01, 0 };

void SLGF_NoAttack_Proc(CHARACTER_WORK *offset) {
    offset->Localid |= 0x200;
    offset->actst |= 0x40;
}

void SLTR_ColliCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 1 && offset->HitColliFlag) {
        offset->HitStopTime = 22;
    }
    if (offset->actno == 14) {
        if (offset->actnoSv != ((CHARACTER_WORK *)offset->parentWork)->actno) {
            SetActNo(offset, 15);
        }
    }
    if (((offset->actno >= 5 && offset->actno <= 7) || offset->actno == 9) && offset->HitColliFlag) {
        SetSearchExecProcAddr(SLGF_NoAttack_Proc);
        SearchCharacterExecute(offset, 0x2C, 5, 0x27);
        SetSearchExecProcAddr(SLGF_NoAttack_Proc);
        SearchCharacterExecute(offset, 0x2C, 6, 0x27);
        SetSearchExecProcAddr(SLGF_NoAttack_Proc);
        SearchCharacterExecute(offset, 0x2C, 7, 0x27);
        SetSearchExecProcAddr(SLGF_NoAttack_Proc);
        SearchCharacterExecute(offset, 0x2C, 9, 0x27);
    }
}

void SLTRAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 1);
}

void SLTRAct_1(CHARACTER_WORK *offset) {
    SetAnten(offset, 1);
    if (SetChangeFrameAnimeSameActno(offset)) {
        ObjIndependence(offset);
    }
    TimetoAction(offset, 2, 8);
    if (IsMaxHit(offset)) {
        SetActNoNext(offset, 2);
    }
}

void SLTRAct_2(CHARACTER_WORK *offset) {
    SetAnten(offset, 1);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void SLGFAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void SLGFAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->actst |= parent->actst & 0x40;
    }
    EndtoEliminate(offset);
    if (parent->idno != 0x2C) {
        offset->Mark = 1;
    }
    if (parent->padid != offset->padid) {
        offset->Mark = 1;
    }
    if (!offset->Mark) {
        offset->actst &= ~6;
        offset->actst |= parent->actst & 6;
        offset->dirflag = parent->dirflag;
    }
}

void SLGFAct_3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->actst |= parent->actst & 0x40;
    }
    EndtoEliminate(offset);
    if (parent->idno != 0x2C) {
        offset->Mark = 1;
    }
    if (parent->padid != offset->padid) {
        offset->Mark = 1;
    }
    if (!offset->Mark) {
        offset->actst &= ~6;
        offset->actst |= parent->actst & 6;
        offset->dirflag = parent->dirflag;
    }
}

void SLGFAct_4(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->actst |= parent->actst & 0x40;
    }
    EndtoEliminate(offset);
    if (parent->idno != 0x2C) {
        offset->Mark = 1;
    }
    if (parent->padid != offset->padid) {
        offset->Mark = 1;
    }
    if (!offset->Mark) {
        offset->actst &= ~6;
        offset->actst |= parent->actst & 6;
        offset->dirflag = parent->dirflag;
    }
}

void SLVVAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    SetParentPosEx(offset);
}

void SLVVAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetParentPosEx(offset);
}

void SLVVAct_3(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        ObjIndependence(offset);
    }
    EndtoEliminate(offset);
}

void SLVIVVAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    SetParentPosEx(offset);
}

void SLVIVVAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 0x14);
}

void SLVIVVAct_3(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        ObjIndependence(offset);
    }
    EndtoEliminate(offset);
}

void SLGVEFAct_1(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    LandtoEliminate(offset);
}

void SLGFEFAct_1(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
    LandtoEliminate(offset);
}

void SLSWAct_00(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->yspeed = -2500;
    offset->angle += 0x1000;
    SetJumpControl(offset);
    EndtoAction(offset, 1);
}

void SLSWAct_01(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->yspeed = 2500;
    offset->angle += 0x1000;
    SetJumpControl(offset);
    EndtoEliminate(offset);
}
