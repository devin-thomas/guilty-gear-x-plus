#include "generated/tse_act.h"

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
extern RECTL ScreenObj;

void TSEX_Act1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (!offset->Localid) {
            offset->dirflag = !offset->dirflag;
            if (offset->dirflag) {
                offset->posx = -(ScreenObj.w / 2);
            } else {
                offset->posx = ScreenObj.w / 2;
            }
            offset->Localid = 0;
            offset->posx += ScreenObj.x;
        } else {
            if (eoffset->posx < offset->posx) {
                offset->dirflag = 0;
            } else {
                offset->dirflag = 1;
            }
        }
    }
    SetObjFlag(offset, 1);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 2);
    }
    EndtoAction(offset, 2);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void TSEX_Act2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (!offset->Localid) {
            offset->Localid = 1;
        } else {
            if (eoffset->posx < offset->posx) {
                offset->dirflag = 0;
            } else {
                offset->dirflag = 1;
            }
        }
    }
    SetObjFlag(offset, 8);
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 2);
    }
    EndtoAction(offset, 2);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
extern GAME_INFOMATION g_info;

void TSEX_Act3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->Localid += 2;
    }
    if ((offset->Localid % 2)) {
        SetObjFlag(offset, 8);
    } else {
        SetObjFlag(offset, 1);
    }
    if (g_info.boss[poffset->padid] == 1) {
        EndtoEliminate(offset);
    } else {
        EndtoEliminate(offset);
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void TSEX_ActK1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
    }
    SetLandBreakActno(offset, offset->actno + 3);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
void OutSideEliminateJust(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
extern int BattleStatus;

void TSPH_Act1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
    }
    EndtoAction(offset, 0x10003);
    SetObjFlag(offset, 2);
    OutSideEliminateJust(offset);
    if (BattleStatus != 2) {
        offset->actst |= 0x80000000;
    }
    SetJumpControl(offset);
}

int IsHitStop(CHARACTER_WORK *offset);
void OutSideEliminateJust(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
int TimetoAction(CHARACTER_WORK *offset, int actno, int time);
float nlCos(short unsigned int r);
extern int BattleStatus;

void TSPH_Act2(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->Localid = 0x4000;
        offset->HitBackx = offset->posy;
    }
    if (BattleStatus != 2) {
        offset->actst |= 0x80000000;
        return;
    }
    SetObjFlag(offset, 2);
    if (offset->attackst & 0x80000) {
        SetActNoNext(offset, 9);
        return;
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 4);
        return;
    }
    if (!IsHitStop(offset)) {
        offset->Localid += 0x100;
        offset->posy = nlCos(offset->Localid) * 40.0f * 100.0f + offset->HitBackx;
        OutSideEliminateJust(offset);
    }
    TimetoAction(offset, 4, 200);
}

void CircleDraw(NL_POINT3 *p, unsigned int c0, unsigned int c1, NL_POINT3 *offp);
int GetBattleTimeStopFlag(int padid);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
float ObjWorld2ScreenX(float x);
float ObjWorld2ScreenY(float y);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetBlendingMode(short int mode);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
extern GAME_INFOMATION g_info;
extern int BattleStatus;

void TSPH_Act3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);
    CHARACTER_WORK *poffset = offset->parentWork;
    CHARACTER_WORK *koffset = SearchObjectWork(poffset, 0x52);
    int highttb[19] = {
        0, 15000, 15000, 15000, 15000, 15000, 15000, 15000, 15000, 15000,
        15000, 15000, 15000, 15000, 15000, 15000, 15000, 15000, 15000
    };
    NL_POINT3 p;
    NL_POINT3 op;

    if (BattleStatus != 2) {
        SetActNoNext(offset, 0xC);
    }
    if (g_info.boss[poffset->padid] != 1 && offset->ActTimer >= 720) {
        SetActNoNext(offset, 0xC);
    }
    if (GetBattleTimeStopFlag(-1)) {
        offset->actst |= 0x80000000;
    }
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->parentFlag |= 2;
        offset->Localid = 600;
        offset->actst &= ~8;
        if (koffset) {
            koffset->Localid = 120;
        }
    }
    offset->RHitBackx = eoffset->posy - highttb[eoffset->idno];
    offset->angle += 0x100;
    if (offset->dirflag) {
        offset->HitBackx = eoffset->posx;
    } else {
        offset->HitBackx = eoffset->posx;
    }
    offset->yspeed = (offset->RHitBackx - offset->posy) / 7;
    offset->xspeed = (offset->HitBackx - offset->posx) / 8;
    if (offset->Localid) {
        p.y = offset->Localid;
        p.z = 1.0f;
        op.x = ObjWorld2ScreenX(offset->posx);
        op.y = ObjWorld2ScreenY(offset->posy);
        SetBlendingMode(1);
        CircleDraw(&p, 0xFFFF0000, 0x55FFFF00, &op);
        SetBlendingMode(0);
        offset->Localid -= 60;
    } else {
        offset->actst |= 8;
        poffset->ply->PersonalWork[0].l = 5;
    }
    SetObjFlag(offset, 2);
    SetJumpControl(offset);
}

