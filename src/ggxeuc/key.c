#include "ggxeuc/objtypes.h"

typedef struct {
    u_int on;
    u_int trg;
    u_int off;
    u_int other[5];
} GAMEKEY;

extern GAMEKEY GameKey[];
extern GAMEKEY pads[];
int GetCPUFlag();

typedef struct {
    u_char key;
    u_char time;
} KEYDATA;

extern KEYDATA CommandKeyData[2][32];
extern int KeyIndex[2];
extern CHARACTER_WORK *PlayerWork;
extern u_short CommandButtonData[2][4];
extern u_int RendaButtonData[2][4];
extern int RendaIndex[2];
int DirectionDirectGet(CHARACTER_WORK *offset);
KEYDATA *PlayerKeyDataGet(int pl, int n);
u_char KeyDataLRFlip(u_char key, int dir);
void PlayerKeyDataSave(int pl);
int KeyRight(u_char key);
int KeyLeft(u_char key);

typedef struct {
    char pad[0x456];
    u_char flag[2];
} USERINFO;
extern USERINFO userinfo;
extern int BattleStatus;
u_int DirectDirectionKeyGet(CHARACTER_WORK *offset);
int IsPG(CHARACTER_WORK *offset);

typedef struct {
    u_short cmd;
    short time;
} COMMAND;
int GetCPUKey(CHARACTER_WORK *offset, int n);
int GetRenda(CHARACTER_WORK *offset, int btn);
int IsCommand(CHARACTER_WORK *offset, COMMAND *cmd);
u_int IsTrgPunch(CHARACTER_WORK *offset);
u_int IsTrgKick(CHARACTER_WORK *offset);
u_int IsTrgSlash(CHARACTER_WORK *offset);
u_int IsTrgHighSlash(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, u_short mask, int count, int time);

void SetLR(CHARACTER_WORK *offset) {
    if (GetCPUFlag() == 0) {
        if (offset->dirflag == 0) {
            GameKey[offset->padid].on = pads[offset->padid].on & ~0xa000;
            if (pads[offset->padid].on & 0x2000) GameKey[offset->padid].on |= 0x8000;
            if (pads[offset->padid].on & 0x8000) GameKey[offset->padid].on |= 0x2000;
            GameKey[offset->padid].trg = pads[offset->padid].trg & ~0xa000;
            if (pads[offset->padid].trg & 0x2000) GameKey[offset->padid].trg |= 0x8000;
            if (pads[offset->padid].trg & 0x8000) GameKey[offset->padid].trg |= 0x2000;
            GameKey[offset->padid].off = pads[offset->padid].off & ~0xa000;
            if (pads[offset->padid].off & 0x2000) GameKey[offset->padid].off |= 0x8000;
            if (pads[offset->padid].off & 0x8000) GameKey[offset->padid].off |= 0x2000;
        } else {
            GameKey[offset->padid].on = pads[offset->padid].on;
            GameKey[offset->padid].trg = pads[offset->padid].trg;
            GameKey[offset->padid].off = pads[offset->padid].off;
        }
    }
}

u_int DirectDirectionKeyGet(CHARACTER_WORK *offset) {
    u_int key = offset->posdirflag;
    u_int k;

    if (GetCPUFlag() != 0) {
        return GameKey[offset->padid].on;
    }
    if (key == 0) {
        k = pads[offset->padid].on;
        key = k & ~0xa000;
        if (k & 0x2000) key |= 0x8000;
        if (k & 0x8000) key |= 0x2000;
    } else {
        key = pads[offset->padid].on;
    }
    return key;
}

KEYDATA *PlayerKeyDataGet(int pl, int n) {
    int idx = KeyIndex[pl] - n;

    if (idx < 0) idx += 32;
    return &CommandKeyData[pl][idx];
}

u_char KeyDataLRFlip(u_char key, int dir) {
    if (dir != 0) return key;
    if (!(key & 0x20)) key ^= 0x10;
    return key;
}

void PlayerKeyDataClear(void) {
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 32; j++) {
            CommandKeyData[i][j].key = 0xff;
            CommandKeyData[i][j].time = 0;
        }
        KeyIndex[i] = 0;
    }
}

void PlayerKeyDataClearHalf(int pl) {
    int j;

    for (j = 0; j < 32; j++) {
        CommandKeyData[pl][j].key = 0xff;
        CommandKeyData[pl][j].time = 0;
    }
    KeyIndex[pl] = 0;
}

