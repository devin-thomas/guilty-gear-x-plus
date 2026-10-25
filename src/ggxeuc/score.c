#include "generated/score.h"

extern GAME_INFOMATION g_info;

void ClearScore(int side)
{
    g_info.comboflagh[side] = 0;
    g_info.comboflagk[side] = 0;
    g_info.comboflagn[side] = 0;
    g_info.combototal[side] = 0;
    g_info.score[side] = 0;
    g_info.scorerate[side] = 0;
}

extern GAME_INFOMATION g_info;

int Score_GetScorerate(int side)
{
    if (g_info.MedalMode[side]) {
        return g_info.scorerate[side] / 1000 + 1;
    }
    return 1;
}

int g_round(int val, int max, int min);
extern GAME_INFOMATION g_info;

void Score_AddScorerate(int side, int level)
{
    static const short valtb[] = { 10, 20, 50, 100, 250, 1000 };
    static const short timetb[] = { 30, 60, 300, 600, 1200, 2000 };
    short time;

    time = timetb[level];
    g_info.scorerate[side] = g_round(g_info.scorerate[side] + valtb[level], 9999, 0);
    g_info.scoreratetime[side] = g_round(g_info.scoreratetime[side] + time, 1200, 0);
}

int g_roundmin(int val, int min);
extern GAME_INFOMATION g_info;

void Score_ExecuteScorerate(int side)
{
    if (g_info.scoreratetime[side] > 0) {
        g_info.scoreratetime[side]--;
    } else {
        g_info.scorerate[side] = g_roundmin(g_info.scorerate[side] - 1, 0);
        g_info.scoreratetime[side] = 1;
    }
}

void InitScore(void)
{
    ClearScore(0);
    ClearScore(1);
}

extern int temphp[2];
extern int temptime[2];
extern int tempperfect[2];

void InitTempScore(void)
{
    temptime[0] = 0;
    temptime[1] = 0;
    temphp[0] = 0;
    temphp[1] = 0;
    tempperfect[0] = 0;
    tempperfect[1] = 0;
}

extern GAME_INFOMATION g_info;

void AddScoreMain(int side, int i)
{
    g_info.score[side] += i;
    CheckScoreCounterStop(side);
}

void AddScore(int side, int i)
{
    AddScoreMain(side, i * Score_GetScorerate(side));
}

extern GAME_INFOMATION g_info;

void CheckScoreCounterStop(int side)
{
    if (g_info.score[side] > 1000000000) {
        g_info.score[side] = g_info.score[side] % 100 + 999999900;
    }
}

int Score_GetScoreByDamage(int dam)
{
    int point;
    unsigned char scoretransfatb[24] = {
        10, 1, 15, 2, 20, 3, 26, 5, 32, 7, 40, 10,
        50, 14, 60, 18, 70, 23, 90, 30, 255, 50, 0, 0
    };
    unsigned char *ptr;

    point = 0;
    for (ptr = scoretransfatb; *ptr; ptr += 2) {
        if (*ptr >= dam) {
            point = ptr[1];
            break;
        }
    }
    return point;
}

int Score_AddByAttack(int side, int dam)
{
    int point;

    point = Score_GetScoreByDamage(dam);
    AddScore(side, point * 100);
    CheckScoreCounterStop(side);
    return point;
}

extern GAME_INFOMATION g_info;

int Score_GetComboBase(int side, int flag, int point)
{
    GAME_INFOMATION *p;

    p = &g_info;
    if (flag & 0x4000000) {
        point = 20;
        if (p->comboflagk[side] < 2) {
            point = (p->comboflagk[side] > 0) ? 60 : 200;
        }
        p->comboflagk[side]++;
    } else if (flag & 4) {
        if (p->comboflagh[side] >= 2) {
            point = point * 1.0;
        } else if (p->comboflagh[side] > 0) {
            point = (int)(point * 1.5) + 1;
        } else {
            point = (int)(point * 2.0) + 5;
        }
        p->comboflagh[side]++;
    } else {
        if (p->comboflagn[side] >= 4) {
            point = point * 0.5;
        } else if (p->comboflagn[side] >= 3) {
            point = point * 0.7;
        } else if (p->comboflagn[side] >= 2) {
            point = point * 0.8;
        } else if (p->comboflagn[side] > 0) {
            point = point * 0.9;
        }
        p->comboflagn[side]++;
    }
    return point;
}

