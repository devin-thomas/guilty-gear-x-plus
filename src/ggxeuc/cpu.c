#include "ggxeuc/objtypes.h"
static const short idtb[] = { 1, 2, 3 }; /* the TU static every GGX TU carries (KR 0x5c04e0); its 8 bytes start .rodata */

typedef struct {
    u_int on;
    u_int trg;
    u_int unk8;
    u_int other[5];
} GAMEKEY;

extern GAMEKEY GameKey[];

typedef struct {
    short prob;
    u_short flag;
    short x4;
    short x6;
    short x8;
    short xa;
    int xc;
    int x10;
    int x14;
} THINKTB;

typedef struct {
    void *func;
    u_int flag;
    u_int status;
    u_int key;
    short unk10;
    u_char time;
    u_char unk13;
    int param;
    short prob[6];
    THINKTB *unk24;
    u_char unk28;
    u_char level;
    u_char unk2a;
    u_char unk2b;
} CPU_ENTRY;

extern CPU_ENTRY cpu_entry[];

typedef struct {
    int rand;
    int dist;
    int unk8;
    int unkc;
    int unk10;
    CHARACTER_WORK *enemy;
} CPU_STATIC;

extern CPU_STATIC CPUStaticWork[];
extern CPU_STATIC *CPUWorkp;

int GetOtherAttackFlagNoDist(CHARACTER_WORK *offset);
int GetOtherAttackGFlag(CHARACTER_WORK *offset);
int Random(void);
int Tension_Cmp(CHARACTER_WORK *offset, int val);
void CPUGuardCheck(CHARACTER_WORK *offset, CPU_ENTRY *cpu);
int CPUSitGuard(CHARACTER_WORK *offset, CPU_ENTRY *cpu);
int CPUStandGuard(CHARACTER_WORK *offset, CPU_ENTRY *cpu);
int CPUSkyGuard(CHARACTER_WORK *offset, CPU_ENTRY *cpu);
void SetCPUFunc(CPU_ENTRY *cpu, void *func, int arg, int param);
char *G_sprintf_arg1(char *fmt, int arg);
void CPUEM_Attack(CHARACTER_WORK *offset);

typedef struct {
    signed char time;
    char pad1[2];
    signed char level;
    char pad4[0x66 - 0x4];
    signed char x66[2];
    char pad68[0x6C - 0x68];
    int x6c[2];
    char pad74[0x78 - 0x74];
    u_int flag78;
    char pad7c[0x94 - 0x7C];
    int x94;
} G_INFO;
extern G_INFO g_info;

typedef struct {
    char pad0[0x25];
    signed char cpulvl;
} TR_INFO;
extern TR_INFO *tr_info;

typedef struct {
    int x0;
    char pad4[0x30 - 0x4];
    short cpulvl;
} SV_INFO;
extern SV_INFO *sv_info;

typedef struct {
    char pad0[0x484];
    int cpulvl;
} GL_INFO;
extern GL_INFO *gl_info;

int g_round(int val, int max, int min);
void debugf(char *fmt, ...);
void sv_set_cpulvl(int lvl);

void CpuDebugPrint(char *str) {
}

int CPUSitGuard(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    int atk;
    int prob;

    atk = GetOtherAttackFlagNoDist(offset);
    if (cpu->time == 0) {
        prob = (cpu->prob[2] << 6) / 10000 + 0x10;
        if ((Random() & 0xFF) < prob) {
            cpu->param = 1;
        } else {
            cpu->param = 0;
        }
        cpu->key |= 0xC0;
    }
    GameKey[offset->padid].on = 0xC000;
    if (cpu->param && Tension_Cmp(offset, 100) && atk) {
        GameKey[offset->padid].trg |= offset->ply->PunchKey | offset->ply->KickKey;
    }
    atk = GetOtherAttackGFlag(offset);
    if (offset->actst & 0x200) {
        if (!(cpu->status & 0x800)) {
            if (cpu->time) {
                cpu->time--;
            }
        }
        cpu->status |= 0x400;
        cpu->key &= ~0x80;
    } else if (!atk) {
        return 1;
    }
    if (cpu->status & 0x400) {
        if (!atk) {
            cpu->status |= 0x800;
        }
    }
    return 0;
}

