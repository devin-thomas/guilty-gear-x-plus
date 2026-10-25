#include "generated/bgsub.h"
static const short int bgsub_idtb[] = { 1, 2, 3 }; /* real name idtb (mdebug): a TU-unique name so the full build places it from its references, not symbol_addrs idtb (another TU copy) */
static MYSHP_WORK *mybg;


void may_backgirl_init(void) {
}


unsigned int gCurrentColorGet(void);
float GetChaseTrnsl(int id);
void nlUnitMatrix(void);
void nlRotateAX(u_short r);
void nlRotateAZ(u_short r);
void gStripSetPalette(int no);
void SetBlendingMode(short int mode);
float G_Sin(short unsigned int r);
float G_Cos(short unsigned int r);
void nlCalcPoint(NL_POINT3 *p1, NL_POINT3 *p2);
void nlDispStripTexTrnsl(int num, NLVERTEX03 *vt, int spr);
int BGWorld2ScreenX(int x);
int BGWorld2ScreenY(int y);
extern BGPOS_PRECALC *Bgpos_precalc;

void MagicSquare_Execute(BG_WORK *offset)
{
    static int rotatex = 0x3900;
    static int rotatez = 0;
    NL_POINT3 p[4];
    NL_POINT3 pd[4];
    static NLVERTEX03 vt[4] = {
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0xF0FFFFFF, 0 },
    };
    int i;
    unsigned int col;
    int angle;
    float offx;
    float offy;
    BGPOS_PRECALC *pr;
    float fx;
    float fy;

    angle = 0;
    pr = Bgpos_precalc;
    rotatez += 0x60;
    p[0].x = p[0].y = p[1].x = p[2].y = pr->scale * -350.0f;
    p[1].y = p[2].x = p[3].x = p[3].y = pr->scale * 350.0f;
    p[0].z = p[1].z = p[2].z = p[3].z = 0.0f;
    col = gCurrentColorGet();
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = ((int)(GetChaseTrnsl(0) * 250.0f) << 24) | col;
    nlUnitMatrix();
    nlRotateAX(rotatex);
    nlRotateAZ(rotatez);
    offx = BGWorld2ScreenX(offset->posx);
    offy = BGWorld2ScreenY(offset->posy);
    gStripSetPalette(offset->SpriteId);
    SetBlendingMode(1);
    p[0].z = 0.0f;
    vt[0].u = 0.5f;
    vt[0].v = 0.5f;
    for (angle = 0; angle < 0x10000; angle += 0x1000) {
        fx = G_Sin(angle) * 0.95f;
        fy = G_Cos(angle) * 0.95f;
        p[0].x = fx * 340.0f * pr->scale;
        p[0].y = fy * 340.0f * pr->scale;
        vt[1].u = fx * 0.5f + 0.5f;
        vt[1].v = fy * 0.5f + 0.5f;
        fx = G_Sin(angle + 0x1000) * 0.95f;
        fy = G_Cos(angle + 0x1000) * 0.95f;
        p[1].x = fx * 340.0f * pr->scale;
        p[1].y = fy * 340.0f * pr->scale;
        vt[2].u = fx * 0.5f + 0.5f;
        vt[2].v = fy * 0.5f + 0.5f;
        for (i = 0; i < 2; i++) {
            nlCalcPoint(&p[i], &pd[i]);
        }
        vt[0].x = offx;
        vt[0].y = offy;
        vt[0].z = offset->priority;
        vt[1].x = offx + pd[0].x;
        vt[1].y = offy + pd[0].y;
        vt[1].z = offset->priority;
        vt[2].x = offx + pd[1].x;
        vt[2].y = offy + pd[1].y;
        vt[2].z = offset->priority;
        nlDispStripTexTrnsl(3, vt, offset->SpriteId);
    }
    SetBlendingMode(0);
}

void *memset(void *s, int c, unsigned int n);
void nlsDispStripTex(int num, NLVERTEX03 *vt, int spr);

