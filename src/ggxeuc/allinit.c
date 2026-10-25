#include "generated/allinit.h"

extern USER_SETTING userinfo;
void g_main_End(void);
void set_soundmode(int mode);
void JobChange(int job);

extern char bgmvol[5];
extern SYS_COND sc;

void userinfo_init(void)
{
    int i;
    int n;

    sc.mon_id = 0;
    userinfo.xpos = 0;
    userinfo.ypos = 0;
    userinfo.offs_cockpit_upper = 0x16;
    userinfo.offs_cockpit_lower = -0x16;
    userinfo.roundtime = 2;
    userinfo.roundnum = 1;
    userinfo.glevel = 2;
    userinfo.medal = 0;
    userinfo.damage[0] = 1;
    userinfo.damage[1] = 1;
    userinfo.phase = 0;
    userinfo.clear = 0;
    userinfo.bgm = 3;
    userinfo.vol_bgm_mst = bgmvol[3];
    userinfo.vol_bgm = bgmvol[3];
    userinfo.vol_se_mst = 0x7F;
    userinfo.vol_se = 0x7F;
    userinfo.sound_mode = 0;
    userinfo.autosave = 0;
    userinfo.quickconti = 0;
    userinfo.ggmode[0] = 0;
    userinfo.ggmode[1] = 0;
    userinfo.gguse[0] = 0;
    userinfo.gguse[1] = 0;
    for (i = 0; i < 128; i++) {
        userinfo.gstart[i] = 0;
        userinfo.gcheck[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        userinfo.gstart[i] = 1;
        userinfo.gcheck[i] = 0;
    }
    for (i = 8; i < 16; i++) {
        userinfo.gstart[i] = 1;
        userinfo.gcheck[i] = 0;
    }
    for (i = 16; i < 24; i++) {
        userinfo.gstart[i] = 1;
        userinfo.gcheck[i] = 0;
    }
    for (i = 24; i < 32; i++) {
        userinfo.gstart[i] = 0;
        userinfo.gcheck[i] = 0;
    }
    for (i = 32; i < 40; i++) {
        userinfo.gstart[i] = 1;
        userinfo.gcheck[i] = 0;
    }
    for (i = 40; i < 82; i++) {
        userinfo.gstart[i] = 1;
        userinfo.gcheck[i] = 0;
    }
    for (i = 82; i < 98; i++) {
        userinfo.gstart[i] = 1;
        userinfo.gcheck[i] = 0;
    }
    for (i = 0; i < 2; i++) {
        userinfo.key[i].punch = 0x80;
        userinfo.key[i].kick = 0x40;
        userinfo.key[i].slash = 0x10;
        userinfo.key[i].hslash = 0x20;
        userinfo.key[i].fuckin = 8;
        userinfo.key[i].memory = 4;
        userinfo.key[i].pandk = 0;
        userinfo.key[i].sandhs = 0;
        userinfo.key[i].pks = 0;
        userinfo.key[i].pkshs = 0;
        userinfo.key[i].vibrate = 0;
    }
    userinfo.tbkup.level = 10;
    userinfo.tbkup.rejene = 3;
    userinfo.tbkup.ggage = 0;
    userinfo.tbkup.tension = 0;
    userinfo.tbkup.damage_disp = 0;
    userinfo.tbkup.mode = 0;
    userinfo.tbkup.type = 0;
    userinfo.tbkup.guard = 0;
    userinfo.tbkup.ukemi = 0;
    for (i = 0; i < 18; i++) {
        userinfo.svresult[i].id = i + 1;
        userinfo.svresult[i].level = 0;
        userinfo.svresult[i].stage = 0;
        for (n = 0; n < 10; n++) {
            userinfo.svresult[i].section_time[n] = 0;
        }
        userinfo.svresult[i].total_time = 0;
        userinfo.usecol[i] = 0x1F;
    }
    sc.selcnt[0] = 0;
    sc.selcnt[1] = 0;
    sc.lcd_count = 0;
    sc.reset_possib = 0;
    sc.select_read = 0;
    sc.net_entry = 0;
    for (i = 0; i < 4; i++) {
        sc.grpvar[i] = 0;
    }
}


void Reset(void) {
    g_main_End();
    set_soundmode(userinfo.sound_mode);
    JobChange(6);
}

void AllCharacterWorkRelease(void);
void heap_clear(void);
void EnterFight_Clear(void);
void FadeInit(void);
void sound_Request_Init(void);
void gStripDraw_Init(void);
void RandomInit(int num);
void CockpitLoadInit(void);
void CreditFontLoad(int no);
int SetLimitStatus(void);

void SoftReset(void) {
    AllCharacterWorkRelease();
    heap_clear();
    FileHeapClear();
    EnterFight_Clear();
    FadeInit();
    sound_Request_Init();
    gStripDraw_Init();
    RandomInit(0x51AF0);
    CockpitLoadInit();
    CreditFontLoad(0x28);
    SetLimitStatus();
    Reset();
}

extern int SubJob;
void PXF_WhiteSet(void);
void heap_clear(void);
void EnterFight_Clear(void);
void sound_Init(void);
void MakeFileTable(void);
void SinMake(void);
void gStripDraw_Init(void);
void RandomInit(int num);
void CockpitLoad(void);
void DCFontLoad(void);
void IconFileRead(void);
void EffCmnLoad(void);
void sound_load_Common(void);
void AutoLoadInit(void);
void JobNext(void);

void AllInitInit(void) {
    PXF_WhiteSet();
    heap_clear();
    FileHeapClear();
    EnterFight_Clear();
    sound_Init();
    MakeFileTable();
    SinMake();
    gStripDraw_Init();
    RandomInit(0x51AF0);
    CockpitLoad();
    DCFontLoad();
    IconFileRead();
    EffCmnLoad();
    sound_load_Common();
    SubJob = 0;
    AutoLoadInit();
    JobNext();
}

extern int JobMode;
extern PAD pads[2];
extern SYS_COND sc;
int FileReadCheck(void);
int MemoryCheck(void);
void JobChange(int job);

void soft_reset_check(void) {
    int i;
    int flag;

    flag = 0;
    if (JobMode < 3) {
        return;
    }
    if (!FileReadCheck()) {
        return;
    }
    if (MemoryCheck() > 0) {
        return;
    }
    for (i = 0; i < 2; i++) {
        if (pads[i].data == 0x90F) {
            flag = 1;
        }
    }
    if (flag) {
        sc.grpvar[0] = 0;
        JobChange(0);
    }
    sc.reset = flag;
}

extern char *VsFile[3];
extern char *OpeningFile;
extern char *Player1File;
extern char *Player2File;
extern char *WinnerFile;
extern char *WinnerChrFile;
extern char *ProfileFile;
extern char *StgSelFile;
extern char *EndingFile;
extern char *EdFontFile;

void FileHeapClear(void) {
    EndingFile = 0;
    EdFontFile = 0;
    OpeningFile = 0;
    Player1File = 0;
    Player2File = 0;
    WinnerFile = 0;
    WinnerChrFile = 0;
    ProfileFile = 0;
    StgSelFile = 0;
    VsFile[0] = 0;
    VsFile[1] = 0;
    VsFile[2] = 0;
}

extern int SubJob;
int FileReadCheck(void);
void CockpitLoadInit(void);
void CreditFontLoad(int no);
void BackupromInit(void);
void Setting_Default(HEADWORK *hw);
int AutoLoadMain(void);
int SetLimitStatus(void);

void AllInitMain(void) {
    if (SubJob == 0) {
        if (FileReadCheck()) {
            CockpitLoadInit();
            CreditFontLoad(0x28);
            BackupromInit();
            Setting_Default(0);
            userinfo_init();
            SubJob++;
        }
        return;
    }
    if (SubJob == 1) {
        if (AutoLoadMain() > 0) {
            SetLimitStatus();
            Reset();
        }
    }
}
