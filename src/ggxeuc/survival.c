#include "generated/survival.h"
void init_survival(int ply);
void end_survival(void);
void sv_hoimi_set(void);
void sv_hoimi(void);
int InitSurvivalBattle(void);
int get_rarecolor(int id);
int chk_complete2(void);
void sv_set_newcolor(int id, int color);
int chk_select_color(int id, int color);
int get_select_color(int id);
int sv_ending_chk(int flg);
void init_enemytb(void);
int get_maxchara(void);
void chk_survival_stat(void);
void chk_survivallevel(void);
void sv_set_sectiontime(void);
int sv_damage_hosei(CHARACTER_WORK *offset, int damage);
int chk_iai(void);
int chk_nikudan(void);
int add_exp_combo(int combo);
int add_exp_damage(int damage);
void sv_levelup(int lvl);
void sv_save_enemystatus(int side);
void sv_set_enemy_status(int side);
void sv_levelup_info(int sw);
void sv_levelup_info2(int num);
void SvResultInit(void);
void SvResultMain(void);
void result_disp(int flg);
void result_title(int time);
void result_player(int time);
void result_level(int time);
void result_stage(int time);
void result_time(int time);
void time_convert(char *buf, u_int time, int id);
void sv_timehosei(void);
void result_bg_mask(void);
void sv_result_init(void);
void set_clearflg_sv(void);
void sv_save_result(void);
void sv_IntIrqMain(void);
void SvBossInit(void);
extern SURVIVAL_INFOMATION *sv_info;
extern int PLAYER_HITPOINTMAX;
extern CHARACTER_WORK *PlayerWork;
extern GAME_INFOMATION g_info;

int sv_levelflag = 0;
static const short idtb[] = { 1, 2, 3 };

static CHARACTER_ORIGINAL_STATUS sv_status[12] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 940, -780, 2400, -1700, 900, -750, 110, 85, 2000, 14, 10, 1, 1, 15, 15, 27, 27, 0, 16, 20 },
    { 75, -75, 350, -350, 200, -200, 50, 100, 1000, 10, 8, 4, 4, 1, 1, 75, 75, 5, 22, 20 },
    { 500, -500, 1000, -1000, 500, -500, 50, 100, 1000, 20, 10, 2, 2, 56, 50, 95, 95, 3, 54, 30 },
    { 350, -1320, 1000, -2500, 750, -1500, 50, 100, 2000, 25, 35, 3, 3, 240, 240, 240, 240, 3, 32, 0 },
    { 690, -550, 1624, -1800, 1000, -1000, 68, 70, 1000, 20, 10, 1, 1, 88, 80, 100, 100, 4, 50, 50 },
    { 3700, -3100, 5720, -6600, 1880, -1800, 180, 20, 100, 12, 10, 1, 1, 150, 150, 170, 170, 2, 45, 45 },
    { 560, -440, 1550, -1600, 1000, -1000, 60, 70, 500, 27, 15, 1, 1, 70, 70, 120, 120, 3, 55, 60 },
    { 360, -295, 9999, -9999, 470, -500, 30, 100, 100, 22, 10, 1, 1, 100, 100, 100, 100, 5, 10, 20 },
    { 700, -550, 1600, -1800, 1000, -1000, 80, 50, 100, 10, 5, 1, 1, 100, 100, 100, 100, 5, 70, 50 },
    { 50, -25, 100, -150, 100, -100, 10, 50, 100, 10, 5, 1, 1, 10, 10, 10, 10, 0, 150, 0 },
    { 1700, -1550, 2600, -2800, 1000, -1000, 80, 50, 100, 10, 5, 1, 1, 100, 100, 100, 100, 5, 70, 50 },
};
int bosschk[13] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 };

void *mmalloc(int size, char *name);

void init_survival(int ply)
{
    int i;
    int *p;

    if (sv_info == 0) sv_info = mmalloc(0x68, "sv_info");
    p = (int *)sv_info;
    for (i = 0; i < sizeof(SURVIVAL_INFOMATION) / sizeof(int); i++) *p++ = 0;
    sv_info->ply = ply;
    sv_info->win = 0;
    sv_info->hitpoint = PLAYER_HITPOINTMAX;
    sv_info->time_cnt = 0;
    sv_info->hoimi = 0;
    sv_info->sv_ext.survival_level = 1;
    sv_info->index = 0;
    sv_info->sv_ext.boss_cnt = 0;
    sv_info->sv_ext.boss_flg = 0;
    sv_info->sv_ext.battle = 0;
    sv_info->sv_ext.allclear = 0;
    sv_info->sv_ext.fullcolor_on = 0;
    sv_info->sv_ext.rejene = 0;
    for (i = 9; i >= 0; i--) sv_info->sv_ext.survival_sectiontime[i] = 0;
    sv_levelflag = 0;
    sv_levelup_info(-1);
}

void free(void *aptr);

void end_survival(void)
{
    free(sv_info);
    sv_info = 0;
}

int GetPassBattleTime(void);
extern int RTimeLimitTb[];
extern short int UseCharacter1p2p[];
extern USER_SETTING userinfo;

void sv_hoimi_set(void)
{
    int dat;

    sv_info->hitpoint = PlayerWork[sv_info->ply].HitPoint;
    sv_info->oldhp = sv_info->hitpoint;
    dat = GetPassBattleTime() / g_info.TimeByCount;
    dat = RTimeLimitTb[userinfo.roundtime] - dat;
    if (sv_info->sv_ext.battle) {
        if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 1) {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint == 0) {
                dat += PLAYER_HITPOINTMAX;
            } else {
                dat += PLAYER_HITPOINTMAX / 2;
            }
        } else {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint) {
                dat += PLAYER_HITPOINTMAX / 4;
            } else {
                dat += PLAYER_HITPOINTMAX / 2;
            }
        }
    } else {
        if (g_info.PlayerPalette[sv_info->ply == 0] == 10) {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint) {
                dat += PLAYER_HITPOINTMAX / 4;
            } else {
                dat += PLAYER_HITPOINTMAX / 2;
            }
        } else {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint) {
                dat += 10;
            } else {
                dat += 20;
            }
        }
    }
    sv_info->hitpoint += dat;
    if (sv_info->hitpoint > PLAYER_HITPOINTMAX) sv_info->hitpoint = PLAYER_HITPOINTMAX;
    sv_info->time_cnt = 0;
    sv_info->hoimi = 0;
    sv_info->win++;
    if (sv_info->sv_ext.boss_flg && sv_info->sv_ext.survival_level >= 38) sv_info->hitpoint = PLAYER_HITPOINTMAX;
}

void sv_hoimi(void)
{
    if (PlayerWork[sv_info->ply].HitPoint < sv_info->hitpoint) PlayerWork[sv_info->ply].HitPoint++;
}

void Change_ScreenScale(int i);
void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu);
void SetStageNo(int no);
void SetVsFlag(void);
extern int FileSelected[7];
extern short int UseCharacter1p2p[];
extern int bosschk[];