void AnotherSpace_Execute(BG_WORK *offset)
{
    static int rotatex = 0x3200;
    static int rotatez = 0;
    NL_POINT3 p[4];
    NL_POINT3 pd[4];
    NLVERTEX03 vt[4] = {
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 512.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 512.0f, 0.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 512.0f, 512.0f, 0xF0FFFFFF, 0 },
    };
    int i;
    unsigned int col;
    float offx;
    float offy;
    BGPOS_PRECALC *pr;

    pr = Bgpos_precalc;
    rotatez += 0xF0;
    p[0].x = p[0].y = p[1].x = p[2].y = pr->scale * -900.0f;
    p[1].y = p[2].x = p[3].x = p[3].y = pr->scale * 900.0f;
    p[0].z = p[1].z = p[2].z = p[3].z = 0.0f;
    col = gCurrentColorGet() & 0xFF;
    col *= GetChaseTrnsl(0);
    if (col == 0) {
        return;
    }
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = 0xFF000000 | (col << 16) | (col << 8) | col;
    nlUnitMatrix();
    nlRotateAX(rotatex);
    nlRotateAZ(rotatez);
    for (i = 0; i < 4; i++) {
        nlCalcPoint(&p[i], &pd[i]);
    }
    offx = BGWorld2ScreenX(offset->posx);
    offy = BGWorld2ScreenY(offset->posy);
    vt[0].x = offx + pd[0].x;
    vt[0].y = offy + pd[0].y;
    vt[1].x = offx + pd[1].x;
    vt[1].y = offy + pd[1].y;
    vt[2].x = offx + pd[2].x;
    vt[2].y = offy + pd[2].y;
    vt[3].x = offx + pd[3].x;
    vt[3].y = offy + pd[3].y;
    vt[3].z = vt[2].z = vt[1].z = vt[0].z = offset->priority;
    SetBlendingMode(0);
    nlsDispStripTex(4, vt, offset->SpriteId);
}

extern u_int TotalCounter;
void nlDispStrip(int num, NLVERTEX00 *vt);

void MayCloudR_Execute(BG_WORK *offset)
{
    NLVERTEX03 vt[4];
    float offx;
    float offy;
    float trnsl;
    unsigned int col;
    BGPOS_PRECALC *pr;

    pr = Bgpos_precalc;
    col = gCurrentColorGet();
    trnsl = GetChaseTrnsl(0);
    offx = BGWorld2ScreenX(offset->posx);
    offy = BGWorld2ScreenY(offset->posy);
    vt[0].x = offx + pr->scale * 0.0f;
    vt[0].y = offy + pr->scale * 0.0f;
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = col | ((int)(trnsl * 80.0f) << 24);
    vt[1].x = offx + pr->scale * 500.0f;
    vt[1].y = offy - pr->scale * 100.0f;
    vt[2].x = offx + pr->scale * 400.0f;
    vt[2].y = offy + pr->scale * 250.0f;
    vt[3].x = offx + pr->scale * 900.0f;
    vt[3].y = offy + pr->scale * 150.0f;
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = offset->priority;
    vt[2].u = vt[0].u = 0.0f;
    vt[3].u = vt[1].u = 1.0f;
    vt[2].v = vt[3].v = (float)(TotalCounter % 48) / 48.0f;
    vt[0].v = vt[1].v = vt[2].v + 1.0f;
    SetBlendingMode(0);
    nlDispStripTexTrnsl(4, vt, offset->SpriteId);
}

