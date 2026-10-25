#include "generated/training.h"

#define NULL 0
extern TRAINING_SETTING *tr_info;
extern USER_SETTING userinfo;
void MarkDraw(char *str, int x, int y, int z, int alpha);
extern PAD pads[2];
void *mmalloc(int size, char *name);
extern CHARACTER_WORK *PlayerWork;
extern CHARANGE_CTRL *gl_info;
extern PRACALL prac_all[];
extern GAME_INFOMATION g_info;
void g_main_End(void);
void JobChange(int job);
void nlSetBackColor(int c1, int c2, int c3);
extern SYS_COND sc;
extern unsigned int Pause;
void gSpriteDrawPS(NL_SPR_ARG *sprite);
extern DCFONT_INFO fi;
int IsHoldPunch(CHARACTER_WORK *offset);
int IsHoldKick(CHARACTER_WORK *offset);
int IsHoldSlash(CHARACTER_WORK *offset);
int IsHoldHighSlash(CHARACTER_WORK *offset);
int IsHoldFuckin(CHARACTER_WORK *offset);
int IsTrgPunch(CHARACTER_WORK *offset);
int IsTrgKick(CHARACTER_WORK *offset);
int IsTrgSlash(CHARACTER_WORK *offset);
int IsTrgHighSlash(CHARACTER_WORK *offset);
int IsTrgFuckin(CHARACTER_WORK *offset);
void InitScore(void);
void MayBack(int a);
extern int BattleStatus;
extern int StageNo;
void set_char2num(int num, char *block, int id);
void FontDrawDC(char *str, float x, float y, float z, int alpha, int id);
int GetBattleTimeStopFlag(int padid);
void FontDraw(char *str, int x, int y, float z, int alfa);
char *G_sprintf(char *buff, char *str, int *valtb);
extern short int UseCharacter1p2p[];
void ResetScreen(void);
void EliminateRoundEnd(void);
void PlayerParaInit(void);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
void ResultInitial(void);
void effect_killall(void);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void object_killall(void);
void font_entry(int side, char *str);
void FadeInit(void);
extern short int PDDef_Gravity[];
extern unsigned int ResultFlag;
unsigned int gStripGetFadeColor(unsigned int col);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
extern char D_5CB910[]; /* RODATA_EXTERN 0x5cb910: "tr_info" (.sdata; TranningStat_chk strings follow) */
void InitGameinfo_ByGameOver(int side);
extern int PLAYER_HITPOINTMAX;

void Tranning_init(void)
{
    int i;

    if (tr_info == NULL) {
        tr_info = mmalloc(0x38F8, D_5CB910);
    }
    for (i = 0; i < 2; i++) {
        g_info.LoadPlayerID[i] = -1;
        UseCharacter1p2p[i] = 0;
        InitGameinfo_ByGameOver(i);
    }
    StageNo = 0;
    tr_info->cnfside = 0;
    tr_info->stat = 0;
    tr_info->itigeki = 0;
    tr_info->wait = 0;
    tr_info->damage_origin = 0;
    tr_info->damage = 0;
    tr_info->total_damage = 0;
    tr_info->max_damage = 0;
    tr_info->max_combo = 0;
    tr_info->iobkup[0] = 0;
    tr_info->iobkup[1] = 0;
    tr_info->iobkup[2] = 0;
    tr_info->iobkup[3] = 0;
    tr_info->quickselect = 0;
    tr_info->tene.memflg = 0;
    tr_info->tene.iomem[0][0] = -1;
    tr_info->tene.iomem[1][0] = -1;
    tr_info->tene.iomem[2][0] = -1;
    tr_info->tene.iomem[3][0] = -1;
    tr_info->tene.iomem[0][899] = -1;
    tr_info->tene.iomem[1][899] = -1;
    tr_info->tene.memcnt = 0;
    tr_info->tene.resettim = 0;
    tr_info->tene.iomem[2][899] = -1;
    tr_info->tene.iomem[3][899] = -1;
    tr_info->begin = 0;
    tr_info->clear = 0;
    tr_info->attackcnt = 0;
    tr_info->tcom.ggage = userinfo.tbkup.ggage;
    tr_info->tcom.tension = userinfo.tbkup.tension;
    tr_info->tcom.damage_disp = userinfo.tbkup.damage_disp;
    tr_info->tene.mode = userinfo.tbkup.mode;
    tr_info->tene.level = userinfo.tbkup.level;
    tr_info->tene.type = userinfo.tbkup.type;
    tr_info->tene.guard = userinfo.tbkup.guard;
    tr_info->tene.ukemi = userinfo.tbkup.ukemi;
    tr_info->tene.rejene = userinfo.tbkup.rejene;
    tr_info->itigeki = 0;
    mark_init_main();
}

void Tranning_init_prac(void)
{
    tr_info->pracno = 0;
    tr_info->pracidx = -1;
    tr_info->praccnt = -1;
}

