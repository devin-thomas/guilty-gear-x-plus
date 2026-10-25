#include "generated/screen.h"
static RECTL Screen;
static RECTL ScreenSv;
static float ScreenZoom;
static float scrx;
static float scry;
static float avrx;
static float avry;
void *smalloc(int size, char *name);
extern CHARACTER_WORK *PlayerWork;
int g_roundmin(int val, int min);
int g_roundmax(int val, int max);
extern int BattleStatus;

void Change_ScreenScale(int i)
{
    static int j;

    if (i == -1) {
        i = (j == 0);
    }
    j = i;
    if (i) {
        COMMON_PALETTE = 1;
    } else {
        COMMON_PALETTE = 0;
    }
}

void Set_ScreenZoom(float zoom)
{
    ScreenZoom = zoom;
}

void Screen_MemKeep(void)
{
    scr_stp = smalloc(0x34, "scr_stp");
}

void Screen_Init(void)
{
    Screen.x = -32000;
    Screen.y = -38000;
    Screen.w = 76000;
    Screen.h = 48000;
    scr_stp->stScreenScale = 1.0f;
    scr_stp->stNowScale = 80000;
    scr_stp->stReduceLimitTime = 60;
    scr_stp->stYClipOff = 0;
    scr_stp->stScreenFlag = 0;
    ScreenZoom = 0;
}

void ScreenClear(void)
{
    scr_stp->stLastPosX1P = PlayerWork[0].posx;
    scr_stp->stChaseSpeed1P = 0;
    scr_stp->stLastPosX2P = PlayerWork[1].posx;
    scr_stp->stChaseSpeed2P = 0;
    scr_stp->stScreenYOffset = 0x4B00;
    {
        GROUND_SHAKE *p = &groundshakey;

        p->move = 0;
        p->power = 0;
        p->time = 0;
    }
    {
        GROUND_SHAKE *p = &groundshakex;

        p->move = 0;
        p->power = 0;
        p->time = 0;
    }
}

void SetYClipOff(void)
{
    scr_stp->stYClipOff = 1;
}

RECTL *GetScreenRect(void)
{
    return &Screen;
}

int GetScreenBGY(void)
{
    return ScreenBG.y;
}

int World2ScreenX(int x)
{
    x -= Screen.x;
    return (x < 0) ? (x - 99) / 100 : x / 100;
}

int World2ScreenY(int x)
{
    x -= Screen.y;
    return (x < 0) ? (x - 99) / 100 : x / 100;
}

float ObjWorld2ScreenX(float x)
{
    x = (x - ScreenObj.x) * scr_stp->stScreenScaleObj + 32000.0f;
    if (x < 0) {
        return (x - 99.0f) / 100.0f;
    }
    return x / 100.0f;
}

float ObjWorld2ScreenY(float y)
{
    y = (y - ScreenObj.y) * scr_stp->stScreenScaleObj + 44000.0f;
    if (y < 0) {
        return (y - 99.0f) / 100.0f;
    }
    return y / 100.0f;
}

float ObjScreen2WorldY(float y)
{
    return (y * 100.0f - 44000.0f) / scr_stp->stScreenScaleObj + ScreenObj.y;
}

void ObjWorld2ScreenV(CHARACTER_WORK *offset, NL_POINT3 *p)
{
    p->x = ObjWorld2ScreenX(offset->posx);
    p->y = ObjWorld2ScreenY(offset->posy);
    p->z = GetObjScale();
}

float GetObjScale(void)
{
    return scr_stp->stScreenScaleObj;
}

float GetBGScale(void)
{
    return scr_stp->stScreenScaleBG;
}

