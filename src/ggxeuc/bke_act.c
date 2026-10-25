#include "generated/bke_act.h"

void BKTA_ColliCheck(CHARACTER_WORK *offset) {
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void BKTAAct_Sky(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetLandBreakActno(offset, 4);
}

void BKSA_ColliCheck(CHARACTER_WORK *offset) {
}

int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void BKSA_Act(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    LandtoEliminate(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKEntryB_EF1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    SetChangeFrameAnime(offset, 9);
    if (parent->Mark & 1) {
        offset->actst |= 0x80000000;
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void BKEntryB_EF2(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 0xA);
    SetLandBreakActno(offset, 0xB);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKEntryB_EF3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0xB);
    EndtoEliminate(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void BKEntryC_EF0(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    SetChangeFrameAnime(offset, 0xC);
    if (parent->Mark & 1) {
        offset->actst |= 0x80000000;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int LandtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void BKEntryC_EF1(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnime(offset, 0xD);
    EndtoEliminate(offset);
    LandtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKITI_BG1(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posx = 0;
        offset->posy = 0;
    }
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKITI_BG2(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posx = 51200;
        offset->posy = 0;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKItigeki_obj1(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 0x8001;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKItigeki_objDef(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    SetChangeFrameAnimeSameActno(offset);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKItigeki_Blood(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->posx = eoffset->posx;
        offset->posy = eoffset->posy;
        offset->dirflag = eoffset->dirflag == 0;
    }
}

void CellMain(CHARACTER_WORK *offset);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void BK_WhiteBoardDraw(CHARACTER_WORK *offset) {
    CellMain(offset);
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, offset->priority, gStripGetFadeColor(0xFF808080));
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BK_WhiteBoardDraw(CHARACTER_WORK *offset);

void BKItigeki_WhiteBoard(CHARACTER_WORK *offset) {
    offset->graphicfunc = BK_WhiteBoardDraw;
    SetChangeFrameAnimeSameActno(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetExCollision(CHARACTER_WORK *offset, short int x, short int y, short unsigned int w, short unsigned int h, unsigned int attr);

void BKItigeki_Murder(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    SetChangeFrameAnimeSameActno(offset);
    offset->posx = eoffset->posx;
    offset->posy = eoffset->posy;
    SetExCollision(offset, -200, -200, 400, 400, 1);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void BKMJ_Def(CHARACTER_WORK *offset) {
    SetAnten(offset, 0);
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}
