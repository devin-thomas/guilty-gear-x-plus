#include "generated/opt_rec.h"

extern PAD pads[2];
extern OPTINFO opt;
void DiagramMakeSortTable(void);
int sound_request(int no);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
extern SYS_COND sc;
int GetLimitStatus(void);
void MarkDraw(char *str, int x, int y, int z, int alpha);
int iodat_chk(int stat);
void RankDrawSub(int idno, int x, int y);
void DispDiagram(unsigned char alfa, int x, int y);
extern USER_SETTING userinfo;
void FontDraw(char *str, int x, int y, float z, int alfa);
void time_convert(char *buf, u_int time, int id);
void disp_koron(int x, int y);
void *memcpy(void *dst, const void *src, unsigned int n);
void set_char2num(int num, char *block, int id);

static void result_draw_sectiontime(int id)
{
    int i;
    char buf[18];
    char *lvl[10] = { " 1 - 10", "10 - 20", "20 - 30", "30 - 40", "40 - 50", "50 - 60", "60 - 70", "70 - 80", "80 - 90", "90 -100" };

    for (i = 0; i < 5; i++) {
        FontDraw(lvl[i], 64, i * 32 + 272, 4.0f, 0xff);
        if (userinfo.svresult[id].section_time[i] == 0 && userinfo.svresult[id].level < i * 10 + 1) {
            memcpy(buf, "--------", 16);
        } else {
            time_convert(buf, userinfo.svresult[id].section_time[i], 1);
        }
        FontDraw(buf, 192, i * 32 + 272, 4.0f, 0xff);
        if (buf[0] != '-') disp_koron(192, i * 32 + 272);
    }
    for (i = 0; i < 5; i++) {
        FontDraw(lvl[i + 5], 320, i * 32 + 272, 4.0f, 0xff);
        if (userinfo.svresult[id].section_time[i + 5] == 0 && userinfo.svresult[id].level < (i + 5) * 10 + 1) {
            memcpy(buf, "--------", 16);
        } else {
            time_convert(buf, userinfo.svresult[id].section_time[i + 5], 1);
        }
        FontDraw(buf, 448, i * 32 + 272, 4.0f, 0xff);
        if (buf[0] != '-') disp_koron(448, i * 32 + 272);
    }
}

static void result_draw(int page)
{
    RRESULT_DATA rrd[18] = {
        { 1, "SOL BADGUY" }, { 2, "KY KISKE" }, { 3, "MAY" }, { 4, "MILLIA RAGE" },
        { 5, "AXL LOW" }, { 6, "POTEMKIN" }, { 7, "CHIPP ZANUFF" }, { 8, "ZATO-ONE" },
        { 9, "BAIKEN" }, { 10, "FAUST" }, { 12, "JAM KURADOBERI" }, { 13, "ANJI MITO" },
        { 14, "JOHNNY" }, { 15, "VENOM" }, { 11, "TESTAMENT" }, { 16, "DIZZY" },
        { 17, "KLIFF" }, { 18, "JUSTICE" },
    };
    char buf[18];

    FontDraw("PLAYER", 64, 144, 2.0f, 0xff);
    FontDraw(rrd[page].name, 192, 144, 2.0f, 0xff);
    FontDraw("LEVEL", 64, 176, 2.0f, 0xff);
    set_char2num(userinfo.svresult[rrd[page].id - 1].level, buf, 0);
    FontDraw(buf, 192, 176, 2.0f, 0xff);
    if (userinfo.svresult[rrd[page].id - 1].level >= 100) FontDraw("(ALL)", 240, 176, 2.0f, 0xff);
    FontDraw("STAGE", 64, 208, 2.0f, 0xff);
    set_char2num(userinfo.svresult[rrd[page].id - 1].stage, buf, 0);
    FontDraw(buf, 192, 208, 2.0f, 0xff);
    FontDraw("SECTIONTIME", 64, 240, 2.0f, 0xff);
    result_draw_sectiontime(rrd[page].id - 1);
    FontDraw("TOTALTIME", 64, 432, 2.0f, 0xff);
    if (userinfo.svresult[rrd[page].id - 1].total_time == 0 && userinfo.svresult[rrd[page].id - 1].level == 0) {
        memcpy(buf, "--------", 16);
    } else {
        time_convert(buf, userinfo.svresult[rrd[page].id - 1].total_time, 1);
    }
    FontDraw(buf, 192, 432, 4.0f, 0xff);
    if (buf[0] != '-') disp_koron(192, 432);
}

