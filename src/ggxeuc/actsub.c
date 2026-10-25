#include "generated/actsub.h"
#include "generated/act.h"
#include "generated/objwork.h"
#include "generated/main.h"
#include "generated/effect.h"
#include "generated/key.h"
#include "generated/actsub2.h"

static const short idtb[] = { 1, 2, 3 };

extern short PDJumpCount[];
extern short PDSkyDashCount[];

extern char D_5BF268[]; /* RODATA_EXTERN 0x5bf268: "SetActNoNext Danger\n" */

void SetActNoNext(CHARACTER_WORK *offset, unsigned int no) {
    if (offset == 0) {
        debugf(D_5BF268);
        return;
    }
    if (offset->actnonext == 0xFFFFFFFF) offset->actnonext = no;
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no) {
    if (offset->InstSt & 0x20) {
        SetAnimeNo(offset, no);
        offset->InstSt &= ~0x20;
        return 1;
    }
    return 0;
}

int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset) {
    return SetChangeFrameAnime(offset, offset->actno);
}

void SetXSpeed(CHARACTER_WORK *offset, int xspeed) {
    if (offset->dirflag == 0) {
        offset->xspeed = -xspeed;
        return;
    }
    offset->xspeed = xspeed;
}

void AddXSpeedCmp(CHARACTER_WORK *offset, short xspeed, int power) {
    int val;
    if (offset->HitStopTime == 0) {
        if (offset->dirflag == 0) xspeed = -xspeed;
        val = (xspeed - offset->xspeed) * power;
        val /= 100;
        offset->xspeed += val;
    }
}

void AddXSpeed(CHARACTER_WORK *offset, short xspeed) {
    if (offset->HitStopTime == 0) {
        if (offset->dirflag == 0) offset->xspeed -= xspeed;
        else offset->xspeed += xspeed;
    }
}

void ParXSpeed(CHARACTER_WORK *offset, float par) {
    if (offset->HitStopTime == 0) offset->xspeed *= par;
}

void AddYSpeed(CHARACTER_WORK *offset, short yspeed) {
    if (offset->HitStopTime == 0) {
        offset->yspeed += yspeed;
    }
}

void ParYSpeed(CHARACTER_WORK *offset, float par) {
    if (offset->HitStopTime == 0) offset->yspeed *= par;
}

void ParInertia(CHARACTER_WORK *offset, float par) {
    if (offset->HitStopTime == 0) offset->ply->inertia *= par;
}

void SetADHitBackXSpeed(CHARACTER_WORK *offset, short xspeed) {
    if (offset->dirflag == 0) {
        offset->AdBackx = (int) -xspeed;
        return;
    }
    offset->AdBackx = (int) xspeed;
}

CHARACTER_WORK *GetOtherPlayer(CHARACTER_WORK *offset) {
    if (offset->actst & 2) return PlayerWork + 1;
    return PlayerWork;
}

void DirectionDirectChange(CHARACTER_WORK *offset) {
    int enemy_x;
    int x;

    enemy_x = GetOtherPlayer(offset)->posx;
    x = offset->posx;
    if (x < enemy_x) {
        offset->dirflag = 1;
        return;
    }
    if (enemy_x < x) {
        offset->dirflag = 0;
    }
}

void DirectionEnemyReverse(CHARACTER_WORK *offset) {
    offset->dirflag = GetOtherPlayer(offset)->dirflag == 0;
}

void DirectionEnemySame(CHARACTER_WORK *offset) {
    offset->dirflag = GetOtherPlayer(offset)->dirflag;
}

void SetPriority(CHARACTER_WORK *offset, int PriAdd) { SetPriorityMain(offset, PriAdd, 3); }

void SetPriorityMain(CHARACTER_WORK *offset, int PriAdd, int Mode) {
    short int pri;

    if (offset->SpriteFlag & 2) {
        pri = PriAdd;
    } else {
        if (offset->idno < 0x20) {
            pri = PriAdd + 0xB4;
        } else if (offset->idno < 0x61) {
            pri = PriAdd + 0xA0;
        } else if (offset->idno < 0x7E) {
            pri = PriAdd + 0x87;
        } else {
            pri = PriAdd + 0x104;
        }
        if (offset->actst & 4) {
            pri += 7;
        }
    }
    if (Mode & 1) {
        offset->priority = pri;
    }
    if (Mode & 2) {
        if (Mode & 1) {
            offset->priority2 = pri - 2;
        } else {
            offset->priority2 = pri;
        }
    }
}


int GetPlayerDist(void) {
    int ret = PlayerWork[0].posx - PlayerWork[1].posx;
    return ret <= -1 ? -ret : ret;
}

