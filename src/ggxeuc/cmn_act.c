#include "generated/cmn_act.h"
extern GAME_INFOMATION g_info;
extern int BattleStatus;
extern story_t story;
int Random(void);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void CmnAct_StdAtk_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);
void CmnAct_StdAtk_ToSit(CHARACTER_WORK *offset, short unsigned int Anime);
void CmnAct_StdAtk_Jump(CHARACTER_WORK *offset, short unsigned int Anime);
int IsDragonInstall(CHARACTER_WORK *offset);

void CmnAct_Stand(CHARACTER_WORK *offset) {
    CHARACTER_WORK *p;

    offset->yspeed = 0;
    offset->CommandFlag = 0x101;
    offset->actst = (offset->actst & ~0x1000) | 0x40000;
    if (SetChangeFrameAnime(offset, 0)) {
        SetXSpeed(offset, 0);
    }
    if (BattleStatus == 3) {
        return;
    }
    if (BattleStatus == 4) {
        return;
    }
    if ((g_info.mode & 0x2000) && story.is_enter) {
        return;
    }
    switch (offset->idno) {
    case 1:
    case 5:
    case 12:
    case 15:
        if (offset->InstSt & 0x80) {
            if (!(Random() & 7)) {
                SetActNoNext(offset, 0x8003D);
            }
        }
        break;
    case 13:
        if (offset->InstSt & 0x80) {
            if (!(Random() & 7)) {
                SetActNoNext(offset, 0x8003D);
            } else if (!(Random() & 7)) {
                SetActNoNext(offset, 0x80052);
            }
        }
        break;
    case 11:
        if (offset->InstSt & 0x80) {
            p = SearchObjectWork(offset, 0x52);
            if (p) {
                if (!(Random() & 7)) {
                    if (p->actno == 0 && offset->ply->PersonalWork[0].l == 0) {
                        SetActNoNext(offset, 0x80091);
                    }
                }
            }
        }
        break;
    }
}

void CmnAct_Sit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x108;
    offset->actst |= 0x400;
    if (offset->idno == 2 && (offset->actst & 0x1000)) {
        SetChangeFrameAnime(offset, 0x8D);
    } else {
        SetChangeFrameAnime(offset, 1);
    }
    offset->actst &= ~0x1000;
}

int EndtoSit(CHARACTER_WORK *offset);

void CmnAct_Stand2Sit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x108;
    offset->actst |= 0x1400;
    SetChangeFrameAnime(offset, 2);
    EndtoSit(offset);
}

int EndtoStand(CHARACTER_WORK *offset);

void CmnAct_Sit2Stand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x101;
    offset->actst |= 0x1000;
    SetChangeFrameAnime(offset, 2);
    EndtoStand(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);