static void rank_normal(void)
{
    int i;
    int alpha;
    int page;
    int pad;
    int max;

    if (++rinfo.cnt & 7) alpha = 1; else alpha = 0xa0;
    if (GetLimitStatus() == 1) max = 10;
    else if (GetLimitStatus() == 2) max = 10;
    else if (GetLimitStatus() == 4) max = 11;
    else if (GetLimitStatus() == 5) max = 11;
    else max = 9;
    page = rinfo.rminfo[rinfo.mode].page;
    if (page < max - 1) {
        if (page == 0) {
            RankDrawSub(0, 0xc0, 0xc0);
        } else {
            for (i = 0; i < 2; i++) {
                if (GetLimitStatus() == 1 && i == 1 && page == max - 2) break;
                if (GetLimitStatus() == 4 && i == 1 && page == max - 2) break;
                RankDrawSub(page * 2 + i - 1, i * 288 + 48, 0xc0);
            }
            MarkDraw("4", 32, 432, 2, alpha);
        }
        MarkDraw("6", 610, 432, 2, alpha);
    } else {
        MarkDraw("4", 32, 432, 2, alpha);
        DispDiagram(0xff, 0x18, 0x80);
    }
    if ((pad = iodat_chk(0x8000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 7))) {
            sound_request(0x37);
            if (--page < 0) page = 0;
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x2000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 7))) {
            sound_request(0x37);
            if (++page >= max) page = max - 1;
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
    rinfo.rminfo[rinfo.mode].page = page;
}

static void rank_survival(void)
{
    int page;
    int max;
    int alpha;
    int pad;

    if (++rinfo.cnt & 7) alpha = 1; else alpha = 0xa0;
    if (GetLimitStatus() == 1) max = 15;
    else if (GetLimitStatus() == 2) max = 16;
    else if (GetLimitStatus() == 4) max = 17;
    else if (GetLimitStatus() == 5) max = 18;
    else max = 14;
    page = rinfo.rminfo[rinfo.mode].page;
    result_draw(page);
    if (page > 0) MarkDraw("4", 32, 432, 2, alpha);
    if (page < max - 1) MarkDraw("6", 610, 432, 2, alpha);
    if ((pad = iodat_chk(0x8000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 7))) {
            sound_request(0x37);
            if (--page < 0) page = 0;
        }
        sc.cnt[pad - 1]++;
    } else if ((pad = iodat_chk(0x2000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 7))) {
            sound_request(0x37);
            if (++page >= max) page = max - 1;
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
    rinfo.rminfo[rinfo.mode].page = page;
}

void Opt_UserRanking(int sw)
{
    void (*rankproc[2])() = { rank_normal, rank_survival };
    int i;
    int alpha;
    static HEADWORK hw[2] = {
        { "ARCADE", 64.0f, 96.0f, 0, 0, 0 },
        { "SURVIVAL", 224.0f, 96.0f, 0, 0, 0 },
    };

    if (sw == 0) {
        DiagramMakeSortTable();
rinfo.mode = 0; rinfo.cnt = 0; rinfo.rminfo[0].page = 0; rinfo.rminfo[1].page = 0;

        return;
    }
    if (!((pads[0].trg | pads[1].trg) & 0x840)) {
    FontDrawDC("RANKING", 32.0f, 40.0f, 4.0f, 1, 0);
    for (i = 0; i < 2; i++) {
        if (rinfo.mode == i) alpha = 1; else alpha = 0xa0;
        FontDrawDC(hw[i].head, hw[i].xp, hw[i].yp, 2.0f, alpha, 1);
    }
    if ((pads[0].trg | pads[1].trg) & 0xf) {
        sound_request(0x37);
        rinfo.mode ^= 1;
    }
    rankproc[rinfo.mode]();
    } else {
        sound_request(0x3b);
        opt.thdflg &= ~8;
    }
}
