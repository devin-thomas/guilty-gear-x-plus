#include "ggxeuc/objtypes.h"
#include "generated/types/CREATE_ARG.h"

/* act.c's own functions and globals (names and types from generated/act.h).
 * The forms are the ones this TU matches with: the empty-list declarations stay
 * unprototyped for the calls made before each definition, and the [0] table
 * size keeps Act_Func_Size_Tb out of $gp addressing. */
extern ACT_FUNC_SIZE Act_Func_Size_Tb[0];
extern u_int *InstTbWork;
extern unsigned short StopTime1P;
extern unsigned short StopTime2P;
extern u_int *InstTbP;

void af_RENEWAL_COLLISION();          /* (CHARACTER_WORK *, TACTNORMAL *) */
int GetInstSize(unsigned char id);
void InstTbInit();
void *GetInstTb(void *in);
void SetAnimeData();                  /* (CHARACTER_WORK *, TACTNORMAL *) */
void SetParentPosEx();
void ParentFlagSet();                 /* (CHARACTER_WORK *, CHARACTER_WORK *, TINITINSTANCE *) */
void ExePosByEnemy();
void NextInstExecute();
void SetAnimeNo();                    /* (CHARACTER_WORK *, int) */
void HitStopTimeExeRoot();
void SetForceStopTime(short t1p, short t2p);
void ExeAnime();

void SetDamage(CHARACTER_WORK *offset, unsigned short damage);

void af_ASTFLAGCONTROL(CHARACTER_WORK *offset, TACTNORMAL *ip)
{
    unsigned int bits;

    bits = 0;
    if (ip->arg1.u == 0) {
        bits = 0x100000;
    } else if (ip->arg1.u == 1) {
        bits = 0x1000000;
    }
    if (ip->arg2.s) {
        offset->actst |= bits;
    } else {
        offset->actst &= ~bits;
    }
}


void af_BACK_MOTION(CHARACTER_WORK *offset, TACTNORMAL *ip)
{
    offset->attackst |= 0x800;
    offset->attackst &= ~0x4000;
    offset->actst |= 0x40;
    offset->GuardSt &= ~0x400;
    offset->ActHeader.flag &= ~0x2000;
    if (ip->arg1.u == 0) {
        offset->attackst &= ~0x10;
        offset->attackst |= 0x400;
        offset->attackst &= ~0x20;
    }
}


void af_AFTERIMAGE(CHARACTER_WORK *offset, TACTNORMAL *ip)
{
    if (ip->arg2.u == 0) {
        if (ip->arg1.u) {
            offset->ply->pactst |= 0x2;
        } else {
            offset->ply->pactst &= ~0x2;
        }
    } else if (ip->arg2.u == 1) {
        if (ip->arg1.u) {
            offset->ply->pactst |= 0x2000;
        } else {
            offset->ply->pactst &= ~0x2000;
        }
    }
}


void SetDefaultHitVoice(CHARACTER_WORK *offset);
void SetDefaultTension(CHARACTER_WORK *offset);
void ExCollisionClear(CHARACTER_WORK *offset);

void SetAnimeNo(CHARACTER_WORK *offset, int no)
{
    offset->AnimeNo = no;
    if (offset->InstTb != NULL) {
        int *lps;

        lps = (int *)offset->InstTb[no];
        offset->ActHeaderp = (TACTHEADER *)lps;
        *(int *)&offset->ActHeader.flag = lps[0];
        *(int *)&offset->ActHeader.lvflag = lps[1];
        SetDamage(offset, offset->ActHeader.damage);
        offset->SpriteFlag &= ~0x400002;
        offset->InstAddr = (int *)(offset->ActHeaderp + 1);
        if (offset->HitParam != NULL) {
            offset->HitParam->DownX = -1;
            offset->HitParam->DownY = -1;
            offset->HitParam->DownGrav = -1;
            offset->HitParam->DownUkemiTime = -1;
            offset->HitParam->DownFlag = 0;
            offset->HitParam->FaintPoint = 0x10;
            offset->HitParam->Kezuri = 0x10;
            offset->HitParam->DamageHosei = -1;
            offset->HitParam->HitSE.l = 0;
            offset->HitParam->GuardSE.l = 0;
            offset->HitParam->DamInt = NULL;
            SetDefaultHitVoice(offset);
        }
        offset->trans = 0xFF;
        offset->trans2 = 0xFF;
        offset->transmode = 0;
        offset->transmode2 = 0;
        if (!(offset->InstSt & 0x4000)) {
            offset->scale = -1;
            offset->scaleY = -1;
        }
        offset->Mark = 0;
        offset->ActHitCount = 0;
        offset->PaletteFlag = 0;
        offset->MaxHitCount = 0x20;
        offset->InstSt = 0x102;
        SetDefaultTension(offset);
        ExCollisionClear(offset);
    }
}



typedef struct {
    u_char id;
    u_char arg1;
    u_char arg2;
    u_char arg3;
} TACTBYTE4;

int Random(void);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *offset);

typedef struct {
    char pad[0x456];
    u_char flag[2];
} USERINFO;
extern USERINFO userinfo;

typedef struct {
    char pad[0x30];
    u_int flag;
    char pad34[0x3C - 0x34];
    u_char cellsel[2];
    char pad3e[0x6A - 0x3E];
    u_char kakusei[2];
    char pad6c[0x78 - 0x6C];
    u_int flag78;
} G_INFO;
extern G_INFO g_info;

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void AddXSpeed(CHARACTER_WORK *offset, short xspeed);
void ParXSpeed(CHARACTER_WORK *offset, float par);
void MinXSpeed(CHARACTER_WORK *offset, short min);
void MaxXSpeed(CHARACTER_WORK *offset, short max);
void ParInertia(CHARACTER_WORK *offset, float par);
void MovePosX(CHARACTER_WORK *offset, int val);
void MovePosY(CHARACTER_WORK *offset, int val);
float nlSin(u_short angle);
float nlCos(u_short angle);
void AddYSpeed(CHARACTER_WORK *offset, short yspeed);
void ParYSpeed(CHARACTER_WORK *offset, float par);