int InitSurvivalBattle(void)
{
    int ene;
    int id;
    int color;
    int chk;
    int bossid[13] = { 11, 16, 7, 6, 9, 10, 15, 4, 8, 14, 1, 17, 18 };
    char StageTable[18] = { 8, 8, 5, 3, 1, 6, 2, 7, 2, 6, 9, 4, 2, 5, 7, 10, 11, 12 };

    color = 0;
    if (sv_ending_chk(1)) return 1;
    ene = sv_info->ply == 0;
    if (sv_info->index == 0) init_enemytb();
    chk = 0;
    if (sv_info->sv_ext.boss_flg & bosschk[sv_info->sv_ext.boss_cnt]) {
        id = bossid[sv_info->sv_ext.boss_cnt];
        chk = 1;
        sv_info->sv_ext.boss_flg = 0;
        sv_info->sv_ext.boss_cnt++;
        sv_info->sv_ext.battle = 1;
        color = 9;
        if (sv_info->sv_ext.survival_level >= 40) sv_info->hitpoint = PLAYER_HITPOINTMAX;
        FileSelected[0] = -1;
        FileSelected[1] = -1;
    } else {
        id = sv_info->enetb[sv_info->index];
        if (id < 1 || id > 16) {
            init_enemytb();
            sv_info->index = 0;
            id = sv_info->enetb[0];
        }
        sv_info->sv_ext.battle = 0;
    }
    if (id == 0) init_enemytb();
    Change_ScreenScale(1);
    if (chk == 0) {
        color = 6;
        if (sv_info->sv_ext.survival_level > 40) {
            color = 7;
            if (sv_info->sv_ext.survival_level > 70) {
                color = 8;
                if (sv_info->sv_ext.survival_level > 99) color = get_rarecolor(id);
            }
        }
        if (UseCharacter1p2p[ene ? 0 : 1] == id) {
            if (g_info.PlayerPalette[!ene] == color) color = 5;
        }
    }
    EntrySelectCharacter(ene, id, color, 1);
    if (id == 4 && sv_info->sv_ext.battle) {
        SetStageNo(StageTable[7]);
    } else {
        SetStageNo(StageTable[UseCharacter1p2p[ene] - 1]);
    }
    SetVsFlag();
    if (sv_info->sv_ext.battle == 0) sv_info->index++;
    if (sv_info->index == get_maxchara()) sv_info->index = 0;
    sv_set_newcolor(id, color);
    sv_levelup_info(-2);
    return 0;
}

int Random(void);
extern USER_SETTING userinfo;

int get_rarecolor(int id)
{
    int color;
    int i;

    color = 0;
    for (i = 6; i < 9; i++) {
        if (!((userinfo.usecol[id - 1] >> i) & 1)) {
            color = i;
            break;
        }
    }
    if (color == 0) {
        if (!chk_complete2()) {
            do {
                color = (Random() & 3) + 6;
            } while (color >= 9);
        } else {
            color = 10;
        }
    }
    return color;
}

extern USER_SETTING userinfo;

int chk_complete2(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        if (userinfo.usecol[i] < 0x3FF) return 0;
    }
    return 1;
}

extern USER_SETTING userinfo;

void sv_set_newcolor(int id, int color)
{
    if (color != 9) userinfo.usecol[id - 1] |= 1 << color;
}

extern USER_SETTING userinfo;

int chk_select_color(int id, int color)
{
    return userinfo.usecol[id - 1] & (1 << color);
}

int Random(void);

int get_select_color(int id)
{
    int col;

    while (1) {
        col = Random() & 0xF;
        if (col >= 11) continue;
        if (col >= 9 && !(Random() & 1)) continue;
        if (chk_select_color(id, col)) break;
    }
    return col;
}

extern PAD pads[2];

int sv_ending_chk(int flg)
{
    int ret = 0;

    if (flg) {
        if (sv_info->sv_ext.allclear >= 2) return 0;
    }
    if (sv_info->sv_ext.boss_cnt >= 9 && sv_info->sv_ext.survival_level >= 100) {
        if (flg) {
            if (pads[sv_info->ply].data == 0xC) {
                sv_info->sv_ext.allclear = 2;
                ret = 0;
            } else {
                ret = 1;
            }
        } else {
            ret = 1;
        }
    }
    return ret;
}

int Random(void);

void init_enemytb(void)
{
    int i;
    int n;
    int max;
    int id;
    int ret;

    max = get_maxchara();
    for (i = 0; i < 16; i++) sv_info->enetb[i] = 0;
    for (i = 0; i < max; i++) {
        do {
            do {
                id = (Random() & 0xF) + 1;
            } while (!((max == 14 && id != 11 && id != 16) || (max == 16 && (!(i < 8) || (id != 11 && id != max)))));
            ret = 0;
            for (n = 0; n < i; n++) {
                if (sv_info->enetb[n] == id) {
                    ret = 1;
                    break;
                }
            }
            if (n == 0 && sv_info->enetb[max - 1] == id) ret = 1;
        } while (ret);
        sv_info->enetb[i] = id;
    }
}

extern USER_SETTING userinfo;

int get_maxchara(void)
{
    int max;

    if (userinfo.phase >= 3) {
        max = 16;
    } else {
        max = (sv_info->sv_ext.survival_level < 40) ? 14 : 16;
    }
    return max;
}

extern short int UseCharacter1p2p[];
extern int BattleStatus;

void chk_survival_stat(void)
{
    chk_survivallevel();
    sv_set_sectiontime();
    if (sv_info->sv_ext.battle && UseCharacter1p2p[sv_info->ply ? 0 : 1] == 10) {
        (sv_info->ply ? PlayerWork : PlayerWork + 1)->ply->Tension = 10000;
    }
    if (sv_info->sv_ext.battle && UseCharacter1p2p[sv_info->ply ? 0 : 1] == 1) {
        (sv_info->ply ? PlayerWork : PlayerWork + 1)->ply->Tension = 10000;
        if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->actst & 0x20) sv_info->sv_ext.rejene = 300;
        if (BattleStatus == 2 && sv_info->sv_ext.survival_totaltime % 10 == 0) {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint > 0 && (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint < PLAYER_HITPOINTMAX && sv_info->sv_ext.rejene == 0) {
                (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint++;
            }
        }
        if (sv_info->sv_ext.rejene > 0) sv_info->sv_ext.rejene--;
    }
    if (sv_info->sv_ext.battle && UseCharacter1p2p[sv_info->ply ? 0 : 1] == 17) {
        (sv_info->ply ? PlayerWork : PlayerWork + 1)->ply->Tension = 10000;
        if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->actst & 0x20) sv_info->sv_ext.rejene = 300;
        if (BattleStatus == 2 && sv_info->sv_ext.survival_totaltime % 10 == 0) {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint > 0 && (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint < PLAYER_HITPOINTMAX && sv_info->sv_ext.rejene == 0) {
                (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint++;
            }
        }
        if (sv_info->sv_ext.rejene > 0) sv_info->sv_ext.rejene--;
    }
    if (sv_info->sv_ext.battle && UseCharacter1p2p[sv_info->ply ? 0 : 1] == 18) {
        (sv_info->ply ? PlayerWork : PlayerWork + 1)->ply->Tension = 10000;
        if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->actst & 0x20) sv_info->sv_ext.rejene = 300;
        if (BattleStatus == 2 && sv_info->sv_ext.survival_totaltime % 10 == 0) {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint > 0 && (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint < PLAYER_HITPOINTMAX && sv_info->sv_ext.rejene == 0) {
                (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint++;
            }
        }
        if (sv_info->sv_ext.rejene > 0) sv_info->sv_ext.rejene--;
    }
    if (g_info.PlayerPalette[sv_info->ply == 0] == 10) {
        (sv_info->ply ? PlayerWork : PlayerWork + 1)->ply->Tension = 10000;
    }
}

int RefereeWorkStop(void);
void sv_IntIrq(void);
extern int bosschk[];
extern USER_SETTING userinfo;
extern int BattleStatus;

