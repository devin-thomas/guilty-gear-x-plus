#include "generated/opt_vm.h"

/* the memory card system directory is region-specific (memory.c has the same) */
#ifdef VERSION_KR
#define MC_SYSDIR "BKSLPM-64525GGX"
#else
#define MC_SYSDIR "BISLPS-20137GGX"
#endif

extern USER_SETTING userinfo;
static VM_INFO vinfo;
void FontDraw(char *str, int x, int y, float z, int alfa);
extern PAD pads[2];
extern SYS_COND sc;
int strlen(char *s);
int sound_request(int no);
int iodat_chk(int stat);
int MemoryCheck(void);
unsigned int gStripGetFadeColor(unsigned int color);
void slot_display(int slot);
void vm_error(int no, int flg, int slot);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
int vm_get_freeblock(int port);
void MemoryDir(int slot, char *fname, int *p);
void MemoryClose(void);
void MemorySave(int slot);
void MemoryFormatAndSave(int slot);
void debugf(char *format, ...);
void MemoryOpen(void);
int SetLimitStatus(void);
extern char D_5C2BB8[];
/* RODATA_EXTERN 0x5c2bb8: "MEMORY CARD" */
extern char D_5C2BC8[];
/* RODATA_EXTERN 0x5c2bc8: "MEMORY CARD SLOT 1 ONLY" */
extern char D_5CBB60[];
/* RODATA_EXTERN 0x5cbb60: "(PS2)" (.sdata) */
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
extern OPTINFO opt;
typedef struct { PARTS p[3]; } PT3;
extern PT3 D_5C2B88;
/* RODATA_EXTERN 0x5c2b88: PARTS pt[3] = { {"OFF",288,10,40}, {"ON",352,10,32}, {0,-16,64,72} } */
void MemorySetSystemDir(char *data, int size);

void Opt_MemAccess(int sw)
{
    static int ret;
    static int subthd;
    int pad;
    static HEADWORK hw[8] = {
        { "DATA SAVE", 64.0f, 112.0f, 0, 0, 0 },
        { "DATA LOAD", 352.0f, 112.0f, 0, 0, 0 },
        { "AUTO SAVE", 64.0f, 352.0f, 0, 2, 0 },
        { "EXIT", 64.0f, 400.0f, 0, 0, 0 },
        { "MEMORY CARD SLOT1", 64.0f, 160.0f, 0, 1, 0 },
        { "MEMORY CARD SLOT2", 64.0f, 256.0f, 0, 1, 0 },
    };
    PT3 ptw = D_5C2B88;

    if (sw == 0) {
        MemorySetSystemDir((char *)&userinfo, 0x1614);
        MemoryOpen();
        MemAccess_Init(hw, ptw.p);
        ret = 0;
        subthd = 0;
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
        return;
    }
    if (vinfo.pos != -2 && !((pads[0].trg | pads[1].trg) & 0x800)) {
        vm_disp(hw);
        disp_freeblock(hw, ptw.p);
        if (vinfo.slot < 0) {
        if ((pad = iodat_chk(0x1000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                vinfo.pos = vinfo.pos ? vinfo.pos - 1 : 3;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                vinfo.pos = (vinfo.pos == 3) ? 0 : vinfo.pos + 1;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x8000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (hw[vinfo.pos].p) {
                    if (hw[vinfo.pos].pos) hw[vinfo.pos].pos--;
                    else hw[vinfo.pos].pos = hw[vinfo.pos].maxhead - 1;
                } else {
                    vinfo.pos = vinfo.pos ? vinfo.pos - 1 : 3;
                }
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x2000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (hw[vinfo.pos].p) {
                    if (hw[vinfo.pos].pos >= hw[vinfo.pos].maxhead - 1) hw[vinfo.pos].pos = 0;
                    else hw[vinfo.pos].pos++;
                } else {
                    vinfo.pos = (vinfo.pos == 3) ? 0 : vinfo.pos + 1;
                }
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        switch (vinfo.pos) {
        case 0:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                sound_request(0x39);
                vinfo.mode = 0;
                vinfo.slot = 0;
                slot_execute(0);
            }
            break;
        case 1:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                sound_request(0x39);
                vinfo.mode = 1;
                vinfo.slot = 0;
                slot_execute(0);
            }
            break;
        case 2:
            userinfo.autosave = hw[2].pos;
            break;
        case 3:
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                sound_request(0x39);
                vinfo.pos = -2;
            }
            break;
        }
        if ((pads[0].trg | pads[1].trg) & 0x40) {
            sound_request(0x3b);
            vinfo.pos = -2;
        }
        } else {
            slot_execute(1);
            hw[2].pos = userinfo.autosave;
        }
    } else if (MemoryCheck() > 0) {
        vm_disp(hw);
        disp_freeblock(hw, ptw.p);
        slot_execute(1);
    } else {
        if ((pads[0].trg | pads[1].trg) & 0x840) sound_request(0x3b);
        opt.thdflg &= ~4;
        MemoryClose();
    }
}