typedef struct {
    u_char id;
    _BYTE_ arg1;
    _WORD_ arg2;
    u_int arg3;
} TACTLONG;
void SetParentPosX(CHARACTER_WORK *offset, short pos);
void SetParentPosY(CHARACTER_WORK *offset, short pos);
short *Get_GCLExNo(CHARACTER_WORK *offset, int no);

void af_RENEWAL_COLLISION(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    RenewalCollision();
}

void SetPriorityMain(CHARACTER_WORK *offset, int PriAdd, int flag);

void af_SET_PRIORITY(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    int flag = (ip->arg1.u & 1) ? 2 : 3;

    if (ip->arg1.u & 2) {
        SetPriorityMain(offset, ip->arg2.s, flag);
        return;
    }
    switch (ip->arg2.u) {
    case 0xFA:
        SetPriorityMain(offset, 0x37, flag);
        break;
    case 0xF9:
        SetPriorityMain(offset, 0x41, flag);
        break;
    case 0xFC:
        SetPriorityMain(offset, -10, flag);
        break;
    case 0xFE:
        SetPriorityMain(offset, -15, flag);
        break;
    case 0xFB:
        SetPriorityMain(offset, -30, flag);
        break;
    case 0xFD:
        SetPriorityMain(offset, 10, flag);
        break;
    case 0:
        SetPriorityMain(offset, 0, flag);
        break;
    default:
        offset->SpriteFlag |= 2;
        SetPriorityMain(offset, ip->arg2.u, flag);
        break;
    }
}

void SetExCollision(CHARACTER_WORK *offset, int x, int y, int w, int h, int type);

void af_SETCOLLISIONDATA(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.u == 1) {
        SetExCollision(offset, -1000, -2, 2000, 4, 1);
    }
}

void af_SEMITRANS(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (!(ip->arg2.u & 0x8000)) {
        offset->trans = ip->arg1.u;
        offset->transmode = ip->arg2.u;
    }
    offset->trans2 = ip->arg1.u;
    offset->transmode2 = ip->arg2.u;
}

int Random(void);

void af_SCALE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.u == 1) {
        offset->scaleY = ip->arg2.u;
    } else if (ip->arg1.u == 0) {
        offset->scale = ip->arg2.u;
    } else if (ip->arg1.u == 3) {
        offset->scaleY += ip->arg2.u;
    } else if (ip->arg1.u == 2) {
        offset->scale += ip->arg2.u;
    } else if (ip->arg1.u == 5) {
        offset->scaleY += ip->arg2.u & Random();
    } else if (ip->arg1.u == 4) {
        offset->scale += ip->arg2.u & Random();
    } else if (ip->arg1.u == 7) {
        offset->scaleY = offset->scaleY * ((float)ip->arg2.u / 100.0);
    } else if (ip->arg1.u == 6) {
        offset->scale = offset->scale * ((float)ip->arg2.u / 100.0);
    }
}

int Random(void);

void af_ROT(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.u == 0) {
        offset->angle = ip->arg2.u;
    } else if (ip->arg1.u == 1) {
        offset->angle = Random() << 8;
    } else if (ip->arg1.u == 2) {
        offset->angle += ip->arg2.u;
    } else if (ip->arg1.u == 3) {
        offset->angle = (u_char)offset->Localid << 8;
    }
}

void RqLndEff();
#ifdef GGX_PORT
/* af_FIXEDEFFECT calls RqLndEff(CHARACTER_WORK *) with no arguments; offset is still in a0. */
#define RqLndEff() RqLndEff(offset)
#endif
void ReqCmnSE_JumpLand(CHARACTER_WORK *offset);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
CHARACTER_WORK *ObjectCreateA(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
void SetAXChainKill_id(unsigned short id);
void SetSearchExecProcAddr(void *proc);
void AXChainKill_Proc();
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *offset, int idno, int a, int b);
void SetActNoNext(CHARACTER_WORK *offset, unsigned short no);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void ReqBKVoice_EntryC(CHARACTER_WORK *offset);
void ReqKYVoice_EntryC(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *offset, int idno);

void af_FIXEDEFFECT(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    CREATE_ARG arg;
    CHARACTER_WORK *work;
    CHARACTER_WORK *eff;
    CHARACTER_WORK *w4;
    CHARACTER_WORK *w8;
    short *ex;
    int i;

    if (ip->arg1.u == 0) {
        RqLndEff();
        ReqCmnSE_JumpLand(offset);
    } else if (ip->arg1.u == 1) {
        eff = offset;
        arg.flag = 0x120;
        for (i = 0; i < 30; i++) {
            arg.Localid = i;
            eff = EffectCreate(eff, 0x4D, &arg);
            arg.flag = 0x20;
        }
        arg.flag = 0x24;
        arg.Localid = i;
        arg.actno = ip->arg2.u;
        ObjectCreateA(eff, 0x4D, &arg);
    } else if (ip->arg1.u == 2) {
        offset->ply->PersonalWork[0].w[1] = ip->arg2.u;
    } else if (ip->arg1.u == 3) {
        SetAXChainKill_id(ip->arg2.u);
        SetSearchExecProcAddr(AXChainKill_Proc);
        SearchCharacterExecute(offset, 0x4D, 0, 0x26);
    } else if (ip->arg1.u == 4) {
        w4 = SearchCharacterExecute(offset, 0x4D, 0, 3);
        if (w4 != NULL) {
            SetActNoNext(w4, ip->arg2.u);
        }
    } else if (ip->arg1.u == 5) {
        work = GetEnemyPlayer(offset);
        if (!(work->actst & 0x10)) {
            ex = Get_GCLExNo(offset, ip->arg2.u);
            if (ex != NULL) {
                if (offset->dirflag == 1) {
                    work->posx = offset->posx - *ex * 100;
                } else {
                    work->posx = offset->posx + *ex * 100;
                }
            }
        }
    } else if (ip->arg1.u == 6) {
        ReqBKVoice_EntryC(offset);
    } else if (ip->arg1.u == 7) {
        ReqKYVoice_EntryC(offset);
    } else if (ip->arg1.u == 8) {
        w8 = SearchObjectWork(offset, 0x23);
        if (w8 != NULL) {
            w8->Localid |= 0x100;
        }
    } else if (ip->arg1.u == 9) {
        offset->ply->PersonalWork[0].b[0] = 4;
    }
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *offset);
void ZTED_Time_Dec(CHARACTER_WORK *offset, u_char time);

