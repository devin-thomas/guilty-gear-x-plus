#include "generated/opt_cfg.h"

extern USER_SETTING userinfo;
extern PAD pads[2];
int sound_request(int no);
extern CHARACTER_WORK *PlayerWork;
extern GAME_INFOMATION g_info;
void FontDraw(char *str, int x, int y, float z, int alfa);
extern SYS_COND sc;
extern OPTINFO opt;
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
int iodat_chk(int stat);
void Setting_Init(HEADWORK *hw, PARTS *pt);
void Setting_Default(HEADWORK *hw);
static const short idtb[] = { 1, 2, 3 };

static void set_btn_execute(int padid, int pos) {
    unsigned int *p;
    unsigned int *p2;
    unsigned int btn;
    unsigned int bk;
    int i;
    int unable;
    unable = 0;
    if (pads[padid].trg & 0x20) {
        btn = 0x20;
    } else if (pads[padid].trg & 0x40) {
        btn = 0x40;
    } else if (pads[padid].trg & 0x2) {
        btn = 0x2;
    } else if (pads[padid].trg & 0x80) {
        btn = 0x80;
    } else if (pads[padid].trg & 0x10) {
        btn = 0x10;
    } else if (pads[padid].trg & 0x1) {
        btn = 0x1;
    } else if (pads[padid].trg & 0x4) {
        btn = 0x4;
    } else if (pads[padid].trg & 0x8) {
        btn = 0x8;
    } else {
        btn = 0;
    }
    if (btn != 0) if (pos >= 0) if (pos < 10) {
    if (pos >= 0) if (pos < 6) {
        sound_request(0x39);
        p = &userinfo.key[padid].punch;
        p2 = p;
        for (i = 0; i < pos; i++) p++;
        bk = *p;
        *p = btn;
        for (i = 0; i < 10; i++, p2++) {
            if (*p2 == btn && p != p2) *p2 = bk;
        }
    }
    if (btn) if (pos >= 6) if (pos < 10) {
        p = &userinfo.key[padid].punch;
        p2 = p;
        for (i = 0; i < pos; i++) p++;
        bk = *p;
        for (i = 0; i < 6; i++, p2++) {
            if (*p2 == btn && p != p2) unable = 1;
        }
        if (bk == 0 && unable) {
            sound_request(0x3b);
        } else {
            sound_request(0x39);
            p = &userinfo.key[padid].punch;
            p2 = p;
            for (i = 0; i < pos; i++) p++;
            bk = *p;
            *p = btn;
            for (i = 0; i < 10; i++, p2++) {
                if (*p2 == btn && p != p2) *p2 = bk;
            }
        }
    }
    if (!(g_info.mode & 0x1000)) set_activebtn(padid);
    }
}

static void set_purupuru(int padid, int pos) {
    int dat;
    if (pos == 10) {
        if (pads[padid].trg & 0xa000) {
            sound_request(0x37);
            userinfo.key[padid].vibrate ^= 1;
        }
    }
}

static void set_default(int padid, int pos) {
    if (pos == 11 && (pads[padid].trg & 0x22)) {
        sound_request(0x39);
        userinfo.key[padid].punch = 0x80;
        userinfo.key[padid].kick = 0x40;
        userinfo.key[padid].slash = 0x10;
        userinfo.key[padid].hslash = 0x20;
        userinfo.key[padid].fuckin = 0x8;
        userinfo.key[padid].memory = 0x4;
        userinfo.key[padid].pandk = 0;
        userinfo.key[padid].sandhs = 0;
        userinfo.key[padid].pks = 0;
        userinfo.key[padid].pkshs = 0;
        userinfo.key[padid].vibrate = 0;
        if (!(g_info.mode & 0x1000)) set_activebtn(padid);
    }
}