void TranningStat_chk(void)
{
    HEADWORK hw[5] = {
        { "", 0.0f, 0.0f, 0, 0, 0 },
        { "STANDBY", 257.0f, 96.0f, 0, 0, 0 },
        { "RECORDING", 236.0f, 96.0f, 0, 0, 0 },
        { "PLAYBACK", 250.0f, 96.0f, 0, 0, 0 },
        { "PRACTICE", 250.0f, 96.0f, 0, 0, 0 },
    };
    int i;
    CHARACTER_WORK *offset;

    Training_DamageChk();
    Training_DamageDIsp();
    if (tr_info->itigeki == 5) {
        tr_info->stat = 2;
    }
    if (Training_StatExecute()) {
        return;
    }
    if (tr_info->tene.mode && tr_info->tene.memflg) {
        tr_info->tene.memflg = 0;
    }
    if (Pause != 1 && Pause != 2) {
        if (tr_info->pracno && !tr_info->tene.memflg) {
            prac_mark_disp();
        } else if (!Pause) {
            mark_execute();
        }
    }
    if (BattleStatus != 2) {
        tr_info->attackcnt = 0;
        return;
    }
    {
        offset = &PlayerWork[tr_info->ply];
        tr_info->pracnomax = prac_all[offset->idno].nomax;
        if (tr_info->tene.mode == 0) {
            PlayerWork[tr_info->ply ? 0 : 1].ply->PunchKey = userinfo.key[tr_info->ply].punch;
            PlayerWork[tr_info->ply ? 0 : 1].ply->KickKey = userinfo.key[tr_info->ply].kick;
            PlayerWork[tr_info->ply ? 0 : 1].ply->SlashKey = userinfo.key[tr_info->ply].slash;
            PlayerWork[tr_info->ply ? 0 : 1].ply->HighSlashKey = userinfo.key[tr_info->ply].hslash;
            PlayerWork[tr_info->ply ? 0 : 1].ply->Fuckin = userinfo.key[tr_info->ply].fuckin;
            PlayerWork[tr_info->ply ? 0 : 1].ply->MemoryKey = userinfo.key[tr_info->ply].memory;
            PlayerWork[tr_info->ply ? 0 : 1].ply->PandK = userinfo.key[tr_info->ply].pandk;
            PlayerWork[tr_info->ply ? 0 : 1].ply->SandHS = userinfo.key[tr_info->ply].sandhs;
            PlayerWork[tr_info->ply ? 0 : 1].ply->PKS = userinfo.key[tr_info->ply].pks;
            PlayerWork[tr_info->ply ? 0 : 1].ply->PKSHS = userinfo.key[tr_info->ply].pkshs;
        } else if (tr_info->tene.mode == 2) {
            PlayerWork[tr_info->ply ? 0 : 1].ply->PunchKey = userinfo.key[tr_info->ply ? 0 : 1].punch;
            PlayerWork[tr_info->ply ? 0 : 1].ply->KickKey = userinfo.key[tr_info->ply ? 0 : 1].kick;
            PlayerWork[tr_info->ply ? 0 : 1].ply->SlashKey = userinfo.key[tr_info->ply ? 0 : 1].slash;
            PlayerWork[tr_info->ply ? 0 : 1].ply->HighSlashKey = userinfo.key[tr_info->ply ? 0 : 1].hslash;
            PlayerWork[tr_info->ply ? 0 : 1].ply->Fuckin = userinfo.key[tr_info->ply ? 0 : 1].fuckin;
            PlayerWork[tr_info->ply ? 0 : 1].ply->MemoryKey = userinfo.key[tr_info->ply ? 0 : 1].memory;
            PlayerWork[tr_info->ply ? 0 : 1].ply->PandK = userinfo.key[tr_info->ply ? 0 : 1].pandk;
            PlayerWork[tr_info->ply ? 0 : 1].ply->SandHS = userinfo.key[tr_info->ply ? 0 : 1].sandhs;
            PlayerWork[tr_info->ply ? 0 : 1].ply->PKS = userinfo.key[tr_info->ply ? 0 : 1].pks;
            PlayerWork[tr_info->ply ? 0 : 1].ply->PKSHS = userinfo.key[tr_info->ply ? 0 : 1].pkshs;
        }
        if (Pause != 1 && Pause != 2) {
            if (tr_info->pracno) {
                if (tr_info->praccnt == -1) {
                    prac_judge_main();
                } else {
                    prac_judge_get_next(0);
                }
            } else {
                prac_judge_get_next(0);
            }
            for (i = 0; i < 2; i++) {
                if (tr_info->itigeki == 0
                    && (((tr_info->tene.rejene & 1) && tr_info->ply == i) || ((tr_info->tene.rejene & 2) && tr_info->ply != i))) {
                    if (PlayerWork[i].HitPoint == 0) {
                        PlayerWork[i].HitPoint = 1;
                    }
                    if (!(PlayerWork[i].actst & 0x20) && PlayerWork[i].HitPoint < PLAYER_HITPOINTMAX) {
                        PlayerWork[i].HitPoint += 4;
                        if (PlayerWork[i].HitPoint > PLAYER_HITPOINTMAX) {
                            PlayerWork[i].HitPoint = PLAYER_HITPOINTMAX;
                        }
                    }
                }
                chk_itigeki(i);
            }
        }
        if (tr_info->wait) {
            tr_info->wait--;
        } else {
            tr_info->wait = 0;
        }
        if (Pause == 0) {
            if (tr_info->praccnt != -1) {
                FontDrawDC(hw[4].head, hw[4].xp, hw[4].yp, 4.0f, 1, 1);
            } else {
                FontDrawDC(hw[tr_info->tene.memflg].head, hw[tr_info->tene.memflg].xp, hw[tr_info->tene.memflg].yp, 4.0f, 1, 1);
            }
            if (pads[tr_info->ply].data == userinfo.key[tr_info->ply].memory && tr_info->tene.memflg != 1) {
                if (tr_info->tene.resettim < 60) {
                    tr_info->tene.resettim++;
                } else if (tr_info->pracno == 0) {
                    tr_info->tene.memcnt = 0;
                    tr_info->tene.memflg = 1;
                }
            } else {
                tr_info->tene.resettim = 0;
            }
            if (tr_info->tene.mode == 0 && tr_info->tene.type == 3 && tr_info->tene.memflg == 0) {
                training_disp_attackcnt();
            } else {
                tr_info->attackcnt = 0;
            }
            if (tr_info->tene.mode == 0) {
                if ((tr_info->iobkup[2] & userinfo.key[tr_info->ply].memory) && !tr_info->wait) {
                    switch (tr_info->tene.memflg) {
                    case 0:
                        if (!tr_info->pracno) {
                            if (tr_info->tene.iomem[0][0] != -1) {
                                tr_info->tene.memflg = 3;
                                tr_info->tene.memcnt = 0;
                                tr_info->wait = 2;
                            }
                        } else if (tr_info->praccnt != -1) {
                            tr_info->wait = 2;
                            tr_info->pracidx = -1;
                            tr_info->praccnt = -1;
                            tranning_bkup_clr();
                            PlayerWork[tr_info->ply].actnonexttemp = 0;
                        } else {
                            tr_info->wait = 2;
                            tr_info->pracidx = 0;
                            tr_info->praccnt = 0;
                        }
                        break;
                    case 1:
                        disp_memcnt();
                        tranning_bkup_clr();
                        tr_info->tene.memcnt = 0;
                        if (tr_info->tene.resettim == 0) {
                            tr_info->tene.memflg = 2;
                            tr_info->wait = 2;
                        }
                        break;
                    case 2:
                        tr_info->tene.memflg = 0;
                        tr_info->tene.iomem[0][tr_info->tene.memcnt] = -1;
                        tr_info->tene.iomem[1][tr_info->tene.memcnt] = -1;
                        tr_info->tene.iomem[2][tr_info->tene.memcnt] = -1;
                        tr_info->tene.iomem[3][tr_info->tene.memcnt] = -1;
                        tr_info->wait = 2;
                        break;
                    case 3:
                        tr_info->tene.memflg = 0;
                        break;
                    }
                } else {
                    if (tr_info->tene.mode == 1) {
                        tr_info->tene.memflg = 0;
                    }
                    switch (tr_info->tene.memflg) {
                    case 2:
                        disp_memcnt();
                        if (tr_info->tene.memcnt >= 899 || BattleStatus != 2) {
                            tr_info->tene.iomem[0][tr_info->tene.memcnt] = -1;
                            tr_info->tene.iomem[1][tr_info->tene.memcnt] = -1;
                            tr_info->tene.iomem[2][tr_info->tene.memcnt] = -1;
                            tr_info->tene.iomem[3][tr_info->tene.memcnt] = -1;
                            tr_info->tene.memflg = 0;
                        } else {
                            tr_info->tene.memcnt++;
                        }
                        break;
                    case 3:
                        if (tr_info->tene.iomem[1][tr_info->tene.memcnt] == -1) {
                            tr_info->tene.memflg = 0;
                        } else {
                            tr_info->tene.memcnt++;
                        }
                        break;
                    case 1:
                        disp_memcnt();
                        break;
                    default:
                        tr_info->tene.memcnt = 0;
                        break;
                    }
                }
            } else if ((pads[tr_info->ply].trg & userinfo.key[tr_info->ply].memory) && !tr_info->wait && tr_info->pracno) {
                if (tr_info->praccnt != -1) {
                    tr_info->wait = 2;
                    tr_info->pracidx = -1;
                    tr_info->praccnt = -1;
                    tranning_bkup_clr();
                    PlayerWork[tr_info->ply].actnonexttemp = 0;
                } else {
                    tr_info->wait = 2;
                    tr_info->pracidx = 0;
                    tr_info->praccnt = 0;
                }
            }
            switch (tr_info->tcom.tension) {
            case 1:
                PlayerWork[tr_info->ply].ply->Tension = 10000;
                break;
            case 2:
                PlayerWork[tr_info->ply ? 0 : 1].ply->Tension = 10000;
                break;
            case 3:
                PlayerWork[0].ply->Tension = 10000;
                PlayerWork[1].ply->Tension = 10000;
                break;
            }
        } else {
            prac_judge_get_next(0);
            tr_info->pracidx = -1;
            tr_info->praccnt = -1;
            PlayerWork[tr_info->ply].actnonexttemp = 0;
            if (tr_info->tene.memflg == 2 || tr_info->tene.memflg == 3) {
                tr_info->tene.memflg = 0;
            }
            PlayerWork[tr_info->ply ? 0 : 1].ply->CPU = 0;
            g_info.isCPU[tr_info->ply == 0] = 0;
        }
        if (tr_info->tene.mode == 0 || tr_info->tene.mode == 2) {
            PlayerWork[tr_info->ply ? 0 : 1].ply->CPU = 0;
            g_info.isCPU[tr_info->ply == 0] = 0;
        } else if (tr_info->tene.mode == 1) {
            if (PlayerWork[tr_info->ply ? 0 : 1].ply->CPU == 0) {
                PlayerWork[tr_info->ply ? 0 : 1].ply->CPU = 1;
                g_info.isCPU[tr_info->ply == 0] = 1;
            }
        }
        Training_ResetChk();
    }
}