int TSKR_IsChase(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (offset->dirflag) {
        offset->HitBackx = poffset->posx - 12000;
    } else {
        offset->HitBackx = poffset->posx + 12000;
    }
    offset->RHitBackx = poffset->posy - 30000;
    if (((offset->HitBackx - offset->posx) < 0 ? (offset->posx - offset->HitBackx) : (offset->HitBackx - offset->posx)) > 500 || ((offset->RHitBackx - offset->posy) < 0 ? (offset->posy - offset->RHitBackx) : (offset->RHitBackx - offset->posy)) > 500) {
        return 1;
    }
    return 0;
}

int GetBattleTimeStopFlag(int padid);
int Random(void);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
int TSKR_IsChase(CHARACTER_WORK *offset);
void debugf(char *format);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void TSKR_Act_Norm(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->xspeed = 0;
        offset->yspeed = 0;
        offset->actst |= 8;
        offset->gravity = 0;
    }
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (poffset->ply->PersonalWork[0].ul == 5) {
        poffset->ply->PersonalWork[0].ul = 0;
        if (g_info.boss[poffset->padid] == 1) {
            if (offset->Localid >= 30) {
                switch (Random() & 3) {
                case 0:
                    SetActNoNext(offset, 5);
                    break;
                case 1:
                    SetActNoNext(offset, 6);
                    break;
                case 2:
                    SetActNoNext(offset, 7);
                    break;
                case 3:
                    SetActNoNext(offset, 9);
                    break;
                }
                offset->Localid = 0;
                return;
            }
        } else if (offset->Localid >= 120) {
            switch (Random() & 3) {
            case 0:
                SetActNoNext(offset, 5);
                break;
            case 1:
                SetActNoNext(offset, 6);
                break;
            case 2:
                SetActNoNext(offset, 7);
                break;
            case 3:
                SetActNoNext(offset, 9);
                break;
            }
            offset->Localid = 0;
            return;
        }
        offset->Localid++;
    } else if (poffset->ply->PersonalWork[0].ul) {
        if (poffset->ply->PersonalWork[0].ul < 13) {
            SetActNoNext(offset, poffset->ply->PersonalWork[0].ul);
        } else {
            debugf("TSKR Danger 1\n");
        }
    }
    offset->dirflag = poffset->dirflag;
    SetJumpControl(offset);
    if (TSKR_IsChase(offset)) {
        SetActNoNext(offset, 1);
        if (poffset->ply->PersonalWork[0].ul == 0) {
            poffset->ply->PersonalWork[0].ul = 1;
        }
    }
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
}

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void TSKR_Act_Stand2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (poffset->Mark == 1) {
        offset->xspeed = 0;
        offset->yspeed = 0;
        offset->actst &= ~8;
    }
    if (poffset->Mark == 2) {
        offset->actst |= 8;
        if (offset->dirflag) {
            offset->xspeed = -500;
        } else {
            offset->xspeed = 500;
        }
        offset->yspeed = -100;
    }
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->dirflag) {
            offset->xspeed = 500;
        } else {
            offset->xspeed = -500;
        }
        offset->yspeed = 100;
    }
    if (poffset->actno != 0x91) {
        SetActNoNext(offset, 0);
        if (poffset->ply->PersonalWork[0].l == 2) {
            poffset->ply->PersonalWork[0].l = 0;
        }
    }
    SetJumpControl(offset);
}

int GetBattleTimeStopFlag(int padid);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void TSKR_Act_LH1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    COLLISION *ex;

    poffset = offset->parentWork;
    ex = Get_GCLExNo(poffset, 0);
    SetChangeFrameAnime(offset, offset->actno);
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (poffset->actno != 0x50 && poffset->actno != 0x51) {
        SetActNo(offset, 0);
        if (poffset->ply->PersonalWork[0].l == 3) {
            poffset->ply->PersonalWork[0].l = 0;
        }
        return;
    }
    if (poffset->Mark == 1) {
        SetActNo(offset, 4);
    }
    if (poffset->dirflag) {
        offset->xspeed = (poffset->posx - ex->x * 100 - offset->posx) / 4;
    } else {
        offset->xspeed = (poffset->posx + ex->x * 100 - offset->posx) / 4;
    }
    offset->yspeed = (poffset->posy - 1600 + ex->y * 100 - offset->posy) / 4;
    SetJumpControl(offset);
}