int KeyRight(u_char key) {
    int k = key;

    if (k < 0x13) {
        if (k >= 0x10) return 1;
    }
    return 0;
}

int KeyLeft(u_char key) {
    int k = key;

    if (k < 3) {
        if (k >= 0) return 1;
    }
    return 0;
}

void PlayerCommandKeyExecute(CHARACTER_WORK *offset) {
    PlayerKeyDataSave(offset->padid);
}

int IsDash(CHARACTER_WORK *offset) {
    int i = 1;
    int flag = 0;
    u_char dir = offset->dirflag;
    int time;
    KEYDATA *kd;
    int key;

    if (GetCPUFlag() == 4) return 1;
    if (offset->ply->CPU != 0) return 0;
    if (!(offset->actst & 0x10)) dir = offset->posdirflag;
    kd = PlayerKeyDataGet(offset->padid, 0);
    if (KeyDataLRFlip(kd->key, dir) != 0x10) return 0;
    time = kd->time;
    if (time >= 2) return 0;
    for (;;) {
        kd = PlayerKeyDataGet(offset->padid, i);
        key = KeyDataLRFlip(kd->key, dir);
        time += kd->time;
        if (time >= 13) break;
        if (key & 1) return 0;
        if (userinfo.flag[offset->padid] & 1) {
            if (key == 0x22 || key == 0x20 || key == 0x12) flag = 1;
            else if (KeyRight(key) && flag) return 1;
        } else {
            if (key == 0x20) flag = 1;
            else if (KeyRight(key) && flag) return 1;
        }
        i++;
    }
    return 0;
}

int IsBackStep(CHARACTER_WORK *offset) {
    int flag = 0;
    int i = 1;
    u_char dir = offset->dirflag;
    int time;
    KEYDATA *kd;
    int key;

    if (GetCPUFlag() == 5) return 1;
    if (offset->ply->CPU != 0) return 0;
    if (!(offset->actst & 0x10)) dir = offset->posdirflag;
    kd = PlayerKeyDataGet(offset->padid, 0);
    if (KeyDataLRFlip(kd->key, dir) != 0) return 0;
    time = kd->time;
    if (time >= 2) return 0;
    for (;;) {
        kd = PlayerKeyDataGet(offset->padid, i);
        key = KeyDataLRFlip(kd->key, dir);
        time += kd->time;
        if (time >= 13) break;
        if (key & 1) return 0;
        if (key == 0x20) flag = 1;
        else if (KeyLeft(key) && flag) return 1;
        i++;
    }
    return 0;
}

void SetGuardSt(CHARACTER_WORK *offset, int mode) {
    u_int back;
    int pad = offset->padid;

    back = GameKey[pad].on & 0x8000;
    back |= DirectDirectionKeyGet(offset) & 0x8000;
    if ((offset->actst & 0x80000) && offset->HitStopTime != 0) return;
    if (BattleStatus != 2) return;
    if (offset->GuardSt & 0x200) {
        offset->GuardSt &= ~0x200;
        return;
    }
    if (mode & 1) offset->GuardSt &= 0xfff8;
    if (mode & 2) offset->GuardSt &= 0xfff0;
    if (!(offset->CommandFlag & 0x100) && !(offset->actst & 0x200)) return;
    if ((mode & 2) && (back || (offset->actst & 0x200)) && mode && (offset->GuardSt & 0x40) && IsPG(offset)) {
        offset->GuardSt |= 8;
        if (offset->actst & 0x10) offset->GuardSt |= 4;
        else if (GameKey[pad].on & 0x4000) offset->GuardSt |= 2;
        else offset->GuardSt |= 1;
    }
    if (mode & 1) {
        if (back || (offset->actst & 0x200)) {
            if (offset->actst & 0x10) offset->GuardSt |= 4;
            else if (GameKey[pad].on & 0x4000) offset->GuardSt |= 2;
            else offset->GuardSt |= 1;
        }
    }
}