int Training_StatExecute(void)
{
    switch (tr_info->stat) {
    case 0:
        break;
    case 1:
        g_main_End();
        g_info.PlayerSelected[0] = 0;
        g_info.PlayerSelected[1] = 0;
        JobChange(0xC);
        return 1;
    case 2:
        tranning_reset();
        tr_info->stat = 0;
        break;
    case 3:
        g_main_End();
        bkup_trainin_setting();
        g_info.PlayerSelected[0] = 0;
        g_info.PlayerSelected[1] = 0;
        if (userinfo.autosave) {
            nlSetBackColor(0, 0, 0);
            sc.grpvar[1] = -1;
            JobChange(0x2D);
        } else {
            nlSetBackColor(0, 0, 0);
            JobChange(3);
        }
        return 1;
    }
    return 0;
}

void bkup_trainin_setting(void)
{
    userinfo.tbkup.ggage = tr_info->tcom.ggage;
    userinfo.tbkup.tension = tr_info->tcom.tension;
    userinfo.tbkup.damage_disp = tr_info->tcom.damage_disp;
    userinfo.tbkup.mode = tr_info->tene.mode;
    userinfo.tbkup.level = tr_info->tene.level;
    userinfo.tbkup.type = tr_info->tene.type;
    userinfo.tbkup.guard = tr_info->tene.guard;
    userinfo.tbkup.ukemi = tr_info->tene.ukemi;
    userinfo.tbkup.rejene = tr_info->tene.rejene;
}

void tranning_reset(void)
{
    int i;

    EliminateRoundEnd();
    ResultInitial();
    FadeInit();
    font_entry(-1, NULL);
    PlayerParaInit();
    for (i = 0; i < 2; i++) {
        PlayerWork[i].parentx = 0;
        PlayerWork[i].parenty = 0;
        PlayerWork[i].actno = 0;
        PlayerWork[i].actnonext = -1;
        PlayerWork[i].actnonexttemp = -1;
        SetActNo(&PlayerWork[i], 0);
        SetAnimeNo(&PlayerWork[i], 0);
        if (UseCharacter1p2p[i] == 8) {
            PlayerWork[i].ply->PersonalWork[0].b[1] = 1;
            PlayerWork[i].ply->PersonalWork[1].w[0] = 600;
        }
        PlayerWork[i].gravity = PDDef_Gravity[PlayerWork[i].padid];
        PlayerWork[i].ply->JumpFlag = 0;
        PlayerWork[i].ply->SkyDashCorrect = 0;
        PlayerWork[i].ply->maintain = 0;
    }
    ResetScreen();
    effect_killall();
    object_killall();
    tr_info->itigeki = 0;
    tr_info->tene.memflg = 0;
    g_info.BattleControlFlag = 0;
    BattleStatus = 2;
    ResultFlag = 0;
    tr_info->pracidx = -1;
    tr_info->praccnt = -1;
    tr_info->pj.idx = -1;
    pads[0].trg &= ~(PlayerWork[0].ply->Fuckin | PlayerWork[0].ply->MemoryKey | 0x800);
    pads[1].trg &= ~(PlayerWork[1].ply->Fuckin | PlayerWork[1].ply->MemoryKey | 0x800);
}

void tranning_memorize_execute(int side)
{
    if ((tr_info->iobkup[2] & userinfo.key[side ? 0 : 1].memory) && tr_info->wait == 0) {
        tr_info->tene.iomem[0][tr_info->tene.memcnt] = -1;
        tr_info->tene.iomem[1][tr_info->tene.memcnt] = -1;
        tr_info->tene.iomem[2][tr_info->tene.memcnt] = -1;
        tr_info->tene.iomem[3][tr_info->tene.memcnt] = -1;
    } else {
        tr_info->tene.iomem[2][tr_info->tene.memcnt] = pads[side].trg;
        tr_info->tene.iomem[1][tr_info->tene.memcnt] = pads[side].data;
        tr_info->tene.iomem[0][tr_info->tene.memcnt] = pads[side].old;
        tr_info->tene.iomem[3][tr_info->tene.memcnt] = pads[side].offtrg;
    }
}

void tranning_execute_execute(int side)
{
    if (tr_info->tene.iomem[1][tr_info->tene.memcnt] != -1) {
        pads[side].trg = tr_info->tene.iomem[2][tr_info->tene.memcnt];
        pads[side].data = tr_info->tene.iomem[1][tr_info->tene.memcnt];
        pads[side].old = tr_info->tene.iomem[0][tr_info->tene.memcnt];
        pads[side].offtrg = tr_info->tene.iomem[3][tr_info->tene.memcnt];
    } else {
        pads[side].trg = 0;
        pads[side].data = 0;
    }
}

void tranning_plykey_save(int side)
{
    int key = pads[side].data;

    tr_info->iobkup[0] = tr_info->iobkup[1];
    tr_info->iobkup[2] = ~tr_info->iobkup[1] & key;
    tr_info->iobkup[3] = tr_info->iobkup[1] & ~key;
    tr_info->iobkup[1] = key;
    pads[side].old = tr_info->iobkup[0];
    pads[side].data = tr_info->iobkup[1];
    pads[side].trg = tr_info->iobkup[2];
    pads[side].offtrg = tr_info->iobkup[3];
}

void tranning_bkup_clr(void)
{
    tr_info->iobkup[0] = 0;
    tr_info->iobkup[1] = 0;
    tr_info->iobkup[2] = 0;
    tr_info->iobkup[3] = 0;
}

