#include "generated/opening.h"
static const short idtb[] = { 1, 2, 3 };
void RegistSpr(int id, int no);
int Opening_Seen1(int count);
int Opening_Seen2(int fms);
void Opening_Seen4(int fms);
int Opening_Seen3(int fms);
void Opening_Seen7_5(int fms);
int Opening_Seen5(int fms);
void Opening_Seen7Sub(void);
void Opening_Seen7Anime(int count);
void ScrlString(void);
int Opening_Seen7(int count);
int Opening_Seen9Sub(int fms);
int Opening_Seen9(int count);
int Opening_Seen10_00(int count);
int Opening_Seen10_01(int count);
int Opening_Seen11(int count);
int Opening_Seen12(int count);
int Opening_Seen13(int count);
int Opening_Seen14(int count);
int Opening_Seen15(int count);
void XEffectExe(void);
int Opening_Seen16(int count);
void Opening_Draw(void);
void OpeningInit(void);
int OpeningEnd(int count);
void Opening_BlankFunc(void);
int Opening_Wait1(int count);
int Opening_Flash(int count);
void OpeningMain(void);
void OpeningBgVertInit(int col);
void OpeningBgRUInit(int col);
void OpeningBgSideDraw(int dir, int fade);
void OpeningBgDrawRU(int fade);
void OpeningBgDrawLU(int fade);
void OpeningBgSideR(int fade);
void OpeningBgSideL(int fade);
extern void (*bgdrawfunctb[])();
extern int (*opfunc[])();
char *OpeningFile = 0;
extern int animetb[];
extern void (*bgdrawfunctb[])();
extern int (*opfunc[])();