int IsCommand(CHARACTER_WORK *offset, COMMAND *cmd) {
    int i = 1;
    int ok = 0;
    int prev;
    u_char dir = offset->dirflag;
    KEYDATA *kd;
    int key;
    int time;

    if (offset->ply->CPU != 0) return 0;
    if (!(offset->actst & 0x10)) dir = offset->posdirflag;
    kd = PlayerKeyDataGet(offset->padid, 0);
    key = KeyDataLRFlip(kd->key, dir);
    time = cmd->time;
    if (cmd->cmd & 0x1000) {
        time -= kd->time;
        if (time < 0) return 0;
    }
    for (;;) {
        prev = ok;
        ok = 0;
        switch (cmd->cmd & 0xff) {
        case 0:
            ok = 1;
            break;
        case 1:
            if (key == 0x20) ok = 1;
            break;
        case 2:
            if (key == 0) ok = 1;
            break;
        case 3:
            ok = key == 1;
            break;
        case 4:
            ok = key == 0x21;
            break;
        case 5:
            ok = key == 0x11;
            break;
        case 6:
            ok = key == 0x10;
            break;
        case 7:
            ok = key == 0x12;
            break;
        case 8:
            ok = key == 0x22;
            break;
        case 9:
            ok = key == 2;
            break;
        case 11:
            if (key == 0x22) ok = 1;
            if (key == 0x12) ok = 1;
            if (key == 2) ok = 1;
            break;
        case 10:
            if (key == 0x21) ok = 1;
            if (key == 0x11) ok = 1;
            if (key == 1) ok = 1;
            break;
        case 13:
            if (key == 2) ok = 1;
            if (key == 0) ok = 1;
            if (key == 1) ok = 1;
            break;
        case 14:
            if (key == 0x12) ok = 1;
            if (key == 0x10) ok = 1;
            if (key == 0x11) ok = 1;
            break;
        case 12:
            ok = 1;
            if (key == 0x21) ok = 0;
            if (key == 0x11) ok = 0;
            if (key == 1) ok = 0;
            break;
        case 0xff:
            return 1;
        }
        if (cmd->cmd & 0x2000) {
            if (ok != 1) return 0;
            time -= kd->time;
            if (time <= 0) return 1;
            kd = PlayerKeyDataGet(offset->padid, i++);
            key = KeyDataLRFlip(kd->key, dir);
        } else if (!(cmd->cmd & 0x1000)) {
            if (ok == 1) {
                cmd++;
                if ((cmd->cmd & 0xff) == 0xff) return 1;
                time = cmd->time;
            } else {
                time -= kd->time;
                kd = PlayerKeyDataGet(offset->padid, i++);
                key = KeyDataLRFlip(kd->key, dir);
                if (time <= 0) return 0;
            }
        } else {
            if (ok == 0) {
                if (prev == 1) {
                    cmd++;
                    if ((cmd->cmd & 0xff) == 0xff) return 1;
                    time = cmd->time;
                    if (cmd->cmd & 0x1000) {
                        time -= kd->time;
                        if (time < 0) return 0;
                    }
                    continue;
                }
            } else if (prev == 1) {
                time -= kd->time;
                if (time < 0) return 0;
                kd = PlayerKeyDataGet(offset->padid, i++);
                key = KeyDataLRFlip(kd->key, dir);
                continue;
            }
            if (ok == 1) {
                kd = PlayerKeyDataGet(offset->padid, i++);
                key = KeyDataLRFlip(kd->key, dir);
            } else {
                kd = PlayerKeyDataGet(offset->padid, i++);
                key = KeyDataLRFlip(kd->key, dir);
                time -= kd->time;
                if (time < 0) return 0;
            }
        }
    }
}

int GetRenda(CHARACTER_WORK *offset, int btn) {
    int n = 0;
    u_int bits = RendaButtonData[offset->padid][btn];
    int i = 32;

    while (i--) {
        n += bits & 1;
        bits >>= 1;
    }
    return n;
}

int IsRendaPunch(CHARACTER_WORK *offset) {
    int n;

    if (GetCPUKey(offset, 0x16)) return 5;
    if (offset->ply->CPU != 0) return 0;
    n = GetRenda(offset, 0);
    if (n >= 5) return n;
    return 0;
}

int IsRendaKick(CHARACTER_WORK *offset) {
    int n;

    if (GetCPUKey(offset, 0x16)) return 5;
    if (offset->ply->CPU != 0) return 0;
    n = GetRenda(offset, 1);
    if (n >= 5) return n;
    return 0;
}

int IsRendaSlash(CHARACTER_WORK *offset) {
    int n;

    if (GetCPUKey(offset, 0x16)) return 5;
    if (offset->ply->CPU != 0) return 0;
    n = GetRenda(offset, 2);
    if (n >= 5) return n;
    return 0;
}

int IsRendaHighSlash(CHARACTER_WORK *offset) {
    int n;

    if (GetCPUKey(offset, 0x16)) return 5;
    if (offset->ply->CPU != 0) return 0;
    n = GetRenda(offset, 3);
    if (n >= 5) return n;
    return 0;
}

