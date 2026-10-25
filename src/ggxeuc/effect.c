#include "generated/effect.h"

void FileRead(int fid, void *adr);
int GetFileSize(int fid);
void *mmalloc(int size, char *name);
extern char *EffectCmnFile;

void EffCmnLoad(void)
{
    if (EffectCmnFile == 0) {
        EffectCmnFile = mmalloc(GetFileSize(0x9D), "EffectCmnFile");
    }
    FileRead(0x9D, EffectCmnFile);
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void ExcuteActJumpTbByIdno(CHARACTER_WORK *offset);

void EffectMain(CHARACTER_WORK *offset)
{
    if (offset->actnonext != -1) {
        SetActNo(offset, offset->actnonext);
        offset->actnonext = -1;
    }
    if (offset->HitStopTime == 0 && offset->LandBreakActno != -1 && (~offset->actst & 0x10)) {
        SetActNo(offset, offset->LandBreakActno);
    }
    ExcuteActJumpTbByIdno(offset);
    if (offset->HitStopTime == 0) {
        offset->ActTimer++;
    }
}

void EffectColliCheck(CHARACTER_WORK *offset) {
}

void JumpControlMain(CHARACTER_WORK *offset);

void EffectMove(CHARACTER_WORK *offset)
{
    offset->posx += offset->xspeed;
    if (offset->actst & 0x200000) {
        JumpControlMain(offset);
    }
}

void AXCHCalc(CHARACTER_WORK *offset, CHARACTER_WORK *next);
void AXCHCalcHook(CHARACTER_WORK *offset, CHARACTER_WORK *next);
void EffectColliCheck(CHARACTER_WORK *offset);
void EffectMain(CHARACTER_WORK *offset);
void EffectMove(CHARACTER_WORK *offset);
void ExeAnime(CHARACTER_WORK *offset);
void ObjectKill(CHARACTER_WORK *offset);
void ParentExe(CHARACTER_WORK *offset);
void ParentExe_Position(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
extern CHARACTER_WORK *PlayerWork;
extern CHARACTER_WORK *EffectWork;
extern int EffectLiveCount;
extern int KunsyouLiveCount;

void EffectExecute(void)
{
    CHARACTER_WORK *offset;
    int i;
    int AXFlag;
    CHARACTER_WORK *parent;

    AXFlag = 0;
loop:
    for (i = 0; i < 160; i++) {
        offset = &EffectWork[i];
        if (offset && (offset->actst & 1) && (~offset->actst & 0x40000000)) {
            ParentExe(offset);
            EffectColliCheck(offset);
            EffectMain(offset);
            if (offset->HitStopTime == 0) {
                EffectMove(offset);
            }
            ParentExe_Position(offset);
            offset->actst |= 0x40000000;
            if (offset->actst & 0x80000000) {
                ObjectKill(offset);
            } else if (offset->InstTb) {
                ExeAnime(offset);
            }
        }
    }
    if (!AXFlag) {
        AXFlag = 1;
        for (i = 0; i < 2; i++) {
            offset = SearchCharacterExecute(&PlayerWork[i], 0x4D, 0, 0x86);
            if (offset) {
                parent = offset;
                while ((offset = SearchCharacterExecute(offset, 0x4D, 0, 0x86))) {
                    AXCHCalc(parent, offset);
                    parent = offset;
                }
                offset = SearchCharacterExecute(parent, 0x4D, 0, 0x83);
                if (offset) {
                    AXCHCalc(parent, offset);
                    AXCHCalcHook(parent, offset);
                }
            }
        }
    }
    offset = EffectWork;
    for (i = 0; i < 160; i++) {
        offset = &EffectWork[i];
        if (offset && (offset->actst & 1) && (~offset->actst & 0x40000000)) {
            goto loop;
        }
    }
    EffectLiveCount = 0;
    KunsyouLiveCount = 0;
    for (i = 0; i < 160; i++) {
        offset = &EffectWork[i];
        if (offset && (offset->actst & 1)) {
            EffectLiveCount++;
            if (offset->idno == 0x62) {
                KunsyouLiveCount++;
            }
            offset->actst &= ~0x40000000;
        }
    }
}

CHARACTER_WORK *CharacterWork_Initial(unsigned int InitId, short unsigned int idno);
unsigned int *GetInstTbAddr(short int side, short unsigned int idno);
void ParentFlagSet(CHARACTER_WORK *poffset, CHARACTER_WORK *offset, TINITINSTANCE *ins);
CHARACTER_WORK *SearchEffectWork(CHARACTER_WORK *off, short unsigned int idno);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetAnimeNo(CHARACTER_WORK *offset, int no);

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg)
{
    CHARACTER_WORK *offset;
    int flag;

    flag = 0;
    if (arg) {
        flag = arg->flag;
    }
    if (flag & 8) {
        offset = SearchEffectWork(poffset, idno);
        if (offset) {
            return offset;
        }
    }
    offset = CharacterWork_Initial(0x30, idno);
    if (offset == 0) {
        return 0;
    }
    offset->parentWork = poffset;
    offset->actst |= 8;
    if (poffset) {
        if (flag & 0x80) {
            offset->dirflag = !poffset->dirflag;
        } else {
            offset->dirflag = poffset->dirflag;
        }
    } else {
        offset->dirflag = 0;
    }
    if (flag & 1) {
        offset->posx = arg->x;
        offset->posy = arg->y;
    } else if (flag & 2) {
        if (offset->dirflag == 1) {
            offset->posx = poffset->posx + arg->x;
        } else {
            offset->posx = poffset->posx - arg->x;
        }
        offset->posy = poffset->posy + arg->y;
    } else if (poffset) {
        offset->posx = poffset->posx;
        offset->posy = poffset->posy;
    }
    if (flag & 4) {
        SetActNo(offset, arg->actno);
    }
    if (flag & 0x40) {
        offset->HitBackx = arg->tx;
        offset->RHitBackx = arg->ty;
    }
    if (flag & 0x20) {
        offset->Localid = arg->Localid;
    }
    if (flag & 0x100) {
        TINITINSTANCE ins;
        ins.Kind = 0x20;
        ins.Flag = 0;
        ParentFlagSet(poffset, offset, &ins);
    }
    if (flag & 0x200) {
        TINITINSTANCE ins;
        ins.Kind = 0x40;
        ins.Flag = 0;
        ParentFlagSet(poffset, offset, &ins);
    }
    if (flag & 0x400) {
        TINITINSTANCE ins;
        ins.Kind = 2;
        ins.Flag = 0;
        ParentFlagSet(poffset, offset, &ins);
    }
    if (flag & 0x1000) {
        offset->angle = arg->angle;
    }
    offset->priority += -1.1;
    offset->InstTb = GetInstTbAddr(offset->padid, offset->idno);
    SetAnimeNo(offset, 0);
    offset->InstSt |= 0x20;
    if (poffset) {
        offset->padid = poffset->padid;
        offset->actst |= poffset->actst & 6;
    }
    if (flag & 0x800) {
        offset->parentWork = poffset->parentWork;
    }
    return offset;
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno)
{
    CREATE_ARG c_arg;

    if (arg == 0) {
        c_arg.flag = 0;
        arg = &c_arg;
    }
    arg->actno = actno;
    arg->flag |= 4;
    return EffectCreate(poffset, idno, arg);
}

void ObjectKill(CHARACTER_WORK *offset);
extern CHARACTER_WORK *EffectWork;

void effect_killall(void)
{
    CHARACTER_WORK *offset;
    int i;

    offset = EffectWork;
    for (i = 0; i < 160; i++, offset++) {
        if ((offset->idno < 0x80 || offset->idno > 0x83) && (offset->actst & 1)) {
            ObjectKill(offset);
        }
    }
}