int GetPlayerDistOff(CHARACTER_WORK *offset) {
    int distance = PlayerWork[offset->padid == 0 ? 1 : 0].posx - PlayerWork[offset->padid].posx;
    return offset->dirflag == 0 ? -distance : distance;
}

int GetPlayerDisty(void) {
    int ret = PlayerWork[0].posy - PlayerWork[1].posy;
    return ret <= -1 ? -ret : ret;
}

int GetPlayerDistyOff(CHARACTER_WORK *offset) {
    return PlayerWork[offset->padid == 0 ? 1 : 0].posy - PlayerWork[offset->padid].posy;
}

void SetInertia(CHARACTER_WORK *offset) {
    PLAYER_ENTRY *ply;

    ply = offset->ply;
    ply->inertia = offset->xspeed;
    ply->inertiaflag |= 2;
}

int IsChase(void) {
    int i;
    for (i = 0; i < 2; i++) if (PlayerWork[i].ply->homingtime > 0) return 1;
    return 0;
}

int IsBattleEnd(CHARACTER_WORK *offset) {
    if (offset->DamColliFlag) return 0;
    if (offset->actst & 0x40050000) {
        if (offset->ply->KeyHook == 0) return 1;
    }
    return 0;
}

int IsKirimomi(CHARACTER_WORK *offset) {
    return GetEnemyPlayer(offset)->actno == 0x34;
}

void CreatePerfectGuardEffect(CHARACTER_WORK *offset) {
    CREATE_ARG arg;
    if (offset->GuardSt & 8) {
        arg.flag = 8;
        EffectCreate(offset, 0x73, &arg);
    }
}

void PerfectGuardOnCheck(CHARACTER_WORK *offset) {
    if (IsPG(offset)) offset->GuardSt |= 8;
    else offset->GuardSt &= ~8;
}

int GetEntryOrder(void) {
    static char table[12] = {2, 1, 16, 1, 3, 14, 4, 8, 1, 5, 8, 15};
    int i;
    for (i = 0; i < 6; i++) {
        if (PlayerWork[0].idno == table[i * 2] && PlayerWork[1].idno == table[i * 2 + 1]) return 1;
    }
    return 0;
}

int WinActionSet(CHARACTER_WORK *offset) {
    unsigned short temp_v0;

    temp_v0 = offset->actnoSv;
    if (temp_v0 != 0) {
        SetActNoNext(offset, (unsigned int) temp_v0);
    }
    return (int) offset->actnoSv;
}

void SetSemiTrans(CHARACTER_WORK *offset, char trans, short transmode) {
    offset->trans = trans;
    offset->transmode = transmode;
}

void SetNormalBegin(CHARACTER_WORK *offset) {
    if (offset->ActTimer < 2 && (~offset->attackst & 0x200000)) {
        offset->attackst |= 0x200;
    } else {
        offset->attackst &= ~0x200;
    }
}

void SetDustBegin(CHARACTER_WORK *offset) {
    if (offset->ActTimer < 2 && (~offset->attackst & 0x200000)) {
        offset->attackst |= 0x400000;
    } else {
        offset->attackst &= ~0x400000;
    }
}

void ObjIndependence(CHARACTER_WORK *offset) {
    offset->parentFlag &= 0x200;
}

void ObjParentCutOff(CHARACTER_WORK *offset) {
    offset->parentFlag &= 0xFF7F;
}

void SetParentPosY(CHARACTER_WORK *offset, short pos) {
    CHARACTER_WORK *parent = offset->parentWork;
    offset->parenty = pos;
    offset->posy = parent->posy + pos * 100;
}

void OutSideEliminate(CHARACTER_WORK *offset) {
    double x = offset->posx;
    if (x <= -95000.0) offset->actst |= 0x80000000;
    if (x >= 95000.0) offset->actst |= 0x80000000;
}

void OutSideEliminateJust(CHARACTER_WORK *offset) {
    double x = offset->posx;
    if (x <= -77520.0) offset->actst |= 0x80000000;
    if (x >= 77520.0) offset->actst |= 0x80000000;
}

void SetSkyJumpCount(CHARACTER_WORK *offset) {
    offset->ply->JumpCount = PDJumpCount[offset->padid];
}

void SetSkyDashCount(CHARACTER_WORK *offset) {
    offset->ply->SkyDashCount = PDSkyDashCount[offset->padid];
}

int GetXMove(CHARACTER_WORK *offset, int val) {
    if (offset->dirflag == 0) return -val;
    return val;
}

void MovePosX(CHARACTER_WORK *offset, int val) {
    if (offset->dirflag == 0) offset->posx -= val;
    else offset->posx += val;
}

void MovePosY(CHARACTER_WORK *offset, int val) {
    offset->posy += val;
}

void RenewalCollision(CHARACTER_WORK *offset) {
    if ((unsigned char) offset->ActHitCount < (unsigned char) offset->MaxHitCount) {
        offset->actst &= 0xFFFFFFBF;
    }
}