void MayPropeller_Execute(BG_WORK *offset)
{
    static int rotatez = 0;
    NL_POINT3 p[4];
    static NLVERTEX03 vt[4] = {
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0xF0FFFFFF, 0 },
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0xF0FFFFFF, 0 },
    };
    NLVERTEX03 *vp;
    int i;
    unsigned int col;
    float offx;
    float offy;
    BGPOS_PRECALC *pr;

    rotatez += 0x1900;
    pr = Bgpos_precalc;
    if (rotatez >= 0x2800) {
        rotatez -= 0x6800;
    }
    p[0].x = p[0].y = p[1].y = p[2].x = pr->scale * -200.0f;
    p[0].z = p[1].z = p[2].z = p[3].z = p[1].x = p[2].y = p[3].x = p[3].y = 0.0f;
    col = gCurrentColorGet();
    vt[0].col = vt[1].col = vt[2].col = vt[3].col = ((int)(GetChaseTrnsl(0) * 255.0f) << 24) | col;
    nlUnitMatrix();
    nlRotateAZ(rotatez);
    offx = BGWorld2ScreenX(offset->posx);
    offy = BGWorld2ScreenY(offset->posy);
    vp = vt;
    for (i = 0; i < 4; i++) {
        nlCalcPoint(&p[i], (NL_POINT3 *)vp);
        vp->x += offx;
        vp->y += offy;
        vp++;
    }
    vt[0].z = vt[1].z = vt[2].z = vt[3].z = offset->priority;
    nlDispStripTexTrnsl(4, vt, offset->SpriteId);
}

void BGLayerVisualON(short int id);
void BGLayerVisualOFF(short int id);
extern CHARACTER_WORK *PlayerWork;

void MayBack(int a)
{
    static unsigned char mybg_act = 0;
    static unsigned char mybg_flg = 0;
    static unsigned short mybg_frame = 0;
    static unsigned short mybg_count = 0;
    static const char acttb[5][4] = {
        { 18, 19, 20, 19 },
        { 21, 22, 21, 22 },
        { 21, 22, 21, 22 },
        { 23, 23, 23, 23 },
        { 24, 25, 25, 25 },
    };
    static const unsigned short fmstb[5][4] = {
        { 3, 3, 3, 3 },
        { 10, 10, 10, 10 },
        { 10, 10, 10, 10 },
        { 10, 10, 10, 10 },
        { 10, 255, 255, 0xFFFF },
    };
    int no;
    int i;
    CHARACTER_WORK *off1;
    CHARACTER_WORK *off2;

    if (a) {
        mybg_act = 0;
        mybg_flg = 1;
        mybg_frame = 0;
        mybg_count = 0;
    }
    if (mybg_flg == 0) {
        off1 = PlayerWork;
        off2 = &PlayerWork[1];
        if (off1->idno == 3 || off1->idno == 14) {
            if (off1->actst & 0x20) {
                mybg_act = 3;
                mybg_flg = 1;
                mybg_frame = 0;
            } else if (off2->actst & 0x20) {
                mybg_act = 1;
                mybg_flg = 1;
                mybg_frame = 0;
            } else {
                mybg_act = 0;
            }
            if (off1->HitPoint == 0) {
                mybg_act = 4;
                mybg_flg = 1;
                mybg_frame = 0;
            } else if (off2->HitPoint == 0) {
                mybg_act = 2;
                mybg_flg = 1;
                mybg_frame = 0;
            }
        } else if (off2->idno == 3 || off2->idno == 14) {
            if (off2->actst & 0x20) {
                mybg_act = 3;
                mybg_flg = 1;
                mybg_frame = 0;
            } else if (off1->actst & 0x20) {
                mybg_act = 1;
                mybg_flg = 1;
                mybg_frame = 0;
            } else {
                mybg_act = 0;
            }
            if (off2->HitPoint == 0) {
                mybg_act = 4;
                mybg_flg = 1;
                mybg_frame = 0;
            } else if (off1->HitPoint == 0) {
                mybg_act = 2;
                mybg_flg = 1;
                mybg_frame = 0;
            }
        } else {
            if ((off2->actst & 0x20) || (off1->actst & 0x20)) {
                mybg_act = 3;
                mybg_flg = 1;
                mybg_frame = 0;
            } else {
                mybg_act = 0;
            }
            if (off2->HitPoint == 0 || off1->HitPoint == 0) {
                mybg_act = 4;
                mybg_flg = 1;
                mybg_frame = 0;
            }
        }
    }
    no = acttb[mybg_act][mybg_frame];
    if (++mybg_count == fmstb[mybg_act][mybg_frame]) {
        mybg_count = 0;
        if (++mybg_frame == 4) {
            mybg_frame = 0;
            mybg_flg = 0;
        }
    }
    for (i = 18; i < 26; i++) {
        if (no == i) {
            BGLayerVisualON(i);
        } else {
            BGLayerVisualOFF(i);
        }
    }
}