void tranning_keyset_deku(int side)
{
    pads[side].trg = change_iodat_plytodeku(side, tr_info->iobkup[2]);
    pads[side].data = change_iodat_plytodeku(side, tr_info->iobkup[1]);
    pads[side].old = change_iodat_plytodeku(side, tr_info->iobkup[0]);
    pads[side].offtrg = change_iodat_plytodeku(side, tr_info->iobkup[3]);
}

u_int change_iodat_plytodeku(int side, u_int dat)
{
    int tmp;

    tmp = 0;
    if (dat & 0x1000) {
        tmp |= 0x1000;
    }
    if (dat & 0x4000) {
        tmp |= 0x4000;
    }
    if (dat & 0x8000) {
        tmp |= 0x8000;
    }
    if (dat & 0x2000) {
        tmp |= 0x2000;
    }
    if (dat & PlayerWork[side ? 0 : 1].ply->PunchKey) {
        tmp |= PlayerWork[side].ply->PunchKey;
    }
    if (dat & PlayerWork[side ? 0 : 1].ply->KickKey) {
        tmp |= PlayerWork[side].ply->KickKey;
    }
    if (dat & PlayerWork[side ? 0 : 1].ply->SlashKey) {
        tmp |= PlayerWork[side].ply->SlashKey;
    }
    if (dat & PlayerWork[side ? 0 : 1].ply->HighSlashKey) {
        tmp |= PlayerWork[side].ply->HighSlashKey;
    }
    if (dat & PlayerWork[side ? 0 : 1].ply->Fuckin) {
        tmp |= PlayerWork[side].ply->Fuckin;
    }
    return tmp;
}

void Training_ResetChk(void)
{
    int i;
    int flg;

    if (Pause) {
        return;
    }
    if (tr_info->itigeki == 3) {
        return;
    }
    if (BattleStatus != 2) {
        return;
    }
    flg = 0;
    for (i = 0; i < 2; i++) {
        if (((pads[i].trg & PlayerWork[i].ply->Fuckin) && (pads[i].data & PlayerWork[i].ply->MemoryKey))
            || ((pads[i].data & PlayerWork[i].ply->Fuckin) && (pads[i].trg & PlayerWork[i].ply->MemoryKey))) {
            flg = 1;
            if (tr_info->begin == 0) {
                tr_info->stat = 2;
                pads[0].trg = 0;
                pads[1].trg = 0;
                tr_info->begin = 2;
                InitScore();
                if (StageNo == 5) {
                    MayBack(1);
                }
                break;
            }
        }
    }
    if (!flg && tr_info->begin > 0) {
        tr_info->begin--;
    }
}

void Training_DamageChk(void)
{
    if ((tr_info->tcom.damage_disp == 0 && Pause == 0) || tr_info->stat == 1) {
        tr_info->damage = 0;
        tr_info->total_damage = 0;
        tr_info->max_damage = 0;
        tr_info->max_combo = 0;
    }
    if (((PlayerWork[tr_info->ply ? 0 : 1].actst & 0x20) || sc.damage[tr_info->ply ? 0 : 1] != 0) && tr_info->itigeki == 0) {
        if (sc.damage[tr_info->ply ? 0 : 1]) {
            tr_info->damage = sc.damage[tr_info->ply ? 0 : 1];
            tr_info->total_damage = tr_info->damage_origin - PlayerWork[tr_info->ply ? 0 : 1].HitPoint;
            if (tr_info->max_damage < tr_info->total_damage) {
                tr_info->max_damage = tr_info->total_damage;
            }
            if (tr_info->max_combo < PlayerWork[tr_info->ply ? 0 : 1].ply->HitCount) {
                tr_info->max_combo = PlayerWork[tr_info->ply ? 0 : 1].ply->HitCount;
            }
        }
    } else {
        tr_info->damage_origin = PlayerWork[tr_info->ply ? 0 : 1].HitPoint;
    }
}

void Training_DamageDIsp(void)
{
    int cpsenterpos[2] = { 160, 480 };
    int valtb[2];

    if (tr_info->tcom.damage_disp && !Pause) {
        valtb[0] = tr_info->damage;
        FontDraw("      DAMAGE", cpsenterpos[tr_info->ply ? 0 : 1] - 118, 112, 265.0f, 0xFF);
        FontDraw(G_sprintf(NULL, "%3d", valtb), cpsenterpos[tr_info->ply ? 0 : 1] + 58, 112, 266.0f, 0xFF);
        valtb[0] = tr_info->total_damage;
        FontDraw("TOTAL DAMAGE", cpsenterpos[tr_info->ply ? 0 : 1] - 118, 136, 265.0f, 0xFF);
        FontDraw(G_sprintf(NULL, "%3d", valtb), cpsenterpos[tr_info->ply ? 0 : 1] + 58, 136, 266.0f, 0xFF);
        valtb[0] = tr_info->max_damage;
        FontDraw("  MAX DAMAGE", cpsenterpos[tr_info->ply ? 0 : 1] - 118, 160, 265.0f, 0xFF);
        FontDraw(G_sprintf(NULL, "%3d", valtb), cpsenterpos[tr_info->ply ? 0 : 1] + 58, 160, 266.0f, 0xFF);
        valtb[0] = tr_info->max_combo;
        FontDraw("  MAX  COMBO", cpsenterpos[tr_info->ply ? 0 : 1] - 118, 184, 265.0f, 0xFF);
        FontDraw(G_sprintf(NULL, "%3d", valtb), cpsenterpos[tr_info->ply ? 0 : 1] + 58, 184, 266.0f, 0xFF);
    }
}

void disp_memcnt(void)
{
    int cnt;
    int xp;
    char buf[4];

    cnt = 900 - tr_info->tene.memcnt;
    set_char2num(cnt / 60, buf, 0);
    xp = (cnt / 60 < 10) ? 0x128 : 0x114;
    FontDrawDC(buf, xp, 136.0f, 4.0f, 1, 2);
    FontDrawDC(":", 322.0f, 136.0f, 4.0f, 1, 2);
    set_char2num(cnt % 60, buf, 2);
    FontDrawDC(buf, 334.0f, 136.0f, 4.0f, 1, 2);
}