int CPUStandGuard(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    int atk;
    int prob;

    atk = GetOtherAttackFlagNoDist(offset);
    if (cpu->time == 0) {
        prob = (cpu->prob[2] << 6) / 10000 + 0x10;
        if ((Random() & 0xFF) < prob) {
            cpu->param = 1;
        } else {
            cpu->param = 0;
        }
        cpu->key |= 0xC0;
    }
    GameKey[offset->padid].on = 0x8000;
    if (cpu->param && Tension_Cmp(offset, 100) && atk) {
        GameKey[offset->padid].trg |= offset->ply->PunchKey | offset->ply->KickKey;
    }
    atk = GetOtherAttackGFlag(offset);
    if (offset->actst & 0x200) {
        cpu->status |= 0x400;
        if (!(cpu->status & 0x800)) {
            if (cpu->time) {
                cpu->time--;
            }
        }
        if (atk & 2) {
            CPUGuardCheck(offset, cpu);
            if (cpu->time) {
                return 0;
            }
            return 4;
        }
        return 4;
    }
    return atk == 0;
}

int CPUSkyGuard(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    int atk;

    atk = GetOtherAttackFlagNoDist(offset);
    if (cpu->time == 0) {
        CpuDebugPrint("Exec:SkyGuard\n");
    }
    if (!atk) {
        return 1;
    }
    if (!(atk & 7)) {
        return 1;
    }
    if ((~atk & 4) && cpu->param && Tension_Cmp(offset, 1)) {
        GameKey[offset->padid].on |= offset->ply->PunchKey | offset->ply->KickKey;
        GameKey[offset->padid].trg |= offset->ply->PunchKey | offset->ply->KickKey;
    }
    GameKey[offset->padid].on = 0x8000;
    return 0;
}

int CPUNeutral(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    if (cpu->time == 0) {
        cpu->key |= 0x41;
        CpuDebugPrint("Exec:Neutral\n");
    }
    cpu->flag = 1;
    GameKey[offset->padid].on = 0;
    return 0;
}

int CPUForward(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    if (cpu->time == 0) {
        cpu->key |= 0x41;
    }
    cpu->flag = 2;
    GameKey[offset->padid].on = 0x2000;
    if (cpu->time > 4 && CPUWorkp->dist < 10000) {
        cpu->time += 2;
    }
    return 0;
}

int CPUBack(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    if (cpu->time == 0) {
        cpu->key |= 0x41;
    }
    cpu->flag = 3;
    GameKey[offset->padid].on = 0x8000;
    return 0;
}

int CPUFDash(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    cpu->flag = 4;
    if (cpu->time == 0) {
        cpu->key |= 0x40;
    }
    GameKey[offset->padid].on = 0x2000;
    if (cpu->time > 6 && CPUWorkp->dist < 30000) {
        cpu->time += 2;
    }
    return 0;
}

int CPUBDash(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    cpu->flag = 5;
    GameKey[offset->padid].on = 0x8000;
    return 0;
}

int CPUVJump(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    cpu->flag = 9;
    if (offset->actst & 0x10) {
        return 1;
    }
    if (cpu->time == 0) {
        cpu->prob[5] = cpu->prob[5] * 0.5;
    }
    GameKey[offset->padid].on = 0x1000;
    return 0;
}

int CPUBJump(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    cpu->flag = 8;
    if (offset->actst & 0x10) {
        return 1;
    }
    if (cpu->time == 0) {
        cpu->prob[5] = cpu->prob[5] * 0.5;
    }
    GameKey[offset->padid].on = 0x9000;
    return 0;
}

int CPUFJump(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    cpu->flag = 7;
    if (offset->actst & 0x10) {
        return 1;
    }
    if (cpu->time == 0) {
        cpu->prob[5] = cpu->prob[5] * 0.5;
    }
    GameKey[offset->padid].on = 0x3000;
    return 0;
}

int CPUSkyRandAttack(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    CPU_STATIC *w;

    w = CPUWorkp;
    cpu->flag = 6;
    if (w->enemy->actst & 0x10) {
        switch ((w->rand >> 3) & 3) {
        case 0:
            GameKey[offset->padid].trg |= offset->ply->PunchKey;
            break;
        case 1:
            GameKey[offset->padid].trg |= offset->ply->KickKey;
            break;
        case 2:
            GameKey[offset->padid].trg |= offset->ply->SlashKey;
            break;
        case 3:
            GameKey[offset->padid].trg |= offset->ply->HighSlashKey;
            break;
        }
        return 1;
    }
    if (offset->yspeed > 0) {
        if (w->unkc < ((Random() & 0x3F) + 120) * 100) {
            switch ((CPUWorkp->rand >> 3) & 3) {
            case 0:
                GameKey[offset->padid].trg |= offset->ply->PunchKey;
                break;
            case 1:
                GameKey[offset->padid].trg |= offset->ply->KickKey;
                break;
            case 2:
                GameKey[offset->padid].trg |= offset->ply->SlashKey;
                break;
            case 3:
                GameKey[offset->padid].trg |= offset->ply->HighSlashKey;
                break;
            }
        }
    }
    return 0;
}