int GetBattleTimeStopFlag(int padid);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSKR_Act_LH2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    COLLISION *ex;

    poffset = offset->parentWork;
    ex = Get_GCLExNo(poffset, 0);
    SetChangeFrameAnime(offset, offset->actno);
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (poffset->Mark == 2) {
        SetActNo(offset, 1);
        if (poffset->ply->PersonalWork[0].l == 3) {
            poffset->ply->PersonalWork[0].l = 1;
        }
        return;
    }
    if (poffset->actno != 0x50 && poffset->actno != 0x51) {
        SetActNo(offset, 0);
        if (poffset->ply->PersonalWork[0].l == 3) {
            poffset->ply->PersonalWork[0].l = 0;
        }
        return;
    }
    if (poffset->dirflag) {
        offset->posx = poffset->posx - ex->x * 100;
    } else {
        offset->posx = poffset->posx + ex->x * 100;
    }
    offset->posy = poffset->posy + ex->y * 100;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetBattleTimeStopFlag(int padid);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
extern int BattleStatus;

void TSKR_Act_Atack1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    if (SetChangeFrameAnime(offset, offset->actno)) {
        int y = offset->posy + 14000;

        offset->xspeed = (eoffset->posx - offset->posx) / 8;
        offset->yspeed = (eoffset->posy - y) / 8;
        if (eoffset->posx < offset->posx) {
            offset->dirflag = 0;
        } else {
            offset->dirflag = 1;
        }
    }
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 1);
    }
    if (offset->dirflag) {
        if (eoffset->posx < offset->posx) {
            SetActNoNext(offset, 1);
        }
    } else {
        if (offset->posx < eoffset->posx) {
            SetActNoNext(offset, 1);
        }
    }
    if (BattleStatus != 2) {
        SetActNo(offset, 1);
    }
    SetJumpControl(offset);
    EndtoAction(offset, 1);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetBattleTimeStopFlag(int padid);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
extern int BattleStatus;

void TSKR_Act_Atack2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->xspeed = (eoffset->posx - offset->posx) / 8;
        if (eoffset->posx < offset->posx) {
            offset->dirflag = 0;
        } else {
            offset->dirflag = 1;
        }
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 1);
    }
    if (offset->dirflag) {
        if (eoffset->posx < offset->posx) {
            SetActNoNext(offset, 1);
        }
    } else {
        if (offset->posx < eoffset->posx) {
            SetActNoNext(offset, 1);
        }
    }
    if (BattleStatus != 2) {
        SetActNo(offset, 1);
    }
    SetJumpControl(offset);
    EndtoAction(offset, 1);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetBattleTimeStopFlag(int padid);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;
extern int BattleStatus;

void TSKR_Act_Atack4a(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    CHARACTER_WORK *eoffset;

    poffset = offset->parentWork;
    eoffset = GetEnemyPlayer(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (g_info.boss[poffset->padid] == 1) {
        if (offset->dirflag) {
            offset->xspeed = 3200;
        } else {
            offset->xspeed = -3200;
        }
    }
    if (g_info.boss[poffset->padid] == 1) {
        if (offset->dirflag) {
            if (eoffset->posx - 3200 < offset->posx) {
                SetActNoNext(offset, 0xC);
                offset->posx = eoffset->posx - 3200;
            }
        } else {
            if (offset->posx < eoffset->posx + 3200) {
                SetActNoNext(offset, 0xC);
                offset->posx = eoffset->posx + 3200;
            }
        }
    } else {
        if (offset->dirflag) {
            if (eoffset->posx < offset->posx) {
                SetActNoNext(offset, 0xC);
            }
        } else {
            if (offset->posx < eoffset->posx) {
                SetActNoNext(offset, 0xC);
            }
        }
    }
    if (BattleStatus != 2) {
        SetActNo(offset, 1);
    }
    SetJumpControl(offset);
    EndtoAction(offset, 0xC);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetBattleTimeStopFlag(int padid);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
extern int BattleStatus;

void TSKR_Act_Atack4b(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 1);
    }
    if (BattleStatus != 2) {
        SetActNo(offset, 1);
    }
    SetJumpControl(offset);
    EndtoAction(offset, 1);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int GetBattleTimeStopFlag(int padid);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);
extern int BattleStatus;