void chk_itigeki(int id)
{
    int i;
    int temp_no;
    int flag;
    int itigeki_table[18][6] = {
        { 0x7C, 0x7D, 0x7E, 0x7F, -1, -1 },
        { 0x60, -1, -1, -1, -1, -1 },
        { 0xA2, 0xA3, 0xA4, -1, -1, -1 },
        { 0x70, 0x71, 0x72, -1, -1, -1 },
        { 0x7F, 0x80, 0x81, -1, -1, -1 },
        { 0x96, 0x97, -1, -1, -1, -1 },
        { 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C },
        { 0x5D, 0x5E, 0x5F, 0x60, -1, -1 },
        { 0x65, 0x66, 0x67, -1, -1, -1 },
        { 0x64, 0x65, 0x66, 0x68, -1, -1 },
        { 0x96, 0x97, 0x5E, -1, -1, -1 },
        { 0x65, 0x66, 0x78, 0x79, 0x73, 0x89 },
        { 0x63, 0x64, 0x65, 0x6E, -1, -1 },
        { 0x7C, 0x7E, 0x7F, -1, -1, -1 },
        { 0x6A, 0x6B, 0x6C, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1 },
        { 0x57, 0x58, 0x59, 0x5A, -1, -1 },
        { 0x5D, 0x5E, 0x5F, -1, -1, -1 },
    };

    if (PlayerWork[id].ply->OverKillFlag == 2) {
        flag = 0;
        for (i = 0; i < 6; i++) {
            temp_no = itigeki_table[PlayerWork[id].idno - 1][i];
            if (temp_no != -1 && temp_no == PlayerWork[id].actno) {
                flag++;
            }
        }
        if (flag == 0) {
            PlayerWork[id].ply->OverKillFlag = 0;
            tr_info->itigeki = 0;
        } else if (GetBattleTimeStopFlag(id)) {
            tr_info->itigeki = 3;
        } else if (tr_info->itigeki != 3) {
            tr_info->itigeki = 2;
        }
    }
}

void mark_execute(void)
{
    int ret;
    u_int key_temp;
    u_int trg_temp;
    CHARACTER_WORK *offset;

    offset = &PlayerWork[tr_info->ply];
    key_temp = pads[tr_info->ply].data;
    key_temp &= 0xF800;
    if (IsHoldPunch(offset)) {
        key_temp |= 0x80;
    }
    if (IsHoldKick(offset)) {
        key_temp |= 0x40;
    }
    if (IsHoldSlash(offset)) {
        key_temp |= 0x10;
    }
    if (IsHoldHighSlash(offset)) {
        key_temp |= 0x20;
    }
    if (IsHoldFuckin(offset)) {
        key_temp |= 1;
    }
    trg_temp = pads[tr_info->ply].trg;
    trg_temp &= 0xF800;
    if (IsTrgPunch(offset)) {
        trg_temp |= 0x80;
    }
    if (IsTrgKick(offset)) {
        trg_temp |= 0x40;
    }
    if (IsTrgSlash(offset)) {
        trg_temp |= 0x10;
    }
    if (IsTrgHighSlash(offset)) {
        trg_temp |= 0x20;
    }
    if (IsTrgFuckin(offset)) {
        trg_temp |= 1;
    }
    minfo->key = key_temp;
    minfo->trg = trg_temp;
    mark_oldkey_clr(key_temp);
    ret = mark_making();
    mark_disp();
    if (--minfo->renewal_time <= 0) {
        minfo->renewal_time = 0;
    }
    if (ret == 0) {
        minfo->disp_time--;
    }
    if (minfo->disp_time <= 0) {
        mark_init_clear();
        mark_init_sort();
        markbuf[0] = mdata;
        minfo->disp_time = 0;
        markbuf[1] = NULL;
    }
}

int mark_making(void)
{
    int ret;

    ret = mark_make_key(minfo->key);
    trg_wait_frame(minfo->trg);
    if (minfo->trg_time >= 3) {
        ret |= mark_make_button(minfo->trg_douji);
    }
    if (ret) {
        if (minfo->renewal_time == 0) {
            minfo->renewal_time = 60;
        } else {
            minfo->renewal_time += 15;
        }
        if (minfo->renewal_time > 60) {
            minfo->renewal_time = 60;
        }
        minfo->disp_time = 240;
    }
    return ret;
}

int mark_make_key(u_int key)
{
    MARKDATA *m;

    key &= 0xFFFFF700;
    if (key == 0 || chk_oldkey(key, 1)) {
        return 0;
    }
    chk_renewal();
    while ((m = get_mark()) == NULL) {
        free_mark(markbuf[1]);
    }
    m->key = key;
    m->id = 1;
    if (key & 0x1000) {
        if (key & 0x8000) {
            m->buf[0] = '7';
        } else if (key & 0x2000) {
            m->buf[0] = '9';
        } else {
            m->buf[0] = '8';
        }
    } else if (key & 0x4000) {
        if (key & 0x8000) {
            m->buf[0] = '1';
        } else if (key & 0x2000) {
            m->buf[0] = '3';
        } else {
            m->buf[0] = '2';
        }
    } else if (key & 0x8000) {
        m->buf[0] = '4';
    } else if (key & 0x2000) {
        m->buf[0] = '6';
    }
    return 1;
}

int chk_oldkey(u_int key, int id)
{
    MARKDATA *m = markbuf[1];

    if (m == NULL) {
        return 0;
    }
    while (m->next != NULL) {
        m = m->next;
    }
    while (m != NULL && m->id != id) {
        m = m->prev;
    }
    return m != NULL && m->key == key;
}

void mark_disp(void)
{
    MARKDATA *m;
    int i;

    for (i = 0, m = markbuf[1]; m != NULL; m = m->next, i++) {
        MarkDraw(m->buf, i * 32 + 16, 400, 4, 1);
    }
}

void mark_init_main(void)
{
    if (minfo == NULL) {
        minfo = mmalloc(0x18, "minfo");
        mdata = mmalloc(0x17C, "mdata");
    }
    mark_init_info();
    mark_init_clear();
    mark_init_sort();
    markbuf[0] = mdata;
    markbuf[1] = NULL;
}

void mark_init_info(void)
{
    minfo->disp_time = 0;
    minfo->renewal_time = 0;
    minfo->key = 0;
    minfo->trg_time = 0;
    minfo->trg_douji = 0;
}

void mark_init_sort(void)
{
    int i;

    mdata[0].prev = NULL;
    mdata[0].next = &mdata[1];
    for (i = 1; i < 18; i++) {
        mdata[i].prev = &mdata[i - 1];
        mdata[i].next = &mdata[i + 1];
    }
    mdata[18].next = NULL;
    mdata[18].prev = &mdata[17];
}

void link_mark_busy(MARKDATA *m)
{
    MARKDATA *p = markbuf[1];

    if (p != NULL) {
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = m;
        m->prev = p;
    } else {
        markbuf[1] = m;
        m->prev = NULL;
    }
    m->next = NULL;
}

void free_mark(MARKDATA *m)
{
    if (m->prev == NULL) {
        if (m->next != NULL) {
            m->next->prev = NULL;
            markbuf[1] = m->next;
        } else {
            markbuf[1] = NULL;
        }
    } else {
        m->prev->next = m->next;
        if (m->next != NULL) {
            m->next->prev = m->prev;
        }
    }
    mark_dataclear(m);
    link_mark_blank(m);
}

void link_mark_blank(MARKDATA *m)
{
    MARKDATA *p = markbuf[0];

    if (p != NULL) {
        m->next = p;
        p->prev = m;
    } else {
        markbuf[0] = m;
        m->next = NULL;
    }
    m->prev = NULL;
}

void mark_init_clear(void)
{
    int i;

    for (i = 0; i < 19; i++) {
        mark_dataclear(&mdata[i]);
    }
}

void mark_dataclear(MARKDATA *m)
{
    m->next = NULL;
    m->prev = NULL;
    m->buf[0] = 0;
    m->buf[1] = 0;
    m->buf[2] = 0;
    m->buf[3] = 0;
    m->id = 0;
    m->key = 0;
}

