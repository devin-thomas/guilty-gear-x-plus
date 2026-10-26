#include "generated/opt_snd.h"

static const short idtb[] = { 1, 2, 3 };
static PROFILE_BGM pbgm[36] = {
    { 4, "01-KEEP YOURSELF ALIVE 2", "", "-- SOL --" },
    { 19, "02-HOLY ORDERS", "    (BE JUST OR BE DEAD)", "-- KY --" },
    { 5, "03-BLUE WATER BLUE SKY", "", "-- MAY --" },
    { 21, "04-WRITHE IN PAIN", "", "-- MILLIA --" },
    { 6, "05-FEEL A FEAR", "", "-- ZATO --" },
    { 8, "06-BURLY HEART", "", "-- POTEMKIN --" },
    { 20, "07-SUCK A SAGE", "", "-- CHIPP --" },
    { 9, "08-THE ORIGINAL", "", "-- FAUST --" },
    { 22, "09-MOMENTARY LIFE", "", "-- BAIKEN --" },
    { 13, "10-BABEL NOSE", "", "-- JAM --" },
    { 10, "11-LIQUOR BAR & DRUNKARD", "", "-- JOHNNY --" },
    { 7, "12-MAKE ONESELF", "", "-- AXL --" },
    { 11, "13-FUUGA", "", "-- ANJI --" },
    { 12, "14-A SOLITUDE THAT ASKS", "    NOTHING IN RETURN", "-- VENOM --" },
    { 14, "15-BLOODSTAINED LINEAGE", "", "-- TESTAMENT --" },
    { 15, "16-AWE OF SHE", "", "-- DIZZY --" },
    { 16, "17-NO MERCY", "", "-- SOL VS KY --" },
    { 18, "18-STILL IN THE DARK", "", "-- ZATO VS MILLIA --" },
    { 17, "19-FATAL DUEL", "", "-- DOU CHARA --" },
    { 23, "20-SOUL DEALER", "", "-- OPENING --" },
    { 24, "21-HELLO MY DEAR", "", "-- OPTION --" },
    { 25, "22-STAND BY", "", "-- EDIT --" },
    { 26, "23-IT WAS CALLED VICTIM", "", "-- CHARACTER SELECT --" },
    { 27, "24-GO!", "", "-- PROFILE --" },
    { 28, "25-RETAKE", "", "-- CONTINUE --" },
    { 29, "26-SETTLEMENT", "", "-- GAME OVER --" },
    { 30, "27-A DAREDEVIL", "", "-- VS DEMO --" },
    { 31, "28-IT WAS CALLED STEELVICTIM", "", "-- WIN DEMO --" },
    { 32, "29-KEEP THE FAITH", "", "-- TESTAMENT DEMO --" },
    { 33, "30-GRIEF", "", "-- DIZZY DEMO --" },
    { 34, "31-PRIMAL LIGHT", "", "-- ENDING1 --" },
    { 35, "32-CALM PASSION", "", "-- ENDING2 --" },
    { 36, "33-WALK IN THE DUSK", "", "-- ENDING3 --" },
    { 37, "34-BEYOND THE DARK LIFE", "", "-- ENDING4 --" },
    { 38, "XX-PRIDE AND GLORY", "", "-- KLIFF --" },
    { 39, "XX-MEET AGAIN", "", "-- JUSTICE --" },
};
static PROFILE_SE pvoice[] = {
    { 0, "SOL BADGUY TYPE-A" },
    { 0, "SOL BADGUY TYPE-B" },
    { 0, "SOL BADGUY TYPE-C" },
    { 1, "KY KISKE TYPE-A" },
    { 1, "KY KISKE TYPE-B" },
    { 1, "KY KISKE TYPE-C" },
    { 2, "MAY TYPE-A" },
    { 2, "MAY TYPE-B" },
    { 2, "MAY TYPE-C" },
    { 3, "MILLIA RAGE TYPE-A" },
    { 3, "MILLIA RAGE TYPE-B" },
    { 3, "MILLIA RAGE TYPE-C" },
    { 4, "AXL LOW TYPE-A" },
    { 4, "AXL LOW TYPE-B" },
    { 4, "AXL LOW TYPE-C" },
    { 5, "POTEMKIN TYPE-A" },
    { 5, "POTEMKIN TYPE-B" },
    { 5, "POTEMKIN TYPE-C" },
    { 6, "CHIPP ZANUFF TYPE-A" },
    { 6, "CHIPP ZANUFF TYPE-B" },
    { 6, "CHIPP ZANUFF TYPE-C" },
    { 7, "ZATO-ONE TYPE-A" },
    { 7, "ZATO-ONE TYPE-B" },
    { 7, "ZATO-ONE TYPE-C" },
    { 8, "BAIKEN TYPE-A" },
    { 8, "BAIKEN TYPE-B" },
    { 8, "BAIKEN TYPE-C" },
    { 9, "FAUST TYPE-A" },
    { 9, "FAUST TYPE-B" },
    { 9, "FAUST TYPE-C" },
    { 10, "TESTAMENT TYPE-A" },
    { 10, "TESTAMENT TYPE-B" },
    { 10, "TESTAMENT TYPE-C" },
    { 11, "JAM KURADOBERI TYPE-A" },
    { 11, "JAM KURADOBERI TYPE-B" },
    { 11, "JAM KURADOBERI TYPE-C" },
    { 12, "ANJI MITO TYPE-A" },
    { 12, "ANJI MITO TYPE-B" },
    { 12, "ANJI MITO TYPE-C" },
    { 13, "JOHNNY TYPE-A" },
    { 13, "JOHNNY TYPE-B" },
    { 13, "JOHNNY TYPE-C" },
    { 14, "VENOM TYPE-A" },
    { 14, "VENOM TYPE-B" },
    { 14, "VENOM TYPE-C" },
    { 15, "DIZZY TYPE-A" },
    { 15, "DIZZY TYPE-B" },
    { 15, "DIZZY TYPE-C" },
    { 16, "KLIFF UNDERSN TYPE-A" },
    { 16, "KLIFF UNDERSN TYPE-B" },
    { 16, "KLIFF UNDERSN TYPE-C" },
    { 17, "JUSTICE TYPE-A" },
    { 17, "JUSTICE TYPE-B" },
    { 17, "JUSTICE TYPE-C" },
};
static PROFILE_SE pse[] = {
    { 18, "SOL BADGUY" },
    { 19, "KY KISKE" },
    { 20, "MAY" },
    { 21, "MILLIA RAGE" },
    { 22, "AXL LAW" },
    { 23, "POTEMKIN" },
    { 24, "CHIPP ZANUFF" },
    { 25, "ZATO-ONE" },
    { 26, "BAIKEN" },
    { 27, "FAUST" },
    { 28, "TESTAMENT" },
    { 29, "JAM KURADOBERI" },
    { 30, "ANJI MITO" },
    { 31, "JOHNNY" },
    { 32, "VENOM" },
    { 33, "DIZZY" },
    { 34, "KLIFF UNDERSN" },
    { 35, "JUSTICE" },
    { 36, "COMMON" },
};
static PROFILE_SE se_profile[] = {
    { 30, "VOICE -- SOL BADGUY --" },
    { 41, "VOICE -- KY KISKE --" },
    { 43, "VOICE -- MAY --" },
    { 28, "VOICE -- MILLIA RAGE --" },
    { 41, "VOICE -- AXL LOW --" },
    { 37, "VOICE -- POTEMKIN --" },
    { 40, "VOICE -- CHIPP ZANUFF --" },
    { 38, "VOICE -- ZATO-ONE --" },
    { 44, "VOICE -- BAIKEN --" },
    { 43, "VOICE -- FAUST --" },
    { 32, "VOICE -- TESTAMENT --" },
    { 47, "VOICE -- JAM KURADOBERI --" },
    { 34, "VOICE -- ANJI MITO --" },
    { 39, "VOICE -- JOHNNY --" },
    { 31, "VOICE -- VENOM --" },
    { 25, "VOICE -- DIZZY --" },
    { 28, "VOICE -- KLIFF UNDERSN --" },
    { 28, "VOICE -- JUSTICE --" },
    { 7, "SE -- SOL BADGUY --" },
    { 6, "SE -- KY KISKE --" },
    { 6, "SE -- MAY --" },
    { 4, "SE -- MILLIA RAGE --" },
    { 5, "SE -- AXL LOW --" },
    { 8, "SE -- POTEMKIN --" },
    { 7, "SE -- CHIPP ZANUFF --" },
    { 7, "SE -- ZATO-ONE --" },
    { 6, "SE -- BAIKEN --" },
    { 8, "SE -- FAUST --" },
    { 8, "SE -- TESTAMENT --" },
    { 9, "SE -- JAM KURADOBERI --" },
    { 7, "SE -- ANJI MITO --" },
    { 6, "SE -- JOHNNY --" },
    { 7, "SE -- VENOM --" },
    { 8, "SE -- DIZZY --" },
    { 5, "SE -- KLIFF UNDERSN --" },
    { 5, "SE -- JUSTICE --" },
    { 96, "SE -- COMMON -- " },
};
static OPTSND os;
static int ret;
extern OPTINFO opt;
extern GDCOND gcond;
extern SYS_COND sc;
extern PAD pads[2];
extern USER_SETTING userinfo;
extern char bgmvol[5];
extern int current_bgm;
int iodat_chk(int stat);
int sound_request(int no);
int SnDrvBgmCheck(void);
void BGM_Request(int no, int loop);
void BGM_Free(void);
void VoiceFree(void);
void get_soundmode(void);
void set_bgmvol(void);
void set_soundmode(int mode);
int FileReadCheck(void);
int sound_request_main(int no, int type);
void set_char2num(int num, char *block, int id);
void sound_rec_clear(void);
void stest_packload(int no, int no2);
void FontDraw(char *str, int x, int y, float z, int alfa);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void *memcpy(void *d, const void *s, unsigned int n);