void af_COMMUNICATE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    CHARACTER_WORK *target = NULL;

    if (ip->arg1.u == 0) {
        target = offset;
    } else if (ip->arg1.u == 1) {
        target = GetParentPlayer(offset);
    } else if (ip->arg1.u == 2) {
        target = GetEnemyPlayer(offset);
    }
    if (((u_char *)ip)[2] == 0) {
        ZTED_Time_Dec(target, ((u_char *)ip)[3]);
    }
}

void DrawParent();

void af_VISUAL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.u == 0) {
        if (ip->arg2.u != 0) {
            offset->actst |= 8;
        } else {
            offset->actst &= ~8;
        }
    } else if (ip->arg1.u == 1) {
        offset->SpriteFlag |= 0x10;
    } else if (ip->arg1.u == 2) {
        if (ip->arg2.u != 0) {
            offset->graphicfunc = DrawParent;
        } else {
            offset->graphicfunc = NULL;
        }
    } else if (ip->arg1.u == 3) {
        if (ip->arg2.u != 0) {
            offset->SpriteFlag |= 0x200;
        } else {
            offset->SpriteFlag &= ~0x200;
        }
    } else if (ip->arg1.u == 4) {
        if (ip->arg2.u != 0) {
            offset->SpriteFlag |= 0x200000;
        } else {
            offset->SpriteFlag &= ~0x200000;
        }
    } else if (ip->arg1.u == 5) {
        if (ip->arg2.u != 0) {
            offset->SpriteFlag |= 0x400000;
        } else {
            offset->SpriteFlag &= ~0x400000;
        }
    }
}

void sound_request(int no);
void voice_off(CHARACTER_WORK *offset);
void voice_request(CHARACTER_WORK *offset, int no);
void privatese_request(CHARACTER_WORK *offset, int no);

void af_RQ_SOUND(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    TACTBYTE4 *bp = (TACTBYTE4 *)ip;

    if ((bp->arg2 & 0x30) == 0x10 && bp->arg3 != 0) {
        if ((u_int)(Random() & 0xFF) > bp->arg3) {
            return;
        }
    }
    if (bp->arg2 & 1) {
        sound_request(bp->arg1);
    } else if (bp->arg2 & 4) {
        if (GetParentPlayer(offset)->HitPoint > 0) {
            voice_off(offset);
            voice_request(offset, bp->arg1);
        }
    } else if (bp->arg2 & 2) {
        privatese_request(offset, bp->arg1);
    }
}

CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *offset, int idno, int a, int b);

void af_DELETECHARACTER(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    TACTBYTE4 *bp = (TACTBYTE4 *)ip;

    if (bp->arg2 == 0) {
        SearchCharacterExecute(offset, bp->arg1, 0, 0x12);
    } else if (bp->arg2 == 1) {
        SearchCharacterExecute(offset, 0, 0, 0x10);
    } else if (bp->arg2 == 2) {
        SearchCharacterExecute(offset, bp->arg1, bp->arg3, 0x16);
    } else if (bp->arg2 == 3) {
        SearchCharacterExecute(offset, bp->arg1, 0, 0x13);
    } else if (bp->arg2 == 4) {
        SearchCharacterExecute(offset, 0, 0, 0x11);
    } else if (bp->arg2 == 5) {
        SearchCharacterExecute(offset, bp->arg1, bp->arg3, 0x17);
    }
}

void af_GUARD_VOICE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    TACTBYTE4 *bp = (TACTBYTE4 *)ip;

    if (offset->HitParam != NULL) {
        offset->HitParam->gno = bp->arg1;
        offset->HitParam->gprob = bp->arg3;
    }
}

void af_DAMAGE_VOICE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    TACTBYTE4 *bp = (TACTBYTE4 *)ip;

    if (offset->HitParam != NULL) {
        offset->HitParam->dno = bp->arg1;
        offset->HitParam->dprob = bp->arg3;
    }
}

typedef struct {
    int flag;
    int pad[3];
} DAMAGE_ARG;

int PlayerDamage(CHARACTER_WORK *offset, unsigned short damage, DAMAGE_ARG *arg);

void af_ENEMYDAMAGE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    DAMAGE_ARG arg;
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    int dmg;

    if (ip->arg1.u == 0) {
        arg.flag = 0;
        PlayerDamage(enemy, ip->arg2.u, &arg);
        enemy->actst |= 0x2000000;
    } else if (ip->arg1.u == 1) {
        enemy->ply->DownBoundDamage = ip->arg2.u;
    } else if (ip->arg1.u == 2) {
        arg.flag = 0x4000000;
        PlayerDamage(enemy, ip->arg2.u, &arg);
        enemy->actst |= 0x2000000;
    } else if (ip->arg1.u == 3) {
        arg.flag = 4;
        PlayerDamage(enemy, ip->arg2.u, &arg);
        enemy->actst |= 0x2000000;
    } else if (ip->arg1.u == 4) {
        arg.flag = 0;
        dmg = ip->arg2.u;
        if (dmg >= offset->HitPoint) {
            dmg = offset->HitPoint - 1;
        }
        offset->HitPoint -= dmg;
        enemy->actst |= 0x2000000;
    }
}