extern COMMAND CKData_Hadou[];

int IsHadou(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 1)) return -1;
    return IsCommand(offset, CKData_Hadou);
}

extern COMMAND CKData_Hadoux2[];

int IsHadoux2(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 4)) return -1;
    return IsCommand(offset, CKData_Hadoux2);
}

extern COMMAND CKData_Syoryu[];

int IsSyoryu(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 2)) return -1;
    return IsCommand(offset, CKData_Syoryu);
}

extern COMMAND CKData_Tatsumaki[];

int IsTatsumaki(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 3)) return -1;
    return IsCommand(offset, CKData_Tatsumaki);
}

extern COMMAND CKData_Tatsumakix2[];

int IsTatsumakix2(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 6)) return -1;
    return IsCommand(offset, CKData_Tatsumakix2);
}

extern COMMAND CKData_GyakuSyoryu[];

int IsGyakuSyoryu(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 7)) return -1;
    return IsCommand(offset, CKData_GyakuSyoryu);
}

extern COMMAND CKData_Yoga[];

int IsYoga(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 8)) return -1;
    return IsCommand(offset, CKData_Yoga);
}

extern COMMAND CKData_Yogax2[];

int IsYogax2(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 10)) return -1;
    return IsCommand(offset, CKData_Yogax2);
}

extern COMMAND CKData_GyakuYoga[];

int IsGyakuYoga(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 9)) return -1;
    return IsCommand(offset, CKData_GyakuYoga);
}

extern COMMAND CKData_GyakuYogax2[];

int IsGyakuYogax2(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 11)) return -1;
    return IsCommand(offset, CKData_GyakuYogax2);
}

extern COMMAND CKData_FireWheel[];

int IsFireWheel(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 12)) return -1;
    return IsCommand(offset, CKData_FireWheel);
}

extern COMMAND CKData_HaouSyoko[];

int IsHaouSyoko(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 13)) return -1;
    return IsCommand(offset, CKData_HaouSyoko);
}

extern COMMAND CKData_GyakuHaouSyoko[];

int IsGyakuHaouSyoko(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 21)) return -1;
    return IsCommand(offset, CKData_GyakuHaouSyoko);
}

extern COMMAND CKData_Geiza[];

int IsGeiza(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 14)) return -1;
    return IsCommand(offset, CKData_Geiza);
}

extern COMMAND CKData_Kaen[];

int IsKaen(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 18)) return -1;
    return IsCommand(offset, CKData_Kaen);
}

extern COMMAND CKData_GRAY[];

int IsGRAY(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 19)) return -1;
    return IsCommand(offset, CKData_GRAY);
}

extern COMMAND CKData_HARD1[];

int IsHard1(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 20)) return -1;
    return IsCommand(offset, CKData_HARD1);
}

extern COMMAND CKData_GRAY2[];

int IsGRAY2(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 23)) return -1;
    return IsCommand(offset, CKData_GRAY2);
}

extern COMMAND CKData_KRK[];

int IsKRK(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 24)) return -1;
    return IsCommand(offset, CKData_KRK);
}

extern COMMAND CKData_SitaSita[];

int IsSitaSita(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 17)) return -1;
    return IsCommand(offset, CKData_SitaSita);
}

extern COMMAND CKData_Sita[];

int IsSita(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 17)) return -1;
    return IsCommand(offset, CKData_Sita);
}

extern COMMAND CKData_LeftTAMERight[];

int IsLeftTAMERight(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 15)) return -1;
    return IsCommand(offset, CKData_LeftTAMERight);
}

extern COMMAND CKData_DownTAMEUp[];

int IsDownTAMEUp(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 16)) return -1;
    return IsCommand(offset, CKData_DownTAMEUp);
}

extern COMMAND CKData_LeftTAMERight45[];

int IsLeftTAMERight45(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 15)) return -1;
    return IsCommand(offset, CKData_LeftTAMERight45);
}

extern COMMAND CKData_LeftRight[];

int IsLeftRight(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 15)) return -1;
    return IsCommand(offset, CKData_LeftRight);
}

extern COMMAND CKData_DownTAMEUp45[];

int IsDownTAMEUp45(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 16)) return -1;
    return IsCommand(offset, CKData_DownTAMEUp45);
}

extern COMMAND CKData_LeftTAMERight60[];

int IsLeftTAMERight60(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 15)) return -1;
    return IsCommand(offset, CKData_LeftTAMERight60);
}

extern COMMAND CKData_DownTAMEUp60[];

