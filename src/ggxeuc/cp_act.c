#include "generated/cp_act.h"

void WinMrk_C(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 5);
}

void WinMrk_D(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 3);
}

void WinMrk_P(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 2);
}

void WinMrk_R(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 7);
}

void WinMrk_S(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 6);
}

void WinMrk_T(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 4);
}

void WinMrk_V(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 1);
}

void WinMrk_ST(CHARACTER_WORK *offset) {
    offset->SpriteFlag |= 1;
    GetWinMrkPosition(offset);
    SetChangeFrameAnime(offset, 0);
    if (offset->InstSt & 0x48) {
        SetActNoNext(offset, 1);
    }
}

extern USER_SETTING userinfo;

void GetWinMrkPosition(CHARACTER_WORK *offset) {
    int offu;
    int n;
    int side;

    offu = userinfo.offs_cockpit_upper;
    if (offset->Localid < 0) {
        n = -offset->Localid;
        side = 1;
    } else {
        n = offset->Localid;
        side = 0;
    }
    if (side) {
        offset->posx = (n * 32 + 0x150) * 100;
    } else {
        offset->posx = (0x130 - n * 32) * 100;
    }
    offset->posy = (offu + 0x44) * 100;
}

void GraphicExecute(CHARACTER_WORK *offset);

void SlashDraw(CHARACTER_WORK *offset) {
    int x;

    GraphicExecute(offset);
    x = offset->posx;
    offset->posx = offset->HitBackx;
    if (offset->ActTimer < 9) {
        GraphicExecute(offset);
    }
    offset->posx = x;
}

int sound_request(int no);
int EndtoEliminate(CHARACTER_WORK *offset);

void Slash_Act1(CHARACTER_WORK *offset) {
    offset->graphicfunc = SlashDraw;
    offset->SpriteFlag |= 1;
    if (SetChangeFrameAnime(offset, 0)) {
        offset->posx = -0x639C;
        offset->posy = 0x5DC0;
        offset->trans = 0x37;
        offset->scale = 3000;
        offset->actst |= 8;
        offset->HitBackx = 0x15D9C;
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        sound_request(0x4C);
        offset->CommonPalette = offset->CellTop[offset->ImageNo];
    } else if (offset->ActTimer < 9) {
        offset->trans += 0x19;
        offset->scale -= 0xFA;
        offset->posx += 0x1C13;
        offset->HitBackx -= 0x1C13;
    } else if (offset->ActTimer >= 0x46) {
        offset->actst |= 0x80000000;
    } else {
        offset->trans = 0xFF;
        offset->scale = 1000;
        offset->posx = 0x7D00;
    }
    offset->priority = 2;
    EndtoEliminate(offset);
}