void CPUATTACKKEYSET(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    GameKey[offset->padid].trg = 0;
    GameKey[offset->padid].on = 0;
    if (cpu->param & 0x1000) {
        GameKey[offset->padid].trg |= offset->ply->PunchKey;
    }
    if (cpu->param & 0x2000) {
        GameKey[offset->padid].trg |= offset->ply->KickKey;
    }
    if (cpu->param & 0x4000) {
        GameKey[offset->padid].trg |= offset->ply->SlashKey;
    }
    if (cpu->param & 0x8000) {
        GameKey[offset->padid].trg |= offset->ply->HighSlashKey;
    }
    if (cpu->param & 0x100) {
        GameKey[offset->padid].trg |= 0x1000;
    }
    if (cpu->param & 0x200) {
        GameKey[offset->padid].trg |= 0x4000;
    }
    if (cpu->param & 0x400) {
        GameKey[offset->padid].trg |= 0x8000;
    }
    if (cpu->param & 0x800) {
        GameKey[offset->padid].trg |= 0x2000;
    }
    GameKey[offset->padid].on = GameKey[offset->padid].unk8 = GameKey[offset->padid].trg;
}

int CPUAttack(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    if (cpu->time == 0) {
        CpuDebugPrint(G_sprintf_arg1("At%8x\n", cpu->param));
    }
    if (!(cpu->status & 8)) {
        CPUATTACKKEYSET(offset, cpu);
    }
    if (cpu->status & 4) {
        CPUEM_Attack(offset);
        cpu->status = (cpu->status | 8) & ~4;
    }
    if (cpu->key & 0x38) {
        return 3;
    }
    return 0;
}