short unsigned int DirectionDirectGet(CHARACTER_WORK *offset)
{
    CHARACTER_WORK *enemy = GetOtherPlayer(offset);

    if (offset->posx < enemy->posx) {
        return 1;
    }
    if (enemy->posx < offset->posx) {
        return 0;
    }
    return 2;
}

void ParentPosLink(CHARACTER_WORK *offset, CHARACTER_WORK *poffset) {
    if (offset->dirflag == 1) {
        offset->posx = poffset->posx - offset->parentx * 100;
    } else {
        offset->posx = poffset->posx + offset->parentx * 100;
    }
    offset->posy = poffset->posy + offset->parenty * 100;
}

int Random(void);

void RandMove(CHARACTER_WORK *offset, unsigned int val) {
    offset->posx += ((Random() & (val * 2 - 1)) - val) * 100;
    offset->posy += ((Random() & (val * 2 - 1)) - val) * 100;
}

int Tension_Cmp(CHARACTER_WORK *offset, int Val);
extern int BattleStatus;

void PerfectGuardEnableCheck(CHARACTER_WORK *offset) {
    if (!Tension_Cmp(offset, 1) || BattleStatus != 2) {
        offset->GuardSt &= 0xFFB7;
    } else {
        offset->GuardSt |= 0x40;
    }
}

extern short unsigned int EntryActNoTb[][8];
int EnterActionSet(CHARACTER_WORK *offset) {
    if (offset->actnoSv == 0xFFFF) {
        return 0;
    }
    SetActNoNext(offset, EntryActNoTb[offset->idno][offset->actnoSv] | 0x200000);
    return 1;
}

extern short unsigned int TimeOutLoseActNoTb[];
int LoseActionSet(CHARACTER_WORK *offset) {
    if (offset->actst & 0x10000) {
        return 0;
    }
    if (!(offset->actst & 0x40000)) {
        return 0;
    }
    SetActNoNext(offset, TimeOutLoseActNoTb[offset->idno]);
    return TimeOutLoseActNoTb[offset->idno];
}

void GetColliCenter(CHARACTER_WORK *offset, COLLISION *colli, COLLIPOS *pos) {
    if (offset->dirflag == 1) {
        pos->x = offset->posx - (colli->x + (colli->w >> 1)) * 100;
    } else {
        pos->x = offset->posx + (colli->x + (colli->w >> 1)) * 100;
    }
    pos->y = offset->posy + (colli->y + (colli->h >> 1)) * 100;
}

RECTL *GetScreenRect(void);

void ScreenSideEliminate(CHARACTER_WORK *offset) {
    RECTL *rect = GetScreenRect();

    if (offset->dirflag == 1) {
        if (offset->posx < rect->x + rect->w - 2000) {
            return;
        }
    } else {
        if (rect->x + 2000 < offset->posx) {
            return;
        }
    }
    offset->actst |= 0x80000000;
}

int IsITIGEKI(CHARACTER_WORK *offset);

int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE) {
    int (*check)(CHARACTER_WORK *);
    int (*check2)(CHARACTER_WORK *);

    if (offset->ply->OverKillFlag == 1) {
        check = functb[0];
        check2 = functb[1];
        if (check(offset) && check2(offset)) {
            SetAttackActNo(offset, actITIGEKI | 0x2080000);
            return 1;
        }
    } else if (offset->ply->OverKillFlag) {
        return 0;
    }
    if (IsITIGEKI(offset)) {
        SetAttackActNo(offset, actOVERKILLMODE | 0x2080000);
        return 1;
    }
    return 0;
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no) {
    if (no & 0x1000000) {
        offset->SpriteFlag |= 0x200;
    }
    if (no & 0x2000000) {
        if (offset->attackst & 0x20000) {
            return 0;
        }
        offset->attackst |= 0x10000;
    } else if (no & 0x4000000) {
        if (offset->attackst & 0x20000) {
            return 0;
        }
        offset->attackst |= 0x20000;
    } else if (offset->attackst & 0x30000) {
        return 0;
    }
    if (offset->HitStopTime == 0) {
        SetActNo(offset, no);
        return 1;
    }
    offset->actnonext = no;
    return 1;
}

void SetDefaultHitVoice(CHARACTER_WORK *offset) {
    unsigned char dlv;
    unsigned char glv;
    unsigned short lv;

    lv = offset->ActHeader.lvflag;
    dlv = lv & 3;
    glv = (lv >> 2) & 3;
    if (offset->HitParam) {
        offset->HitParam->dno = dlv;
        offset->HitParam->gno = glv;
        if (dlv == 0) {
            offset->HitParam->dprob = 0x40;
        } else if (dlv == 1) {
            offset->HitParam->dprob = 0x60;
        } else {
            offset->HitParam->dprob = 0x80;
        }
        if (glv == 0) {
            offset->HitParam->gprob = 0x40;
        } else if (glv == 1) {
            offset->HitParam->gprob = 0x60;
        } else {
            offset->HitParam->gprob = 0x80;
        }
    }
}