void af_CHAIN_CANCEL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.u == 1) {
        offset->attackst &= ~0x10;
    } else if (ip->arg1.u == 2) {
        offset->attackst = (offset->attackst | 0x400) & ~0x20;
    } else if (ip->arg1.u == 3) {
        if (!(userinfo.flag[offset->padid] & 1)) {
            offset->attackst = (offset->attackst | 0x400) & ~0x20;
        }
    } else {
        offset->attackst |= 0x10;
    }
}

void SetFadeSpeed(unsigned short speed);
void SetFadeEndCommand(int cmd);
void Rq_Fadeout();
void Rq_Fadein();
void SetFadeCockpitFlag(int flag);

void af_FADE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    CHARACTER_WORK *enemy;
    CHARACTER_WORK *enemy2;

    if (ip->arg1.u == 3) {
        if (ip->arg2.u != 0) {
            enemy = GetEnemyPlayer(offset);
            offset->SpriteFlag |= 0x40000;
            enemy->SpriteFlag |= 0x40000;
        } else {
            enemy2 = GetEnemyPlayer(offset);
            offset->SpriteFlag &= ~0x40000;
            enemy2->SpriteFlag &= ~0x40000;
        }
    } else if (ip->arg1.u == 2) {
        if (ip->arg2.u != 0) {
            g_info.flag |= 8;
        } else {
            g_info.flag &= ~8;
        }
    } else if (ip->arg1.u == 1) {
        SetFadeSpeed(ip->arg2.u);
        SetFadeEndCommand(0);
        Rq_Fadeout();
        SetFadeCockpitFlag(1);
    } else {
        SetFadeSpeed(ip->arg2.u);
        SetFadeEndCommand(0);
        Rq_Fadein();
        SetFadeCockpitFlag(1);
    }
}

void af_VEBALL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    TACTBYTE4 *bp = (TACTBYTE4 *)ip;

    offset->attackst |= 0x4000;
    if (offset->ply != NULL) {
        offset->ply->PersonalWork[0].ub[2] = bp->arg1;
        offset->ply->PersonalWork[0].ub[0] = bp->arg2;
        offset->ply->PersonalWork[0].ub[3] = bp->arg3;
    }
}

typedef struct {
    int flag;
    char pad[0x45C];
    struct { int x; int pad[3]; } sub[2];
} ACT_UNK460;
extern ACT_UNK460 *act_unk_gp;

void Tension_Dec(CHARACTER_WORK *offset, int val);
int g_roundmin(int a, int b);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
void sound_request(int no);

void af_KAKUSEI(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    TACTBYTE4 *bp = (TACTBYTE4 *)ip;
    PLAYER_ENTRY *ply;
    CREATE_ARG arg;

    if ((char)bp->arg3 == 2) {
        ply = offset->ply;
        if (ply->OverKillFlag == 1) {
            ply->OverKillFlag = 0;
            offset->ply->Tension = 0;
            return;
        }
        if ((g_info.flag78 & 0x8000) && ply->CPU == 0) {
            if (act_unk_gp->sub[act_unk_gp->flag ? 0 : 1].x == 0) {
                ply->OverKillFlag = 0;
                return;
            }
        }
        ply->OverKillFlag = 1;
        offset->ply->Tension = g_roundmin(offset->ply->Tension, 1000);
    } else if ((char)bp->arg3 == 3) {
        g_info.kakusei[offset->padid] = bp->arg1;
    } else {
        Tension_Dec(offset, -6);
        arg.flag = 0x22;
        arg.x = 0;
        if (offset->idno == 0x12) {
            arg.y = -16000;
        } else {
            arg.y = -8000;
        }
        arg.Localid = bp->arg1 | (bp->arg2 << 8);
        if (arg.Localid == 0) {
            arg.Localid = 0x201D;
        }
        EffectCreate(offset, 0x77, &arg);
        GetEnemyPlayer(offset)->attackst |= 0x200000;
        if ((char)bp->arg3 != 0) {
            sound_request(0x34);
        } else {
            sound_request(0x32);
        }
    }
}

void af_SETXSPEED(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.s == 0) {
        SetXSpeed(offset, ip->arg2.s);
    } else if (ip->arg1.s == 1) {
        AddXSpeed(offset, ip->arg2.s);
    } else if (ip->arg1.s == 2) {
        MovePosX(offset, ip->arg2.s);
    } else if (ip->arg1.s == 3) {
        ParXSpeed(offset, (float)ip->arg2.s / 100.0);
    } else if (ip->arg1.s == 4) {
        MinXSpeed(offset, ip->arg2.s);
    } else if (ip->arg1.s == 5) {
        MaxXSpeed(offset, ip->arg2.s);
    } else if (ip->arg1.s == 6) {
        offset->posx = ip->arg2.s * 100;
    } else if (ip->arg1.s == 7) {
        SetXSpeed(offset, ip->arg2.s * nlSin(offset->angle));
        offset->yspeed = ip->arg2.s * nlCos(offset->angle);
    } else if (ip->arg1.s == 8) {
        AddXSpeed(offset, ip->arg2.u & Random());
    } else if (ip->arg1.s == 9) {
        ParInertia(offset, (float)ip->arg2.s / 100.0);
    } else if (ip->arg1.s == 10) {
        if (userinfo.flag[offset->padid] & 1) {
            SetXSpeed(offset, ip->arg2.s * 12 / 10);
        } else {
            SetXSpeed(offset, ip->arg2.s);
        }
    }
}