void Opt_SoundTest(int sw) {
    int i;
    int alpha;
    int pad;
    void (*stst_proc[6])() = { stst_mode, stst_bgm_vol, stst_bgm, stst_se, stst_voice, stst_exit };
    void (*stst_font_proc[6])() = { stst_font_mode, stst_font_bgm_vol, stst_font_bgm, stst_font_se, stst_font_voice, stst_font_exit };
    static HEADWORK mh = { 0, 64.0f, 112.0f, 0, 6, 0 };
#ifdef GGX_PORT
    /* mh.p keeps pointing at mp after Opt_SoundTest(0) returns. On the EE the
       later calls put mp at the same stack address and fill it again; clang
       drops that fill in calls that never read mp by name, so mh.p would read
       a stale frame. One static copy reads as the EE's does. */
    static
#endif
    PARTS mp[6] = {
        { "MODE", 0.0f, 0.0f, 0 },
        { "BGM VOLUME", 0.0f, 48.0f, 0 },
        { "BGM TEST", 0.0f, 96.0f, 0 },
        { "SE TEST", 0.0f, 176.0f, 0 },
        { "VOICE TEST", 0.0f, 224.0f, 0 },
        { "EXIT", 0.0f, 272.0f, 0 },
    };
    HEADWORK hw[6] = {
        { 0, 64.0f, 160.0f, 0, 0, 0 },
        { 0, 64.0f, 160.0f, 0, 0, 0 },
        { 0, 64.0f, 160.0f, 0, 0, 0 },
        { 0, 64.0f, 160.0f, 0, 0, 0 },
        { 0, 64.0f, 160.0f, 0, 0, 0 },
        { 0, 64.0f, 160.0f, 0, 0, 0 },
    };
    if (sw == 0) {
        soundtest_init();
        ret = 0;
        mh.p = mp;
        return;
    }
    if (ret == 0) {
        FontDrawDC("SOUND TEST", 32.0f, 40.0f, 4.0f, 1, 0);
        for (i = 0; i < mh.maxhead; i++) {
            FontDrawDC(mh.p[i].head, mh.xp + mh.p[i].xp, mh.yp + mh.p[i].yp, 4.0f, (mh.pos == i) ? 1 : 0xa0, 1);
        }
        disp_usage(mh.pos);
        if (os.loadtime < 0 && !iodat_chk(0x8000) && !iodat_chk(0x2000)) {
            if ((pad = iodat_chk(0x1000))) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    sound_request(0x37);
                    if (mh.pos == 0) mh.pos = mh.maxhead - 1; else mh.pos = mh.pos - 1;
                    os.vonumpos = -1;
                    os.senumpos = -1;
                    os.wakupos = 0;
                }
                sc.cnt[pad - 1]++;
            } else if ((pad = iodat_chk(0x4000))) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    sound_request(0x37);
                    if (mh.pos == mh.maxhead - 1) {
                        mh.pos = 0;
                    } else {
                        mh.pos = mh.pos + 1;
                    }
                    os.vonumpos = -1;
                    os.senumpos = -1;
                    os.wakupos = 0;
                }
                sc.cnt[pad - 1]++;
            } else {
                sc.cnt[0] = 0;
                sc.cnt[1] = 0;
            }
        }
        if (os.loadtime == -1) {
            if ((pads[0].trg | pads[1].trg) & 0x40) {
                if ((opt.plyno == -1 && mh.pos == 2) || (os.vonumpos == -1 && mh.pos == 4) || (os.senumpos == -1 && mh.pos == 3) || (mh.pos == 0 || mh.pos == 1) || mh.pos == 5) {
                    sound_request(0x3b);
                    ret = 1;
                }
            }
            if ((pads[0].trg | pads[1].trg) & 0x20) {
                if (mh.pos == mh.maxhead - 1) {
                    sound_request(0x39);
                    ret = 1;
                }
            }
        }
        stst_proc[mh.pos](&hw[mh.pos]);
        play_nextsnd(pbgm[opt.plyno].no);
        for (i = 0; i < mh.maxhead; i++) {
            stst_font_proc[i](&hw[mh.pos]);
        }
    }
    if (ret != 0) {
        if (SnDrvBgmCheck()) {
            BGM_Free();
            opt.plyno = -1;
        } else {
            BGM_Request(0x15, 1);
            VoiceFree();
            opt.thdflg &= ~2;
        }
    }
}

