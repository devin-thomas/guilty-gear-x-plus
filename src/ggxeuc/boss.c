#include "generated/boss.h"
extern short int UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern CHARACTER_WORK *PlayerWork;
extern int BattleStatus;
extern int PLAYER_HITPOINTMAX;
extern u_int TotalCounter;
extern unsigned int Pause;
extern CHARACTER_ORIGINAL_STATUS boss_status[18];
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void chk_boss_stat(void);
int boss_damage_hosei(CHARACTER_WORK *offset, int damage);
int boss_chk_iai(int side);
int boss_chk_rai(int side);
int boss_chk_nikudan(int side);
void boss_set_status(int side);

extern short int UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern CHARACTER_WORK *PlayerWork;
extern int BattleStatus;
extern int PLAYER_HITPOINTMAX;
extern u_int TotalCounter;
extern unsigned int Pause;
void boss_set_status(int side);

void chk_boss_stat(void)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (g_info.boss[i] == 1) {
            boss_set_status(i);
            switch (UseCharacter1p2p[i]) {
            case 1:
                PlayerWork[i].ply->Tension = 10000;
                if (BattleStatus == 2 && Pause == 0 && TotalCounter % 10 == 0) {
                    if (PlayerWork[i].HitPoint > 0 && PlayerWork[i].HitPoint < PLAYER_HITPOINTMAX && !(PlayerWork[i].actst & 0x20)) {
                        PlayerWork[i].HitPoint++;
                    }
                }
                if (g_info.mode & 0x2000) {
                    if (PlayerWork[i].ply->PersonalWork[0].w[0] > 0) {
                        PlayerWork[i].ply->PersonalWork[0].w[0] = 0x1E00;
                    }
                } else {
                    PlayerWork[i].ply->PersonalWork[0].w[0] = 0x1E00;
                }
                break;
            case 12:
                PlayerWork[i].ply->PersonalWork[0].b[2] = 3;
                PlayerWork[i].ply->PersonalWork[1].b[1] = 3;
                PlayerWork[i].ply->PersonalWork[1].b[0] = 3;
                PlayerWork[i].ply->PersonalWork[1].b[2] = 3;
                break;
            case 15:
                if (BattleStatus == 2 && Pause == 0) {
                    if (PlayerWork[i].ply->PersonalWork[0].ub[1] <= 0x60 && !(TotalCounter & 3)) {
                        PlayerWork[i].ply->PersonalWork[0].ub[1]++;
                    }
                }
                break;
            case 10:
            case 17:
                PlayerWork[i].ply->Tension = 10000;
                break;
            }
        }
        if (g_info.boss[i] == 2) {
            PlayerWork[i].ply->Tension = 10000;
        }
    }
}

int boss_damage_hosei(CHARACTER_WORK *offset, int damage)
{
    int dam;
    int damres;
    int rate;
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    damres = damage;
    dam = damres;
    if (g_info.boss[eoffset->padid] == 1) {
        if (UseCharacter1p2p[eoffset->padid] == 14) {
            if (boss_chk_nikudan(offset->padid)) {
                rate = boss_status[UseCharacter1p2p[eoffset->padid] - 1].add_damage;
            } else {
                rate = 100;
            }
        } else if (UseCharacter1p2p[eoffset->padid] == 2) {
            if (boss_chk_rai(offset->padid)) {
                rate = boss_status[UseCharacter1p2p[eoffset->padid] - 1].add_damage + 50;
            } else {
                rate = boss_status[UseCharacter1p2p[eoffset->padid] - 1].add_damage;
            }
        } else {
            rate = boss_status[UseCharacter1p2p[eoffset->padid] - 1].add_damage;
        }
        damres = damage + dam * rate / 100;
        if (UseCharacter1p2p[eoffset->padid] == 14 && boss_chk_iai(offset->padid)) {
            damres = PLAYER_HITPOINTMAX * 2;
        }
    }
    if (g_info.boss[offset->padid] == 1) {
        rate = boss_status[UseCharacter1p2p[offset->padid] - 1].throw_damage;
        damres = damres - damres * rate / 100;
        if (damres == 0) {
            damres = 1;
        }
    }
    return damres;
}

int boss_chk_iai(int side)
{
    if (PlayerWork[side].actst & 0x4000) {
        PlayerWork[side].DamActHeader[0].flag &= ~0x20000000;
    }
    if (PlayerWork[side].DamActHeader[0].flag & 0x20000000) {
        return 1;
    }
    return 0;
}

int boss_chk_rai(int side)
{
    TACTHEADER *header;
    int i;
    int ret;

    ret = 0;
    for (i = 0; i < 3; i++) {
        header = &PlayerWork[side].DamActHeader[i];
        if (header->flag & 0x80000) {
            ret = 1;
            break;
        }
    }
    return ret;
}

int boss_chk_nikudan(int side)
{
    TACTHEADER *header;
    int i;
    int ret;

    ret = 0;
    for (i = 0; i < 3; i++) {
        header = &PlayerWork[side].DamActHeader[i];
        if (header->flag & 1) {
            ret = 1;
            break;
        }
    }
    return ret;
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

void boss_set_status(int side)
{
    if (g_info.boss[side] == 1) {
        ForwardWalk_XSpeedByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].fwalk_speed;
        BackWalk_XSpeedByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].bwalk_speed;
        Dash_XSpeedByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].fdash_speed;
        BackStep_XSpeedByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].bdash_speed;
        FrontJump_XSpeedByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].fjump_speed;
        BackJump_XSpeedByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].bjump_speed;
        Dash_AccelByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].fdash_accel;
        Dash_ResistByIdno[side] = boss_status[UseCharacter1p2p[side] - 1].fdash_resist;
        SkyDash_Height[side] = boss_status[UseCharacter1p2p[side] - 1].sdash_height;
        PDSkyDashF_Time[side] = boss_status[UseCharacter1p2p[side] - 1].sfdash_time;
        PDSkyDashB_Time[side] = boss_status[UseCharacter1p2p[side] - 1].sbdash_time;
        PDSkyDashF_AttackTime[side] = boss_status[UseCharacter1p2p[side] - 1].sfdash_time_noattack;
        PDSkyDashB_AttackTime[side] = boss_status[UseCharacter1p2p[side] - 1].sbdash_tiem_noattack;
        PDTensionAdd_FWalk[side] = boss_status[UseCharacter1p2p[side] - 1].fwalk_tensionup;
        PDTensionAdd_FJump[side] = boss_status[UseCharacter1p2p[side] - 1].fjump_tensionup;
        PDTensionAdd_FDash[side] = boss_status[UseCharacter1p2p[side] - 1].fdash_tensionup;
        PDTensionAdd_FSDash[side] = boss_status[UseCharacter1p2p[side] - 1].sfdash_tensionup;
        PDKonjo[side] = boss_status[UseCharacter1p2p[side] - 1].konp;
    }
}