void af_SETYSPEED(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.s == 0) {
        offset->yspeed = ip->arg2.s;
    } else if (ip->arg1.s == 1) {
        AddYSpeed(offset, ip->arg2.s);
    } else if (ip->arg1.s == 2) {
        MovePosY(offset, ip->arg2.s);
    } else if (ip->arg1.s == 3) {
        ParYSpeed(offset, (float)ip->arg2.s / 100.0);
    } else if (ip->arg1.s == 6) {
        offset->posy = ip->arg2.s * 100;
    } else if (ip->arg1.s == 8) {
        offset->yspeed += Random() & ip->arg2.s;
    }
}

void af_SETDOWNFLAG(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    DAMAGEPARAM *hp = offset->HitParam;

    if (hp != NULL) {
        if (ip->arg1.u == 0) {
            hp->DownFlag |= 1;
        } else if (ip->arg1.u == 1) {
            hp->DownFlag |= 0x10;
        } else if (ip->arg1.u == 2) {
            hp->DownFlag |= 0x400;
        } else if (ip->arg1.u == 3) {
            hp->DownFlag |= 0x800;
        } else if (ip->arg1.u == 4) {
            hp->DownFlag |= 0x1000;
        } else if (ip->arg1.u == 5) {
            hp->DownFlag |= 0x4000;
        }
    }
}

void SetSit(CHARACTER_WORK *offset);
void SetStand(CHARACTER_WORK *offset);

void af_SIT(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg2.u & 0xFF) {
        SetSit(offset);
    } else {
        SetStand(offset);
    }
}

void af_SET_DAMAGE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    SetDamage(offset, ip->arg2.u);
}

void af_SET_LEVEL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->ActHeader.lvflag = ip->arg2.u;
}

void af_SET_ACTTYPE(CHARACTER_WORK *offset, TACTLONG *ip) {
    if (ip->arg1.u == 1) {
        offset->ActHeader.flag &= ~0x1C00;
        offset->ActHeader.flag |= ip->arg3 & 0x1C00;
    } else {
        offset->ActHeader.flag = ip->arg3;
    }
}

void af_SETMARK(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    if (ip->arg1 != 0) {
        offset->Mark = ip->arg2;
    } else {
        offset->Mark |= ip->arg2;
    }
}

void af_SETMASK(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    offset->Mark &= ~ip->arg2;
}

void af_RENDACHANSEL(CHARACTER_WORK *offset, TACTLONG *ip) {
    offset->ply->rendaflag |= ip->arg3;
}

void af_JUMPCANCEL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.s == 1) {
        offset->attackst |= 0x40000;
    } else if (ip->arg1.s == 2) {
        if (userinfo.flag[offset->padid] & 1) {
            offset->attackst |= 0x40000;
        }
    } else {
        offset->attackst &= ~0x40000;
    }
}

void af_SETGRAVITY(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->gravity = ip->arg2.s;
}

void af_REVERSE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->dirflag = !offset->dirflag;
}

void af_DRAW_NORMAL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->dirflag = 0;
}

void af_DRAW_REVERSE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->dirflag = 1;
}

void SetParentPosX(CHARACTER_WORK *offset, short pos);

void af_PARENT_OFFSET_POS_X(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    SetParentPosX(offset, ip->arg2.s);
}

void SetParentPosY(CHARACTER_WORK *offset, short pos);

void af_PARENT_OFFSET_POS_Y(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    SetParentPosY(offset, ip->arg2.s);
}

void af_DOWN_GRAV(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->DownGrav = ip->arg2.u;
    }
}

void af_DOWN_X(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->DownX = ip->arg2.u;
    }
}

void af_DOWN_Y(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->DownY = ip->arg2.u;
    }
}

void af_SET_DOWNTIME(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->DownUkemiTime = ip->arg2.u;
    }
}

void DeleteIttai(CHARACTER_WORK *p);

void af_DELETE_ITTAI(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    DeleteIttai(offset);
}

void af_DO_NOT_CHECK_ATTACK(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->actst |= 0x40;
}

void af_DO_NOT_CHECK_DAMAGE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->actst |= 0x80;
}

void af_REVIVEDAMAGE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->actst &= ~0x80;
}

void af_POSBYENEMY(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->ply != NULL) {
        offset->ply->PosByEnemyAddr = (void *)ip;
    }
}

void af_TENSION(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    offset->TensionAdd = ip->arg2;
    if (offset->ply != NULL) {
        offset->ply->TensionHissatuAddVal = ip->arg1;
        if ((offset->ActHeaderp->flag & 4) && ip->arg1 != 0) {
            offset->ply->TensionFlag |= 2;
        } else {
            offset->ply->TensionFlag &= ~2;
        }
    }
}

void af_SETMAXHIT(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->MaxHitCount = ip->arg1.u;
}

void ObjIndependence(CHARACTER_WORK *offset);

void af_INDEPENDENCE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    ObjIndependence(offset);
}

void af_PALETTE(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    if (ip->arg1 == 0) {
        offset->PaletteFlag |= 1;
        offset->LocalPalette = ip->arg2;
    } else if (ip->arg1 == 1) {
        offset->PaletteFlag |= 2;
    }
}

void af_FREEIMAGE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->SpriteFlag |= 0x100;
}

void af_ENEMY_HIT_SE_MODE(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    int i;

    if (offset->HitParam != NULL) {
        i = offset->HitParam->HitSE.param.flag[0] != 0;
        offset->HitParam->HitSE.param.no[i] = ip->arg1;
        offset->HitParam->HitSE.param.flag[i] = ip->arg2;
    }
}