static void stst_bgm(HEADWORK *hw) {
    int pad;
    if (!iodat_chk(0x1000) && !iodat_chk(0x4000)) {
        if ((pad = iodat_chk(0x8000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                if (os.bgmpos) {
                    sound_request(6);
                    os.bgmpos--;
                }
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x2000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                if (os.bgmpos < 35) {
                    sound_request(6);
                    os.bgmpos++;
                }
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
    }
    if ((pads[0].trg | pads[1].trg) & 0x820) {
        opt.plyno = os.bgmpos + os.wakupos; gcond.cond = 1; gcond.reserve = opt.plyno;
        if ((pads[0].trg | pads[1].trg) & 0x800) {
            os.repeat = 0;
            if (SnDrvBgmCheck()) os.nomedinc = 1;
        } else {
            os.repeat = 1;
        }
        if (SnDrvBgmCheck()) {
            BGM_Free();
        } else {
            BGM_Request(pbgm[opt.plyno].no - 3, os.repeat);
        }
    }
    if ((pads[0].trg | pads[1].trg) & 0x40) {
        if (opt.plyno != -1) {
            sound_request(0x3b);
            opt.plyno = -1;
            os.repeat = -1;
            gcond.reserve = -1;
            gcond.cond = 0;
            BGM_Free();
        }
    }
}

static void stst_font_bgm(HEADWORK *hw) {
    int i;
    char no[16];
    char buf[48];
    char buf2[48];
    memcpy(buf, pbgm[os.bgmpos].bgmname, 48);
    memcpy(buf2, pbgm[os.bgmpos].bgmname2, 48);
    for (i = 0; i < 3; i++) {
        no[i] = buf[i];
        buf[i] = ' ';
    }
    no[2] = 0;
    FontDraw(no, 0x120, 0xd8, 4.0f, 255);
    FontDraw(pbgm[os.bgmpos].profile, 0x140, 0xd8, 4.0f, 255);
    FontDraw(buf, 0x120, 0xec, 4.0f, 255);
    FontDraw(buf2, 0x120, 0x100, 4.0f, 255);
    if (opt.plyno != -1) {
        memcpy(buf, pbgm[opt.plyno].bgmname, 48);
        no[0] = 'P';
        no[1] = 'L';
        no[2] = 'A';
        no[3] = 'Y';
        no[4] = 'N';
        no[5] = 'O';
        no[6] = ' ';
        for (i = 0; i < 3; i++) {
            no[7 + i] = buf[i];
            buf[i] = ' ';
        }
        no[9] = 0;
        FontDraw(no, 0x70, 0xec, 4.0f, 255);
    } else {
        FontDraw("PLAYNO 00", 0x70, 0xec, 4.0f, 0x50);
    }
    if (os.repeat == 0) {
        FontDraw("MEDLEY ON", 0x70, 0x100, 4.0f, 255);
    } else {
        FontDraw("MEDLEY OFF", 0x70, 0x100, 4.0f, 0x50);
    }
}

static void stst_mode(void) {
    int pad;
    if (!iodat_chk(0x1000) && !iodat_chk(0x4000)) {
        if ((pad = iodat_chk(0x8000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(6);
                get_soundmode();
                if (userinfo.sound_mode == 0) {
                    set_soundmode(1);
                } else {
                    set_soundmode(0);
                }
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x2000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(6);
                get_soundmode();
                if (userinfo.sound_mode == 0) {
                    set_soundmode(1);
                } else {
                    set_soundmode(0);
                }
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
    }
}

static void stst_font_mode(void) {
    char *smode[2] = { "STEREO", "MONO" };
    FontDraw(smode[userinfo.sound_mode], 0x120, 0x78, 4.0f, 255);
}

static void stst_bgm_vol(void) {
    int pad;
    int vmd;
    if (!iodat_chk(0x1000) && !iodat_chk(0x4000)) {
        if ((pad = iodat_chk(0x8000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(6);
                vmd = userinfo.bgm;
                if (--vmd < 0) vmd = 4;
                userinfo.bgm = vmd;
                userinfo.vol_bgm = userinfo.vol_bgm_mst = bgmvol[userinfo.bgm];
                set_bgmvol();
            }
            sc.cnt[pad - 1]++;
        } else if ((pad = iodat_chk(0x2000))) {
            if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                sound_request(6);
                vmd = userinfo.bgm;
                if (++vmd >= 5) vmd = 0;
                userinfo.bgm = vmd;
                userinfo.vol_bgm = userinfo.vol_bgm_mst = bgmvol[userinfo.bgm];
                set_bgmvol();
            }
            sc.cnt[pad - 1]++;
        } else {
            sc.cnt[0] = 0;
            sc.cnt[1] = 0;
        }
    }
}

static void stst_font_bgm_vol(void) {
    char *vmode[8] = { "SILENT", "SMALL", "MEDIUM", "STANDARD", "MAX" };
    FontDraw(vmode[userinfo.bgm], 0x120, 0xa8, 4.0f, 255);
}

void stst_exit(void) {
}

void stst_font_exit(void) {
}

static void play_nextsnd(int no) {
    if (!SnDrvBgmCheck()) {
        if (current_bgm >= 0 && os.loadtime >= 0) {
            BGM_Free();
        } else if (FileReadCheck()) {
            if (os.loadtime == -1) {
                if (os.repeat == 0) {
                    BGM_Free();
                    if (os.nomedinc == 0) {
                        opt.plyno++;
                    } else {
                        os.nomedinc = 0;
                    }
                    if (opt.plyno >= 36) opt.plyno = 0;
                    BGM_Request(pbgm[opt.plyno].no - 3, 0);
                } else if (opt.plyno != os.loadtime) {
                    BGM_Free();
                    BGM_Request(pbgm[opt.plyno].no - 3, 0);
                }
            }
        }
    }
}

static void stst_voice(HEADWORK *hw) {
    int pad;
    if (os.loadtime < 0) {
        if (!iodat_chk(0x1000) && !iodat_chk(0x4000)) {
            if ((pad = iodat_chk(0x8000))) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    if (os.vonumpos == -1) {
                        sound_request(6);
                        if (--os.vopos < 0) os.vopos = 50;
                    } else {
                        sound_request(6);
                        if (--os.vonumpos < 0) os.vonumpos = os.maxno;
                    }
                }
                sc.cnt[pad - 1]++;
            } else if ((pad = iodat_chk(0x2000))) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    if (os.vonumpos == -1) {
                        sound_request(6);
                        if (++os.vopos >= 50 + 1) os.vopos = 0;
                    } else {
                        sound_request(6);
                        if (++os.vonumpos > os.maxno) os.vonumpos = 0;
                    }
                }
                sc.cnt[pad - 1]++;
            } else {
                sc.cnt[0] = 0;
                sc.cnt[1] = 0;
            }
        }
    }
    if (os.loadtime > 0) {
        if (FileReadCheck()) os.loadtime--;
    } else if (os.loadtime == 0 && os.vonumpos == -1) {
        if (FileReadCheck()) os.vonumpos = 0;
        os.loadtime = -1;
    } else if ((pads[0].trg | pads[1].trg) & 0x20) {
        if (os.vonumpos == -1) {
            if (SnDrvBgmCheck()) os.nomedinc = 1;
            sound_request(0x39);
            if (os.loaddat[0] || os.loaddat[1] != os.vopos) {
                stest_packload(pvoice[os.vopos].no + 1, os.vopos % 3);
                os.loaddat[1] = os.vopos;
                gcond.reserve = opt.plyno;
                gcond.cond = 1;
                os.loaddat[0] = 0;
            }
            os.loadtime = 0x20;
            os.profile = pvoice[os.vopos].sename;
            os.maxno = se_profile[pvoice[os.vopos].no].no;
            os.id = 2;
        } else {
            sound_rec_clear();
            os.playno = os.vonumpos;
            sound_request_main(os.vonumpos, os.id);
        }
    } else if ((pads[0].trg | pads[1].trg) & 0x40) {
        if (os.vonumpos != -1) {
            sound_request(0x3b);
            os.vonumpos = -1;
        }
    }
}

static void stst_font_voice(HEADWORK *hw) {
    int alpha;
    int temp;
    char *buf;
    char *buf2;
    char buf_num[16];
    char buf_temp[16];
    if (os.vonumpos == -1) {
        alpha = 0x50;
        temp = 0;
    } else {
        alpha = 255;
        temp = os.vonumpos;
    }
    buf = pvoice[os.vopos].sename;
    set_char2num(temp, buf_num, 0);
    buf_temp[0] = ' ';
    buf_temp[1] = ' ';
    buf_temp[2] = ' ';
    buf_temp[3] = 'P';
    buf_temp[4] = 'L';
    buf_temp[5] = 'A';
    buf_temp[6] = 'Y';
    buf_temp[7] = 'N';
    buf_temp[8] = 'O';
    buf_temp[9] = ' ';
    if (os.vonumpos < 10) { buf_temp[10] = '0'; buf_temp[11] = buf_num[0]; buf_temp[12] = 0; } else { buf_temp[10] = buf_num[0]; buf_temp[11] = buf_num[1]; buf_temp[12] = 0; } buf2 = buf_temp;
    FontDraw(buf, 0x120, 0x152, 4.0f, 255);
    FontDraw(buf2, 0x120, 0x166, 4.0f, alpha);
}

static void stst_se(HEADWORK *hw) {
    int pad;
    if (os.loadtime < 0) {
        if (!iodat_chk(0x1000) && !iodat_chk(0x4000)) {
            if ((pad = iodat_chk(0x8000))) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    if (os.senumpos == -1) {
                        sound_request(6);
                        if (--os.sepos < 0) os.sepos = 18;
                    } else {
                        sound_request(6);
                        if (--os.senumpos < 0) os.senumpos = os.maxno;
                    }
                }
                sc.cnt[pad - 1]++;
            } else if ((pad = iodat_chk(0x2000))) {
                if (sc.cnt[pad - 1] == 0 || (sc.cnt[pad - 1] > 16 && !(sc.cnt[pad - 1] & 3))) {
                    if (os.senumpos == -1) {
                        sound_request(6);
                        if (++os.sepos >= 18 + 1) os.sepos = 0;
                    } else {
                        sound_request(6);
                        if (++os.senumpos > os.maxno) os.senumpos = 0;
                    }
                }
                sc.cnt[pad - 1]++;
            } else {
                sc.cnt[0] = 0;
                sc.cnt[1] = 0;
            }
        }
    }
    if (os.loadtime > 0) {
        if (FileReadCheck()) os.loadtime--;
    } else if (os.loadtime == 0 && os.senumpos == -1) {
        if (FileReadCheck()) os.senumpos = 0;
        os.loadtime = -1;
    } else if ((pads[0].trg | pads[1].trg) & 0x20) {
        if (os.senumpos == -1) {
            if (SnDrvBgmCheck()) os.nomedinc = 1;
            sound_request(0x39);
            if (os.loaddat[0] != 1 || os.loaddat[1] != os.sepos) {
                stest_packload(os.sepos + 1, -1);
                os.loaddat[1] = os.sepos;
                gcond.reserve = opt.plyno;
                os.loaddat[0] = 1;
                gcond.cond = 1;
            }
            os.loadtime = 0x20;
            os.profile = pse[os.sepos].sename;
            os.maxno = se_profile[pse[os.sepos].no].no;
            if (os.sepos != 18) {
                os.id = 3;
            } else {
                os.id = 0;
            }
        } else {
            sound_rec_clear();
            os.playno = os.senumpos;
            if (os.id == 0) {
                sound_request(os.senumpos);
            } else {
                sound_request_main(os.senumpos, os.id);
            }
        }
    } else if ((pads[0].trg | pads[1].trg) & 0x40) {
        if (os.senumpos != -1) {
            sound_request(0x3b);
            os.senumpos = -1;
        }
    }
}