void chk_survivallevel(void)
{
    int chklvl[14] = { 20, 30, 40, 50, 60, 70, 80, 90, 99, 120, 150, 200, 300, -1 };
    int i;

    sv_levelup_info(0);
    if (sv_info->sv_ext.survival_level < 20) {
        sv_info->sv_ext.survival_nextexp = sv_info->sv_ext.survival_level * 10 + 100;
    } else if (sv_info->sv_ext.survival_level < 50) {
        sv_info->sv_ext.survival_nextexp = sv_info->sv_ext.survival_level + 300;
    } else if (sv_info->sv_ext.survival_level < 70) {
        sv_info->sv_ext.survival_nextexp = 300 + sv_info->sv_ext.survival_level + sv_info->sv_ext.survival_level / 2;
    } else {
        sv_info->sv_ext.survival_nextexp = sv_info->sv_ext.survival_level * 2 + 300;
    }
    if (sv_info->sv_ext.survival_level == 99 && sv_info->sv_ext.battle == 0) {
        sv_info->sv_ext.survival_exp = 0;
    } else if (sv_info->sv_ext.boss_cnt + 1 < 13 && chklvl[sv_info->sv_ext.boss_cnt + 1] > 0 && BattleStatus == 1
               && sv_info->sv_ext.survival_level == chklvl[sv_info->sv_ext.boss_cnt + 1] - 1) {
        sv_info->sv_ext.survival_exp = 0;
    } else if (sv_info->sv_ext.survival_level == 999) {
        sv_info->sv_ext.survival_exp = 0;
    } else if (sv_info->sv_ext.survival_exp >= sv_info->sv_ext.survival_nextexp
               || (sv_info->sv_ext.survival_level == 99 && sv_info->sv_ext.allclear)) {
        sv_info->sv_ext.survival_level++;
        if (sv_info->sv_ext.survival_level != 100) {
            if (sv_info->sv_ext.survival_level != 99) {
                sv_info->sv_ext.survival_exp -= sv_info->sv_ext.survival_nextexp;
            } else {
                sv_info->sv_ext.survival_exp = 0;
            }
        } else {
            sv_info->sv_ext.res_level = sv_info->sv_ext.survival_level;
            sv_info->sv_ext.res_stage = sv_info->win;
            sv_info->sv_ext.survival_exp = 0;
            if (sv_info->sv_ext.survival_totaltime < 36000) {
                sv_info->sv_ext.fullcolor_on = 1;
                for (i = 1; i <= 18; i++) userinfo.usecol[18 - i] = 0x7FF;
            }
            for (i = 0; i < 18; i++) userinfo.usecol[i] |= 0x200;
            if (userinfo.phase < 3) userinfo.phase = 3;
        }
        sv_levelup(sv_info->sv_ext.survival_level);
        sv_levelup_info(1);
        if (sv_info->sv_ext.survival_level == chklvl[sv_info->sv_ext.boss_cnt]) {
            if (sv_info->sv_ext.survival_level < 100 || chk_complete2()) {
                if (sv_info->sv_ext.survival_level >= 40) sv_info->hitpoint = PLAYER_HITPOINTMAX;
                sv_info->sv_ext.boss_flg |= bosschk[sv_info->sv_ext.boss_cnt];
                BattleStatus = 1;
                RefereeWorkStop();
                sv_IntIrq();
            }
        }
    }
}

void sv_set_sectiontime(void)
{
    int section;
    int total;
    int i;

    if (sv_info->sv_ext.survival_level < 100) {
        section = sv_info->sv_ext.survival_level / 10;
        if (section == 0) {
            sv_info->sv_ext.survival_sectiontime[0] = sv_info->sv_ext.survival_totaltime;
            return;
        }
        total = 0;
        for (i = 0; i < section; i++) total += sv_info->sv_ext.survival_sectiontime[i];
        sv_info->sv_ext.survival_sectiontime[section] = sv_info->sv_ext.survival_totaltime - total;
    }
}

extern short int SV_ADD_DAMAGE[];
extern short int UseCharacter1p2p[];

int sv_damage_hosei(CHARACTER_WORK *offset, int damage)
{
    float rate;
    int dam;
    int lvl;
    int exp;
    int now;

    lvl = sv_info->sv_ext.survival_level - 1;
    if (lvl <= 0) lvl = 0;
    if (offset->padid == sv_info->ply) {
        if (sv_info->sv_ext.battle && sv_info->sv_ext.survival_level >= 40) {
            if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 14) {
                if (chk_nikudan()) {
                    rate = sv_status[sv_info->sv_ext.boss_cnt - 2].add_damage;
                } else {
                    rate = 100.0f;
                }
            } else {
                rate = sv_status[sv_info->sv_ext.boss_cnt - 2].add_damage;
            }
        } else if (sv_info->sv_ext.survival_level >= 100) {
            rate = sv_info->sv_ext.survival_level - 50;
        } else {
            rate = SV_ADD_DAMAGE[lvl];
        }
        damage += (int)((float)damage * (rate / 100.0));
        if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 14 && sv_info->sv_ext.battle) {
            if (chk_iai()) damage = PLAYER_HITPOINTMAX * 2;
        }
        now = sv_info->sv_ext.survival_exp - (int)((float)damage * 0.05);
        if (now <= 0) {
            sv_info->sv_ext.survival_exp = 0;
        } else {
            sv_info->sv_ext.survival_exp = now;
        }
        if (g_info.PlayerPalette[sv_info->ply == 0] == 10) {
            if ((sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint > 0 && (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint < PLAYER_HITPOINTMAX) {
                (sv_info->ply ? PlayerWork : PlayerWork + 1)->HitPoint++;
            }
        }
        return damage;
    } else {
        if (sv_info->sv_ext.battle && sv_info->sv_ext.survival_level >= 40) {
            rate = sv_status[sv_info->sv_ext.boss_cnt - 2].throw_damage;
        } else {
            rate = (sv_info->sv_ext.survival_level < 100) ? sv_info->sv_ext.survival_level / 2 : sv_info->sv_ext.survival_level - 100;
            if (rate > 50.0f) rate = 50.0f;
            if (g_info.PlayerPalette[sv_info->ply == 0] == 10) {
                rate = 35.0f;
                if (sv_info->sv_ext.survival_level >= 150) rate = 50.0f;
            }
        }
        damage -= (int)((float)damage * (rate / 100.0));
        if (damage == 0) damage = 1;
        if (sv_info->sv_ext.survival_level != 99) {
            exp = damage;
            if (sv_info->sv_ext.survival_level >= 40 && ((sv_info->sv_ext.battle & 0xFF) & 1)) {
                exp = damage * 4;
            } else if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 11) {
                exp = damage * 2;
            } else if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 16) {
                exp = damage * 3;
            }
            if (g_info.PlayerPalette[sv_info->ply == 0] == 10) exp *= 4;
            if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 1 && sv_info->sv_ext.battle) exp *= 5;
            if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 14 && sv_info->sv_ext.battle) exp *= 5;
            if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 17 && sv_info->sv_ext.battle) exp *= 5;
            if (UseCharacter1p2p[sv_info->ply ? 0 : 1] == 18 && sv_info->sv_ext.battle) exp *= 5;
            exp += add_exp_combo((sv_info->ply ? PlayerWork : PlayerWork + 1)->ply->HitCount);
            exp += add_exp_damage(damage);
            exp += sv_info->sv_ext.survival_level / 10;
            sv_info->sv_ext.survival_exp += exp;
        }
        if (PlayerWork[sv_info->ply].HitPoint > 0) {
            if (sv_info->sv_ext.survival_level < 10) {
                rate = 0.05f;
            } else if (sv_info->sv_ext.survival_level < 20) {
                rate = 0.07f;
            } else if (sv_info->sv_ext.survival_level < 30) {
                rate = 0.09f;
            } else if (sv_info->sv_ext.survival_level < 40) {
                rate = 0.11f;
            } else if (sv_info->sv_ext.survival_level < 50) {
                rate = 0.13f;
            } else if (sv_info->sv_ext.survival_level < 60) {
                rate = 0.15f;
            } else if (sv_info->sv_ext.survival_level < 70) {
                rate = 0.17f;
            } else if (sv_info->sv_ext.survival_level < 80) {
                rate = 0.19f;
            } else if (sv_info->sv_ext.survival_level < 90) {
                rate = 0.21f;
            } else if (sv_info->sv_ext.survival_level < 100) {
                rate = 0.23f;
            } else {
                rate = 0.25f;
            }
            if (g_info.PlayerPalette[sv_info->ply == 0] == 10) rate = 1.0f;
            dam = damage * rate;
            if (dam == 0) dam = 1;
            PlayerWork[sv_info->ply].HitPoint += dam;
            if (PlayerWork[sv_info->ply].HitPoint > PLAYER_HITPOINTMAX) PlayerWork[sv_info->ply].HitPoint = PLAYER_HITPOINTMAX;
        }
        return damage;
    }
}