void af_ENEMY_GUARD_SE_MODE(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    int i;

    if (offset->HitParam != NULL) {
        i = offset->HitParam->GuardSE.param.flag[0] != 0;
        offset->HitParam->GuardSE.param.no[i] = ip->arg1;
        offset->HitParam->GuardSE.param.flag[i] = ip->arg2;
    }
}

void SetFlash(int type, u_short time);

void af_FLASH(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    SetFlash(ip->arg1.s, ip->arg2.u);
}

void SetGroundShake(u_short time);

void af_GROUNDSHAKE(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    SetGroundShake(ip->arg2.u);
}

void af_KEZURI(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->Kezuri = ip->arg2.u;
    }
}

void af_DAMAGEHOSEI(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->DamageHosei = ip->arg2.u;
    }
}

void af_MUTEKI(CHARACTER_WORK *offset, TACTBYTE4 *ip) {
    offset->ply->MutekiTime = ip->arg2;
    offset->actst |= 0x20000;
}

void af_NOTHROW(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg2.u != 0) {
        offset->actst |= 0x800000;
    } else {
        offset->actst &= ~0x800000;
    }
}

void RandMove(CHARACTER_WORK *offset, u_int val);

void af_RANDPOS(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    RandMove(offset, ip->arg2.u);
}

void af_FAINT(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (offset->HitParam != NULL) {
        offset->HitParam->FaintPoint = ip->arg2.u;
    }
}

void SetLandBreakActno(CHARACTER_WORK *offset, int actno);
void SetJumpControl(CHARACTER_WORK *offset);

void af_JUMPCONTROL(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg2.u != 0) {
        SetLandBreakActno(offset, 0x15);
    } else {
        SetJumpControl(offset);
    }
}

void af_SETLOCALID(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.s == 0) {
        offset->Localid = ip->arg2.u;
    } else if (ip->arg1.s < 0) {
        offset->Localid |= ip->arg2.u;
    } else {
        offset->Localid &= ~ip->arg2.u;
    }
}

void CmnGuardAct();

void af_GUARD(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    if (ip->arg1.u != 0) {
        offset->ply->GuardFunc = CmnGuardAct;
        offset->GuardSt = 0x201;
        offset->attackst = (offset->attackst | 0x100000) & ~0x200;
    } else {
        offset->GuardSt = 0;
        offset->attackst &= ~0x100000;
        offset->ply->ExCommandFunc = NULL;
        offset->ply->GuardFunc = NULL;
    }
}

int GetInstSize(unsigned char id) {
    static int last = 0;
    int size;

    size = Act_Func_Size_Tb[id].size;
    if (id == 0xFF) {
        return 4;
    }
    if (size != 0) {
        last = id;
        return size;
    }
    return 0;
}

void *dmalloc(int size, char *name);

void InstTbInit() {
    InstTbP = InstTbWork = dmalloc(0x10000, "InstTbWork");
}

extern int actnoSV;
extern void *InstInitAddr;
void *SkipInstExtInfo(void *in);

void *GetInstTb(void *in) {
    void *tb[512];
    TACTNORMAL *ip;
    TACTNORMAL *prev;
    u_int *ret;
    int n = 0;
    int i;

    ip = SkipInstExtInfo(in);
    while (*(u_short *)ip != 0xFD) {
        actnoSV = n;
        tb[n] = ip;
        n++;
        InstInitAddr = ip;
        ip = (TACTNORMAL *)((TACTHEADER *)ip + 1);
        do {
            prev = ip;
            ip += GetInstSize(ip->id) / sizeof(TACTNORMAL);
        } while (prev->id != 0xFF);
    }
    ret = InstTbP;
    InstTbP += n;
    for (i = 0; i < n; i++) {
        ret[i] = (u_int)tb[i];
    }
    return ret;
}

void SetImageNo(CHARACTER_WORK *offset);

void SetAnimeData(CHARACTER_WORK *offset, TACTNORMAL *ip) {
    offset->AnimeTime = 0;
    offset->AnimeRate = ip->arg1.u;
    offset->CellNo = ip->arg2.u;
    SetImageNo(offset);
}

int SetParentPosExNo(CHARACTER_WORK *offset, int no) {
    short *ex = Get_GCLExNo(offset->parentWork, no);

    if (ex != NULL) {
        SetParentPosX(offset, ex[0]);
        SetParentPosY(offset, ex[1]);
        return 1;
    }
    return 0;
}

short *Get_GCLEx(CHARACTER_WORK *offset);

void SetParentPosEx(CHARACTER_WORK *offset) {
    short *ex = Get_GCLEx(offset->parentWork);

    if (ex != NULL) {
        SetParentPosX(offset, ex[0]);
        SetParentPosY(offset, ex[1]);
    }
}

short *Get_GCLEx(CHARACTER_WORK *offset);
short *Get_GCLExRand(CHARACTER_WORK *offset, int no);
void *Get_GCLColli(CHARACTER_WORK *offset, int no);
void GetColliCenter(CHARACTER_WORK *offset, void *colli, int *pos);