int Random(void);

void VEEye_Execute(void)
{
    static unsigned char act = 0;
    static unsigned char frame = 0;
    static const char acttb[9][3] = {
        { 0, 0, 0 },
        { 11, 12, 11 },
        { 13, 14, 13 },
        { 15, 16, 15 },
        { 17, 18, 17 },
        { 19, 20, 19 },
        { 21, 22, 21 },
        { 23, 24, 23 },
        { 25, 26, 25 },
    };
    static const char closeacttb[9] = { 27, 28, 29, 30, 30, 30, 29, 28, 27 };
    int no;
    int i;

    if (act == 0) {
        no = -1;
        act = Random() % 11;
    } else if (act >= 9) {
        no = closeacttb[frame];
        if (++frame >= 9) {
            frame = 0;
            act = 0;
        }
    } else {
        no = acttb[act][frame];
        if (++frame >= 3) {
            frame = 0;
            act = 0;
        }
    }
    for (i = 11; i < 31; i++) {
        if (no == i) {
            BGLayerVisualON(i);
        } else {
            BGLayerVisualOFF(i);
        }
    }
}

void JMBack(void)
{
    static const char acttb1[] = { 18, 19, 20, 19, 21 };
    static const char acttb[3][3] = {
        { 10, 11, 12 },
        { 4, 5, 6 },
        { 9, 8, 7 },
    };
    static unsigned char flg[4] = { 0, 0, 0, 0 };
    static char act[4] = { 0, 0, 0, 0 };
    int i;
    int no[4];

    for (i = 0; i < 3; i++) {
        switch (flg[i]) {
        case 0:
            if ((Random() & 0xF) == 0) {
                flg[i] = 1;
            }
            break;
        case 1:
            if (++act[i] == 2) {
                flg[i] = 2;
            }
            break;
        case 2:
            if ((Random() & 0xF) == 0) {
                flg[i] = 3;
            }
            break;
        case 3:
            if (--act[i] == 0) {
                flg[i] = 0;
            }
            break;
        }
        no[i] = acttb[i][act[i]];
    }
    if (++flg[3] & 1) {
        if (++act[3] == 4) {
            act[3] = 0;
        } else if (act[3] == 5) {
            act[3] = 2;
        }
        if (act[3] == 1 && (Random() & 3) == 0) {
            act[3] = 4;
        }
    }
    no[3] = acttb1[act[3]];
    for (i = 7; i < 10; i++) {
        if (no[2] == i) {
            BGLayerVisualON(i);
        } else {
            BGLayerVisualOFF(i);
        }
    }
    for (i = 4; i < 7; i++) {
        if (no[1] == i) {
            BGLayerVisualON(i);
        } else {
            BGLayerVisualOFF(i);
        }
    }
    for (i = 10; i < 13; i++) {
        if (no[0] == i) {
            BGLayerVisualON(i);
        } else {
            BGLayerVisualOFF(i);
        }
    }
    for (i = 18; i < 22; i++) {
        if (no[3] == i) {
            BGLayerVisualON(i);
        } else {
            BGLayerVisualOFF(i);
        }
    }
}

void *dmalloc(int size, char *name);
extern BG_WORK *BgWork;

