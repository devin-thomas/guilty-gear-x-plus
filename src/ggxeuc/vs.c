#include "generated/vs.h"

extern int rannyuuflag;
extern int SubJob;
extern int SubJobCount;
static int skip;
extern short int UseCharacter1p2p[];
void pack_allfree(void);
char *FileReadSelect(int fid, int chan);
void FileRead(int fid, void *adr);
void JobNext(void);
void FadeInit(void);
void EnterFight_Main(void);
int FileReadCheck(void);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void LinkAddrTb(u_int *addr);
int IST_Load(int *texadr, int sprid, int palid);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
unsigned int gStripGetFadeColor(unsigned int color);
void Read_Playdat(int size);
int IsRannyuu(void);
void DispCreditMsg(void);
static NL_SPR_ARG sprite[3];
extern int StageNo;
void BGM_KeepRequest(int no, int loop);
void EasySpriteDrawM(int id, int x, int y, float z, float u, int flg);
void EasySpriteDrawVS(int id, int x, int y, float z);
void SetStageNo(int no);
void set_bgmvol(void);
void SetFadeSpeed(int frame);
void ForeStellPlayer(void);
void Read_Playdat(int flag);
float nlCos(short unsigned int r);
float nlSin(short unsigned int r);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
static int namewidth[18] = { 0xca, 0x92, 0x47, 0xc4, 0x8f, 0xa3, 0xdf, 0xa8, 0x73, 0x64, 0xb7, 0x116, 0xa3, 0x7e, 0x72, 0x5a, 0x100, 0x90 };
extern short unsigned int cp_SprtIdx[128];
int IsDemoBattle(void);
void BGM_Free(void);
void PXF_Free(int id, int pal);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
void Rq_Fadeout(void);
int IsFadeEnd(void);
void sound_alloff(void);
void dfree(void *mem);
void JobChange(int job);

void VsScrEnd(void) {
}

void VsScrInit(void) {
    skip = 0;
    pack_allfree();
    VsFile[0] = FileReadSelect(0xa5, 2);
    if (UseCharacter1p2p[0] == UseCharacter1p2p[1]) {
        VsFile[1] = VsFile[2] = FileReadSelect(UseCharacter1p2p[0] + 0xa5, 3);
    } else {
        VsFile[1] = FileReadSelect(UseCharacter1p2p[0] + 0xa5, 3);
        VsFile[2] = FileReadSelect(UseCharacter1p2p[1] + 0xa5, 4);
    }
    FileRead(0x1a, (void *)1);
    JobNext();
}

void VsScrLoad(void) {
    int *p;
    int i;
    int j;
    if (FileReadCheck()) {
        nlFreeCommonTextureAll();
        nlFreePaletteAll();
        p = (int *)VsFile[0];
        LinkAddrTb((u_int *)p);
        p = (int *)*p;
        LinkAddrTb((u_int *)p);
        for (i = 0; p[i]; i++) {
            IST_Load(&p[i], i, -1);
        }
        for (i = 0; i < 2; i++) {
            if (i != 0) {
                p = (int *)VsFile[2];
            } else {
                p = (int *)VsFile[1];
            }
            LinkAddrTb((u_int *)p);
            IST_Load(p, i ? 11 : 10, -1);
            p = (int *)VsFile[0];
            p = (int *)p[UseCharacter1p2p[i]];
            LinkAddrTb((u_int *)p);
            for (j = 0; j < 3; j++, p++) {
                IST_Load(p, i != 0 ? j + 7 : j + 4, -1);
            }
        }
        CockpitLoadInit();
        CreditFontLoad(20);
        rannyuuflag = 0;
        SubJob = 0;
        SubJobCount = 0;
        JobNext();
    }
}