void TSKR_Act_Atack3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    if (offset->attackst & 0x1000) {
        SetActNoNext(offset, 1);
    }
    if (BattleStatus != 2) {
        SetActNo(offset, 1);
    }
    SetJumpControl(offset);
    EndtoAction(offset, 1);
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);

void TSKR_Act_Leave(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    OutSideEliminate(offset);
    SetJumpControl(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
float nlSin(short unsigned int r);

void TSKR_Act_Win(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->RHitBackx = offset->posy;
        offset->Localid = 0;
        poffset->ply->PersonalWork[0].l = 9;
    }
    offset->Localid += 0x200;
    offset->posy = nlSin(offset->Localid) * 20.0f * 100.0f + offset->RHitBackx;
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int PlayerDamage(CHARACTER_WORK *offset, int dam, TACTHEADER *ahp);
void ReqCmnVoice_Damage(CHARACTER_WORK *offset);
void ReqHitSE(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetFlash(char time, short unsigned int flag);
extern short unsigned int KRDmpNoTb[];

int TSKRDamInt_Die(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    CREATE_ARG c_arg;
    TACTHEADER die;

    SetActNoNext(offset, KRDmpNoTb[offset->idno]);
    ReqHitSE(offset, header->flag, header->lvflag);
    ReqCmnVoice_Damage(offset);
    c_arg.flag = 0x25;
    c_arg.x = offset->DamColliPos[cnt].x;
    c_arg.y = offset->DamColliPos[cnt].y;
    c_arg.actno = 9;
    c_arg.Localid = 0x56;
    EffectCreate(offset, 0x67, &c_arg);
    offset->DamColliFlag = 0;
    die.flag = 0x06000000;
    PlayerDamage(offset, 1, &die);
    SetFlash(2, 0x901);
    return 1;
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TSKRDamInt_Die(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void TSKR_Act_Itigeki(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;
    CHARACTER_WORK *bead = 0;
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);
    COLLISION *ex = 0;
    int beadx;
    int beady;
    int exy;

    if (poffset) {
        bead = SearchEffectWorkActno(poffset, 0x50, 9);
    }
    beadx = 0;
    beady = 0;
    exy = 0;
    if (bead) {
        ex = Get_GCLExNo(bead, 1);
        if (ex) {
            exy = ex->y;
        }
        beadx = bead->posx;
        beady = bead->posy;
    }
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->HitParam) {
            offset->HitParam->DamInt = TSKRDamInt_Die;
        }
    }
    if (ex) {
        if (offset->dirflag) {
            offset->posx = beadx - ex->x * 100;
        } else {
            offset->posx = ex->x * 100 + beadx;
        }
    }
    offset->posy = exy * 100 + beady;
    if (offset->Mark == 0) {
        if (eoffset) {
            eoffset->actst |= 0x4000;
        }
        if (poffset) {
            poffset->actst |= 0x4000;
        }
    } else {
        if (eoffset) {
            eoffset->actst &= ~0x4000;
        }
        if (poffset) {
            poffset->actst &= ~0x4000;
        }
    }
    if (EndtoAction(offset, 1) && poffset) {
        poffset->ply->PersonalWork[0].l = 1;
    }
    SetAnten(offset, 2);
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void TSSL_Act1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;
    CHARACTER_WORK *hand_b = SearchEffectWorkActno(poffset, 0x4F, 0xE);
    CHARACTER_WORK *hand_c = SearchEffectWorkActno(poffset, 0x4F, 0xF);
    CHARACTER_WORK *hand_d = SearchEffectWorkActno(poffset, 0x4F, 0x10);

    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->dirflag) {
            if (offset->posx >= 72000) {
                offset->posx = 72000;
            }
        } else {
            if (offset->posx <= -72000) {
                offset->posx = -72000;
            }
        }
    }
    if (hand_b) {
        hand_b->priority = offset->priority - 1;
        hand_b->posx = offset->posx;
    }
    if (hand_c) {
        hand_c->priority = offset->priority - 2;
        hand_c->posx = offset->posx;
    }
    if (hand_d) {
        hand_d->priority = offset->priority - 3;
        hand_d->posx = offset->posx;
    }
    SetObjFlag(offset, 4);
    EndtoEliminate(offset);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);
extern int BattleStatus;

