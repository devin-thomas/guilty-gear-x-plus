#include "generated/nameentr.h"

static char getchardatabycode(int code)
{
    char table[44] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ,.0123456789!?&d>";

    return table[code];
}

int strcmp(const char *s1, const char *s2);
char *strcpy(char *dst, const char *src);

int NameEntryDecision(NAMEENTRYCONTROL *p, int side)
{
    char *strp;
    int i;

    strp = p->name;
    for (i = 0; i < 3; i++, strp++) {
        if (*strp == '>') {
            *strp = ' ';
        }
        if (*strp == 'd') {
            *strp = ' ';
        }
        if (*strp == 0) {
            *strp = ' ';
        }
    }
    p->name[3] = 0;
    p->name[4] = 0;
    if (!strcmp(p->name, "SEX") || !strcmp(p->name, "AAA") || !strcmp(p->name, "AA ")
        || !strcmp(p->name, "A  ") || !strcmp(p->name, "XXX") || !strcmp(p->name, "X X")
        || !strcmp(p->name, "XX ") || !strcmp(p->name, "   ")) {
        strcpy(p->name, "GGX");
    }
    p->cursor = -1;
    p->name[3] = ' ';
    p->time = 0;
    p->name[4] = 0;
    return 0;
}

extern PAD pads[2];
extern SYS_COND sc;
extern USER_SETTING userinfo;

int NameEntryExecute(NAMEENTRYCONTROL *p, int side)
{
    u_int trg;

    trg = pads[side].trg;
    p->time++;
    if (p->cursor < 0) {
        if (p->time > 60) {
            return 1;
        }
        return 0;
    }
    if (p->time > 1800) {
        return NameEntryDecision(p, side);
    }
    if ((trg & (userinfo.key[side].punch | userinfo.key[side].kick | userinfo.key[side].slash | userinfo.key[side].hslash)) && p->time > 60) {
        if (getchardatabycode(p->code) == 'd') {
            if (p->cursor == 0) {
                return 0;
            }
            p->name[p->cursor] = getchardatabycode(0);
            p->codetb[p->cursor] = 0;
            p->cursor = g_roundmin(p->cursor - 1, 0);
            p->code = p->codetb[p->cursor];
            p->name[p->cursor] = getchardatabycode(p->code);
            return 0;
        } else if (getchardatabycode(p->code) == '>') {
            return NameEntryDecision(p, side);
        } else {
            p->cursor++;
            if (p->cursor >= 3) {
                p->code = 43;
            }
            p->codetb[p->cursor] = p->code;
            p->name[p->cursor] = getchardatabycode(p->code);
        }
    } else if (pads[side].data & 0x8000) {
        if (sc.cnt[side] == 0 || (sc.cnt[side] > 16 && !(sc.cnt[side] & 3))) {
            p->code--;
            if (p->cursor >= 3) {
                if (p->code < 42) {
                    p->code = 43;
                }
            } else if (p->code < 0) {
                p->code = 43;
            }
        }
        sc.cnt[side]++;
    } else if (pads[side].data & 0x2000) {
        if (sc.cnt[side] == 0 || (sc.cnt[side] > 16 && !(sc.cnt[side] & 3))) {
            p->code++;
            if (p->cursor >= 3) {
                if (p->code >= 44) {
                    p->code = 42;
                }
            } else if (p->code >= 44) {
                p->code = 0;
            }
        }
        sc.cnt[side]++;
    } else {
        sc.cnt[side] = 0;
    }
    p->codetb[p->cursor] = p->code;
    p->name[p->cursor] = getchardatabycode(p->code);
    return 0;
}

void NameEntryInit(NAMEENTRYCONTROL *p) {
    p->name[0] = 0x20;
    p->name[1] = 0x20;
    p->name[2] = 0x20;
    p->name[3] = 0x20;
    p->name[4] = 0;
    p->codetb[0] = 0;
    p->codetb[1] = 0;
    p->codetb[2] = 0;
    p->codetb[3] = 0;
    p->cursor = 0;
    p->code = 1;
    p->time = 0;
}