int CPUTableChange(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {

    CpuDebugPrint(G_sprintf_arg1("TableChange %8x\n", cpu->param));
    cpu->unk24 = cpu->param;
    cpu->unk28 = 0;
    cpu->param = 0;
    return 2;
}

int func_001B1A18(void);

void SetCPULevel(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    static u_char tb[6] = { 0, 2, 3, 4, 5, 16 };
    static signed char timetb[6] = { 120, 60, 45, 40, 35, 10 };
    int level;
    int time;
    int sec;
    signed char rank;
    short lvl;

    level = (signed char)tb[g_info.level];
    sec = g_info.time * 60;
    rank = g_info.x66[offset->padid == 0];
    time = timetb[g_info.level] * sec;
    if (rank >= 9) {
        time = time * 0.75f;
        level *= 2;
    } else if (rank >= 8) {
        time = time * 0.75f;
        level *= 3;
    } else if (rank >= 7) {
        time = time * 0.7f;
        level *= 2;
    } else if (rank >= 4) {
        time = time * 0.8f;
        level *= 2;
    } else if (rank >= 2) {
        time = time * 0.9f;
    } else {
        level /= 2;
    }
    cpu->level = level + g_info.x6c[offset->padid == 0] / time;
    cpu->level = g_round(cpu->level, 0x20, 0);
    if (g_info.level == 0) {
        cpu->level = g_round(0, 0x20, 0);
    }
    if (g_info.level == 5) {
        cpu->level = g_round(0x20, 0x20, 0);
    }
    if (g_info.flag78 & 0x100) {
        cpu->level = g_round(tr_info->cpulvl, 0x20, 0);
    }
    if (g_info.flag78 & 0x400) {
        if (g_info.x94 == 1) {
            cpu->level = g_round(0x20, 0x20, 0);
        } else if (g_info.x94 == 2) {
            cpu->level = g_round(0x20, 0x20, 0);
        } else {
            cpu->level = 10;
        }
    }
    if (g_info.flag78 & 0x8000) {
        cpu->level = g_round(gl_info->cpulvl, 0x20, 0);
    }
    if (g_info.flag78 & 0x2000) {
        cpu->level = g_round(func_001B1A18(), 0x20, 0);
    }
    if (g_info.flag78 & 0x200) {
        lvl = sv_info->cpulvl;
        if (lvl == 20) {
            sv_set_cpulvl(10);
        } else if (lvl == 30) {
            sv_set_cpulvl(15);
        } else if (lvl == 40) {
            sv_set_cpulvl(20);
        } else if (lvl == 50) {
            sv_set_cpulvl(22);
        } else if (lvl == 60) {
            sv_set_cpulvl(24);
        } else if (lvl == 70) {
            sv_set_cpulvl(26);
        } else if (lvl == 80) {
            sv_set_cpulvl(28);
        } else if (lvl == 90) {
            sv_set_cpulvl(30);
        } else if (lvl == 99) {
            sv_set_cpulvl(32);
        } else {
            sv_set_cpulvl(lvl / 4);
        }
    }
    debugf("SetCPULevel %d\n", cpu->level);
}

void SetCPUFunc(CPU_ENTRY *cpu, void *func, int arg, int param) {
    cpu->func = func;
    cpu->unk13 = arg;
    cpu->status &= ~0xC;
    cpu->param = param;
    cpu->key = 0;
    cpu->time = 0;
}

int g_roundmin(int val, int min);

short GetCPUprobability(CPU_ENTRY *cpu, THINKTB *tb) {
    float rate;
    u_short flag;
    short prob;
    short x;
    int d;

    rate = 0;
    flag = tb->flag;
    prob = tb->prob;
    if (flag & 0x100) {
        if (!(cpu->status & 0x80)) {
            return 0;
        }
    }
    if (flag & 0x200) {
        if (!(cpu->status & 0x100)) {
            return 0;
        }
    }
    if (flag & 0x2000) {
        prob = prob * (cpu->prob[5] / 10000.0);
    }
    if (flag & 0x400) {
        if (!(cpu->status & 0x10)) {
            return 0;
        }
        if (cpu->level < 8) {
            prob /= 8;
        } else if (cpu->level < 16) {
            prob /= 2;
        }
    }
    if (flag & 0x1000) {
        if (!(cpu->status & 0x200)) {
            return 0;
        }
        if (cpu->level < 8) {
            prob /= 8;
        } else if (cpu->level < 16) {
            prob /= 2;
        }
    }
    if (flag & 0x800) {
        rate = g_roundmin(cpu->prob[4] - 1000.0, 0) / 10000.0;
        prob = prob * rate;
    }
    switch (flag & 0xF0) {
    case 0x00:
        return prob;
    case 0x10:
        x = cpu->prob[0];
        break;
    case 0x20:
        x = cpu->prob[1];
        break;
    case 0x30:
        x = cpu->prob[2];
        break;
    case 0x40:
        x = cpu->prob[3];
        break;
    case 0x50:
        flag &= 0xF;
        d = flag - (cpu->level >> 1);
        if (d < -2) {
            prob = 0;
        } else if (d == -2) {
            prob /= 4;
        } else if (d == -1) {
            prob /= 2;
        }
        return prob;
    default:
        return prob;
    }
    flag &= 0xF;
    if (flag == 0) {
        rate = x * 0.5 / 10000.0 + 1.0;
    } else if (flag == 1) {
        rate = x * 0.25 / 10000.0 + 1.0;
    } else if (flag == 2) {
        rate = x / 10000.0 + 1.0;
    } else if (flag == 3) {
        rate = x * 2.0 / 10000.0 + 1.0;
    } else if (flag == 4) {
        rate = x * 3.0 / 10000.0 + 1.0;
    } else if (flag == 5) {
        rate = x * 4.0 / 10000.0 + 1.0;
    } else if (flag == 6) {
        rate = x / 10000.0;
    } else if (flag == 7) {
        rate = x * 0.5 / 10000.0 + 0.5;
    } else if (flag == 8) {
        rate = x * 0.75 / 10000.0 + 0.25;
    }
    prob = rate * prob;
    return prob;
}

typedef struct {
    short prob;
    THINKTB *tb;
} THINKSEL;

void CPUTHINKTBExecute(CPU_ENTRY *cpu, THINKTB *tb) {
    THINKSEL list[64];
    int n;
    int total;
    int i;

    total = 0;
    if (tb->prob != -1) {
        n = 0;
        for (i = 0; i < 64; i++, tb++) {
            if (tb->prob == 0) {
                break;
            }
            if (tb->prob < 0) {
                continue;
            }
            if (tb->x8 != 0) {
                if (CPUWorkp->unk8 - tb->x4 < 0) {
                    continue;
                }
                if (CPUWorkp->unk8 - tb->x8 > 0) {
                    continue;
                }
            }
            if (tb->xa != 0) {
                if (CPUWorkp->unk10 - tb->x6 > 0) {
                    continue;
                }
                if (CPUWorkp->unk10 - tb->xa < 0) {
                    continue;
                }
            }
            list[n].prob = GetCPUprobability(cpu, tb);
            list[n].tb = tb;
            total += list[n].prob;
            n++;
        }
        if (total == 0) {
            CpuDebugPrint("CpuThinkTb False\n");
            return;
        }
        total = Random() % total;
        for (n = 0; total >= list[n].prob; n++) {
            total -= list[n].prob;
        }
        tb = list[n].tb;
    }
    if (tb->x10 & 0x10000) {
        SetCPUFunc(cpu, (void *)tb->xc, (CPUWorkp->rand & 0xF) + 10, tb->x14);
    } else {
        SetCPUFunc(cpu, (void *)tb->xc, 3, tb->x14);
    }
    cpu->key = (u_short)tb->x10;
}

int g_roundmax(int val, int max);

void CPUGuardCheck(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    static u_char table[32] = {
        0, 64, 4, 72, 8, 72, 12, 80,
        16, 96, 24, 96, 32, 104, 44, 104,
        56, 112, 68, 112, 80, 120, 96, 120,
        112, 128, 128, 136, 128, 144, 128, 152,
    };
    int atk;
    int rnd;
    int prob;
    int i;
    u_char lvl;

    atk = GetOtherAttackFlagNoDist(offset);
    rnd = Random() & 0xFF;
    lvl = cpu->level;
    if (atk) {
        i = g_roundmax(lvl >> 1, 15) * 2;
        prob = table[i] + cpu->prob[0] / (10000 / table[i + 1]);
        if (rnd < prob) {
            if (offset->actst & 0x10) {
                rnd = Random() & 0xFF;
                SetCPUFunc(cpu, CPUSkyGuard, 30, prob > rnd * 0.75);
            } else if ((atk & 3) == 3) {
                if (Random() & 3) {
                    SetCPUFunc(cpu, CPUStandGuard, 30, 0);
                } else {
                    SetCPUFunc(cpu, CPUSitGuard, 30, 0);
                }
            } else if (atk & 2) {
                SetCPUFunc(cpu, CPUSitGuard, 30, 0);
            } else if (atk & 1) {
                SetCPUFunc(cpu, CPUStandGuard, 30, 0);
            }
        }
        cpu->key &= ~1;
    }
}

typedef struct {
    short idno;
    u_short actno;
    short num;
    short min;
    short max;
    short ret;
} EVASIONTB;

CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *offset, int idno, int a, int b);