void VsScrSub0(void) {
    static int x;
    static float x1;
    static float x2;
    static float x3;
    static float x4;
    static float y1;
    static float y2;
    static float y3;
    static float y4;
    if (SubJobCount == 0) {
        int i;
        for (i = 0; i < 3; i++) {
            sprite[i].zm_x = 1.0f;
            sprite[i].zm_y = 1.0f;
            sprite[i].ang = 0;
            sprite[i].u0 = 0.0f;
            sprite[i].u1 = 1.0f;
            sprite[i].v0 = 0.0f;
            sprite[i].v1 = 1.0f;
            sprite[i].listType = 1;
            sprite[i].trnsl = 0.0f;
            sprite[i].attr = 13;
        }
        sprite[0].sprno = 1;
        sprite[1].sprno = 2;
        sprite[2].sprno = 3;
        sprite[0].z = 7.0f;
        sprite[1].z = 9.0f;
        sprite[2].z = 5.0f;
        FadeInit();
        SetFadeSpeed(20);
        x1 = 156.0f;
        x2 = 157.0f;
        x3 = 484.0f;
        x4 = 485.0f;
        x = 0; y1 = 0.0f; y2 = 0.0f; y3 = 480.0f; y4 = 480.0f;
        BGM_KeepRequest(0x1b, 0);
        set_bgmvol();
        SetStageNo(StageNo);
    }
    Read_Playdat(0x80);
    rannyuuflag |= IsRannyuu();
    DispCreditMsg();
    if (x < 480) {
        polydraw_FBox(0.0f, x, 160.0f, 480.0f, 11.0f, 0xff000000);
        polydraw_FBox(480.0f, 0.0f, 640.0f, 480 - x, 11.0f, 0xff000000);
    } else {
        polydraw_FBox(0.0f, 0.0f, 160.0f, x - 1024, 11.0f, 0xff000000);
        polydraw_FBox(480.0f, 480.0f, 640.0f, 1504 - x, 11.0f, 0xff000000);
    }
    if (SubJobCount < 40) {
        y1 += 6.0;
        y3 -= 6.0;
    } else {
        y1 += 60.0 / 13.0;
        y3 -= 60.0 / 13.0;
        x1 += 163.0 / 52.0;
        x3 -= 163.0 / 52.0;
        if (x1 > 320.0) x1 = 320.0f;
        if (x3 < 320.0) x3 = 320.0f;
        x2 = x1 + 1.0;
        x4 = x3 - 1.0;
    }
    polydraw_FBox(x1, y1, x2, y2, 11.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(x3, y3, x4, y4, 11.0f, gStripGetFadeColor(0xffffffff));
    EasySpriteDrawM(6, 0, x, 13.0f, 0.0f, 13);
    EasySpriteDrawM(6, 0, x - 512, 13.0f, 0.5f, 13);
    EasySpriteDrawM(9, 0x180, 992 - x, 13.0f, 0.5f, 0x35);
    EasySpriteDrawM(9, 0x180, 480 - x, 13.0f, 0.0f, 0x35);
    EasySpriteDrawVS(10, 0x50, 0x1e0, 15.0f);
    EasySpriteDrawVS(11, 0x230, 0x1e0, 15.0f);
    x += 19;
    if (++SubJobCount >= 108) {
        SubJobCount = 0;
        SubJob++;
    }
}

void VsScrSub1(void) {
    static float x1;
    static float x2;
    static float x3;
    static float x4;
    static float y1;
    static float y2;
    static float y3;
    static float y4;
    if (SubJobCount == 0) {
        ForeStellPlayer();
        x3 = x1 = 319.0f;
        x4 = x2 = 321.0f;
        y1 = 120.0f;
        y2 = 121.0f;
        y3 = 360.0f;
        y4 = 361.0f;
    }
    Read_Playdat(0x80);
    rannyuuflag |= IsRannyuu();
    DispCreditMsg();
    if (SubJobCount < 31) {
        x1 -= 26.666666666666668;
        x2 += 26.666666666666668;
        x3 = x1;
        x4 = x2;
    } else {
        y1 -= 2.0;
        y2 += 2.0;
        y3 -= 2.0;
        y4 += 2.0;
    }
    polydraw_FBox(x1, y1, x2, y2, 11.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(x3, y3, x4, y4, 11.0f, gStripGetFadeColor(0xffffffff));
    if (++SubJobCount >= 60) {
        SubJobCount = 0;
        SubJob++;
    }
}

void VsScrSub2(void) {
    static float x1;
    static float x2;
    static float x3;
    static float x4;
    static float x5;
    static float x6;
    static float x7;
    static float x8;
    static float y1;
    static float y2;
    static float y3;
    static float y4;
    static int x;
    static int count;
    static int bright;
    static u_short ang;
    int i;
    if (SubJobCount == 0) {
        y1 = 54.0f;
        y2 = 186.0f;
        y3 = 294.0f;
        y4 = 426.0f;
        x1 = 640.0f;
        x2 = 640.0f;
        x3 = 0.0f;
        x4 = 0.0f;
        x5 = -640.0f;
        x6 = 640.0f;
        x7 = 640.0f;
        x8 = 0.0f;
        x = 0;
        count = 0;
        bright = 0;
        ang = 0xc000;
        sprite[2].x = 184.0f;
        sprite[2].y = 252.0f;
    }
    rannyuuflag |= IsRannyuu();
    DispCreditMsg();
    if (SubJobCount >= 27) {
        if (SubJobCount < 41) {
            x1 -= 320.0 / 7.0;
            x4 += 320.0 / 7.0;
        } else {
            x1 = 0.0f;
            x4 = 640.0f;
        }
    }
    if (SubJobCount >= 9) {
        if (SubJobCount < 21) {
            x5 += 640.0 / 12.0;
            x6 -= 640.0 / 12.0;
        } else {
            x5 = 0.0f;
            x6 = 0.0f;
        }
    }
    if (SubJobCount >= 101) {
        if (SubJobCount < 106) {
            bright += 42;
        } else if (SubJobCount == 106) {
            bright = 255;
        } else {
            bright = 0;
        }
    }
    if (SubJobCount >= 7) {
        if (SubJobCount < 19) {
            x7 -= 610.0 / 12.0;
            x8 += 610.0 / 12.0;
        } else {
            x7 = 30.0f;
            x8 = 610.0f;
        }
    }
    if (SubJobCount >= 107) {
        if (SubJobCount < 123) {
            sprite[2].trnsl += 0.0625;
        } else {
            sprite[2].trnsl = 1.0f;
        }
    }
    if (SubJobCount < 40) {
        sprite[0].x = nlSin(ang) * 25.0 + 284.0;
        sprite[0].y = nlCos(ang) * 25.0 + 272.0;
        sprite[1].x = nlSin(ang + 0x8000) * 25.0 + 284.0;
        sprite[1].y = nlCos(ang + 0x8000) * 25.0 + 272.0;
    } else if (SubJobCount < 57) {
        sprite[0].x = nlSin(ang) * 25.0 + 284.0;
        sprite[0].y = nlCos(ang) * 25.0 + 272.0;
        sprite[1].x = nlSin(ang + 0x8000) * 25.0 + 284.0;
        sprite[1].y = nlCos(ang + 0x8000) * 25.0 + 272.0;
        ang += 0x800;
    } else if (SubJobCount < 100) {
        sprite[0].x -= 25.0 / 22.0;
        sprite[1].x += 25.0 / 22.0;
    } else {
        sprite[0].x = 260.0f;
        sprite[1].x = 308.0f;
        sprite[2].x = 184.0f;
    }
    if (SubJobCount >= 41) {
        if (SubJobCount < 49) {
            sprite[0].trnsl += 0.05;
            sprite[1].trnsl += 0.05;
        } else if (SubJobCount < 57) {
            sprite[0].trnsl -= 0.025;
            sprite[1].trnsl -= 0.025;
        } else if (SubJobCount >= 81) {
            if (SubJobCount < 100) {
                sprite[0].trnsl += 0.04;
                sprite[1].trnsl += 0.04;
            } else if (SubJobCount == 100) {
                sprite[0].trnsl = 1.0f;
                sprite[1].trnsl = 1.0f;
            }
        }
    }
    if (SubJobCount >= 171) {
        polydraw_FBox(x + 320, 0.0f, 320 - x, 480.0f, 3.0f, 0xff000000);
        polydraw_GBox(x + 352, 0.0f, x + 320, 480.0f, 3.0f, 0x01000000, 0xff000000, 0x01000000, 0xff000000);
        polydraw_GBox(288 - x, 0.0f, 320 - x, 480.0f, 3.0f, 0x01000000, 0xff000000, 0x01000000, 0xff000000);
        x += 16;
    }
    EasySpriteDrawEX(4, x5, 0xb4, 11.0f, 0xd, 1.0f);
    EasySpriteDrawEX(7, x6, 0x1a4, 11.0f, 0x1d, 1.0f);
    EasySpriteDrawEX(5, x7 + 48.0f, 0xa4, 9.0f, 0xd, 1.0f);
    EasySpriteDrawEX(8, x8 - 48.0f - namewidth[UseCharacter1p2p[1] - 1], 0x194, 11.0f, 0xd, 1.0f);
    EasySpriteDrawEX(0, x7, 0xa8, 7.0f, 0xd, 1.0f);
    EasySpriteDrawEX(0, x8, 0x198, 7.0f, 0x1f, 1.0f);
    polydraw_FBox(x7 + 8.0f, 162.0f, 640.0f, 165.0f, 7.0f, 0xff000000);
    polydraw_FBox(0.0f, 402.0f, x8 - 8.0f, 405.0f, 7.0f, 0xff000000);
    polydraw_FBox(0.0f, 61.0f, 640.0f, 180.0f, 13.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(0.0f, 301.0f, 640.0f, 420.0f, 13.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(x1, y1, x2, y1 + 1.0f, 11.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(x1, y2, x2, y2 + 1.0f, 11.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(x3, y3, x4, y3 + 1.0f, 11.0f, gStripGetFadeColor(0xffffffff));
    polydraw_FBox(x3, y4, x4, y4 + 1.0f, 11.0f, gStripGetFadeColor(0xffffffff));
    polydraw_GBox(0.0f, y2, 640.0f, 240.0f, 3.0f, (bright / 2) << 24 | 0xffffff, (bright / 2) << 24 | 0xffffff, bright << 24 | 0xffffff, bright << 24 | 0xffffff);
    polydraw_GBox(0.0f, 240.0f, 640.0f, y3, 3.0f, (bright / 2) << 24 | 0xffffff, (bright / 2) << 24 | 0xffffff, bright << 24 | 0xffffff, bright << 24 | 0xffffff);
    for (i = 0; i < 3; i++) {
        gSpriteDraw_NaomiLibVersion(&sprite[i]);
    }
    if (++SubJobCount >= 190) {
        SubJobCount = 1;
        SubJob++;
    }
}

void VsScrSkip(void) {
    static float x5;
    static float x6;
    static float x7;
    static float x8;
    int i;
    int color;
    if (SubJobCount == 0) {
        if (!rannyuuflag) ForeStellPlayer();
        x5 = 0.0f;
        x6 = 0.0f;
        x7 = 30.0f;
        x8 = 610.0f;
        sprite[0].x = 260.0f;
        sprite[0].y = 272.0f;
        sprite[1].x = 308.0f;
        sprite[1].y = 272.0f;
        sprite[2].x = 184.0f;
        sprite[2].y = 252.0f;
        sprite[0].trnsl = 1.0f;
        sprite[1].trnsl = 1.0f;
        sprite[2].trnsl = 1.0f;
        skip = 1;
        FadeInit();
        SetFadeSpeed(60);
        Rq_Fadeout();
    }
    if (!IsFadeEnd() || rannyuuflag) {
        Read_Playdat(0x80);
        color = gStripGetFadeColor(0xffffffff);
        DispCreditMsg();
        EasySpriteDrawEX(4, x5, 0xb4, 11.0f, 0xd, 1.0f);
        EasySpriteDrawEX(7, x6, 0x1a4, 11.0f, 0x1d, 1.0f);
        EasySpriteDrawEX(5, x7 + 48.0f, 0xa4, 9.0f, 0xd, 1.0f);
        EasySpriteDrawEX(8, x8 - 48.0f - namewidth[UseCharacter1p2p[1] - 1], 0x194, 9.0f, 0xd, 1.0f);
        EasySpriteDrawEX(0, x7, 0xa8, 7.0f, 0xd, 1.0f);
        EasySpriteDrawEX(0, x8, 0x198, 7.0f, 0x1f, 1.0f);
        polydraw_FBox(x7 + 8.0f, 162.0f, 640.0f, 165.0f, 7.0f, 0xff000000);
        polydraw_FBox(0.0f, 402.0f, x8 - 8.0f, 405.0f, 7.0f, 0xff000000);
        polydraw_FBox(0.0f, 60.0f, 640.0f, 180.0f, 13.0f, color);
        polydraw_FBox(0.0f, 300.0f, 640.0f, 420.0f, 13.0f, color);
        polydraw_FBox(0.0f, 54.0f, 640.0f, 55.0f, 11.0f, color);
        polydraw_FBox(0.0f, 186.0f, 640.0f, 187.0f, 11.0f, color);
        polydraw_FBox(0.0f, 294.0f, 640.0f, 295.0f, 11.0f, color);
        polydraw_FBox(0.0f, 426.0f, 640.0f, 427.0f, 11.0f, color);
        for (i = 0; i < 3; i++) {
            gSpriteDraw_NaomiLibVersion(&sprite[i]);
        }
        SubJobCount++;
        return;
    } else {
        sound_alloff();
        BGM_Free();
        for (i = 0; i < 4; i++) {
            PXF_Free(i, 0);
        }
        PXF_Free(10, 0);
        PXF_Free(5, 0);
        PXF_Free(6, 0);
        PXF_Free(11, 0);
        PXF_Free(8, 0);
        PXF_Free(9, 0);
        PXF_Free(cp_SprtIdx[0x43], 0);
        PXF_Free(cp_SprtIdx[0x44], 0);
        PXF_Free(cp_SprtIdx[0x45], 0);
        dfree(VsFile[0]);
        VsFile[0] = 0;
        if (VsFile[1] == VsFile[2]) {
            dfree(VsFile[1]);
            VsFile[0] = 0;
        } else {
            dfree(VsFile[1]);
            dfree(VsFile[2]);
            VsFile[1] = 0;
            VsFile[2] = 0;
        }
        if (IsDemoBattle() && skip) {
            JobChange(6);
        } else {
            JobChange(0x1e);
        }
    }
}

void VsScrMain(void) {
    if (SubJob == 0) {
        VsScrSub0();
    } else if (SubJob == 1) {
        VsScrSub1();
    } else if (SubJob == 2) {
        VsScrSub2();
    } else {
        VsScrSkip();
    }
    if (rannyuuflag) {
        FadeInit();
        EnterFight_Main();
    }
}
