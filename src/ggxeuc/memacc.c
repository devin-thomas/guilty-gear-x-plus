#include "generated/memacc.h"
static const short idtb[] = { 1, 2, 3 };
void FontDraw(char *str, int x, int y, float z, int alfa);
int strlen(const char *s);
int sound_request(int no);
extern PAD pads[2];
int AutoSaveMode = 0;
int AutoSaveCursor = 0;
int AutoSaveFormat = 0;
int AutoLoadMode = 0;
int AutoLoadCursor = 0;
static int type_buf;
void slot_display(int slot);
void slot_display_ex(int slot, char *str, int size);
void vm_error(int no, int flg, int slot);
int save_err_mes(char *str);
int save_format_mes(char *str);
int save_err_mes_ex(char *str);
int load_err_mes_ex(char *str);
void AutoLoadInit(void);
int load_err_mes(char *str);

void FontDraw(char *str, int x, int y, float z, int alfa);

void slot_display(int slot)
{
    if (slot == 0) {
        FontDraw("MEMORY CARD SLOT 1", 212, 216, 2.0f, 255);
    } else {
        FontDraw("MEMORY CARD SLOT 2", 212, 216, 2.0f, 255);
    }
}

void FontDraw(char *str, int x, int y, float z, int alfa);
int strlen(const char *s);

void slot_display_ex(int slot, char *str, int size)
{
    if (slot == 0) {
        FontDraw("MEMORY CARD SLOT 1", 212, 124, 2.0f, 255);
        {
            int w = 6;
            int cx = 320;
            FontDraw(str, cx - strlen(str) * w, 144, 2.0f, 255);
        }
        if (size == 1) {
            FontDraw("73KB IS NEEDED", 236, 164, 2.0f, 255);
        }
    } else {
        FontDraw("MEMORY CARD SLOT 2", 212, 192, 2.0f, 255);
        {
            int w = 6;
            int cx = 320;
            FontDraw(str, cx - strlen(str) * w, 212, 2.0f, 255);
        }
        if (size == 1) {
            FontDraw("73KB IS NEEDED", 236, 232, 2.0f, 255);
        }
    }
}

void FontDraw(char *str, int x, int y, float z, int alfa);
int strlen(const char *s);

void vm_error(int no, int flg, int slot)
{
    char *error_mes[9] = {
        "FATAL ERROR!!",
        "MEMORY CARD (PS2) NOT INSERTED!!",
        "ILLEGAL MEMORY CARD (PS2)!!",
        "MEMORY CARD (PS2) IS NOT FORMATTED!!",
        "SHORT OF MEMORY!!",
        "FILE NOT FOUND",
        "FORMAT ERROR!!",
        "LOAD ERROR!!",
        "SAVE ERROR!!",
    };

    slot_display(slot);
    FontDraw(error_mes[-no - 1], 320 - strlen(error_mes[-no - 1]) * 6, 236, 2.0f, 255);
}

int save_err_mes(char *str)
{
    FontDraw(str, 320 - strlen(str) * 6, 240, 2.0f, 255);
    FontDraw("SKIP AUTOSAVE?", 236, 260, 2.0f, 255);
    if (AutoSaveCursor == 0) {
        FontDraw("NO     ", 278, 288, 2.0f, 255);
        FontDraw("     YES", 272, 288, 2.0f, 80);
    } else {
        FontDraw("NO     ", 278, 288, 2.0f, 80);
        FontDraw("     YES", 272, 288, 2.0f, 255);
    }
    if ((pads[0].trg | pads[1].trg) & 0x20) {
        sound_request(0x39);
        if (AutoSaveCursor == 1) {
            return 1;
        }
        if (AutoSaveFormat) {
            AutoSaveFormat = 0;
            return 0;
        }
    }
    if ((pads[0].trg | pads[1].trg) & 0xA000) {
        sound_request(0x37);
        AutoSaveCursor ^= 1;
    }
    return 0;
}