int SearchEvasionObject(CHARACTER_WORK *offset) {
    static EVASIONTB table[] = {
        { 1, 44, 5, 150, 250, 1 },
        { 1, 44, 6, 150, 250, 1 },
        { 1, 44, 7, 150, 250, 1 },
        { 1, 44, 9, 150, 250, 1 },
        { 2, 41, 1, 200, 300, 1 },
        { 2, 41, 4, 150, 250, 1 },
        { 4, 33, 1, 100, 200, 1 },
        { 8, 65, 0, 100, 200, 1 },
        { 8, 65, 1, 70, 150, 1 },
        { 11, 80, 0, -150, 0, 1 },
        { 11, 80, 1, 100, 200, 1 },
        { 13, 78, 0, 120, 220, 1 },
        { 15, 34, 0, 120, 220, 1 },
        { 15, 50, 1, 300, 400, 1 },
        { 0, 0, 1, 300, 400, 1 },
    };
    EVASIONTB *p;
    CHARACTER_WORK *enemy;
    CHARACTER_WORK *obj;
    int d;
    int dy;

    p = table;
    enemy = GetEnemyPlayer(offset);
    for (; p->idno != 0; p++) {
        if (p->idno == enemy->idno) {
            obj = SearchCharacterExecute(enemy, p->actno, p->num, 7);
            if (obj) {
                d = obj->posx - offset->posx;
                if (offset->dirflag == 0) {
                    d = -d;
                }
                d /= 100;
                if (p->min < d && d < p->max) {
                    dy = offset->posy - obj->posy;
                    d = dy < 0 ? -dy : dy;
                    if (d / 100 < 150) {
                        return p->ret;
                    }
                }
            }
        }
    }
    return 0;
}

extern THINKTB *CpuThinkLandTbByIdno[];
extern THINKTB *CpuThinkSkyTbByIdno[];

void SetDefaultThinkTb(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    if (offset->actst & 0x10) {
        CpuDebugPrint("Table:SkyDefault\n");
        cpu->unk24 = CpuThinkSkyTbByIdno[offset->idno];
        cpu->unk28 = 1;
    } else {
        CpuDebugPrint("Table:LandDefault\n");
        cpu->unk24 = CpuThinkLandTbByIdno[offset->idno];
        cpu->unk28 = 0;
    }
}