void MaySHPInit(void)
{
    int i;
    int id;
    BG_WORK *offset;

    offset = BgWork;
    mybg = dmalloc(sizeof(MYSHP_WORK) * 3, "mybg");
    for (i = 0; i < 3; i++) {
        mybg[i].xflag = Random() & 1;
        mybg[i].yflag = Random() & 1;
        mybg[i].xwait = 0;
        mybg[i].ywait = 0;
        mybg[i].xcount = 0;
        mybg[i].ycount = 0;
        mybg[i].xtime = 0;
        mybg[i].ytime = 0;
        mybg[i].xmode = 0;
        mybg[i].ymode = 0;
    }
    for (id = 0; id < 3; id++) {
        for (i = 0; i < 640; i++) {
            if ((offset[i].actst & 1) && offset[i].idno == bgsub_idtb[id]) {
                mybg[id].xorg = offset[i].dbg_centerx;
                mybg[id].yorg = offset[i].dbg_centery;
                break;
            }
        }
    }
}

int mybg_addxy(int id, int x, int y)
{
    int r;
    int i;
    BG_WORK *offset;

    r = 0;
    offset = BgWork;
    for (i = 0; i < 640; i++) {
        if ((offset[i].actst & 1) && offset[i].idno == id) {
            r = offset[i].dbg_centerx;
            offset[i].dbg_centerx += x;
            offset[i].dbg_centery += y;
        }
        offset[i].posy = -(offset[i].dbg_centery + offset[i].dbg_imagey) * 100;
        offset[i].posx = (-offset[i].dbg_centerx + offset[i].dbg_imagex) * 100;
    }
    return r;
}

void mybg_setx(int id, int x)
{
    int i;
    BG_WORK *offset;

    offset = BgWork;
    for (i = 0; i < 640; i++) {
        if ((offset[i].actst & 1) && offset[i].idno == id) {
            offset[i].dbg_centerx = x;
        }
        offset[i].posy = -(offset[i].dbg_centery + offset[i].dbg_imagey) * 100;
        offset[i].posx = (-offset[i].dbg_centerx + offset[i].dbg_imagex) * 100;
    }
}

void mybg_sety(int id, int y)
{
    int i;
    BG_WORK *offset;

    offset = BgWork;
    for (i = 0; i < 640; i++) {
        if ((offset[i].actst & 1) && offset[i].idno == id) {
            offset[i].dbg_centery = y;
        }
        offset[i].posy = -(offset[i].dbg_centery + offset[i].dbg_imagey) * 100;
        offset[i].posx = (-offset[i].dbg_centerx + offset[i].dbg_imagex) * 100;
    }
}