int save_format_mes(char *str)
{
    FontDraw(str, 320 - strlen(str) * 6, 240, 2.0f, 255);
    FontDraw("FORMAT NOW?", 254, 260, 2.0f, 255);
    if (AutoSaveCursor == 0) {
        FontDraw("NO     ", 278, 288, 2.0f, 255);
        FontDraw("     YES", 272, 288, 2.0f, 80);
    } else {
        FontDraw("NO     ", 278, 288, 2.0f, 80);
        FontDraw("     YES", 272, 288, 2.0f, 255);
    }
    if ((pads[0].trg | pads[1].trg) & 0x20) {
        sound_request(0x39);
        if (AutoSaveCursor == 1) {
            return 1;
        }
        if (AutoSaveFormat == 0) {
            AutoSaveFormat = 1;
            return 0;
        }
    }
    if ((pads[0].trg | pads[1].trg) & 0xA000) {
        sound_request(0x37);
        AutoSaveCursor ^= 1;
    }
    return 0;
}

int save_err_mes_ex(char *str)
{
    int trg;

    FontDraw(str, 320 - strlen(str) * 6, 240, 2.0f, 255);
    FontDraw("SKIP AUTOSAVE?", 236, 260, 2.0f, 255);
    if (AutoSaveCursor == 0) {
        FontDraw("NO     ", 278, 288, 2.0f, 255);
        FontDraw("     YES", 272, 288, 2.0f, 80);
    } else {
        FontDraw("NO     ", 278, 288, 2.0f, 80);
        FontDraw("     YES", 272, 288, 2.0f, 255);
    }
    trg = pads[0].trg | pads[1].trg;
    if (trg & 0x20) {
        sound_request(0x39);
        return AutoSaveCursor == 1;
    }
    if (trg & 0xA000) {
        sound_request(0x37);
        AutoSaveCursor ^= 1;
    }
    return -1;
}

int load_err_mes_ex(char *str)
{
    int trg;

    FontDraw(str, 320 - strlen(str) * 6, 240, 2.0f, 255);
    FontDraw("SKIP AUTOLOAD?", 236, 260, 2.0f, 255);
    if (AutoLoadCursor == 0) {
        FontDraw("NO     ", 278, 288, 2.0f, 255);
        FontDraw("     YES", 272, 288, 2.0f, 80);
    } else {
        FontDraw("NO     ", 278, 288, 2.0f, 80);
        FontDraw("     YES", 272, 288, 2.0f, 255);
    }
    trg = pads[0].trg | pads[1].trg;
    if (trg & 0x20) {
        sound_request(0x39);
        return AutoLoadCursor == 1;
    }
    if (trg & 0xA000) {
        sound_request(0x37);
        AutoLoadCursor ^= 1;
    }
    return -1;
}

int MemoryCheck(void);
void MemoryClose(void);
void MemoryFormatAndSave(int slot);
void MemoryOpen(void);
void MemorySave(int slot);
void MemorySetSystemDir(char *data, int size);
void MemorySlotClear(int slot);
void MemoryType(int slot, int *type, int *free, int *exist);
extern SYS_COND sc;
extern USER_SETTING userinfo;