void CmnAct_JumpReady(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *temp_v1;

    offset->CommandFlag = 0xD05F;
    temp_v1 = offset->ply;
    temp_v1->JumpFlag |= 2;
    SetChangeFrameAnime(offset, 3);
    EndtoAction(offset, offset->actnoSv | 0x30000);
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
int IsDragonInstall(CHARACTER_WORK *offset);
extern short int ForwardWalk_XSpeedByIdno[];  /* sdata 0x5cccf8 playdat.c */

void CmnAct_ForwardWalk(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x102;
    if (IsDragonInstall(offset) != 0) {
        SetXSpeed(offset, ForwardWalk_XSpeedByIdno[offset->padid] * 2);
    } else {
        SetXSpeed(offset, (int) ForwardWalk_XSpeedByIdno[offset->padid]);
    }
    Add_TensionVal_atMove(offset);
    SetChangeFrameAnime(offset, 5);
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);

void CmnAct_SitForwardWalk(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x4100;
    offset->actst = (offset->actst & 0xFFFFEFFF) | 0x400;
    Add_TensionVal_atMove(offset);
    SetChangeFrameAnime(offset, 0x61);
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
int IsDragonInstall(CHARACTER_WORK *offset);
extern short int BackWalk_XSpeedByIdno[];  /* sdata 0x5ccd00 playdat.c */

void CmnAct_BackWalk(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x104;
    if (IsDragonInstall(offset) != 0) {
        SetXSpeed(offset, BackWalk_XSpeedByIdno[offset->padid] * 2);
    } else {
        SetXSpeed(offset, (int) BackWalk_XSpeedByIdno[offset->padid]);
    }
    Add_TensionVal_atMove(offset);
    SetChangeFrameAnime(offset, 6);
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);

void CmnAct_SitBackWalk(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x8100;
    offset->actst = (offset->actst & 0xFFFFEFFF) | 0x400;
    Add_TensionVal_atMove(offset);
    SetChangeFrameAnime(offset, 0x62);
}

void DirectionDirectChange(CHARACTER_WORK *offset);
void ReqCmnSE_Jump(CHARACTER_WORK *offset);
void RqJmpEff(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetPlJumpSpeed(CHARACTER_WORK *offset);

void CmnAct_Jump_Vert(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC53F;
    if (SetChangeFrameAnime(offset, 7) != 0) {
        ReqCmnSE_Jump(offset);
        SetPlJumpSpeed(offset);
        RqJmpEff(offset);
    }
    SetJumpControl(offset);
    if (offset->ply->JumpFlag & 1) {
        DirectionDirectChange(offset);
    }
    if (offset->yspeed > 0) {
        SetActNoNext(offset, 0x30008);
    }
}

void DirectionDirectChange(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int g_roundmin(int val, int min);
extern short int Jump_GravityByIdno[];  /* sdata 0x5ccd40 playdat.c */

void CmnAct_Drop_Vert(CHARACTER_WORK *offset) {
    unsigned short temp_a1;

    offset->CommandFlag = 0xC43F;
    temp_a1 = (unsigned short) Jump_GravityByIdno[offset->padid];
    offset->gravity = g_roundmin(offset->gravity, (int) ((short) temp_a1 + ((unsigned int) (temp_a1 << 0x10) >> 0x1F)) >> 1);
    offset->CommandFlag |= 0x100;
    SetChangeFrameAnime(offset, 8);
    if (offset->ply->JumpFlag & 1) {
        DirectionDirectChange(offset);
    }
    SetLandBreakActno(offset, 0x200015);
}

void DirectionDirectChange(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void CmnAct_Drop_Forward(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC53F;
    SetChangeFrameAnime(offset, 0x16);
    if (offset->ply->JumpFlag & 1) {
        DirectionDirectChange(offset);
    }
    SetLandBreakActno(offset, 0x200015);
}

void DirectionDirectChange(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void CmnAct_Drop_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC53F;
    SetChangeFrameAnime(offset, 0x4B);
    if (offset->ply->JumpFlag & 1) {
        DirectionDirectChange(offset);
    }
    SetLandBreakActno(offset, 0x200015);
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
void DirectionDirectChange(CHARACTER_WORK *offset);
void ReqCmnSE_Jump(CHARACTER_WORK *offset);
void RqJmpEff(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetPlJumpSpeed(CHARACTER_WORK *offset);

void CmnAct_Jump_Forward(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC53F;
    if (SetChangeFrameAnime(offset, 9) != 0) {
        ReqCmnSE_Jump(offset);
        SetPlJumpSpeed(offset);
        RqJmpEff(offset);
    }
    Add_TensionVal_atMove(offset);
    SetJumpControl(offset);
    if (offset->ply->JumpFlag & 1) {
        DirectionDirectChange(offset);
    }
    if (offset->yspeed > 0) {
        switch (offset->idno) {
        case 3:
        case 4:
        case 12:
            SetActNoNext(offset, 0x30016);
            break;
        default:
            SetActNoNext(offset, 0x30008);
            break;
        }
    }
    if ((-offset->gravity * 6) < offset->yspeed) {
        if (offset->idno == 7) {
            SetActNoNext(offset, 0x30016);
        }
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
void DirectionDirectChange(CHARACTER_WORK *offset);
void ReqCmnSE_Jump(CHARACTER_WORK *offset);
void RqJmpEff(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
void SetPlJumpSpeed(CHARACTER_WORK *offset);

void CmnAct_Jump_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC53F;
    if (SetChangeFrameAnime(offset, 0xA) != 0) {
        ReqCmnSE_Jump(offset);
        SetPlJumpSpeed(offset);
        RqJmpEff(offset);
    }
    Add_TensionVal_atMove(offset);
    SetJumpControl(offset);
    if (offset->ply->JumpFlag & 1) {
        DirectionDirectChange(offset);
    }
    if (offset->yspeed > 0) {
        switch (offset->idno) {
        case 2:
        case 3:
        case 4:
        case 12:
            SetActNoNext(offset, 0x3004B);
            break;
        default:
            SetActNoNext(offset, 0x30008);
            break;
        }
    }
    if ((-offset->gravity * 6) < offset->yspeed) {
        if (offset->idno == 7) {
            SetActNoNext(offset, 0x3004B);
        }
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
void ReqCmnSE_Jump(CHARACTER_WORK *offset);
void RqJmpEff(CHARACTER_WORK *offset);
void SetPlJumpSpeed(CHARACTER_WORK *offset);

void CmnAct_Jump_Homing(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *temp_v1;

    offset->CommandFlag = 0xC43F;
    if (SetChangeFrameAnime(offset, 0xB) != 0) {
        ReqCmnSE_Jump(offset);
        RqJmpEff(offset);
        SetPlJumpSpeed(offset);
        offset->ply->homingtime = 0x1E;
    }
    Add_TensionVal_atMove(offset);
    JumpControl(offset);
    temp_v1 = offset->ply;
    if (temp_v1->homingtime < 0x10) {
        temp_v1->homingtime = 0x10;
    }
    if (!(offset->actst & 0x10)) {
        SetActNoNext(offset, 0x30008);
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
void ReqCmnSE_SkyDash(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
extern short int Jump_GravityByIdno[];  /* sdata 0x5ccd40 playdat.c */
extern short int PDSkyDashF_AttackTime[];  /* sdata 0x5ccde8 playdat.c */
extern short int PDSkyDashF_Time[];  /* sdata 0x5ccda0 playdat.c */
extern USER_SETTING userinfo;  /* data 0x55c0b8 dcextval.c */

void CmnAct_JumpDash_Forward(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *temp_v1;

    if (IsActTimeOver(offset, PDSkyDashF_AttackTime[offset->padid])) {
        offset->CommandFlag = 0xC01F;
    } else {
        offset->CommandFlag = 0xC05F;
    }
    if (SetChangeFrameAnime(offset, 0xC)) {
        ReqCmnSE_SkyDash(offset);
        offset->ply->maintain = PDSkyDashF_Time[offset->padid];
        if (!(userinfo.gguse[offset->padid] & 1)) {
            offset->ply->SkyDashCorrect = 3;
        } else {
            offset->ply->SkyDashCorrect = 0;
        }
        temp_v1 = offset->ply;
        temp_v1->JumpFlag |= 1;
        offset->ply->homingtime = 0;
    }
    offset->ply->JumpFlag |= 0x10;
    offset->gravity = (int) Jump_GravityByIdno[offset->padid];
    Add_TensionVal_atMove(offset);
    SetJumpControl(offset);
    if (TimetoAction(offset, 0x30008, 0x3C) != 0) {
        offset->ply->maintain = 0;
        offset->xspeed = (int) offset->xspeed / 2;
    }
    if (EndtoAction(offset, 0x30008) != 0) {
        offset->ply->maintain = 0;
        offset->xspeed = (int) offset->xspeed / 2;
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
void ReqCmnSE_SkyDash(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
extern short int Jump_GravityByIdno[];  /* sdata 0x5ccd40 playdat.c */
extern short int PDSkyDashB_AttackTime[];  /* sdata 0x5ccde8 playdat.c */
extern short int PDSkyDashB_Time[];  /* sdata 0x5ccda0 playdat.c */

void CmnAct_JumpDash_Back(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *temp_v1;

    if (IsActTimeOver(offset, PDSkyDashB_AttackTime[offset->padid])) {
        offset->CommandFlag = 0xC01F;
    } else {
        offset->CommandFlag = 0xC05F;
    }
    if (SetChangeFrameAnime(offset, 0xD)) {
        ReqCmnSE_SkyDash(offset);
        offset->ply->maintain = PDSkyDashB_Time[offset->padid];
        offset->ply->SkyDashCorrect = 4;
        temp_v1 = offset->ply;
        temp_v1->JumpFlag |= 1;
        offset->ply->homingtime = 0;
    }
    offset->ply->JumpFlag |= 0x10;
    offset->gravity = (int) Jump_GravityByIdno[offset->padid];
    Add_TensionVal_atMove(offset);
    SetJumpControl(offset);
    if (EndtoAction(offset, 0x30008) != 0) {
        offset->ply->maintain = 0;
        offset->xspeed = (int) offset->xspeed / 2;
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int IsDragonInstall(CHARACTER_WORK *offset);
void MaxXSpeed(CHARACTER_WORK *offset, short int max);
void ReqCmnSE_Dash(CHARACTER_WORK *offset);
void SetDashXSpeed(CHARACTER_WORK *offset, short int xspeed);
void SetInertia(CHARACTER_WORK *offset);
extern short int Dash_XSpeedByIdno[];  /* sdata 0x5ccd08 playdat.c */
extern PAD GameKey[];  /* data 0x565fe8 key.c */

void CmnAct_Dash(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xE00F;
    offset->actst |= 0x40;
    if (SetChangeFrameAnime(offset, 0xE) != 0) {
        ReqCmnSE_Dash(offset);
    }
    if (IsDragonInstall(offset) != 0) {
        SetDashXSpeed(offset, (short) (Dash_XSpeedByIdno[offset->padid] * 2));
    } else {
        SetDashXSpeed(offset, Dash_XSpeedByIdno[offset->padid]);
    }
    SetInertia(offset);
    Add_TensionVal_atMove(offset);
    if (offset->ply->EnemyTouch != 0) {
        MaxXSpeed(offset, 0x898);
    } else {
        MaxXSpeed(offset, 0xBB8);
    }
    if (offset->ActTimer >= 10 && !(GameKey[offset->padid].data & 0x2000)) {
        SetActNoNext(offset, 0x3000F);
    }
    if (!(offset->ActTimer & 7)) {
        EffectCreate_ActNo(offset, 0x61, 0, 1);
    }
}

void ReqCmnSE_Reduce(CHARACTER_WORK *offset);

void CmnAct_Reduce(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC00F;
    if (offset->ActTimer < 3) {
        offset->CommandFlag = 0xE00F;
    }
    if (SetChangeFrameAnime(offset, 0xF) != 0) {
        ReqCmnSE_Reduce(offset);
    }
    SetXSpeed(offset, 0);
    if (offset->ActTimer >= 0xD) {
        SetActNoNext(offset, 0);
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
void ReqCmnSE_Dash(CHARACTER_WORK *offset);
void SetDashXSpeed(CHARACTER_WORK *offset, short int xspeed);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
extern short int Dash_XSpeedByIdno[];  /* sdata 0x5ccd08 playdat.c */

void JYAct_Dash(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xE04F;
    offset->actst |= 0x40;
    if (SetChangeFrameAnime(offset, 0xE) != 0) {
        ReqCmnSE_Dash(offset);
    }
    SetDashXSpeed(offset, Dash_XSpeedByIdno[offset->padid]);
    SetLandBreakActno(offset, 0xF);
    Add_TensionVal_atMove(offset);
}

void ReqCmnSE_Reduce(CHARACTER_WORK *offset);

void JYAct_Reduce(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC00F;
    if (offset->ActTimer < 3) {
        offset->CommandFlag = 0xE00F;
    }
    if (SetChangeFrameAnime(offset, 0xF) != 0) {
        ReqCmnSE_Reduce(offset);
    }
    SetXSpeed(offset, 0);
    if (offset->ActTimer >= 0xA) {
        SetActNoNext(offset, 0);
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
void ReqCmnSE_Dash(CHARACTER_WORK *offset);
void SetDashXSpeed(CHARACTER_WORK *offset, short int xspeed);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
extern short int Dash_XSpeedByIdno[];  /* sdata 0x5ccd08 playdat.c */

void POAct_Dash(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xE04F;
    offset->actst |= 0x40;
    if (SetChangeFrameAnime(offset, 0xE) != 0) {
        ReqCmnSE_Dash(offset);
    }
    SetDashXSpeed(offset, Dash_XSpeedByIdno[offset->padid]);
    SetLandBreakActno(offset, 0xF);
    Add_TensionVal_atMove(offset);
}

void ReqCmnSE_Reduce(CHARACTER_WORK *offset);

void POAct_Reduce(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC00F;
    if (offset->ActTimer < 3) {
        offset->CommandFlag = 0xE00F;
    }
    if (SetChangeFrameAnime(offset, 0xF) != 0) {
        ReqCmnSE_Reduce(offset);
    }
    SetXSpeed(offset, 0);
    if (offset->ActTimer >= 0xA) {
        SetActNoNext(offset, 0);
    }
}

void Add_TensionVal_atMove(CHARACTER_WORK *offset);
int IsDragonInstall(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
void ReqCmnSE_JumpLand(CHARACTER_WORK *offset);
void RqLndEff(CHARACTER_WORK *offset);
extern short int BackStep_GravityByIdno[];
extern short int BackStep_XSpeedByIdno[];
extern short int BackStep_YSpeedByIdno[];

void CmnAct_BackStep(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (IsDragonInstall(offset)) {
        SetXSpeed(offset, BackStep_XSpeedByIdno[offset->padid] * 1.5);
    } else {
        SetXSpeed(offset, BackStep_XSpeedByIdno[offset->padid]);
    }
    offset->gravity = BackStep_GravityByIdno[offset->padid];
    if (SetChangeFrameAnime(offset, 0x10)) {
        offset->yspeed = BackStep_YSpeedByIdno[offset->padid];
    }
    Add_TensionVal_atMove(offset);
    JumpControl(offset);
    offset->ply->BackStepInvalid = 5;
    if (!(offset->actst & 0x10)) {
        RqLndEff(offset);
        ReqCmnSE_JumpLand(offset);
        SetActNoNext(offset, 0);
    }
}

void CmnAct_StandTurnLeft(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x109;
    SetChangeFrameAnime(offset, 0x11);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

void CmnAct_StandTurnRight(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x109;
    SetChangeFrameAnime(offset, 0x12);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

void CmnAct_SitTurnLeft(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x109;
    SetChangeFrameAnime(offset, 0x13);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 1);
    }
}

void CmnAct_SitTurnRight(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x109;
    SetChangeFrameAnime(offset, 0x14);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 1);
    }
}

int EndtoStand(CHARACTER_WORK *offset);
void ReqCmnSE_JumpLand(CHARACTER_WORK *offset);
void RqLndEff(CHARACTER_WORK *offset);

void CmnAct_JumpLand(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC11F;
    offset->actst |= 0x1000;
    offset->ply->inertia = 0;
    offset->xspeed = 0;
    offset->yspeed = 0;
    offset->actst &= 0xFFFFFFEF;
    if (SetChangeFrameAnime(offset, 0x15) != 0) {
        RqLndEff(offset);
        ReqCmnSE_JumpLand(offset);
    }
    EndtoStand(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
void ReqCmnSE_JumpLand(CHARACTER_WORK *offset);
void RqLndEff(CHARACTER_WORK *offset);
void SetSit(CHARACTER_WORK *offset);

void CmnAct_KYJumpLand(CHARACTER_WORK *offset) {
    if (offset->Mark) {
        offset->CommandFlag = 9;
    } else {
        offset->CommandFlag = 0xC01F;
    }
    offset->CommandFlag |= 0x100;
    SetSit(offset);
    offset->ply->inertia = 0;
    offset->xspeed = 0;
    offset->yspeed = 0;
    offset->actst &= 0xFFFFEFEF;
    if (SetChangeFrameAnime(offset, 0x15) != 0) {
        RqLndEff(offset);
        ReqCmnSE_JumpLand(offset);
    }
    EndtoSit(offset);
}

int EndtoStand(CHARACTER_WORK *offset);

void CmnAct_GetUp(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *temp_a1;

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x17);
    temp_a1 = offset->ply;
    offset->actst |= 0x8000;
    temp_a1->DownFlag &= 0xFFDF;
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);

void CmnAct_ForwardGetUp(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *temp_a1;

    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x45);
    temp_a1 = offset->ply;
    offset->actst |= 0x8000;
    temp_a1->DownFlag &= 0xFFDF;
    EndtoStand(offset);
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
void SetLR(CHARACTER_WORK *offset);
void SetNormalBegin(CHARACTER_WORK *offset);
void SetStand(CHARACTER_WORK *offset);

void CmnAct_StdAtk_ToStand(CHARACTER_WORK *offset, unsigned short Anime) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    SetStand(offset);
    if (SetChangeFrameAnime(offset, Anime & 0xFFFF) != 0) {
        DirectionDirectChange(offset);
        SetLR(offset);
    }
    EndtoStand(offset);
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int EndtoSit(CHARACTER_WORK *offset);
void SetLR(CHARACTER_WORK *offset);
void SetNormalBegin(CHARACTER_WORK *offset);
void SetSit(CHARACTER_WORK *offset);

void CmnAct_StdAtk_ToSit(CHARACTER_WORK *offset, unsigned short Anime) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    SetSit(offset);
    if (SetChangeFrameAnime(offset, Anime & 0xFFFF) != 0) {
        DirectionDirectChange(offset);
        SetLR(offset);
    }
    EndtoSit(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void SetNormalBegin(CHARACTER_WORK *offset);
int g_roundmax(int val, int max);
extern short int Jump_GravityByIdno[];  /* sdata 0x5ccd40 playdat.c */

void CmnAct_StdAtk_Jump(CHARACTER_WORK *offset, unsigned short Anime) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 1;
    SetNormalBegin(offset);
    if (SetChangeFrameAnime(offset, Anime & 0xFFFF) != 0) {
        if (offset->ply->homingtime == 0) {
            offset->gravity = g_roundmax(offset->gravity + 0xA, (int) Jump_GravityByIdno[offset->padid]);
        }
    }
    EndtoAction(offset, 0x30008);
    SetLandBreakActno(offset, 0x200015);
}

void DirectionDirectChange(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int EndtoStand(CHARACTER_WORK *offset);
void SetDustBegin(CHARACTER_WORK *offset);
void SetLR(CHARACTER_WORK *offset);

void CmnAct_DustAtk(CHARACTER_WORK *offset, unsigned short Anime) {
    offset->CommandFlag = 0xC05F;
    offset->attackst |= 0x80;
    SetDustBegin(offset);
    offset->actst &= 0xFFFFFBFF;
    if (offset->InstSt & 0x20) {
        DirectionDirectChange(offset);
        SetLR(offset);
    }
    SetChangeFrameAnime(offset, Anime & 0xFFFF);
    if (offset->idno != 0x11) {
        EndtoStand(offset);
        return;
    }
    EndtoAction(offset, 0x5B);
}

void CmnAct_StandPunch(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x18, 0x9A };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_RightPunch(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x3E, 0xA7 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_StandKick(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x19, 0x9B };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_RightKick(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x47, 0x47 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_StandSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x1A, 0x9C };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_StandSlashNear(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x1B, 0x9D };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_StandHighSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x1C, 0x9E };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_SuperHSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x41, 0xA9 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToStand(offset, act[i]);
}

void CmnAct_SitPunch(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x1D, 0x9F };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToSit(offset, act[i]);
}

void CmnAct_SitKick(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x1E, 0xA0 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToSit(offset, act[i]);
}

void CmnAct_SitRightKick(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x40, 0xA8 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToSit(offset, act[i]);
}

void CmnAct_SitSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x1F, 0xA1 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToSit(offset, act[i]);
}

void CmnAct_SitHighSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x20, 0xA2 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_ToSit(offset, act[i]);
}

void CmnAct_JumpPunch(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x21, 0xA3 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_Jump(offset, act[i]);
}

void CmnAct_JumpKick(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x22, 0xA4 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_Jump(offset, act[i]);
}

void CmnAct_JumpSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x23, 0xA5 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_Jump(offset, act[i]);
}

void CmnAct_JumpHighSlash(CHARACTER_WORK *offset) {
    static const u_short act[] = { 0x24, 0xA6 };
    int i;

    i = IsDragonInstall(offset);
    CmnAct_StdAtk_Jump(offset, act[i]);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);

void FREffectPut(CHARACTER_WORK *offset) {
    CHARACTER_WORK *offobj;
    CREATE_ARG carg;
    int t;

    t = (offset->HitStopTime ? offset->HitStopTime >> 1 : (u_short)offset->ActTimer) & 0xF;
    if (t == 0 || t == 5 || t == 11) {
        if (offset->SpriteFlag & 0x1000) {
            carg.flag = 0x600;
            offobj = EffectCreate(offset, 0x6A, &carg);
            if (offobj) {
                offobj->parentFlag |= 0x800;
            }
        }
        if (offset->SpriteFlag & 0x2000) {
            carg.flag = 0x600;
            offobj = EffectCreate(offset, 0x6B, &carg);
            if (offobj) {
                offobj->parentFlag |= 0x800;
            }
        }
    }
}

int EndtoStand(CHARACTER_WORK *offset);
void FREffectPut(CHARACTER_WORK *offset);
void RqDmgEff(CHARACTER_WORK *offset);

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, Anime);
    if (!offset->HitStopTime) {
        offset->HitBackx -= offset->HitBackx * 0.12;
        offset->xspeed -= offset->xspeed * 0.12;
        if (offset->xspeed) {
            RqDmgEff(offset);
        }
    }
    EndtoStand(offset);
    FREffectPut(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
void FREffectPut(CHARACTER_WORK *offset);
void RqDmgEff(CHARACTER_WORK *offset);

void CmnAct_StdDmg_ToSit(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x400;
    SetChangeFrameAnime(offset, Anime);
    if (!offset->HitStopTime) {
        offset->HitBackx -= offset->HitBackx * 0.15;
        offset->xspeed -= offset->xspeed * 0.15;
        RqDmgEff(offset);
    }
    EndtoSit(offset);
    FREffectPut(offset);
}

void FREffectPut(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
extern short int PDDef_Gravity[];

void CmnAct_SkyDmg_ToJump(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x20;
    SetChangeFrameAnime(offset, Anime);
    JumpControl(offset);
    if (!offset->HitStopTime) {
        offset->HitBackx -= offset->HitBackx * 0.18;
    }
    if (!offset->ply->DustTime) {
        offset->gravity = PDDef_Gravity[offset->padid];
    }
    if (!(offset->ply->pactst & 0x200)) {
        YSpeedtoAction(offset, 0x30033, 0);
    }
    if (!(offset->actst & 0x10)) {
        offset->ply->DownTimer = -1;
        if (offset->actst & 0x100) {
            offset->actst |= 0x8000;
        }
        SetActNoNext(offset, 0x3004C);
    }
    FREffectPut(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void FREffectPut(CHARACTER_WORK *offset);
void SetGroundShakeX(int lv, int dir);
int g_roundmax(int val, int max);
extern short int PDDef_Gravity[];

void CmnAct_SkyDmgBound(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x20;
    offset->ply->JumpFlag |= 2;
    if (SetChangeFrameAnime(offset, Anime)) {
        offset->yspeed = g_roundmax(offset->yspeed, -800);
        offset->gravity = PDDef_Gravity[offset->padid];
        offset->xspeed = -offset->xspeed / 3;
        offset->ply->DownFlag &= ~1;
        SetGroundShakeX(2, offset->dirflag);
    }
    EndtoAction(offset, 0x10043);
    FREffectPut(offset);
}

int WallBoundCheck(CHARACTER_WORK *offset) {
    if (offset->ply->DownFlag & 1) {
        if (offset->actst & 0x2800) {
            SetActNoNext(offset, 0x10030);
            return 1;
        }
        /* Duplicate return node #4. Try simplifying control flow for better match */
        return 0;
    }
    return 0;
}

void FREffectPut(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
int WallBoundCheck(CHARACTER_WORK *offset);
extern short int PDDef_Gravity[];

void CmnAct_SkyDmg_ToDrop(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x20;
    offset->ply->JumpFlag &= ~2;
    SetChangeFrameAnime(offset, Anime);
    JumpControl(offset);
    if (!offset->HitStopTime) {
        offset->HitBackx -= offset->HitBackx * 0.18;
        if (!offset->ply->DownTimer) {
            offset->gravity = PDDef_Gravity[offset->padid];
        }
        if (!flag && WallBoundCheck(offset)) {
            return;
        }
    }
    if (!flag) {
        if (offset->yspeed > 0) {
            SetActNoNext(offset, next | 0x30000);
        }
    } else {
        if (-offset->gravity * 10 < offset->yspeed && !(offset->actst & 0x400000)) {
            SetActNoNext(offset, next | 0x30000);
        }
    }
    FREffectPut(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
void FREffectPut(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
int WallBoundCheck(CHARACTER_WORK *offset);
extern short int PDDef_Gravity[];

void CmnAct_SkyDmg_ToDown(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x20;
    offset->ply->JumpFlag &= ~2;
    SetChangeFrameAnime(offset, Anime);
    JumpControl(offset);
    if (!offset->HitStopTime) {
        offset->HitBackx -= offset->HitBackx * 0.18;
        if (!offset->ply->DownTimer) {
            offset->gravity = PDDef_Gravity[offset->padid];
        }
        if (WallBoundCheck(offset)) {
            return;
        }
    }
    if (!(offset->actst & 0x10)) {
        if (!(offset->SpriteFlag & 0x40000)) {
            EffectCreate(offset, 0x6C, 0);
        }
        if (flag || !(offset->ply->DownFlag & 2)) {
            offset->ply->DownTimer = -1;
        }
        if (!(offset->ply->DownFlag & 0x200)) {
            if (offset->actst & 0x100) {
                offset->actst |= 0x8000;
            }
            if (offset->ply->DownFlag & 0x20) {
                offset->actst |= 0x8000;
            }
            offset->actst |= 0x100;
        } else {
            offset->actst &= ~0x8000;
            offset->actst &= ~0x100;
        }
        SetActNoNext(offset, next | 0x30000);
    }
    FREffectPut(offset);
}

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StandDamage(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToStand(offset, 0x25);
}

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StandDamage2(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToStand(offset, 0x26);
}

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StandDamage3(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToStand(offset, 0x27);
}

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StandLowDamage(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToStand(offset, 0x28);
}

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StandLowDamage2(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToStand(offset, 0x29);
}

void CmnAct_StdDmg_ToStand(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StandLowDamage3(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToStand(offset, 0x2A);
}

void CmnAct_StdDmg_ToSit(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SitDamage(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToSit(offset, 0x2B);
}

void CmnAct_StdDmg_ToSit(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SitDamage2(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToSit(offset, 0x2C);
}

void CmnAct_StdDmg_ToSit(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SitDamage3(CHARACTER_WORK *offset) {
    CmnAct_StdDmg_ToSit(offset, 0x2D);
}

void CmnAct_SkyDmg_ToJump(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SkyDamage(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToJump(offset, 0x2E);
}

void CmnAct_SkyDmg_ToJump(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SkyDamage2(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToJump(offset, 0x2F);
}

void CmnAct_SkyDmgBound(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_WallBound(CHARACTER_WORK *offset) {
    CmnAct_SkyDmgBound(offset, 0x30);
}

void CmnAct_SkyDmg_ToDrop(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag);

void CmnAct_Down(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToDrop(offset, 0x31, 0x33, 0);
}

void CmnAct_SkyDmg_ToDown(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag);

void CmnAct_Down2(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToDown(offset, 0x33, 0x4C, 0);
}

void CmnAct_SkyDmg_ToDrop(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag);

void CmnAct_VertDown(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToDrop(offset, 0x44, 0x49, 0x1);
}

void CmnAct_SkyDmg_ToDown(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag);

void CmnAct_VertDownDrop(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToDown(offset, 0x49, 0x4E, 0x1);
}

void CmnAct_SkyDmg_ToDown(CHARACTER_WORK *offset, short unsigned int Anime, int next, int flag);

void CmnAct_ForwardDown(CHARACTER_WORK *offset) {
    CmnAct_SkyDmg_ToDown(offset, 0x43, 0x4D, 0x1);
}

void FREffectPut(CHARACTER_WORK *offset);
void ReqCmnSE_DownLand(CHARACTER_WORK *offset);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);

void CmnAct_DownLandSub(CHARACTER_WORK *offset, int instno, int next) {
    offset->actst |= 0x20;
    if (!(offset->ply->DownFlag & 0x200)) {
        offset->actst |= 0x100;
    } else {
        offset->actst &= ~0x100;
    }
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, instno)) {
        ReqCmnSE_DownLand(offset);
    }
    if (offset->HitPoint > 0) {
        if (!(offset->ply->pactst & 0x200)) {
            if (offset->ply->DownFlag & 0x10) {
                TimetoAction(offset, next | 0x20000, 60);
            } else {
                TimetoAction(offset, next | 0x20000, 15);
            }
        }
    } else {
        offset->actst |= 0x10000;
    }
    FREffectPut(offset);
}

void CmnAct_DownLandSub(CHARACTER_WORK *offset, int instno, int next);

void CmnAct_DownLand(CHARACTER_WORK *offset) {
    u_short FAAfrNoTb[19] = {
        0x0, 0xB3, 0x97, 0x9B, 0x92, 0x7B, 0x9B, 0xAA, 0x8F, 0x89,
        0xAA, 0x7D, 0xA3, 0x86, 0x96, 0x93, 0x84, 0x89, 0x89,
    };

    if (offset->ply->pactst & 0x100) {
        CmnAct_DownLandSub(offset, FAAfrNoTb[offset->idno], 0x17);
    }
    if (offset->ply->pactst & 0x200) {
        offset->actst |= 0x40000000;
        CmnAct_DownLandSub(offset, 0xB2, 0x17);
    }
    CmnAct_DownLandSub(offset, 0x32, 0x17);
}

void CmnAct_DownLandSub(CHARACTER_WORK *offset, int instno, int next);

void CmnAct_ForwardDownLand(CHARACTER_WORK *offset) {
    CmnAct_DownLandSub(offset, 0x48, 0x45);
}

void CmnAct_DownLandSub(CHARACTER_WORK *offset, int instno, int next);

void CmnAct_VertDownLand(CHARACTER_WORK *offset) {
    CmnAct_DownLandSub(offset, 0x4A, 0x45);
}

void CmnAct_SkyDmg_ToJump(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_Kirimomi(CHARACTER_WORK *offset) {
    if (offset->ply->pactst & 0x200) {
        CmnAct_SkyDmg_ToJump(offset, 0xAB);
    } else {
        CmnAct_SkyDmg_ToJump(offset, 0x34);
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int PlayerDamage(CHARACTER_WORK *offset, int dam, TACTHEADER *ahp);
void ReqCmnSE_DownLand(CHARACTER_WORK *offset);
void SetGroundShake(int lv);
void SetJumpControl(CHARACTER_WORK *offset);
int g_roundmax(int val, int max);

void CmnAct_DownBoundSub(CHARACTER_WORK *offset, int instno, int next) {
    offset->HitBackx = 0;
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x20;
    if (SetChangeFrameAnime(offset, instno)) {
        if (!(offset->SpriteFlag & 0x40000)) {
            EffectCreate(offset, 0x6C, 0);
        }
        if (offset->yspeed > 1800) {
            SetGroundShake(2);
        } else {
            SetGroundShake(1);
        }
        ReqCmnSE_DownLand(offset);
        offset->SpriteFlag &= ~0x3000;
        if (offset->ply->DownFlag & 2) {
            offset->ply->DownTimer += 10;
            if (offset->ply->DownBoundDamage) {
                offset->actst &= ~0x2000000;
                PlayerDamage(offset, offset->ply->DownBoundDamage, 0);
                offset->actst |= 0x2000000;
            }
        } else {
            if (!(offset->ply->DownFlag & 0x200)) {
                offset->actst |= 0x100;
            }
            offset->xspeed /= 4;
        }
    } else if (offset->Mark & 2) {
        if (~offset->actst & 0x10) {
            SetGroundShake(1);
            SetActNoNext(offset, next | 0x20000);
        }
    } else if (offset->Mark & 1) {
        if (offset->ply->DownFlag & 2) {
            offset->actst &= ~0x100;
            offset->ply->DownFlag &= ~2;
            offset->yspeed = -offset->yspeed * 0.9;
            SetActNoNext(offset, 0x30031);
            return;
        }
        offset->yspeed = g_roundmax(-offset->yspeed / 3, -800);
        offset->gravity = 150;
        offset->Mark = 2;
    }
    SetJumpControl(offset);
}

void CmnAct_DownBoundSub(CHARACTER_WORK *offset, int instno, int next);

void CmnAct_DownBound(CHARACTER_WORK *offset) {
    if (offset->ply->pactst & 0x200) {
        CmnAct_DownBoundSub(offset, 0xAD, 0x32);
    } else {
        CmnAct_DownBoundSub(offset, 0x4C, 0x32);
    }
}

void CmnAct_DownBoundSub(CHARACTER_WORK *offset, int instno, int next);

void CmnAct_ForwardDownBound(CHARACTER_WORK *offset) {
    CmnAct_DownBoundSub(offset, 0x4D, 0x48);
}

void CmnAct_DownBoundSub(CHARACTER_WORK *offset, int instno, int next);

void CmnAct_VertDownBound(CHARACTER_WORK *offset) {
    CmnAct_DownBoundSub(offset, 0x4E, 0x4A);
}

void CmnAct_Faint(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->ply->FaintPoint = 0;
    SetChangeFrameAnime(offset, 0x46);
    if (offset->ply->FaintTime <= 0) {
        SetActNoNext(offset, 0);
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void ReqCmnSE_Ukemi(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void CmnAct_Ukemi(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    if (SetChangeFrameAnime(offset, 0x42) != 0) {
        ReqCmnSE_Ukemi(offset);
        offset->ply->whiteouttime = 8;
    }
    SetLandBreakActno(offset, 0x15);
    EndtoAction(offset, 0x10008);
}

void CreatePerfectGuardEffect(CHARACTER_WORK *offset);
unsigned int GetOtherAttackFlag(CHARACTER_WORK *offset);
void PerfectGuardEnableCheck(CHARACTER_WORK *offset);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void CmnAct_StdGuard(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0x8304;
    offset->actst &= ~0x1000;
    SetChangeFrameAnime(offset, Anime);
    PerfectGuardEnableCheck(offset);
    CreatePerfectGuardEffect(offset);
    if ((*(unsigned int *)&offset->parentFlag & 0x480000) == 0x480000) {
        if (!offset->HitStopTime) {
            Tension_Dec(offset, -2);
        }
    } else if (!GetOtherAttackFlag(offset)) {
        if (offset->actst & 0x400) {
            SetActNoNext(offset, 1);
        } else {
            SetActNoNext(offset, 0);
        }
    }
}

void CreatePerfectGuardEffect(CHARACTER_WORK *offset);
void PerfectGuardEnableCheck(CHARACTER_WORK *offset);
void PerfectGuardOnCheck(CHARACTER_WORK *offset);
void RqDmgEff(CHARACTER_WORK *offset);

void CmnAct_StdGuard2(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC05F;
    offset->actst = (offset->actst & ~0x1000) | 0x200;
    SetChangeFrameAnime(offset, Anime);
    PerfectGuardOnCheck(offset);
    PerfectGuardEnableCheck(offset);
    CreatePerfectGuardEffect(offset);
    if (!offset->HitStopTime) {
        offset->xspeed -= offset->xspeed * 0.1;
        offset->HitBackx -= offset->HitBackx * 0.1;
        RqDmgEff(offset);
    }
    if (offset->ActTimer >= offset->AnimeTimeMax) {
        if (offset->actst & 0x400) {
            SetActNoNext(offset, 1);
        } else {
            SetActNoNext(offset, 0);
        }
    }
}

void CreatePerfectGuardEffect(CHARACTER_WORK *offset);
unsigned int GetOtherAttackFlag(CHARACTER_WORK *offset);
void PerfectGuardEnableCheck(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void CmnAct_SkyGuard_(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC13F;
    SetChangeFrameAnime(offset, Anime);
    PerfectGuardEnableCheck(offset);
    CreatePerfectGuardEffect(offset);
    if ((*(unsigned int *)&offset->parentFlag & 0x480000) == 0x480000) {
        if (!offset->HitStopTime) {
            Tension_Dec(offset, -3);
        }
    } else if (!GetOtherAttackFlag(offset) || !(offset->GuardSt & 4)) {
        SetActNoNext(offset, 0x30008);
    }
    SetLandBreakActno(offset, 0x35);
}

void CreatePerfectGuardEffect(CHARACTER_WORK *offset);
void JumpControl(CHARACTER_WORK *offset);
void PerfectGuardEnableCheck(CHARACTER_WORK *offset);
void PerfectGuardOnCheck(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
int g_roundmin(int val, int min);

void CmnAct_SkyGuard2_(CHARACTER_WORK *offset, short unsigned int Anime) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x200;
    SetChangeFrameAnime(offset, Anime);
    PerfectGuardOnCheck(offset);
    PerfectGuardEnableCheck(offset);
    CreatePerfectGuardEffect(offset);
    JumpControl(offset);
    SetLandBreakActno(offset, 0x36);
    if (!offset->HitStopTime) {
        offset->HitBackx -= offset->HitBackx * 0.15;
        offset->AnimeTimeMax = g_roundmin(offset->AnimeTimeMax--, 1) - 1;
    }
    if (offset->ActTimer >= 24) {
        SetActNoNext(offset, 0x30008);
    }
    if (offset->AnimeTimeMax < 4) {
        SetActNoNext(offset, 0x30008);
    }
}

void CmnAct_StdGuard(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StdGuardV(CHARACTER_WORK *offset) {
    offset->actst &= ~0x400;
    CmnAct_StdGuard(offset, 0x35);
}

void CmnAct_StdGuard2(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StdGuardV2(CHARACTER_WORK *offset) {
    offset->actst &= ~0x400;
    CmnAct_StdGuard2(offset, 0x36);
}

void CmnAct_StdGuard(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StdGuardH(CHARACTER_WORK *offset) {
    offset->actst &= ~0x400;
    CmnAct_StdGuard(offset, 0x37);
}

void CmnAct_StdGuard2(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_StdGuardH2(CHARACTER_WORK *offset) {
    offset->actst &= ~0x400;
    CmnAct_StdGuard2(offset, 0x38);
}

void CmnAct_StdGuard(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SitGuard(CHARACTER_WORK *offset) {
    offset->actst |= 0x400;
    CmnAct_StdGuard(offset, 0x39);
}

void CmnAct_StdGuard2(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SitGuard2(CHARACTER_WORK *offset) {
    offset->actst |= 0x400;
    CmnAct_StdGuard2(offset, 0x3A);
}

void CmnAct_SkyGuard_(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SkyGuard(CHARACTER_WORK *offset) {
    CmnAct_SkyGuard_(offset, 0x3B);
}

void CmnAct_SkyGuard2_(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_SkyGuard2(CHARACTER_WORK *offset) {
    CmnAct_SkyGuard2_(offset, 0x3C);
}

void CmnAct_DustAtk(CHARACTER_WORK *offset, short unsigned int Anime);

void CmnAct_DustAttack(CHARACTER_WORK *offset) {
    CmnAct_DustAtk(offset, 0x3F);
}

int EndtoStand(CHARACTER_WORK *offset);

void CmnAct_Stand2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x101;
    offset->actst = (offset->actst & 0xFFFFEFFF) | 0x40000;
    SetChangeFrameAnime(offset, 0x3D);
    EndtoStand(offset);
}