void MaySHPExe(void)
{
    short int waittb[3] = { 2, 4, 6 };
    int i;

    for (i = 0; i < 3; i++) {
        switch (mybg[i].xmode) {
        case 0:
            mybg[i].xwait = 10;
            mybg[i].xtime = Random() & 0x1F;
            mybg[i].xmode = 1;
            mybg[i].xcount = 0;
            if (mybg[i].xflag) {
                mybg[i].xflag = 0;
            } else {
                mybg[i].xflag = 1;
            }
            break;
        case 1:
            if (mybg[i].xcount++ == mybg[i].xwait) {
                if (mybg[i].xflag) {
                    mybg_addxy(bgsub_idtb[i], 1, 0);
                } else {
                    mybg_addxy(bgsub_idtb[i], -1, 0);
                }
                mybg[i].xcount = 0;
                if (mybg[i].xwait-- == waittb[i]) {
                    mybg[i].xmode = 2;
                    mybg[i].xwait = waittb[i];
                }
            }
            break;
        case 2:
            if (mybg[i].xcount++ == mybg[i].xwait) {
                if (mybg[i].xflag) {
                    mybg_addxy(bgsub_idtb[i], 1, 0);
                } else {
                    mybg_addxy(bgsub_idtb[i], -1, 0);
                }
                mybg[i].xcount = 0;
                if (--mybg[i].xtime == -1) {
                    mybg[i].xmode = 3;
                }
            }
            break;
        case 3:
            if (mybg[i].xcount++ == mybg[i].xwait) {
                if (mybg[i].xflag) {
                    mybg_addxy(bgsub_idtb[i], 1, 0);
                } else {
                    mybg_addxy(bgsub_idtb[i], -1, 0);
                }
                mybg[i].xcount = 0;
                if (++mybg[i].xwait == 11) {
                    mybg[i].xmode = 0;
                }
            }
            break;
        }
        switch (mybg[i].ymode) {
        case 0:
            mybg[i].ywait = 10;
            mybg[i].ytime = Random() & 0x1F;
            mybg[i].ymode = 1;
            mybg[i].ycount = 0;
            if (mybg[i].yflag) {
                mybg[i].yflag = 0;
            } else {
                mybg[i].yflag = 1;
            }
            break;
        case 1:
            if (mybg[i].ycount++ == mybg[i].ywait) {
                if (mybg[i].yflag) {
                    mybg_addxy(bgsub_idtb[i], 0, 1);
                } else {
                    mybg_addxy(bgsub_idtb[i], 0, -1);
                }
                mybg[i].ycount = 0;
                if (mybg[i].ywait-- == waittb[i]) {
                    mybg[i].ymode = 2;
                    mybg[i].ywait = waittb[i];
                }
            }
            break;
        case 2:
            if (mybg[i].ycount++ == mybg[i].ywait) {
                if (mybg[i].yflag) {
                    mybg_addxy(bgsub_idtb[i], 0, 1);
                } else {
                    mybg_addxy(bgsub_idtb[i], 0, -1);
                }
                mybg[i].ycount = 0;
                if (--mybg[i].ytime == -1) {
                    mybg[i].ymode = 3;
                }
            }
            break;
        case 3:
            if (mybg[i].ycount++ == mybg[i].ywait) {
                if (mybg[i].yflag) {
                    mybg_addxy(bgsub_idtb[i], 0, 1);
                } else {
                    mybg_addxy(bgsub_idtb[i], 0, -1);
                }
                mybg[i].ycount = 0;
                if (++mybg[i].ywait == 11) {
                    mybg[i].ymode = 0;
                }
            }
            break;
        }
    }
}

void dfree(void *mem);

void MaySHPEnd(void)
{
    dfree(mybg);
    mybg = 0;
}

void MaySHPOrg(void)
{
    int id;

    for (id = 0; id < 3; id++) {
        mybg_setx(bgsub_idtb[id], mybg[id].xorg);
        mybg_sety(bgsub_idtb[id], mybg[id].yorg);
    }
}

void PoBarExe(void)
{
    static short int wait = 80;

    if (--wait == -1) {
        if (mybg_addxy(3, 1, 0) >= 760) {
            mybg_setx(3, -510);
        }
        wait = 80;
    }
}

extern u_int TotalCounter;

void PoEyeExe(void)
{
    static short int flg;

    if (TotalCounter & 0xF) {
        return;
    }
    if (Random() & 3) {
        return;
    }
    if (flg) {
        BGLayerVisualOFF(30);
        flg = 0;
    } else {
        BGLayerVisualON(30);
        flg = 1;
    }
}

void Gray_Exe(void)
{
    if ((Random() & 0x7F) == 0) {
        BGLayerVisualON(20);
    } else {
        BGLayerVisualOFF(20);
    }
}

void gCurrentColorSet(float val);
void gCurrentColorSetARBGFade(unsigned int arbg, float val);
void gCurrentColorClear(void);
void nlSetBackColor(int c1, int c2, int c3);
void nlDispStripTex(int num, NLVERTEX03 *vt, int spr);

/* TYPE_STANDIN: NLVERTEX03 with 16-byte alignment. The original type is 16-aligned (STABS does not record
   alignment, so generated/types/NLVERTEX03.h lacks it). With 4-byte alignment expand_expr pre-adds each
   field offset to the frame (vt+4, vt+8, ... hoisted by gcse), and loop.c cannot combine the strip givs. */
typedef struct {
    float x;
    float y;
    float z;
    float w;
    float u;
    float v;
    u_int col;
    u_int ofs;
} __attribute__((aligned(16))) NLVERTEX03A;

