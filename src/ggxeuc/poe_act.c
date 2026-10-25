#include "generated/poe_act.h"

int SetActNo(CHARACTER_WORK *offset, unsigned int no);

void POBM_ColliCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 9 && offset->HitColliFlag) {
        SetActNo(offset, 0x1000A);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
void RandMove(CHARACTER_WORK *offset, unsigned int val);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void POBMAct_1(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        RandMove(offset, 0x40);
    }
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void POBMAct_PBFire1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetParentPosExNo(offset, 0);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void POBMAct_PBFire2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetParentPosExNo(offset, 1);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void POBMAct_SHW2_Fire(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (!(offset->actst & 0x80000000)) {
        SetParentPosExNo(offset, 0);
    }
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void POBMAct_SHW2_Ice(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    if (!(offset->actst & 0x80000000)) {
        SetParentPosExNo(offset, 1);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void POBMAct_POE2_1(CHARACTER_WORK *offset) {
    SetAnten(offset, 0);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void POBMAct_POE2_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void POBMAct_POE2_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}

int Random(void);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void POBMAct_POED_1(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetXSpeed(offset, -400 - (Random() & 0x1FF));
        SetYSpeed(offset, -600 - (Random() & 0x1FF), 50);
    }
    offset->angle += 0x2000;
    SetLandBreakActno(offset, 0x1000D);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int privatese_request(CHARACTER_WORK *offset, int no);

void POBMAct_POED_2(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->yspeed = -offset->yspeed / 2;
        offset->xspeed = offset->xspeed / 2;
        privatese_request(offset, 6);
    }
    offset->angle += 0x2000;
    SetJumpControl(offset);
    EndtoEliminate(offset);
    LandtoEliminate(offset);
}

int Random(void);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void POBMAct_POED_3(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetXSpeed(offset, -300 - (Random() & 0xFF));
        SetYSpeed(offset, -(Random() & 0xFF), 50);
    }
    offset->angle += 0x2000;
    SetLandBreakActno(offset, 0x1000D);
}

int GetXMove(CHARACTER_WORK *offset, int val);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void POGBAct_0(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posx += GetXMove(offset, -20000);
        offset->posy += -60000;
    }
    SetLandBreakActno(offset, 1);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void POGBAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
}