extern GAME_INFOMATION g_info;

void Score_AddByCombo(int side, int dam, int combo, unsigned int flag)
{
    GAME_INFOMATION *p;
    int point;

    p = &g_info;
    point = Score_AddByAttack(side, dam);
    if (combo < 2) {
        p->comboflagh[side] = 0;
        p->comboflagk[side] = 0;
        p->comboflagn[side] = 0;
        p->combototal[side] = 0;
    }
    point = Score_GetComboBase(side, flag, point) * 100;
    if (point < 100) {
        point = 100;
    }
    point *= Score_GetScorerate(side);
    AddScoreMain(side, point);
    p->combototal[side] += point;
    CheckScoreCounterStop(side);
    if (combo >= 2) {
        p->combodisptime[side] = 60;
    }
}

void Score_AddByKunsyou(int side, int level)
{
    static int tb[] = { 100, 500, 1000, 3000, 10000, 50000 };

    AddScore(side, tb[level]);
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int IsBattleDecision(void);
int IsDemoBattle(void);
int g_round(int val, int max, int min);
extern GAME_INFOMATION g_info;
extern int KunsyouLiveCount;

void MedalPut(CHARACTER_WORK *offset, COLLIPOS *colli, TACTHEADER *header)
{
    CHARACTER_WORK *eoffset;
    CREATE_ARG c_arg;
    int rank;
    int actno;
    int side;

    eoffset = GetEnemyPlayer(offset);
    side = offset->padid;
    if (!offset->ply) {
        return;
    }
    if (IsDemoBattle()) {
        return;
    }
    if (!(g_info.mode & 0x100) && !g_info.isCPU[offset->padid]) {
        return;
    }
    if (!g_info.MedalMode[!offset->padid]) {
        return;
    }
    if (IsBattleDecision()) {
        return;
    }
    if (KunsyouLiveCount >= 11) {
        return;
    }
    c_arg.flag = 1;
    if (header->flag & 0x4000000) {
        rank = header->damage / 4;
        if (g_info.comboflagk[side] >= 2) {
            rank = rank * 0.25 + 3.0;
        } else if (g_info.comboflagk[side] > 0) {
            rank = rank * 0.75 + 10.0;
        } else {
            rank = rank + 25.0;
        }
    } else if (header->flag & 4) {
        rank = 3;
        if (g_info.comboflagh[side] < 2) {
            rank = (g_info.comboflagh[side] > 0) ? 5 : 10;
        }
        rank += header->damage / 5;
    } else {
        if (g_info.comboflagn[side] >= 5) {
            rank = 3;
        } else if (g_info.comboflagn[side] >= 4) {
            rank = 5;
        } else if (g_info.comboflagn[side] >= 3) {
            rank = 10;
        } else if (g_info.comboflagn[side] >= 2) {
            rank = 5;
        } else {
            rank = 0;
        }
        rank += header->damage / 6;
    }
    if (offset->ply->DownFlag & 4) {
        rank += 20;
    }
    if (offset->ply->HitCount < 3) {
    } else if (offset->ply->HitCount == 3) {
        rank += 10;
    } else if (offset->ply->HitCount < 6) {
        rank += 2;
    } else if (offset->ply->HitCount == 6) {
        rank += 12;
    } else if (offset->ply->HitCount < 10) {
        rank += 3;
    } else if (offset->ply->HitCount == 10) {
        rank += 15;
    } else if (offset->ply->HitCount < 16) {
        rank += 4;
    } else if (offset->ply->HitCount == 16) {
        rank += 18;
    } else {
        rank += 5;
    }
    actno = g_round(rank / 10, 5, 0);
    if (colli) {
        c_arg.x = colli->x;
        c_arg.y = colli->y;
        EffectCreate_ActNo(eoffset, 0x62, &c_arg, actno);
    } else {
        c_arg.x = offset->posx;
        c_arg.y = offset->posy - 15000;
        EffectCreate_ActNo(eoffset, 0x62, &c_arg, actno);
    }
}