int chk_iai(void)
{
    if (PlayerWork[sv_info->ply].actst & 0x4000) {
        PlayerWork[sv_info->ply].DamActHeader[0].flag &= ~0x20000000;
    }
    if (PlayerWork[sv_info->ply].DamActHeader[0].flag & 0x20000000) return 1;
    return 0;
}

int chk_nikudan(void)
{
    TACTHEADER *header;
    int i;
    int ret = 0;

    for (i = 0; i < 3; i++) {
        header = &PlayerWork[sv_info->ply].DamActHeader[i];
        if (header->flag & 1) {
            ret = 1;
            break;
        }
    }
    return ret;
}

int add_exp_damage(int damage)
{
    int plus; if (damage >= 40) plus = damage / 10 + damage % 10; return damage;
}

void sv_set_cpulvl(int lvl);

void sv_levelup(int lvl)
{
    if (sv_info->sv_ext.battle) {
        if (sv_info->sv_ext.survival_level < 30) {
            sv_set_cpulvl(12);
        } else if (sv_info->sv_ext.survival_level < 40) {
            sv_set_cpulvl(15);
        } else if (sv_info->sv_ext.survival_level < 50) {
            sv_set_cpulvl(20);
        } else if (sv_info->sv_ext.survival_level < 60) {
            sv_set_cpulvl(1);
        } else if (sv_info->sv_ext.survival_level < 70) {
            sv_set_cpulvl(24);
        } else if (sv_info->sv_ext.survival_level < 80) {
            sv_set_cpulvl(26);
        } else if (sv_info->sv_ext.survival_level < 90) {
            sv_set_cpulvl(28);
        } else if (sv_info->sv_ext.survival_level < 99) {
            sv_set_cpulvl(30);
        } else if (sv_info->sv_ext.survival_level == 99) {
            sv_set_cpulvl(32);
        } else {
            sv_set_cpulvl(32);
        }
    } else {
        sv_set_cpulvl(lvl / 4);
    }
    sv_set_enemy_status(sv_info->ply == 0);
}

extern short int BackJump_XSpeedByIdno[];
extern short int BackStep_XSpeedByIdno[];
extern short int BackWalk_XSpeedByIdno[];
extern short int Dash_AccelByIdno[];
extern short int Dash_ResistByIdno[];
extern short int Dash_XSpeedByIdno[];
extern short int ForwardWalk_XSpeedByIdno[];
extern short int FrontJump_XSpeedByIdno[];
extern short int PDKonjo[];
extern short int PDSkyDashB_AttackTime[];
extern short int PDSkyDashB_Time[];
extern short int PDSkyDashF_AttackTime[];
extern short int PDSkyDashF_Time[];
extern short int PDTensionAdd_FDash[];
extern short int PDTensionAdd_FJump[];
extern short int PDTensionAdd_FSDash[];
extern short int PDTensionAdd_FWalk[];
extern short int SkyDash_Height[];

void sv_save_enemystatus(int side)
{
    sv_status[0].fwalk_speed = ForwardWalk_XSpeedByIdno[side];
    sv_status[0].bwalk_speed = BackWalk_XSpeedByIdno[side];
    sv_status[0].fdash_speed = Dash_XSpeedByIdno[side];
    sv_status[0].bdash_speed = BackStep_XSpeedByIdno[side];
    sv_status[0].fjump_speed = FrontJump_XSpeedByIdno[side];
    sv_status[0].bjump_speed = BackJump_XSpeedByIdno[side];
    sv_status[0].fdash_accel = Dash_AccelByIdno[side];
    sv_status[0].fdash_resist = Dash_ResistByIdno[side];
    sv_status[0].sdash_height = SkyDash_Height[side];
    sv_status[0].sfdash_time = PDSkyDashF_Time[side];
    sv_status[0].sbdash_time = PDSkyDashB_Time[side];
    sv_status[0].sfdash_time_noattack = PDSkyDashF_AttackTime[side];
    sv_status[0].sbdash_tiem_noattack = PDSkyDashB_AttackTime[side];
    sv_status[0].fwalk_tensionup = PDTensionAdd_FWalk[side];
    sv_status[0].fjump_tensionup = PDTensionAdd_FJump[side];
    sv_status[0].fdash_tensionup = PDTensionAdd_FDash[side];
    sv_status[0].sfdash_tensionup = PDTensionAdd_FSDash[side];
    sv_status[0].konp = PDKonjo[side];
}

extern short int BackJump_XSpeedByIdno[];
extern short int BackStep_XSpeedByIdno[];
extern short int BackWalk_XSpeedByIdno[];
extern short int Dash_AccelByIdno[];
extern short int Dash_ResistByIdno[];
extern short int Dash_XSpeedByIdno[];
extern short int ForwardWalk_XSpeedByIdno[];
extern short int FrontJump_XSpeedByIdno[];
extern short int PDKonjo[];
extern short int PDSkyDashB_AttackTime[];
extern short int PDSkyDashB_Time[];
extern short int PDSkyDashF_AttackTime[];
extern short int PDSkyDashF_Time[];
extern short int PDTensionAdd_FDash[];
extern short int PDTensionAdd_FJump[];
extern short int PDTensionAdd_FSDash[];
extern short int PDTensionAdd_FWalk[];
extern short int SkyDash_Height[];


