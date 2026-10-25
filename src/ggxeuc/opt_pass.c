#include "generated/opt_pass.h"

static const short idtb[] = { 1, 2, 3 };
extern PAD pads[2];
extern SYS_COND sc;
extern OPTINFO opt;
extern USER_SETTING userinfo;
int sound_request(int no);
int iodat_chk(int stat);
int SetLimitStatus(void);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void FontDrawDCCenter(char *str, float x, float y, float z, int alpha, int id);

void Opt_Password(int sw)
{
    int i;
    int j;
    int n;
    int pad;
    char buf[16];
    char *pass[37] = { "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    int password[6][8] = {
        { 13, 10, 14, 11, 16, 14, 16, 14 },
        { 11, 25, 21, 19, 2, 14, 2, 14 },
        { 30, 7, 15, 21, 28, 19, 1, 4 },
        { 34, 9, 18, 1, 11, 21, 19, 1 },
        { 19, 29, 11, 35, 1, 30, 11, 28 },
        { 9, 14, 9, 20, 9, 1, 12, 4 },
    };
    int passno;
    int passflag;
    static int pos;
    static int flg;
    static HEADWORK hw[2] = {
        { "INPUT", 64.0f, 112.0f, 0, 0, 0 },
        { "EXIT", 64.0f, 160.0f, 0, 0, 0 },
    };
    static int passbuf[8];
    static int passmode;
    static int passidx;
    int alpha;

    if (sw == 0) {
        for (i = 0; i < 16; i++) buf[i] = 0;
        for (i = 0; i < 8; i++) passbuf[i] = 0;
flg = 0; sc.cnt[0] = passidx = passmode = pos = 0; sc.cnt[1] = 0;        return;
    }
    FontDrawDC("PASSWORD", 32.0f, 40.0f, 4.0f, 1, 0);
    for (n = 0; n < 2; n++) {
        if (n == pos) alpha = 1; else alpha = 0xa0;
        FontDrawDC(hw[n].head, hw[n].xp, hw[n].yp, 4.0f, alpha, 1);
    }
    if (passmode == 0) {
        if (pos != -1 && !((pads[0].trg | pads[1].trg) & 0x840)) {
        if ((pad = iodat_chk(0x1000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = pos ? pos - 1 : 1;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = (pos == 1) ? 0 : pos + 1;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x8000)) || (pad = iodat_chk(0x2000))) {
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        switch (pos) {
        case 0:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                if (flg) {
                    flg = 0;
                    return;
                }
                sound_request(0x39);
                passbuf[0] = 1;
                passmode = 1;
                passidx = 0;
            }
            break;
        case 1:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                if (flg) {
                    flg = 0;
                    return;
                }
                sound_request(0x39);
                pos = -1;
            }
            break;
        }
        if (flg) flg = 0;
        } else {
            if ((pads[0].trg | pads[1].trg) & 0x840) sound_request(0x3b);
            opt.thdflg &= ~0x40;
        }
        return;
    }
    FontDrawDCCenter("INPUT PASSWORD FROM GGP2", 320.0f, 240.0f, 4.0f, 1, 1);
    for (i = 0; i < 8; i++) {
        if (passbuf[i]) FontDrawDCCenter(pass[passbuf[i]], 192 + i * 32, 288.0f, 4.0f, 1, 1);
    }
    if (passidx == 8) {
    } else if (passidx == 9) {
        FontDrawDCCenter("OK", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("PASSWORD 1 INPUT COMPLETED", 320.0f, 384.0f, 4.0f, 1, 1);
    } else if (passidx == 10) {
        FontDrawDCCenter("OK", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("PASSWORD 2 INPUT COMPLETED", 320.0f, 384.0f, 4.0f, 1, 1);
    } else if (passidx == 11) {
        FontDrawDCCenter("OK", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("PASSWORD 3 INPUT COMPLETED", 320.0f, 384.0f, 4.0f, 1, 1);
    } else if (passidx == 12) {
        FontDrawDCCenter("OK", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("EX PASSWORD 1 INPUT COMPLETED", 320.0f, 384.0f, 4.0f, 1, 1);
    } else if (passidx == 13) {
        FontDrawDCCenter("OK", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("EX PASSWORD 2 INPUT COMPLETED", 320.0f, 384.0f, 4.0f, 1, 1);
    } else if (passidx == 14) {
        FontDrawDCCenter("OK", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("INIT PASSWORD INPUT COMPLETED", 320.0f, 384.0f, 4.0f, 1, 1);
    } else if (passidx == 15) {
        FontDrawDCCenter("NG", 320.0f, 336.0f, 4.0f, 1, 1);
        FontDrawDCCenter("PASSWORD INPUT FAILED", 320.0f, 384.0f, 4.0f, 1, 1);
    }
    if (pos != -1 && !((pads[0].trg | pads[1].trg) & 0x840)) {
    if (passidx < 8) {
        if ((pad = iodat_chk(0x1000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (--passbuf[passidx] <= 0) passbuf[passidx] = 36;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (++passbuf[passidx] >= 37) passbuf[passidx] = 1;
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
    }
    switch (passidx) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            if (passbuf[passidx] > 0) {
                sound_request(0x39);
                passidx++;
                passbuf[passidx] = 1;
            }
        }
        break;
    case 8:
        passno = -1;
        for (i = 0; i < 6; i++) {
            if (passno == -1) {
                passflag = 1;
                for (j = 0; j < 8; j++) {
                    if (passbuf[j] != password[i][j]) passflag = 0;
                }
                if (passflag) passno = i;
            }
        }
        sound_request(0x39);
        switch (passno) {
        case 0:
            for (i = 0; i < 18; i++) userinfo.usecol[i] = 0x7ff;
            passidx = 9;
            break;
        case 1:
            if (userinfo.phase < 3) userinfo.phase = 3;
            passidx = 10;
            SetLimitStatus();
            break;
        case 2:
            for (i = 24; i < 32; i++) userinfo.gstart[i] = 1;
            passidx = 11;
            break;
        case 3:
            if (userinfo.phase < 4) userinfo.phase = 4;
            SetLimitStatus();
            passidx = 12;
            break;
        case 4:
            if (userinfo.phase < 5) userinfo.phase = 5;
            SetLimitStatus();
            passidx = 13;
            break;
        case -1:
        case 5:
        default:
            passidx = 15;
            break;
        }
        break;
    case 9:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        }
        break;
    case 10:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        }
        break;
    case 11:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        }
        break;
    case 12:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        }
        break;
    case 13:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        }
        break;
    case 14:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        }
        break;
    case 15:
        if ((pads[0].trg | pads[1].trg) & 0x20) {
            if (flg) {
                flg = 0;
                return;
            }
            sound_request(0x3b);
            passmode = 1;
            passidx = 7;
        }
        break;
    }
    if (flg) flg = 0;
    } else {
        if ((pads[0].trg | pads[1].trg) & 0x800) {
            sound_request(0x3b);
            passmode = 0;
            passidx = 0;
            for (i = 0; i < 8; i++) passbuf[i] = 0;
        } else if ((pads[0].trg | pads[1].trg) & 0x40) {
            sound_request(0x3b);
            passbuf[passidx] = 0;
            if (passidx >= 1 && passidx <= 8) {
                passidx--;
            } else {
                passmode = 0;
                passidx = 0;
                for (i = 0; i < 8; i++) passbuf[i] = 0;
            }
        }
    }
}