static void disp_activebtn(int padid) {
    int *p;
    int i;
    p = (int *)&userinfo.key[padid];
    for (i = 0; i < 10; i++) {
        switch (*p) {
        case 0x20:
            FontDraw("w", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x40:
            FontDraw("x", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x2:
            FontDraw("R2", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x80:
            FontDraw("y", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x10:
            FontDraw("z", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x1:
            FontDraw("L2", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x4:
            FontDraw("L1", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        case 0x8:
            FontDraw("R1", padid * 256 + 256, i * 24 + 138, 2.0f, 255);
            break;
        }
        p++;
    }
}

void set_activebtn(int padid) {
    PlayerWork[padid].ply->PunchKey = userinfo.key[padid].punch;
    PlayerWork[padid].ply->KickKey = userinfo.key[padid].kick;
    PlayerWork[padid].ply->SlashKey = userinfo.key[padid].slash;
    PlayerWork[padid].ply->HighSlashKey = userinfo.key[padid].hslash;
    PlayerWork[padid].ply->Fuckin = userinfo.key[padid].fuckin;
    PlayerWork[padid].ply->MemoryKey = userinfo.key[padid].memory;
    PlayerWork[padid].ply->PandK = userinfo.key[padid].pandk;
    PlayerWork[padid].ply->SandHS = userinfo.key[padid].sandhs;
    PlayerWork[padid].ply->PKS = userinfo.key[padid].pks;
    PlayerWork[padid].ply->PKSHS = userinfo.key[padid].pkshs;
}

void Opt_GameSetting(int sw)
{
    static int pos;
    static int flg = 0;
    int n;
    int i;
    int cnt;
    int pad;
    static HEADWORK hw[8] = {
        { "GAME LEVEL", 64.0f, 112.0f, 0, 6, 0 },
        { "TIME LIMIT", 64.0f, 112.0f, 0, 4, 0 },
        { "ROUND", 64.0f, 112.0f, 0, 3, 0 },
        { "SHORT CUT", 64.0f, 112.0f, 0, 2, 0 },
        { "MEDAL MODE", 64.0f, 112.0f, 0, 2, 0 },
        { "GGMODE", 64.0f, 112.0f, 0, 5, 0 },
        { "DEFAULT", 64.0f, 112.0f, 0, 0, 0 },
        { "EXIT", 64.0f, 112.0f, 0, 0, 0 },
    };
    PARTS pt[20] = {
        { "BEGINNER", 224.0f, 0.0f, 96 },
        { "EASY", 328.0f, 0.0f, 48 },
        { "NORMAL", 432.0f, 0.0f, 72 },
        { "HARD", 228.0f, 18.0f, 48 },
        { "VERYHARD", 332.0f, 18.0f, 96 },
        { "MANIAC", 436.0f, 18.0f, 72 },
        { "30", 224.0f, 9.0f, 24 },
        { "60", 272.0f, 10.0f, 24 },
        { "99", 320.0f, 10.0f, 24 },
        { "INFINITY", 368.0f, 10.0f, 24 },
        { "1", 224.0f, 10.0f, 12 },
        { "3", 264.0f, 10.0f, 12 },
        { "5", 304.0f, 10.0f, 12 },
        { "OFF", 224.0f, 10.0f, 32 },
        { "ON", 288.0f, 10.0f, 24 },
        { "OFF", 224.0f, 10.0f, 0 },
        { "1P", 288.0f, 10.0f, 0 },
        { "2P", 336.0f, 10.0f, 0 },
        { "BOTH", 384.0f, 10.0f, 0 },
        { "RND", 456.0f, 10.0f, 0 },
    };
    int alpha;
    int ypos;

    if (sw == 0) {
        Setting_Init(hw, pt);
        pos = 0;
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
        return;
    }
    if (pos != -1) {
        FontDrawDC("GAME OPTIONS", 32.0f, 40.0f, 4.0f, 1, 0);
        cnt = 0;
        for (n = 0; n < 8; n++) {
            if (n == 5 && userinfo.phase < 3) continue;
            if (n == pos) alpha = 1; else alpha = 0xa0;
            ypos = hw[n].yp; ypos += cnt * (userinfo.phase < 3 ? 48 : 40);
            FontDrawDC(hw[n].head, hw[n].xp, ypos, 4.0f, alpha, 1);
            if (hw[n].p != 0) {
                for (i = 0; i < hw[n].maxhead; i++) {
                    if (hw[n].pos == i) alpha = 0xff; else alpha = 0x50;
                    ypos = hw[n].yp + hw[n].p[i].yp; ypos += cnt * (userinfo.phase < 3 ? 48 : 40);
                    FontDraw(hw[n].p[i].head, hw[n].xp + hw[n].p[i].xp, ypos, 4.0f, alpha);
                }
            }
            cnt++;
        }
        if ((pad = iodat_chk(0x1000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = pos ? pos - 1 : 7;
                if (pos == 5 && userinfo.phase < 3) pos = 4;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                pos = (pos == 7) ? 0 : pos + 1;
                if (pos == 5 && userinfo.phase < 3) pos = 6;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x8000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (hw[pos].p) {
                    if (hw[pos].pos == 0) hw[pos].pos = hw[pos].maxhead - 1; else hw[pos].pos--;
                }
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x2000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (hw[pos].p) {
                    if (hw[pos].pos >= hw[pos].maxhead - 1) hw[pos].pos = 0;
                    else hw[pos].pos++;
                }
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        if ((pads[0].trg | pads[1].trg) & 0x840) {
            sound_request(0x3b);
            pos = -1;
        }
        switch (pos) {
        case 0:
            userinfo.glevel = hw[pos].pos;
            break;
        case 1:
            userinfo.roundtime = hw[pos].pos;
            break;
        case 2:
            userinfo.roundnum = hw[pos].pos;
            break;
        case 3:
            userinfo.quickconti = hw[pos].pos;
            break;
        case 4:
            userinfo.medal = hw[pos].pos;
            break;
        case 5:
            userinfo.ggmode[0] = 0;
            userinfo.ggmode[1] = 0;
            if (hw[pos].pos & 1) userinfo.ggmode[0] = 1;
            if (hw[pos].pos & 2) userinfo.ggmode[1] = 1;
            if (hw[pos].pos == 4) {
                userinfo.ggmode[0] = -1;
                userinfo.ggmode[1] = -1;
            }
            break;
        case 6:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                sound_request(0x39);
                Setting_Default(hw);
            }
            break;
        case 7:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                if (flg == 0) {
                    sound_request(0x39);
                    pos = -1;
                }
            }
            break;
        }
        if (flg) flg = 0;
    } else {
        opt.thdflg &= ~1;
    }
}

void Setting_Default(HEADWORK *hw) {
    userinfo.glevel = 2;
    userinfo.roundtime = 2;
    userinfo.roundnum = 1;
    userinfo.quickconti = 0;
    userinfo.medal = 0;
    userinfo.ggmode[0] = 0;
    userinfo.ggmode[1] = 0;
    if (hw) {
        hw[0].pos = 2;
        hw[1].pos = userinfo.roundtime;
        hw[2].pos = userinfo.roundnum;
        hw[3].pos = userinfo.quickconti;
        hw[4].pos = userinfo.medal;
        hw[5].pos = 0;
    }
}

void Setting_Init(HEADWORK *hw, PARTS *pt) {
    int ret;
    hw[0].p = pt;
    hw[1].p = pt + 6;
    hw[2].p = pt + 10;
    hw[3].p = pt + 13;
    hw[4].p = pt + 13;
    hw[5].p = pt + 15;
    hw[0].pos = userinfo.glevel;
    hw[1].pos = userinfo.roundtime;
    hw[2].pos = userinfo.roundnum;
    hw[3].pos = userinfo.quickconti;
    hw[4].pos = userinfo.medal;
    ret = 0;
    if (userinfo.ggmode[0] > 0 && userinfo.ggmode[1] > 0) ret = 3;
    else if (userinfo.ggmode[0] > 0 && userinfo.ggmode[1] <= 0) ret = 1;
    else if (userinfo.ggmode[0] <= 0 && userinfo.ggmode[1] > 0) ret = 2;
    else if (userinfo.ggmode[0] == -1) ret = 4;
    else ret = 0;
    hw[5].pos = ret;
}

void Opt_KeyConfig(int sw)
{
    static int pos[2];
    static int btn;
    int i;
    int n;
    char *moji[2] = { "OFF", "ON" };
    HEADWORK hw[13] = {
        { "PUNCH", 64.0f, 136.0f, 0, 0, 0 },
        { "KICK", 64.0f, 160.0f, 0, 0, 0 },
        { "SLASH", 64.0f, 184.0f, 0, 0, 0 },
        { "HSLASH", 64.0f, 208.0f, 0, 0, 0 },
        { "RESPECT", 64.0f, 232.0f, 0, 0, 0 },
        { "MEMORY", 64.0f, 256.0f, 0, 0, 0 },
        { "  P AND K", 64.0f, 280.0f, 0, 0, 0 },
        { "  S AND HS", 64.0f, 304.0f, 0, 0, 0 },
        { "  P K S", 64.0f, 328.0f, 0, 0, 0 },
        { "  P K S HS", 64.0f, 352.0f, 0, 0, 0 },
        { "VIBRATE", 64.0f, 376.0f, 0, 0, 0 },
        { "DEFAULT", 64.0f, 400.0f, 0, 0, 0 },
        { "EXIT", 64.0f, 424.0f, 0, 0, 0 },
    };

    if (sw == 0) {
        pads[0].trg = 0;
        pads[1].trg = 0;
        btn = 0;
        pos[0] = pos[1] = 0;
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
    if (btn >= 0) {
        FontDrawDC("KEY CONFIG", 32.0f, 40.0f, 2.0f, 1, 0);
        for (i = 0; i < 2; i++) {
            FontDrawDC(i ? "2P" : "1P", i * 256 + 64, 96.0f, 2.0f, 1, 1);
            for (n = 0; n < 13; n++) {
                int alpha;
                if (n == pos[i]) alpha = 1;
                else if ((n == 6 && userinfo.key[i].pandk == 0) ||
                         (n == 7 && userinfo.key[i].sandhs == 0) ||
                         (n == 8 && userinfo.key[i].pks == 0) ||
                         (n == 9 && userinfo.key[i].pkshs == 0))
                    alpha = 0xc0;
                else
                    alpha = 0xa0;
                FontDrawDC(hw[n].head, hw[n].xp + (i * 256), hw[n].yp, 2.0f, alpha, 2);
            }
            FontDraw(moji[userinfo.key[i].vibrate], i * 256 + 256, 378, 2.0f, 0xff);
            if (pos[i] == 12 && (pads[i].trg & 0x20)) {
                sound_request(0x39);
                btn = -1;
                return;
            }
            if ((pads[0].trg | pads[1].trg) & 0x800) {
                sound_request(0x3b);
                btn = -1;
            }
            if (pads[i].data & 0x1000) {
                if (sc.cnt[i] == 0 || (sc.cnt[i] > 16 && !(sc.cnt[i] & 3))) {
                    sound_request(0x37);
                    pos[i] = pos[i] ? pos[i] - 1 : 12;
                }
                sc.cnt[i]++;
            } else if (pads[i].data & 0x4000) {
                if (sc.cnt[i] == 0 || (sc.cnt[i] > 16 && !(sc.cnt[i] & 3))) {
                    sound_request(0x37);
                    pos[i] = (pos[i] == 12) ? 0 : pos[i] + 1;
                }
                sc.cnt[i]++;
            } else {
                sc.cnt[i] = 0;
            }
            set_btn_execute(i, pos[i]);
            set_purupuru(i, pos[i]);
            set_default(i, pos[i]);
            disp_activebtn(i);
        }
    } else {
        opt.thdflg &= ~0x20;
        sc.grpvar[0] = 0;
    }
}