void scaleset(int zx, int zy, int limit)
{
    int change;
    int int100;
    float scale;

    int100 = 100;
    zx += 16000;
    if (limit) {
        if (zx < 64000) {
            zx = 64000;
        }
        if (zx > 80000) {
            zx = 80000;
        }
    }
    zy = zy * 4 / 3 + 32000;
    if (limit) {
        if (zy < 64000) {
            zy = 64000;
        }
        if (zy > 80000) {
            zy = 80000;
        }
    }
    if (zx < zy) {
        zx = zy;
    }
    change = (scr_stp->stNowScale - zx) / 15;
    scr_stp->stScreenSettle = 0;
    change = (change > -1) ? change : -change;
    if (change < int100) change = int100;
    if (!(scr_stp->stScreenFlag & 0x40000)) {
        if (scr_stp->stNowScale + 100 < zx) {
            scr_stp->stNowScale += 2000;
            if (scr_stp->stNowScale >= zx) {
                scr_stp->stNowScale = zx;
            }
            scr_stp->stReduceLimitTime = 45;
        } else if (zx < scr_stp->stNowScale - 100) {
            if (scr_stp->stReduceLimitTime > 0) {
                scr_stp->stReduceLimitTime--;
            } else {
                scr_stp->stNowScale -= change;
            }
        } else {
            scr_stp->stNowScale = zx;
            scr_stp->stScreenSettle = 1;
        }
    } else {
        scr_stp->stNowScale = zx;
        scr_stp->stScreenSettle = 1;
    }
    zx = scr_stp->stNowScale;
    ScreenObj.w = zx;
    ScreenBG.w = zx;
    Screen.w = zx;
    scale = zx / 64000.0f;
    zx = 1.0f / scale * 250.0f;
    scale = zx / 250.0f;
    scr_stp->stScreenScale = scale;
    Screen.h = 48000.0f / scale;
    scale *= ScreenZoom + 1.0f;
    scr_stp->stScreenScaleBG = scale;
    scr_stp->stScreenScaleObj = scale;
}

int PosChaseExe(int tx, int nx, int *speed)
{
    int x;
    int absx;
    int retx;

    x = tx - nx;
    absx = abs(x);
    if (absx < *speed) {
        retx = tx;
    } else if (x > 0) {
        retx = nx + *speed;
    } else if (x < 0) {
        retx = nx - *speed;
    } else {
        retx = tx;
    }
    if (absx < *speed) {
        if (*speed > 60) {
            *speed = *speed - *speed / 10;
        } else {
            *speed = 60;
        }
    } else if (absx >= *speed * 10) {
        *speed = *speed + absx / 30;
    }
    return retx;
}

void ScreenYOffsetExe(void)
{
    if (scr_stp->stScreenFlag & 0x1000) {
        if (PlayerWork[0].posy < -14000 && PlayerWork[1].posy < -14000) {
            scr_stp->stScreenYOffset = 10000;
        } else {
            scr_stp->stScreenYOffset = 0x4B00;
        }
    }
    if (PlayerWork[0].posy < -14000 && PlayerWork[1].posy < -14000 && abs(PlayerWork[0].posy - PlayerWork[1].posy) < 20000) {
        scr_stp->stScreenYOffset = g_roundmin(scr_stp->stScreenYOffset - 200, 12000);
    } else {
        scr_stp->stScreenYOffset = g_roundmax(scr_stp->stScreenYOffset + 200, 0x4B00);
    }
}

