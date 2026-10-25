#include "generated/fight.h"
#include "generated/fight.h"

extern CHARACTER_WORK *PlayerWork;
extern GAME_INFOMATION g_info;
extern char winflg[2][8];
extern int BattleTime;
extern int final;
extern unsigned int ResultFlag;
extern int EnterFightJob;
extern int EnterFightCount;

/* TU statics (0x5cddc4..0x5cddcc, after all function statics in .sbss); declared extern
   until every function static of the TU is in place, then they become static again. */
extern unsigned int FightThreadFlag;
extern int BattleTimeStopFlag;
extern MYDES_WORK *myp;
extern int BattleStatus;
extern int AfterBattleCounter;
extern int RTimeLimitTb[];
extern USER_SETTING userinfo;
extern TRAINING_SETTING *tr_info;
extern SURVIVAL_INFOMATION *sv_info;
extern CHARANGE_CTRL *gl_info;
extern short int UseCharacter1p2p[];
extern story_t story;
extern int StageNo;
extern int GlobalHitStop;
extern NL_SPR_ARG nlsp[];
extern FIGHT_CONTROL rocktb[];
extern FIGHT_CONTROL dueltb[];
extern FIGHT_CONTROL duel1_8[];
extern FIGHT_CONTROL fduel[];
extern FIGHT_CONTROL heaven[];
extern FIGHT_CONTROL Winnertb[];
extern FIGHT_CONTROL perfecttb[];
extern FIGHT_CONTROL doubleko[];
extern FIGHT_CONTROL drawtb[];
extern FIGHT_CONTROL losetb[];
extern FIGHT_CONTROL destroytb[];
extern FIGHT_CONTROL maydstroy[];
extern FIGHT_CONTROL timeup[];
extern FIGHT_CONTROL ranyu[];
extern int PLAYER_HITPOINTMAX;
extern SYS_COND sc;

void Cmd_AddX(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].x += p->data1;
}

void Cmd_AddY(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].y += p->data1;
}

void Cmd_AddO(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    unsigned int r;
    unsigned int g;
    unsigned int b;

    sp = &n[p->id];
    r = ((u_char *)&sp->offset_color)[2] + p->data2;
    g = ((u_char *)&sp->offset_color)[1] + p->data2;
    b = ((u_char *)&sp->offset_color)[0] + p->data2;
    sp->offset_color = (r << 16) | (g << 8) | b;
}

void Cmd_SetB(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].base_color = p->data2;
}

void Cmd_SetO(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].offset_color = p->data2;
}

void Cmd_SetY(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].y = p->data1;
}

void Cmd_SetX(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].x = p->data1;
}

void Cmd_SetZ(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].z = p->data1;
}

void Cmd_SetXY(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].x = p->data1;
    n[p->id].y = p->data2;
}

void Cmd_AddTrnsl(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].trnsl += p->data1;
}

void Cmd_SetTrnsl(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].trnsl = p->data1;
}

void Cmd_SetZM(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_x = p->data1;
    n[p->id].zm_y = p->data1;
}

void Cmd_SetZMX(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_x = p->data1;
}

void Cmd_SetZMY(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_y = p->data1;
}

void Cmd_AddZM(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_x += p->data1;
    n[p->id].zm_y += p->data1;
}

void Cmd_AddZMY(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_y += p->data1;
}

void Cmd_AddZMX(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_x += p->data1;
}

void Cmd_AddZX(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_x += p->data1;
}

