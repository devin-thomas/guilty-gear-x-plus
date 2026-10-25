#include "generated/opt_ajst.h"

extern PAD pads[2];
extern SYS_COND sc;
extern USER_SETTING userinfo;
extern OPTINFO opt;
int sound_request(int no);
int iodat_chk(int stat);
void FontDraw(char *str, int x, int y, float z, int alfa);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void set_char2num(int num, char *block, int id);

void Opt_DispAdjust(int sw)
{
    int n;
    int pad;
    char buf[16];
    static int pos;
    static int flg;
    static HEADWORK hw[4] = {
        { "POSITION V", 64.0f, 112.0f, 0, 0, 0 },
        { "POSITION H", 64.0f, 160.0f, 0, 0, 0 },
        { "DEFAULT", 64.0f, 208.0f, 0, 0, 0 },
        { "EXIT", 64.0f, 256.0f, 0, 0, 0 },
    };
    int alpha;

    if (sw == 0) {
        hw[0].pos = userinfo.ypos; hw[1].pos = userinfo.xpos; flg = 0; pos = 0; sc.cnt[0] = 0; sc.cnt[1] = 0;

        return;
    }
    if (pos != -1 && !((pads[0].trg | pads[1].trg) & 0x840)) {
    FontDrawDC("ADJUST SETTINGS", 32.0f, 40.0f, 4.0f, 1, 0);
    for (n = 0; n < 4; n++) {
        if (n == pos) alpha = 1; else alpha = 0xa0;
        FontDrawDC(hw[n].head, hw[n].xp, hw[n].yp, 4.0f, alpha, 1);
    }
    if ((pad = iodat_chk(0x1000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            pos = pos ? pos - 1 : 3;
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x4000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            sound_request(0x37);
            pos = (pos == 3) ? 0 : pos + 1;
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x8000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            if (pos < 2) {
                if (hw[pos].pos > -15) {
                    sound_request(0x37);
                    hw[pos].pos--;
                } else {
                    hw[pos].pos = -15;
                }
            }
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x2000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            if (pos < 2) {
                if (hw[pos].pos < 15) {
                    sound_request(0x37);
                    hw[pos].pos++;
                } else {
                    hw[pos].pos = 15;
                }
            }
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
    switch (pos) {
    case 0:
    case 1:
        userinfo.xpos = hw[1].pos;
        userinfo.ypos = hw[0].pos;
        break;
    case 2:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            userinfo.xpos = 0;
            hw[0].pos = 0;
            userinfo.ypos = 0;
            hw[1].pos = 0;
        }
        break;
    case 3:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg == 0) {
                sound_request(0x39);
                pos = -1;
            }
        }
        break;
    }
    if (userinfo.ypos < 0) buf[0] = '-'; else buf[0] = '+';
    buf[1] = 0;
    FontDraw(buf, 320, 116, 4.0f, 0xff);
    set_char2num(userinfo.ypos, buf, 0);
    FontDrawDC(buf, 336.0f, 112.0f, 4.0f, 1, 1);
    if (userinfo.xpos < 0) buf[0] = '-'; else buf[0] = '+';
    buf[1] = 0;
    FontDraw(buf, 320, 164, 4.0f, 0xff);
    set_char2num(userinfo.xpos, buf, 0);
    FontDrawDC(buf, 336.0f, 160.0f, 4.0f, 1, 1);
    if (flg) flg = 0;
    } else {
        if ((pads[0].trg | pads[1].trg) & 0x840) sound_request(0x3b);
        opt.thdflg &= ~0x10;
    }
}