void MayCloud_Execute(BG_WORK *offset)
{
    NLVERTEX03A vt[32];
    NLVERTEX00 v0[6];
    int x;
    int y;
    float offy[2];
    float trnsl;
    float f;
    float fade;
    unsigned int col;
    int last;
    unsigned int alfa[2];

    col = gCurrentColorGet();
    trnsl = GetChaseTrnsl(0);
    fade = trnsl * ((col & 0xFF) / 255.0f);
    v0[0].x = 0.0f; v0[0].y = 0.0f; v0[0].z = 700.0f;
    v0[1].x = 640.0f; v0[1].y = 0.0f; v0[1].z = 700.0f;
    v0[0].col = v0[1].col = 0xFF000000 | ((int)(fade * 40.0f) << 16) | ((int)(fade * 80.0f) << 8) | (int)(fade * 255.0f);
    v0[2].x = 0.0f; v0[2].y = 240.0f; v0[2].z = 700.0f;
    v0[3].x = 640.0f; v0[3].y = 240.0f; v0[3].z = 700.0f;
    v0[2].col = v0[3].col = 0xFF000000 | ((int)(fade * 160.0f) << 16) | ((int)(fade * 230.0f) << 8) | (int)(fade * 255.0f);
    v0[4].x = 0.0f; v0[4].y = 480.0f; v0[4].z = 700.0f;
    v0[5].x = 640.0f; v0[5].y = 480.0f; v0[5].z = 700.0f;
    v0[4].col = v0[5].col = 0xFF000000 | ((int)(fade * 180.0f) << 16) | ((int)(fade * 255.0f) << 8) | (int)(fade * 255.0f);
    nlDispStrip(6, v0);
    SetBlendingMode(1);
    for (y = 2; y < 8; y++) {
        offy[0] = y - (float)(TotalCounter % 48) / 48.0f;
        offy[1] = offy[0] + 1.0f;
        for (x = 0; x < 2; x++) {
            f = (offy[x] - 1.0f) / 3.0f + 1.0f;
            offy[x] += f * f - 1.3f;
            alfa[x] = 1600.0f / offy[x];
            if (alfa[x] >= 256) {
                alfa[x] = 255;
            }
            if (alfa[x] <= 100) {
                alfa[x] = 0;
            } else {
                alfa[x] -= 100;
            }
            alfa[x] = alfa[x] * trnsl;
        }
        last = 0;
        for (x = 0; x < 16 && !last; x++) {
            vt[x * 2].x = x * 640.0f / offy[0];
            vt[x * 2].y = 360.0f - 1000.0f / offy[0];
            vt[x * 2 + 1].x = x * 640.0f / offy[1];
            vt[x * 2 + 1].y = 360.0f - 1000.0f / offy[1];
            if (vt[x * 2].x > 640.0f && vt[x * 2 + 1].x > 640.0f) {
                last = 1;
            }
            vt[x * 2].z = vt[x * 2 + 1].z = 680.0f;
            vt[x * 2].u = 0.0f;
            vt[x * 2 + 1].u = 1.0f;
            if (x & 1) {
                vt[x * 2].v = 0.0f;
                vt[x * 2 + 1].v = 0.0f;
            } else {
                vt[x * 2].v = 1.0f;
                vt[x * 2 + 1].v = 1.0f;
            }
            gCurrentColorSet(1.0f);
            vt[x * 2].col = gCurrentColorGet() | (alfa[0] << 24);
            vt[x * 2 + 1].col = gCurrentColorGet() | (alfa[1] << 24);
        }
        nlDispStripTex(x * 2, (NLVERTEX03 *)vt, offset->SpriteId);
    }
    SetBlendingMode(1);
    {
        float y;
        float c;

        y = vt[1].y;
        c = (y - 240.0f) / 240.0f;
        gCurrentColorSetARBGFade(((int)(c * 40.0f + 140.0f) << 16) | ((int)(c * 55.0f + 200.0f) << 8) | 0xFF, fade);
    }
    {
        unsigned int col;

        col = gCurrentColorGet();
        nlSetBackColor(col, col, col);
    }
    gCurrentColorClear();
}