static void slot_execute(int sw)
{
    static int mode;
    int pad;

    if (sw == 0) {
        mode = 0;
        return;
    }
    if (mode == 0) {
        if ((pad = iodat_chk(0x1000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (--vinfo.slot < 0) vinfo.slot = 1;
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x4000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(0x37);
                if (++vinfo.slot > 1) vinfo.slot = 0;
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
        if ((pads[0].trg | pads[1].trg) & 0x40) {
            sound_request(0x3b);
            vinfo.slot = -1;
        } else if ((pads[0].trg | pads[1].trg) & 0x20) {
            sound_request(0x39);
            if (vinfo.mode == 0) {
                MemAccess_Save(0, vinfo.slot);
                mode = 1;
            } else if (vinfo.mode == 1) {
                MemoryClose();
                MemAccess_Load(0, vinfo.slot);
                mode = 2;
            }
        }
    } else if (mode == 1) {
        if (MemAccess_Save(1, vinfo.slot)) {
            MemoryOpen();
            mode = 0;
        }
    } else if (mode == 2) {
        if (MemAccess_Load(1, vinfo.slot)) {
            SetLimitStatus();
            MemoryOpen();
            mode = 0;
        }
    }
}

static void vm_disp(HEADWORK *hw)
{
    int i;
    int n;
    int alpha;

    FontDrawDC(D_5C2BB8, 32.0f, 40.0f, 4.0f, 1, 0);
    FontDraw(D_5CBB60, 384, 55, 4.0f, 0xff);
    for (n = 0; n < 4; n++) {
        if (n == vinfo.pos) alpha = 1; else alpha = 0xa0;
        FontDrawDC(hw[n].head, hw[n].xp, hw[n].yp, 4.0f, alpha, 1);
        if (hw[n].p != 0) {
            for (i = 0; i < hw[n].maxhead; i++) {
                if (hw[n].p[i].head) {
                    if (hw[n].pos == i) alpha = 0xff; else alpha = 0x50;
                    FontDraw(hw[n].p[i].head, hw[n].xp + hw[n].p[i].xp, hw[n].yp + hw[n].p[i].yp, 4.0f, alpha);
                }
            }
        }
    }
    FontDraw(D_5C2BC8, 264, 384, 4.0f, 0xff);
    for (n = 0; n < 2; n++) { int alpha; if (vinfo.slot == n) alpha = 1; else alpha = 0xa0; FontDrawDC(hw[n + 4].head, hw[n + 4].xp + 32.0f, hw[n + 4].yp, 4.0f, alpha, 1); }
}

static void MemAccess_Init(HEADWORK *hw, PARTS *pt)
{
    hw[2].p = pt;
    hw[4].p = &pt[2];
    hw[2].pos = userinfo.autosave;
vinfo.mode = -1; vinfo.slot = -1; vinfo.errcode = 0; vinfo.pos = 0; vinfo.wait = 0;

}

void FontDraw(char *str, int x, int y, float z, int alfa);
int vm_get_freeblock(int port);
char *G_sprintf(char *buff, char *str, int *valtb);

static void disp_freeblock(HEADWORK *hw, PARTS *pt)
{
    int x;
    int y;
    int i;
    int xoff;
    int size;
    int valtb[2];

    for (i = 0; i < 2; i++) {
        size = vm_get_freeblock(i);
        xoff = 42;
        x = hw[4].p->xp + hw[4 + i].xp;
        y = hw[4].p->yp + hw[4 + i].yp + 56.0f;
        if (size >= 0) {
            valtb[0] = size;
            if (size >= 100) {
                xoff = 46;
            }
            FontDraw(G_sprintf(0, "%3dKB FREE", valtb), x + xoff, y - 72, 4.0f, 0xff);
        } else if (size == -1) {
            FontDraw("UNFORMAT", x + 46, y - 72, 4.0f, 0xff);
        } else if (size == -2) {
            FontDraw("ILLEGAL", x + 46, y - 72, 4.0f, 0xff);
        } else {
            FontDraw("NO CARD", x + 46, y - 72, 4.0f, 0xff);
        }
    }
}

static int MessageSet(char *s1, char *s2, int *cursor)
{
    int pad;

    FontDraw(s1, 320 - strlen(s1) * 6, 240, 2.0f, 0xff);
    FontDraw(s2, 320 - strlen(s2) * 6, 260, 2.0f, 0xff);
    if (*cursor == 0) {
        FontDraw("NO     ", 278, 288, 2.0f, 0xff);
        FontDraw("     YES", 272, 288, 2.0f, 0x50);
    } else {
        FontDraw("NO     ", 278, 288, 2.0f, 0x50);
        FontDraw("     YES", 272, 288, 2.0f, 0xff);
    }
    if ((pads[0].trg | pads[1].trg) & 0x20) {
        sound_request(0x39);
        return 1;
    }
    if ((pad = iodat_chk(0xa000))) {
        if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
            *cursor ^= 1;
        }
        sc.cnt[pad - 1]++;
    } else {
        sc.cnt[0] = 0;
        sc.cnt[1] = 0;
    }
    return 0;
}

static int MemAccess_Save(int sw, int slot)
{
    static int time;
    static int cursor;
    static int accmode;
    static int ret;
    static int err;

    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 3.0f, gStripGetFadeColor(0xe0000000));
    if (sw == 0) {
        accmode = 0;
        time = 0;
        cursor = 0;
        return 0;
    }
    switch (accmode) {
    case 0:
        MemoryDir(slot, MC_SYSDIR, &ret);
        debugf("MemoryDir Start %s\n", MC_SYSDIR);
        accmode++;
        break;
    case 1:
        err = MemoryCheck();
        if (err > 0) break;
        if (err == 0) {
            if (ret) accmode = 3; else accmode = 2;
        } else if (err == -4) {
            accmode = 4;
        } else {
            accmode = 6;
        }
        break;
    case 2:
        if (vm_get_freeblock(slot) < 0) return 1;
        slot_display(slot);
        if (!MessageSet("", "SAVE NOW?", &cursor)) return 0;
        MemoryClose();
        if (!cursor) return 1;
        MemorySave(slot);
        accmode = 5;
        break;
    case 3:
        if (vm_get_freeblock(slot) < 0) return 1;
        slot_display(slot);
        if (!MessageSet("FILE EXIST", "OVERWRITE?", &cursor)) return 0;
        MemoryClose();
        if (!cursor) return 1;
        MemorySave(slot);
        accmode = 5;
        break;
    case 4:
        if (vm_get_freeblock(slot) != -1) return 1;
        slot_display(slot);
        if (!MessageSet("MEMORY CARD (PS2) IS NOT FORMATTED", "FORMAT NOW?", &cursor)) return 0;
        MemoryClose();
        if (!cursor) return 1;
        MemoryFormatAndSave(slot);
        accmode = 5;
        break;
    case 5:
        err = MemoryCheck();
        if (err > 0) {
            slot_display(slot);
            FontDraw("NOW SAVING...", 242, 236, 2.0f, 0xff);
            FontDraw("PLEASE DO NOT REMOVE THE MEMORY CARD (PS2)", 68, 272, 2.0f, 0xff);
            return 0;
        }
        accmode = 6;
    case 6:
        if (err == 0) {
            slot_display(slot);
            FontDraw("SAVE COMPLETED", 236, 236, 2.0f, 0xff);
        } else if (err == -8) {
            vm_error(-9, 2, slot);
        } else {
            vm_error(err, 2, slot);
            if (err == -5) FontDraw("73KB IS NEEDED", 236, 276, 2.0f, 0xff);
        }
        if (++time > 180) return 1;
        if ((pads[0].trg | pads[1].trg) & 0x20) return 1;
        break;
    }
    return 0;
}

void MemoryLoad(int slot);

static int MemAccess_Load(int sw, int slot)
{
    static int time;
    static int cur;
    int err;

    if (sw == 0) {
        MemoryLoad(slot);
        time = -100;
        cur = 0;
    } else {
        if (time == -100) {
            if (MemoryCheck() > 0) return 0;
            time = 180;
        }
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 3.0f, gStripGetFadeColor(0xe0000000));
        err = MemoryCheck();
        if (err > 0) {
            slot_display(slot);
            FontDraw("NOW LOADING", 254, 236, 2.0f, 0xff);
            return 0;
        }
        if (err == 0) {
            slot_display(slot);
            FontDraw("LOAD COMPLETED", 236, 236, 2.0f, 0xff);
            if (time-- <= 0) return 1;
            if ((pads[0].trg | pads[1].trg) & 0x20) return 1;
        } else if (err == -1) {
            slot_display(slot);
            FontDraw("LOAD ERROR!!", 248, 240, 2.0f, 0xff);
            FontDraw("DATA IS CORRUPTED", 218, 256, 2.0f, 0xff);
            if (time-- <= 0) return 1;
            if ((pads[0].trg | pads[1].trg) & 0x20) return 1;
        } else {
            vm_error(err, 2, slot);
            if (time-- <= 0) return 1;
            if ((pads[0].trg | pads[1].trg) & 0x20) return 1;
        }
    }
    return 0;
}
