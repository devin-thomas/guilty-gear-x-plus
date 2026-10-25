#include "generated/g_grasub.h"

extern int FlashOutFms;

extern GAME_INFOMATION g_info;
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void FlashExecute(void)
{
    unsigned int Ccode[16] = {
        0xFF000000, 0xFF7F0000, 0xFF007F00, 0xFF7F7F00, 0xFF00007F, 0xFF7F007F, 0xFF007F7F, 0xFF7F7F7F,
        0xFFC0C0C0, 0xFFFF0000, 0xFF00FF00, 0xFFFFFF00, 0xFF0000FF, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF
    };
    int Pcode[2] = { 300, 3 };
    GAME_INFOMATION *p;
    unsigned int a;

    p = &g_info;
    if (p->Flash) {
        if (FlashOutFms && FlashOutFms >= p->Flash) {
            a = 0x100 - 0x100 / FlashOutFms;
            polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, Pcode[p->FlashFlag & 0xFF], (a << 24) | (Ccode[(u_short)p->FlashFlag >> 8] & 0xFFFFFF));
            FlashOutFms--;
        } else {
            polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, Pcode[p->FlashFlag & 0xFF], Ccode[(u_short)p->FlashFlag >> 8]);
        }
        p->Flash--;
    }
    if (p->flash_span_count) {
        p->flash_span_count--;
    }
}


void SetFlashOut(int fms) {
    FlashOutFms = fms;
}

extern short unsigned int firepal[];
extern u_int TotalCounter;
void nlSetPaletteData(int id, u_short *tb, int num);

void FirePalette_Load(unsigned int palid) {
    nlSetPaletteData(palid, &firepal[TotalCounter & 0xF], 0xF);
}

extern short unsigned int raipal[];
extern u_int TotalCounter;
void nlSetPaletteData(int id, u_short *tb, int num);

void RaiPalette_Load(unsigned int palid) {
    nlSetPaletteData(palid, &raipal[TotalCounter & 0xF], 0xF);
}

extern int whiteout;
extern GAME_INFOMATION g_info;

void SetFlash(char time, short unsigned int flag) {
    whiteout = 0;
    g_info.Flash = time;
    g_info.FlashFlag = flag;
}
