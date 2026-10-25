#include "generated/cp_obj.h"

void CpObjMove(CHARACTER_WORK *offset) {
    offset->posx += offset->xspeed;
}

extern CHARACTER_WORK *CockpitWork;
void ObjectKill(CHARACTER_WORK *offset);

void objkill_winmrk(void) {
    int i;
    CHARACTER_WORK *p;

    p = CockpitWork;
    for (i = 0; i < 48; p++, i++) {
        if (p->idno == 0x7E) {
            ObjectKill(p);
        }
    }
}

extern void (*WINMRK_Act_jumpTb[])();
extern void (*SLASH_Act_jumpTb[])();
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

void CpObjMain(CHARACTER_WORK *offset) {
    if (offset->actnonext != 0xFFFFFFFF) {
        SetActNo(offset, offset->actnonext);
        offset->actnonext = 0xFFFFFFFF;
    }
    switch (offset->idno) {
    case 0x7E:
        WINMRK_Act_jumpTb[offset->actno](offset);
        break;
    case 0x7F:
        SLASH_Act_jumpTb[offset->actno](offset);
        break;
    }
    offset->ActTimer++;
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
CHARACTER_WORK *CharacterWork_Initial(unsigned int InitId, short unsigned int idno);
unsigned int *GetInstTbAddr(short int side, short unsigned int idno);

CHARACTER_WORK *CpObjCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg) {
    CHARACTER_WORK *offset;
    int flag;

    flag = 0;
    if (arg) {
        flag = arg->flag;
    }
    offset = CharacterWork_Initial(0x40, idno);
    if (offset == 0) {
        return 0;
    }
    offset->parentWork = poffset;
    offset->actst |= 8;
    if (flag & 0x20) {
        offset->Localid = arg->Localid;
    }
    if (flag & 4) {
        SetActNo(offset, arg->actno);
    }
    offset->priority += -1.1;
    offset->InstTb = GetInstTbAddr(offset->padid, offset->idno);
    SetAnimeNo(offset, 0);
    offset->InstSt |= 0x20;
    switch (idno) {
    case 0x7E:
        break;
    case 0x7F:
        offset->trans = 0;
        break;
    }
    return offset;
}

void ObjectKill(CHARACTER_WORK *offset);
void ExeAnime(CHARACTER_WORK *offset);

void CpObjExecute(void) {
    CHARACTER_WORK *offset;
    int i;

retry:
    for (i = 0; i < 16; i++) {
        offset = &CockpitWork[i];
        if (!(offset->actst & 1) || !(~offset->actst & 0x40000000)) {
            continue;
        }
        CpObjMain(offset);
        CpObjMove(offset);
        offset->actst |= 0x40000000;
        if (offset->actst & 0x80000000) {
            ObjectKill(offset);
        } else if (offset->InstTb) {
            ExeAnime(offset);
        }
    }
    for (i = 0; i < 16; i++) {
        offset = &CockpitWork[i];
        if ((offset->actst & 1) && (~offset->actst & 0x40000000)) {
            goto retry;
        }
    }
    for (i = 0; i < 16; i++) {
        offset = &CockpitWork[i];
        if (offset->actst & 1) {
            offset->actst &= ~0x40000000;
        }
    }
}
