#include "generated/skillref.h"

static int inst_ypos[18] = { 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70 };
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
extern short int UseCharacter1p2p[];
static SKILLREF sr;
extern USER_SETTING userinfo;
void FontDraw(char *str, int x, int y, float z, int alfa);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
char *G_sprintf(char *buff, char *str, int *valtb);
void SkillDraw(int no, int side, int x, int y, int z, int alpha);
void CommandDraw(char *str, int x, int y, int z, int alpha);
extern DCFONT_INFO fi;
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;
extern SYS_COND sc;
extern PAD pads[2];
int sound_request(int no);

void InstDisp(int sw)
{
    static int id;
    static int side;
    static int max;

    if (sw == 0) {
        if (g_info.mode & 0x100) id = UseCharacter1p2p[tr_info->ply];
        else id = UseCharacter1p2p[sc.grpvar[0] - 1];
        if (g_info.mode & 0x100) {
            side = tr_info->ply;
        } else {
            side = sc.grpvar[0] - 1;
        }
        sr.now = side;
        sr.p1id = UseCharacter1p2p[0];
        sr.p2id = UseCharacter1p2p[1];
        sr.pos = 0;
        return;
    }
#ifdef VERSION_KR
    if ((userinfo.gguse[sr.now] & 2) && UseCharacter1p2p[sr.now] == 2)
#else
    if ((userinfo.gguse[sr.now] & 2) && id == 2)
#endif
    {
        max = 10;
    } else {
        if (sr.now == 0) max = char_command_num[sr.p1id - 1];
        else max = char_command_num[sr.p2id - 1];
    }
    if (sr.now == 0) {
        skillref_disp(sr.p1id);
    } else {
        skillref_disp(sr.p2id);
    }
    if (pads[side].data & 0x1000) {
        if (sc.cnt[side] == 0 || (sc.cnt[side] > 16 && !(sc.cnt[side] & 3))) {
            if (sr.pos > 0) {
                sound_request(0x37);
                sr.pos--;
            } else {
                sr.pos = 0;
            }
        }
        sc.cnt[side]++;
    } else if (pads[side].data & 0x4000) {
        if (sc.cnt[side] == 0 || (sc.cnt[side] > 16 && !(sc.cnt[side] & 3))) {
            if (sr.pos < max - 5) {
                sound_request(0x37);
                sr.pos++;
            } else {
                sr.pos = max - 5;
            }
        }
        sc.cnt[side]++;
    } else if ((pads[0].trg | pads[1].trg) & 0xa000) {
        sound_request(0x37);
        if (sr.now == 0) {
            sr.now = 1;
        } else {
            sr.now = 0;
        }
        sr.pos = 0;
    } else {
        sc.cnt[side] = 0;
    }
    if ((pads[0].trg | pads[1].trg) & 0x40) {
        sound_request(0x3b);
        sc.grpvar[0] = 0;
    }
}


void skillref_disp(int id)
{
    int i;
    int j;
    int side;
    int temp;
    char command_buf[32];
    char *p;
    char *p2;
    void *adrs[18] = {
        command_sl, command_ky, command_my, command_ml, command_ax, command_po,
        command_ch, command_zt, command_bk, command_fa, command_ts, command_jm,
        command_an, command_jy, command_ve, command_dz, command_kr, command_js,
    };
    char *name[18] = {
        "SOL BADGUY", "KY KISKE", "MAY", "MILLIA RAGE", "AXL LOW", "POTEMKIN",
        "CHIPP ZANUFF", "ZATO ONE", "BAIKEN", "FAUST", "TESTAMENT", "JAM KURADOBERI",
        "ANJI MITO", "JOHNNY", "VENOM", "DIZZY", "KLIFF", "JUSTICE",
    };
    int val[2];

    side = sr.now != 0;
#ifdef VERSION_KR
    if ((userinfo.gguse[side] & 2) && UseCharacter1p2p[side] == 2)
#else
    if ((userinfo.gguse[side] & 2) && id == 2)
#endif
    {
        FontDrawDC("ROBO KY", 96.0f, inst_ypos[id - 1] - 24, 1.0f, 1, 1);
        p = (char *)command_ggky;
    } else {
        FontDrawDC(name[id - 1], 96.0f, inst_ypos[id - 1] - 24, 1.0f, 1, 1);
        p = adrs[id - 1];
    }
    for (i = 0; i < 5; i++) {
        p2 = p + (i + sr.pos) * 32;
        for (j = 1; p2[j] != -1; j++) {
            command_buf[j - 1] = p2[j] + '0';
        }
        command_buf[j - 1] = 0;
        val[0] = i + sr.pos + 1;
        FontDraw(G_sprintf(0, "%2d", val), 0x60, inst_ypos[id - 1] + i * 64 + 12, 1.0f, 0xff);
        if ((userinfo.gguse[side] & 2) && id == 2) {
            switch (i + sr.pos) {
            case 2: temp = 11; break;
            case 5: temp = 9; break;
            case 6: temp = 10; break;
            case 8: temp = 12; break;
            case 9: temp = 8; break;
            default: temp = i + sr.pos; break;
            }
        } else {
            temp = i + sr.pos;
        }
        SkillDraw(temp, side, 0x80, inst_ypos[id - 1] + i * 64, 1, 0x40);
        CommandDraw(command_buf, 0x60, inst_ypos[id - 1] + (i * 64 + 0x20), 1, 1);
        command_waku_disp(id, p2[0], i * 2);
    }
}

