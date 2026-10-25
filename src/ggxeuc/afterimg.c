#include "generated/afterimg.h"

static AFTERIMAGE_WORK afimg_work[2][16];
static AFTERIMAGE_WORK afimg_stack;

void PushAfterImage(CHARACTER_WORK *offset) {
    AFTERIMAGE_WORK *p;

    p = &afimg_stack;
    p->posx = offset->posx;
    p->posy = offset->posy;
    p->corex = offset->corex;
    p->corey = offset->corey;
    p->Imagew = offset->Imagew;
    p->Imageh = offset->Imageh;
    p->SpriteId = offset->SpriteId;
    p->dirflag = offset->dirflag;
    p->SphAddr = offset->SphAddr;
    p->trans = offset->trans;
    p->priority = offset->priority;
    p->CellNo = offset->CellNo;
    p->scale = offset->scale;
}

int SetAfterImage(CHARACTER_WORK *offset, int index) {
    AFTERIMAGE_WORK *p;

    p = &afimg_work[offset->padid][(offset->ply->AfterImageIndex - index) & 0xF];
    if (p->SpriteId == -1) {
        return 0;
    }
    offset->posx = p->posx;
    offset->posy = p->posy;
    offset->corex = p->corex;
    offset->corey = p->corey;
    offset->Imagew = p->Imagew;
    offset->Imageh = p->Imageh;
    offset->SpriteId = p->SpriteId;
    offset->dirflag = p->dirflag;
    offset->SphAddr = p->SphAddr;
    offset->CellNo = p->CellNo;
    return 1;
}

void PopAfterImage(CHARACTER_WORK *offset) {
    AFTERIMAGE_WORK *p;

    p = &afimg_stack;
    offset->posx = p->posx;
    offset->posy = p->posy;
    offset->corex = p->corex;
    offset->corey = p->corey;
    offset->Imagew = p->Imagew;
    offset->Imageh = p->Imageh;
    offset->SpriteId = p->SpriteId;
    offset->dirflag = p->dirflag;
    offset->SphAddr = p->SphAddr;
    offset->trans = p->trans;
    offset->priority = p->priority;
    offset->CellNo = p->CellNo;
    offset->scale = p->scale;
}

void InitAfterImage(void) {
    AFTERIMAGE_WORK *p;
    int i;

    p = afimg_work[0];
    i = 32;
    while (i--) {
        p->SpriteId = -1;
        p++;
    }
}

void SaveAfterImage(CHARACTER_WORK *offset) {
    AFTERIMAGE_WORK *p;
    int i;
    int flag;
    int index;

    flag = 0;
    offset->ply->AfterImageIndex++;
    offset->ply->AfterImageIndex &= 0xF;
    p = &afimg_work[offset->padid][offset->ply->AfterImageIndex];
    p->posx = offset->posx;
    p->posy = offset->posy;
    p->corex = offset->corex;
    p->corey = offset->corey;
    p->Imagew = offset->Imagew;
    p->Imageh = offset->Imageh;
    p->SpriteId = offset->SpriteId;
    p->dirflag = offset->dirflag;
    p->SphAddr = offset->SphAddr;
    p->CellNo = offset->CellNo;
    index = offset->ply->AfterImageIndex - 1;
    for (i = 14; i != -1; index--, i--) {
        index &= 0xF;
        p = &afimg_work[offset->padid][index];
        if (p->SpriteId == offset->SpriteId) {
            if (flag) {
                p->SpriteId = -1;
            }
        } else {
            flag = 1;
        }
    }
}