extern short int FrontJump_XSpeedByIdno[];
extern short int BackJump_XSpeedByIdno[];
extern short int FrontHighJump_XSpeedByIdno[];
extern short int BackHighJump_XSpeedByIdno[];

void SetPlJumpSpeed(CHARACTER_WORK *offset) {
    int dir = offset->ply->JumpDir & 0xF;

    if (dir == 0) {
        SetXSpeed(offset, 0);
    } else if (dir == 1) {
        SetXSpeed(offset, FrontJump_XSpeedByIdno[offset->padid]);
    } else if (dir == 2) {
        SetXSpeed(offset, BackJump_XSpeedByIdno[offset->padid]);
    } else if (dir == 4) {
        SetXSpeed(offset, 0);
    } else if (dir == 5) {
        SetXSpeed(offset, FrontHighJump_XSpeedByIdno[offset->padid]);
    } else if (dir == 6) {
        SetXSpeed(offset, BackHighJump_XSpeedByIdno[offset->padid]);
    }
    if (offset->ply->JumpDir & 0x24) {
        offset->ply->pactst |= 1;
    }
}

void InertiaExe(CHARACTER_WORK *offset) {
    if (offset->xspeed && (offset->xspeed < 0) != (offset->ply->inertia < 0)) {
        offset->ply->inertia = 0;
    }
    if (!(offset->actst & 0x10)) {
        if (offset->ply->JumpFlag & 2) {
            return;
        }
        if (offset->ply->inertiaflag & 2) {
            if (~offset->ply->inertiatime & 0x80) {
                offset->ply->inertiatime++;
            }
            offset->ply->inertiaflag &= 0xFD;
            return;
        }
        if (offset->ply->inertiaflag & 1) {
            offset->ply->inertia -= offset->ply->inertia / 5;
        } else {
            offset->ply->inertia -= offset->ply->inertia / 10;
        }
        offset->ply->inertiatime = 0;
    }
    offset->posx += offset->ply->inertia;
    if ((unsigned int)(offset->ply->inertia + 49) < 99) {
        offset->ply->inertia = 0;
    }
    if (offset->ply->inertia == 0) {
        offset->ply->inertiaflag = 0;
    }
}

void JumpControl(CHARACTER_WORK *offset) {
    offset->actst |= 0x10;
    if (offset->HitStopTime) {
        return;
    }
    if (offset->yspeed >= 0 && offset->posy >= 0) {
        offset->posy = 0;
        offset->actst = (offset->actst | 0x50) & ~0x10;
        return;
    }
    offset->posy += offset->yspeed;
    if (offset->ply == 0) {
        offset->yspeed += offset->gravity;
        return;
    }
    if (offset->ply->maintain) {
        offset->ply->maintain--;
        if (offset->ply->SkyDashCorrect) {
            offset->ply->SkyDashCorrect--;
            if (offset->yspeed < 0) {
                offset->yspeed -= offset->yspeed / 8;
                return;
            }
        }
        offset->yspeed = 0;
        return;
    }
    offset->yspeed += offset->gravity;
    if (offset->posy >= 0) {
        offset->CommandFlag |= 0x40;
    }
}

void JumpControlMain(CHARACTER_WORK *offset) {
    offset->actst |= 0x10;
    if (offset->HitStopTime) {
        return;
    }
    offset->posy += offset->yspeed;
    if (offset->idno < 0x20 || !(offset->actst & 0x100000)) {
        if (offset->yspeed > 0 && offset->posy >= 0) {
            offset->CommandFlag |= 0x40;
            offset->actst &= ~0x10;
            offset->posy = 0;
            if (offset->GuardSt & 4) {
                offset->GuardSt |= 3;
            }
            return;
        }
    }
    if (offset->ply == 0) {
        offset->yspeed += offset->gravity;
        return;
    }
    if (offset->ply->maintain) {
        offset->ply->maintain--;
        if (offset->ply->JumpFlag & 0x10) {
            offset->ply->maintain = g_roundmin(offset->ply->maintain, 1);
        }
        if (offset->ply->maintain == 0) {
            offset->xspeed /= 2;
        }
        if (offset->ply->SkyDashCorrect && offset->yspeed < 0) {
            offset->ply->SkyDashCorrect--;
            offset->yspeed -= offset->yspeed / 8;
        } else {
            offset->ply->SkyDashCorrect = 0;
            offset->yspeed = 0;
        }
        return;
    }
    if (offset->ply->JumpFlag & 0x10) {
        return;
    }
    offset->yspeed += offset->gravity;
}