void mark_oldkey_clr(u_int key)
{
    MARKDATA *m;

    if (key == 0) {
        for (m = markbuf[1]; m != NULL; m = m->next) {
            m->key = 0;
        }
    }
}

void chk_renewal(void)
{
    if (minfo->renewal_time == 0) {
        mark_init_clear();
        mark_init_sort();
        markbuf[0] = mdata;
        minfo->renewal_time = 60;
        markbuf[1] = NULL;
    }
}

void trg_wait_frame(u_int trg)
{
    trg &= 0xFFFF07FF;
    if (trg) {
        minfo->trg_douji |= trg;
        minfo->trg_time++;
    } else if (minfo->trg_time) {
        minfo->trg_time++;
    }
}

void training_execute_prac(void)
{
    CHARACTER_WORK *offset;
    PRACLIST *prac_list_temp;
    PRACDATA *prac_data_temp;
    u_int key;

    offset = &PlayerWork[tr_info->ply];
    prac_list_temp = prac_all[offset->idno].list;
    prac_data_temp = prac_list_temp[tr_info->pracno].data;
    if (tr_info->praccnt == -1) {
        return;
    }
    key = training_execute_prac_sub(prac_data_temp[tr_info->pracidx].key);
    if (pads[tr_info->ply].data & offset->ply->MemoryKey) {
        key |= offset->ply->MemoryKey;
    }
    if (pads[tr_info->ply].data & offset->ply->Fuckin) {
        key |= offset->ply->Fuckin;
    }
    if (pads[tr_info->ply].data & 0x800) {
        key |= 0x800;
    }
    if (tr_info->pracidx == 0 && tr_info->praccnt == 0) {
        tr_info->iobkup[0] = tr_info->iobkup[1] | (offset->ply->Fuckin | offset->ply->MemoryKey | 0x800);
    } else {
        tr_info->iobkup[0] = tr_info->iobkup[1];
    }
    tr_info->iobkup[2] = ~tr_info->iobkup[0] & key;
    tr_info->iobkup[3] = tr_info->iobkup[0] & ~key;
    tr_info->iobkup[1] = key;
    pads[tr_info->ply].old = tr_info->iobkup[0];
    pads[tr_info->ply].data = tr_info->iobkup[1];
    pads[tr_info->ply].trg = tr_info->iobkup[2];
    pads[tr_info->ply].offtrg = tr_info->iobkup[3];
    if (++tr_info->praccnt >= prac_data_temp[tr_info->pracidx].frame) {
        tr_info->praccnt = 0;
        tr_info->pracidx++;
        if (prac_data_temp[tr_info->pracidx].frame == -1) {
            tr_info->pracidx = -1;
            tr_info->praccnt = -1;
        }
    }
}

u_int training_execute_prac_sub(u_int key)
{
    int ret;
    u_int key_buf;
    CHARACTER_WORK *offset;

    offset = &PlayerWork[tr_info->ply];
    key_buf = 0;
    if (key & 1) {
        key_buf |= offset->ply->PunchKey;
    }
    if (key & 2) {
        key_buf |= offset->ply->KickKey;
    }
    if (key & 4) {
        key_buf |= offset->ply->SlashKey;
    }
    if (key & 8) {
        key_buf |= offset->ply->HighSlashKey;
    }
    if (key & 0x10) {
        key_buf |= offset->ply->Fuckin;
    }
    if (offset->dirflag == 0) {
        if (key & 0x100) {
            key_buf |= 0x2000;
        }
        if (key & 0x200) {
            key_buf |= 0x8000;
        }
    } else {
        if (key & 0x100) {
            key_buf |= 0x8000;
        }
        if (key & 0x200) {
            key_buf |= 0x2000;
        }
    }
    if (key & 0x400) {
        key_buf |= 0x1000;
    }
    if (key & 0x800) {
        key_buf |= 0x4000;
    }
    ret = key_buf;
    return ret;
}

u_int training_execute_prac_sub_gallery(u_int key)
{
    int ret;
    u_int key_buf;
    CHARACTER_WORK *offset;

    offset = &PlayerWork[gl_info->ply];
    key_buf = 0;
    if (key & 1) {
        key_buf |= offset->ply->PunchKey;
    }
    if (key & 2) {
        key_buf |= offset->ply->KickKey;
    }
    if (key & 4) {
        key_buf |= offset->ply->SlashKey;
    }
    if (key & 8) {
        key_buf |= offset->ply->HighSlashKey;
    }
    if (key & 0x10) {
        key_buf |= offset->ply->Fuckin;
    }
    if (offset->dirflag == 0) {
        if (key & 0x100) {
            key_buf |= 0x2000;
        }
        if (key & 0x200) {
            key_buf |= 0x8000;
        }
    } else {
        if (key & 0x100) {
            key_buf |= 0x8000;
        }
        if (key & 0x200) {
            key_buf |= 0x2000;
        }
    }
    if (key & 0x400) {
        key_buf |= 0x1000;
    }
    if (key & 0x800) {
        key_buf |= 0x4000;
    }
    ret = key_buf;
    return ret;
}