static NL_SPR_ARG *opsp;
static int opcancelflag;
static int Seen7mode;
static int Seen7num;
static int Seen9Flag;
static FIGHT_CONTROL *seen9;
static int colno;
static int backcol;
static int *OpBgWorkY;
static int *OpBgWorkX;
static int *OpBgWork;
static int xecount;
int IST_Load(int *texadr, int sprid, int palid);
int IST_Load_Bt(int *IstAddrTb, unsigned int id, unsigned int palid);
void Cmd_AddB(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddO(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddTrnsl(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddU0(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddU1(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddV0(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddV1(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddZM(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddZMX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddZMY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddZX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddZY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_DebugNo(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MoveDes(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MoveU0(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MoveU1(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MoveV0(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MoveV1(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MyDesExe(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_MyDesInit(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_ReqName(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_ReqSe(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_ReqSeNum(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetAttr(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetB(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetO(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetTrnsl(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetU0(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetU1(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetUVZ(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetV0(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetV1(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetXY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetZ(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetZM(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetZMX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetZMY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_Trap(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void FightControl_Execute(FIGHT_CONTROL *tablep, NL_SPR_ARG *sp, int time);

int IST_Load(int *texadr, int sprid, int palid);

void RegistSpr(int id, int no)
{
    IST_Load((int *)OpeningFile + id, no + 0x80, -1);
    opsp[no].sprno = no + 0x80;
}

int Opening_Seen1(int count)
{
    static const FIGHT_CONTROL Seen1[] = {
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_AddTrnsl, 0.05f, 0, 0, 0, 20 },
        { 4, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 20, 200 },
        { 4, (void *)Cmd_AddTrnsl, -0.05f, 0, 0, 200, 220 },
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 220, 221 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    int fms;
    int no;

    no = count / 231;
#ifdef VERSION_JP
    if (no >= 2) return 1;
#else
    if (no >= 3) return 1;
#endif
    fms = count % 231;
    if (fms == 0) RegistSpr(no + 1, 4);
    if (fms < 221) FightControl_Execute(Seen1, opsp, fms);
    return 0;
}

int Opening_Seen2(int fms)
{
    static const FIGHT_CONTROL Seen2[] = {
        { 4, (void *)Cmd_SetX, 0.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetY, 280.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_AddTrnsl, 0.025f, 0, 0, 0, 39 },
        { 4, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 40, 79 },
        { 4, (void *)Cmd_AddTrnsl, -0.025f, 0, 0, 80, 119 },
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 119, 120 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };

    if (fms < 33) return 0;
    if (fms == 33) RegistSpr(4, 4);
    if (fms < 153) {
        FightControl_Execute(Seen2, opsp, fms - 33);
        return 0;
    }
    return 1;
}

int GetFadeFlag(void);
int GetFadeVal(void);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void Opening_Seen4(int fms)
{
    static const FIGHT_CONTROL Seen4[] = {
        { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetZ, 300.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_AddTrnsl, 0.016666668f, 0, 0, 0, 60 },
        { 0, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 60, 61 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    int col;

    if (fms == 0) RegistSpr(6, 0);
    if (fms < 60) {
        FightControl_Execute(Seen4, opsp, fms);
        if (GetFadeFlag() == 0) {
            col = (int)(opsp->trnsl * 255.0) << 24;
        } else {
            col = GetFadeVal() << 24;
        }
        polydraw_FBox(0.0f, 61.0f, 640.0f, 420.0f, 500.0f, col | 0xFFFFFF);
    }
}

int Opening_Seen3(int fms)
{
    static const FIGHT_CONTROL Seen3_1[] = {
        { 4, (void *)Cmd_SetX, 320.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetY, 240.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_AddTrnsl, 0.033333335f, 0, 0, 0, 29 },
        { 4, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 30, 120 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    static const FIGHT_CONTROL Seen3_2[] = {
        { 4, (void *)Cmd_AddTrnsl, -0.016666668f, 0, 0, 0, 60 },
        { 4, (void *)Cmd_AddZMX, 0.033333335f, 0, 0, 0, 60 },
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 59, 60 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };

    if (fms == 0) {
        RegistSpr(5, 4);
        opsp[4].attr = 0x20A;
    }
    if (fms < 120) {
        FightControl_Execute(Seen3_1, opsp, fms);
        return 0;
    }
    if (fms < 180) {
        FightControl_Execute(Seen3_2, opsp, fms - 120);
        Opening_Seen4(fms - 120);
        return 0;
    }
    opsp[4].attr = 0x20D;
    opsp[4].zm_x = 1.0f;
    return 1;
}

int GetFadeFlag(void);
int GetFadeVal(void);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void Opening_Seen7_5(int fms)
{
    static const FIGHT_CONTROL Seen7_0[] = {
        { 4, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetZ, 100.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetY, 60.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetX, 640.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_AddX, -10.666667f, 0, 0, 0, 60 },
        { 4, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 7, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 7, (void *)Cmd_SetZ, 100.0f, 0, 0, 0, 1 },
        { 7, (void *)Cmd_SetY, 60.0f, 0, 0, 0, 1 },
        { 7, (void *)Cmd_SetX, 640.0f, 0, 0, 0, 1 },
        { 7, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 8, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 8, (void *)Cmd_SetZ, 100.0f, 0, 0, 0, 1 },
        { 8, (void *)Cmd_SetY, 60.0f, 0, 0, 0, 1 },
        { 8, (void *)Cmd_SetX, 1280.0f, 0, 0, 0, 1 },
        { 8, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 5, (void *)Cmd_SetZ, 101.0f, 0, 0, 0, 1 },
        { 5, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 5, (void *)Cmd_SetX, 640.0f, 0, 0, 0, 1 },
        { 5, (void *)Cmd_SetY, 420.0f, 0, 0, 0, 1 },
        { 5, (void *)Cmd_AddX, -10.666667f, 0, 0, 0, 60 },
        { 5, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 9, (void *)Cmd_SetZ, 101.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetX, 640.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetY, 420.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 10, (void *)Cmd_SetZ, 101.0f, 0, 0, 0, 1 },
        { 10, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 10, (void *)Cmd_SetX, 1280.0f, 0, 0, 0, 1 },
        { 10, (void *)Cmd_SetY, 420.0f, 0, 0, 0, 1 },
        { 10, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    int col;

    if (fms == 0) {
        RegistSpr(10, 4);
        RegistSpr(15, 12);
        RegistSpr(16, 13);
        RegistSpr(17, 14);
        RegistSpr(18, 15);
        RegistSpr(14, 1);
        opsp[7].sprno = opsp[4].sprno;
        opsp[8].sprno = opsp[4].sprno;
        opsp[5].sprno = opsp[4].sprno;
        opsp[9].sprno = opsp[4].sprno;
        opsp[10].sprno = opsp[4].sprno;
        opsp[4].attr = 0x20D;
        opsp[7].attr = 0x20D;
        opsp[8].attr = 0x20D;
        opsp[5].attr = 0x205;
        opsp[9].attr = 0x205;
        opsp[10].attr = 0x205;
        opsp[0].z = 300.0f;
        Seen7mode = 0;
    }
    if (fms < 61) {
        FightControl_Execute(Seen7_0, opsp, fms);
        if (GetFadeFlag() == 0) {
            col = 0xFF000000;
        } else {
            col = GetFadeVal() << 24;
        }
        polydraw_FBox(0.0f, 61.0f, 640.0f, 420.0f, 500.0f, col | 0xFFFFFF);
    }
}

int GetFadeFlag(void);
int GetFadeVal(void);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

int Opening_Seen5(int fms)
{
    static const FIGHT_CONTROL Seen4[] = {
        { 0, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 0, (void *)Cmd_AddB, 0.0f, -4, 0, 0, 58 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    int col;

    if (fms == 0) {
        RegistSpr(7, 0);
    } else if (fms == 30) {
        RegistSpr(8, 0);
    } else if (fms == 60) {
        opsp->attr = 0x10D;
    }
    if (GetFadeFlag() == 0) {
        col = 0xFF000000;
    } else {
        col = GetFadeVal() << 24;
    }
    polydraw_FBox(0.0f, 61.0f, 640.0f, 420.0f, 500.0f, col | 0xFFFFFF);
    if (fms < 60) return 0;
    if (fms < 120) {
        FightControl_Execute(Seen4, opsp, fms - 60);
        Opening_Seen7_5(fms - 60);
        return 0;
    }
    Opening_Seen7_5(60);
    return 1;
}

void Opening_Seen7Sub(void)
{
    static const FIGHT_CONTROL Seen4[] = {
        { 0, (void *)Cmd_AddB, 0.0f, -4, 0, 0, 60 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };

    if (Seen7mode & 1) FightControl_Execute(Seen4, opsp, 0);
    OpeningBgSideDraw(0, 255);
}

int animetb[4] __attribute__((section(".rodata"))) = { 19, 31, 51, 61 };

void Opening_Seen7Anime(int count)
{
    static int fms;
    int tb[4] = { 12, 20, 10, 7 };

    if (count == 0) fms = 0;
    if (fms == 0) {
        if (!(Seen7mode & 2)) return;
        fms = 1;
    }
    if ((fms - 1) / 2 < tb[Seen7num]) {
        if (fms & 1) RegistSpr(animetb[Seen7num] + (fms - 1) / 2, 6);
        fms++;
    }
}

void ScrlString(void)
{
    opsp[4].x -= 5.0;
    if (opsp[4].x <= -640.0) opsp[4].x = 1280.0f;
    opsp[7].x -= 5.0;
    if (opsp[7].x <= -640.0) opsp[7].x = 1280.0f;
    opsp[8].x -= 5.0;
    if (opsp[8].x <= -640.0) opsp[8].x = 1280.0f;
    opsp[5].x -= 5.0;
    if (opsp[5].x <= -640.0) opsp[5].x = 1280.0f;
    opsp[9].x -= 5.0;
    if (opsp[9].x <= -640.0) opsp[9].x = 1280.0f;
    opsp[10].x -= 5.0;
    if (opsp[10].x <= -640.0) opsp[10].x = 1280.0f;
}

void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

int Opening_Seen7(int count)
{
    static const FIGHT_CONTROL Seen7_1[] = {
        { 4, (void *)Cmd_AddY, 12.0f, 0, 0, 0, 15 },
        { 4, (void *)Cmd_AddO, 0.0f, 17, 0, 0, 15 },
        { 4, (void *)Cmd_SetO, 0.0f, 16777215, 0, 14, 15 },
        { 4, (void *)Cmd_SetY, 258.0f, 0, 0, 14, 15 },
        { 7, (void *)Cmd_AddY, 12.0f, 0, 0, 0, 15 },
        { 7, (void *)Cmd_AddO, 0.0f, 17, 0, 0, 15 },
        { 7, (void *)Cmd_SetO, 0.0f, 16777215, 0, 14, 15 },
        { 7, (void *)Cmd_SetY, 258.0f, 0, 0, 14, 15 },
        { 8, (void *)Cmd_AddY, 12.0f, 0, 0, 0, 15 },
        { 8, (void *)Cmd_AddO, 0.0f, 17, 0, 0, 15 },
        { 8, (void *)Cmd_SetO, 0.0f, 16777215, 0, 14, 15 },
        { 8, (void *)Cmd_SetY, 258.0f, 0, 0, 14, 15 },
        { 5, (void *)Cmd_AddY, -12.0f, 0, 0, 0, 15 },
        { 5, (void *)Cmd_AddO, 0.0f, 17, 0, 0, 15 },
        { 5, (void *)Cmd_SetO, 0.0f, 16777215, 0, 14, 15 },
        { 5, (void *)Cmd_SetY, 226.0f, 0, 0, 14, 15 },
        { 9, (void *)Cmd_AddY, -12.0f, 0, 0, 0, 15 },
        { 9, (void *)Cmd_AddO, 0.0f, 17, 0, 0, 15 },
        { 9, (void *)Cmd_SetO, 0.0f, 16777215, 0, 14, 15 },
        { 9, (void *)Cmd_SetY, 226.0f, 0, 0, 14, 15 },
        { 10, (void *)Cmd_AddY, -12.0f, 0, 0, 0, 15 },
        { 10, (void *)Cmd_AddO, 0.0f, 17, 0, 0, 15 },
        { 10, (void *)Cmd_SetO, 0.0f, 16777215, 0, 14, 15 },
        { 10, (void *)Cmd_SetY, 226.0f, 0, 0, 14, 15 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    static const FIGHT_CONTROL Seen7_2[] = {
        { 4, (void *)Cmd_AddY, -12.0f, 0, 0, 1, 16 },
        { 4, (void *)Cmd_AddO, 0.0f, -17, 0, 1, 16 },
        { 4, (void *)Cmd_SetY, 60.0f, 0, 0, 15, 16 },
        { 4, (void *)Cmd_SetO, 0.0f, 0, 0, 15, 16 },
        { 7, (void *)Cmd_AddY, -12.0f, 0, 0, 1, 16 },
        { 7, (void *)Cmd_AddO, 0.0f, -17, 0, 1, 16 },
        { 7, (void *)Cmd_SetY, 60.0f, 0, 0, 15, 16 },
        { 7, (void *)Cmd_SetO, 0.0f, 0, 0, 15, 16 },
        { 8, (void *)Cmd_AddY, -12.0f, 0, 0, 1, 16 },
        { 8, (void *)Cmd_AddO, 0.0f, -17, 0, 1, 16 },
        { 8, (void *)Cmd_SetY, 60.0f, 0, 0, 15, 16 },
        { 8, (void *)Cmd_SetO, 0.0f, 0, 0, 15, 16 },
        { 5, (void *)Cmd_AddY, 12.0f, 0, 0, 1, 16 },
        { 5, (void *)Cmd_AddO, 0.0f, -17, 0, 1, 16 },
        { 5, (void *)Cmd_SetY, 420.0f, 0, 0, 15, 16 },
        { 5, (void *)Cmd_SetO, 0.0f, 0, 0, 15, 16 },
        { 9, (void *)Cmd_AddY, 12.0f, 0, 0, 1, 16 },
        { 9, (void *)Cmd_AddO, 0.0f, -17, 0, 1, 16 },
        { 9, (void *)Cmd_SetY, 420.0f, 0, 0, 15, 16 },
        { 9, (void *)Cmd_SetO, 0.0f, 0, 0, 15, 16 },
        { 10, (void *)Cmd_AddY, 12.0f, 0, 0, 1, 16 },
        { 10, (void *)Cmd_AddO, 0.0f, -17, 0, 1, 16 },
        { 10, (void *)Cmd_SetY, 420.0f, 0, 0, 15, 16 },
        { 10, (void *)Cmd_SetO, 0.0f, 0, 0, 15, 16 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };
    int tb[4] = { 31, 14, 35, 39 };
    unsigned int col;

    if (count == 0) OpeningBgVertInit(0);
    Seen7num = count / 106;
    count = count % 106;
    Opening_Seen7Sub();
    if (Seen7num == 4) {
        opsp[4].trnsl = 0.0f;
        opsp[4].attr = 0x20D;
        opsp[7].trnsl = 0.0f;
        opsp[7].attr = 0x20D;
        opsp[8].trnsl = 0.0f;
        opsp[8].attr = 0x20D;
        opsp[5].trnsl = 0.0f;
        opsp[5].attr = 0x20D;
        opsp[9].trnsl = 0.0f;
        opsp[9].attr = 0x20D;
        opsp[10].trnsl = 0.0f;
        opsp[10].attr = 0x20D;
        opsp[6].trnsl = 0.0f;
        opsp[0].trnsl = 0.0f;
        opsp[0].u0 = 0.0f;
        opsp[0].u1 = 1.0f;
        opsp[0].v0 = 0.0f;
        opsp[0].v1 = 1.0f;
        opsp[0].x = 0.0f;
        opsp[0].y = 0.0f;
        opsp[0].zm_x = 1.0f;
        opsp[0].attr = 0x20D;
        opsp[0].zm_y = 1.0f;
        return 1;
    }
    if (count == 0) Seen7mode = 0;
    if (count < 15) {
        FightControl_Execute(Seen7_1, opsp, count);
        polydraw_FBox(0.0f, 0.0f, 640.0f, opsp[4].y - 32.0f, 100.0f, 0xFF000000);
        polydraw_FBox(0.0f, opsp[5].y + 32.0f, 640.0f, 480.0f, 100.0f, 0xFF000000);
        col = (count * 17) << 24;
        polydraw_FBox(0.0f, opsp[7].y, 640.0f, opsp[7].y - 32.0f, 99.0f, col | 0xFFFFFF);
        polydraw_FBox(0.0f, opsp[5].y + 32.0f, 640.0f, opsp[5].y, 99.0f, col | 0xFFFFFF);
        ScrlString();
    } else {
        if (count == 15) {
            if (Seen7num != 3) {
                RegistSpr(Seen7num + 11, 0);
            } else {
                opsp[0].sprno = opsp[1].sprno;
            }
            opsp[10].sprno = opsp[9].sprno = opsp[5].sprno = opsp[8].sprno = opsp[7].sprno = opsp[4].sprno = opsp[Seen7num + 12].sprno;
            OpeningBgVertInit(Seen7num);
            opsp[0].u0 = 0.059375f;
            opsp[0].u1 = 0.871875f;
            opsp[0].v0 = 0.11666667f;
            opsp[0].v1 = 0.90833336f;
            opsp[0].zm_x = 0.8125f;
            opsp[0].zm_y = 0.79166669f;
            opsp[0].x = 38.0f;
            opsp[0].y = 88.0f;
            opsp[0].attr = 0x105;
            opsp[0].base_color = -1;
            opsp[6].trnsl = 1.0f;
            opsp[6].z = 200.0f;
            polydraw_FBox(0.0f, opsp[7].y, 640.0f, opsp[7].y - 32.0f, 99.0f, 0xFFFFFFFF);
            polydraw_FBox(0.0f, opsp[5].y + 32.0f, 640.0f, opsp[5].y, 99.0f, 0xFFFFFFFF);
        }
        if (count < 31) {
            if (count == 16) RegistSpr(animetb[Seen7num], 6);
            FightControl_Execute(Seen7_2, opsp, count - 15);
            polydraw_FBox(0.0f, 0.0f, 640.0f, opsp[4].y - 32.0f, 100.0f, 0xFF000000);
            polydraw_FBox(0.0f, opsp[5].y + 32.0f, 640.0f, 480.0f, 100.0f, 0xFF000000);
            col = ((30 - count) * 17) << 24;
            polydraw_FBox(0.0f, opsp[7].y, 640.0f, opsp[7].y - 32.0f, 99.0f, col | 0xFFFFFF);
            polydraw_FBox(0.0f, opsp[5].y + 32.0f, 640.0f, opsp[5].y, 99.0f, col | 0xFFFFFF);
            ScrlString();
        } else if (count < 106) {
            polydraw_FBox(0.0f, 0.0f, 640.0f, opsp[4].y - 32.0f, 100.0f, 0xFF000000);
            polydraw_FBox(0.0f, opsp[5].y + 32.0f, 640.0f, 480.0f, 100.0f, 0xFF000000);
            if (count == 51) Seen7mode |= 1;
            if (count - 31 == tb[Seen7num]) Seen7mode |= 2;
            ScrlString();
            Opening_Seen7Anime(count - 31);
        }
    }
    return 0;
}

static const FIGHT_CONTROL Seen9_00[] = {
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetX, -120.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetY, 600.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetZ, 300.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_AddX, 10.666667f, 0, 0, 0, 30 },
    { 0, (void *)Cmd_AddY, -8.0f, 0, 0, 0, 30 },
    { 0, (void *)Cmd_AddTrnsl, 0.1f, 0, 0, 0, 10 },
    { 0, (void *)Cmd_AddTrnsl, -0.1f, 0, 0, 20, 30 },
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 29, 31 },
    { -1, 0, 0.0f, 0, 0, 0, 0 },
};
static const FIGHT_CONTROL Seen9_01[] = {
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetX, 420.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetY, 600.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetZ, 300.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_AddX, -10.666667f, 0, 0, 0, 30 },
    { 0, (void *)Cmd_AddY, -8.0f, 0, 0, 0, 30 },
    { 0, (void *)Cmd_AddTrnsl, 0.1f, 0, 0, 0, 10 },
    { 0, (void *)Cmd_AddTrnsl, -0.1f, 0, 0, 20, 30 },
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 29, 31 },
    { -1, 0, 0.0f, 0, 0, 0, 0 },
};
static const FIGHT_CONTROL Seen9_02[] = {
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetX, 320.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetY, 480.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetZ, 300.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_AddX, -10.666667f, 0, 0, 0, 30 },
    { 0, (void *)Cmd_AddTrnsl, 0.1f, 0, 0, 0, 10 },
    { 0, (void *)Cmd_AddTrnsl, -0.1f, 0, 0, 20, 30 },
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 29, 31 },
    { -1, 0, 0.0f, 0, 0, 0, 0 },
};
static const FIGHT_CONTROL Seen9_03[] = {
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetX, -120.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetY, 480.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_SetZ, 300.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_AddX, 10.666667f, 0, 0, 0, 30 },
    { 0, (void *)Cmd_AddTrnsl, 0.1f, 0, 0, 0, 10 },
    { 0, (void *)Cmd_AddTrnsl, -0.1f, 0, 0, 20, 30 },
    { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 29, 31 },
    { -1, 0, 0.0f, 0, 0, 0, 0 },
};
static FIGHT_CONTROL *seen9tb[14] = {
    Seen9_00, Seen9_01, Seen9_00, Seen9_01, Seen9_02, Seen9_03, Seen9_00,
    Seen9_01, Seen9_00, Seen9_01, Seen9_02, Seen9_03, Seen9_02, Seen9_03,
};
void (*bgdrawfunctb[])() = {
    OpeningBgDrawRU, OpeningBgDrawLU, OpeningBgDrawRU, OpeningBgDrawLU, OpeningBgSideL, OpeningBgSideR, OpeningBgDrawRU,
    OpeningBgDrawLU, OpeningBgDrawRU, OpeningBgDrawLU, OpeningBgSideL, OpeningBgSideR, OpeningBgSideL, OpeningBgSideR,
};

int Opening_Seen9Sub(int fms)
{
    void (*func)() = bgdrawfunctb[Seen9Flag];

    if (fms == 0) {
        OpeningBgVertInit(0);
        OpeningBgRUInit(0);
    }
    if (fms < 30) {
        func((int)(opsp->trnsl * 255.0f));
        FightControl_Execute(seen9, opsp, fms);
        return 0;
    }
    return 1;
}

extern FIGHT_CONTROL *seen9tb[14];

int Opening_Seen9(int count)
{
    static int fms;

    if (count == 0) {
        Seen9Flag = 0;
        fms = 0;
    }
    if (fms == 0) RegistSpr(Seen9Flag + 0x44, 0);
    seen9 = seen9tb[Seen9Flag];
    if (Opening_Seen9Sub(fms) == 0) {
        fms++;
    } else {
        fms = 0;
        Seen9Flag++;
        if (Seen9Flag >= 14) return 1;
    }
    return 0;
}

void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send);

int Opening_Seen10_00(int count)
{
    NL_SPR_ARG oparg[2] = {
        { 132, 0.0f, 480.0f, 300.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
        { 133, 0.0f, 480.0f, 200.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
    };
    int no;

    if (count == 0) {
        RegistSpr(0x52, 4);
        SprArgSetOne(4, &oparg[0], opsp);
        SprArgSetOne(5, &oparg[1], opsp);
        return 0;
    }
    no = (count - 1) / 5;
    if (no >= 4) {
        opsp[4].trnsl = 0.0f;
        opsp[5].trnsl = 0.0f;
        return 1;
    }
    if ((count - 1) % 5 == 0) RegistSpr(no + 0x53, 5);
    return 0;
}

void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send);

int Opening_Seen10_01(int count)
{
    NL_SPR_ARG oparg[2] = {
        { 128, 0.0f, 480.0f, 300.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
        { 132, 0.0f, 480.0f, 200.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
    };
    int no;

    if (count == 0) {
        RegistSpr(0x57, 0);
        SprArgSetOne(0, &oparg[0], opsp);
        SprArgSetOne(4, &oparg[1], opsp);
    }
    if (count / 33 >= 3) {
        opsp[4].trnsl = 0.0f;
        opsp[0].trnsl = 0.0f;
        return 1;
    }
    no = count / 3 % 11;
    if (count % 3 == 0) RegistSpr(no + 0x58, 4);
    return 0;
}

void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send);

int Opening_Seen11(int count)
{
    NL_SPR_ARG oparg[1] = {
        { 132, 0.0f, 480.0f, 300.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
    };
    int no;

    if (count == 0) SprArgSetOne(4, &oparg[0], opsp);
    no = count / 3;
    if (no >= 7) {
        opsp[4].trnsl = 0.0f;
        return 1;
    }
    if (count % 3 == 0) RegistSpr(no + 0x63, 4);
    return 0;
}

void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send);

int Opening_Seen12(int count)
{
    NL_SPR_ARG oparg[1] = {
        { 128, 0.0f, 480.0f, 300.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
    };

    if (count == 0) {
        RegistSpr(0x6A, 0);
        SprArgSetOne(0, &oparg[0], opsp);
    } else if (count == 80) {
        opsp[0].trnsl = 0.0f;
        return 1;
    }
    return 0;
}

void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send);

int Opening_Seen13(int count)
{
    int i;
    NL_SPR_ARG oparg[2] = {
        { 128, 0.0f, 480.0f, 300.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
        { 132, 0.0f, 480.0f, 200.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, 1, 525, 0, 0, 0, 0, 0 }, 
    };

    if (count == 0) {
        RegistSpr(0x84, 0);
        opsp[0].trnsl = 0.0f;
        return 0;
    }
    if (count == 1) {
        SprArgSetOne(0, &oparg[0], opsp);
        SprArgSetOne(4, &oparg[1], opsp);
    }
    i = (count - 1) / 4;
    if (i >= 24) {
        opsp[4].trnsl = 0.0f;
        opsp[0].trnsl = 0.0f;
        return 1;
    }
    if ((count - 1) % 4 == 0) RegistSpr(i + 0x6C, 4);
    return 0;
}

int Opening_Seen14(int count)
{
    static const FIGHT_CONTROL Seen4[] = {
        { 0, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetO, 0.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_AddO, 0.0f, 64, 0, 46, 49 },
        { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 49, 50 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };

    if (count == 0) {
        RegistSpr(0x85, 0);
        OpeningBgRUInit(0);
    }
    if (count < 50) {
        FightControl_Execute(Seen4, opsp, count);
        OpeningBgDrawLU(255);
        return 0;
    }
    return 1;
}

int Opening_Seen15(int count)
{
    static const FIGHT_CONTROL Seen4[] = {
        { 0, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetO, 0.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_AddO, 0.0f, 64, 0, 46, 49 },
        { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 48, 50 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };

    if (count == 0) {
        RegistSpr(0x86, 0);
        OpeningBgRUInit(0);
    }
    if (count < 50) {
        FightControl_Execute(Seen4, opsp, count);
        OpeningBgDrawRU(255);
        return 0;
    }
    return 1;
}

void XEffectExe(void)
{
    opsp[xecount + 5].x = opsp[4].x;
    opsp[xecount + 5].attr = opsp[4].attr;
    opsp[xecount + 5].y = opsp[4].y;
    opsp[xecount + 5].z = opsp[4].z + xecount;
    opsp[xecount + 5].zm_x = opsp[4].zm_x;
    opsp[xecount + 5].zm_y = opsp[4].zm_y;
    opsp[xecount + 5].trnsl = opsp[4].trnsl;
    if (++xecount >= 3) xecount = 0;
}

void SetFlash(char time, short unsigned int flag);

int Opening_Seen16(int count)
{
    static const FIGHT_CONTROL Seen4[] = {
        { 4, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetY, 240.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetX, 320.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetZM, 4.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_SetZ, 100.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 9, (void *)Cmd_SetY, 240.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetX, 320.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetO, 0.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetZ, 300.0f, 0, 0, 0, 1 },
        { 0, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 1, (void *)Cmd_SetB, 0.0f, -1, 0, 0, 1 },
        { 1, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 1, (void *)Cmd_SetO, 0.0f, 0, 0, 0, 1 },
        { 1, (void *)Cmd_SetZ, 200.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 0, 1 },
        { 9, (void *)Cmd_SetZ, 100.0f, 0, 0, 0, 1 },
        { 4, (void *)Cmd_AddTrnsl, 0.1f, 0, 0, 0, 10 },
        { 4, (void *)Cmd_AddZM, -0.4f, 0, 0, 0, 10 },
        { 4, (void *)Cmd_SetZM, 1.0f, 0, 0, 10, 11 },
        { 4, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 10, 11 },
        { 0, (void *)Cmd_AddTrnsl, 0.025f, 0, 0, 60, 100 },
        { 0, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 100, 101 },
        { 1, (void *)Cmd_AddTrnsl, 0.025f, 0, 0, 60, 100 },
        { 1, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 100, 101 },
        { 9, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 100, 101 },
        { 9, (void *)Cmd_AddB, 0.0f, -8, 0, 268, 299 },
        { 0, (void *)Cmd_AddB, 0.0f, -8, 0, 268, 299 },
        { 1, (void *)Cmd_AddB, 0.0f, -8, 0, 268, 299 },
        { 4, (void *)Cmd_AddB, 0.0f, -8, 0, 268, 299 },
        { 9, (void *)Cmd_SetB, 0.0f, 0, 0, 299, 300 },
        { 0, (void *)Cmd_SetB, 0.0f, 0, 0, 299, 300 },
        { 1, (void *)Cmd_SetB, 0.0f, 0, 0, 299, 300 },
        { 4, (void *)Cmd_SetB, 0.0f, 0, 0, 299, 300 },
        { -1, 0, 0.0f, 0, 0, 0, 0 },
    };

    if (count == 0) {
        RegistSpr(0x87, 0);
        RegistSpr(0, 1);
        RegistSpr(0x8A, 9);
        RegistSpr(0x89, 10);
        RegistSpr(0x88, 4);
        RegistSpr(0x88, 5);
        RegistSpr(0x88, 6);
        RegistSpr(0x88, 7);
        opsp[0].attr = 0x10D;
        opsp[4].attr = 0x10A;
        opsp[1].attr = 0x10D;
        opsp[9].attr = 0x10A;
        opsp[10].attr = 0x10E;
        xecount = 0;
    }
    if (count < 300) {
        FightControl_Execute(Seen4, opsp, count);
        if (count & 1) XEffectExe();
        if (count >= 10) {
            opsp[5].trnsl = 0.0f;
            opsp[6].trnsl = 0.0f;
            opsp[7].trnsl = 0.0f;
        }
        if (count == 100) SetFlash(2, 0xF01);
        return 0;
    }
    return 1;
}

void FlashExecute(void);
void gCurrentColorClear(void);
void gCurrentColorSetARBG(unsigned int arbg);
void gSetObjColorOffset(unsigned int val);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void Opening_Draw(void)
{
    int i;

    for (i = 0; i < 20; i++) {
        if (opsp[i].trnsl != 0.0) {
            if (opsp[i].attr & 0x100) gCurrentColorSetARBG(opsp[i].base_color);
            if (opcancelflag == 0) gSetObjColorOffset(opsp[i].offset_color);
            gSpriteDraw_NaomiLibVersion(&opsp[i]);
            if (opcancelflag == 0) gSetObjColorOffset(0);
            if (opsp[i].attr & 0x100) gCurrentColorClear();
        }
    }
    FlashExecute();
}

char *FileReadSelect(int fid, int chan);
extern int SubJob;
extern int SubJobCount;
int FileReadCheck(void);
void *dmalloc(int size, char *name);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void JobNext(void);
void LinkAddrTb(u_int *addr);
void SprArgSetOne(int count, NL_SPR_ARG *arg, NL_SPR_ARG *send);

void OpeningInit(void)
{
    int i;
    static NL_SPR_ARG oparg = { 0, 0.0f, 480.0f, 100.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0.0f, 1, 525, 0, 0, 0, 0, 0 };

    OpeningFile = FileReadSelect(0xA1, 0);
    if (FileReadCheck()) {
        LinkAddrTb((u_int *)OpeningFile);
        opcancelflag = 0;
        CockpitLoadInit();
        CreditFontLoad(40);
        opsp = dmalloc(0x640, "opsp");
        OpBgWork = dmalloc(0x188, "OpBgWork");
        OpBgWorkX = dmalloc(0x188, "OpBgWorkX");
        OpBgWorkY = dmalloc(0x188, "OpBgWorkY");
        for (i = 0; i < 20; i++) SprArgSetOne(i, &oparg, opsp);
        SubJob = 0;
        SubJobCount = 0;
        JobNext();
    }
}

void BGM_Free(void);
void JobChange(int job);
void dfree(void *mem);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void sound_alloff(void);

int OpeningEnd(int count)
{
    sound_alloff();
    BGM_Free();
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    dfree(opsp);
    dfree(OpBgWork);
    dfree(OpBgWorkX);
    dfree(OpBgWorkY);
    JobChange(6);
    return 0;
}

void DispCreditMsg(void);
int IsFadeEnd(void);
void Rq_Fadeout(void);
extern PAD pads[2];

void Opening_BlankFunc(void)
{
    if (opcancelflag == 0) {
        DispCreditMsg();
        if ((pads[0].trg | pads[1].trg) & 0x800) {
            opcancelflag = 1;
            Rq_Fadeout();
        }
    } else if (opcancelflag == 1) {
        if (IsFadeEnd()) opcancelflag = 2;
    }
}

void BGM_Request(int no, int loop);
void set_bgmvol(void);

int Opening_Wait1(int count)
{
    if (count == 0) {
        set_bgmvol();
        BGM_Request(20, 0);
    }
    if (count < 30) return 0;
    return 1;
}

void SetFlash(char time, short unsigned int flag);

int Opening_Flash(int count)
{
    if (count == 0) SetFlash(2, 0xF01);
    if (count < 6) {
        if (count == 1) SetFlash(5, 0xF01);
        return 0;
    }
    return 1;
}

extern int SubJob;
extern int SubJobCount;
int Random(void);

int (*opfunc[])() = {
    Opening_Seen1, Opening_Wait1, Opening_Seen2, Opening_Seen3, Opening_Seen5, Opening_Seen7,
    Opening_Seen9, Opening_Seen10_00, Opening_Seen10_01, Opening_Seen11, Opening_Flash, Opening_Seen12,
    Opening_Seen13, Opening_Seen14, Opening_Seen15, Opening_Seen16, OpeningEnd, 0,
};

void OpeningMain(void)
{
    Random();
    while (opfunc[SubJob](SubJobCount)) {
        SubJobCount = 0;
        SubJob++;
    }
    if (SubJob != 16) {
        SubJobCount++;
        Opening_BlankFunc();
        if (opcancelflag == 2) {
            OpeningEnd(0);
            return;
        }
        if (SubJob != 10) Opening_Draw();
    }
}

int Random(void);

void OpeningBgVertInit(int col)
{
    int i;
    int coltb[4] = { 0xFF000080, 0xFF808000, 0xFF800080, 0xFF800000 };

    colno = col;
    backcol = coltb[col];
    for (i = 0; i < 98; i++) OpBgWork[i] = (Random() & 0xFF) * 2;
}

void OpeningBgRUInit(int col)
{
    int i;
    int coltb[4] = { 0xFF000080, 0xFF808000, 0xFF800080, 0xFF800000 };

    colno = col;
    backcol = coltb[col];
    for (i = 97; i >= 0; i--) OpBgWorkY[i] = 0;
    for (i = 97; i >= 0; i--) OpBgWorkX[i] = i * 10;
}

int GetFadeFlag(void);
int GetFadeVal(void);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void OpeningBgSideDraw(int dir, int fade)
{
    int i;
    int n;
    float x;
    NLVERTEX00 vt[4];
    int a;
    int ltb[8] = { 0x80, 0xFF, 0x808000, 0xFFFF00, 0x800080, 0xFF00FF, 0x800000, 0xFF0000 };

    if (GetFadeFlag()) {
        a = GetFadeVal() << 24;
    } else {
        a = fade << 24;
    }
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 500.0f, (backcol & 0xFFFFFF) | a);
    for (i = 0; i < 45; i++) {
        vt[0].z = vt[1].z = vt[2].z = vt[3].z = 499.0f;
        vt[0].y = vt[2].y = i * 8 + 60;
        vt[3].y = vt[1].y = vt[0].y + 4.0f;
        for (n = 0; n < 10; n++) {
            if (dir == 0) {
                x = OpBgWork[i] + n * 80;
                if (x < -80.0f) x = OpBgWork[i] + (n + 10) * 80;
                if (x > 640.0f) x = OpBgWork[i] + (10 - n) * -80;
            } else {
                x = OpBgWork[i] + n * -80;
                if (x > 640.0f) x = OpBgWork[i] + (n + 10) * -80;
                if (x < -80.0f) x = OpBgWork[i] + (10 - n) * 80;
            }
            if (x > 640.0f || x < -80.0f) continue;
            vt[1].x = vt[0].x = x;
            vt[3].x = vt[2].x = x + 80.0f;
            if (n & 1) {
                vt[0].col = vt[1].col = ltb[colno * 2] | a;
                vt[2].col = vt[3].col = ltb[colno * 2 + 1] | a;
            } else {
                vt[0].col = vt[1].col = ltb[colno * 2 + 1] | a;
                vt[2].col = vt[3].col = ltb[colno * 2] | a;
            }
            nlDispStripTrnsl(4, vt);
        }
        if (dir == 0) {
            if (i & 1) OpBgWork[i] -= 20;
            else OpBgWork[i] -= 8;
            if (OpBgWork[i] < -640) OpBgWork[i] += 640;
        } else {
            if (i & 1) OpBgWork[i] += 20;
            else OpBgWork[i] += 8;
            if (OpBgWork[i] > 1200) OpBgWork[i] -= 1200;
        }
    }
}

int GetFadeFlag(void);
int GetFadeVal(void);
int Random(void);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void OpeningBgDrawRU(int fade)
{
    int i;
    int n;
    float x;
    float y;
    NLVERTEX00 vt[4];
    int a;

    if (GetFadeFlag()) {
        a = GetFadeVal() << 24;
    } else {
        a = fade << 24;
    }
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 500.0f, (backcol & 0xFFFFFF) | a);
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = 499.0f;
    for (i = 0; i < 96; i++) {
        x = OpBgWorkX[i];
        y = OpBgWorkY[i];
        for (n = 0; n < 9; n++) {
            vt[0].x = x + n * -80 + 60.0f;
            vt[0].y = y + n * 80;
            if (vt[0].x < 0.0f) {
                vt[0].x = ((n - 8) * -80) + x + 60.0f;
                vt[0].y = ((n - 8) * 80) + y;
            } else if (vt[0].y < -80.0f) {
                vt[0].x = ((n + 8) * -80) + x + 60.0f;
                vt[0].y = ((n + 8) * 80) + y;
            }
            if (vt[0].y >= 480.0f) {
                vt[0].x = ((n - 8) * -80) + x + 60.0f;
                vt[0].y = ((n - 8) * 80) + y;
            }
            if (vt[0].x < 0.0f || vt[0].y < -80.0f || vt[0].x > 720.0f || vt[0].y > 480.0f) continue;
            vt[1].x = vt[0].x + 4.0f;
            vt[1].y = vt[0].y + 4.0f;
            vt[2].x = vt[0].x - 80.0f;
            vt[2].y = vt[0].y + 80.0f;
            vt[3].x = vt[2].x + 4.0f;
            vt[3].y = vt[2].y + 4.0f;
            if (!(n & 1)) {
                vt[0].col = vt[1].col = a | 0x80;
                vt[2].col = vt[3].col = a | 0xFF;
            } else {
                vt[0].col = vt[1].col = a | 0xFF;
                vt[2].col = vt[3].col = a | 0x80;
            }
            nlDispStripTrnsl(4, vt);
        }
        x = (Random() & 3) * 8;
        OpBgWorkY[i] -= x;
        OpBgWorkX[i] += x;
        if (OpBgWorkY[i] < -480 || OpBgWorkX[i] > 720) {
            OpBgWorkY[i] = i * 10;
            OpBgWorkX[i] = 0;
        }
    }
}

int GetFadeFlag(void);
int GetFadeVal(void);
int Random(void);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void OpeningBgDrawLU(int fade)
{
    int i;
    int n;
    float x;
    float y;
    NLVERTEX00 vt[4];
    int a;

    if (GetFadeFlag()) {
        a = GetFadeVal() << 24;
    } else {
        a = fade << 24;
    }
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 500.0f, (backcol & 0xFFFFFF) | a);
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = 499.0f;
    for (i = 0; i < 96; i++) {
        x = OpBgWorkX[i] - 160.0f;
        y = OpBgWorkY[i] + 80.0f;
        for (n = 0; n < 9; n++) {
            vt[0].x = x + n * 80 - 60.0f - 80.0f;
            vt[0].y = y + n * 80;
            if (vt[0].x < -220.0f || vt[0].y < -90.0f) {
                vt[0].x = ((n + 9) * 80) + x - 60.0f - 80.0f;
                vt[0].y = ((n + 9) * 80) + y;
            }
            if (vt[0].y > 480.0f) {
                vt[0].x = ((n - 8) * 80) + x - 60.0f - 80.0f;
                vt[0].y = ((n - 8) * 80) + y;
            }
            if (vt[0].x > 640.0f) {
                vt[0].x = ((n - 8) * 80) + x - 60.0f - 80.0f;
                vt[0].y = ((n - 8) * 80) + y;
            }
            if (vt[0].x < -90.0f || vt[0].y < -90.0f || vt[0].x > 720.0f || vt[0].y > 480.0f) continue;
            vt[2].x = vt[0].x - 4.0f;
            vt[2].y = vt[0].y + 4.0f;
            vt[1].x = vt[0].x + 80.0f;
            vt[3].x = vt[2].x + 80.0f;
            vt[3].y = vt[2].y + 80.0f;
            vt[1].y = vt[0].y + 80.0f;
            if (n & 1) {
                vt[0].col = vt[2].col = a | 0x80;
                vt[1].col = vt[3].col = a | 0xFF;
            } else {
                vt[0].col = vt[2].col = a | 0xFF;
                vt[1].col = vt[3].col = a | 0x80;
            }
            nlDispStripTrnsl(4, vt);
        }
        x = (Random() & 3) * 8;
        OpBgWorkY[i] -= x;
        OpBgWorkX[i] -= x;
        if (OpBgWorkY[i] < -480 || OpBgWorkX[i] < -480) {
            OpBgWorkY[i] = i * 10;
            OpBgWorkX[i] = 640;
        }
    }
}

void OpeningBgSideR(int fade)
{
    OpeningBgSideDraw(1, fade);
}

void OpeningBgSideL(int fade)
{
    OpeningBgSideDraw(0, fade);
}