void ParentFlagSet(CHARACTER_WORK *parent, CHARACTER_WORK *child, TINITINSTANCE *init) {
    static int pexno;
    short *ex;
    void *colli;

    if (init->Kind & 2) {
        child->parentFlag |= 0x80;
    }
    if (init->Kind & 0x200) {
        child->parentFlag |= 0x200;
    }
    if (init->Kind & 0x4000) {
        child->angle = parent->angle;
    }
    child->dirflag = parent->dirflag;
    if (init->Kind & 0x20) {
        ex = Get_GCLEx(parent);
        if (ex != NULL) {
            SetParentPosX(child, ex[0]);
            SetParentPosY(child, ex[1]);
        }
        pexno = 1;
    } else if (init->Kind & 0x3000) {
        ex = Get_GCLExNo(parent, (init->Kind >> 12) & 3);
        if (ex != NULL) {
            SetParentPosX(child, ex[0]);
            SetParentPosY(child, ex[1]);
        }
    } else if (init->Kind & 0x100) {
        ex = Get_GCLExNo(parent, pexno);
        if (ex != NULL) {
            SetParentPosX(child, ex[0]);
            SetParentPosY(child, ex[1]);
        }
        pexno++;
    } else if (init->Kind & 0x40) {
        ex = Get_GCLExRand(parent, pexno);
        if (ex != NULL) {
            SetParentPosX(child, ex[0]);
            SetParentPosY(child, ex[1]);
        }
    } else if (init->Kind & 0x400) {
        colli = Get_GCLColli(parent, 1);
        if (colli != NULL) {
            GetColliCenter(parent, colli, &child->posx);
        }
    }
    child->actnoSv = parent->actno;
    if (init->Flag & 1) {
        child->parentFlag |= 1;
    }
    if (init->Flag & 2) {
        child->parentFlag |= 2;
    }
    if (init->Flag & 4) {
        child->parentFlag |= 4;
    }
    if (init->Kind & 8) {
        child->dirflag = !parent->dirflag;
    } else {
        child->dirflag = parent->dirflag;
    }
}

typedef struct {
    u_char id;
    u_char pad;
    short x;
    short y;
    u_short flag;
} TACTPOSBYENEMY;

void ExePosByEnemy(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    PLAYER_ENTRY *ply;
    TACTPOSBYENEMY *pb;

    if (offset->ply->PosByEnemyAddr != NULL) {
        enemy = GetEnemyPlayer(offset);
        ply = offset->ply;
        pb = ply->PosByEnemyAddr;
        if (enemy->dirflag == 0) {
            offset->posx = enemy->posx - pb->x;
        } else {
            offset->posx = enemy->posx + pb->x;
        }
        if (!(pb->flag & 1)) {
            offset->dirflag = enemy->dirflag;
        } else {
            offset->dirflag = enemy->dirflag ^ 1;
        }
        offset->posy = enemy->posy + pb->y;
        if (!(pb->flag & 2)) {
            ply->PosByEnemyAddr = NULL;
        }
    }
}

CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *offset, unsigned short idno, unsigned short actno);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *offset, unsigned short idno, unsigned short actno);
CHARACTER_WORK *ObjectCreate(CHARACTER_WORK *poffset, unsigned short idno);
void SetActFANG(CHARACTER_WORK *offset, CHARACTER_WORK *child);

void NextInstExecute(CHARACTER_WORK *offset) {
    TACTNORMAL *ip = (TACTNORMAL *)offset->InstAddr;
    void (*func)(CHARACTER_WORK *, TACTNORMAL *);
    TINITINSTANCE *ii;
    CHARACTER_WORK *work;

    if (ip->id == 0xFF) {
        return;
    }
    SetAnimeData(offset, ip);
    ip++;
    while (ip->id != 0 && ip->id != 0xFF) {
        func = Act_Func_Size_Tb[ip->id].func;
        if (func != NULL) {
            func(offset, ip);
        } else {
            switch (ip->id) {
            case 0x27:
                if (((TACTBYTE4 *)ip)->arg1 == 0) {
                    offset->InstAddr = (int *)GetCellBeginAddr(offset, ((TACTBYTE4 *)ip)->arg3);
                    ip = (TACTNORMAL *)offset->InstAddr;
                    SetAnimeData(offset, ip);
                } else if (((TACTBYTE4 *)ip)->arg1 == 1 && g_info.cellsel[offset->padid] == ((TACTBYTE4 *)ip)->arg2) {
                    offset->InstAddr = (int *)GetCellBeginAddr(offset, ((TACTBYTE4 *)ip)->arg3);
                    ip = (TACTNORMAL *)offset->InstAddr;
                    SetAnimeData(offset, ip);
                }
                break;
            case 0x19:
                ii = (TINITINSTANCE *)ip;
                if (ii->Kind & 0x8000) {
                    if (!(offset->attackst & 0x80000)) {
                        break;
                    }
                }
                if (ii->Kind & 0x10) {
                    if (offset->actst & 0x40) {
                        break;
                    }
                }
                if (ii->Kind & 0x800) {
                    if (ii->Effect != 0) {
                        work = SearchEffectWorkActno(offset, ii->idno, ii->AnimeNo);
                    } else {
                        work = SearchObjectWorkActno(offset, ii->idno, ii->AnimeNo);
                    }
                    if (work != NULL) {
                        break;
                    }
                }
                if (ii->Effect != 0) {
                    work = EffectCreate(offset, ii->idno, NULL);
                } else {
                    work = ObjectCreate(offset, ii->idno);
                }
                if (work != NULL) {
                    if (ii->Kind & 0x80) {
                        offset->idno = 10;
                        SetActFANG(offset, work);
                    } else {
                        SetActNoNext(work, ii->AnimeNo);
                    }
                    work->parentWork = offset;
                    ParentFlagSet(offset, work, ii);
                    if (ii->Localid != 0) {
                        work->Localid = ii->Localid;
                    }
                }
                break;
            }
        }
        ip += GetInstSize(ip->id) / sizeof(TACTNORMAL);
    }
    if (ip->id == 0xFF) {
        offset->InstSt |= 1;
        if (ip->arg1.u == 3) {
            offset->InstSt |= 0x10;
        } else if (ip->arg1.u == 4) {
            offset->InstSt |= 0x200;
            offset->InstVal = ip->arg2.u;
        } else if (ip->arg1.u == 5) {
            offset->InstSt |= 0x2000;
        } else if (ip->arg1.u == 6) {
            offset->InstSt |= 0x400;
        } else if (ip->arg1.u == 8) {
            offset->InstSt |= 0x800;
        } else if (ip->arg1.u == 7) {
            offset->InstSt |= 0x1000;
        }
    } else {
        offset->InstAddr = (int *)ip;
    }
}