void prac_mark_disp(void)
{
    int buf_max;
    int i;
    int idx[64];
    int alpha;
    int cmp[64];
    u_int key;
    u_int key_buf;
    CHARACTER_WORK *offset;
    PRACLIST *prac_list_temp;
    PRACDATA *prac_data_temp;
    char buf[64][4];
    int state_chk;
    int state_max;
    int state[8];

    buf_max = 0;
    for (i = 0; i < 64; i++) {
        idx[i] = 0;
        cmp[i] = 0;
    }
    offset = &PlayerWork[tr_info->ply];
    prac_list_temp = prac_all[offset->idno].list;
    prac_data_temp = prac_list_temp[tr_info->pracno].data;
    state_max = 0;
    state_chk = 1;
    for (i = 0; i < 8; i++) {
        if (prac_list_temp[tr_info->pracno].state & state_chk) {
            state[i] = 1;
            state_max++;
        } else {
            state[i] = 0;
        }
        state_chk <<= 1;
    }
    for (i = 0; prac_data_temp[i].frame != -1; i++) {
        key_buf = prac_data_temp[i].key;
        if (i > 0) {
            key = prac_data_temp[i - 1].key;
            key &= 0xF00;
            key_buf &= 0xF00;
            if (key == key_buf) {
                key_buf = prac_data_temp[i].key & 0xFFFFF0FF;
            } else {
                key_buf = prac_data_temp[i].key;
            }
        }
        key = training_execute_prac_sub(key_buf);
        key_buf = key;
        if (key) {
            key &= 0xF800;
            if (key_buf & offset->ply->PunchKey) {
                key |= 0x80;
            }
            if (key_buf & offset->ply->KickKey) {
                key |= 0x40;
            }
            if (key_buf & offset->ply->SlashKey) {
                key |= 0x10;
            }
            if (key_buf & offset->ply->HighSlashKey) {
                key |= 0x20;
            }
            if (key_buf & offset->ply->Fuckin) {
                key |= 1;
            }
            prac_mark_init_buf(buf[buf_max]);
            if ((key & 0xF000) && prac_mark_make_key(key, buf[buf_max])) {
                idx[buf_max] = i;
                buf_max++;
            }
            prac_mark_init_buf(buf[buf_max]);
            if ((key & 0xFFFF0FFF) && prac_mark_make_button(key, buf[buf_max])) {
                idx[buf_max] = i;
                buf_max++;
            }
            prac_mark_init_buf(buf[buf_max]);
        }
    }
    if (Pause == 1 || Pause == 2) {
        if (buf_max <= 0) {
            return;
        }
        for (i = 0; i < buf_max; i++) {
            MarkDraw(buf[i], (i % 19) * 32 | 0x10, (i / 19) * 32 + 0x170, 2, 1);
        }
        if (state_max > 0) {
            for (i = 0; i < 8; i++) {
                if (state[i] && state_max > 0) {
                    StateDraw(i, 0x270 - state_max * 32, 0x14C, 2, 1);
                    state_max--;
                }
            }
        }
        return;
    }
    if (buf_max <= 0) {
        return;
    }
    polydraw_FBox(8.0f, 364.0f, 664.0f, 430.0f, 12.0f, gStripGetFadeColor(0xC0100000));
    for (i = 0; i < buf_max; i++) {
        if (tr_info->praccnt == -1) {
            alpha = 0xA0;
            if (tr_info->pj.idx >= idx[i]) {
                alpha = 1;
            }
        } else {
            if (tr_info->pracidx >= idx[i]) {
                cmp[i] = 1;
            } else {
                cmp[i] = 0;
            }
            alpha = 1;
        }
        MarkDraw(buf[i], (i % 19) * 32 | 0x10, (i / 19) * 32 + 0x170, 4, alpha);
    }
    if (tr_info->praccnt != -1) {
        for (i = 0; i < buf_max; i++) {
            if (cmp[i] && !cmp[i + 1]) {
                StateDraw(8, (i % 19) * 32 | 0x10, (i / 19) * 32 + 0x170, 4, 1);
            }
        }
    }
    if (state_max > 0) {
        for (i = 0; i < 8; i++) {
            if (state[i] && state_max > 0) {
                StateDraw(i, 0x270 - state_max * 32, 0x14C, 2, 1);
                state_max--;
            }
        }
    }
}

void prac_mark_disp_gallery(void)
{
    int buf_max;
    int i;
    int idx[64];
    int cmp[64];
    u_int key;
    u_int key_buf;
    CHARACTER_WORK *offset;
    PRACLIST *prac_list_temp;
    PRACDATA *prac_data_temp;
    char buf[64][4];
    int state_chk;
    int state_max;
    int state[8];

    buf_max = 0;
    if (gl_info->clear & 2) {
        int valtb[2];

        valtb[0] = gl_info->num;
        FontDrawDC(G_sprintf(NULL, "%3d", valtb), 256.0f, 80.0f, 2.0f, 1, 1);
        FontDrawDC("BEAT", 304.0f, 80.0f, 2.0f, 1, 1);
    }
    if (gl_info->combo == -1) {
        return;
    }
    for (i = 0; i < 64; i++) {
        idx[i] = 0;
        cmp[i] = 0;
    }
    offset = &PlayerWork[gl_info->ply];
    prac_list_temp = prac_all[offset->idno].list;
    prac_data_temp = prac_list_temp[gl_info->combo].data;
    state_max = 0;
    state_chk = 1;
    for (i = 0; i < 8; i++) {
        if (prac_list_temp[gl_info->combo].state & state_chk) {
            state[i] = 1;
            state_max++;
        } else {
            state[i] = 0;
        }
        state_chk <<= 1;
    }
    for (i = 0; prac_data_temp[i].frame != -1; i++) {
        key_buf = prac_data_temp[i].key;
        if (i > 0) {
            key = prac_data_temp[i - 1].key;
            key &= 0xF00;
            key_buf &= 0xF00;
            if (key == key_buf) {
                key_buf = prac_data_temp[i].key & 0xFFFFF0FF;
            } else {
                key_buf = prac_data_temp[i].key;
            }
        }
        key = training_execute_prac_sub_gallery(key_buf);
        key_buf = key;
        if (key) {
            key &= 0xF800;
            if (key_buf & offset->ply->PunchKey) {
                key |= 0x80;
            }
            if (key_buf & offset->ply->KickKey) {
                key |= 0x40;
            }
            if (key_buf & offset->ply->SlashKey) {
                key |= 0x10;
            }
            if (key_buf & offset->ply->HighSlashKey) {
                key |= 0x20;
            }
            if (key_buf & offset->ply->Fuckin) {
                key |= 1;
            }
            prac_mark_init_buf(buf[buf_max]);
            if ((key & 0xF000) && prac_mark_make_key(key, buf[buf_max])) {
                idx[buf_max] = i;
                buf_max++;
            }
            prac_mark_init_buf(buf[buf_max]);
            if ((key & 0xFFFF0FFF) && prac_mark_make_button(key, buf[buf_max])) {
                idx[buf_max] = i;
                buf_max++;
            }
            prac_mark_init_buf(buf[buf_max]);
        }
    }
    if (buf_max <= 0) {
        return;
    }
    if (!(Pause == 1 || Pause == 2)) {
        polydraw_FBox(8.0f, 364.0f, 664.0f, 430.0f, 12.0f, gStripGetFadeColor(0xC0100000));
    }
    for (i = 0; i < buf_max; i++) {
        MarkDraw(buf[i], (i % 19) * 32 | 0x10, (i / 19) * 32 + 0x170, 2, 1);
    }
    if (state_max > 0) {
        for (i = 0; i < 8; i++) {
            if (state[i] && state_max > 0) {
                StateDraw(i, 0x270 - state_max * 32, 0x14C, 2, 1);
                state_max--;
            }
        }
    }
}

int prac_mark_make_key(u_int key, char *buf)
{
    key &= 0xF000;
    if (key == 0) {
        return 0;
    }
    if (key & 0x1000) {
        if (key & 0x8000) {
            *buf = '7';
        } else if (key & 0x2000) {
            *buf = '9';
        } else {
            *buf = '8';
        }
    } else if (key & 0x4000) {
        if (key & 0x8000) {
            *buf = '1';
        } else if (key & 0x2000) {
            *buf = '3';
        } else {
            *buf = '2';
        }
    } else if (key & 0x8000) {
        *buf = '4';
    } else if (key & 0x2000) {
        *buf = '6';
    }
    return 1;
}

int prac_mark_make_button(u_int button, char *buf)
{
    int data;
    int i;

    button &= 0xF1;
    if (button == 0) {
        return 0;
    }
    data = 0;
    if (button & 0x20) {
        data |= 1;
    }
    if (button & 0x40) {
        data |= 2;
    }
    if (button & 0x80) {
        data |= 4;
    }
    if (button & 0x10) {
        data |= 8;
    }
    if (data) {
        data += 0x60;
        buf[0] = data;
        i = 1;
    } else {
        i = 0;
    }
    if (button & 1) {
        data = 0x44;
        buf[i] = data;
    }
    return 1;
}

void prac_mark_init_buf(char *buf)
{
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
}

