#include "generated/jye_act.h"

void JYEF_ColliCheck(CHARACTER_WORK *offset) {
    if (offset->actno == 0xB && offset->HitColliFlag) {
        offset->actst |= 0x80000000;
    }
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYSMAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0xB);
    EndtoAction(offset, 0xC);
}

int EndtoAction(CHARACTER_WORK *offset, int actno);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYSMAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0xC);
    EndtoAction(offset, 0xD);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYSMAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0xD);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetJumpControl(CHARACTER_WORK *offset);

void JYEFAct_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetParentPosEx(CHARACTER_WORK *offset);

void JYEFAct_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    SetParentPosEx(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYEFAct_S(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 0xA)) {
        offset->posx += 0x190;
    }
    if (offset->Localid) {
        offset->Localid = 0;
        offset->posx += 0x320;
    } else {
        offset->Localid = 1;
        offset->posx -= 0x320;
    }
    EndtoEliminate(offset);
}

int IsBattleDecision(void);
void RequestHit(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int cnt);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);

extern int jyacttb[];

int JYAct_KILL_Intrpt(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt) {
    CHARACTER_WORK *eoffset = offset->ply->Enemy;

    if (!IsBattleDecision() && !eoffset->DamColliFlag && eoffset->idno == 0xE && eoffset->actno == 0x7C) {
        offset->actst |= 0x4020;
        offset->DamColliPos[cnt].x -= offset->posx;
        SetActNo(offset, jyacttb[offset->idno] | 0x400000);
        RequestHit(offset, header->flag, header->lvflag, cnt);
        offset->DamColliFlag = 0;
        return 1;
    }
    return 0;
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* JYAct_KILL_Intrpt: no debug record */

int JYAct_KILL_Intrpt(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoff, int cnt);

void JYSMAct_4(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    if (SetChangeFrameAnime(offset, 0xE)) {
        if (offset->HitParam) {
            offset->HitParam->DamInt = JYAct_KILL_Intrpt;
        }
    }
    if (offset->attackst & 0x80000) {
        poffset->ply->MutekiTime = 0x14;
        offset->actst |= 0x80000000;
        return;
    }
    if (offset->attackst & 0x1000) {
        offset->actst |= 0x80000000;
    }
    OutSideEliminate(offset);
    if (offset->actst & 0x80000000) {
        if (poffset->actno == 0x7C) {
            SetActNo(poffset, 0x7E);
        }
    }
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYKLAct(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    offset->dirflag = 0;
    offset->posy = 0;
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JYEFAct_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* D_5C93F0: no debug record */

void JYEFAct_6(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    CREATE_ARG c_arg;
    int i;

    c_arg.flag = 0x100;
    offset->dirflag = 0;
    poffset = offset->parentWork;
    if (!SetChangeFrameAnime(offset, offset->actno) && poffset->actno != 0x7A) {
        offset->actst |= 0x80000000;
    }
    if (poffset->Mark == 1) {
        EffectCreate_ActNo(offset, 0x3F, &c_arg, 7);
        poffset->Mark = 0;
    }
    if (poffset->Mark & 4) {
        short int anmtb[7] = { 0, 9, 0xA, 0x16, 0x17, 0x18, 0x19 };
        COLLISION *ex;

        c_arg.flag = 5;
        for (i = 1; i < 7; i++) {
            ex = Get_GCLExNo(offset, i);
            c_arg.actno = anmtb[i];
            c_arg.x = offset->posx + ex->x * 100;
            c_arg.y = offset->posy + ex->y * 100;
            EffectCreate(offset, 0x3F, &c_arg);
        }
        poffset->Mark &= ~4;
    }
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
int EndtoEliminate(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* D_5C9400: no debug record */

void JYEFAct_7(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    CREATE_ARG c_arg;
    int i;

    c_arg.flag = 0x100;
    offset->dirflag = 0;
    poffset = offset->parentWork;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->priority = poffset->priority - 1;
        offset->parentWork = poffset->parentWork;
    } else if (poffset->actno != 0x7A) {
        offset->actst |= 0x80000000;
    }
    if (poffset->Mark == 2) {
        EffectCreate_ActNo(offset, 0x3F, &c_arg, 8);
        poffset->Mark = 0;
    }
    if (poffset->Mark & 8) {
        COLLISION *ex;
        short int anmtb[8] = { 0, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20 };

        c_arg.flag = 5;
        for (i = 1; i < 8; i++) {
            ex = Get_GCLExNo(offset, i);
            c_arg.x = offset->posx + ex->x * 100;
            c_arg.y = offset->posy + ex->y * 100;
            c_arg.actno = anmtb[i];
            EffectCreate(offset, 0x3F, &c_arg);
        }
        poffset->Mark &= ~8;
    }
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int EndtoEliminate(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
/* D_5C9410: no debug record */

void JYEFAct_8(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    CREATE_ARG c_arg;
    int i;

    c_arg.flag = 0x100;
    offset->dirflag = 0;
    poffset = offset->parentWork;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->priority = poffset->priority - 1;
        offset->parentWork = poffset->parentWork;
    } else if (poffset->actno != 0x7A) {
        offset->actst |= 0x80000000;
    }
    if (poffset->Mark & 0x10) {
        COLLISION *ex;
        short int anmtb[7] = { 0, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26 };

        c_arg.flag = 5;
        for (i = 1; i < 6; i++) {
            ex = Get_GCLExNo(offset, i);
            c_arg.x = offset->posx + ex->x * 100;
            c_arg.y = offset->posy + ex->y * 100;
            c_arg.actno = anmtb[i];
            EffectCreate(offset, 0x3F, &c_arg);
        }
        poffset->Mark &= ~0x10;
    }
    EndtoEliminate(offset);
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetObjFlag(CHARACTER_WORK *offset, unsigned char flag);

void JYEFAct_Mist(CHARACTER_WORK *offset) {
    if (!SetChangeFrameAnime(offset, offset->actno) && offset->ActTimer >= offset->Mark) {
        offset->actst |= 0x80000000;
    }
    OutSideEliminate(offset);
    SetObjFlag(offset, 1);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYEFAct_Iai2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    SetChangeFrameAnime(offset, offset->actno);
    if (poffset->dirflag) {
        offset->posx = poffset->posx - 0x1964;
    } else {
        offset->posx = poffset->posx + 0x1964;
    }
    offset->posy = poffset->posy - 0x4650;
    if (poffset->actno != 0x5A && poffset->actno != 0x61 && poffset->actno != 0x5B) {
        offset->actst |= 0x80000000;
    }
    if (poffset->ply->PersonalWork[0].w[0] >= 200) {
        offset->actst |= 0x80000000;
    }
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JYEFAct_Iai3(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    SetChangeFrameAnime(offset, offset->actno);
    if (poffset->dirflag) {
        offset->posx = poffset->posx - 0x1964;
    } else {
        offset->posx = poffset->posx + 0x1964;
    }
    offset->posy = poffset->posy - 0x4650;
    if (poffset->actno != 0x5A && poffset->actno != 0x61 && poffset->actno != 0x5B) {
        offset->actst |= 0x80000000;
    }
}

void FontDraw(char *str, int x, int y, float z, int alfa);
char *G_sprintf_arg1(char *str, int arg1);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int JYGetIaiLevel(CHARACTER_WORK *offset);
void SetBlendingMode(short int mode);
/* D_5C9420: no debug record */

void JYEFBARDraw(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = GetParentPlayer(offset);
    int x = offset->posx / 100;
    int y = offset->posy / 100;

    SetBlendingMode(offset->transmode);
    FontDraw(G_sprintf_arg1("LEVEL %d", JYGetIaiLevel(poffset) + 1), x - 42, y, offset->priority, 0xFF);
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
/* JYEFBARDraw: no debug record */

void JYEFBARDraw(CHARACTER_WORK *offset);

void JYEFAct_Bar(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = GetParentPlayer(offset);
    int a;

    SetChangeFrameAnimeSameActno(offset);
    if (offset->ActTimer > 60) {
        offset->actst |= 0x80000000;
    }
    if (offset->Localid != poffset->ply->PersonalWork[0].w[0]) {
        offset->ActTimer = 0;
        offset->Localid = poffset->ply->PersonalWork[0].w[0];
    }
    offset->posy = 0xA410;
    offset->graphicfunc = JYEFBARDraw;
    offset->SpriteFlag |= 1;
    a = offset->padid ? 1 : -1;
    offset->posx = (a * 230 + 320) * 100;
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
void GraphicExecute(CHARACTER_WORK *offset);

extern GAME_INFOMATION g_info;

void JYEFLeftCoinDraw(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = GetParentPlayer(offset);
    int posx = offset->posx;
    int priority = offset->priority;
    int cnt;

    if (!(g_info.BattleControlFlag & 2)) {
        cnt = 8 - poffset->ply->PersonalWork[1].b[0];
        while (cnt--) {
            GraphicExecute(offset);
            if (offset->padid) {
                offset->posx -= 0x320;
            } else {
                offset->posx += 0x320;
            }
            offset->priority--;
        }
        offset->posx = posx;
        offset->priority = priority;
    }
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
/* JYEFLeftCoinDraw: no debug record */

extern USER_SETTING userinfo;
void JYEFLeftCoinDraw(CHARACTER_WORK *offset);

void JYEFAct_LeftCoin(CHARACTER_WORK *offset) {
    int off = userinfo.offs_cockpit_lower;
    int a;

    SetChangeFrameAnimeSameActno(offset);
    offset->graphicfunc = JYEFLeftCoinDraw;
    offset->posy = (off + 440) * 100;
    offset->SpriteFlag |= 1;
    a = offset->padid ? 1 : -1;
    offset->posx = (a * 280 + 320) * 100;
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JYCOAct_Create(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    OutSideEliminate(offset);
    SetLandBreakActno(offset, 0x10001);
}

void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void JYCOAct_Bound(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    OutSideEliminate(offset);
    SetLandBreakActno(offset, 0x10002);
}

int EndtoEliminate(CHARACTER_WORK *offset);
void OutSideEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JYCOAct_Dead(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    OutSideEliminate(offset);
    EndtoEliminate(offset);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int g_round(int val, int max, int min);

void JYCO_ColliCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    if (offset->actno == 0) {
        if (offset->attackst & 0x80000) {
            poffset = GetParentPlayer(offset);
            poffset->ply->PersonalWork[0].w[0] = g_round(poffset->ply->PersonalWork[0].w[0] + 1, 2, 0);
            if (!SearchEffectWorkActno(poffset, 0x3F, 0x29)) {
                EffectCreate_ActNo(poffset, 0x3F, 0, 0x29);
            }
        }
        if (*(unsigned int *)&offset->ColliFlag & 0xFFFF00) {
            SetActNo(offset, 0x10001);
            offset->xspeed = -offset->xspeed;
        }
    }
}