extern GAME_INFOMATION g_info;
extern SURVIVAL_INFOMATION *sv_info;
extern short int UseCharacter1p2p[];

void ParentExe(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    if (poffset == 0) {
        return;
    }
    if (offset->parentFlag == 0) {
        return;
    }
    if (offset->parentFlag & 0x80) {
        if (!(offset->parentFlag & 0x800)) {
            offset->HitStopTime = poffset->HitStopTime;
        }
        offset->ColliFlag = poffset->ColliFlag;
    }
    if (offset->parentFlag & 0x200) {
        offset->HitStopTime = poffset->HitStopTime;
    }
    if (!(g_info.mode & 0x200) || sv_info->ply == poffset->padid || UseCharacter1p2p[sv_info->ply == 0] != 9 || !sv_info->sv_ext.battle) {
        if (poffset->actst & 0x20) {
            if (offset->parentFlag & 2) {
                offset->actst |= 0x80000000;
            }
            offset->parentFlag |= 0x40;
        }
    }
    if (offset->actnoSv != poffset->actno) {
        if (offset->parentFlag & 1) {
            offset->actst |= 0x80000000;
        }
        offset->parentFlag |= 0x20;
    }
}

CHARACTER_WORK *GetFriendPlayer(CHARACTER_WORK *offset);
extern int PLAYER_HITPOINTMAX;

void SetDamage(CHARACTER_WORK *offset, short unsigned int damage) {
    CHARACTER_WORK *friend;
    CHARACTER_WORK *enemy;

    friend = GetFriendPlayer(offset);
    enemy = GetEnemyPlayer(offset);
    if (offset->ActHeader.flag & 0x8000) {
        if (enemy->HitPoint < PLAYER_HITPOINTMAX / 8) {
            damage = 0;
        }
        if (enemy->HitPoint < PLAYER_HITPOINTMAX / 4) {
            damage /= 4;
        } else if (enemy->HitPoint < PLAYER_HITPOINTMAX / 2) {
            damage /= 2;
        }
    }
    if (friend->ply) {
        if (friend->ply->AttackDown && damage) {
            offset->ActHeader.damage = g_roundmin(damage / 2, 1);
        } else {
            offset->ActHeader.damage = damage;
        }
    }
}

extern short unsigned int WinActNoTb[][8];
extern PAD pads[];
extern GAME_INFOMATION g_info;
void set_random_seed_abs(void);
int JMWinB_KeyHook(CHARACTER_WORK *offset);
int Random(void);

int WinActionDecide(CHARACTER_WORK *offset) {
    int i;
    int ano;
    CHARACTER_WORK *enemy;

    enemy = GetEnemyPlayer(offset);
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    for (i = 6; i < 8; i++) {
        if ((WinActNoTb[offset->idno][i] >> 8) == offset->actno) {
            ano = i;
            goto found;
        }
    }
    if (offset->actst & 0x40000000) {
        return 0;
    }
    for (i = 4; i < 6; i++) {
        if ((WinActNoTb[offset->idno][i] >> 8) == enemy->idno) {
            ano = i;
            goto found;
        }
    }
    for (i = 0; i < 4 && WinActNoTb[offset->idno][i]; i++) {
    }
    if (i == 0) {
        offset->actnoSv = 0;
        return 0;
    }
    if (pads[offset->padid].data & offset->ply->PunchKey) {
        ano = 0;
    } else if (pads[offset->padid].data & offset->ply->KickKey) {
        ano = 1;
    } else if (pads[offset->padid].data & offset->ply->SlashKey) {
        ano = 2;
    } else if (pads[offset->padid].data & offset->ply->HighSlashKey) {
        ano = 3;
    } else {
        ano = Random() % i;
    }
    if (WinActNoTb[offset->idno][ano] == 0) {
        ano = (Random() & 3) % i;
    }
found:
    i = (unsigned char)WinActNoTb[offset->idno][ano];
    offset->actnoSv = i;
    if (offset->idno == 0xC && i == 0x5D) {
        offset->ply->KeyHook = JMWinB_KeyHook;
        return 1;
    }
    return 0;
}

extern story_t story;
action_t *get_act(void);
int change_actLR(int i, int act);

int EnterActionSet_story(CHARACTER_WORK *offset, int side) {
    int i;
    action_t *act;

    i = (side != story.self_side);
    act = get_act();
    if (act[story.actind[i]].kind == 1) {
        if (story.actcnt[i] == 0) {
            SetActNoNext(&offset[side], change_actLR(i, act[story.actind[i]].No) | 0x200000);
        }
        story.actcnt[i]++;
        if (story.actcnt[i] >= act[story.actind[i]].frame) {
            story.actind[i] += 2;
            story.actcnt[i] = 0;
        }
    } else if (act[story.actind[i]].kind == 0 || act[story.actind[i]].kind == 4 || act[story.actind[i]].kind == 5) {
        story.actind[i] += 2;
        story.actcnt[i] = 0;
    }
    return 1;
}