void Cmd_AddZY(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].zm_y += p->data1;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_AddU1(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->zm_x = sp->u1 = sp->u1 + p->data1 / x;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_AddV1(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->zm_y = sp->v1 = sp->v1 + p->data1 / y;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_SetU1(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->zm_x = sp->u1 = p->data1 / x;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_SetV1(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->zm_y = sp->v1 = p->data1 / y;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_AddU0(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->u0 += p->data1 / x;
    sp->zm_x = 1.0f - sp->u0;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_AddV0(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->v0 += p->data1 / y;
    sp->zm_y = 1.0f - sp->v0;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_SetU0(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->u0 = p->data1 / x;
    sp->zm_x = 1.0f - sp->u0;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_SetV0(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->v0 = p->data1 / y;
    sp->zm_y = 1.0f - sp->v0;
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_MoveDes(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    int x;
    int y;

    sp = &n[p->id];
    PXF_GetSize(sp->sprno, &x, &y);
    sp->y += p->data1;
    if (!(p->data2 < sp->y)) {
        sp->zm_x = 0.25f;
        sp->v1 = ((p->data2 - sp->y) * 4.0f) / y;
        sp->zm_y = sp->v1 * 0.25f;
    }
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_MoveU0(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    UVWH *uvz;

    sp = &n[p->id];
    uvz = p->data3;
    sp->x += p->data1;
    SetUVZ(sp, uvz->u + (uvz->w - (sp->x - p->data2)), uvz->v, sp->x - p->data2, uvz->h);
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_MoveV0(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    UVWH *uvz;

    sp = &n[p->id];
    uvz = p->data3;
    sp->y += p->data1;
    SetUVZ(sp, uvz->u, uvz->v + (uvz->h - (sp->y - p->data2)), uvz->w, sp->y - p->data2);
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_MoveU1(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    UVWH *uvz;

    sp = &n[p->id];
    uvz = p->data3;
    sp->x += p->data1;
    SetUVZ(sp, uvz->u, uvz->v, p->data2 - sp->x, uvz->h);
}

void PXF_GetSize(int id, int *x, int *y);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_MoveV1(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    UVWH *uvz;

    sp = &n[p->id];
    uvz = p->data3;
    sp->y += p->data1;
    SetUVZ(sp, uvz->u, uvz->v, uvz->w, p->data2 - sp->y);
}

void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);

void Cmd_SetUVZ(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    SetUVZ(&n[p->id], ((UVWH *)p->data3)->u, ((UVWH *)p->data3)->v, ((UVWH *)p->data3)->w, ((UVWH *)p->data3)->h);
}

void Cmd_SetAttr(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    n[p->id].attr = p->data2;
}

void trapa(int t);

void Cmd_Trap(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    trapa(0x99);
}

int Random(void);

void Cmd_MyDesInit(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    MYDES_WORK *myw;

    myw = &myp[p->id];
    myw->target = Random() & 0x1F;
    myw->flag = 0;
    myw->count = 0;
}

int Random(void);

void Cmd_MyDesExe(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    MYDES_WORK *myw;

    myw = &myp[p->id];
    sp = &n[p->id];
    if (myw->flag == 0 || myw->flag == 3) {
        sp->y += 0.5f;
    } else {
        sp->y -= 0.5f;
    }
    myw->count++;
    if (myw->count == myw->target) {
        if (myw->flag == 3) {
            myw->flag = 0;
            myw->target = Random() & 0x1F;
        } else {
            myw->flag++;
        }
        myw->count = 0;
    }
}

int sound_request(int no);

void Cmd_ReqSe(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    sound_request(p->id);
}

int sound_request(int no);

void Cmd_ReqSeNum(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    sound_request(g_info.Round + 0x40);
}

int GetWinnerSide(void);
int privatese_request(CHARACTER_WORK *offset, int no);

void Cmd_ReqName(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    int namenotb[19] = { 0, 7, 6, 6, 4, 5, 8, 7, 0, 6, 8, 8, 9, 7, 6, 7, 8, 5, 5 };
    CHARACTER_WORK *offset;

    offset = &PlayerWork[GetWinnerSide()];
    privatese_request(offset, namenotb[offset->idno]);
}

void Cmd_DebugNo(FIGHT_CONTROL *p, NL_SPR_ARG *n) {
}

void FightControl_Execute(FIGHT_CONTROL *tablep, NL_SPR_ARG *sp, int time)
{
    FIGHT_CONTROL *p;

    for (p = tablep; p->id != 0xFFFFFFFF; p++) {
        if (p->proc != 0 && time >= p->time && time < p->end) {
            ((void (*)(FIGHT_CONTROL *, NL_SPR_ARG *))p->proc)(p, sp);
        }
    }
}

void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send)
{
    NL_SPR_ARG *to;

    to = &send[count];
    to->sprno = arg->sprno;
    to->x = arg->x;
    to->y = arg->y;
    to->z = arg->z;
    to->zm_x = arg->zm_x;
    to->zm_y = arg->zm_y;
    to->u0 = arg->u0;
    to->v0 = arg->v0;
    to->u1 = arg->u1;
    to->v1 = arg->v1;
    to->ang = arg->ang;
    to->trnsl = arg->trnsl;
    to->listType = arg->listType;
    to->attr = arg->attr;
    to->base_color = arg->base_color;
    to->offset_color = arg->offset_color;
}

void SprArgSet(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send)
{
    int i;

    for (i = 0; i < count; i++) {
        SprArgSetOne(i, &arg[i], send);
    }
}

void SetBattleTimeStopFlag(int padid)
{
    BattleTimeStopFlag |= 1 << padid;
}

int GetBattleTimeStopFlag(int padid)
{
    if (padid < 0) {
        return BattleTimeStopFlag;
    }
    return BattleTimeStopFlag & (1 << padid);
}

void InitTempScore(void);

void ResultInitial(void)
{
    int i;
    int j;

    final = 0;
    g_info.Round = 0;
    ResultFlag = 0;
    BattleTimeStopFlag = 0;
    InitTempScore();
    for (i = 0; i < 2; i++) {
        for (j = 7; j >= 0; j--) {
            winflg[i][j] = -1;
        }
    }
}

CHARACTER_WORK *CpObjCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int Cr_IsPlaying(int side);
void Diagram_SetResult(char *src, int win, int lose);

void RequestResult(int id, int flag)
{
    int i;
    int j;
    CREATE_ARG c_arg;

    i = (id != 0);
    if (g_info.mode & 0x100) {
        return;
    }
    for (j = 0; j < 8; j++) {
        if (winflg[i][j] == -1) {
            winflg[i][j] = flag;
            if (Cr_IsPlaying(0) && Cr_IsPlaying(1)) {
                Diagram_SetResult(userinfo.UserBackUp.diagram, PlayerWork[i].idno, PlayerWork[i ? 0 : 1].idno);
            }
            j++;
            c_arg.flag = 0x20;
            c_arg.Localid = j;
            if (i) {
                c_arg.Localid = -j;
            }
            CpObjCreate(0, 0x7E, &c_arg);
            return;
        }
    }
}

CHARACTER_WORK *CpObjCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int GetCPUCharacterFlag(void);

void RequestAiuchi(void)
{
    int i;
    int j;
    int point[2];
    int max;

    if (GetCPUCharacterFlag() == 2) {
        max = g_info.VSRoundMax;
    } else {
        max = g_info.CPURoundMax;
    }
    if (g_info.mode & 0xE600) {
        max = 1;
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 8; j++) {
            if (winflg[i][j] == -1) {
                point[i] = j;
                j = 8;
            }
        }
    }
    if (point[0] + 1 >= max && point[1] + 1 >= max) {
        if (final == 0) {
            final = 1;
        } else {
            final = 2;
        }
        if (g_info.mode & 0x100) {
            final = 0;
        }
        if (g_info.mode & 0x200) {
            final = 2;
            g_info.WinnerSide = sv_info->ply == 0;
        }
        if (g_info.mode & 0x8000) {
            final = 2;
            g_info.WinnerSide = gl_info->ply == 0;
        }
        if (g_info.mode & 0x2000) {
            final = 2;
            g_info.WinnerSide = story.self_side == 0;
        }
    } else if (!(g_info.mode & 0x100)) {
        for (i = 0; i < 2; i++) {
            j = point[i];
            if (j + 1 < max) {
                CREATE_ARG c_arg;

                point[i] = j + 1;
                c_arg.flag = 0x20;
                winflg[i][j] = 0;
                c_arg.Localid = point[i];
                if (i) {
                    c_arg.Localid = -c_arg.Localid;
                }
                CpObjCreate(0, 0x7E, &c_arg);
            }
        }
    }
    if (final == 2) {
        for (i = 0; i < 2; i++) {
            CREATE_ARG c_arg;

            c_arg.flag = 0x20;
            if (i) {
                c_arg.Localid = -(point[i] + 1);
            } else {
                c_arg.Localid = point[0] + 1;
            }
            CpObjCreate(0, 0x7E, &c_arg);
        }
    }
}

int IsDemoBattle(void);

void BattleTimeInit(void)
{
    if (IsDemoBattle() || (g_info.mode & 0x2600)) {
        BattleTime = g_info.TimeByCount * 99;
    } else if (g_info.mode & 0x100) {
        BattleTime = -g_info.TimeByCount;
    } else if (g_info.mode & 0x8000) {
        BattleTime = g_info.TimeByCount * gl_info->time;
    } else if (g_info.mode & 0x4000) {
        BattleTime = g_info.TimeByCount * 30;
    } else {
        BattleTime = g_info.TimeByCount * RTimeLimitTb[userinfo.roundtime];
    }
}

int GetPassBattleTime(void)
{
    return g_info.TimeByCount * RTimeLimitTb[userinfo.roundtime] - BattleTime;
}

int RequestWin(int id)
{
    if (g_info.mode & 0x100) {
        if (tr_info->itigeki == 0 && tr_info->tene.rejene == 3) {
            return 0;
        }
    }
    if (id == -1) {
        BattleStatus = 3;
        ResultFlag = 0x40000;
        AfterBattleCounter = 0;
        return 1;
    }
    id = (id == 0);
    if (ResultFlag & 0x10000) {
        return 0;
    }
    BattleStatus = 3;
    if (ResultFlag & 0x80000) {
        AfterBattleCounter = 0;
    } else {
        AfterBattleCounter = 0x5A;
    }
    ResultFlag |= 1 << id;
    if (g_info.mode & 0x8000) {
        if (gl_info->ply == id && (ResultFlag & 3) != 3) {
            gl_info->win = 1;
            userinfo.gcheck[gl_info->no] = 1;
        } else {
            gl_info->win = 0;
        }
    }
    return 1;
}

void RequestItigeki(void)
{
    ResultFlag |= 0x80000;
}

int GetWinnerSide(void)
{
    return (ResultFlag >> 1) & 1;
}

int IsBattleDecision(void)
{
    return ResultFlag & 0x10000;
}

int RefereeWorkStop(void)
{
    ResultFlag |= 0x10000;
    return 0;
}

int GetCPUCharacterFlag(void);

int IsGameJudge(int id)
{
    int i;
    int j;
    int max;

    i = (id != 0);
    if (GetCPUCharacterFlag() == 2) {
        max = g_info.VSRoundMax;
    } else {
        max = g_info.CPURoundMax;
    }
    if (g_info.mode & 0xE600) {
        max = 1;
    }
    for (j = 0; j < 8; j++) {
        if (winflg[i][j] == -1) {
            if (j >= max) {
                g_info.Winnerid = UseCharacter1p2p[i];
                g_info.Loserid = UseCharacter1p2p[i ? 0 : 1];
                g_info.WinnerSide = i;
                return 1;
            }
            return 0;
        }
    }
    return 0;
}

int IsRoundEnd(void)
{
    if (IsGameJudge(0) || IsGameJudge(1)) {
        return 1;
    }
    return 0;
}

void BeforeFight_Init(void)
{
    if (EnterFightCount == 0) {
        ResultFlag = 0;
        if (g_info.mode & 0x100) {
            EnterFightJob = 9;
            return;
        }
    }
    if (g_info.Round == 0) {
        EnterFightJob = 5;
    } else {
        EnterFightJob = 6;
    }
}

void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void BeforeFight_Black(void)
{
    if (EnterFightCount < 8) {
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 5.0f, EnterFightCount << 28);
        EnterFightCount++;
    } else {
        EnterFightCount = 0;
        EnterFightJob = 7;
    }
}

void SetScreenFlag(int flag);

void BeforeFight_Skip(void)
{
    if (EnterFightCount == 0) {
        SetScreenFlag(0);
        g_info.DestroyDelayTime[0] = 0;
        g_info.DestroyDelayTime[1] = 0;
        BattleStatus = 2;
        EnterFightCount++;
    } else {
        g_info.BattleControlFlag &= ~4;
        EnterFightJob = 0;
        EnterFightCount = 0;
    }
}

void BGM_Free(void);
void PlayerParaInit(void);
void font_entry(int side, char *str);

void EnterFight_Init(void)
{
    GAME_INFOMATION *p;

    p = &g_info;
    p->Flash = 0;
    PlayerParaInit();
    BattleTimeInit();
    font_entry(-1, 0);
    BGM_Free();
    EnterFightCount = 0;
    EnterFightJob = 1;
}

int IsFadeEnd(void);
void JobChange(int job);
int ReqSongMute(int outtime, int intime, int mutetime, int mval);
void Rq_Fadeout(void);
void SetFadeEndCommand(int val);
void SetFadeSpeed(int frame);
void sound_alloff(void);

void ExitDemoMain(void)
{
    if (EnterFightCount == 0) {
        SetFadeSpeed(20);
        SetFadeEndCommand(0);
        Rq_Fadeout();
        ReqSongMute(1, 10, 240, 5);
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        if (IsFadeEnd()) {
            sound_alloff();
            EnterFightCount++;
        }
    } else {
        JobChange(3);
        EnterFightJob = 0;
        EnterFightCount = 0;
    }
}

void InitScore(void);
int IsFadeEnd(void);
void JobChange(int job);
void Rq_Fadeout(void);
void SetFadeEndCommand(int val);
void SetFadeSpeed(int frame);
void sound_alloff(void);

void ExitFightMain(void)
{
    if (EnterFightCount == 0) {
        SetFadeSpeed(20);
        SetFadeEndCommand(0);
        Rq_Fadeout();
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        sound_alloff();
        if (g_info.mode & 0x800) {
            InitScore();
        }
        EnterFightCount++;
    } else if (EnterFightCount == 2) {
        if (IsFadeEnd()) {
            EnterFightCount++;
        }
    } else {
        JobChange(0x21);
        EnterFightJob = 0;
        EnterFightCount = 0;
    }
}

int IsFadeEnd(void);
void MayBack(int a);
void MaySHPOrg(void);
void PlayerParaInit(void);
void Rq_Fadein(void);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
void SetFadeEndCommand(int val);
void SetFadeSpeed(int frame);
void SetScreenFlag(int flag);
void nlSetBackColor(int c1, int c2, int c3);

void NextFightMain(void)
{
    if (EnterFightCount == 0) {
        SetScreenFlag(0);
        PlayerParaInit();
        SetActNo(&PlayerWork[0], 0);
        SetActNo(&PlayerWork[1], 0);
        SetAnimeNo(&PlayerWork[0], 0);
        SetAnimeNo(&PlayerWork[1], 0);
        BattleTimeInit();
        nlSetBackColor(0, 0, 0);
        if (StageNo == 5) {
            MaySHPOrg();
            MayBack(1);
        }
        SetFadeEndCommand(1);
        SetFadeSpeed(15);
        Rq_Fadein();
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        if (IsFadeEnd()) {
            EnterFightCount++;
        }
    } else if (EnterFightCount < 12) {
        EnterFightCount++;
    } else {
        EnterFightCount = 0;
        EnterFightJob = 4;
    }
}

void ReplaySaveInit(void);
int ReplaySaveMain(void);

void ReplaySave(void)
{
    if (EnterFightCount == 0) {
        ReplaySaveInit();
        EnterFightCount++;
    } else if (ReplaySaveMain()) {
        EnterFightCount = 0;
        EnterFightJob = 0xC;
    }
}

int Accumrater(int target, int sorce)
{
    int sc2 = 0;
    int sc3 = 0;
    int sc4 = 0;
    int sc5 = 0;
    int sc6 = 0;
    int sc7 = 0;
    int sc8 = 0;
    int tg2 = 0;
    int tg3 = 0;
    int tg4 = 0;
    int tg5 = 0;
    int tg6 = 0;
    int tg7 = 0;
    int tg8 = 0;
    int tg = target;
    int sc = sorce;

    while (tg >= 10000000) {
        tg -= 10000000;
        tg8++;
    }
    while (tg >= 1000000) {
        tg -= 1000000;
        tg7++;
    }
    while (tg >= 100000) {
        tg -= 100000;
        tg6++;
    }
    while (tg >= 10000) {
        tg -= 10000;
        tg5++;
    }
    while (tg >= 1000) {
        tg -= 1000;
        tg4++;
    }
    while (tg >= 100) {
        tg -= 100;
        tg3++;
    }
    while (tg >= 10) {
        tg -= 10;
        tg2++;
    }
    while (sc >= 10000000) {
        sc -= 10000000;
        sc8++;
    }
    while (sc >= 1000000) {
        sc -= 1000000;
        sc7++;
    }
    while (sc >= 100000) {
        sc -= 100000;
        sc6++;
    }
    while (sc >= 10000) {
        sc -= 10000;
        sc5++;
    }
    while (sc >= 1000) {
        sc -= 1000;
        sc4++;
    }
    while (sc >= 100) {
        sc -= 100;
        sc3++;
    }
    while (sc >= 10) {
        sc -= 10;
        sc2++;
    }
    if (sc < tg) {
        return sorce + 1;
    }
    if (sc2 < tg2) {
        return sorce + 10;
    }
    if (sc3 < tg3) {
        return sorce + 100;
    }
    if (sc4 < tg4) {
        return sorce + 1000;
    }
    if (sc5 < tg5) {
        return sorce + 10000;
    }
    if (sc6 < tg6) {
        return sorce + 100000;
    }
    if (sc7 < tg7) {
        return sorce + 1000000;
    }
    if (sc8 < tg8) {
        return sorce + 10000000;
    }
    return sorce;
}

extern SYS_COND sc;

int CockpitImageLoad(int no, int *pal, int sprtid);
int IsScaleSettle(void);
void PXF_Free(int id, int pal);
int ReqSongIn(int intime, int mval);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void DKODispMain(void)
{
    static int sprtid;
    static int pal;
    int fightctrl_time;
    int i;

    if (EnterFightCount == 0) {
        FightThreadFlag |= 1;
        if (!IsScaleSettle()) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount < 10) {
        EnterFightCount++;
    } else if (EnterFightCount == 10) {
        if (g_info.mode & 0x100) {
            ReqSongIn(10, 2);
            tr_info->itigeki = 5;
            EnterFightJob = 0;
            EnterFightCount = 0;
            return;
        }
        sprtid = CockpitImageLoad(0x31, &pal, 0);
        EnterFightCount++;
        return;
    } else if (EnterFightCount == 11) {
            RequestAiuchi();
            for (i = 0; i < 6; i++) {
                nlsp[i].sprno = sprtid;
                nlsp[i].attr = 5;
                nlsp[i].listType = 1;
                nlsp[i].trnsl = 1.0f;
            }
            nlsp[0].x = -254.0f;
            nlsp[0].y = 116.0f;
            nlsp[0].z = 3.0f;
            nlsp[0].zm_x = 1.0f;
            nlsp[0].zm_y = 0.125f;
            nlsp[0].u0 = 0;
            nlsp[0].u1 = 1.0f;
            nlsp[0].v0 = 0;
            nlsp[0].v1 = 0.125f;
            nlsp[1].x = 895.0f;
            nlsp[1].y = 144.0f;
            nlsp[1].z = 3.0f;
            nlsp[1].zm_x = 1.0f;
            nlsp[1].zm_y = 0.125f;
            nlsp[1].u0 = 0;
            nlsp[1].u1 = 1.0f;
            nlsp[1].v0 = 0.125f;
            nlsp[1].v1 = 0.25f;
            nlsp[2].x = 419.0f;
            nlsp[2].y = 249.0f;
            nlsp[2].z = 5.0f;
            nlsp[2].zm_x = 1.5f;
            nlsp[2].zm_y = 1.5f;
            nlsp[2].u0 = 0;
            nlsp[2].u1 = 0.5f;
            nlsp[2].v0 = 0.25f;
            nlsp[2].v1 = 0.75f;
            nlsp[2].trnsl = 0;
            nlsp[2].attr = 10;
            nlsp[3].x = 219.0f;
            nlsp[3].y = 249.0f;
            nlsp[3].z = 5.0f;
            nlsp[3].zm_x = 1.5f;
            nlsp[3].zm_y = 1.5f;
            nlsp[3].u0 = 0.5f;
            nlsp[3].u1 = 1.0f;
            nlsp[3].v0 = 0.25f;
            nlsp[3].v1 = 0.75f;
            nlsp[3].trnsl = 0;
            nlsp[3].attr = 10;
            nlsp[4].x = -200.0f;
            nlsp[4].y = 273.0f;
            nlsp[4].z = 7.0f;
            nlsp[4].zm_x = 0.125f;
            nlsp[4].zm_y = 0.125f;
            nlsp[4].u0 = 0;
            nlsp[4].u1 = 0.125f;
            nlsp[4].v0 = 0.75f;
            nlsp[4].v1 = 0.875f;
            nlsp[5].x = -400.0f;
            nlsp[5].y = 273.0f;
            nlsp[5].z = 3.0f;
            nlsp[5].zm_x = 0.125f;
            nlsp[5].zm_y = 0.125f;
            nlsp[5].u0 = 0;
            nlsp[5].u1 = 0.125f;
            nlsp[5].v0 = 0.75f;
            nlsp[5].v1 = 0.875f;
    }
    fightctrl_time = EnterFightCount - 11;
    if (fightctrl_time >= 0) {
        EnterFightCount++;
        if (fightctrl_time <= 120) {
            FightControl_Execute(doubleko, nlsp, fightctrl_time);
            for (i = 0; i < 6; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
        } else {
            sc.loose = 1;
            ReqSongIn(10, 2);
            PXF_Free(sprtid, pal);
            BattleStatus = 1;
            EnterFightJob = 0x11;
            EnterFightCount = 0;
        }
    }
}

CHARACTER_WORK *CpObjCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
int IsBattleEnd(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchCockpitObjectWork(CHARACTER_WORK *off, short unsigned int idno);

void AfterFightMain(void)
{
    static CHARACTER_WORK *offset = 0;
    static int fightctrl_time;

    if (EnterFightCount == 0) {
        FightThreadFlag |= 1;
        GlobalHitStop = 0x28;
        g_info.BattleControlFlag |= 4;
        EnterFightCount = 1;
        fightctrl_time = 0;
    } else if (EnterFightCount == 1) {
        if (fightctrl_time < 120 || !IsBattleEnd(&PlayerWork[0]) || !IsBattleEnd(&PlayerWork[1])) {
            if (fightctrl_time == 10) {
                CREATE_ARG c_arg;

                c_arg.flag = 4;
                c_arg.actno = 0;
                offset = CpObjCreate(0, 0x7F, &c_arg);
                offset->PalId = -1;
            }
            fightctrl_time++;
        } else {
            EnterFightCount++;
        }
    } else if (EnterFightCount == 2) {
        if (SearchCockpitObjectWork(offset, 0x7F) == 0) {
            BattleStatus = 4;
            EnterFightCount++;
        }
    } else if (EnterFightCount < 5) {
        if (ResultFlag & 0x20000) {
            EnterFightJob = 0x18;
        } else {
            GAME_INFOMATION *p;

            p = &g_info;
            if (p->isCPU[GetWinnerSide()]) {
                EnterFightJob = 0x17;
            } else {
                EnterFightJob = 0x16;
            }
        }
        EnterFightCount = 0;
    }
}

int CockpitImageLoad(int no, int *pal, int sprtid);
int IsBattleEnd(CHARACTER_WORK *offset);
void PXF_Free(int id, int pal);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
int sound_request(int no);

void DestroyMain(void)
{
    static int sprtid;
    static int sprtid2;
    static int pal;
    static int fightctrl_time;
    int i;

    if (EnterFightCount == 0) {
        FightThreadFlag |= 1;
        g_info.BattleControlFlag |= 4;
        if (g_info.DestroyDelayTime[GetWinnerSide()]) {
            g_info.DestroyDelayTime[GetWinnerSide()]--;
            return;
        }
        GlobalHitStop = 0x28;
        sound_request(0x35);
        sound_request(0x35);
        sound_request(0x35);
        sound_request(0x35);
        sound_request(0x35);
        sprtid = CockpitImageLoad(0x3F, &pal, 0);
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        sprtid2 = CockpitImageLoad(0x40, &pal, 1);
        EnterFightCount++;
    } else if (EnterFightCount == 2) {
        for (i = 0; i < 4; i++) {
            nlsp[i].listType = 1;
            nlsp[i].z = 3.0f;
            nlsp[i].trnsl = 0;
            nlsp[i].attr = 5;
        }
        nlsp[0].sprno = sprtid;
        nlsp[0].x = 288.0f;
        nlsp[0].y = 256.0f;
        nlsp[0].z = 5.0f;
        nlsp[0].attr |= 0x10;
        SetUVZ(&nlsp[0], 0, 0, 256, 256);
        nlsp[1].sprno = sprtid;
        nlsp[1].x = 288.0f;
        nlsp[1].y = 256.0f;
        nlsp[1].z = 5.0f;
        SetUVZ(&nlsp[1], 0, 0, 256, 256);
        nlsp[2].sprno = sprtid2;
        nlsp[2].x = 64.0f;
        nlsp[2].y = 176.0f;
        nlsp[2].z = 3.0f;
        SetUVZ(&nlsp[2], 0, 0, 256, 128);
        nlsp[3].sprno = sprtid2;
        nlsp[3].x = 320.0f;
        nlsp[3].y = 176.0f;
        nlsp[3].z = 3.0f;
        SetUVZ(&nlsp[3], 0, 128, 256, 128);
        fightctrl_time = 0;
        EnterFightCount++;
    } else if (EnterFightCount == 3) {
        if (fightctrl_time < 120) {
            FightControl_Execute(destroytb, nlsp, fightctrl_time);
            for (i = 0; i < 4; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
            fightctrl_time++;
        } else {
            EnterFightCount = 4;
        }
    } else if (EnterFightCount == 4) {
        if (IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            PXF_Free(sprtid, pal);
            PXF_Free(sprtid2, pal);
            BattleStatus = 4;
            EnterFightCount++;
        }
    } else {
        if (ResultFlag & 0x20000) {
            EnterFightJob = 0x15;
        } else {
            GAME_INFOMATION *p;

            p = &g_info;
            if (p->isCPU[GetWinnerSide()]) {
                EnterFightJob = 0x17;
            } else {
                EnterFightJob = 0x16;
            }
        }
        EnterFightCount = 0;
    }
}

int CockpitImageLoad(int no, int *pal, int sprtid);
void PXF_Free(int id, int pal);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
void *mmalloc(int size, char *name);
int sound_request(int no);

void DestroyMayMain(void)
{
    static int sprtid;
    static int sprtid2;
    static int pal;
    static int fightctrl_time;
    int i;

    if (EnterFightCount == 0) {
        FightThreadFlag |= 1;
        g_info.BattleControlFlag |= 4;
        if (g_info.DestroyDelayTime[GetWinnerSide()]) {
            g_info.DestroyDelayTime[GetWinnerSide()]--;
            return;
        }
        GlobalHitStop = 0x28;
        sound_request(0x35);
        sound_request(0x35);
        sound_request(0x35);
        sound_request(0x35);
        sound_request(0x35);
        if (myp == 0) {
            myp = mmalloc(0x90, "myp");
        }
        sprtid = CockpitImageLoad(0x41, &pal, 0);
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        sprtid2 = CockpitImageLoad(0x42, &pal, 1);
        EnterFightCount++;
    } else if (EnterFightCount == 2) {
        for (i = 0; i < 12; i++) {
            nlsp[i].sprno = sprtid;
            nlsp[i].listType = 1;
            nlsp[i].z = 3.0f;
            nlsp[i].trnsl = 1.0f;
            nlsp[i].attr = 5;
        }
        nlsp[0].x = 125.0f;
        nlsp[0].y = -64.0f;
        SetUVZ(&nlsp[0], 0, 0, 64, 64);
        nlsp[1].x = 140.0f;
        nlsp[1].y = -64.0f;
        SetUVZ(&nlsp[1], 64, 0, 64, 64);
        nlsp[2].x = 199.0f;
        nlsp[2].y = -64.0f;
        SetUVZ(&nlsp[2], 128, 0, 64, 64);
        nlsp[3].x = 217.0f;
        nlsp[3].y = -64.0f;
        SetUVZ(&nlsp[3], 192, 0, 64, 64);
        nlsp[4].x = 292.0f;
        nlsp[4].y = -64.0f;
        SetUVZ(&nlsp[4], 0, 64, 64, 64);
        nlsp[5].x = 358.0f;
        nlsp[5].y = -64.0f;
        SetUVZ(&nlsp[5], 64, 64, 64, 64);
        nlsp[6].x = 378.0f;
        nlsp[6].y = -64.0f;
        SetUVZ(&nlsp[6], 128, 64, 64, 64);
        nlsp[7].x = 439.0f;
        nlsp[7].y = -64.0f;
        SetUVZ(&nlsp[7], 192, 64, 64, 64);
        nlsp[8].x = 453.0f;
        nlsp[8].y = -64.0f;
        SetUVZ(&nlsp[8], 0, 128, 64, 64);
        nlsp[9].x = -256.0f;
        nlsp[9].y = -256.0f;
        SetUVZ(&nlsp[9], 128, 128, 128, 128);
        nlsp[10].sprno = sprtid2;
        nlsp[10].x = 64.0f;
        nlsp[10].y = 64.0f;
        nlsp[10].z = 5.0f;
        nlsp[10].trnsl = 0;
        SetUVZ(&nlsp[10], 0, 0, 256, 256);
        nlsp[11].sprno = sprtid2;
        nlsp[11].x = 320.0f;
        nlsp[11].y = 64.0f;
        nlsp[11].z = 5.0f;
        nlsp[11].trnsl = 0;
        nlsp[11].attr |= 0x10;
        SetUVZ(&nlsp[11], 0, 0, 256, 256);
        fightctrl_time = 0;
        EnterFightCount++;
    } else if (EnterFightCount == 3) {
        if (fightctrl_time <= 120) {
            FightControl_Execute(maydstroy, nlsp, fightctrl_time);
            for (i = 0; i < 12; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
            fightctrl_time++;
        } else {
            PXF_Free(sprtid, pal);
            PXF_Free(sprtid2, pal);
            BattleStatus = 4;
            EnterFightCount++;
        }
    } else if (EnterFightCount == 4) {
        if (ResultFlag & 0x20000) {
            EnterFightJob = 0x15;
        } else {
            GAME_INFOMATION *p;

            p = &g_info;
            if (p->isCPU[GetWinnerSide()]) {
                EnterFightJob = 0x17;
            } else {
                EnterFightJob = 0x16;
            }
        }
        EnterFightCount = 0;
    }
}

void ExitDemo_Init(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0xB;
}

void DestroyInit(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0x1A;
}

void DestroyMayInit(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0x1B;
}

void AfterFightInit(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0xA;
}

void TimeOverDispInit(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0x19;
}

void IntIrq(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0x1C;
}

void sv_IntIrq(void)
{
    EnterFightCount = 0;
    EnterFightJob = 0x1D;
}

void PXF_GetSize(int id, int *x, int *y);

void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h)
{
    int x;
    int y;

    PXF_GetSize(nlsp->sprno, &x, &y);
    if (u < 0 || v < 0 || w < 0 || h < 0) {
        return;
    }
    nlsp->u0 = (float)u / x;
    nlsp->u1 = (float)(u + w) / x;
    nlsp->v0 = (float)v / y;
    nlsp->v1 = (float)(v + h) / y;
    nlsp->zm_x = (float)w / x;
    nlsp->zm_y = (float)h / y;
}

int FTO_Exe(int cmd, int no);
int IsDemoBattle(void);
int IsRannyuu(void);
int ReqSongIn(int intime, int mval);
int ReqSongOut(int outtime, int mval);

void RefereeExe(void)
{
    FTO_Exe(3, 0);
    if (g_info.mode & 0x100) {
        if (tr_info->itigeki == 0 && tr_info->tene.rejene == 3) {
            return;
        }
    }
    if (ResultFlag != 0 && (~ResultFlag & 0x10000)) {
        ResultFlag |= 0x10000;
        if ((ResultFlag & 3) == 3) {
            ResultFlag |= 0x20000;
        }
    }
    if (AfterBattleCounter > 0) {
        AfterBattleCounter--;
    }
    if (BattleStatus == 3) {
        if (!(FightThreadFlag & 1)) {
            ReqSongOut(10, 5);
            if (ResultFlag & 0x80000) {
                if (g_info.mode & 0x2000) {
                    story.is_itigeki = 1;
                }
                if (PlayerWork[GetWinnerSide()].idno == 3) {
                    DestroyMayInit();
                } else {
                    DestroyInit();
                }
            } else if ((g_info.mode & 0x100) && tr_info->begin > 0) {
                ResultFlag = 0;
                BattleStatus = 2;
                FightThreadFlag = 0;
                ReqSongIn(10, 2);
            } else {
                if (g_info.mode & 0x2000) {
                    story.is_itigeki = 0;
                }
                AfterFightInit();
            }
        }
    } else {
        FightThreadFlag = 0;
    }
    if (BattleStatus == 2) {
        if (BattleTimeStopFlag == 0 && (!(g_info.mode & 0x2000) || story.is_enter == 0)) {
            if (BattleTime > 0) {
                BattleTime--;
                if (BattleTime == 0) {
                    FightThreadFlag |= 1;
                    BattleStatus = 3;
                    ResultFlag |= 0x10000;
                    if (g_info.mode & 0x8000) {
                        if (gl_info->clear & 8) {
                            ResultFlag = gl_info->ply == 0 ? ResultFlag | 2 : ResultFlag | 1;
                            gl_info->win = 0;
                            goto timeout;
                        }
                        if (gl_info->clear & 0x10) {
                            ResultFlag |= gl_info->ply + 1;
                            gl_info->win = 1;
                            userinfo.gcheck[gl_info->no] = 1;
                            goto timeout;
                        }
                    }
                    if (PlayerWork[0].HitPoint == PlayerWork[1].HitPoint) {
                        if (g_info.mode & 0x2000) {
                            if (PlayerWork[story.self_side].HitPoint == PLAYER_HITPOINTMAX) {
                                PlayerWork[story.self_side].HitPoint--;
                            } else if (PlayerWork[story.self_side].HitPoint == 1) {
                                PlayerWork[story.self_side ? 0 : 1].HitPoint++;
                            }
                            ResultFlag = story.self_side == 0 ? ResultFlag | 2 : ResultFlag | 1;
                        } else if (g_info.mode & 0x200) {
                            if (PlayerWork[sv_info->ply].HitPoint == PLAYER_HITPOINTMAX) {
                                PlayerWork[sv_info->ply].HitPoint--;
                            } else if (PlayerWork[sv_info->ply].HitPoint == 1) {
                                PlayerWork[sv_info->ply ? 0 : 1].HitPoint++;
                            }
                            ResultFlag = sv_info->ply == 0 ? ResultFlag | 2 : ResultFlag | 1;
                        } else if (g_info.mode & 0x8000) {
                            if (PlayerWork[gl_info->ply].HitPoint == PLAYER_HITPOINTMAX) {
                                PlayerWork[gl_info->ply].HitPoint--;
                            } else if (PlayerWork[gl_info->ply].HitPoint == 1) {
                                PlayerWork[gl_info->ply ? 0 : 1].HitPoint++;
                            }
                            ResultFlag = gl_info->ply == 0 ? ResultFlag | 2 : ResultFlag | 1;
                            gl_info->win = 0;
                        } else {
                            ResultFlag |= 0x20000;
                        }
                    } else if (PlayerWork[1].HitPoint < PlayerWork[0].HitPoint) {
                        if (g_info.mode & 0x2000) {
                            story.is_towin = story.self_side == 0;
                        }
                        ResultFlag |= 1;
                    } else {
                        if (g_info.mode & 0x2000) {
                            story.is_towin = story.self_side == 1;
                        }
                        ResultFlag |= 2;
                    }
                timeout:
                    ReqSongOut(10, 5);
                    TimeOverDispInit();
                }
            }
        }
        if (!(PlayerWork[0].CommandFlag & 0x40)) {
            BattleTimeStopFlag &= ~1;
        }
        if (!(PlayerWork[1].CommandFlag & 0x40)) {
            BattleTimeStopFlag &= ~2;
        }
        if (!IsDemoBattle()) {
            IsRannyuu();
        }
    } else {
        BattleTimeStopFlag = 0;
    }
}

void EnterFight_Clear(void)
{
    EnterFightJob = 0;
}

int IsGameJudge(int id);
void clr_preaction(void);
void EliminateRoundEnd(void);
int is_retry(void);
void next_scene(int is_win);
extern CREDIT_RAM cr_ram[2];
/* RoundEndMain's function static flg; extern so the verifier solves its address (function statics land in .sbss in source order) */
extern int RoundEndMain_flg;

void RoundEndMain(void)
{
    int side;

    if (EnterFightCount == 0) {
        RoundEndMain_flg = 0;
        if (IsGameJudge(0)) {
            RoundEndMain_flg = 1;
        }
        if (IsGameJudge(1)) {
            RoundEndMain_flg = 1;
        }
        if (g_info.mode & 0x4000) {
            clr_preaction();
            RoundEndMain_flg = 1;
        }
        if (RoundEndMain_flg || final == 2) {
            if (g_info.mode & 0x800) {
                g_info.wincnt[g_info.WinnerSide]++;
                g_info.wincnt[g_info.WinnerSide == 0] = 0;
            }
            if ((g_info.mode & 3) && !(g_info.mode & ~0x803) && userinfo.quickconti) {
                side = g_info.WinnerSide;
                if (!((g_info.mode >> side) & 1)) {
                    EnterFightCount = 0;
                    EnterFightJob = 0xE;
                } else if ((g_info.mode & 3) == 3 && cr_ram[side].mode == 5) {
                    EnterFightCount = 0;
                    EnterFightJob = 0xE;
                } else {
                    EnterFightCount = 0;
                    EnterFightJob = 0xC;
                }
            } else if (g_info.mode & 0x2000) {
                if (!((g_info.mode >> g_info.WinnerSide) & 1)) {
                    if (is_retry()) {
                        EnterFightCount = 0;
                        EnterFightJob = 0xF;
                    } else {
                        EnterFightCount = 0;
                        EnterFightJob = 0xC;
                    }
                    do {
                        next_scene(0);
                    } while (0); /* a call in a loop nest is never a sibcall */
                } else {
                    EnterFightCount = 0;
                    EnterFightJob = 0xC;
                }
            } else if (g_info.mode & 0x8000) {
                if ((g_info.mode >> g_info.WinnerSide) & 1) {
                    EnterFightJob = 0xC;
                } else {
                    EnterFightJob = 0x10;
                }
                EnterFightCount = 0;
            } else {
                EnterFightCount = 0;
                EnterFightJob = 0xC;
            }
        } else {
            SetFadeSpeed(15);
            SetFadeEndCommand(0);
            Rq_Fadeout();
            EnterFightCount++;
        }
    } else if (IsFadeEnd()) {
        EliminateRoundEnd();
        font_entry(-1, 0);
        EnterFightCount = 0;
        EnterFightJob = 0xD;
    }
}

void FadeInit(void);
void JobChange(int job);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void sv_levelup_info(int sw);
extern int intrapt;
/* IntIrqMain's function statics, extern so the verifier solves them */
extern int IntIrqMain_pal1; /* STATIC_EXTERN 0x5cc72c: pal1 */
extern int IntIrqMain_sprtid1; /* STATIC_EXTERN 0x5cc730: sprtid1 */
extern int IntIrqMain_sprtid2; /* STATIC_EXTERN 0x5cc734: sprtid2 */
extern int IntIrqMain_sprtid3; /* STATIC_EXTERN 0x5cc738: sprtid3 */
extern char IntIrqMain_a; /* STATIC_EXTERN 0x5cc73c: a */
extern int IntIrqMain_fightctrl_time; /* STATIC_EXTERN 0x5cc740: fightctrl_time */

void IntIrqMain(void)
{
    int i;

    if (EnterFightCount == 0) {
        BattleStatus = 1;
        EnterFightCount = 1;
    } else if (EnterFightCount == 1) {
        BattleStatus = 1;
        IntIrqMain_sprtid1 = CockpitImageLoad(0x32, &IntIrqMain_pal1, 0);
        EnterFightCount++;
    } else if (EnterFightCount == 2) {
        BattleStatus = 1;
        IntIrqMain_sprtid2 = CockpitImageLoad(0x33, &IntIrqMain_pal1, 1);
        EnterFightCount++;
    } else if (EnterFightCount == 3) {
        BattleStatus = 1;
        IntIrqMain_sprtid3 = CockpitImageLoad(0x34, &IntIrqMain_pal1, 2);
        EnterFightCount++;
    } else if (EnterFightCount == 4) {
        BattleStatus = 1;
        for (i = 0; i < 9; i++) {
            nlsp[i].sprno = IntIrqMain_sprtid1;
            nlsp[i].z = 2.0f;
            nlsp[i].zm_x = 1.0f;
            nlsp[i].zm_y = 1.0f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 1.0f;
            nlsp[i].v0 = 0;
            nlsp[i].v1 = 1.0f;
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
        }
        nlsp[0].attr = 0x17;
        nlsp[0].x = -320.0f;
        nlsp[0].y = -16.0f;
        nlsp[1].x = -320.0f;
        nlsp[1].y = -16.0f;
        nlsp[2].sprno = IntIrqMain_sprtid2;
        nlsp[2].x = 960.0f;
        nlsp[2].y = 240.0f;
        nlsp[2].attr |= 0x10;
        nlsp[3].sprno = IntIrqMain_sprtid2;
        nlsp[3].x = 704.0f;
        nlsp[3].y = 240.0f;
        nlsp[4].sprno = IntIrqMain_sprtid3;
        nlsp[4].x = 64.0f;
        nlsp[4].y = 183.0f;
        nlsp[4].z = 1.0f;
        SetUVZ(&nlsp[4], 0, 0, 0x100, 0x40);
        nlsp[4].trnsl = 0;
        nlsp[5].sprno = IntIrqMain_sprtid3;
        nlsp[5].x = 320.0f;
        nlsp[5].y = 183.0f;
        nlsp[5].z = 1.0f;
        SetUVZ(&nlsp[5], 0, 0x40, 0x100, 0x40);
        nlsp[5].trnsl = 0;
        nlsp[6].sprno = IntIrqMain_sprtid3;
        nlsp[6].x = 64.0f;
        nlsp[6].y = 183.0f;
        nlsp[6].z = 1.0f;
        SetUVZ(&nlsp[6], 0, 0, 0x100, 0x40);
        nlsp[6].trnsl = 0;
        nlsp[7].sprno = IntIrqMain_sprtid3;
        nlsp[7].x = 320.0f;
        nlsp[7].y = 183.0f;
        nlsp[7].z = 1.0f;
        SetUVZ(&nlsp[7], 0, 0x40, 0x100, 0x40);
        nlsp[7].trnsl = 0;
        nlsp[8].sprno = IntIrqMain_sprtid3;
        nlsp[8].x = 320.0f;
        nlsp[8].y = 183.0f;
        nlsp[8].z = 1.0f;
        SetUVZ(&nlsp[8], 0, 0x80, 0x100, 0x40);
        IntIrqMain_fightctrl_time = 0;
        IntIrqMain_a = 0;
        FadeInit();
        EnterFightCount++;
    } else if (EnterFightCount == 5) {
        if (IntIrqMain_fightctrl_time <= 120) {
            FightControl_Execute(ranyu, nlsp, IntIrqMain_fightctrl_time);
            if (IntIrqMain_fightctrl_time >= 110 && IntIrqMain_fightctrl_time < 118) {
                IntIrqMain_a += 0x1F;
            } else if (IntIrqMain_fightctrl_time == 118) {
                IntIrqMain_a = 0xFF;
            }
            nlsp[8].x = nlsp[2].x - 128.0f;
            nlsp[8].y = nlsp[2].y + 74.0f;
            polydraw_FBox(0, (int)(nlsp[0].y + 255.0f), 640.0f, (int)(nlsp[2].y + 1.0f), 2.0f, IntIrqMain_a << 24);
            polydraw_FBox((int)(nlsp[0].x - 256.0f - 64.0f), (int)nlsp[0].y, (int)(nlsp[0].x - 254.0f), (int)(nlsp[0].y + 256.0f), 2.0f, 0xFF000000);
            polydraw_FBox((int)(nlsp[1].x + 254.0f), (int)nlsp[1].y, (int)(nlsp[1].x + 256.0f + 64.0f), (int)(nlsp[1].y + 256.0f), 2.0f, 0xFF000000);
            polydraw_FBox((int)(nlsp[2].x + 256.0f), (int)nlsp[2].y, (int)(nlsp[2].x + 256.0f + 64.0f), (int)(nlsp[2].y + 256.0f), 2.0f, 0xFF000000);
            polydraw_FBox((int)(nlsp[3].x - 64.0f), (int)nlsp[3].y, (int)(nlsp[3].x + 1.0f), (int)(nlsp[3].y + 256.0f), 2.0f, 0xFF000000);
            for (i = 0; i < 9; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
            IntIrqMain_fightctrl_time++;
        } else {
            polydraw_FBox(0, 0, 640.0f, 480.0f, 2.0f, 0xFF000000);
            EnterFightCount++;
        }
    } else if (EnterFightCount == 6) {
        polydraw_FBox(0, 0, 640.0f, 480.0f, 2.0f, 0xFF000000);
        sv_levelup_info(-1);
        PXF_Free(IntIrqMain_sprtid1, IntIrqMain_pal1);
        PXF_Free(IntIrqMain_sprtid2, IntIrqMain_pal1);
        PXF_Free(IntIrqMain_sprtid3, IntIrqMain_pal1);
        intrapt = 1;
        JobChange(0x21);
        EnterFightJob = 0;
        EnterFightCount = 0;
    }
}

int GetWinnerSide(void);
void polydraw_FLine(float x1, float y1, float x2, float y2, float w, unsigned int color);
/* TimeOverMain's function statics, extern so the verifier solves them */
extern int TimeOverMain_sprtid; /* STATIC_EXTERN 0x5cc718: sprtid */
extern int TimeOverMain_pal; /* STATIC_EXTERN 0x5cc71c: pal */
extern float TimeOverMain_y1; /* STATIC_EXTERN 0x5cc720: y1 */
extern float TimeOverMain_y2; /* STATIC_EXTERN 0x5cc724: y2 */
extern int TimeOverMain_fightctrl_time; /* STATIC_EXTERN 0x5cc728: fightctrl_time */
/* NOTE: byte-identical, but its double literals (0x5be4e8) follow the TU's static FIGHT_CONTROL
   tables in .rodata; until those are defined, appending this breaks Cmd_ReqName's jump table. */

void TimeOverMain(void)
{
    int i;

    if (EnterFightCount == 0) {
        FightThreadFlag |= 1;
        g_info.BattleControlFlag |= 4;
        TimeOverMain_sprtid = CockpitImageLoad(0x2F, &TimeOverMain_pal, 0);
        for (i = 0; i < 4; i++) {
            nlsp[i].sprno = TimeOverMain_sprtid;
            nlsp[i].z = 3.0f;
            nlsp[i].zm_x = 1.0f;
            nlsp[i].zm_y = 0.25f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 1.0f;
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
        }
        nlsp[0].v0 = 0;
        nlsp[0].v1 = 0.25f;
        nlsp[1].v0 = 0.25f;
        nlsp[1].v1 = 0.5f;
        nlsp[2].v0 = 0.5f;
        nlsp[2].v1 = 0.75f;
        nlsp[3].v0 = 0.75f;
        nlsp[3].v1 = 1.0f;
        nlsp[0].x = -239.0f;
        nlsp[1].x = 853.0f;
        nlsp[2].x = 838.0f;
        nlsp[3].x = -198.0f;
        nlsp[0].y = 212.0f;
        nlsp[1].y = 237.0f;
        nlsp[2].y = 212.0f;
        nlsp[3].y = 237.0f;
        TimeOverMain_y1 = -15.0f;
        TimeOverMain_y2 = 495.0f;
        TimeOverMain_fightctrl_time = 0;
        EnterFightCount++;
    }
    if (EnterFightCount == 1) {
        if (TimeOverMain_fightctrl_time < 10) {
            TimeOverMain_y1 += 22.7;
            TimeOverMain_y2 -= 24.1;
        } else {
            TimeOverMain_y1 = 212.0f;
            TimeOverMain_y2 = 254.0f;
        }
        FightControl_Execute(timeup, nlsp, TimeOverMain_fightctrl_time);
        for (i = 0; i < 4; i++) {
            gSpriteDraw_NaomiLibVersion(&nlsp[i]);
        }
        for (i = 0; i < 15; i += 2) {
            polydraw_FLine(112.0f, (int)(TimeOverMain_y1 + i), 528.0f, (int)(TimeOverMain_y1 + i), 5.0f, 0xFFFFFFFF);
            polydraw_FLine(112.0f, (int)(TimeOverMain_y2 + i), 528.0f, (int)(TimeOverMain_y2 + i), 5.0f, 0xFFFFFFFF);
        }
        if (TimeOverMain_fightctrl_time >= 120 && IsBattleEnd(&PlayerWork[0]) && IsBattleEnd(&PlayerWork[1])) {
            EnterFightCount++;
        } else {
            TimeOverMain_fightctrl_time++;
        }
    } else {
        GAME_INFOMATION *p;

        BattleStatus = 4;
        PXF_Free(TimeOverMain_sprtid, TimeOverMain_pal);
        if (ResultFlag & 0x20000) {
            EnterFightJob = 0x15;
        } else {
            p = &g_info;
            if (p->isCPU[GetWinnerSide()]) {
                EnterFightJob = 0x17;
            } else {
                EnterFightJob = 0x16;
            }
        }
        EnterFightCount = 0;
    }
}

int IsRannyuu(void);
/* BeforeFight_Rock's function statics, extern so the verifier solves them */
extern int BeforeFight_Rock_sprtid1; /* STATIC_EXTERN 0x5cc674: sprtid1 */
extern int BeforeFight_Rock_sprtid2; /* STATIC_EXTERN 0x5cc678: sprtid2 */
extern int BeforeFight_Rock_pal; /* STATIC_EXTERN 0x5cc67c: pal */
extern float BeforeFight_Rock_v; /* STATIC_EXTERN 0x5cc680: v */
extern float BeforeFight_Rock_x1; /* STATIC_EXTERN 0x5cc684: x1 */

void BeforeFight_Rock(void)
{
    int i;
    int fightctrl_time;

    if (EnterFightCount == 0) {
        BeforeFight_Rock_sprtid1 = CockpitImageLoad(0x3B, &BeforeFight_Rock_pal, 0);
    } else if (EnterFightCount == 1) {
        BeforeFight_Rock_sprtid2 = CockpitImageLoad(0x3C, &BeforeFight_Rock_pal, 1);
    } else if (EnterFightCount == 2) {
        for (i = 0; i < 20; i++) {
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 0;
            nlsp[i].z = 3.0f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 0;
            nlsp[i].v0 = 0;
            nlsp[i].v1 = 0;
            nlsp[i].zm_x = 0;
            nlsp[i].zm_y = 0;
        }
        nlsp[0].sprno = BeforeFight_Rock_sprtid1;
        nlsp[0].z = 5.0f;
        nlsp[0].x = -1.0f;
        nlsp[0].y = 204.0f;
        nlsp[0].attr = 0xe;
        SetUVZ(&nlsp[0], 0, 0, 0x100, 0x40);
        nlsp[1].sprno = BeforeFight_Rock_sprtid1;
        nlsp[1].z = 5.0f;
        nlsp[1].x = 511.0f;
        nlsp[1].y = 172.0f;
        nlsp[1].attr = 0x9;
        SetUVZ(&nlsp[1], 0, 0, 0x100, 0x40);
        nlsp[2].sprno = BeforeFight_Rock_sprtid1;
        nlsp[2].z = 7.0f;
        nlsp[2].x = 331.0f;
        nlsp[2].y = 214.0f;
        nlsp[2].attr = 0xe;
        SetUVZ(&nlsp[2], 0, 0x40, 0x40, 0x40);
        nlsp[3].sprno = BeforeFight_Rock_sprtid1;
        nlsp[3].z = 7.0f;
        nlsp[3].x = 323.0f;
        nlsp[3].y = 219.0f;
        nlsp[3].attr = 0xd;
        SetUVZ(&nlsp[3], 0x40, 0x40, 0x40, 0x40);
        nlsp[4].sprno = BeforeFight_Rock_sprtid1;
        nlsp[4].z = 7.0f;
        nlsp[4].x = 340.0f;
        nlsp[4].y = 240.0f;
        nlsp[4].attr = 0xd;
        SetUVZ(&nlsp[4], 0x80, 0x40, 0x40, 0x40);
        nlsp[5].sprno = BeforeFight_Rock_sprtid1;
        nlsp[5].z = 7.0f;
        nlsp[5].x = 346.0f;
        nlsp[5].y = 240.0f;
        nlsp[5].attr = 0x9;
        SetUVZ(&nlsp[5], 0xc0, 0x40, 0x40, 0x40);
        nlsp[6].sprno = BeforeFight_Rock_sprtid1;
        nlsp[6].z = 7.0f;
        nlsp[6].x = 340.0f;
        nlsp[6].y = 233.0f;
        nlsp[6].attr = 0x5;
        SetUVZ(&nlsp[6], 0, 0x80, 0x40, 0x40);
        nlsp[7].sprno = BeforeFight_Rock_sprtid1;
        nlsp[7].z = 7.0f;
        nlsp[7].x = 323.0f;
        nlsp[7].y = 261.0f;
        nlsp[7].attr = 0x5;
        SetUVZ(&nlsp[7], 0x40, 0x80, 0x40, 0x40);
        nlsp[8].sprno = BeforeFight_Rock_sprtid1;
        nlsp[8].z = 7.0f;
        nlsp[8].x = 331.0f;
        nlsp[8].y = 266.0f;
        nlsp[8].attr = 0x6;
        SetUVZ(&nlsp[8], 0x80, 0x80, 0x40, 0x40);
        nlsp[9].sprno = BeforeFight_Rock_sprtid1;
        nlsp[9].z = 7.0f;
        nlsp[9].x = 309.0f;
        nlsp[9].y = 266.0f;
        nlsp[9].attr = 0x16;
        SetUVZ(&nlsp[9], 0x80, 0x80, 0x40, 0x40);
        nlsp[10].sprno = BeforeFight_Rock_sprtid1;
        nlsp[10].z = 7.0f;
        nlsp[10].x = 317.0f;
        nlsp[10].y = 261.0f;
        nlsp[10].attr = 0x17;
        SetUVZ(&nlsp[10], 0x40, 0x80, 0x40, 0x40);
        nlsp[11].sprno = BeforeFight_Rock_sprtid1;
        nlsp[11].z = 7.0f;
        nlsp[11].x = 300.0f;
        nlsp[11].y = 234.0f;
        nlsp[11].attr = 0x17;
        SetUVZ(&nlsp[11], 0, 0x80, 0x40, 0x40);
        nlsp[12].sprno = BeforeFight_Rock_sprtid1;
        nlsp[12].z = 7.0f;
        nlsp[12].x = 294.0f;
        nlsp[12].y = 240.0f;
        nlsp[12].attr = 0x1b;
        SetUVZ(&nlsp[12], 0xc0, 0x40, 0x40, 0x40);
        nlsp[13].sprno = BeforeFight_Rock_sprtid1;
        nlsp[13].z = 7.0f;
        nlsp[13].x = 300.0f;
        nlsp[13].y = 241.0f;
        nlsp[13].attr = 0x1f;
        SetUVZ(&nlsp[13], 0x80, 0x40, 0x40, 0x40);
        nlsp[14].sprno = BeforeFight_Rock_sprtid1;
        nlsp[14].z = 7.0f;
        nlsp[14].x = 317.0f;
        nlsp[14].y = 219.0f;
        nlsp[14].attr = 0x1f;
        SetUVZ(&nlsp[14], 0x40, 0x40, 0x40, 0x40);
        nlsp[15].sprno = BeforeFight_Rock_sprtid1;
        nlsp[15].z = 7.0f;
        nlsp[15].x = 309.0f;
        nlsp[15].y = 214.0f;
        nlsp[15].attr = 0x1e;
        SetUVZ(&nlsp[15], 0, 0x40, 0x40, 0x40);
        nlsp[16].sprno = BeforeFight_Rock_sprtid2;
        nlsp[16].z = 5.0f;
        nlsp[16].x = -3.0f;
        nlsp[16].y = 213.0f;
        nlsp[16].attr = 0x7;
        SetUVZ(&nlsp[16], 0, 0, 0x100, 0x80);
        nlsp[17].sprno = BeforeFight_Rock_sprtid2;
        nlsp[17].z = 5.0f;
        nlsp[17].x = 10.0f;
        nlsp[17].y = 216.0f;
        nlsp[17].attr = 0x5;
        SetUVZ(&nlsp[17], 0, 0x80, 0x100, 0x80);
        nlsp[18].sprno = BeforeFight_Rock_sprtid2;
        nlsp[18].z = 5.0f;
        nlsp[18].x = 381.0f;
        nlsp[18].y = 213.0f;
        nlsp[18].attr = 0x5;
        SetUVZ(&nlsp[18], 0, 0, 0x100, 0x80);
        nlsp[19].sprno = BeforeFight_Rock_sprtid2;
        nlsp[19].z = 5.0f;
        nlsp[19].x = 650.0f;
        nlsp[19].y = 216.0f;
        nlsp[19].attr = 0x5;
        SetUVZ(&nlsp[19], 0, 0x80, 0x100, 0x80);
        BeforeFight_Rock_v = 64.0f;
        BeforeFight_Rock_x1 = 1.0f;
    }
    fightctrl_time = EnterFightCount - 2;
    EnterFightCount++;
    if (fightctrl_time >= 0) {
        if (fightctrl_time <= 50) {
            FightControl_Execute(rocktb, nlsp, fightctrl_time);
            if (fightctrl_time < 18) {
                for (i = 0; i < 20; i++) {
                    gSpriteDraw_NaomiLibVersion(&nlsp[i]);
                }
            } else if (fightctrl_time >= 18 && fightctrl_time < 32) {
                for (i = 1; i < 20; i++) {
                    gSpriteDraw_NaomiLibVersion(&nlsp[i]);
                }
            } else if (fightctrl_time >= 32) {
                for (i = 0; i < 20; i++) {
                    if (i != 1 && i != 18 && i != 19) {
                        gSpriteDraw_NaomiLibVersion(&nlsp[i]);
                    }
                }
            }
            if (IsRannyuu()) {
                PXF_Free(BeforeFight_Rock_sprtid1, BeforeFight_Rock_pal);
                PXF_Free(BeforeFight_Rock_sprtid2, BeforeFight_Rock_pal);
                EnterFightCount = 0;
                EnterFightJob = 0x1C;
            }
        } else {
            PXF_Free(BeforeFight_Rock_sprtid1, BeforeFight_Rock_pal);
            PXF_Free(BeforeFight_Rock_sprtid2, BeforeFight_Rock_pal);
            EnterFightCount = 0;
            EnterFightJob = 9;
        }
    }
}

void Cmd_AddB(FIGHT_CONTROL *p, NL_SPR_ARG *n)
{
    NL_SPR_ARG *sp;
    unsigned int c;
    unsigned int a;
    unsigned int r;
    unsigned int g;
    unsigned int b;

    sp = &n[p->id];
    c = sp->base_color;
    a = c >> 24;
    r = (c >> 16) & 0xff;
    r += p->data2;
    g = (c >> 8) & 0xff;
    g += p->data2;
    b = c & 0xff;
    b += p->data2;
    sp->base_color = (a << 24) | (r << 16) | (g << 8) | b;
}

extern int ScoreMain_time; /* STATIC_EXTERN 0x5cc6b0: time */
extern int ScoreMain_hp; /* STATIC_EXTERN 0x5cc6b4: hp */
extern int ScoreMain_tp; /* STATIC_EXTERN 0x5cc6b8: tp */
extern int ScoreMain_pp; /* STATIC_EXTERN 0x5cc6bc: pp */
extern int ScoreMain_t; /* STATIC_EXTERN 0x5cc6c0: t */
extern int ScoreMain_h; /* STATIC_EXTERN 0x5cc6c4: h */
extern int ScoreMain_p; /* STATIC_EXTERN 0x5cc6c8: p */
extern int ScoreMain_wait; /* STATIC_EXTERN 0x5cc6cc: wait */
extern int ScoreMain_total; /* STATIC_EXTERN 0x5cc6d0: total */
extern int temphp[2];
extern int tempperfect[2];
extern int temptime[2];
int GetWinnerSide(void);
char *G_sprintf_arg1(char *str, int arg1);
int IsRoundEnd(void);
int Score_GetScorerate(int side);
void AddScoreMain(int side, int i);
int ChkCancel(void);
void FontDraw(char *str, int x, int y, float z, int alfa);
int sound_request(int no);

void ScoreMain(void)
{
    int i;

    if (EnterFightCount == 0) {
        temphp[GetWinnerSide()] += PlayerWork[GetWinnerSide()].HitPoint * 50000 / PLAYER_HITPOINTMAX * Score_GetScorerate(GetWinnerSide());
        if (BattleTime > 0) {
            temptime[GetWinnerSide()] += (BattleTime / 60) * 500 * Score_GetScorerate(GetWinnerSide());
        }
        if (!IsRoundEnd()) {
            goto end;
        }
        if (g_info.mode & 0xEA00) {
            goto end;
        }
        ScoreMain_time = 60;
        if (BattleTime < 0) {
            ScoreMain_t = 0;
        } else {
            ScoreMain_t = temptime[GetWinnerSide()];
        }
        ScoreMain_h = temphp[GetWinnerSide()];
        ScoreMain_p = tempperfect[GetWinnerSide()];
        ScoreMain_h -= ScoreMain_h % 100;
        ScoreMain_wait = 60;
        ScoreMain_tp = 0;
        EnterFightCount++;
        ScoreMain_hp = 0;
        ScoreMain_pp = 0;
        ScoreMain_total = 0;
    }
    if (!(ScoreMain_tp == ScoreMain_t && ScoreMain_hp == ScoreMain_h && ScoreMain_time == 0)) {
        if (ScoreMain_tp != ScoreMain_t) {
            if (--ScoreMain_wait == -1) {
                ScoreMain_wait = 1;
                ScoreMain_tp = Accumrater(ScoreMain_t, ScoreMain_tp);
                if (ScoreMain_tp == ScoreMain_t) {
                    ScoreMain_wait = 30;
                } else {
                    sound_request(0x2F);
                }
            }
            ScoreMain_total = ScoreMain_hp + ScoreMain_tp;
        } else if (ScoreMain_hp != ScoreMain_h) {
            if (--ScoreMain_wait == -1) {
                ScoreMain_wait = 1;
                ScoreMain_hp = Accumrater(ScoreMain_h, ScoreMain_hp);
                sound_request(0x2F);
            }
            ScoreMain_total = ScoreMain_hp + ScoreMain_tp;
        } else if (ScoreMain_pp != ScoreMain_p && ScoreMain_p > 0) {
            if (--ScoreMain_wait == -1) {
                ScoreMain_wait = 1;
                ScoreMain_pp = Accumrater(ScoreMain_p, ScoreMain_pp);
                sound_request(0x2F);
            }
            ScoreMain_total = ScoreMain_hp + ScoreMain_tp + ScoreMain_pp;
        } else {
            ScoreMain_time--;
        }
        if (ChkCancel()) {
            ScoreMain_hp = ScoreMain_h;
            ScoreMain_tp = ScoreMain_t;
            ScoreMain_pp = ScoreMain_p;
            ScoreMain_total = ScoreMain_hp + ScoreMain_tp + ScoreMain_pp;
        }
        FontDraw("TIME BONUS", 0xB4, 0x78, 9.0f, 0xFF);
        FontDraw(G_sprintf_arg1("%7d", ScoreMain_tp), 0x1AC, 0x78, 9.0f, 0xFF);
        FontDraw("LIFE BONUS", 0xB4, 0xA0, 9.0f, 0xFF);
        FontDraw(G_sprintf_arg1("%7d", ScoreMain_hp), 0x1AC, 0xA0, 9.0f, 0xFF);
        if (ScoreMain_p > 0) {
            FontDraw("PERFECT BONUS", 0xB4, 0xC8, 9.0f, 0xFF);
            FontDraw(G_sprintf_arg1("%7d", ScoreMain_pp), 0x1AC, 0xC8, 9.0f, 0xFF);
        }
        FontDraw("ALL", 0xB4, 0xF0, 9.0f, 0xFF);
        FontDraw(G_sprintf_arg1("%7d", ScoreMain_total), 0x1AC, 0xF0, 9.0f, 0xFF);
        i = 0; /* dead store: optimized away, but it keeps the last FontDraw from becoming a sibcall */

    } else {
        AddScoreMain(GetWinnerSide(), ScoreMain_total);
    end:
        EnterFightCount = 0;
        EnterFightJob = 0x11;
    }
}

void ClearReduceLimitTime(void);
int LoseActionSet(CHARACTER_WORK *offset);
void set_replay_info(void);
void RequestAiuchi(void);
int IsBattleEnd(CHARACTER_WORK *offset);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
/* DrawDispMain's function statics, extern so the verifier solves them */
extern int DrawDispMain_sprtid; /* STATIC_EXTERN 0x5cc6d4: sprtid */
extern int DrawDispMain_pal; /* STATIC_EXTERN 0x5cc6d8: pal */
extern float DrawDispMain_v; /* STATIC_EXTERN 0x5cc6dc: v */
extern float DrawDispMain_x1; /* STATIC_EXTERN 0x5cc6e0: x1 */

void DrawDispMain(void)
{
    int fightctrl_time;
    int i;

    if (EnterFightCount == 0) {
        ClearReduceLimitTime();
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        if (!IsScaleSettle()) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount < 11) {
        EnterFightCount++;
    } else if (EnterFightCount == 11) {
        if (!IsBattleEnd(&PlayerWork[0]) || !IsBattleEnd(&PlayerWork[1])) {
            return;
        }
        EnterFightCount++;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 5;
            EnterFightJob = 0;
            EnterFightCount = 0;
        } else if (g_info.mode & 0x4000) {
            if (sc.replay == 1) {
                set_replay_info();
                EnterFightJob = 0x14;
            } else {
                EnterFightJob = 0xC;
            }
            EnterFightCount = 0;
        }
    } else if (EnterFightCount == 12) {
        DrawDispMain_sprtid = CockpitImageLoad(0x2E, &DrawDispMain_pal, 0);
        EnterFightCount++;
        return;
    } else if (EnterFightCount == 13) {
        RequestAiuchi();
        LoseActionSet(&PlayerWork[0]);
        LoseActionSet(&PlayerWork[1]);
        for (i = 0; i < 2; i++) {
            nlsp[i].sprno = DrawDispMain_sprtid;
            nlsp[i].z = 3.0f;
            nlsp[i].zm_x = 1.0f;
            nlsp[i].zm_y = 0.5f;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
        }
        nlsp[0].x = 320.0f;
        nlsp[0].y = 205.0f;
        nlsp[0].v0 = 0;
        nlsp[0].v1 = 0.5f;
        nlsp[0].u0 = 1.0f;
        nlsp[0].u1 = 1.0f;
        nlsp[1].x = 320.0f;
        nlsp[1].y = 205.0f;
        nlsp[1].u0 = 0;
        nlsp[1].u1 = 0;
        nlsp[1].v0 = 0.5f;
        nlsp[1].v1 = 1.0f;
        nlsp[0].attr = 7;
        nlsp[1].attr = 5;
        DrawDispMain_v = 0;
        DrawDispMain_x1 = 0;
    }
    fightctrl_time = EnterFightCount - 13;
    if (fightctrl_time >= 0) {
        EnterFightCount++;
        if (fightctrl_time <= 120) {
            FightControl_Execute(drawtb, nlsp, fightctrl_time);
            if (fightctrl_time >= 24 && fightctrl_time < 30) {
                DrawDispMain_x1 += 5.333333333333333;
            } else if (fightctrl_time == 30) {
                DrawDispMain_x1 = 32.0f;
            }
            for (i = 0; i < 2; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
            polydraw_FBox((int)(192.0f - DrawDispMain_x1), 232.0f, 192.0f, 248.0f, 13.0f, 0xFF000000);
            polydraw_FBox(448.0f, 232.0f, (int)(DrawDispMain_x1 + 320.0f + 128.0f), 248.0f, 13.0f, 0xFF000000);
        } else {
            sc.loose = 1;
            ReqSongIn(10, 2);
            PXF_Free(DrawDispMain_sprtid, DrawDispMain_pal);
            BattleStatus = 1;
            if (g_info.mode & 0x4000) {
                EnterFightCount = 0;
                EnterFightJob = 0xC;
            } else {
                EnterFightCount = 0;
                EnterFightJob = 0x11;
            }
        }
    }
}

int WinActionDecide(CHARACTER_WORK *offset);
int WinActionSet(CHARACTER_WORK *offset);
int LoseActionSet(CHARACTER_WORK *offset);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);
void ClearReduceLimitTime(void);
void set_replay_info(void);
void SetScreenFlag(int flag);

void LoseDispMain(void)
{
    int i;
    int fightctrl_time;
    NLVERTEX00 *vt;
    static int sprtid;
    static int pal;
    static float v = 0;

    vt = (NLVERTEX00 *)&nlsp[5];
    if (EnterFightCount == 0) {
        if (GetWinnerSide() == 0) {
            SetScreenFlag(0x2D);
        } else {
            SetScreenFlag(0x2E);
        }
        ClearReduceLimitTime();
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        if (!IsScaleSettle()) {
            return;
        }
        if (WinActionDecide(&PlayerWork[GetWinnerSide()])) {
            EnterFightCount++;
        } else {
            EnterFightCount += 2;
        }
    } else if (EnterFightCount == 2) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount == 3) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        WinActionSet(&PlayerWork[GetWinnerSide()]);
        LoseActionSet(GetWinnerSide() ? &PlayerWork[0] : &PlayerWork[1]);
        EnterFightCount++;
    } else if (EnterFightCount < 13) {
        EnterFightCount++;
    } else if (EnterFightCount == 13) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        sc.loose = 1;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 5;
            EnterFightJob = 0;
            EnterFightCount = 0;
        } else if (g_info.mode & 0x4000) {
            if (sc.replay == 1) {
                set_replay_info();
                EnterFightJob = 0x14;
            } else {
                EnterFightJob = 0xC;
            }
            EnterFightCount = 0;
        }
        ReqSongIn(10, 2);
        if (!(g_info.mode & 0x100)) {
            RequestResult(GetWinnerSide(), 0);
            sprtid = CockpitImageLoad(0x2D, &pal, 0);
            EnterFightCount++;
        }
    } else if (EnterFightCount == 14) {
        for (i = 0; i < 2; i++) {
            nlsp[i].sprno = sprtid;
            nlsp[i].z = 3.0f;
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
        }
        nlsp[0].y = 205.0f;
        nlsp[0].x = 766.0f;
        SetUVZ(&nlsp[0], 0, 0, 0x80, 0x40);
        nlsp[1].y = 205.0f;
        nlsp[1].x = -126.0f;
        SetUVZ(&nlsp[1], 0, 0x40, 0x80, 0x40);
        vt[0].y = 224.0f;
        vt[2].x = 320.0f;
        vt[1].y = 224.0f;
        for (i = 0; i < 3; i++) {
            vt[i].z = 4.0f;
            vt[i].col = 0xFF830131;
        }
        v = 0;
        EnterFightCount++;
    }
    fightctrl_time = EnterFightCount - 15;
    if (fightctrl_time >= 0) {
        EnterFightCount++;
        if (fightctrl_time <= 120) {
            FightControl_Execute(losetb, nlsp, fightctrl_time);
            if (fightctrl_time >= 21 && fightctrl_time < 41) {
                vt[0].x = 320.0 - v * 255.0;
                vt[1].x = v * 255.0 + 320.0;
                vt[2].y = v * 32.0 + 224.0;
                nlDispStripTrnsl(3, vt);
                v += 0.05;
            } else if (fightctrl_time > 40) {
                vt[0].x = 65.0f;
                vt[1].x = 575.0f;
                vt[2].y = 256.0f;
                nlDispStripTrnsl(3, vt);
            }
            gSpriteDraw_NaomiLibVersion(&nlsp[0]);
            gSpriteDraw_NaomiLibVersion(&nlsp[1]);
        } else {
            PXF_Free(sprtid, pal);
            BattleStatus = 1;
            EnterFightJob = 0x11;
            EnterFightCount = 0;
        }
    }
}

void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
int Score_GetScorerate(int side);
extern int tempperfect[2];

void PerfectDispMain(void)
{
    int fightctrl_time;
    int i;
    static int sprtid1;
    static int pal;
    static float v;
    static float w;
    static char a;

    if (EnterFightCount == 0) {
        FightThreadFlag |= 1;
        if (!IsScaleSettle()) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount < 10) {
        EnterFightCount++;
    } else if (EnterFightCount == 10) {
        sprtid1 = CockpitImageLoad(0x30, &pal, 0);
        EnterFightCount++;
    } else if (EnterFightCount == 11) {
        for (i = 0; i < 4; i++) {
            nlsp[i].sprno = sprtid1;
            nlsp[i].z = 5.0f;
            nlsp[i].zm_x = 0;
            nlsp[i].zm_y = 0.25f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 1.0f;
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
        }
        nlsp[0].x = 64.0f;
        nlsp[0].y = 186.0f;
        nlsp[0].u1 = 0;
        nlsp[0].v0 = 0;
        nlsp[0].v1 = 0.25f;
        nlsp[1].x = 320.0f;
        nlsp[1].y = 186.0f;
        nlsp[1].u1 = 0;
        nlsp[1].v0 = 0.25f;
        nlsp[1].v1 = 0.5f;
        nlsp[2].x = 320.0f;
        nlsp[2].y = 182.0f;
        nlsp[2].z = 7.0f;
        nlsp[2].u0 = 1.0f;
        nlsp[2].v0 = 0.5f;
        nlsp[2].v1 = 0.75f;
        nlsp[2].attr = 7;
        nlsp[3].x = 320.0f;
        nlsp[3].y = 182.0f;
        nlsp[3].z = 7.0f;
        nlsp[3].u0 = 1.0f;
        nlsp[3].v0 = 0.5f;
        nlsp[3].v1 = 0.75f;
        nlsp[3].attr = 0x15;
        v = 0;
        w = 0;
        a = 0;
        EnterFightCount++;
    }
    fightctrl_time = EnterFightCount - 12;
    if (fightctrl_time >= 0) {
        EnterFightCount++;
        if (fightctrl_time <= 120) {
            if ((g_info.mode & 0x200) && fightctrl_time == 40) {
                int exp = sv_info->sv_ext.survival_level + 200;

                sv_info->sv_ext.survival_exp += exp;
            }
            FightControl_Execute(perfecttb, nlsp, fightctrl_time);
            if (fightctrl_time >= 24 && fightctrl_time < 32) {
                v += 32.0;
            } else if (fightctrl_time >= 32 && fightctrl_time < 40) {
                w += 32.0;
            }
            if (fightctrl_time >= 24 && fightctrl_time < 32) {
                polydraw_GBox((int)(v + 32.0f), 186.0f, (int)(v + 64.0f), 250.0f, 3.0f, 0xFFFFFF, 0xFFFFFFFF, 0xFFFFFF, 0xFFFFFFFF);
                polydraw_GBox((int)(v + 64.0f), 186.0f, (int)(v + 96.0f), 250.0f, 3.0f, 0xFFFFFFFF, 0xFFFFFF, 0xFFFFFFFF, 0xFFFFFF);
            } else if (fightctrl_time >= 32 && fightctrl_time < 40) {
                polydraw_GBox((int)(w + 288.0f), 186.0f, (int)(w + 320.0f), 250.0f, 3.0f, 0xFFFFFF, 0xFFFFFFFF, 0xFFFFFF, 0xFFFFFFFF);
                polydraw_GBox((int)(w + 320.0f), 186.0f, (int)(w + 352.0f), 250.0f, 3.0f, 0xFFFFFFFF, 0xFFFFFF, 0xFFFFFFFF, 0xFFFFFF);
            }
            if (fightctrl_time >= 41 && fightctrl_time < 44) {
                a += 63.75;
            } else if (fightctrl_time == 44) {
                a = -1;
            } else if (fightctrl_time >= 45 && fightctrl_time < 52) {
                a -= 31.875;
            }
            if (fightctrl_time >= 41 && fightctrl_time < 52) {
                polydraw_FBox(0, 0, 640.0f, 480.0f, 7.0f, (a << 24) | 0xFFFFFF);
            }
            for (i = 0; i < 4; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
        } else {
            tempperfect[GetWinnerSide()] += Score_GetScorerate(GetWinnerSide()) * 80000;
            PXF_Free(sprtid1, pal);
            BattleStatus = 1;
            EnterFightJob = 0x13;
            EnterFightCount = 0;
        }
    }
}

void debugf(char *format, ...);
action_t *get_act(void);
int EnterActionSet_story(CHARACTER_WORK *offset, int side);
int EnterActionSet(CHARACTER_WORK *offset);
int GetEntryOrder(void);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void ggx_bgm_start(void);
int QuickConti_Execute(int frm);
int GalleryRetry_Execute(int frm);
void sv_IntIrqMain(void);
int IsFadeEnd(void);
int IsScaleSettle(void);
int IsRannyuu(void);
void TimeOverMain(void);
void LoseDispMain(void);
void DrawDispMain(void);
void PerfectDispMain(void);
void ScoreMain(void);
void DKODispMain(void);

void EnterFight_Main(void)
{
    int i;

    switch (EnterFightJob) {
    case 1:
        if ((g_info.mode & 0x2000) && get_act()) {
            if (EnterFightCount == 0) {
                for (i = 0; i < 2; i++) {
                    if (story.ini_pos[i].posx != 0 || story.ini_pos[i].posy != 0) {
                        PlayerWork[i].posx = story.ini_pos[i].posx;
                        PlayerWork[i].posy = story.ini_pos[i].posy;
                        debugf("*** Init Position posx = %d, posy = %d ***\n", PlayerWork[i].posx, PlayerWork[i].posy);
                    }
                }
                EnterFightCount = 0;
                EnterFightJob = 2;
            }
        }
        if (g_info.mode & 0x8000) {
            if (EnterFightCount < 91) {
                EnterFightCount++;
                break;
            }
            EnterFightCount = 0;
        } else {
            if (!IsScaleSettle()) {
                return;
            }
            if (!IsFadeEnd()) {
                return;
            }
        }
        EnterFightCount = 0;
        EnterFightJob = 2;
    case 2:
        if ((g_info.mode & 0x2000) && get_act()) {
            if (EnterFightCount == 0) {
                BattleStatus = 2;
                PlayerWork[0].ply->CPU = PlayerWork[1].ply->CPU = 0;
                BattleTimeStopFlag = 1;
                story.key_old[1] = 0;
                story.key_old[0] = 0;
            }
            EnterActionSet_story(PlayerWork, story.self_side);
            EnterActionSet_story(PlayerWork, !story.self_side);
            if (get_act()[story.actind[0]].kind == -1 && get_act()[story.actind[1]].kind == -1) {
                BattleStatus = 1;
                (story.self_side ? &PlayerWork[0] : &PlayerWork[1])->ply->CPU = 1;
                EnterFightJob = 3;
                story.actind[1] = 1;
                EnterFightCount = 0;
                story.is_enter = 0;
                story.actind[0] = 0;
                story.actcnt[0] = 0;
                story.actcnt[1] = 0;
                break;
            }
            EnterFightCount++;
            break;
        }
        if (EnterFightCount == 0) {
            EnterActionSet(&PlayerWork[GetEntryOrder()]);
        } else if (EnterFightCount == 30) {
            i = GetEntryOrder();
            EnterActionSet(i ? PlayerWork : PlayerWork + 1);
        }
        if (EnterFightCount++ > 30) {
            EnterFightCount = 0;
            EnterFightJob = 3;
        }
        break;
    case 3:
        if (EnterFightCount == 0) {
            g_info.BattleControlFlag &= ~4;
            EnterFightCount = 1;
        }
        if (!IsBattleEnd(&PlayerWork[0]) || !IsBattleEnd(&PlayerWork[1])) {
            if (IsRannyuu()) {
                EnterFightCount = 0;
                EnterFightJob = 0x1C;
            }
            break;
        }
        if ((g_info.mode & 0x200) && sv_info->sv_ext.battle) {
            if (UseCharacter1p2p[sv_info->ply == 0] == 1) {
                SetAttackActNo(sv_info->ply ? &PlayerWork[0] : &PlayerWork[1], 0x408005F);
            }
        }
        ggx_bgm_start();
        EnterFightCount = 0;
        EnterFightJob = 4;
        break;
    case 4:
        BeforeFight_Init();
        if (EnterFightJob == 5) {
            BeforeFight_Heaven();
        } else if (EnterFightJob == 6) {
            BeforeFight_Black();
        }
        break;
    case 5:
        BeforeFight_Heaven();
        break;
    case 6:
        BeforeFight_Black();
        break;
    case 7:
        BeforeFight_Duel();
        break;
    case 8:
        BeforeFight_Rock();
        break;
    case 9:
        BeforeFight_Skip();
        break;
    case 10:
        AfterFightMain();
        break;
    case 11:
        ExitDemoMain();
        break;
    case 12:
        ExitFightMain();
        break;
    case 13:
        NextFightMain();
        break;
    case 17:
        RoundEndMain();
        break;
    case 14:
    case 15:
        EnterFightCount = QuickConti_Execute(EnterFightCount);
        if (!EnterFightCount) {
            EnterFightJob = 0;
        }
        break;
    case 16:
        EnterFightCount = GalleryRetry_Execute(EnterFightCount);
        if (!EnterFightCount) {
            EnterFightJob = 0;
        }
        break;
    case 18:
        PerfectDispMain();
        break;
    case 19:
        ScoreMain();
        break;
    case 20:
        ReplaySave();
        break;
    case 24:
        DKODispMain();
        break;
    case 21:
        DrawDispMain();
        break;
    case 23:
        LoseDispMain();
        break;
    case 22:
        WinnerDispMain();
        break;
    case 26:
        DestroyMain();
        break;
    case 27:
        DestroyMayMain();
        break;
    case 25:
        TimeOverMain();
        break;
    case 28:
        IntIrqMain();
        break;
    case 29:
        sv_IntIrqMain();
        break;
    }
}

int WinActionDecide(CHARACTER_WORK *offset);
int WinActionSet(CHARACTER_WORK *offset);
int LoseActionSet(CHARACTER_WORK *offset);
void ClearReduceLimitTime(void);
void set_replay_info(void);
void SetScreenFlag(int flag);
void sv_hoimi_set(void);
void sv_hoimi(void);

void WinnerDispMain(void)
{
    static int sprtid;
    static int sprtid2;
    static int pal;
    static int wait;
    int fightctrl_time;
    int i;

    if (EnterFightCount == 0) {
        if (g_info.mode & 0x200) {
            sv_hoimi_set();
            if (sv_info->sv_ext.survival_level == 99) {
                sv_info->sv_ext.allclear = 1;
            }
        }
        if (GetWinnerSide() == 0) {
            SetScreenFlag(0x2D);
        } else {
            SetScreenFlag(0x2E);
        }
        ClearReduceLimitTime();
        EnterFightCount++;
    } else if (EnterFightCount == 1) {
        if (!IsScaleSettle()) {
            return;
        }
        if (WinActionDecide(&PlayerWork[GetWinnerSide()])) {
            EnterFightCount++;
        } else {
            EnterFightCount += 2;
        }
    } else if (EnterFightCount == 2) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount == 3) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount == 4) {
        WinActionSet(&PlayerWork[GetWinnerSide()]);
        LoseActionSet(GetWinnerSide() ? &PlayerWork[0] : &PlayerWork[1]);
        EnterFightCount++;
    } else if (EnterFightCount < 14) {
        EnterFightCount++;
    } else if (EnterFightCount == 14) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        if ((g_info.mode & 0x100) && (UseCharacter1p2p[GetWinnerSide()] == 10 || UseCharacter1p2p[GetWinnerSide()] == 11)) {
            PlayerWork[GetWinnerSide()].actst &= 0xBFFFFFFF;
            EnterFightCount++;
        } else {
            EnterFightCount += 2;
        }
    } else if (EnterFightCount == 15) {
        if (!IsBattleEnd(&PlayerWork[GetWinnerSide()])) {
            return;
        }
        EnterFightCount++;
    } else if (EnterFightCount == 16) {
        ReqSongIn(10, 2);
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 5;
            EnterFightJob = 0;
            EnterFightCount = 0;
        } else if (g_info.mode & 0x4000) {
            if (sc.replay == 1) {
                set_replay_info();
                EnterFightJob = 0x14;
            } else {
                EnterFightJob = 0xC;
            }
            EnterFightCount = 0;
        } else {
            RequestResult(GetWinnerSide(), 0);
            sprtid = CockpitImageLoad(0x2C, &pal, 0);
            EnterFightCount++;
        }
    } else if (EnterFightCount == 17) {
        sprtid2 = CockpitImageLoad(UseCharacter1p2p[GetWinnerSide()] - 1, &pal, 1);
        EnterFightCount++;
    } else if (EnterFightCount == 18) {
        for (i = 0; i < 3; i++) {
            nlsp[i].sprno = sprtid;
            nlsp[i].y = 176.0f;
            nlsp[i].z = 5.0f;
            nlsp[i].zm_x = 1.0f;
            nlsp[i].zm_y = 0.5f;
            nlsp[i].v0 = 0;
            nlsp[i].v1 = 0.5f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 1.0f;
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 0;
        }
        EnterFightCount++;
    } else if (EnterFightCount == 19) {
        nlsp[0].x = 64.0f;
        nlsp[2].x = 192.0f;
        nlsp[1].x = 320.0f;
        for (i = 3; i < 7; i++) {
            nlsp[i].sprno = sprtid;
            nlsp[i].z = 9.0f;
            nlsp[i].zm_x = 0;
            nlsp[i].zm_y = 0.25f;
            nlsp[i].v0 = 0.5f;
            nlsp[i].v1 = 0.75f;
            nlsp[i].u0 = 1.0f;
            nlsp[i].u1 = 1.0f;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
            nlsp[i].x = 320.0f;
        }
        nlsp[3].attr = 5;
        nlsp[3].y = 190.0f;
        nlsp[5].y = 225.0f;
        nlsp[4].attr = 0x17;
        nlsp[5].attr = 0x25;
        nlsp[6].attr = 0x37;
        nlsp[4].y = 190.0f;
        nlsp[6].y = 225.0f;
        EnterFightCount++;
    } else if (EnterFightCount == 20) {
        for (i = 7; i < 9; i++) {
            nlsp[i].sprno = sprtid;
            nlsp[i].z = 7.0f;
            nlsp[i].zm_x = 0;
            nlsp[i].zm_y = 0.25f;
            nlsp[i].v0 = 0.75f;
            nlsp[i].v1 = 1.0f;
            nlsp[i].u0 = 1.0f;
            nlsp[i].u1 = 1.0f;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
            nlsp[i].x = 320.0f;
        }
        nlsp[7].y = 209.0f;
        nlsp[7].attr = 5;
        nlsp[8].attr = 0x17;
        nlsp[8].y = 209.0f;
        EnterFightCount++;
    } else if (EnterFightCount == 21) {
        for (i = 9; i < 11; i++) {
            nlsp[i].sprno = sprtid2;
            nlsp[i].z = 3.0f;
            nlsp[i].zm_x = 1.0f;
            nlsp[i].zm_y = 0.5f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 1.0f;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 0;
            nlsp[i].y = 261.0f;
        }
        nlsp[9].attr = 5;
        nlsp[9].v0 = 0;
        nlsp[9].v1 = 0.5f;
        nlsp[10].attr = 5;
        nlsp[10].v0 = 0.5f;
        nlsp[10].v1 = 1.0f;
        nlsp[9].x = 320.0f;
        nlsp[10].x = 64.0f;
        wait = 0;
        EnterFightCount++;
    } else {
        fightctrl_time = EnterFightCount - 22;
        EnterFightCount++;
        if (fightctrl_time < 120) {
            FightControl_Execute(Winnertb, nlsp, fightctrl_time);
            for (i = 0; i < 11; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
            if (BattleStatus == 1) {
                EnterFightJob = 0;
                EnterFightCount = 0;
            }
        } else {
            BattleStatus = 1;
            PXF_Free(sprtid, pal);
            PXF_Free(sprtid2, pal);
            if (g_info.mode & 0x8000) {
                if (PlayerWork[GetWinnerSide()].HitPoint >= PLAYER_HITPOINTMAX
                    && (BattleTime > 0 || g_info.TimeByCount == -1 || gl_info->time == -1) && !(gl_info->clear & 2)) {
                    EnterFightJob = 0x12;
                } else {
                    EnterFightJob = 0x13;
                }
            } else {
                if (PlayerWork[GetWinnerSide()].HitPoint >= PLAYER_HITPOINTMAX
                    && (BattleTime > 0 || g_info.TimeByCount == -1 || userinfo.roundtime == 3)
                    && (!(g_info.mode & 0x200) || sv_info->oldhp >= PLAYER_HITPOINTMAX)) {
                    if (g_info.mode & 0x2000) {
                        story.is_perfect = 1;
                    }
                    EnterFightJob = 0x12;
                } else {
                    if (g_info.mode & 0x2000) {
                        story.is_perfect = 0;
                    }
                    EnterFightJob = 0x13;
                }
            }
            EnterFightCount = 0;
        }
    }
    if (EnterFightCount >= 19 && (g_info.mode & 0x200)) {
        sv_hoimi();
    }
}

void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
int IsRannyuu(void);
void IntIrqMain(void);

void BeforeFight_Duel(void)
{
    int fightctrl_time;
    int i;
    static int sprtid1;
    static int sprtid2;
    static int sprtid3;
    static int u0;
    static int v0;
    static int pal;
    static int dcount;
    static float x1;
    static float x2;
    static float x3;
    static float h;
    unsigned char fu[9] = {0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80};
    unsigned char fv[9] = {0x20, 0x60, 0x40, 0x80, 0x00, 0x20, 0xA0, 0xC0, 0x40};
    unsigned char fw[9] = {0x80, 0xC0, 0x80, 0xC0, 0x80, 0x80, 0xC0, 0xC0, 0x80};

    if (EnterFightCount == 0) {
        g_info.BattleControlFlag &= ~4;
        dcount = ++g_info.Round - 1;
        sprtid1 = CockpitImageLoad(0x37, &pal, 0);
        polydraw_FBox(0, 0, 640.0f, 480.0f, 15.0f, 0x99000000);
    } else if (EnterFightCount == 1) {
        polydraw_FBox(0, 0, 640.0f, 480.0f, 15.0f, 0x99000000);
    } else if (EnterFightCount == 2) {
        if (final == 0) {
            sprtid2 = CockpitImageLoad(0x38, &pal, 1);
        } else {
            sprtid2 = CockpitImageLoad(0x39, &pal, 1);
            dcount = 8;
        }
        polydraw_FBox(0, 0, 640.0f, 480.0f, 15.0f, 0x99000000);
    } else if (EnterFightCount == 3) {
        polydraw_FBox(0, 0, 640.0f, 480.0f, 15.0f, 0x99000000);
    } else if (EnterFightCount == 4) {
        sprtid3 = CockpitImageLoad(0x3A, &pal, 2);
        polydraw_FBox(0, 0, 640.0f, 480.0f, 15.0f, 0x99000000);
    } else if (EnterFightCount == 5) {
        polydraw_FBox(0, 0, 640.0f, 480.0f, 15.0f, 0x99000000);
    } else if (EnterFightCount == 6) {
        for (i = 0; i < 15; i++) {
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 0;
            nlsp[i].z = 9.0f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 0;
            nlsp[i].v0 = 0;
            nlsp[i].v1 = 0;
            nlsp[i].zm_x = 0;
            nlsp[i].zm_y = 0;
        }
        nlsp[0].sprno = sprtid1;
        nlsp[0].x = 295.0f;
        nlsp[0].y = 87.0f;
        nlsp[0].z = 3.0f;
        nlsp[0].attr = 7;
        SetUVZ(&nlsp[0], 0, 0, 0, 0x40);
        nlsp[1].sprno = sprtid1;
        nlsp[1].x = 294.0f;
        nlsp[1].y = 145.0f;
        nlsp[1].z = 3.0f;
        nlsp[1].attr = 0xD;
        SetUVZ(&nlsp[1], 0x40, 0x1A, 0x20, 0);
        nlsp[2].sprno = sprtid1;
        nlsp[2].z = 3.0f;
        nlsp[2].x = 299.0f;
        nlsp[2].y = 203.0f;
        nlsp[2].attr = 0xD;
        nlsp[3].sprno = sprtid3;
        nlsp[3].x = 73.0f;
        nlsp[3].z = 5.0f;
        nlsp[3].y = 317.0f;
        nlsp[3].attr = 9;
        SetUVZ(&nlsp[3], 0, 0, 0x80, 0x20);
        nlsp[4].sprno = sprtid3;
        nlsp[4].x = 198.0f;
        nlsp[4].z = 5.0f;
        nlsp[4].y = 317.0f;
        nlsp[4].attr = 9;
        SetUVZ(&nlsp[4], fu[dcount], fv[dcount], fw[dcount], 0x20);
        nlsp[5].sprno = sprtid3;
        nlsp[5].x = 253.0f;
        nlsp[5].z = 5.0f;
        nlsp[5].y = 317.0f;
        nlsp[5].attr = 9;
        SetUVZ(&nlsp[5], 0, 0, 0x80, 0x20);
        nlsp[6].sprno = sprtid3;
        nlsp[6].z = 5.0f;
        nlsp[6].x = 378.0f;
        nlsp[6].y = 317.0f;
        nlsp[6].attr = 9;
        SetUVZ(&nlsp[6], fu[dcount], fv[dcount], fw[dcount], 0x20);
        nlsp[7].sprno = sprtid2;
        nlsp[7].z = 7.0f;
        nlsp[7].trnsl = 1.0f;
        nlsp[7].attr = 7;
        if (final == 0) {
            nlsp[7].x = 320.0f;
            nlsp[7].y = 256.0f;
            if (dcount < 4) {
                u0 = dcount * 64;
            } else {
                u0 = (dcount - 4) * 64;
            }
            if (dcount < 4) {
                v0 = 0;
            } else {
                v0 = 0x80;
            }
            uvduel.u = u0;
            uvduel.v = v0;
            uvduel.w = 0x40;
            uvduel.h = 0x80;
        } else {
            uvduel.v = 0;
            uvduel.w = 0x80;
            uvduel.h = 0x80;
            nlsp[7].x = 312.0f;
            nlsp[7].y = 256.0f;
            uvduel.u = 0;
            nlsp[8].sprno = sprtid2;
            nlsp[8].x = 1152.0f;
            nlsp[8].y = 265.0f;
            nlsp[8].z = 7.0f;
            nlsp[8].trnsl = 1.0f;
            nlsp[8].attr = 7;
            SetUVZ(&nlsp[8], 0, 0x80, 0x100, 0x80);
        }
        x1 = 320.0f;
        x2 = 320.0f;
        x3 = 0;
        h = 1.0f;
    }
    fightctrl_time = EnterFightCount - 6;
    EnterFightCount++;
    if (fightctrl_time >= 0) {
        if (fightctrl_time <= 50) {
            if (IsRannyuu()) {
                PXF_Free(sprtid1, pal);
                PXF_Free(sprtid2, pal);
                PXF_Free(sprtid3, pal);
                EnterFightCount = 0;
                EnterFightJob = 0x1C;
                IntIrqMain();
                return;
            }
            FightControl_Execute(dueltb, nlsp, fightctrl_time);
            if (fightctrl_time >= 0 && fightctrl_time < 6) {
                x1 -= 11.0;
                x2 += 11.0;
            } else if (fightctrl_time == 6) {
                x1 = 254.0f;
                x2 = 386.0f;
            } else if (fightctrl_time >= 46 && fightctrl_time < 50) {
                x1 -= 63.5;
                x2 += 63.5;
            }
            if (fightctrl_time >= 18 && fightctrl_time < 20) {
                x3 += 320.0;
            } else if (fightctrl_time == 19) {
                x3 = 640.0f;
            } else if (fightctrl_time >= 20 && fightctrl_time < 24) {
                h += 4.0;
            } else if (fightctrl_time == 23) {
                h = 16.0f;
            } else if (fightctrl_time >= 46 && fightctrl_time < 50) {
                h -= 4.0;
            } else if (fightctrl_time == 49) {
                h = 0;
            }
            if (final == 0) {
                FightControl_Execute(duel1_8, nlsp, fightctrl_time);
            } else {
                FightControl_Execute(fduel, nlsp, fightctrl_time);
            }
            polydraw_FBox(0, 0, x1, 480.0f, 13.0f, 0x99000000);
            polydraw_FBox(x2, 0, 640.0f, 480.0f, 13.0f, 0x99000000);
            if (fightctrl_time >= 18 && fightctrl_time < 50) {
                polydraw_FBox(0, (int)(317.0f - h), x3, 317.0f, 9.0f, 0xCC988830);
                polydraw_FBox(0, 317.0f, x3, (int)(h + 317.0f), 9.0f, 0xCC988830);
            }
            for (i = 0; i < 9; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
        } else {
            PXF_Free(sprtid1, pal);
            PXF_Free(sprtid2, pal);
            PXF_Free(sprtid3, pal);
            EnterFightCount = 0;
            EnterFightJob = 8;
        }
    }
}

void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void polydraw_PBox(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, unsigned int color1);
int IsRannyuu(void);
void IntIrqMain(void);

void BeforeFight_Heaven(void)
{
    int fightctrl_time;
    int i;
    static int sprtid1;
    static int sprtid2;
    static int pal;
    static float v;
    static float x1;
    static float x2;
    static float y;

    if (EnterFightCount == 0) {
        sprtid1 = CockpitImageLoad(0x35, &pal, 0);
    } else if (EnterFightCount == 1) {
        sprtid2 = CockpitImageLoad(0x36, &pal, 1);
    } else if (EnterFightCount == 2) {
        for (i = 0; i < 15; i++) {
            if (i < 6) {
                nlsp[i].sprno = sprtid1;
            } else {
                nlsp[i].sprno = sprtid2;
            }
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
            nlsp[i].z = 3.0f;
            nlsp[i].u0 = 0;
            nlsp[i].u1 = 0;
            nlsp[i].v0 = 0;
            nlsp[i].v1 = 0;
            nlsp[i].zm_x = 0;
            nlsp[i].zm_y = 0;
        }
        nlsp[0].y = 176.0f;
        nlsp[0].x = 218.0f;
        nlsp[0].attr = 7;
        nlsp[1].x = 422.0f;
        nlsp[1].y = 173.0f;
        nlsp[2].x = 223.0f;
        nlsp[2].y = 258.0f;
        nlsp[3].x = 346.0f;
        nlsp[3].y = 258.0f;
        nlsp[4].x = 263.0f;
        nlsp[4].y = 219.0f;
        nlsp[5].x = 306.0f;
        nlsp[5].y = 221.0f;
        nlsp[6].x = 320.0f;
        nlsp[6].y = 263.0f;
        nlsp[6].attr = 6;
        nlsp[6].trnsl = 0;
        SetUVZ(&nlsp[6], 0x80, 0, 0x80, 0x40);
        nlsp[7].x = 214.0f;
        nlsp[7].y = 333.0f;
        nlsp[7].attr = 0xD;
        nlsp[8].x = 299.0f;
        nlsp[8].y = 333.0f;
        nlsp[8].attr = 0xD;
        nlsp[9].x = 215.0f;
        nlsp[9].y = 329.0f;
        nlsp[10].x = 425.0f;
        nlsp[10].y = 329.0f;
        nlsp[10].attr = 7;
        nlsp[11].x = 11.0f;
        nlsp[11].y = 240.0f;
        nlsp[11].attr = 0xD;
        nlsp[11].trnsl = 0;
        SetUVZ(&nlsp[11], 0, 0x100, 0x20, 0);
        nlsp[11].v0 = 1.0f;
        nlsp[11].v1 = 1.0f;
        nlsp[12].x = 11.0f;
        nlsp[12].y = 240.0f;
        nlsp[12].trnsl = 0;
        SetUVZ(&nlsp[12], 0x20, 0, 0x20, 0);
        nlsp[12].v0 = 0;
        nlsp[12].v1 = 0;
        nlsp[13].x = 596.0f;
        nlsp[13].y = 240.0f;
        nlsp[13].attr = 0xD;
        nlsp[13].trnsl = 0;
        SetUVZ(&nlsp[13], 0x40, 0x100, 0x20, 0);
        nlsp[13].v0 = 1.0f;
        nlsp[13].v1 = 1.0f;
        nlsp[14].x = 596.0f;
        nlsp[14].y = 240.0f;
        nlsp[14].trnsl = 0;
        SetUVZ(&nlsp[14], 0x60, 0, 0x20, 0);
        nlsp[14].v0 = 0;
        nlsp[14].v1 = 0;
        v = 0;
        x1 = nlsp[14].v0;
        x2 = 640.0f;
        y = 16.0f;
    }
    fightctrl_time = EnterFightCount - 2;
    EnterFightCount++;
    if (fightctrl_time < 0) {
        return;
    }
    if (fightctrl_time < 111) {
        if (IsRannyuu()) {
            PXF_Free(sprtid1, pal);
            PXF_Free(sprtid2, pal);
            EnterFightCount = 0;
            EnterFightJob = 0x1C;
            IntIrqMain();
            return;
        }
        if (fightctrl_time < 8) {
            x1 += 80.0;
            x2 -= 80.0;
            polydraw_FBox(0, 0, x1, 480.0f, 5.0f, 0x4C000000);
            polydraw_FBox(x2, 0, 640.0f, 480.0f, 5.0f, 0x4C000000);
        } else if (fightctrl_time == 90) {
            x1 = 152.0f;
            x2 = 488.0f;
        } else if (fightctrl_time >= 90 && fightctrl_time < 102) {
            x1 += 14.0;
            x2 -= 14.0;
        }
        if (fightctrl_time == 8) {
            polydraw_FBox(0, 0, 640.0f, 480.0f, 5.0f, 0x99000000);
        } else if (fightctrl_time >= 8 && fightctrl_time < 16) {
            y += 56.0;
            polydraw_FBox(0, (int)y, 640.0f, 480.0f, 5.0f, 0x99000000);
            polydraw_FBox(0, 16.0f, 48.0f, (int)y, 5.0f, 0x99000000);
            polydraw_FBox(592.0f, 16.0f, 640.0f, (int)y, 5.0f, 0x99000000);
            polydraw_PBox(48.0f, 16.0f, 592.0f, 16.0f, y / 1.7297297297297298 + 48.0, y, 592.0 - y / 1.7297297297297298, y, 5.0f, 0x99000000);
            polydraw_FBox(0, 0, 640.0f, 16.0f, 5.0f, 0x99000000);
        } else if (fightctrl_time >= 16 && fightctrl_time < 104) {
            polydraw_FBox(0, 464.0f, 640.0f, 480.0f, 5.0f, 0x99000000);
            polydraw_FBox(0, 16.0f, 48.0f, 464.0f, 5.0f, 0x99000000);
            polydraw_FBox(592.0f, 16.0f, 640.0f, 464.0f, 5.0f, 0x99000000);
            polydraw_PBox(48.0f, 16.0f, 592.0f, 16.0f, 307.0f, 464.0f, 333.0f, 464.0f, 5.0f, 0x99000000);
            polydraw_FBox(0, 0, 640.0f, 16.0f, 5.0f, 0x99000000);
            y = 16.0f;
        } else if (fightctrl_time >= 102 && fightctrl_time < 110) {
            y += 56.0;
            polydraw_FBox(0, 464.0f, 640.0f, 480.0f, 5.0f, 0x99000000);
            polydraw_FBox(0, (int)y, 48.0f, 464.0f, 5.0f, 0x99000000);
            polydraw_FBox(592.0f, (int)y, 640.0f, 464.0f, 5.0f, 0x99000000);
            polydraw_PBox(48.0f, y, 592.0f, y, (448.0f - y) / 1.7297297297297298 + 48.0, 464.0f, 592.0 - (448.0f - y) / 1.7297297297297298, 464.0f, 5.0f, 0x99000000);
            polydraw_FBox(0, 0, 640.0f, y, 5.0f, 0x99000000);
        } else if (fightctrl_time == 110) {
            polydraw_FBox(0, 0, 640.0f, 480.0f, 5.0f, 0x99000000);
        }
        FightControl_Execute(heaven, nlsp, fightctrl_time);
        if (fightctrl_time >= 94 && fightctrl_time < 106) {
            if (nlsp[0].x - x1 <= 64.0f) {
                if (nlsp[0].x - x1 > 0) {
                    SetUVZ(&nlsp[0], (int)(64.0f - (nlsp[0].x - x1)), 0, (int)(nlsp[0].x - x1), 0x80);
                    if (nlsp[0].x - x1 <= 0) {
                        SetUVZ(&nlsp[0], 0, 0, 0, 0);
                    }
                }
            }
            if (nlsp[2].x - x1 <= 64.0f) {
                if (nlsp[2].x - x1 > 0) {
                    SetUVZ(&nlsp[2], (int)(64.0f - (nlsp[2].x - x1) + 64.0f), 0, (int)(nlsp[2].x - x1), 0x80);
                    if (nlsp[2].x - x1 <= 0) {
                        SetUVZ(&nlsp[2], 0, 0, 0, 0);
                    }
                }
            }
            if (nlsp[4].x - x1 <= 64.0f) {
                if (nlsp[4].x - x1 > 0) {
                    SetUVZ(&nlsp[4], (int)(64.0f - (nlsp[4].x - x1) + 128.0f), 0, (int)(nlsp[4].x - x1), 0x80);
                    if (nlsp[4].x - x1 <= 0) {
                        SetUVZ(&nlsp[4], 0, 0, 0, 0);
                    }
                }
            }
            if (nlsp[7].x - x1 <= 128.0f) {
                if (nlsp[7].x - x1 > 0) {
                    SetUVZ(&nlsp[7], (int)(128.0f - (nlsp[7].x - x1) + 128.0f), 0x80, (int)(nlsp[7].x - x1), 0x40);
                    if (nlsp[7].x - x1 <= 0) {
                        SetUVZ(&nlsp[7], 0, 0, 0, 0);
                    }
                }
            }
            if (nlsp[9].x - x1 <= 128.0f) {
                if (nlsp[9].x - x1 > 0) {
                    SetUVZ(&nlsp[9], (int)(128.0f - (nlsp[9].x - x1) + 128.0f), 0x40, (int)(nlsp[9].x - x1), 0x40);
                    if (nlsp[9].x - x1 <= 0) {
                        SetUVZ(&nlsp[9], 0, 0, 0, 0);
                    }
                }
            }
            if (x2 - nlsp[1].x <= 64.0f) {
                if (x2 - nlsp[1].x > 0) {
                    SetUVZ(&nlsp[1], 0, 0x80, (int)(x2 - nlsp[1].x), 0x80);
                    if (x2 - nlsp[1].x <= 0) {
                        SetUVZ(&nlsp[1], 0, 0, 0, 0);
                    }
                }
            }
            if (x2 - nlsp[3].x <= 64.0f) {
                if (x2 - nlsp[3].x > 0) {
                    SetUVZ(&nlsp[3], 0x40, 0, (int)(x2 - nlsp[3].x), 0x80);
                    if (x2 - nlsp[3].x <= 0) {
                        SetUVZ(&nlsp[3], 0, 0, 0, 0);
                    }
                }
            }
            if (x2 - nlsp[5].x <= 64.0f) {
                if (x2 - nlsp[5].x > 0) {
                    SetUVZ(&nlsp[5], 0xC0, 0, (int)(x2 - nlsp[5].x), 0x80);
                    if (x2 - nlsp[5].x <= 0) {
                        SetUVZ(&nlsp[5], 0, 0, 0, 0);
                    }
                }
            }
            if (x2 - nlsp[8].x <= 128.0f) {
                if (x2 - nlsp[8].x > 0) {
                    SetUVZ(&nlsp[8], 0x80, 0xC0, (int)(x2 - nlsp[8].x), 0x40);
                    if (x2 - nlsp[8].x <= 0) {
                        SetUVZ(&nlsp[8], 0, 0, 0, 0);
                    }
                }
            }
            if (x2 - nlsp[10].x <= 128.0f) {
                if (x2 - nlsp[10].x > 0) {
                    SetUVZ(&nlsp[10], 0x80, 0xC0, (int)(x2 - nlsp[10].x), 0x40);
                    if (x2 - nlsp[10].x <= 0) {
                        SetUVZ(&nlsp[5], 0, 0, 0, 0);
                    }
                }
            }
            if (x2 - x1 <= 128.0f) {
                if (x2 - x1 > 0) {
                    SetUVZ(&nlsp[6], (int)(64.0f - (320.0f - x1) + 128.0f), 0, (int)(x2 - x1), 0x40);
                    if (x2 - x1 < 0) {
                        SetUVZ(&nlsp[6], 0, 0, 0, 0);
                    }
                }
            }
        }
        if (fightctrl_time >= 16 && fightctrl_time < 106) {
            for (i = 0; i < 15; i++) {
                gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            }
        }
    } else {
        PXF_Free(sprtid1, pal);
        PXF_Free(sprtid2, pal);
        polydraw_FBox(0, 0, 640.0f, 480.0f, 5.0f, 0x99000000);
        EnterFightCount = 0;
        EnterFightJob = 7;
    }
}