void CPUEM_Damage(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;

    if (offset->ply && offset->ply->CPU) {
        cpu = &cpu_entry[offset->padid];
        cpu->prob[3] += 400;
        cpu->prob[0] += 100;
        cpu->prob[1] += 200;
        cpu->prob[4] += (cpu->level * 2 + 20) * 20;
    }
}

void CPUEM_Guard(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;

    if (offset->ply && offset->ply->CPU) {
        cpu = &cpu_entry[offset->padid];
        cpu->prob[3] += 400;
        cpu->prob[0] -= 500;
        cpu->prob[2] += 1000;
        cpu->prob[1] += 100;
        cpu->prob[4] += cpu->level * 60;
    }
}

void CPUEM_Hit(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;

    if (offset->ply && offset->ply->CPU) {
        cpu = &cpu_entry[offset->padid];
        cpu->prob[3] -= 400;
        cpu->prob[2] -= 200;
        cpu->prob[1] += 800;
    }
}

void CPUEM_Attack(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;
    int lvl;

    if (offset->ply && offset->ply->CPU) {
        cpu = &cpu_entry[offset->padid];
        lvl = cpu->level;
        if (lvl < 8) {
            cpu->prob[4] = cpu->prob[4] * 0.3;
        } else if (lvl < 16) {
            cpu->prob[4] = cpu->prob[4] * 0.6;
        } else if (lvl < 24) {
            cpu->prob[4] = cpu->prob[4] * 0.75;
        } else {
            cpu->prob[4] = cpu->prob[4] * 0.85;
        }
    }
}

extern int BattleStatus;
int CPUInit(CHARACTER_WORK *offset);
int GetPlayerDist(void);
int GetPlayerDistOff(CHARACTER_WORK *offset);
int GetPlayerDisty(void);
int GetPlayerDistyOff(CHARACTER_WORK *offset);
int GetTensionVal(CHARACTER_WORK *offset);
int SearchEvasionObject(CHARACTER_WORK *offset);
void SetDefaultThinkTb(CHARACTER_WORK *offset, CPU_ENTRY *cpu);
void CPUTHINKTBExecute(CPU_ENTRY *cpu, THINKTB *tb);
int CPUFJump(CHARACTER_WORK *offset, CPU_ENTRY *cpu);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);