void SetCPUStatus(CHARACTER_WORK *offset, unsigned int flag);
extern USER_SETTING userinfo;

int SetActNo(CHARACTER_WORK *offset, unsigned int no)
{
    int ret;
    unsigned short actno;

    actno = no;
    ret = 0;
    offset->actnonexttemp = no;
    if ((no & 0x100000) && offset->actno == actno) {
        return 0;
    }
    if (!(no & 0x400000)) {
        offset->actst &= ~0x20;
    }
    offset->actst &= ~0x40;
    offset->actst &= ~0x80;
    offset->actst &= ~0x4000;
    offset->actst &= ~0x40000;
    offset->actst &= ~0x100000;
    offset->actst &= ~0x200000;
    offset->actst &= ~0x800000;
    offset->actst &= ~0x1000000;
    if (!(no & 0x8000000)) {
        offset->GuardSt &= 0xE3FF;
        if (no & 0x800000) {
            offset->attackst = 0x2000;
        } else {
            offset->attackst = 0;
        }
    }
    if (!(no & 0x10000000)) {
        SetPriority(offset, 0);
    }
    offset->actno = actno;
    offset->ActTimer = 0;
    offset->InstSt |= 0x20;
    if (!(no & 0x10000)) {
        SetXSpeed(offset, 0);
        SetHitBackXSpeed(offset, 0);
    }
    if (!(no & 0x40000)) {
        offset->actst &= ~0x200;
    }
    if (no & 0x20000000) {
        offset->dirflag = !offset->dirflag;
        offset->xspeed = -offset->xspeed;
    }
    if (no & 0x80000) {
        SetCPUStatus(offset, 4);
        if (!(userinfo.gguse[offset->padid] & 1)) {
            if (offset->ply->maintain > 60) {
                offset->ply->maintain = 60;
            }
        }
        offset->ply->JumpFlag &= ~2;
        ret = 1;
        offset->ply->sousaitime = 0;
        if (!(no & 0x1000000)) {
            offset->SpriteFlag &= ~0x200;
        }
        if (!(offset->actst & 0x10)) {
            if (offset->ply->inertiatime) {
                if (offset->ply->inertiatime < 5) {
                    offset->ply->inertia /= 8;
                } else if (offset->ply->inertiatime < 10) {
                    offset->ply->inertia /= 2;
                }
            }
        }
    }
    if (no & 0x2000000) {
        offset->ply->maintain = 0;
    }
    if (no & 0x40000000) {
        SearchCharacterExecute(offset, 0x4D, 0, 0x16);
        SearchCharacterExecute(offset, 0x4D, 0, 0x13);
    }
    offset->actnonext = -1;
    offset->LandBreakActno = -1;
    if (offset->ply) {
        switch (offset->actno) {
        case 0:
            offset->CommandFlag = 1;
            break;
        case 1:
            offset->CommandFlag = 8;
            break;
        }
        switch (offset->actno) {
        case 7:
        case 8:
        case 9:
        case 10:
            offset->actst |= 0x10;
        case 0:
            offset->actst &= ~0x400;
        case 1:
        case 5:
        case 6:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            offset->SpriteFlag &= ~0x3000;
            offset->actst &= ~0x2000100;
            offset->CommandFlag |= 0x100;
            if (offset->ply) {
                offset->ply->TensionFlag &= 0xFFFC;
                offset->ply->DownTimer = -1;
                offset->ply->homingtime = 0;
                offset->ply->JumpFlag &= ~2;
            }
            break;
        }
        offset->ply->rendaflag = 0;
        offset->ply->ExCommandFunc = 0;
        offset->ply->AtkHitInterrupt = 0;
        offset->ply->AtkGuardInterrupt = 0;
        offset->ply->DamageInterrupt = 0;
        offset->ply->LandInterrupt = 0;
        offset->ply->GuardFunc = 0;
        if (!(offset->actst & 0x10)) {
            offset->ply->pactst &= ~1;
        }
        offset->ply->pactst &= 0xCFFD;
        offset->ply->JumpFlag &= ~0x10;
        offset->actst |= 8;
        offset->angle = 0;
        offset->SpriteFlag &= ~0x20000;
    }
    if (no & 0x200000) {
        ret = 1;
        offset->CommandFlag = 0xC05F;
    }
    if (no & 0x6000000) {
        offset->actst &= ~0x1000;
        offset->actst &= ~0x400;
        if (offset->ply->homingtime) {
            offset->ply->homingtime = 0;
        }
        if (!(offset->actst & 0x10)) {
            DirectionDirectChange(offset);
        }
    }
    return ret;
}