static void stst_font_se(HEADWORK *hw) {
    int alpha;
    int temp;
    char *buf;
    char *buf2;
    char buf_num[16];
    char buf_temp[16];
    if (os.senumpos == -1) {
        alpha = 0x50;
        temp = 0;
    } else {
        alpha = 255;
        temp = os.senumpos;
    }
    buf = pse[os.sepos].sename;
    set_char2num(temp, buf_num, 0);
    buf_temp[0] = ' ';
    buf_temp[1] = ' ';
    buf_temp[2] = ' ';
    buf_temp[3] = 'P';
    buf_temp[4] = 'L';
    buf_temp[5] = 'A';
    buf_temp[6] = 'Y';
    buf_temp[7] = 'N';
    buf_temp[8] = 'O';
    buf_temp[9] = ' ';
    if (os.senumpos < 10) {
        buf_temp[10] = '0';
        buf_temp[11] = buf_num[0];
        buf_temp[12] = 0;
    } else {
        buf_temp[10] = buf_num[0];
        buf_temp[11] = buf_num[1];
        buf_temp[12] = 0;
    }
    buf2 = buf_temp;
    FontDraw(buf, 0x120, 0x122, 4.0f, 255);
    FontDraw(buf2, 0x120, 0x136, 4.0f, alpha);
}