int CPUMain(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;
    int idx;
    int r;
    u_char t;
    u_char lvl;

    if (!offset->ply) {
        return 0;
    }
    if (!offset->ply->CPU) {
        return 0;
    }
    if (offset->ply->CPU == 1) {
        CPUInit(offset);
        return 0;
    }
    if (BattleStatus != 2) {
        return 0;
    }
    cpu = &cpu_entry[offset->padid];
    CPUWorkp = &CPUStaticWork[offset->padid];
    CPUWorkp->rand = Random();
    CPUWorkp->dist = GetPlayerDist();
    CPUWorkp->unk8 = GetPlayerDistOff(offset) / 100;
    CPUWorkp->unkc = GetPlayerDisty();
    CPUWorkp->enemy = offset->ply->Enemy;
    CPUWorkp->unk10 = GetPlayerDistyOff(offset) / 100;
    cpu->prob[0] = g_round(cpu->prob[0], 10000, 0);
    cpu->prob[1] = g_round(cpu->prob[1], 10000, 0);
    cpu->prob[2] = g_round(cpu->prob[2], 10000, 0);
    cpu->prob[3] = g_round(cpu->prob[3], 10000, 0);
    cpu->prob[4] = g_round(cpu->prob[4], 10000, 0);
    cpu->prob[5] = g_round(cpu->prob[5], 10000, 0);
    GameKey[offset->padid].trg = 0;
    GameKey[offset->padid].on = 0;
    cpu->status &= ~0x230;
    if (GetTensionVal(offset) >= 5000) {
        cpu->status |= 0x10;
    }
    if (GetTensionVal(offset) >= 10000) {
        cpu->status |= 0x200;
    }
    if (GetEnemyPlayer(offset)->actst & 0x20) {
        cpu->status |= 0x20;
    }
    if (offset->attackst & 0x1000) {
        if (!(cpu->status & 1)) {
            if (offset->attackst & 0x10) {
                CpuDebugPrint("Recv:Hit.Chain \n");
            } else {
                CpuDebugPrint("RECV:Hit\n");
            }
            cpu->status |= 1;
        }
    } else {
        cpu->status &= ~1;
    }
    if (offset->attackst & 0x80000) {
        cpu->status |= 0x40;
    } else {
        cpu->status &= ~0x40;
    }
    if (offset->posy >= 0) {
        cpu->status &= ~0x80;
        cpu->status &= ~0x100;
    } else if (offset->yspeed < 0) {
        cpu->status |= 0x80;
        cpu->status &= ~0x100;
    } else {
        cpu->status &= ~0x80;
        cpu->status |= 0x100;
    }
    if (offset->actnonext != 0xFFFFFFFF && *(u_short *)&offset->actnonext < 2) {
        offset->CommandFlag &= ~0x40;
    }
    if (cpu->key & 0x40) {
        if (SearchEvasionObject(offset)) {
            CpuDebugPrint("Search:Evasion\n");
            if ((CPUWorkp->rand & 0x7F) <= cpu->level + 32) {
                SetCPUFunc(cpu, CPUFJump, 3, 0);
            }
            cpu->key &= ~0x40;
        }
    }
    if (cpu->key & 1) {
        CPUGuardCheck(offset, cpu);
    }
    if (cpu->key & 4) {
        if (!(offset->CommandFlag & 0x40)) {
            CpuDebugPrint("Break:Next:Neutral\n");
            cpu->func = 0;
        }
    }
    if (cpu->key & 0x20) {
        if (!(offset->CommandFlag & 0x40)) {
            CpuDebugPrint("Break:End:Neutral\n");
            cpu->unk24 = 0;
            cpu->func = 0;
        } else if (cpu->status & 8) {
            CpuDebugPrint("Break:Next:ATKRSV\n");
            cpu->func = 0;
        }
    }
    if ((cpu->key & 8) && (cpu->status & 8)) {
        if (!(offset->CommandFlag & 0x40)) {
            CpuDebugPrint("Break:Neutral\n");
            cpu->unk24 = 0;
            cpu->func = 0;
        } else if (cpu->status & 1) {
            CpuDebugPrint("Break:Chain\n");
            cpu->func = 0;
        }
    }
    if ((cpu->key & 0x10) && (cpu->status & 8)) {
        if (!(offset->CommandFlag & 0x40)) {
            CpuDebugPrint("Break:Neutral\n");
            cpu->unk24 = 0;
            cpu->func = 0;
        } else if (cpu->status & 0x40) {
            CpuDebugPrint("Break:Dmg\n");
            cpu->func = 0;
        } else if (cpu->status & 1) {
            CpuDebugPrint("Break:Chain\n");
            cpu->unk24 = 0;
            cpu->func = 0;
        }
    }
    if (offset->actst & 0x20) {
        if ((offset->CommandFlag & 0x800) && !((r = CPUWorkp->rand) & 0xF)) {
            int landkey[4] = { 0, 0, 0x8000, 0x2000 };
            int skykey[4] = { 0, 0x8000, 0x2000, 0x2000 };

            idx = (r >> 4) & 3;
            if (offset->actst & 0x2800) {
                idx = skykey[idx];
            } else {
                idx = landkey[idx];
            }
            GameKey[offset->padid].trg = offset->ply->SlashKey | offset->ply->HighSlashKey | idx;
        }
        cpu->func = 0;
        cpu->unk24 = 0;
        return 1;
    }
    if (offset->actst & 0x4000) {
        cpu->func = 0;
        cpu->unk24 = 0;
        return 1;
    }
    if (offset->idno == 10 && (u_int)(offset->actno - 0xA4) < 3 && (cpu->key & 0x38)) {
        cpu->func = 0;
        cpu->unk24 = 0;
    }
    lvl = cpu->level;
    cpu->prob[3] -= 6;
    cpu->prob[0] += (lvl >> 2) + 2;
    cpu->prob[2] -= 4;
    cpu->prob[1] -= 6;
    cpu->prob[5] += 16;
    if (!(offset->CommandFlag & 0x40)) {
        cpu->prob[4] += lvl * 3 + 10;
    } else {
        cpu->prob[4] += cpu->level + 3;
    }
    if (!cpu->func) {
        if (!cpu->unk24) {
            SetDefaultThinkTb(offset, cpu);
        }
        if (!cpu->unk24->prob) {
            SetDefaultThinkTb(offset, cpu);
        }
        if ((cpu->unk28 & 1) && !(offset->actst & 0x10)) {
            SetDefaultThinkTb(offset, cpu);
        }
        CPUTHINKTBExecute(cpu, cpu->unk24);
        if (cpu->unk24->prob < 0) {
            cpu->unk24++;
        }
        if (!cpu->func) {
            return 1;
        }
    }
    switch (((int (*)(CHARACTER_WORK *, CPU_ENTRY *))cpu->func)(offset, cpu)) {
    case 0:
        if (cpu->time >= cpu->unk13) {
            CpuDebugPrint("TimeOut\n");
            cpu->unk13 = 0xFF;
            cpu->status &= ~0xC00;
            cpu->func = 0;
            cpu->time = 0;
            cpu->unk10 = 0;
            SetDefaultThinkTb(offset, cpu);
            break;
        }
        if (cpu->time < 0xFF) {
            cpu->time++;
        }
        break;
    case 1:
        CpuDebugPrint("Table:Break\n");
        cpu->key |= 0x41;
        cpu->unk13 = 0xFF;
        cpu->status &= ~0xC00;
        cpu->func = 0;
        cpu->unk24 = 0;
        cpu->time = 0;
        cpu->unk10 = 0;
        SetDefaultThinkTb(offset, cpu);
        break;
    case 2:
        cpu->func = 0;
        cpu->unk10 = 0;
        break;
    case 3:
        if (cpu->time < 0xFF) {
            cpu->time++;
        }
        break;
    case 4:
        break;
    }
    return 1;
}