void prac_judge_get_next(int flg)
{
    int idx;
    int frame;
    u_int actno;
    CHARACTER_WORK *offset;
    PRACLIST *prac_list_temp;
    PRACDATA *prac_data_temp;

    offset = &PlayerWork[tr_info->ply];
    prac_list_temp = prac_all[offset->idno].list;
    prac_data_temp = prac_list_temp[tr_info->pracno].data;
    if (flg) {
        tr_info->pj.idx = tr_info->pj.idxnext;
    } else {
        tr_info->pj.idx = -1;
    }
    idx = tr_info->pj.idx + 1;
    while (1) {
        frame = prac_data_temp[idx].frame;
        actno = prac_data_temp[idx].actno;
        if (frame == -1 || actno != 0) {
            break;
        }
        idx++;
    }
    if (frame == -1) {
        tr_info->pj.idxnext = -1;
        tr_info->pj.actno = 0;
        tr_info->pj.cnt = 0;
    } else if (actno) {
        tr_info->pj.idxnext = idx;
        tr_info->pj.actno = actno;
        tr_info->pj.cnt = 0;
    }
}

int prac_judge_cmp_next(void)
{
    int ret;
    u_int actno;
    u_int actnonext;
    u_int actnost;
    u_int actnonextst;

    ret = 0;
    actno = tr_info->pj.actno;
    actnonext = tr_info->pj.actnonext;
    actnost = actno & 0xFFFF0000;
    actnonextst = actnonext & 0xFFFF0000;
    actno &= 0xFFFF;
    actnonext &= 0xFFFF;
    if (actno == actnonext) {
        if (actnost) {
            ret = actnost == (actnost & actnonextst);
        } else {
            ret = 1;
        }
    }
    return ret;
}

void prac_judge_main(void)
{
    CHARACTER_WORK *offset;

    offset = &PlayerWork[tr_info->ply];
    tr_info->pj.actnonext = offset->actnonexttemp;
    tr_info->pj.cnt++;
    if (tr_info->pj.idxnext != -1) {
        if (prac_judge_cmp_next()) {
            prac_judge_get_next(1);
        }
    }
    if (tr_info->pj.idxnext != -1) {
        if (tr_info->pj.cnt >= (tr_info->pj.idxnext - tr_info->pj.idx) * 32 + 128) {
            prac_judge_get_next(0);
        }
    } else if (tr_info->pj.cnt >= 128) {
        prac_judge_get_next(0);
    }
    offset->actnonexttemp = 0;
}

void prac_input_main(void)
{
    int i;
    int x;
    int y;
    int valtb[2];
    CHARACTER_WORK *offset;

    offset = &PlayerWork[tr_info->ply];
    if (++tr_info->debugbuf[0] >= 1000) {
        tr_info->debugbuf[0] = 999;
    }
    if (IsTrgPunch(offset) | IsTrgKick(offset) | IsTrgSlash(offset) | IsTrgHighSlash(offset) | IsTrgFuckin(offset)) {
        for (i = 18; i >= 0; i--) {
            tr_info->debugbuf[i + 1] = tr_info->debugbuf[i];
        }
        tr_info->debugbuf[0] = 0;
    }
    for (i = 0; i < 20; i++) {
        x = (i % 10) * 48 + 32;
        y = (i / 10) * 32 + 0x134;
        valtb[0] = tr_info->debugbuf[i];
        FontDraw(G_sprintf(NULL, "%3d", valtb), x, y, 1.0f, 0xFF);
    }
}

void StateDraw(int no, int x, int y, int z, int alpha)
{
    int u;
    int v;
    NL_SPR_ARG sprite;

    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.zm_x = 32.0f;
    sprite.zm_y = 32.0f;
    sprite.ang = 0;
    if (no == 0) {
        u = 5;
        v = 0;
    } else if (no == 1) {
        u = 5;
        v = 1;
    } else if (no == 2) {
        u = 5;
        v = 2;
    } else if (no == 3) {
        u = 5;
        v = 3;
    } else if (no == 4) {
        u = 6;
        v = 0;
    } else if (no == 5) {
        u = 6;
        v = 1;
    } else if (no == 6) {
        u = 6;
        v = 2;
    } else if (no == 7) {
        u = 6;
        v = 3;
    } else if (no == 8) {
        u = 3;
        v = 3;
    } else {
        return;
    }
    sprite.sprno = fi.font[3].spno;
    u = u * 32.0f;
    v = v * 32.0f;
    sprite.u0 = u;
    sprite.v0 = v;
    sprite.u1 = u + 32.0f;
    sprite.v1 = v + 32.0f;
    gSpriteDrawPS(&sprite);
}

MARKDATA *get_mark(void)
{
    MARKDATA *p = markbuf[0];

    if (p == NULL) {
        return NULL;
    }
    if (p->next != NULL) {
        markbuf[0] = p->next;
        markbuf[0]->prev = NULL;
    } else {
        markbuf[0] = NULL;
    }
    mark_dataclear(p);
    link_mark_busy(p);
    return p;
}

extern GAME_INFOMATION g_info;

int get_dekustat(CHARACTER_WORK *offset)
{
    if ((g_info.mode & 0x100) && offset->padid != tr_info->ply && tr_info->tene.mode == 0 && tr_info->tene.memflg != 2 && tr_info->tene.memflg != 3) return 0;
    return 1;
}

int mark_make_button(u_int button)
{
    MARKDATA *m;
    int data = 0;
    int i;

    button &= 0xFFFF07FF;
    minfo->trg_time = 0;
    minfo->trg_douji = 0;
    if (button == 0 || chk_oldkey(button, 2)) {
        return 0;
    }
    chk_renewal();
    while ((m = get_mark()) == NULL) {
        free_mark(markbuf[1]);
    }
    m->id = 2;
    m->key = button;
    if (button & 0x20) {
        data |= 1;
    }
    if (button & 0x40) {
        data |= 2;
    }
    if (button & 0x80) {
        data |= 4;
    }
    if (button & 0x10) {
        data |= 8;
    }
    if (data) {
        data += 0x60;
        m->buf[0] = data;
        i = 1;
    } else {
        i = 0;
    }
    if (button & 4) {
        data = 'A';
        m->buf[i] = data;
        i++;
    }
    if (button & 8) {
        data = 'B';
        m->buf[i] = data;
        i++;
    }
    if (button & 1) {
        data = 'D';
        m->buf[i] = data;
        i++;
    }
    if (button & 2) {
        data = 'H';
        m->buf[i] = data;
    }
    return 1;
}

void training_disp_attackcnt(void)
{
    int cnt;
    int x;
    int y;
    int t;
    char buf[4];

    x = 0x114;
    y = 136;
    if (tr_info->tene.memflg == 0 && tr_info->tene.type == 3) {
        cnt = 300 - tr_info->attackcnt;
        set_char2num(cnt / 60, buf, 0);
        if (cnt / 60 < 10) t = 0x128; else t = x;
        FontDrawDC(buf, t, y, 4.0f, 1, 2);
        FontDrawDC(":", 322.0f, y, 4.0f, 1, 2);
        set_char2num(cnt % 60, buf, 2);
        FontDrawDC(buf, 334.0f, y, 4.0f, 1, 2);
    } else {
        tr_info->attackcnt = 0;
    }
}
