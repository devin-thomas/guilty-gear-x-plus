#include "generated/actsub2.h"

int IsActEnd(CHARACTER_WORK *offset)
{
    return (offset->InstSt & 0xC8) != 0;
}

int IsLand(CHARACTER_WORK *offset)
{
    if (!(offset->actst & 0x10) && offset->yspeed > 0) {
        return 1;
    }
    return 0;
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no)
{
    offset->actst |= 0x200000;
    if (offset->ply && (offset->ply->JumpFlag & 4)) {
        switch (offset->idno) {
        case 2:
            offset->LandBreakActno = 0x8E;
            break;
        case 7:
            offset->LandBreakActno = 0xAF;
            break;
        case 9:
            offset->LandBreakActno = 0x8F;
            break;
        default:
            offset->LandBreakActno = no;
            break;
        }
    } else {
        offset->LandBreakActno = no;
    }
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void SetJumpControl(CHARACTER_WORK *offset)
{
    SetLandBreakActno(offset, 0xFFFFFFFF);
}

int IsActTimeOver(CHARACTER_WORK *offset, int time)
{
    if (offset->ActTimer >= time) return offset->ActTimer - time + 1;
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int EndtoStand(CHARACTER_WORK *offset)
{
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int EndtoSit(CHARACTER_WORK *offset)
{
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 1);
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed)
{
    if (offset->yspeed >= yspeed) {
        SetActNoNext(offset, actno);
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int EndtoDrop(CHARACTER_WORK *offset)
{
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 8);
        return 1;
    }
    return 0;
}

int EndtoDemo(CHARACTER_WORK *offset)
{
    if (offset->InstSt & 0xC8) {
        offset->actst |= 0x40000;
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int EndtoAction(CHARACTER_WORK *offset, int actno)
{
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, actno);
        return 1;
    }
    return 0;
}

int EndtoEliminate(CHARACTER_WORK *offset)
{
    if (offset->InstSt & 0x8) {
        offset->actst |= 0x80000000;
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int LandtoAction(CHARACTER_WORK *offset, int actno)
{
    if (!(offset->actst & 0x10)) {
        SetActNoNext(offset, actno);
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int TimetoAction(CHARACTER_WORK *offset, int actno, int time)
{
    if (offset->ActTimer >= time) {
        SetActNoNext(offset, actno);
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int ParentDamagetoAction(CHARACTER_WORK *offset, int actno)
{
    if (offset->parentFlag & 0x40) {
        SetActNoNext(offset, actno);
        return 1;
    }
    return 0;
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int ParentActChangetoAction(CHARACTER_WORK *offset, int actno)
{
    if (offset->parentFlag & 0x20) {
        SetActNoNext(offset, actno);
        return 1;
    }
    return 0;
}

int TimetoEliminate(CHARACTER_WORK *offset, int time)
{
    if (offset->ActTimer >= time) {
        offset->actst |= 0x80000000;
        return 1;
    }
    return 0;
}

extern short int PDDef_Gravity[];

void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav)
{
    offset->yspeed = yspeed;
    if (grav == -1) {
        offset->gravity = PDDef_Gravity[offset->padid];
    } else {
        offset->gravity = grav;
    }
}

int IsMaxHit(CHARACTER_WORK *offset)
{
    return offset->ActHitCount >= offset->MaxHitCount;
}

void SetNoAttack(CHARACTER_WORK *offset)
{
    offset->actst |= 0x40;
}

int IsDead(CHARACTER_WORK *offset)
{
    return offset->HitPoint == 0;
}

void SetBgObj(CHARACTER_WORK *offset)
{
    offset->SpriteFlag |= 0x800;
}

void ParentFlagSuccession(CHARACTER_WORK *poffset, CHARACTER_WORK *offset)
{
    offset->parentWork = poffset->parentWork;
    offset->parentFlag = poffset->parentFlag;
}

int IsLand(CHARACTER_WORK *offset);

int LandtoEliminate(CHARACTER_WORK *offset)
{
    if (IsLand(offset)) {
        offset->actst |= 0x80000000;
        return 1;
    }
    return 0;
}

void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag)
{
    if (((CHARACTER_WORK *)offset->parentWork)->ply) {
        ((CHARACTER_WORK *)offset->parentWork)->ply->ObjFlag |= flag;
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
extern int StageNo;

void RqJmpEff(CHARACTER_WORK *offset)
{
    if (!(offset->ply->JumpDir & 0x10)) {
        if (StageNo == 3 && offset->posx < -22599) {
            EffectCreate(offset, 0x81, 0);
        } else {
            EffectCreate(offset, 0x61, 0);
        }
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
extern int StageNo;

void RqLndEff(CHARACTER_WORK *offset)
{
    if (StageNo == 3 && offset->posx < -22599) {
        EffectCreate_ActNo(offset, 0x81, 0, 1);
        EffectCreate_ActNo(offset, 0x81, 0, 2);
    } else {
        EffectCreate_ActNo(offset, 0x61, 0, 2);
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);

void RqDmgEff(CHARACTER_WORK *offset)
{
    if (offset->ActTimer & 1) {
        if (offset->ActTimer & 2) {
            EffectCreate_ActNo(offset, 0x61, 0, 5);
        } else {
            EffectCreate_ActNo(offset, 0x61, 0, 4);
        }
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
extern int StageNo;

void RqHamon(CHARACTER_WORK *offset)
{
    if (StageNo == 3 && offset->posx < -22599 && !(offset->actst & 0x10) && offset->posy == 0) {
        EffectCreate_ActNo(offset, 0x81, 0, 1);
        EffectCreate_ActNo(offset, 0x81, 0, 2);
    }
}

int IsHitStop(CHARACTER_WORK *offset)
{
    return offset->HitStopTime != 0;
}

int IsActTimeOver(CHARACTER_WORK *offset, int time);

void SetRespectCommnadFlag(CHARACTER_WORK *offset)
{
    offset->actst &= ~0x1000;
    if (IsActTimeOver(offset, 0x10)) {
        offset->CommandFlag = 0xC01F;
    } else {
        offset->CommandFlag = 0xC05F;
    }
}

int IsActTimeOver(CHARACTER_WORK *offset, int time);

void SetExciteCommnadFlag(CHARACTER_WORK *offset)
{
    offset->actst &= ~0x1000;
    if (IsActTimeOver(offset, 0x3C)) {
        offset->CommandFlag = 0xC01F;
    } else {
        offset->CommandFlag = 0xC05F;
    }
}

void SetStand(CHARACTER_WORK *offset)
{
    offset->actst &= ~0x400;
}

void SetSit(CHARACTER_WORK *offset)
{
    offset->actst &= ~0x400;
}

extern CHARACTER_WORK *PlayerWork;

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset)
{
    return offset->padid ? PlayerWork : PlayerWork + 1;
}

extern CHARACTER_WORK *PlayerWork;

CHARACTER_WORK *GetFriendPlayer(CHARACTER_WORK *offset)
{
    return PlayerWork + offset->padid;
}

int nlArcTan2(int x, int y);

short unsigned int GetAngle_xy(CHARACTER_WORK *offset, int x, int y)
{
    int ang = nlArcTan2(x, y);

    if (offset->dirflag == 1) ang = -ang;
    return ang;
}

extern GAME_INFOMATION g_info;

void SetAnten(CHARACTER_WORK *offset, int flag)
{
    if (flag == 2) {
        g_info.BattleControlFlag |= 0x100;
    } else if (flag == 1) {
        g_info.BattleControlFlag |= 0x1;
    } else {
        g_info.BattleControlFlag |= 0x10;
    }
}

void PlayerPoisonClear(CHARACTER_WORK *offset)
{
    offset->ply->Poison = 0;
}

void FaintClear(CHARACTER_WORK *offset);

void SetWinActFlag(CHARACTER_WORK *offset)
{
    offset->actst |= 0x80;
    offset->SpriteFlag &= ~0x3000;
    FaintClear(offset);
}
