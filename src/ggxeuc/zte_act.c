#include "generated/zte_act.h"

extern GAME_INFOMATION g_info;

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int Random(void);
void set_random_seed_abs(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int EndtoAction(CHARACTER_WORK *offset, int actno);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
int EndtoSit(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int YSpeedtoAction(CHARACTER_WORK *offset, int actno, int yspeed);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);
extern PAD GameKey[2];
extern TRAINING_SETTING *tr_info;
extern PAD pads[2];
int IsHoldKick(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
void OrScreenFlag(int flag);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
extern short int Jump_GravityByIdno[];
int LandtoAction(CHARACTER_WORK *offset, int actno);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);
void Tension_Add_ByLongHair(CHARACTER_WORK *offset);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);
int EndtoDemo(CHARACTER_WORK *offset);
void SetWinActFlag(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
void Tension_Dec(CHARACTER_WORK *offset, int Val);
void SetAnten(CHARACTER_WORK *offset, int flag);
void ZT_PreDrillDraw(CHARACTER_WORK *offset);
void ZTEDBARDrawSub(CHARACTER_WORK *offset, unsigned int color);
void ZTEDBARDraw(CHARACTER_WORK *offset);
void ZTEDBARDrawOverheat(CHARACTER_WORK *offset);
void ZTEDBARDrawReady(CHARACTER_WORK *offset);
void SetZTEDBARPos(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int EndtoEliminate(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
static unsigned char DrillObjFlag[2] = { 1, 2 };

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
extern SURVIVAL_INFOMATION *sv_info;

void ZTED_Time_Dec(CHARACTER_WORK *offset, int val) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    val *= 6;
    if (((g_info.mode & 0x200) && (sv_info->ply == poffset->padid || !sv_info->sv_ext.battle))
        || g_info.boss[poffset->padid] != 1) {
        poffset->ply->PersonalWork[1].w[0] -= val;
    }
    if (poffset->ply->PersonalWork[1].w[0] <= 0) {
        poffset->ply->PersonalWork[1].w[0] = 0;
        poffset->ply->PersonalWork[0].b[1] = 0;
    }
}

void RequestHit(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);
void ZTED_Time_Dec(CHARACTER_WORK *offset, int val);

void ZTED_ColliCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 0xC) {
        return;
    }
    if (offset->DamColliFlag) {
        RequestHit(offset, offset->DamActHeader[0].flag, offset->DamActHeader[0].lvflag, 0);
        SetAttackActNo(offset, 9);
        ZTED_Time_Dec(offset, 600);
    }
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int IsLeft(CHARACTER_WORK *offset);
int IsOffHighSlash(CHARACTER_WORK *offset);
int IsOffKick(CHARACTER_WORK *offset);
int IsOffPunch(CHARACTER_WORK *offset);
int IsOffSlash(CHARACTER_WORK *offset);
int IsRight(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
void ZTED_Time_Dec(CHARACTER_WORK *offset, int val);
extern int BattleStatus;
extern SURVIVAL_INFOMATION *sv_info;

void ZTED_CommandCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    int R;
    int L;

    poffset = GetParentPlayer(offset);
    SetObjFlag(offset, 4);
    if (offset->actno == 9 || offset->actno == 0xC) {
        return;
    }
    if (!SearchEffectWorkActno(offset, 0x41, 7) && !SearchEffectWorkActno(offset, 0x41, 8)) {
        EffectCreate_ActNo(offset, 0x41, 0, 7);
    }
    if (poffset->ply->PersonalWork[0].ub[0] == 4) {
        SetAttackActNo(offset, 10);
        poffset->ply->PersonalWork[0].b[0] = 0;
    } else if (poffset->ply->PersonalWork[0].ub[0] == 1) {
        SetActNo(offset, 3);
        poffset->ply->PersonalWork[0].b[0] = 0;
    } else if (poffset->ply->PersonalWork[0].ub[0] == 2) {
        SetActNo(offset, 4);
        poffset->ply->PersonalWork[0].b[0] = 0;
    } else if (poffset->ply->PersonalWork[0].ub[0] == 3) {
        SetActNo(offset, 7);
        poffset->ply->PersonalWork[0].b[0] = 0;
    } else {
        if (offset->Localid & 0x100) {
            offset->Localid &= ~0x100;
            SetAttackActNo(offset, 9);
            return;
        }
        if (poffset->actst & 0x220) {
            SetAttackActNo(offset, 9);
            return;
        }
        if (offset->CommandFlag & 0x40) {
            return;
        }
        if (BattleStatus != 2) {
            SetActNo(offset, 9);
            return;
        }
        if (((g_info.mode & 0x200) && (sv_info->ply == poffset->padid || !sv_info->sv_ext.battle))
            || g_info.boss[offset->padid] != 1) {
            poffset->ply->PersonalWork[1].w[0]--;
        }
        ZTED_Time_Dec(offset, 0);
        if (poffset->ply->PersonalWork[1].w[0] <= 0) {
            SetActNo(offset, 9);
            return;
        }
        if (offset->dirflag == 1) {
            if (GetEnemyPlayer(offset)->posx < offset->posx - 8000) {
                offset->dirflag = !offset->dirflag;
            }
        } else {
            if (GetEnemyPlayer(offset)->posx > offset->posx + 8000) {
                offset->dirflag = !offset->dirflag;
            }
        }
        if (IsOffPunch(poffset)) {
            SetActNo(offset, 3);
            return;
        }
        if (IsOffKick(poffset)) {
            SetActNo(offset, 4);
            return;
        }
        if (IsOffSlash(poffset)) {
            SetActNo(offset, 7);
            return;
        }
        if (IsOffHighSlash(poffset)) {
            SetActNo(offset, 0xB);
            return;
        }
        if (offset->dirflag == poffset->dirflag) {
            R = IsRight(poffset);
            L = IsLeft(poffset);
        } else {
            L = IsRight(poffset);
            R = IsLeft(poffset);
        }
        if ((~offset->CommandFlag & 1) && !R && !L) {
            SetActNo(offset, 0);
            return;
        }
        if ((~offset->CommandFlag & 0x4002) && R) {
            SetActNo(offset, 1);
            return;
        }
        if ((~offset->CommandFlag & 0x8004) && L) {
            SetActNo(offset, 2);
        }
    }
}

void ZTEDAct_0(CHARACTER_WORK *offset) {
    offset->CommandFlag = 1;
    SetChangeFrameAnime(offset, 0);
    SetXSpeed(offset, 0);
    offset->actst |= 0x01000000;
}

void ZTEDAct_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x4002;
    SetChangeFrameAnime(offset, 1);
    offset->actst |= 0x01000000;
}