int MemAccess_AutoSave(int mode)
{
    int result;
    int type;
    int free;
    int exist;
    static int sw;
    static int time;

    if (mode) {
        if (AutoSaveMode == 0) {
            MemoryType(0, &type, &free, &exist);
            if (type != type_buf) {
                AutoSaveMode = 0;
                AutoSaveCursor = 0;
                AutoSaveFormat = 0;
            }
            type_buf = type;
            if (type == 0) {
                AutoSaveMode = 0;
                AutoSaveCursor = 0;
                AutoSaveFormat = 0;
                return 0;
            }
            if (type == 4) {
                if (exist == 0) {
                    if (free < 71) {
                        slot_display(0);
                        FontDraw("73KB IS NEEDED", 236, 316, 2.0f, 255);
                        if (save_err_mes("SHORT OF MEMORY")) {
                            MemoryClose();
                            sc.grpvar[1] = 1;
                            return 1;
                        }
                        return 0;
                    }
                    AutoSaveMode = 1;
                    return 0;
                } else if (exist == 1) {
                    AutoSaveMode = exist;
                    return 0;
                }
                return 0;
            } else if (type == 1) {
                slot_display(0);
                if (save_err_mes("NO MEMORY CARD (PS2)")) {
                    MemoryClose();
                    sc.grpvar[1] = 1;
                    return 1;
                }
                return 0;
            } else if (type == 3) {
                slot_display(0);
                if (AutoSaveFormat) {
                    if (save_err_mes("UNFORMATTED MEMORY CARD (PS2)")) {
                        MemoryClose();
                        sc.grpvar[1] = 1;
                        return 1;
                    }
                    return 0;
                }
                if (save_format_mes("UNFORMATTED MEMORY CARD (PS2)")) {
                    AutoSaveMode = 2;
                    return 0;
                }
                return 0;
            } else {
                slot_display(0);
                if (save_err_mes("ILLEGAL MEMORY CARD (PS2)")) {
                    MemoryClose();
                    sc.grpvar[1] = 1;
                    return 1;
                }
                return 0;
            }
        } else if (AutoSaveMode == 1) {
            MemoryClose();
            MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
            MemorySave(0);
            sw = 1;
            AutoSaveMode = 3;
            time = 0;
            AutoSaveCursor = 0;
            return 0;
        } else if (AutoSaveMode == 2) {
            MemoryClose();
            MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
            MemoryFormatAndSave(0);
            sw = 1;
            AutoSaveMode = 3;
            time = 0;
            AutoSaveCursor = 0;
            return 0;
        } else {
            if (sw > 0) {
                sw = MemoryCheck();
                slot_display(0);
                FontDraw("NOW SAVING...", 242, 236, 2.0f, 255);
                FontDraw("PLEASE DO NOT REMOVE THE MEMORY CARD (PS2)", 68, 272, 2.0f, 255);
                return 0;
            }
            if (time < 180) {
                time++;
                if (sw == 0) {
                    slot_display(0);
                    FontDraw("SAVE COMPLETED", 236, 236, 2.0f, 255);
                    return 0;
                } else if (sw == -8) {
                    vm_error(-9, 2, 0);
                    return 0;
                } else {
                    vm_error(sw, 2, 0);
                    if (sw == -5) {
                        FontDraw("73KB IS NEEDED", 236, 276, 2.0f, 255);
                    }
                    return 0;
                }
            }
            if (sw == 0) {
                sc.grpvar[1] = 1;
                return 1;
            }
            slot_display(0);
            result = save_err_mes_ex("SAVE FAILED");
            if (result == 1) {
                sc.grpvar[1] = result;
                return 1;
            }
            if (result == 0) {
            MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
            MemorySlotClear(0);
            MemorySlotClear(1);
            MemoryOpen();
            AutoSaveMode = 0;
            AutoSaveCursor = 0;
            AutoSaveFormat = 0;
            type_buf = 0;
            return 0;
            }
        }
    } else {
        MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
        MemorySlotClear(0);
        MemorySlotClear(1);
        MemoryOpen();
        AutoSaveMode = 0;
        AutoSaveCursor = 0;
        AutoSaveFormat = 0;
        type_buf = 0;
        return 0;
    }
    return 0;
}

void JobChange(int job);
void JobNext(void);
void MemoryOpen(void);
void MemorySetSystemDir(char *data, int size);
void MemorySlotClear(int slot);
void g_main_End(void);
void load_font(void);
extern SYS_COND sc;
extern USER_SETTING userinfo;
extern int SubJob;
extern int SubJobCount;

void AutoSaveInit(void)
{
    if (sc.grpvar[1] == -1) {
        g_main_End();
        load_font();
        sc.grpvar[1] = 0;
        MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
        MemorySlotClear(0);
        MemorySlotClear(1);
        MemoryOpen();
        AutoSaveMode = 0;
        AutoSaveCursor = 0;
        AutoSaveFormat = 0;
        type_buf = 0;
        SubJob = 0;
        SubJobCount = 0;
        JobNext();
    } else if (sc.grpvar[1]) {
        JobChange(3);
        sc.grpvar[1] = 0;
    }
}

void JobChange(int job);
int MemoryCheck(void);
void MemoryClose(void);
void MemoryFormatAndSave(int slot);
void MemorySave(int slot);
void MemorySetSystemDir(char *data, int size);
void MemoryType(int slot, int *type, int *free, int *exist);
extern SYS_COND sc;
extern USER_SETTING userinfo;
extern int SubJob;
extern int SubJobCount;

