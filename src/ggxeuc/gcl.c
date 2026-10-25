#include "generated/gcl.h"

void debugf(char *fmt, ...);

void GetGCLCoreXY(int *addr, RECT *ret, int id)
{
    int cnt;

    cnt = *addr++;
    addr += cnt * 3;
    if (addr == (int *)3) {
        debugf("GetGCLCoreXY Danger(%x) %x %d\n", id, addr, cnt);
        return;
    }
    ret->x = ((short *)addr)[0];
    ret->y = ((short *)addr)[1];
}

void ExCollisionClear(CHARACTER_WORK *offset)
{
    if (offset->ExColliAddr) {
        ((int *)offset->ExColliAddr)[-1] = 0;
        offset->ExColliAddr = 0;
    }
}

COLLISION *GetCollisionTop(CHARACTER_WORK *offset)
{
    int *addr;

    addr = (int *)offset->GclTop[offset->CellNo];
    offset->ColliCnt = *(unsigned char *)addr;
    addr++;
    offset->ColliAddr = (COLLISION *)addr;
    return (COLLISION *)addr;
}

void InitExCollisionWork(void)
{
    int i;
    int *lp;

    lp = ExCollisionWork;
    for (i = 0; i < 8; i++) {
        *lp = 0;
        lp += 4;
    }
}

COLLISION *GetExCollisionWork(void)
{
    int i;
    int *lp;

    lp = ExCollisionWork;
    for (i = 0; i < 8; i++) {
        if (*lp == 0) {
            *lp = 1;
            return (COLLISION *)(lp + 1);
        }
        lp += 4;
    }
    return 0;
}

char Set_GCLVRect(CHARACTER_WORK *offset)
{
    int *addr;

    addr = (int *)offset->GclTop[offset->CellNo];
    offset->ColliCnt = *addr;
    addr++;
    offset->ColliNo = -1;
    offset->ColliAddr = (COLLISION *)addr;
    return Set_GCLVRectNext(offset);
}

COLLISION *Get_GCLColli(CHARACTER_WORK *offset, unsigned int flag)
{
    int *addr;
    int i;
    COLLISION *Addr;

    addr = (int *)offset->GclTop[offset->CellNo];
    offset->ColliCnt = *addr;
    Addr = (COLLISION *)(addr + 1);
    for (i = 0; i < offset->ColliCnt; i++) {
        if (Addr[i].attr == flag) {
            return &Addr[i];
        }
    }
    return 0;
}

COLLISION *Get_GCLEx(CHARACTER_WORK *offset)
{
    int *addr;
    int i;
    COLLISION *Addr;

    addr = (int *)offset->GclTop[offset->CellNo];
    offset->ColliCnt = *addr;
    Addr = (COLLISION *)(addr + 1);
    for (i = 0; i < offset->ColliCnt; i++) {
        if (Addr[i].attr == 5) {
            return &Addr[i];
        }
    }
    return 0;
}

COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no)
{
    int *addr;
    int i;
    int cnt;
    COLLISION *Addr;

    cnt = no;
    addr = (int *)offset->GclTop[offset->CellNo];
    offset->ColliCnt = *addr;
    Addr = (COLLISION *)(addr + 1);
    for (i = 0; i < offset->ColliCnt; i++) {
        if (Addr[i].attr == 5) {
            if (cnt-- <= 0) {
                return &Addr[i];
            }
        }
    }
    return 0;
}

void Set_GCL(CHARACTER_WORK *offset)
{
    RECT rect;

    if (offset == 0) {
        debugf("Set_GCL Danger\n");
        return;
    }
    if (offset->GclTop == 0) {
        debugf("Set_GCL Danger2\n");
        return;
    }
    GetGCLCoreXY((int *)offset->GclTop[offset->CellNo], &rect, offset->idno);
    offset->corex = rect.x - 0x80;
    offset->corey = rect.y + 0x80;
}

char Set_GCLVRectNext(CHARACTER_WORK *offset)
{
    unsigned int attr;

    for (offset->ColliNo++; offset->ColliNo < offset->ColliCnt; offset->ColliNo++) {
        attr = offset->ColliAddr[offset->ColliNo].attr & 0xFF;
        if (attr == 3 || attr == 6) {
            return offset->ColliNo;
        }
    }
    offset->ColliNo = -1;
    return -1;
}

void SetImageNo(CHARACTER_WORK *offset)
{
    int *addr;
    int i;

    if (offset == 0) {
        debugf("SetImageNo : NULL\n");
        return;
    }
    addr = (int *)offset->GclTop[offset->CellNo];
    if (addr == (int *)3) {
        debugf("SetImageNo Danger(%x): %x %d\n", offset->idno, offset->GclTop, offset->CellNo);
        return;
    }
    i = *(unsigned char *)addr;
    addr++;
    addr += i * 3;
    offset->ImageNo = ((COLLISION *)addr)->attr;
}

int Random(void);

COLLISION *Get_GCLExRand(CHARACTER_WORK *offset)
{
    int *addr;
    int i;
    int Cnt;
    char tb[16];
    COLLISION *Addr;

    Cnt = 0;
    addr = (int *)offset->GclTop[offset->CellNo];
    offset->ColliCnt = *addr;
    Addr = (COLLISION *)(addr + 1);
    for (i = 0; i < offset->ColliCnt; i++) {
        if (Addr[i].attr == 5) {
            tb[Cnt++] = i;
            if (Cnt == 16) {
                break;
            }
        }
    }
    if (Cnt == 0) {
        return 0;
    }
    return &Addr[tb[Random() % Cnt]];
}

void SetExCollision(CHARACTER_WORK *offset, short x, short y, unsigned short w, unsigned short h, unsigned int attr)
{
    COLLISION *p;

    if (offset->ExColliAddr) {
        ExCollisionClear(offset);
    }
    p = GetExCollisionWork();
    offset->ExColliAddr = p;
    if (p) {
        p->x = x;
        p->y = y;
        p->w = w;
        p->h = h;
        p->attr = attr;
    }
}