void ZTEDAct_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x8004;
    SetChangeFrameAnime(offset, 2);
    offset->actst |= 0x01000000;
}

void ZTEDAct_3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnime(offset, 3);
    EndtoAction(offset, 0);
}

void ZTEDAct_4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnime(offset, 4);
    EndtoAction(offset, 5);
    offset->actst |= 0x01000000;
}

void ZTEDAct_5(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnime(offset, 5);
    if (offset->actst & 0x2800) {
        SetAttackActNo(offset, 8);
    } else {
        TimetoAction(offset, 6, 0x1C);
    }
    offset->actst |= 0x01000000;
}

void ZTEDAct_6(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnime(offset, 6);
    EndtoAction(offset, 0);
    offset->actst |= 0x01000000;
}

void ZTEDAct_7(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnime(offset, 7);
    EndtoAction(offset, 0);
}

void ZTEDAct_8(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnimeSameActno(offset);
    EndtoAction(offset, 0);
}

int EndtoEliminate(CHARACTER_WORK *offset);

void ZTEDAct_Attack2Eliminate(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);

void ZTEDAct_BigFace(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0x40;
    offset->TexSizeMax = 9;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);

void ZTBDAct_1(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset) != 0) {
        offset->posx += ((Random() & 0x3F) - 0x20) * 0x64;
        offset->posy += ((Random() & 0x3F) - 0x20) * 0x64;
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void ZTBDAct_2(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset) != 0) {
        SetXSpeed(offset, -0xC8 - (Random() & 0xFF));
        SetYSpeed(offset, -0xC8 - (Random() & 0xFF), 0x1E);
    }
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);

void ZTBDAct_3(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset) != 0) {
        SetXSpeed(offset, -0x320 - (Random() & 0xFF));
        SetYSpeed(offset, -0x320 - (Random() & 0xFF), 0x1E);
    }
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void ZTDRAct_0(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->parentFlag |= 0x400;
        offset->Localid = offset->actno;
    }
    SetObjFlag(offset, DrillObjFlag[offset->Localid]);
    if (offset->parentFlag & 0x40) {
        SetActNoNext(offset, 2);
    }
    EndtoAction(offset, 2);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void ZTDRAct_End(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->Localid < 2) {
        SetObjFlag(offset, DrillObjFlag[offset->Localid]);
    }
    EndtoEliminate(offset);
}

void ZTDRAct_Bom(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset) != 0) {
        offset->Localid = 2;
    }
    EndtoAction(offset, 2);
}

void EllipseDraw(NL_POINT3 *p, unsigned int c0, unsigned int c1, NL_POINT3 *offp);
void ObjWorld2ScreenV(CHARACTER_WORK *offset, NL_POINT3 *p);
void SetBlendingMode(short int mode);