void AutoSaveMain(void)
{
    int result;
    int type;
    int free;
    int exist;

    if (AutoSaveMode == 0) {
        MemoryType(0, &type, &free, &exist);
        if (type != type_buf) {
            AutoSaveMode = 0;
            AutoSaveCursor = 0;
            AutoSaveFormat = 0;
        }
        type_buf = type;
        if (type == 0) {
            AutoSaveMode = 0;
            AutoSaveCursor = 0;
            AutoSaveFormat = 0;
        } else if (type == 4) {
            if (exist == 0) {
                if (free < 71) {
                    slot_display(0);
                    FontDraw("73KB IS NEEDED", 236, 316, 2.0f, 255);
                    if (save_err_mes("SHORT OF MEMORY")) {
                        MemoryClose();
                        sc.grpvar[1] = 1;
                        JobChange(3);
                    }
                } else {
                    AutoSaveMode = 1;
                }
            } else if (exist == 1) {
                AutoSaveMode = exist;
            }
        } else if (type == 1) {
            slot_display(0);
            if (save_err_mes("NO MEMORY CARD (PS2)")) {
                MemoryClose();
                sc.grpvar[1] = 1;
                JobChange(3);
            }
        } else if (type == 3) {
            slot_display(0);
            if (AutoSaveFormat) {
                if (save_err_mes("UNFORMATTED MEMORY CARD (PS2)")) {
                    MemoryClose();
                    sc.grpvar[1] = 1;
                    JobChange(3);
                }
            } else if (save_format_mes("UNFORMATTED MEMORY CARD (PS2)")) {
                AutoSaveMode = 2;
            }
        } else {
            slot_display(0);
            if (save_err_mes("ILLEGAL MEMORY CARD (PS2)")) {
                MemoryClose();
                sc.grpvar[1] = 1;
                JobChange(3);
            }
        }
    } else if (AutoSaveMode == 1) {
        MemoryClose();
        MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
        MemorySave(0);
        SubJob = 1;
        AutoSaveMode = 3;
        SubJobCount = 0;
        AutoSaveCursor = 0;
    } else if (AutoSaveMode == 2) {
        MemoryClose();
        MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
        MemoryFormatAndSave(0);
        SubJob = 1;
        AutoSaveMode = 3;
        SubJobCount = 0;
        AutoSaveCursor = 0;
    } else if (SubJob > 0) {
        SubJob = MemoryCheck();
        slot_display(0);
        FontDraw("NOW SAVING...", 242, 236, 2.0f, 255);
        FontDraw("PLEASE DO NOT REMOVE THE MEMORY CARD (PS2)", 68, 272, 2.0f, 255);
    } else if (SubJobCount < 180) {
        SubJobCount++;
        if (SubJob == 0) {
            slot_display(0);
            FontDraw("SAVE COMPLETED", 236, 236, 2.0f, 255);
        } else if (SubJob == -8) {
            vm_error(-9, 2, 0);
        } else {
            vm_error(SubJob, 2, 0);
            if (SubJob == -5) {
                FontDraw("73KB IS NEEDED", 236, 276, 2.0f, 255);
            }
        }
    } else if (SubJob == 0) {
        sc.grpvar[1] = 1;
        JobChange(3);
    } else {
        slot_display(0);
        result = save_err_mes_ex("SAVE FAILED");
        if (result == 1) {
            sc.grpvar[1] = result;
            JobChange(3);
        } else if (result == 0) {
            sc.grpvar[1] = -1;
            JobChange(45);
        }
    }
}

void MemoryOpen(void);
void MemorySetSystemDir(char *data, int size);
void MemorySlotClear(int slot);
extern USER_SETTING userinfo;

void AutoLoadInit(void)
{
    MemorySetSystemDir((char *)&userinfo, sizeof(USER_SETTING));
    MemorySlotClear(0);
    MemorySlotClear(1);
    MemoryOpen();
    AutoLoadMode = 0;
    AutoLoadCursor = 0;
}

int load_err_mes(char *str)
{
    FontDraw("CONTINUE?", 266, 260, 2.0f, 255);
    if (AutoLoadCursor == 0) {
        FontDraw("NO     ", 278, 288, 2.0f, 255);
        FontDraw("     YES", 272, 288, 2.0f, 80);
    } else {
        FontDraw("NO     ", 278, 288, 2.0f, 80);
        FontDraw("     YES", 272, 288, 2.0f, 255);
    }
    if ((pads[0].trg | pads[1].trg) & 0x20) {
        sound_request(0x39);
        if (AutoLoadCursor == 1) {
            return 1;
        }
    }
    if ((pads[0].trg | pads[1].trg) & 0xA000) {
        sound_request(0x37);
        AutoLoadCursor ^= 1;
    }
    return 0;
}