void ParentExe_Position(CHARACTER_WORK *offset)
{
    if (!offset->parentWork) return;
    if (!offset->parentFlag) return;
    if (offset->parentFlag & 0x80) {
        ParentPosLink(offset, offset->parentWork);
    }
}

void MinXSpeed(CHARACTER_WORK *offset, short int min)
{
    int xspeed;

    if (offset->HitStopTime) return;
    if (!offset->dirflag) xspeed = -offset->xspeed; else xspeed = offset->xspeed;
    if (xspeed < min) {
        if (!offset->dirflag) offset->xspeed = -min; else offset->xspeed = min;
    }
}

void MaxXSpeed(CHARACTER_WORK *offset, short int max)
{
    int xspeed;

    if (offset->HitStopTime) return;
    if (!offset->dirflag) xspeed = -offset->xspeed; else xspeed = offset->xspeed;
    if (max < xspeed) {
        if (!offset->dirflag) offset->xspeed = -max; else offset->xspeed = max;
    }
}

void SetShakeoffX(CHARACTER_WORK *offset)
{
    int x;

    if (offset->ply->ShakeTime & 1) {
        x = 100;
        if (!offset->dirflag) {
            offset->ply->offx = x;
        } else {
            offset->ply->offx = -100;
        }
    } else {
        x = 100;
        if (!offset->dirflag) {
            offset->ply->offx = -100;
        } else {
            offset->ply->offx = x;
        }
    }
}

void SetHitBackXSpeed(CHARACTER_WORK *offset, short int xspeed)
{
    if (offset->ply) {
        if (offset->ply->DamageTime >= 840) xspeed = xspeed * 2.0;
        else if (offset->ply->DamageTime >= 600) xspeed = xspeed * 1.7;
        else if (offset->ply->DamageTime >= 420) xspeed = xspeed * 1.4;
        else if (offset->ply->DamageTime >= 300) xspeed = xspeed * 1.2;
        else xspeed = xspeed * 1.0;
        if (offset->dirflag == 0) offset->HitBackx = -xspeed;
        else offset->HitBackx = xspeed;
    }
}

extern short int Dash_AccelByIdno[];
extern short int Dash_ResistByIdno[];

void SetDashXSpeed(CHARACTER_WORK *offset, short int xspeed)
{
    if (Dash_ResistByIdno[offset->padid] == 0) {
        debugf("DashRegist Break %d %d\n", offset->idno, offset->padid);
    }
    if (offset->dirflag == 0) {
        if (-xspeed < offset->xspeed) {
            offset->xspeed = -xspeed;
        } else {
            offset->xspeed -= Dash_AccelByIdno[offset->padid];
            offset->xspeed -= offset->xspeed / Dash_ResistByIdno[offset->padid];
        }
    } else {
        if (offset->xspeed < xspeed) {
            offset->xspeed = xspeed;
        } else {
            offset->xspeed += Dash_AccelByIdno[offset->padid];
            offset->xspeed -= offset->xspeed / Dash_ResistByIdno[offset->padid];
        }
    }
}

extern short int HomingJump_XMAX[];
extern short int HomingJump_XResist[];
extern short int HomingJump_YMAX[];
extern short int HomingTarget_YOffset[];
float nlCos(short unsigned int r);
float nlSin(short unsigned int r);
int nlArcTan2(int x, int y);
int g_round(int val, int max, int min);

#define HE_ABS(a) (((a) < 0) ? -(a) : (a))

void HomingExecute(CHARACTER_WORK *offset)
{
    u_short t;
    float x;
    float y;
    CHARACTER_WORK *aoff;

    aoff = &PlayerWork[offset->padid ^ 1];
    x = aoff->posx - offset->posx;
    if (offset->ply->HomingJumpDir == 0 && 0 < x) {
        offset->ply->homingtime = 0;
    }
    if (offset->ply->HomingJumpDir == 1 && x < 0) {
        offset->ply->homingtime = 0;
    }
    if (offset->ply->homingtime <= 0) {
        return;
    }
    if (offset->HitStopTime) {
        return;
    }
    offset->ply->homingtime--;
    y = aoff->posy + HomingTarget_YOffset[offset->padid] - offset->posy;
    t = nlArcTan2(x, y);
    y = nlCos(t) * HomingJump_YMAX[offset->padid];
    if (y > 50.0) {
        y = 50.0f;
    }
    offset->xspeed += nlSin(t) * HomingJump_XMAX[offset->padid];
    offset->yspeed += y;
    if (HE_ABS(x) < 20000.0f) {
        offset->xspeed -= offset->xspeed / (HomingJump_XResist[offset->padid] / 2);
    } else {
        offset->xspeed -= offset->xspeed / HomingJump_XResist[offset->padid];
    }
    offset->xspeed = g_round(offset->xspeed, 800, -800);
    offset->yspeed -= offset->yspeed / 100;
}