void scrxyset(int x, int y)
{
    int limx;
    int limy;
    int change;
    int settle;

    settle = 1;
    y = y + scr_stp->stScreenYOffset / scr_stp->stScreenScaleObj;
    if (y > 0) {
        y = 0;
    }
    if (!scr_stp->stYClipOff) {
        limy = 40000.0f / scr_stp->stScreenScale + -87600.0f;
        if (y < limy) {
            y = limy;
        }
    }
    limx = 76000.0f - 64000.0f / scr_stp->stScreenScale * 0.5f;
    if (x > limx) {
        x = limx;
    }
    limx = -limx;
    if (x < limx) {
        x = limx;
    }
    if (scr_stp->stScreenFlag & 8) {
        change = x - ScreenSv.x;
        if (abs(change) > 100) {
            change = change / 10;
            if (change > 0) {
                if (change < 100) {
                    change = 100;
                }
            } else {
                if (change > -100) {
                    change = -100;
                }
            }
            x = ScreenSv.x + change;
            settle = 0;
        }
        change = y - ScreenSv.y;
        if (abs(change) > 100) {
            change = change / 10;
            if (change > 0) {
                if (change < 100) {
                    change = 100;
                }
            } else {
                if (change > -100) {
                    change = -100;
                }
            }
            y = ScreenSv.y + change;
            settle = 0;
        }
    }
    scr_stp->stScreenSettle &= settle;
    ScreenSv.x = x;
    ScreenSv.y = y;
    ScreenObj.x = x + groundshakex.move * 100.0f;
    Screen.x = x - Screen.w / 2;
    ScreenObj.y = y + groundshakey.move * 100.0f;
    Screen.y = y - 40000;
    ScreenBG.x = ScreenObj.x;
    ScreenBG.y = ScreenObj.y;
}

void GroundShakeExe(GROUND_SHAKE *p)
{
    static char powerTb1[9] = { 0, -3, -4, -3, 0, 2, 3, 2, 127 };
    static char powerTb2[11] = { 0, -4, -6, -4, 0, 3, 5, 3, 0, -2, 127 };
    static char powerTb3[13] = { 0, -6, -9, -6, 0, 5, 8, 5, 0, -2, -3, -2, 127 };
    static char powerTb4[17] = { 0, -8, -12, -8, 0, 7, 10, 7, 0, -4, -6, -4, 0, 2, 3, 2, 127 };
    static char *ptb[4] = { powerTb1, powerTb2, powerTb3, powerTb4 };
    char move;

    if (p->time == 0) {
        return;
    }
    move = ptb[p->power & 3][p->time];
    if (move == 127) {
        p->time = 0;
        p->move = 0;
        return;
    }
    if (p->power & 4) {
        move = -move;
    }
    p->time++;
    p->move = -move;
}

void SetGroundShake(int lv)
{
    GROUND_SHAKE *p;

    p = &groundshakey;
    p->power = lv;
    p->time = 1;
    p->move = 0;
}

void SetGroundShakeX(int lv, int dir)
{
    GROUND_SHAKE *p;

    p = &groundshakex;
    p->power = lv;
    if (dir) {
        p->power = lv | 4;
    }
    p->move = 0;
    p->time = 1;
}