static void disp_usage(int flg) {
    char buf[6][64] = {
        "w-      x-END   df-CHANGE  START-",
        "w-PLAY  x-STOP  df-CHANGE  START-MEDLEY",
        "w-PLAY  x-END   df-CHANGE  START-MEDLEY",
        "w-PLAY  x-BACK  df-CHANGE  START-",
        "w-LOAD  x-END   df-CHANGE  START-",
        "w-END   x-END   df-        START-",
    };
    int idx;
    switch (flg) {
    case 0:
    case 1:
        idx = 0;
        break;
    case 2:
        idx = (opt.plyno != -1) ? 1 : 2;
        break;
    case 3:
        idx = (os.senumpos != -1) ? 3 : 4;
        break;
    case 4:
        idx = (os.vonumpos != -1) ? 3 : 4;
        break;
    case 5:
        idx = 5;
        break;
    default:
        idx = 0;
        break;
    }
    FontDraw(buf[idx], 0x40, 0x1b0, 4.0f, 255);
}

static void soundtest_init(void) {
     opt.plyno = -1; os.wakupos = 0; gcond.cond = 0; gcond.reserve = -1; os.bgmpos = 0; os.sepos = 0; os.vopos = 0; os.senumpos = -1; os.vonumpos = -1; os.nomedinc = 0; os.pos[0] = 0; os.pos[1] = 0; os.pos[2] = 0; os.profile = 0; os.loaddat[0] = -1; os.loaddat[1] = -1; os.maxno = 0; os.playno = -1; os.id = -1; os.repeat = -1; os.loadtime = -1;

}
