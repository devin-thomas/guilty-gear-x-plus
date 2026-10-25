#include "generated/che_act.h"

CHARACTER_WORK *SearchObjectWorkActnoLocalid(CHARACTER_WORK *off, short unsigned int idno, int actno, int Localid);
float G_Cos(short unsigned int r);
float G_Sin(short unsigned int r);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
int LandtoEliminate(CHARACTER_WORK *offset);
int Random(void);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetParentPosEx(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

#include "generated/eff_act.h"

void (*CHCB_Act_jumpTb[])() = { CHCBAct_0, CHCBAct_1, CHCBAct_2, 0 };

void (*CHBB_Act_jumpTb[])() = {
    CHBBAct_1, CHBBAct_2, CMNEFFActDef, CMNEFFActDef, CMNEFFActDef, CHBBAct_5, CHBBAct_5,
    CHITAct_7, CHITAct_8, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef,
    CHITAct_Moji, CHITAct_d, CHSRAct_1, CMNEFFActWithJumpDef, CMNEFFActWithJumpDef, 0,
};

void (*CHLF_Act_jumpTb[])() = { CHLFAct_0, CHLFAct_Circle, CHLFAct_2, CHLFAct_3, CHLFAct_Circle, 0 };

void (*CHTU_Act_jumpTb[])() = { CHKHAct_1, CHTUAct_1, CHTUAct_2, CHKHAct_1, CHKHAct_1, CHKHAct_1, CMNEFFActWithJumpDef, 0 };

void CHCB_ColliCheck(CHARACTER_WORK *offset) {
    if (*(unsigned int *)&offset->ColliFlag & 0xffff00) {
        SetActNo(offset, 1);
    }
}

void CHCBAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    EndtoAction(offset, 2);
}

void CHCBAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent;

    if (SetChangeFrameAnime(offset, 1)) {
        parent = offset->parentWork;
        if (parent->actno == 0x68 || parent->actno == 0xcb || parent->actno == 0xcc) {
            parent->Mark = 1;
        }
    }
    EndtoEliminate(offset);
}

void CHCBAct_2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent;

    SetChangeFrameAnime(offset, 2);
    if (EndtoEliminate(offset)) {
        parent = offset->parentWork;
        if (parent->actno == 0x68) {
            parent->Mark = 1;
        }
    }
}

void CHBBAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    EndtoEliminate(offset);
    SetParentPosEx(offset);
}

void CHBBAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    EndtoEliminate(offset);
    SetParentPosEx(offset);
}

void CHBBAct_5(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno)) {
        SetXSpeed(offset, (Random() & 3) * -200);
        SetYSpeed(offset, (Random() & 3) * -250, 20);
    }
    if (offset->xspeed) {
        offset->xspeed -= offset->xspeed / 10;
    }
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void CHITAct_7(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        switch (offset->Localid) {
        case 0:
            SetXSpeed(offset, G_Sin(0x8000) * 1000.0f);
            SetYSpeed(offset, G_Cos(0x8000) * 1000.0f, 0);
            break;
        case 1:
            SetXSpeed(offset, G_Sin(0xb300) * 1000.0f);
            SetYSpeed(offset, G_Cos(0xb300) * 1000.0f, 0);
            break;
        case 2:
            SetXSpeed(offset, G_Sin(0xe600) * 1000.0f);
            SetYSpeed(offset, G_Cos(0xe600) * 1000.0f, 0);
            break;
        case 3:
            SetXSpeed(offset, G_Sin(0x1900) * 1000.0f);
            SetYSpeed(offset, G_Cos(0x1900) * 1000.0f, 0);
            break;
        case 4:
            SetXSpeed(offset, G_Sin(0x4c00) * 1000.0f);
            SetYSpeed(offset, G_Cos(0x4c00) * 1000.0f, 0);
            break;
        }
    }
    SetJumpControl(offset);
    if (offset->attackst & 0x80000) {
        ((CHARACTER_WORK *)offset->parentWork)->attackst |= 0x80000;
        SetActNoNext(offset, 8);
    }
}

void CHITAct_8(CHARACTER_WORK *offset) {
    CHARACTER_WORK *work;

    if (SetChangeFrameAnimeSameActno(offset)) {
        switch (offset->Localid) {
        case 0:
            offset->angle = -0x4d00;
            break;
        case 1:
            offset->angle = -0x1a00;
            break;
        case 2:
            offset->angle = 0x1a00;
            break;
        case 3:
            offset->angle = 0x4d00;
            break;
        case 4:
            offset->angle = -0x8000;
            break;
        }
    }
    if (offset->Mark) {
        work = SearchObjectWorkActnoLocalid(offset, 0x3b, 7, (offset->Localid + 2) % 5);
        if (work) {
            SetActNoNext(work, 8);
        }
    }
    SetJumpControl(offset);
}

void CHITAct_Moji(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    EndtoEliminate(offset);
}

void CHITAct_d(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        switch (offset->Localid) {
        case 0:
            SetXSpeed(offset, G_Sin(0x8000) * -1000.0f);
            SetYSpeed(offset, G_Cos(0x8000) * -1000.0f, 0);
            break;
        case 1:
            SetXSpeed(offset, G_Sin(0xb300) * -1000.0f);
            SetYSpeed(offset, G_Cos(0xb300) * -1000.0f, 0);
            break;
        case 2:
            SetXSpeed(offset, G_Sin(0xe600) * -1000.0f);
            SetYSpeed(offset, G_Cos(0xe600) * -1000.0f, 0);
            break;
        case 3:
            SetXSpeed(offset, G_Sin(0x1900) * -1000.0f);
            SetYSpeed(offset, G_Cos(0x1900) * -1000.0f, 0);
            break;
        case 4:
            SetXSpeed(offset, G_Sin(0x4c00) * -1000.0f);
            SetYSpeed(offset, G_Cos(0x4c00) * -1000.0f, 0);
            break;
        }
    }
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void CHSRAct_1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *parent = offset->parentWork;

    SetChangeFrameAnime(offset, offset->actno);
    if (parent->actno == 0xb1) {
        SetParentPosEx(offset);
    }
    EndtoEliminate(offset);
}

void CHLFAct_0(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void CHLFAct_Circle(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void CHLFAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->posy = 0;
    offset->SpriteFlag |= 8;
}

void CHLFAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    offset->posy = 0;
    EndtoEliminate(offset);
}

void CHKHAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    LandtoEliminate(offset);
    EndtoEliminate(offset);
}

void CHTUAct_1(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    SetLandBreakActno(offset, 2);
}

void CHTUAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}