int IsDownTAMEUp60(CHARACTER_WORK *offset) {
    if (GetCPUKey(offset, 16)) return -1;
    return IsCommand(offset, CKData_DownTAMEUp60);
}

extern COMMAND CKData_GuardCancel[];

int IsGCancel(CHARACTER_WORK *offset) {
    return IsCommand(offset, CKData_GuardCancel);
}

extern COMMAND CKData_HighJump[];

int IsHighJump(CHARACTER_WORK *offset) {
    return IsCommand(offset, CKData_HighJump);
}

extern COMMAND CKData_StdRight[];

int IsStdRight(CHARACTER_WORK *offset) {
    if (offset->ply->CPU != 0) {
        u_int k = GameKey[offset->padid].on;

        if (k & 0x2000) {
            if (!(k & 0x4000)) return 1;
        }
        return 0;
    }
    return IsCommand(offset, CKData_StdRight);
}

extern COMMAND CKData_StdLeft[];

int IsStdLeft(CHARACTER_WORK *offset) {
    return IsCommand(offset, CKData_StdLeft);
}

/* first matched by Codex */
u_int IsRight(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & 0x2000;
}

/* first matched by Codex */
u_int IsLeft(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & 0x8000;
}

u_int IsDown(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & 0x4000;
}

u_int IsUp(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & 0x1000;
}

extern COMMAND CKData_SitRight[];

int IsSitRight(CHARACTER_WORK *offset) {
    if (offset->ply->CPU != 0) {
        u_int k = GameKey[offset->padid].on;

        if (k & 0x2000) {
            if (k & 0x4000) return 1;
        }
        return 0;
    }
    return IsCommand(offset, CKData_SitRight);
}

u_int IsTrgPunch(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & offset->ply->PunchKey;
}

u_int IsTrgAndOffPunch(CHARACTER_WORK *offset) {
    return offset->ply->PunchKey & (GameKey[offset->padid].trg | GameKey[offset->padid].off);
}

u_int IsHoldPunch(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & offset->ply->PunchKey;
}

u_int IsOffPunch(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].off & offset->ply->PunchKey;
}

u_int IsTrgKick(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & offset->ply->KickKey;
}

u_int IsTrgAndOffKick(CHARACTER_WORK *offset) {
    return offset->ply->KickKey & (GameKey[offset->padid].trg | GameKey[offset->padid].off);
}

u_int IsHoldKick(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & offset->ply->KickKey;
}

u_int IsOffKick(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].off & offset->ply->KickKey;
}

u_int IsTrgSlash(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & offset->ply->SlashKey;
}

u_int IsTrgAndOffSlash(CHARACTER_WORK *offset) {
    return offset->ply->SlashKey & (GameKey[offset->padid].trg | GameKey[offset->padid].off);
}

u_int IsHoldSlash(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & offset->ply->SlashKey;
}

u_int IsOffSlash(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].off & offset->ply->SlashKey;
}

u_int IsTrgHighSlash(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & offset->ply->HighSlashKey;
}

u_int IsTrgAndOffHighSlash(CHARACTER_WORK *offset) {
    return offset->ply->HighSlashKey & (GameKey[offset->padid].trg | GameKey[offset->padid].off);
}

u_int IsHoldHighSlash(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & offset->ply->HighSlashKey;
}

u_int IsOffHighSlash(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].off & offset->ply->HighSlashKey;
}

u_int IsTrgFuckin(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & offset->ply->Fuckin;
}

u_int IsHoldFuckin(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].on & offset->ply->Fuckin;
}

u_int IsTrgAndOffStart(CHARACTER_WORK *offset) {
    return (GameKey[offset->padid].trg | GameKey[offset->padid].off) & 0x800;
}

u_int IsTrgL(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & 4;
}

u_int IsTrgR(CHARACTER_WORK *offset) {
    return GameKey[offset->padid].trg & 8;
}

int IsDouji(CHARACTER_WORK *offset, u_short mask, int count, int time) {
    int n = 0;
    int i;

    if (offset->ply->CPU != 0) {
        if (mask & 1) {
            if (IsTrgPunch(offset)) n++;
        }
        if (mask & 2) {
            if (IsTrgKick(offset)) n++;
        }
        if (mask & 4) {
            if (IsTrgSlash(offset)) n++;
        }
        if (mask & 8) {
            if (IsTrgHighSlash(offset)) n++;
        }
    } else {
        for (i = 0; i < 4; i++, mask >>= 1) {
            int t;

            if (~mask & 1) continue;
            t = CommandButtonData[offset->padid][i];
            if (t > 0 && t <= time + 1) n++;
        }
    }
    return n >= count;
}