int MemoryCheck(void);
void MemoryClose(void);
void MemoryLoad(int slot);
void MemoryType(int slot, int *type, int *free, int *exist);
void debugf(char *format);

int AutoLoadMain(void)
{
    int ret;
    int result;
    int type[2];
    int free[2];
    int exist[2];
    static int port;

    if (AutoLoadMode == 0) {
        MemoryType(0, &type[0], &free[0], &exist[0]);
        MemoryType(1, &type[1], &free[1], &exist[1]);
        if (type[0] == 0 || type[1] == 0) {
            return 0;
        }
        if (type[0] == 4 && exist[0] == -1) {
            return 0;
        }
        if (type[1] == 4 && exist[1] == -1) {
            return 0;
        }
        if (type[0] == 4) {
            if (exist[0] == 0 && free[0] >= 71) {
                MemoryClose();
                return 1;
            }
            if (exist[0] == 1) {
                AutoLoadMode = exist[0];
                port = 0;
                return 0;
            }
        }
        if (type[1] == 4) {
            if (exist[1] == 0 && free[1] >= 71) {
                MemoryClose();
                return 1;
            }
            if (exist[1] == 1) {
                MemoryClose();
                return 1;
            }
        }
        if (type[0] == 4) {
            if (exist[0] == 0 && free[0] < 71) {
                slot_display_ex(0, "SHORT OF MEMORY", 1);
            }
        } else if (type[0] == 3) {
            slot_display_ex(0, "UNFORMATTED MEMORY CARD (PS2)", 0);
        } else if (type[0] == 1) {
            slot_display_ex(0, "NO MEMORY CARD (PS2)", 0);
        } else {
            slot_display_ex(0, "ILLEGAL MEMORY CARD (PS2)", 0);
        }
        if (type[1] == 4) {
            if (exist[1] == 0 && free[1] < 71) {
                slot_display_ex(1, "SHORT OF MEMORY", 1);
            }
        } else if (type[1] == 3) {
            slot_display_ex(1, "UNFORMATTED MEMORY CARD (PS2)", 0);
        } else if (type[1] == 1) {
            slot_display_ex(1, "NO MEMORY CARD (PS2)", 0);
        } else {
            slot_display_ex(1, "ILLEGAL MEMORY CARD (PS2)", 0);
        }
        if (load_err_mes("DUMMY")) {
            MemoryClose();
            return 1;
        }
        return 0;
    } else if (AutoLoadMode == 1) {
        debugf("Memory Loading\n");
        MemoryClose();
        MemoryLoad(port);
        AutoLoadCursor = 0;
        AutoLoadMode = 2;
        return 0;
    } else {
        ret = MemoryCheck();
        if (ret > 0) {
            slot_display(port);
            FontDraw("LOADING.....", 248, 240, 2.0f, 255);
            return 0;
        }
        if (AutoLoadMode < 180) {
            AutoLoadMode++;
            if (ret == 0) {
                slot_display(port);
                FontDraw("LOAD COMPLETE", 242, 240, 2.0f, 255);
                return 0;
            }
            if (ret == -1) {
                slot_display(port);
                FontDraw("LOAD ERROR!!", 248, 240, 2.0f, 255);
                FontDraw("DATA IS CORRUPTED", 218, 256, 2.0f, 255);
                return 0;
            }
            if (ret != -6) {
                vm_error(ret, 2, port);
                return 0;
            }
            return 0;
        }
        if (ret == 0) {
            return 1;
        }
        slot_display(port);
        result = load_err_mes_ex("LOAD FAILED");
        if (result == 1) {
            return 1;
        }
        if (result == 0) {
            AutoLoadInit();
        }
        return 0;
    }
}

void MemoryType(int slot, int *type, int *free, int *exist);

int vm_get_freeblock(int port)
{
    int type;
    int free;

    MemoryType(port, &type, &free, 0);
    if (type == 3) {
        return -1;
    }
    if (type == 2) {
        return -2;
    }
    if (type == 1 || (type == 4 && free < 0)) {
        return -3;
    }
    if (type == 4) {
        return free;
    }
    return -3;
}
