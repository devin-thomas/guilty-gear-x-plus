#include "generated/kye_act.h"

#include "generated/eff_act.h"

void (*KYSE_Act_jumpTb[])() = {
    KYSEAct_00, KYSEAct_Fly, KYSEAct_Hit, KYSEAct_10, KYSEAct_Fly, KYSEAct_20, KYSEAct_Fly,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, KYSEAct_EntryC, CMNEFFActDef,
    KYSEAct_CS, KYSEAct_RTL, KYSEAct_RTLDisappear, KYSEAct_RTLS, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, KYSEAct_30, KYSEAct_Fly,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef,
    CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, 0,
};

int IsMaxHit(CHARACTER_WORK *offset);
void ParXSpeed(CHARACTER_WORK *offset, float par);
void ParYSpeed(CHARACTER_WORK *offset, float par);
void RenewalCollision(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetHitStop(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int type);
extern GAME_INFOMATION g_info;

void KYSE_ColliCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (offset->actno == 1) {
        if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
            if (g_info.boss[poffset->padid] == 1) {
                if (IsMaxHit(offset)) {
                    SetActNo(offset, 2);
                    return;
                }
                SetHitStop(offset, offset->ActHeader.flag, offset->ActHeader.lvflag, 0);
                ParXSpeed(offset, 0.7f);
                RenewalCollision(offset);
                offset->ActHeader.damage >>= 1;
            } else {
                SetActNo(offset, 2);
                return;
            }
        }
    } else if (offset->actno == 4) {
        if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
            if (IsMaxHit(offset)) {
                if (g_info.boss[poffset->padid] == 1) {
                    offset->ActHitCount = 0;
                } else {
                    SetActNo(offset, 0x10029);
                    return;
                }
            } else if (g_info.boss[poffset->padid] == 1) {
                offset->ActHitCount = 0;
            }
            SetHitStop(offset, offset->ActHeader.flag, offset->ActHeader.lvflag, 0);
            ParXSpeed(offset, 0.7f);
            return;
        }
    } else if (offset->actno == 6 || offset->actno == 36) {
        if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
            if (g_info.boss[poffset->padid] == 1) {
                if (IsMaxHit(offset)) {
                    SetActNo(offset, 2);
                    return;
                }
                SetHitStop(offset, offset->ActHeader.flag, offset->ActHeader.lvflag, 0);
                ParXSpeed(offset, 0.7f);
                ParYSpeed(offset, 0.7f);
                RenewalCollision(offset);
                offset->ActHeader.damage >>= 1;
            } else {
                SetActNo(offset, 2);
                return;
            }
        }
    }
    return;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_00(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 1);
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
extern GAME_INFOMATION g_info;

void KYSEAct_Fly(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (SetChangeFrameAnimeSameActno(offset)) {
        if (g_info.boss[poffset->padid] == 1) {
            offset->MaxHitCount = 3;
        }
    }
    SetObjFlag(offset, 1);
    SetLandBreakActno(offset, 2);
    OutSideEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);

void KYSEAct_Hit(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetObjFlag(offset, 1);
    SetXSpeed(offset, 0);
    EndtoEliminate(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_10(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 4);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_20(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 6);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_30(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 0x24);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetParentPosEx(CHARACTER_WORK *offset);

void KYSEAct_EntryC(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetParentPosEx(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_CS(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_RTL(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    if (poffset->Mark) {
        SetActNoNext(offset, 0x1B);
    }
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_RTLDisappear(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->scale = 1000;
    }
    if (offset->scale <= 100) {
        offset->actst |= 0x80000000;
    } else {
        offset->scale -= 50;
    }
}

float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);

void KY_RTLSPosSet(CHARACTER_WORK *offset, CHARACTER_WORK *poffset) {
    int angle;
    int r;

    if (offset->HitStopTime == 0) {
        if (offset->scale < 800) {
            offset->HitBackx += 0x300;
            offset->AdBackx += 5;
        } else {
            offset->HitBackx += 0x600;
            offset->AdBackx += 20;
        }
    }
    angle = offset->HitBackx;
    angle += offset->Localid << 12;
    r = offset->AdBackx;
    if (offset->dirflag == 1) {
        offset->posx = poffset->posx + G_Sin(angle) * r * 100.0f - offset->parentx * 100;
    } else {
        offset->posx = poffset->posx - G_Sin(angle) * r * 100.0f + offset->parentx * 100;
    }
    offset->posy = poffset->posy + G_Cos(angle) * r * 100.0f + offset->parenty * 100;
}

int EndtoEliminate(CHARACTER_WORK *offset);
void KY_RTLSPosSet(CHARACTER_WORK *offset, CHARACTER_WORK *poffset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KYSEAct_RTLS(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->AdBackx = 110;
    }
    KY_RTLSPosSet(offset, poffset);
    EndtoEliminate(offset);
}