static void command_waku_disp(int id, int type, int cnt) {
    int col;
    int col2;
    if (type == 0) {
        col = 0x800000c0;
        col2 = 0x80000040;
    } else if (type == 1) {
        col = 0x8000c000;
        col2 = 0x80004000;
    } else if (type == 2) {
        col = 0x80c00000;
        col2 = 0x80400000;
    } else {
        return;
    }
    polydraw_FBox(96.0f, inst_ypos[id - 1] + cnt * 32, 560.0f, inst_ypos[id - 1] + cnt * 32 + 32, 2.0f, col);
    polydraw_FBox(96.0f, inst_ypos[id - 1] + (cnt + 1) * 32, 560.0f, inst_ypos[id - 1] + (cnt + 1) * 32 + 32, 2.0f, col2);
}

extern DCFONT_INFO fi;
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void CommandDraw(char *str, int x, int y, int z, int alpha)
{
    int u;
    int v;
    float off;
    float len;
    NL_SPR_ARG sprite;

    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.zm_x = 0.125f;
    sprite.zm_y = 0.125f;
    sprite.ang = 0;
    while (*str) {
        len = 32.0f;
        sprite.zm_x = len * 0.00390625;
        off = len;
        if (*str == '0') {
            u = 4;
            v = 0;
        } else if (*str == '1') {
            u = 4;
            v = 1;
        } else if (*str == '2') {
            u = 4;
            v = 2;
        } else if (*str == '3') {
            u = 4;
            v = 3;
        } else if (*str == '4') {
            u = 3;
            v = 1;
        } else if (*str == '5') {
            u = 2;
            v = 1;
        } else if (*str == '6') {
            u = 3;
            v = 0;
        } else if (*str == '7') {
            u = 2;
            v = 0;
        } else if (*str == '8') {
            u = 3;
            v = 2;
        } else if (*str == '9') {
            u = 2;
            v = 2;
        } else if (*str == ':') {
            u = 1;
            v = 1;
        } else if (*str == ';') {
            u = 0;
            v = 0;
        } else if (*str == '<') {
            u = 1;
            v = 0;
        } else if (*str == '=') {
            u = 1;
            v = 2;
        } else if (*str == '>') {
            u = 0;
            v = 1;
        } else if (*str == '?') {
            u = 1;
            v = 3;
        } else if (*str == '@') {
            u = 0;
            v = 2;
        } else if (*str == 'A') {
            u = 0;
            v = 3;
        } else if (*str == 'B') {
            u = 0;
            off = 24.0f;
            v = 4;
        } else if (*str == 'C') {
            u = 1;
            v = 4;
        } else if (*str == 'D') {
            len = 128.0f;
            u = 1;
            off = 112.0f;
            v = 4;
            sprite.zm_x = len * 0.00390625;
        } else if (*str == 'E') {
            len = 64.0f;
            u = 0;
            off = 48.0f;
            v = 5;
            sprite.zm_x = len * 0.00390625;
        } else if (*str == 'F') {
            len = 64.0f;
            u = 1;
            off = 48.0f;
            v = 5;
            sprite.zm_x = len * 0.00390625;
        } else if (*str == 'G') {
            len = 128.0f;
            u = 1;
            off = 96.0f;
            v = 5;
            sprite.zm_x = len * 0.00390625;
        } else if (*str == 'H') {
            len = 64.0f;
            u = 0;
            v = 6;
            sprite.zm_x = len * 0.00390625;
            off = len;
        } else if (*str == 'I') {
            len = 64.0f;
            u = 1;
            v = 6;
            sprite.zm_x = len * 0.00390625;
            off = len;
        } else if (*str == 'J') {
            u = 4;
            off = 16.0f;
            v = 6;
        } else if (*str == 'K') {
            len = 64.0f;
            u = 3;
            off = 40.0f;
            v = 6;
            sprite.zm_x = len * 0.00390625;
        } else if (*str == 'L') {
            len = 128.0f;
            u = 0;
            v = 7;
            off = 120.0f;
            sprite.zm_x = len * 0.00390625;
        } else if (*str == 'M') {
            len = 128.0f;
            u = 1;
            off = 96.0f;
            v = 7;
            sprite.zm_x = len * 0.00390625;
        } else {
            goto next;
        }
        u = u * len;
        sprite.sprno = fi.font[3].spno;
        v = v * 32.0;
        sprite.u0 = u * 0.00390625;
        sprite.v0 = v * 0.00390625;
        sprite.u1 = (u + len) * 0.00390625;
        sprite.v1 = (v + 32.0) * 0.00390625;
        gSpriteDraw_NaomiLibVersion(&sprite);
next:
        sprite.x += off;
        str++;
    }
}

void SkillDraw(int no, int side, int x, int y, int z, int alpha)
{
    int u;
    int v;
    float len;
    float off;
    NL_SPR_ARG sprite;

    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.zm_x = 0.0625f;
    sprite.zm_y = 0.0625f;
    sprite.ang = 0;
    len = 512.0f;
    off = 0.0f;
    sprite.zm_x = len * 0.001953125;
    u = 0;
    v = no;
    u = u * len;
    sprite.sprno = fi.font[side + 5].spno;
    v = v * 32.0;
    sprite.u0 = u * 0.001953125;
    sprite.v0 = v * 0.001953125;
    sprite.u1 = (u + len) * 0.001953125;
    sprite.v1 = (v + 32.0) * 0.001953125;
    gSpriteDraw_NaomiLibVersion(&sprite);
    sprite.x += off;
}