void sv_set_enemy_status(int side)
{
    if (sv_info->sv_ext.battle == 0 || sv_info->sv_ext.survival_level < 40) {
        ForwardWalk_XSpeedByIdno[side] = sv_status[0].fwalk_speed + sv_info->sv_ext.survival_level;
        BackWalk_XSpeedByIdno[side] = sv_status[0].bwalk_speed - sv_info->sv_ext.survival_level;
        Dash_XSpeedByIdno[side] = sv_status[0].fdash_speed + sv_info->sv_ext.survival_level;
        BackStep_XSpeedByIdno[side] = sv_status[0].bdash_speed - sv_info->sv_ext.survival_level;
        FrontJump_XSpeedByIdno[side] = sv_status[0].fjump_speed + sv_info->sv_ext.survival_level;
        BackJump_XSpeedByIdno[side] = sv_status[0].bjump_speed - sv_info->sv_ext.survival_level;
        Dash_AccelByIdno[side] = sv_status[0].fdash_accel + sv_info->sv_ext.survival_level;
        Dash_ResistByIdno[side] = sv_status[0].fdash_resist + sv_info->sv_ext.survival_level;
        SkyDash_Height[side] = sv_status[0].sdash_height - sv_info->sv_ext.survival_level * 50;
        PDSkyDashF_Time[side] = sv_status[0].sfdash_time + sv_info->sv_ext.survival_level / 5;
        PDSkyDashB_Time[side] = sv_status[0].sbdash_time + sv_info->sv_ext.survival_level / 5;
        PDSkyDashF_AttackTime[side] = sv_status[0].sfdash_time_noattack - sv_info->sv_ext.survival_level / 10;
        if (PDSkyDashF_AttackTime[side] <= 0) PDSkyDashF_AttackTime[side] = 0;
        PDSkyDashB_AttackTime[side] = sv_status[0].sbdash_tiem_noattack - sv_info->sv_ext.survival_level / 10;
        if (PDSkyDashB_AttackTime[side] <= 0) PDSkyDashB_AttackTime[side] = 0;
        PDTensionAdd_FWalk[side] = sv_status[0].fwalk_tensionup + sv_info->sv_ext.survival_level / 10;
        PDTensionAdd_FJump[side] = sv_status[0].fjump_tensionup + sv_info->sv_ext.survival_level / 10;
        PDTensionAdd_FDash[side] = sv_status[0].fdash_tensionup + sv_info->sv_ext.survival_level / 10;
        PDTensionAdd_FSDash[side] = sv_status[0].sfdash_tensionup + sv_info->sv_ext.survival_level / 10;
        PDKonjo[side] = sv_status[0].konp;
    } else {
        ForwardWalk_XSpeedByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fwalk_speed;
        BackWalk_XSpeedByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].bwalk_speed;
        Dash_XSpeedByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fdash_speed;
        BackStep_XSpeedByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].bdash_speed;
        FrontJump_XSpeedByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fjump_speed;
        BackJump_XSpeedByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].bjump_speed;
        Dash_AccelByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fdash_accel;
        Dash_ResistByIdno[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fdash_resist;
        SkyDash_Height[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].sdash_height;
        PDSkyDashF_Time[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].sfdash_time;
        PDSkyDashB_Time[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].sbdash_time;
        PDSkyDashF_AttackTime[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].sfdash_time_noattack;
        PDSkyDashB_AttackTime[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].sbdash_tiem_noattack;
        PDTensionAdd_FWalk[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fwalk_tensionup;
        PDTensionAdd_FJump[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fjump_tensionup;
        PDTensionAdd_FDash[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].fdash_tensionup;
        PDTensionAdd_FSDash[side] = sv_status[sv_info->sv_ext.boss_cnt - 2].sfdash_tensionup;
        PDKonjo[side] = sv_status[sv_info->sv_ext.boss_cnt - 1].konp;
    }
}

int sound_request(int no);

void sv_levelup_info(int sw)
{
    int se;
    static int num = -1;

    if (sw > 0) {
        num = 0;
    } else if (sw < -1) {
        num = -1;
    } else if (num != -1) {
        sv_levelup_info2(num);
        if (num == 0) {
            sound_request(0x31);
        } else if (num == 8) {
            sound_request(0x31);
        } else if (num == 24) {
            sound_request(0x31);
        } else if (num == 31) {
            sound_request(0x31);
        } else if (num == 32) {
            if (sv_info->sv_ext.survival_level == 99) {
                se = 0x5C;
            } else {
                se = (sv_info->sv_ext.survival_level >= 40) ? 0x4E : 0x60;
            }
            sound_request(se);
        }
        num++;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
char *G_sprintf(char *buff, char *str, int *valtb);

void sv_levelup_info2(int num)
{
    int j;
    int val[2];
    char *dat = "SURVIVAL LEVEL";
    static char buf[16];

    if (num < 14) {
        for (j = 0; j <= num; j++) buf[num] = dat[num];
        buf[j] = 0;
        FontDrawDC(buf, 192.0f, 208.0f, 267.0f, 1, 1);
    } else if (num - 14 < 120) {
        FontDrawDC(buf, 192.0f, 208.0f, 267.0f, 1, 1);
        val[0] = sv_info->sv_ext.survival_level;
        val[1] = 0;
        FontDrawDC(G_sprintf(0, "%3d", val), 448.0f, 208.0f, 267.0f, 1, 1);
    }
}

int FileReadCheck(void);
char *FileReadSelect(int fid, int chan);
void JobNext(void);
void heap_clear(void);
extern char *OpeningFile;
extern int SubJob;

void SvResultInit(void)
{
    if (OpeningFile == 0) {
        OpeningFile = FileReadSelect(0xA1, 0);
    } else if (FileReadCheck()) {
        heap_clear();
        sv_result_init();
        set_clearflg_sv();
        sv_save_result();
        result_disp(0);
        SubJob = 0;
        JobNext();
    }
}

void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
int IsFadeEnd(void);
void JobChange(int job);
void sound_alloff(void);
extern PAD pads[2];
extern int SubJob;
extern USER_SETTING userinfo;
extern SYS_COND sc;

void SvResultMain(void)
{
    if (SubJob == 0) {
        if ((pads[0].trg | pads[1].trg) & 0x800) SubJob = 1;
        result_disp(1);
        EasySpriteDrawEX(0x30, 0, 480, 20.0f, 0x20D, 1.0f);
    }
    if (SubJob == 1) {
        if (IsFadeEnd() == 0) {
            result_disp(1);
            EasySpriteDrawEX(0x30, 0, 480, 20.0f, 0x20D, 1.0f);
        } else {
            SubJob++;
        }
    }
    if (SubJob == 2) {
        sound_alloff();
        if (userinfo.autosave) {
            sc.grpvar[1] = -1;
            JobChange(0x2D);
        } else {
            JobChange(3);
        }
    }
}

extern PAD pads[2];

void result_disp(int flg)
{
    static int time = 0;

    if (flg) {
        result_bg_mask();
        result_title(time);
        result_player(time);
        result_level(time);
        result_stage(time);
        result_time(time);
        time++;
        if ((pads[0].trg | pads[1].trg) & 0x20) time = 450;
    } else {
        time = 0;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);

void result_title(int time)
{
    static short alpha = 255;

    if (time >= 10) {
        alpha -= 4;
        if (alpha <= 0) alpha = 1;
        FontDrawDC("RESULT SURVIVAL", 32.0f, 40.0f, 4.0f, alpha, 0);
    } else {
        alpha = 255;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void *memcpy(void *dst, const void *src, unsigned int n);
extern short int UseCharacter1p2p[];

void result_player(int time)
{
    static short alpha = 255;
    int len = 0;
    int off = 0;
    char name[16];

    if (time >= 120) {
        switch (UseCharacter1p2p[sv_info->ply]) {
        case 1:
            memcpy(name, "SOL BADGUY", 16);
            len = 10;
            off = -3;
            break;
        case 2:
            memcpy(name, "KY KISKE", 16);
            len = 8;
            off = 5;
            break;
        case 3:
            memcpy(name, "MAY", 16);
            len = 3;
            off = 25;
            break;
        case 4:
            memcpy(name, "MILLIA RAGE", 16);
            len = 11;
            off = -6;
            break;
        case 5:
            memcpy(name, "AXL LOW", 16);
            len = 7;
            off = 8;
            break;
        case 6:
            memcpy(name, "POTEMKIN", 16);
            len = 8;
            off = 5;
            break;
        case 7:
            memcpy(name, "CHIPP ZANUFF", 16);
            len = 12;
            off = -10;
            break;
        case 8:
            memcpy(name, "ZATO-ONE", 16);
            len = 8;
            off = 5;
            break;
        case 9:
            memcpy(name, "BAIKEN", 16);
            len = 6;
            off = 13;
            break;
        case 10:
            memcpy(name, "FAUST", 16);
            len = 5;
            off = 17;
            break;
        case 11:
            memcpy(name, "TESTAMENT", 16);
            len = 9;
            off = 1;
            break;
        case 12:
            memcpy(name, "JAM KURADOBERI", 16);
            len = 14;
            off = -18;
            break;
        case 13:
            memcpy(name, "ANJI MITO", 16);
            len = 9;
            off = 1;
            break;
        case 14:
            memcpy(name, "JOHNNY", 16);
            len = 6;
            off = 14;
            break;
        case 15:
            memcpy(name, "VENOM", 16);
            len = 5;
            off = 17;
            break;
        case 16:
            memcpy(name, "DIZZY", 16);
            len = 5;
            off = 17;
            break;
        case 17:
            memcpy(name, "KLIFF", 16);
            len = 5;
            off = 17;
            break;
        case 18:
            memcpy(name, "JUSTICE", 16);
            len = 7;
            off = 8;
            break;
        }
        alpha -= 4;
        if (alpha <= 0) alpha = 1;
        FontDrawDC("PLAYER", 64.0f, 96.0f, 4.0f, alpha, 1);
        FontDrawDC(name, off + (480 - len * 8), 96.0f, 4.0f, alpha, 2);
    } else {
        alpha = 255;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
char *G_sprintf(char *buff, char *str, int *valtb);

void result_level(int time)
{
    static short alpha = 255;
    int val[2];

    if (time >= 150) {
        alpha -= 4;
        if (alpha <= 0) alpha = 1;
        FontDrawDC("SURVIVAL LEVEL", 64.0f, 128.0f, 4.0f, alpha, 1);
        val[0] = sv_info->sv_ext.survival_level;
        FontDrawDC(G_sprintf(0, "%3d", val), 466.0f, 128.0f, 4.0f, alpha, 2);
        if (sv_info->sv_ext.allclear) FontDrawDC("(ALL)", 534.0f, 128.0f, 4.0f, alpha, 2);
    } else {
        alpha = 255;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
char *G_sprintf(char *buff, char *str, int *valtb);

void result_stage(int time)
{
    static short alpha = 255;
    int val[2];

    if (time >= 180) {
        alpha -= 4;
        if (alpha <= 0) alpha = 1;
        FontDrawDC("LAST STAGE", 64.0f, 160.0f, 4.0f, alpha, 1);
        val[0] = sv_info->win + 1;
        FontDrawDC(G_sprintf(0, "%3d", val), 464.0f, 160.0f, 4.0f, alpha, 2);
    } else {
        alpha = 255;
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
void FontDraw(char *str, int x, int y, float z, int alfa);
void disp_koron(int x, int y);
void *memcpy(void *dst, const void *src, unsigned int n);

void result_time(int time)
{
    static short alpha[2] = { 255, 255 };
    static short salpha[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char buf[16];
    int i;
    char *res[10] = {
        " 1 - 10 : ", "10 - 20 :", "20 - 30 :", "30 - 40 :", "40 - 50 :",
        "50 - 60 : ", "60 - 70 :", "70 - 80 :", "80 - 90 :", "90 -100 :",
    };

    if (time >= 240) {
        alpha[0] -= 4;
        if (alpha[0] <= 0) alpha[0] = 1;
        FontDrawDC("SECTION TIME", 64.0f, 208.0f, 4.0f, alpha[0], 1);
    } else {
        alpha[0] = 255;
    }
    for (i = 0; i < 5; i++) {
        if (time >= i * 10 + 300) {
            salpha[i] += 4;
            if (salpha[i] >= 256) salpha[i] = 255;
            FontDraw(res[i], 96, i * 32 + 256, 4.0f, salpha[i]);
            if (sv_info->sv_ext.survival_sectiontime[i] == 0 && sv_info->sv_ext.survival_level < i * 10 + 1) {
                memcpy(buf, "--------", 16);
            } else {
                time_convert(buf, sv_info->sv_ext.survival_sectiontime[i], 1);
            }
            FontDraw(buf, 224, i * 32 + 256, 4.0f, salpha[i]);
            if (buf[0] != '-') disp_koron(224, i * 32 + 256);
        } else {
            salpha[i] = 0;
        }
    }
    for (i = 5; i < 10; i++) {
        if (time >= i * 10 + 300) {
            salpha[i] += 4;
            if (salpha[i] >= 256) salpha[i] = 255;
            FontDraw(res[i], 352, (i - 5) * 32 + 256, 4.0f, salpha[i]);
            if (sv_info->sv_ext.survival_sectiontime[i] == 0 && sv_info->sv_ext.survival_level < i * 10 + 1) {
                memcpy(buf, "--------", 16);
            } else {
                time_convert(buf, sv_info->sv_ext.survival_sectiontime[i], 1);
            }
            FontDraw(buf, 480, (i - 5) * 32 + 256, 4.0f, salpha[i]);
            if (buf[0] != '-') disp_koron(480, (i - 5) * 32 + 256);
        } else {
            salpha[i] = 0;
        }
    }
    if (time >= 450) {
        alpha[1] -= 4;
        if (alpha[1] <= 0) alpha[1] = 1;
        FontDrawDC("TOTAL TIME", 64.0f, 428.0f, 4.0f, alpha[1], 1);
        if (sv_info->sv_ext.survival_totaltime == 0 && sv_info->sv_ext.survival_level == 0) {
            memcpy(buf, "--------", 16);
        } else {
            time_convert(buf, sv_info->sv_ext.survival_totaltime, 1);
        }
        FontDrawDC(buf, 384.0f, 428.0f, 4.0f, alpha[1], 2);
    } else {
        alpha[1] = 255;
    }
}

void *memset(void *dst, int c, unsigned int n);
void *memcpy(void *dst, const void *src, unsigned int n);
void set_char2num(int num, char *block, int id);

void time_convert(char *buf, u_int time, int id)
{
    int data;

    memset(buf, 0, 16);
    if (time == 0 && id) {
        memcpy(buf, "00:00:00", 16);
        return;
    }
    data = time / 216000;
    set_char2num(data, buf, 2);
    buf[2] = ':';
    if (time > 215999) time -= time / 216000 * 216000;
    data = time / 3600;
    set_char2num(data, buf + 3, 2);
    buf[5] = ':';
    if (time >= 3600) time -= time / 3600 * 3600;
    set_char2num(time / 60, buf + 6, 2);
    buf[8] = 0;
}

void sv_timehosei(void)
{
    int i;
    u_int time;

    time = sv_info->sv_ext.survival_totaltime;
    sv_info->sv_ext.survival_totaltime = 0;
    for (i = 0; i < 10; i++) {
        if (sv_info->sv_ext.survival_sectiontime[i]) {
            sv_info->sv_ext.survival_sectiontime[i] = sv_info->sv_ext.survival_sectiontime[i] / 60 * 60;
            sv_info->sv_ext.survival_totaltime += sv_info->sv_ext.survival_sectiontime[i];
        }
    }
    if (sv_info->sv_ext.survival_level >= 100) sv_info->sv_ext.survival_totaltime = time;
}

int Random(void);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);

void result_bg_mask(void)
{
    static RGBINFO rgb = { 255, 0, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 } } };

    rgb.alpha--;
    if (rgb.alpha < 176) rgb.alpha = 176;
    do {
        rgb.id = Random() & 3;
    } while (rgb.id >= 3);
    if (rgb.cnt > 10) {
        if (rgb.rgb[rgb.id].flg) {
            rgb.rgb[rgb.id].col--;
        } else {
            rgb.rgb[rgb.id].col++;
        }
        if (rgb.rgb[rgb.id].col <= 0) {
            rgb.rgb[rgb.id].col = 0;
            rgb.rgb[rgb.id].flg ^= 1;
        } else if (rgb.rgb[rgb.id].col >= 24) {
            rgb.rgb[rgb.id].flg ^= 1;
        }
        rgb.cnt = 0;
    }
    rgb.cnt++;
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 16.0f,
                  gStripGetFadeColor((rgb.alpha << 24) | (rgb.rgb[0].col << 16) | (rgb.rgb[2].col << 8) | rgb.rgb[1].col));
}

void BGM_Request(int no, int loop);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void FadeInit(void);
int IST_Load(int *texadr, int sprid, int palid);
void InitCancelFlag(void);
void LinkAddrTb(u_int *addr);
void PXF_SpriteClear(void);
void dcfont_init(void);
extern char *OpeningFile;

void sv_result_init(void)
{
    u_int *p;
    int bgm;

    PXF_SpriteClear();
    FadeInit();
    p = (u_int *)OpeningFile;
    LinkAddrTb(p);
    IST_Load((int *)(p + 0x8B), 0x30, -1);
    dcfont_init();
    CockpitLoadInit();
    CreditFontLoad(20);
    InitCancelFlag();
    if (sv_ending_chk(0)) {
        bgm = 0x1F;
        if (sv_info->sv_ext.survival_level >= 100) bgm = (sv_info->sv_ext.allclear >= 2) ? 0x22 : 0x1F;
    } else {
        bgm = 0x1E;
    }
    BGM_Request(bgm, 1);
}

int SetLimitStatus(void);
extern USER_SETTING userinfo;

void set_clearflg_sv(void)
{
    int i;

    if (userinfo.phase < 5) {
        if (sv_info->sv_ext.boss_cnt >= 13 || (sv_info->sv_ext.boss_cnt == 12 && sv_info->sv_ext.battle == 0)) userinfo.phase = 5;
    }
    if (userinfo.phase < 4) {
        if (sv_info->sv_ext.boss_cnt >= 12 || (sv_info->sv_ext.boss_cnt == 11 && sv_info->sv_ext.battle == 0)) userinfo.phase = 4;
    }
    if (sv_info->sv_ext.survival_level >= 100) {
        if (userinfo.phase < 3) userinfo.phase = 3;
        for (i = 0; i < 18; i++) userinfo.usecol[i] |= 0x200;
        if (sv_info->sv_ext.fullcolor_on) {
            for (i = 1; i <= 18; i++) userinfo.usecol[18 - i] = 0x7FF;
        }
    }
    if (userinfo.phase < 2) {
        if (sv_info->sv_ext.boss_cnt >= 3 || (sv_info->sv_ext.boss_cnt == 2 && sv_info->sv_ext.battle == 0)) userinfo.phase = 2;
    }
    if (userinfo.phase <= 0) {
        if (sv_info->sv_ext.boss_cnt >= 2 || (sv_info->sv_ext.boss_cnt == 1 && sv_info->sv_ext.battle == 0)) userinfo.phase = 1;
    }
    SetLimitStatus();
}

extern USER_SETTING userinfo;
extern short int UseCharacter1p2p[];

void sv_save_result(void)
{
    int id;
    int i;

    id = UseCharacter1p2p[sv_info->ply];
    if (sv_info->sv_ext.survival_level >= 100) {
        sv_info->sv_ext.survival_totaltime = 0;
        sv_info->sv_ext.survival_level = 100;
        for (i = 0; i < 10; i++) sv_info->sv_ext.survival_totaltime += sv_info->sv_ext.survival_sectiontime[i];
    } else {
        sv_info->sv_ext.res_level = sv_info->sv_ext.survival_level;
        sv_info->sv_ext.res_stage = sv_info->win;
    }
    if (sv_info->sv_ext.survival_level < userinfo.svresult[id - 1].level) return;
    if (userinfo.svresult[id - 1].level == sv_info->sv_ext.survival_level
        && sv_info->sv_ext.survival_totaltime >= userinfo.svresult[id - 1].total_time) return;
    userinfo.svresult[id - 1].id = id;
    userinfo.svresult[id - 1].level = sv_info->sv_ext.res_level;
    userinfo.svresult[id - 1].stage = sv_info->sv_ext.res_stage + 1;
    for (i = 0; i < 10; i++) userinfo.svresult[id - 1].section_time[i] = sv_info->sv_ext.survival_sectiontime[i];
    userinfo.svresult[id - 1].total_time = sv_info->sv_ext.survival_totaltime;
}

int CockpitImageLoad(int no, int *pal, int sprtid);
void FadeInit(void);
void FightControl_Execute(FIGHT_CONTROL *tablep, NL_SPR_ARG *sp, int time);
void JobChange(int job);
void PXF_Free(int id, int pal);
void SetUVZ(NL_SPR_ARG *nlsp, int u, int v, int w, int h);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void Cmd_AddTrnsl(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_AddY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_ReqSe(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetTrnsl(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetX(FIGHT_CONTROL *p, NL_SPR_ARG *n);
void Cmd_SetY(FIGHT_CONTROL *p, NL_SPR_ARG *n);
extern int EnterFightCount;
extern int EnterFightJob;
extern int BattleStatus;

static const FIGHT_CONTROL ranyu2[] = {
    { 94, (void *)Cmd_ReqSe, 0.0f, 0, 0, 0, 1 },
    { 0, (void *)Cmd_AddX, 32.25f, 0, 0, 0, 20 },
    { 1, (void *)Cmd_AddX, 32.25f, 0, 0, 0, 20 },
    { 2, (void *)Cmd_AddX, -32.25f, 0, 0, 0, 20 },
    { 3, (void *)Cmd_AddX, -32.25f, 0, 0, 0, 20 },
    { 0, (void *)Cmd_AddX, -1.75f, 0, 0, 20, 24 },
    { 1, (void *)Cmd_AddX, -1.75f, 0, 0, 20, 24 },
    { 2, (void *)Cmd_AddX, 1.75f, 0, 0, 20, 24 },
    { 3, (void *)Cmd_AddX, 1.75f, 0, 0, 20, 24 },
    { 0, (void *)Cmd_AddX, 0.5f, 0, 0, 24, 28 },
    { 1, (void *)Cmd_AddX, 0.5f, 0, 0, 24, 28 },
    { 2, (void *)Cmd_AddX, -0.5f, 0, 0, 24, 28 },
    { 3, (void *)Cmd_AddX, -0.5f, 0, 0, 24, 28 },
    { 0, (void *)Cmd_SetX, 320.0f, 0, 0, 28, 29 },
    { 1, (void *)Cmd_SetX, 320.0f, 0, 0, 28, 29 },
    { 2, (void *)Cmd_SetX, 320.0f, 0, 0, 28, 29 },
    { 3, (void *)Cmd_SetX, 64.0f, 0, 0, 28, 29 },
    { 0, (void *)Cmd_AddY, -30.0f, 0, 0, 110, 118 },
    { 1, (void *)Cmd_AddY, -30.0f, 0, 0, 110, 118 },
    { 2, (void *)Cmd_AddY, 30.0f, 0, 0, 110, 118 },
    { 3, (void *)Cmd_AddY, 30.0f, 0, 0, 110, 118 },
    { 0, (void *)Cmd_SetY, -240.0f, 0, 0, 118, 119 },
    { 1, (void *)Cmd_SetY, -240.0f, 0, 0, 118, 119 },
    { 2, (void *)Cmd_SetY, 480.0f, 0, 0, 118, 119 },
    { 3, (void *)Cmd_SetY, 480.0f, 0, 0, 118, 119 },
    { 4, (void *)Cmd_AddTrnsl, 0.033333335f, 0, 0, 20, 50 },
    { 5, (void *)Cmd_AddTrnsl, 0.033333335f, 0, 0, 20, 50 },
    { 4, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 50, 51 },
    { 5, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 50, 51 },
    { 6, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 50, 51 },
    { 7, (void *)Cmd_SetTrnsl, 1.0f, 0, 0, 50, 51 },
    { 4, (void *)Cmd_SetTrnsl, 0.5f, 0, 0, 90, 91 },
    { 5, (void *)Cmd_SetTrnsl, 0.5f, 0, 0, 90, 91 },
    { 6, (void *)Cmd_SetTrnsl, 0.5f, 0, 0, 90, 91 },
    { 7, (void *)Cmd_SetTrnsl, 0.5f, 0, 0, 90, 91 },
    { 4, (void *)Cmd_AddX, -8.0f, 0, 0, 90, 110 },
    { 5, (void *)Cmd_AddX, -8.0f, 0, 0, 90, 110 },
    { 6, (void *)Cmd_AddX, 8.0f, 0, 0, 90, 110 },
    { 7, (void *)Cmd_AddX, 8.0f, 0, 0, 90, 110 },
    { 4, (void *)Cmd_AddTrnsl, -0.025f, 0, 0, 90, 110 },
    { 5, (void *)Cmd_AddTrnsl, -0.025f, 0, 0, 90, 110 },
    { 6, (void *)Cmd_AddTrnsl, -0.025f, 0, 0, 90, 110 },
    { 7, (void *)Cmd_AddTrnsl, -0.025f, 0, 0, 90, 110 },
    { 4, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 110, 111 },
    { 5, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 110, 111 },
    { 6, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 110, 111 },
    { 7, (void *)Cmd_SetTrnsl, 0.0f, 0, 0, 110, 111 },
    { -1, 0, 0.0f, 0, 0, 0, 0 },
};

void sv_IntIrqMain(void)
{
    static NL_SPR_ARG nlsp[9];
    static int fightctrl_time;
    int pal1;
    static int sprtid1;
    static int sprtid2;
    static int sprtid3;
    static char a;
    int i;

    if (EnterFightCount == 0) {
        BattleStatus = 1;
        EnterFightCount = 1;
    } else if (EnterFightCount == 1) {
        BattleStatus = 1;
        sprtid1 = CockpitImageLoad(50, &pal1, 0);
        EnterFightCount++;
    } else if (EnterFightCount == 2) {
        BattleStatus = 1;
        sprtid2 = CockpitImageLoad(51, &pal1, 1);
        EnterFightCount++;
    } else if (EnterFightCount == 3) {
        BattleStatus = 1;
        sprtid3 = CockpitImageLoad(52, &pal1, 2);
        EnterFightCount++;
    } else if (EnterFightCount == 4) {
        BattleStatus = 1;
        for (i = 0; i < 9; i++) {
            nlsp[i].sprno = sprtid1;
            nlsp[i].z = 2.0f;
            nlsp[i].zm_x = 1.0f;
            nlsp[i].zm_y = 1.0f;
            nlsp[i].u0 = 0.0f;
            nlsp[i].u1 = 1.0f;
            nlsp[i].v0 = 0.0f;
            nlsp[i].v1 = 1.0f;
            nlsp[i].attr = 5;
            nlsp[i].listType = 1;
            nlsp[i].trnsl = 1.0f;
        }
        nlsp[0].attr = 0x17;
        nlsp[0].x = -320.0f;
        nlsp[0].y = -16.0f;
        nlsp[1].x = -320.0f;
        nlsp[1].y = -16.0f;
        nlsp[2].sprno = sprtid2;
        nlsp[2].x = 960.0f;
        nlsp[2].y = 240.0f;
        nlsp[2].attr |= 0x10;
        nlsp[3].sprno = sprtid2;
        nlsp[3].x = 704.0f;
        nlsp[3].y = 240.0f;
        nlsp[4].sprno = sprtid3;
        nlsp[4].x = 64.0f;
        nlsp[4].y = 183.0f;
        nlsp[4].z = 1.0f;
        SetUVZ(&nlsp[4], 0, 0, 256, 64);
        nlsp[4].trnsl = 0.0f;
        nlsp[5].sprno = sprtid3;
        nlsp[5].x = 320.0f;
        nlsp[5].y = 183.0f;
        nlsp[5].z = 1.0f;
        SetUVZ(&nlsp[5], 0, 64, 256, 64);
        nlsp[5].trnsl = 0.0f;
        nlsp[6].sprno = sprtid3;
        nlsp[6].x = 64.0f;
        nlsp[6].y = 183.0f;
        nlsp[6].z = 1.0f;
        SetUVZ(&nlsp[6], 0, 0, 256, 64);
        nlsp[6].trnsl = 0.0f;
        nlsp[7].sprno = sprtid3;
        nlsp[7].x = 320.0f;
        nlsp[7].y = 183.0f;
        nlsp[7].z = 1.0f;
        SetUVZ(&nlsp[7], 0, 64, 256, 64);
        nlsp[7].trnsl = 0.0f;
        nlsp[8].sprno = sprtid3;
        nlsp[8].x = 320.0f;
        nlsp[8].y = 183.0f;
        nlsp[8].z = 1.0f;
        SetUVZ(&nlsp[8], 0, 128, 256, 64);
        fightctrl_time = 0;
        a = 0;
        FadeInit();
        EnterFightCount++;
    }
    if (EnterFightCount == 5) {
        if (fightctrl_time < 121) {
            FightControl_Execute((FIGHT_CONTROL *)ranyu2, nlsp, fightctrl_time);
            if (fightctrl_time >= 110 && fightctrl_time < 118) {
                a += 31;
            } else if (fightctrl_time == 118) {
                a = -1;
            }
            nlsp[8].x = nlsp[2].x - 128.0f;
            nlsp[8].y = nlsp[2].y + 74.0f;
            polydraw_FBox(0.0f, (int)(nlsp[0].y + 255.0f), 640.0f, (int)(nlsp[2].y + 1.0f), 2.0f, a << 24);
            polydraw_FBox((int)(nlsp[0].x - 256.0f - 64.0f), (int)nlsp[0].y, (int)(nlsp[0].x - 254.0f), (int)(nlsp[0].y + 256.0f), 2.0f, 0xFF000000);
            polydraw_FBox((int)(nlsp[1].x + 254.0f), (int)nlsp[1].y, (int)(nlsp[1].x + 256.0f + 64.0f), (int)(nlsp[1].y + 256.0f), 2.0f, 0xFF000000);
            polydraw_FBox((int)(nlsp[2].x + 256.0f), (int)nlsp[2].y, (int)(nlsp[2].x + 256.0f + 64.0f), (int)(nlsp[2].y + 256.0f), 2.0f, 0xFF000000);
            polydraw_FBox((int)(nlsp[3].x - 64.0f), (int)nlsp[3].y, (int)(nlsp[3].x + 1.0f), (int)(nlsp[3].y + 256.0f), 2.0f, 0xFF000000);
            for (i = 0; i < 9; i++) gSpriteDraw_NaomiLibVersion(&nlsp[i]);
            fightctrl_time++;
        } else {
            EnterFightCount = 6;
        }
    }
    if (EnterFightCount == 6) {
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 2.0f, 0xFF000000);
        EnterFightCount++;
    } else if (EnterFightCount == 7) {
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 2.0f, 0xFF000000);
        PXF_Free(sprtid1, pal1);
        PXF_Free(sprtid2, pal1);
        PXF_Free(sprtid3, pal1);
        EnterFightCount = 0;
        EnterFightJob = 0;
        JobChange(0x25);
    }
}

void JobChange(int job);
void g_main_End(void);
extern USER_SETTING userinfo;

void SvBossInit(void)
{
    g_main_End();
    InitSurvivalBattle();
    if (userinfo.quickconti) {
        JobChange(0x1E);
    } else {
        JobChange(0x15);
    }
}

int add_exp_combo(int combo)
{
    int plus;

    if (combo >= 20) {
        plus = (combo / 10) >> 1;
        plus = combo * plus + plus;
    } else {
        plus = combo;
    }
    return plus;
}