extern unsigned int *ChainTableByIdno[];

void SetChainFlag(CHARACTER_WORK *offset)
{
    unsigned int *tb;
    unsigned int *flag;

    tb = ChainTableByIdno[offset->padid];
    flag = &offset->ply->chainflag;
    switch (offset->actno) {
    case 24: *flag = tb[0]; break;
    case 62: *flag = tb[1]; break;
    case 25: *flag = tb[2]; break;
    case 26: *flag = tb[3]; break;
    case 27: *flag = tb[4]; break;
    case 28: *flag = tb[5]; break;
    case 65: *flag = tb[6]; break;
    case 29: *flag = tb[7]; break;
    case 30: *flag = tb[8]; break;
    case 64: *flag = tb[9]; break;
    case 31: *flag = tb[10]; break;
    case 32: *flag = tb[11]; break;
    case 33: *flag = tb[12]; break;
    case 34: *flag = tb[13]; break;
    case 35: *flag = tb[14]; break;
    case 36: *flag = tb[15]; break;
    default: *flag = 0; break;
    }
    if (offset->idno == 7) {
        if (offset->actno == 112) {
            *flag = tb[17];
        }
    }
}

void DeleteIttai(CHARACTER_WORK *p)
{
    int i;
    CHARACTER_WORK *offset;
    CHARACTER_WORK *poffset;

    for (i = 0; i < 48; i++) {
        offset = &ObjectWork[i];
        if (offset->actst & 1) {
            poffset = offset->parentWork;
            if (offset->parentFlag && p == poffset) {
                ObjectKill(offset);
            }
        }
    }
}

extern GAME_INFOMATION g_info;
extern story_t story;
extern int StageNo;
extern short unsigned int PreEntryActNoTb[][8];
void set_random_seed_abs(void);
int change_dirLR(int dir);
int change_posLR(int pos);
action_t *get_act(void);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);

int PreEnterActionSet(CHARACTER_WORK *offset)
{
    short unsigned int i;
    short unsigned int ano;
    CHARACTER_WORK *enemy;

    enemy = GetEnemyPlayer(offset);
    if ((g_info.mode & 0x2000) && get_act()) {
        int ind;
        action_t *act;

        ind = offset->padid != story.self_side;
        act = get_act();
        story.direction[ind] = offset->padid == 0;
        story.ini_pos[offset->padid].posx = 0;
        story.ini_pos[offset->padid].posy = 0;
        while (1) {
            if (act[story.actind[ind]].kind == 0) {
                story.direction[ind] = change_dirLR(act[story.actind[ind]].No);
                story.is_boss[offset->padid] = act[story.actind[ind]].frame;
                story.actind[ind] += 2;
            } else if (act[story.actind[ind]].kind == 4) {
                story.ini_pos[offset->padid].posx = change_posLR(act[story.actind[ind]].No);
                story.ini_pos[offset->padid].posy = act[story.actind[ind]].frame;
                story.actind[ind] += 2;
            } else if (act[story.actind[ind]].kind == 5) {
                story.actind[ind] += 2;
            } else {
                break;
            }
        }
        debugf("*** is_boss[0] = %d, is_boss[1] = %d ***\n", story.is_boss[0], story.is_boss[1]);
        story.is_enter = 1;
        return 0;
    }
    if (g_info.mode & 0x100) {
        offset->actnoSv = 0xffff;
        return 0;
    }
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    for (i = 4; i < 8; i++) {
        if ((PreEntryActNoTb[offset->idno][i] >> 10) == enemy->idno) {
            ano = i;
            goto found;
        }
    }
    for (i = 0; i < 4; i++) {
        if (PreEntryActNoTb[offset->idno][i] == 0) break;
    }
    if (i == 0) {
        offset->actnoSv = 0xffff;
        return 0;
    }
    ano = Random() % i;
found:
    if (offset->idno == 16 && StageNo != 10 && ano == 0) {
        ano = 1;
    }
    offset->actnoSv = ano;
    i = PreEntryActNoTb[offset->idno][ano] & 0x3ff;
    if (i != 0xffff) {
        SetActNoNext(offset, i | 0x200000);
    }
    return ano;
}

void SetParentPosX(CHARACTER_WORK *offset, short int pos)
{
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    offset->parentx = pos;
    if (poffset->dirflag == 1) offset->posx = poffset->posx - offset->parentx * 100;
    else offset->posx = poffset->posx + offset->parentx * 100;
}