void ScreenExecute(void)
{
    GroundShakeExe(&groundshakex);
    GroundShakeExe(&groundshakey);
    if (!(scr_stp->stScreenFlag & 0x40)) {
        if (scr_stp->stScreenFlag & 4) {
            avrx = 0;
            avry = 0;
        } else {
            avrx = PlayerWork[0].posx - PlayerWork[1].posx;
            avrx = (avrx < 0) ? -avrx : avrx;
            avry = PlayerWork[0].posy - PlayerWork[1].posy;
            avry = (avry < 0) ? -avry : avry;
        }
    }
    scaleset(avrx, avry, 1);
    if (ScreenZoom > 0) {
        ScreenZoom -= 0.04f;
        if (ScreenZoom < 0) {
            ScreenZoom = 0;
        }
    }
    if (ScreenZoom < 0) {
        ScreenZoom += 0.04f;
        if (ScreenZoom > 0) {
            ScreenZoom = 0;
        }
    }
    ScreenYOffsetExe();
    if (scr_stp->stScreenFlag & 0x80) {
        scrx = (scr_stp->stLastPosX1P + scr_stp->stLastPosX2P) / 2;
        if (PlayerWork[0].posy < PlayerWork[1].posy) {
            scry = PlayerWork[0].posy;
        } else {
            scry = PlayerWork[1].posy;
        }
        scrxyset(scrx, scr_stp->stLastPosY);
    } else if ((scr_stp->stScreenFlag & 3) == 3) {
        scrx = (scr_stp->stLastPosX1P + scr_stp->stLastPosX2P) / 2;
        if (PlayerWork[0].posy < PlayerWork[1].posy) {
            scry = PlayerWork[0].posy;
        } else {
            scry = PlayerWork[1].posy;
        }
        scr_stp->stLastPosY = scry;
        scrxyset(scrx, scry);
    } else if (scr_stp->stScreenFlag & 3) {
        if (scr_stp->stScreenFlag & 1) {
            scr_stp->stLastPosX1P = PosChaseExe(PlayerWork[0].posx, scr_stp->stLastPosX1P, &scr_stp->stChaseSpeed1P);
            scr_stp->stLastPosX2P = PosChaseExe(PlayerWork[0].posx, scr_stp->stLastPosX2P, &scr_stp->stChaseSpeed2P);
            scry = PlayerWork[0].posy;
        } else {
            scr_stp->stLastPosX1P = PosChaseExe(PlayerWork[1].posx, scr_stp->stLastPosX1P, &scr_stp->stChaseSpeed1P);
            scr_stp->stLastPosX2P = PosChaseExe(PlayerWork[1].posx, scr_stp->stLastPosX2P, &scr_stp->stChaseSpeed2P);
            scry = PlayerWork[1].posy;
        }
        scrx = (scr_stp->stLastPosX1P + scr_stp->stLastPosX2P) / 2;
        scr_stp->stLastPosY = scry;
        scrxyset(scrx, scry);
    } else {
        if (!(scr_stp->stScreenFlag & 0x100)) {
            if (scr_stp->stScreenFlag & 0x1000) {
                scr_stp->stLastPosX1P = PlayerWork[0].posx;
            } else {
                scr_stp->stLastPosX1P = PosChaseExe(PlayerWork[0].posx, scr_stp->stLastPosX1P, &scr_stp->stChaseSpeed1P);
            }
        }
        if (!(scr_stp->stScreenFlag & 0x200)) {
            if (scr_stp->stScreenFlag & 0x1000) {
                scr_stp->stLastPosX2P = PlayerWork[1].posx;
            } else {
                scr_stp->stLastPosX2P = PosChaseExe(PlayerWork[1].posx, scr_stp->stLastPosX2P, &scr_stp->stChaseSpeed2P);
            }
        }
        scrx = (scr_stp->stLastPosX1P + scr_stp->stLastPosX2P) / 2;
        if (PlayerWork[0].posy < PlayerWork[1].posy) {
            scry = PlayerWork[0].posy;
        } else {
            scry = PlayerWork[1].posy;
        }
        if (scr_stp->stScreenFlag & 0x4000) {
            scry = 0;
        }
        if (scr_stp->stScreenFlag & 0x10000) {
            scry = PlayerWork[0].posy;
        }
        if (scr_stp->stScreenFlag & 0x20000) {
            scry = PlayerWork[1].posy;
        }
        if ((scr_stp->stScreenFlag & 0x30000) == 0x30000) {
            scry = 0;
        }
        scr_stp->stLastPosY = scry;
        scrxyset(scrx, scry);
    }
    scr_stp->stYClipOff = 0;
    scr_stp->stScreenFlag &= ~0xC0;
    if (BattleStatus == 2) {
        scr_stp->stScreenFlag = 0;
    }
}

void ScreenDebug(void) {
}

int IsScaleSettle(void)
{
    return scr_stp->stScreenSettle;
}

void SetScreenFlag(int flag)
{
    scr_stp->stScreenFlag = flag;
}

void OrScreenFlag(int flag)
{
    scr_stp->stScreenFlag |= flag;
}

void ClearReduceLimitTime(void)
{
    scr_stp->stReduceLimitTime = 0;
}

void ResetScreen(void)
{
    Screen_Init();
    ScreenClear();
}