void ZT_PreDrillDraw(CHARACTER_WORK *offset) {
    NL_POINT3 p[2];
    NL_POINT3 off;

    ObjWorld2ScreenV(offset, &off);
    SetBlendingMode(0);
    p[1].z = 200.0f;
    p[0].z = 200.0f;
    p[1].x = offset->scale * off.z;
    p[0].x = p[1].x * 0.5;
    p[1].y = offset->scale * off.z * 0.25f;
    p[0].y = p[1].y * 0.5;
    EllipseDraw(p, 0x800080, offset->trans << 24, &off);
}

int EndtoEliminate(CHARACTER_WORK *offset);

void ZTDRAct_Hamon(CHARACTER_WORK *offset) {
    offset->graphicfunc = ZT_PreDrillDraw;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void ZTEDBARDrawSub(CHARACTER_WORK *offset, unsigned int color);

void ZTEDBARDraw(CHARACTER_WORK *offset) {
    ZTEDBARDrawSub(offset, 0xC000FFFF);
}

void ZTEDBARDrawSub(CHARACTER_WORK *offset, unsigned int color);

void ZTEDBARDrawOverheat(CHARACTER_WORK *offset) {
    ZTEDBARDrawSub(offset, 0xC0FF0000);
}

void ZTEDBARDrawSub(CHARACTER_WORK *offset, unsigned int color);

void ZTEDBARDrawReady(CHARACTER_WORK *offset) {
    ZTEDBARDrawSub(offset, 0xFFFFFFFF);
}

void SetZTEDBARPos(CHARACTER_WORK *offset) {
    int a;

    a = offset->padid ? 1 : -1;
    offset->posy = 10000;
    offset->SpriteFlag |= 1;
    offset->posx = (a * 230 + 320) * 100;
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void SetZTEDBARPos(CHARACTER_WORK *offset);

void ZTEDBAR(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    offset->graphicfunc = ZTEDBARDraw;
    SetChangeFrameAnimeSameActno(offset);
    SetZTEDBARPos(offset);
    if (poffset->ply->PersonalWork[1].w[0] == 0) {
        SetActNoNext(offset, 8);
    }
    if (~poffset->ply->ObjFlag & 4) {
        SetActNoNext(offset, 9);
    }
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void SetZTEDBARPos(CHARACTER_WORK *offset);

void ZTEDBAR_Overheat(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    offset->graphicfunc = ZTEDBARDrawOverheat;
    SetChangeFrameAnimeSameActno(offset);
    if (poffset->ply->PersonalWork[0].ub[1]) {
        SetActNoNext(offset, 9);
    }
    SetZTEDBARPos(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void SetZTEDBARPos(CHARACTER_WORK *offset);

void ZTEDBAR_Ready(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = GetParentPlayer(offset);
    offset->graphicfunc = ZTEDBARDrawReady;
    SetChangeFrameAnimeSameActno(offset);
    SetZTEDBARPos(offset);
    if (poffset->ply->PersonalWork[0].ub[1]) {
        EndtoEliminate(offset);
    }
    if (poffset->ply->ObjFlag & 4) {
        EndtoEliminate(offset);
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);

void ZTDR_Mine(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *offset);
void SetBlendingMode(short int mode);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
extern USER_SETTING userinfo;

/* Eddie gauge under Zato: a bar of len px (gauge PersonalWork[1].w[0], 0..600 -> 0..128 px)
   drawn in `color`, over a black 129 px backing box one unit deeper. a = +1 grows right
   (1P, padid 0), -1 grows left (2P). offs = user's upper-cockpit screen offset. */
void ZTEDBARDrawSub(CHARACTER_WORK *offset, unsigned int color) {
    int a = -1;
    int offs;
    CHARACTER_WORK *parent;
    int x, y, len, na, xo, t0, t1, t2, t3;

    parent = GetParentPlayer(offset);
    x = offset->posx / 100;
    y = offset->posy / 100;
    if (offset->padid == 0) a = 1;
    len = (parent->ply->PersonalWork[1].w[0] << 7) / 600;
    offs = userinfo.offs_cockpit_upper;
    if (offs) offs -= 6;
    SetBlendingMode(offset->transmode);
    na = -a;
    xo = na * 64;
    t0 = offs - 10;
    t1 = offs - 8;
    polydraw_FBox(x + xo, y + t0, a * len + (x + xo), y + t1, offset->priority + 0.0, color);
    t2 = offs - 11;
    t3 = offs - 7;
    polydraw_FBox(x + (xo + na), y + t2, x + (xo - na) + a * 129, y + t3, offset->priority + 1.0, 0xFF000000);
}