int CPUMain(CHARACTER_WORK *offset);

int CPUExecute(CHARACTER_WORK *offset) {
    int ret;

    ret = CPUMain(offset);
    GameKey[offset->padid].on |= GameKey[offset->padid].trg;
    return ret;
}

void SetCPULevel(CHARACTER_WORK *offset, CPU_ENTRY *cpu);

int CPUInit(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;
    int i;

    if (!offset->ply) {
        return 0;
    }
    if (offset->ply->CPU) {
        offset->ply->CPU = 2;
        cpu = &cpu_entry[offset->padid];
        cpu->func = 0;
        cpu->prob[0] = 5000;
        cpu->prob[1] = 5000;
        cpu->prob[2] = 5000;
        cpu->prob[3] = 5000;
        cpu->prob[4] = 5000;
        cpu->prob[5] = 5000;
        cpu->status = 0;
        cpu->unk24 = 0;
        SetCPULevel(offset, cpu);
        CPUWorkp = &CPUStaticWork[offset->padid];
        i = 16;
        while (i--) {
            CpuDebugPrint("Guilty Gear X\n");
        }
    }
    return 0;
}

u_int GetCPUFlag(CHARACTER_WORK *offset) {
    CPU_ENTRY *cpu;

    if (!offset->ply->CPU) {
        return 0;
    }
    cpu = &cpu_entry[offset->padid];
    return cpu->flag;
}

int GetCPUKey(CHARACTER_WORK *offset, int key) {
    CPU_ENTRY *cpu;

    if (!offset->ply->CPU) {
        return 0;
    }
    cpu = &cpu_entry[offset->padid];
    return (u_char)cpu->param == key;
}

void SetCPUStatus(CHARACTER_WORK *offset, u_int status) {
    CPU_ENTRY *cpu;

    if (offset->ply->CPU) {
        cpu = &cpu_entry[offset->padid];
        cpu->status |= status;
    }
}

void sv_set_cpulvl(int lvl) {
    CPU_ENTRY *cpu;

    cpu = sv_info->x0 ? &cpu_entry[0] : &cpu_entry[1];
    cpu->level = g_round(lvl, 32, 0);
}

u_char get_cpulvl(int pl) {
    return cpu_entry[pl].level;
}

/* Second switch on bit 7 with an empty-effect case 1 (on |= 0): the case-1 test lives until jump2/dbr, which leaves the dead li v0,1 in the bnez slot, gives the beql dispatch, and its GameKey reference puts %hi(GameKey) in t0. */
int CPURandAttack(CHARACTER_WORK *offset, CPU_ENTRY *cpu) {
    cpu->flag = 6;
    GameKey[offset->padid].trg = 0;
    GameKey[offset->padid].on = 0;
    switch ((CPUWorkp->rand >> 3) & 3) {
    case 0: GameKey[offset->padid].trg |= offset->ply->PunchKey; break;
    case 1: GameKey[offset->padid].trg |= offset->ply->KickKey; break;
    case 2: GameKey[offset->padid].trg |= offset->ply->SlashKey; break;
    case 3: GameKey[offset->padid].trg |= offset->ply->HighSlashKey; break;
    }
    switch ((CPUWorkp->rand >> 7) & 1) {
    case 0: GameKey[offset->padid].on |= 0x4000; break;
    case 1: GameKey[offset->padid].on |= 0; break;
    }
    return 0;
}