int IsUkemi(CHARACTER_WORK *offset) {
    return IsDouji(offset, 0xf, 2, 2);
}

int IsDustAt(CHARACTER_WORK *offset) {
    return IsDouji(offset, 0xc, 2, 2);
}

int IsPG(CHARACTER_WORK *offset) {
    return IsDouji(offset, 3, 2, 10000);
}

int IsROMANCHANSEL(CHARACTER_WORK *offset) {
    return IsDouji(offset, 0xf, 3, 2);
}

int IsITIGEKI(CHARACTER_WORK *offset) {
    return IsDouji(offset, 0xf, 4, 2);
}

int IsThrow(CHARACTER_WORK *offset) {
    int dir;

    if (GameKey[offset->padid].on & 0x1000) return 0;
    if (GameKey[offset->padid].on & 0x4000) return 0;
    dir = 0;
    if ((GameKey[offset->padid].on & 0x2000) || (GameKey[offset->padid].on & 0x8000)) dir = 1;
    if (IsTrgHighSlash(offset) && dir) return 1;
    return 0;
}

typedef struct { u_char b0; u_char b1; } KEYBYTES;

unsigned long IsGuard(CHARACTER_WORK *offset) {
    unsigned long b = ((KEYBYTES *)&GameKey[offset->padid].on)->b1;

    return b >> 7;
}

void clrl_commanddat(int pl) {
    CommandButtonData[pl][0] = 0;
    CommandButtonData[pl][1] = 4;
    CommandButtonData[pl][2] = 8;
    CommandButtonData[pl][3] = 12;
}

void PlayerKeyDataSave(int id) {
    int data;
    int index;
    u_int keymask;
    CHARACTER_WORK *offset;
    u_int swdata;

    offset = &PlayerWork[id];
    swdata = pads[offset->padid].on;
    if (swdata & 0x8000) {
        if (swdata & 0x1000) data = 2;
        else if (swdata & 0x4000) data = 1;
        else data = 0;
    } else if (swdata & 0x2000) {
        if (swdata & 0x1000) data = 0x12;
        else if (swdata & 0x4000) data = 0x11;
        else data = 0x10;
    } else {
        if (swdata & 0x1000) data = 0x22;
        else if (swdata & 0x4000) data = 0x21;
        else data = 0x20;
    }

    index = KeyIndex[id];
    if (CommandKeyData[id][index].key == data) {
        if (CommandKeyData[id][index].time < 0xff) CommandKeyData[id][index].time++;
    } else {
        KeyIndex[id] = (index + 1) & 0x1f;
        CommandKeyData[id][KeyIndex[id]].key = data;
        CommandKeyData[id][KeyIndex[id]].time = 1;
    }

    if (GameKey[id].on & offset->ply->PunchKey) CommandButtonData[id][0]++; else CommandButtonData[id][0] = 0;
    if (GameKey[id].on & offset->ply->KickKey) CommandButtonData[id][1]++; else CommandButtonData[id][1] = 0;
    if (GameKey[id].on & offset->ply->SlashKey) CommandButtonData[id][2]++; else CommandButtonData[id][2] = 0;
    if (GameKey[id].on & offset->ply->HighSlashKey) CommandButtonData[id][3]++; else CommandButtonData[id][3] = 0;

    index = RendaIndex[id] >> 1;
    keymask = 1 << index;
    data = GameKey[id].trg;
    if (data & offset->ply->PunchKey) RendaButtonData[id][0] |= keymask;
    if (data & offset->ply->KickKey) RendaButtonData[id][1] |= keymask;
    if (data & offset->ply->SlashKey) RendaButtonData[id][2] |= keymask;
    if (data & offset->ply->HighSlashKey) RendaButtonData[id][3] |= keymask;
    RendaIndex[id] = (RendaIndex[id] + 1) & 0x3f;
    if (!(RendaIndex[id] & 1)) {
        index = RendaIndex[id] >> 1;
        keymask = ~(1 << index);
        RendaButtonData[id][0] &= keymask;
        RendaButtonData[id][1] &= keymask;
        RendaButtonData[id][2] &= keymask;
        RendaButtonData[id][3] &= keymask;
    }

    if ((offset->posdirflag = DirectionDirectGet(offset)) == 2) offset->posdirflag = offset->dirflag;
}