void SetAnimeNoLoop(CHARACTER_WORK *offset, int no) {
    TACTHEADER *hdr;

    offset->AnimeNo = no;
    if (offset->InstTb != NULL) {
        hdr = (TACTHEADER *)offset->InstTb[no];
        offset->ActHeaderp = hdr;
        offset->ActHeader.flag = hdr->flag;
        *(int *)&offset->ActHeader.lvflag = *(int *)&hdr->lvflag;
        SetDamage(offset, offset->ActHeader.damage);
        offset->InstAddr = (int *)(offset->ActHeaderp + 1);
        offset->InstSt = 2;
    }
}

extern int BattleStatus;

void HitStopTimeExe(CHARACTER_WORK *offset) {
    if (BattleStatus == 3) {
        offset->HitStopTime -= 2;
    } else {
        offset->HitStopTime -= 2;
    }
}

void ForceStopTimeExe(CHARACTER_WORK *offset) {
    if (offset->idno < 0x61 && !(offset->SpriteFlag & 0x800)) {
        if (StopTime1P != 0 && offset->padid == 0) {
            offset->actst |= 0x80000;
            offset->HitStopTime = StopTime1P;
            offset->ColliFlag = -1;
        } else if (StopTime2P != 0 && offset->padid != 0) {
            offset->actst |= 0x80000;
            offset->HitStopTime = StopTime2P;
            offset->ColliFlag = -1;
        }
    }
}

extern CHARACTER_WORK *PlayerWork;
extern CHARACTER_WORK *ObjectWork;
extern CHARACTER_WORK *EffectWork;
void ForceStopTimeExe(CHARACTER_WORK *offset);
void HitStopTimeExe(CHARACTER_WORK *offset);

void HitStopTimeExeRoot() {
    CHARACTER_WORK *p;
    int i;

    p = PlayerWork;
    for (i = 2; i-- != 0; p++) {
        if (p->actst & 1) {
            ForceStopTimeExe(p);
            if (p->HitStopTime != 0) {
                HitStopTimeExe(p);
            }
        }
    }
    p = ObjectWork;
    for (i = 48; i-- != 0; p++) {
        if (p->actst & 1) {
            ForceStopTimeExe(p);
            if (p->HitStopTime != 0) {
                HitStopTimeExe(p);
            }
        }
    }
    p = EffectWork;
    for (i = 160; i-- != 0; p++) {
        if (p->actst & 1) {
            ForceStopTimeExe(p);
            if (p->HitStopTime != 0) {
                HitStopTimeExe(p);
            }
        }
    }
    SetForceStopTime(-1, -1);
}

void SetForceStopTime(short t1p, short t2p) {
    if (t1p < 0) {
        StopTime2P = 0;
        StopTime1P = 0;
        return;
    }
    if (StopTime1P < t1p) {
        StopTime1P = t1p;
    }
    if (StopTime2P < t2p) {
        StopTime2P = t2p;
    }
}

void ExeAnime(CHARACTER_WORK *offset) {
    if (offset->HitStopTime != 0 && offset->ColliFlag < 0 && !(offset->InstSt & 0x100)) {
        return;
    }
    if (offset->actst & 0x400000) {
        offset->actst &= ~0x400000;
        if (!(offset->InstSt & 0x100)) {
            return;
        }
    }
    if (offset->InstSt & 2) {
        if (!(offset->InstSt & 8)) {
            if (offset->ply != NULL) {
                offset->ply->PosByEnemyAddr = NULL;
            }
            NextInstExecute(offset);
        }
        offset->InstSt = (offset->InstSt & 0xFEFD) | 4;
    } else {
        offset->InstSt &= ~4;
    }
    if (offset->HitStopTime != 0) {
        return;
    }
    if (++offset->AnimeTime >= offset->AnimeRate) {
        offset->InstSt |= 2;
        offset->InstSt &= ~0x40;
        if (offset->InstSt & 1) {
            if (offset->InstSt & 0x10) {
                SetAnimeNoLoop(offset, offset->AnimeNo);
            } else if (offset->InstSt & 0x200) {
                SetActNoNext(offset, offset->InstVal);
            } else if (offset->InstSt & 0x2000) {
                offset->actst |= 0x80000000;
            } else if (offset->InstSt & 0x400) {
                SetActNoNext(offset, 8);
            } else if (offset->InstSt & 0x800) {
                SetActNoNext(offset, 1);
            } else if (offset->InstSt & 0x1000) {
                SetActNoNext(offset, 0);
            } else {
                offset->InstSt |= 8;
            }
        }
    } else if (offset->AnimeTime + 1 == offset->AnimeRate) {
        if (offset->InstSt & 1) {
            if (~offset->InstSt & 0x10) {
                offset->InstSt |= 0x40;
            } else {
                offset->InstSt |= 0x80;
            }
        }
    } else {
        offset->InstSt &= 0xFF3F;
    }
}

TACTNORMAL *GetCellBeginAddr(CHARACTER_WORK *offset, int no) {
    u_int *p = (u_int *)(offset->ActHeaderp + 1);
    u_char id = ((TACTNORMAL *)p)->id;
    u_int CellNo = 0;

    for (;;) {
        switch (id) {
        case 0:
            if (CellNo == no) {
                return (TACTNORMAL *)p;
            }
            /* Single-pass scope preserves EE-GCC case-block scheduling. */
            do {
                p += GetInstSize(id) / sizeof(*p);
                CellNo++;
                id = ((TACTNORMAL *)p)->id;
            } while (0);
            break;
        case 0xFF:
            break;
        default:
            {
                p += GetInstSize(id) / sizeof(*p);
                id = ((TACTNORMAL *)p)->id;
            }
            break;
        }
    }
}