void TSSL_Act2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);
    CHARACTER_WORK *poffset = offset->parentWork;
    CHARACTER_WORK *hand_b = SearchEffectWorkActno(poffset, 0x4F, 0x11);
    CHARACTER_WORK *hand_c = SearchEffectWorkActno(poffset, 0x4F, 0x12);
    CHARACTER_WORK *hand_d = SearchEffectWorkActno(poffset, 0x4F, 0x13);

    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (BattleStatus != 3) {
            SetActNo(eoffset, 0);
        }
        eoffset->posy = 0;
    } else if (eoffset->actst & 0x4120) {
        SetActNoNext(offset, 7);
        if (hand_b) {
            SetActNoNext(hand_b, 0x14);
        }
        if (hand_c) {
            SetActNoNext(hand_c, 0x15);
        }
        if (hand_d) {
            SetActNoNext(hand_d, 0x16);
        }
    }
    eoffset->ply->TShand |= 1;
    eoffset->posx = offset->posx;
    eoffset->xspeed = 0;
    eoffset->posy = offset->posy;
    SetObjFlag(offset, 4);
    if (hand_b) {
        hand_b->priority = offset->priority - 1;
    }
    if (hand_c) {
        hand_c->priority = offset->priority - 2;
    }
    if (hand_d) {
        hand_d->priority = offset->priority - 3;
    }
    if (EndtoAction(offset, 7)) {
        if (hand_b) {
            SetActNoNext(hand_b, 0x14);
        }
        if (hand_c) {
            SetActNoNext(hand_c, 0x15);
        }
        if (hand_d) {
            SetActNoNext(hand_d, 0x16);
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSSL_Act2B(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->dirflag) {
            if (offset->posx >= 72000) {
                offset->posx = 72000;
            }
        } else {
            if (offset->posx <= -72000) {
                offset->posx = -72000;
            }
        }
    }
    EndtoEliminate(offset);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void TSEX_Act_Itigeki(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);
    COLLISION *ex = Get_GCLExNo(offset, 0);

    if (!SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->dirflag) {
            eoffset->posx = offset->posx - ex->x * 100;
        } else {
            eoffset->posx = offset->posx + ex->x * 100;
        }
        eoffset->posy = offset->posy + ex->y * 100;
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int TSDamInt_PoisonHit(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void TSKK_Kakusei(CHARACTER_WORK *offset) {
    CHARACTER_WORK *koffset = SearchObjectWork(offset, 0x52);

    if (koffset) {
        koffset->HitStopTime = 10;
    }
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->HitParam) {
        offset->HitParam->DamInt = TSDamInt_PoisonHit;
    }
    EndtoEliminate(offset);
}

#define ABSV(a) (((a) < 0) ? -(a) : (a))
int GetBattleTimeStopFlag(int padid);
void SetJumpControl(CHARACTER_WORK *offset);
extern GAME_INFOMATION g_info;

void TSKR_Act_Chase(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    int dx;
    int dy;

    poffset = offset->parentWork;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->actst |= 8;
    }
    if (GetBattleTimeStopFlag(offset->padid == 0)) {
        SetActNoNext(offset, 0xB);
        return;
    }
    offset->RHitBackx = poffset->posy - 30000;
    if (offset->dirflag) {
        offset->HitBackx = poffset->posx - 12000;
    } else {
        offset->HitBackx = poffset->posx + 12000;
    }
    offset->yspeed = (offset->RHitBackx - offset->posy) / 14;
    offset->xspeed = (offset->HitBackx - offset->posx) / 16;
    offset->dirflag = poffset->dirflag;
    if (poffset->ply->PersonalWork[0].l == 5) {
        offset->Localid++;
        if (g_info.boss[poffset->padid] == 1) {
            if (offset->Localid >= 30) {
                SetActNoNext(offset, 0);
                poffset->ply->PersonalWork[0].l = 0;
                return;
            }
        } else if (offset->Localid >= 180) {
            SetActNoNext(offset, 0);
            poffset->ply->PersonalWork[0].l = 0;
            return;
        }
    } else if (poffset->ply->PersonalWork[0].l != 8 && poffset->ply->PersonalWork[0].l != 1) {
        if ((unsigned int)poffset->ply->PersonalWork[0].l < 13) {
            SetActNoNext(offset, poffset->ply->PersonalWork[0].l);
        } else {
            debugf("TSKR Danger 2\n");
        }
    }

    if (ABSV(offset->HitBackx - offset->posx) < 500) {

        if (ABSV(offset->RHitBackx - offset->posy) < 500) {
            offset->posx = offset->HitBackx;
            offset->posy = offset->RHitBackx;
            SetActNoNext(offset, 0);
            if (poffset->ply->PersonalWork[0].l == 1 || poffset->ply->PersonalWork[0].l == 5) {
                poffset->ply->PersonalWork[0].l = 0;
            }
        }
    }
    SetJumpControl(offset);
}
