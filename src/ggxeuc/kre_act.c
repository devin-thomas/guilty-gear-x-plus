#include "generated/kre_act.h"

int BAKAMON_Check(short unsigned int actno) {
    switch (actno) {
    case 12:
    case 13:
    case 27:
    case 28:
        return 0;
    case 15:
    case 16:
    case 24:
    case 25:
    case 30:
    case 31:
    case 39:
    case 40:
        return 1;
    case 18:
    case 19:
    case 33:
    case 34:
        return 2;
    case 21:
    case 22:
    case 36:
    case 37:
        return 3;
    }
    return -1;
}

int BAKAMON_Check(short unsigned int actno);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);

void KREF_ColliCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    CREATE_ARG c_arg;

    poffset = GetParentPlayer(offset);
    if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
        switch (BAKAMON_Check(offset->actno)) {
        case 0:
            offset->Localid = 0;
            break;
        case 1:
            offset->Localid = 0;
            break;
        case 2:
            offset->Localid = 0;
            break;
        case 3:
            offset->Localid = 0;
            break;
        }
    }
    switch (offset->actno) {
    case 12:
    case 15:
    case 18:
    case 21:
    case 24:
    case 27:
    case 30:
    case 33:
    case 36:
    case 39:
        if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
            c_arg.flag = 0x801;
            c_arg.x = offset->posx;
            c_arg.y = offset->posy;
            offset->actst |= 0x80000000;
            ObjectCreate_ActNo(poffset, 0x57, &c_arg, offset->actno + 2);
        }
        break;
    case 13:
    case 16:
    case 19:
    case 22:
    case 25:
    case 28:
    case 31:
    case 34:
    case 37:
    case 40:
        if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
            c_arg.flag = 0x801;
            c_arg.x = offset->posx;
            c_arg.y = offset->posy;
            offset->actst |= 0x80000000;
            ObjectCreate_ActNo(poffset, 0x57, &c_arg, offset->actno + 1);
        }
        break;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void KREFAct_KAKUSEI(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void KREFAct_ITIGEKI2(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->ActTimer >= 25 && !(offset->ActTimer & 7)) {
        c_arg.flag = 1;
        c_arg.x = offset->posx;
        c_arg.y = 0;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 10);
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 11);
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void KREFAct_ITIGEKI3(CHARACTER_WORK *offset) {
    CREATE_ARG c_arg;

    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->ActTimer < 24 && !(offset->ActTimer & 7)) {
        c_arg.flag = 1;
        c_arg.x = offset->posx;
        c_arg.y = 0;
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 10);
        ObjectCreate_ActNo(offset, 0x57, &c_arg, 11);
    }
    EndtoEliminate(offset);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_BAKAMON_Strat(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    SetJumpControl(offset);
    if (SetChangeFrameAnimeSameActno(offset)) {
        switch (BAKAMON_Check(offset->actno)) {
        case 0:
            offset->Localid = 1;
            poffset->ply->PersonalWork[1].ub[0] = 0xFF;
            break;
        case 1:
            offset->Localid = 1;
            break;
        case 2:
            offset->Localid = 1;
            break;
        case 3:
            offset->Localid = 1;
            break;
        }
    }
    SetLandBreakActno(offset, offset->actno + 1);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_BAKAMON_Jump(CHARACTER_WORK *offset) {
    int b_count;

    SetJumpControl(offset);
    b_count = -1;
    if (SetChangeFrameAnimeSameActno(offset)) {
        switch (BAKAMON_Check(offset->actno)) {
        case 0:
            offset->Localid++;
            break;
        case 1:
            offset->Localid++;
            break;
        case 2:
            offset->Localid++;
            break;
        case 3:
            offset->Localid++;
            break;
        }
    }
    switch (BAKAMON_Check(offset->actno)) {
    case 0:
        b_count = offset->Localid;
        break;
    case 1:
        b_count = offset->Localid;
        break;
    case 2:
        b_count = offset->Localid;
        break;
    case 3:
        b_count = offset->Localid;
        break;
    }
    if (b_count < 3) {
        SetLandBreakActno(offset, offset->actno);
    } else {
        offset->actst |= 0x100000;
        if (offset->posy > 20000) {
            switch (BAKAMON_Check(offset->actno)) {
            case 0:
                offset->Localid = 0;
                break;
            case 1:
                offset->Localid = 0;
                break;
            case 2:
                offset->Localid = 0;
                break;
            case 3:
                offset->Localid = 0;
                break;
            }
            offset->actst |= 0x80000000;
        }
        EndtoEliminate(offset);
    }
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_BAKAMON_Hit(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    offset->actst |= 0x100000;
    switch (BAKAMON_Check(offset->actno)) {
    case 0:
        offset->Localid = 0;
        break;
    case 1:
        offset->Localid = 0;
        break;
    case 2:
        offset->Localid = 0;
        break;
    case 3:
        offset->Localid = 0;
        break;
    }
    EndtoEliminate(offset);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_EntryA_1(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_EntryA_2(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->posy > -3201) {
        offset->actst |= 0x80000000;
    }
    EndtoEliminate(offset);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_EntryA_3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (poffset->Mark) {
        SetActNo(offset, 0x2A);
    }
    EndtoEliminate(offset);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_EntryA_4(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    SetJumpControl(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (poffset->Mark) {
        SetActNo(offset, offset->actno + 4);
    }
    EndtoEliminate(offset);
}

int BAKAMON_Check(short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void KREFAct_EntryA_5(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}
