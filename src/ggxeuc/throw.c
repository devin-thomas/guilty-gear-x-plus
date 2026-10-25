#include "ggxeuc/objtypes.h"

unsigned int throwenableflag[] = {
    0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF,
    0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF,
};

unsigned int commandthrowenableflag[] = {
    0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0x3FFFF, 0, 0, 0, 0, 0, 0, 0, 0x3FFFF, 0x3FFFF,
    0, 0, 0, 0,
};

short unsigned int ThrowActno[] = {
    0, 0x56, 0x76, 0x63, 0x54, 0x78, 0x54, 0x78, 0x54, 0x85, 0x8E, 0x7E, 0x7C, 0x87, 0x76, 0x75,
    0x73, 0x56, 0x56,
};

short int ThrowRange[] = {
    0, 4300, 4300, 4300, 4000, 4500, 5500, 3600, 4900, 4000, 4500, 4500, 4300, 4300, 4500, 4300,
    4900, 4300, 4300,
};

short unsigned int CommandThrowActno[] = { 0, 0x87, 0xAE, 0x7B, 0x7B, 0x7B, 0x91, 0, 0, 0, 0, 0, 0, 0, 0x60, 0xA3, 0, 0, 0 };

short unsigned int CommandThrowTension[] = { 0, 0x258, 0x258, 0x4B0, 0x258, 0x258, 0x258, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

short int CommandThrowRange[] = { 0, 6500, 6500, 16000, 7000, 7500, 1000, 0, 0, 0, 0, 0, 0, 0, 10000, 7000, 0, 0, 0 };

short unsigned int ThrowDamageNoTb[][19] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    {
        0, 0x57, 0x70, 0x5E, 0x69, 0x5C, 0x5C, 0x79, 0x6B, 0x6B, 0x6F, 0x5F, 0x80, 0x67, 0x6C, 0x76,
        0x63, 0x6D, 0x6D,
    },
    {
        0, 0x8C, 0x7B, 0x77, 0x7A, 0x63, 0x5E, 0x81, 0x74, 0x71, 0x77, 0x61, 0x8D, 0x6D, 0x74, 0x7E,
        0x65, 0x73, 0x73,
    },
    {
        0, 0x8E, 0x7D, 0x5D, 0x7C, 0x61, 0x66, 0x83, 0x76, 0x6A, 0x87, 0x62, 0x90, 0x6F, 0x80, 0x80,
        0x6D, 0x6C, 0x6C,
    },
    {
        0, 0x74, 0x74, 0x62, 0x6D, 0x60, 0x61, 0x7D, 0x6F, 0x6F, 0x73, 0x64, 0x84, 0x6B, 0x70, 0x7A,
        0x68, 0x71, 0x71,
    },
    {
        0, 0xB0, 0x95, 0x98, 0x90, 0x79, 0x99, 0xA8, 0x8D, 0x87, 0xA3, 0x7B, 0xA1, 0x84, 0x94, 0x91,
        0x80, 0x87, 0x87,
    },
    {
        0, 0x61, 0x71, 0x5F, 0x6A, 0x5D, 0x5F, 0x7A, 0x6C, 0x6C, 0x70, 0x65, 0x81, 0x68, 0x6D, 0x77,
        0x66, 0x6E, 0x6E,
    },
    {
        0, 0x8B, 0x7A, 0x79, 0x79, 0x65, 0x64, 0x80, 0x73, 0x73, 0x76, 0x66, 0x8C, 0x73, 0x73, 0x7D,
        0x6B, 0x75, 0x75,
    },
    {
        0, 0x8A, 0x79, 0x78, 0x78, 0x66, 0x63, 0x7F, 0x72, 0x74, 0x75, 0x67, 0x8B, 0x71, 0x72, 0x7C,
        0x6A, 0x74, 0x74,
    },
    {
        0, 0xAE, 0x94, 0x97, 0x8F, 0x77, 0x98, 0xA7, 0x8C, 0x86, 0xA2, 0x7A, 0xA0, 0x83, 0x93, 0x90,
        0x7F, 0x86, 0x86,
    },
    {
        0, 0x96, 0x84, 0x8E, 0x86, 0x6F, 0x8C, 0x9D, 0x82, 0x7D, 0x92, 0x70, 0x97, 0x7B, 0x87, 0x87,
        0x76, 0x7D, 0x7D,
    },
    {
        0, 0xB4, 0x98, 0x9C, 0x93, 0x7C, 0x9C, 0xAB, 0x90, 0x8A, 0xAE, 0x7F, 0xA6, 0x89, 0x97, 0x94,
        0x85, 0x8A, 0x8A,
    },
    {
        0, 0x73, 0x73, 0x61, 0x6C, 0x5F, 0x60, 0x7C, 0x6E, 0x6E, 0x72, 0x69, 0x83, 0x6A, 0x6F, 0x79,
        0x67, 0x70, 0x70,
    },
    {
        0, 0xB5, 0x99, 0x9D, 0x94, 0x7D, 0x9D, 0xAC, 0x91, 0x8B, 0xAF, 0x80, 0xA7, 0x88, 0x98, 0x95,
        0x86, 0x8B, 0x8B,
    },
    {
        0, 0x8D, 0x7C, 0x7A, 0x7B, 0x68, 0x65, 0x82, 0x75, 0x75, 0x78, 0x6A, 0x8E, 0x74, 0x77, 0x7F,
        0x6C, 0x76, 0x76,
    },
    {
        0, 0x72, 0x72, 0x60, 0x6B, 0x5E, 0x62, 0x7B, 0x6D, 0x6D, 0x71, 0x6B, 0x82, 0x69, 0x6E, 0x78,
        0x69, 0x6F, 0x6F,
    },
    {
        0, 0x94, 0x86, 0x90, 0x88, 0x71, 0x8E, 0x9F, 0x84, 0x7F, 0x97, 0x72, 0x99, 0x7D, 0x89, 0x89,
        0x74, 0x7F, 0x7F,
    },
    {
        0, 0xCC, 0xAC, 0xB1, 0xAC, 0xB4, 0xB0, 0xC6, 0xA6, 0xA6, 0xCB, 0x9D, 0xD3, 0xB2, 0xB6, 0xAD,
        0x9A, 0x9A, 0x9A,
    },
    {
        0, 0xCA, 0xAA, 0xAF, 0xAA, 0xB2, 0xAE, 0xC4, 0xA4, 0xA4, 0xC9, 0x9B, 0xD1, 0xB0, 0xB4, 0xAB,
        0x98, 0x98, 0x98,
    },
};

short unsigned int CommandThrowDamageNoTb[][19] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    {
        0, 0x89, 0x75, 0x76, 0x74, 0x62, 0x5D, 0x7E, 0x70, 0x70, 0x74, 0x60, 0x8A, 0x6C, 0x71, 0x7B,
        0x64, 0x72, 0x72,
    },
    {
        0, 0xCD, 0xAD, 0xB2, 0xAD, 0xB5, 0xB1, 0xC7, 0xA7, 0xA7, 0xCC, 0x9E, 0xD4, 0xB3, 0xB7, 0xAE,
        0xA0, 0x9D, 0x9B,
    },
    {
        0, 0x90, 0x7F, 0x8A, 0x7E, 0x69, 0x7E, 0x87, 0x78, 0x77, 0x8A, 0x6C, 0x92, 0x75, 0x82, 0x82,
        0x70, 0x79, 0x79,
    },
    {
        0, 0x8F, 0x7E, 0x7D, 0x7D, 0x64, 0x67, 0x84, 0x77, 0x72, 0x88, 0x63, 0x91, 0x70, 0x81, 0x81,
        0x6E, 0x77, 0x77,
    },
    {
        0, 0x93, 0x82, 0x89, 0x81, 0x67, 0x81, 0x94, 0x7E, 0x76, 0x8D, 0x68, 0x95, 0x72, 0x85, 0x85,
        0x6F, 0x78, 0x78,
    },
    {
        0, 0x95, 0x83, 0x8D, 0x85, 0x6E, 0x8B, 0x93, 0x81, 0x7C, 0x96, 0x6F, 0x96, 0x7A, 0x86, 0x86,
        0x75, 0x7C, 0x7C,
    },
    {
        0, 0x91, 0x80, 0x8B, 0x7F, 0x6A, 0x7F, 0x88, 0x79, 0x78, 0x8B, 0x6D, 0x93, 0x76, 0x83, 0x83,
        0x71, 0x7A, 0x7A,
    },
    {
        0, 0x92, 0x81, 0x8C, 0x80, 0x6B, 0x80, 0x89, 0x7A, 0x79, 0x8C, 0x6E, 0x94, 0x77, 0x84, 0x84,
        0x72, 0x7B, 0x7B,
    },
    {
        0, 0xAA, 0x90, 0x93, 0x8B, 0x73, 0x89, 0xA3, 0x88, 0x81, 0x9D, 0x76, 0x9C, 0x7F, 0x8F, 0x8C,
        0x7B, 0x82, 0x82,
    },
    {
        0, 0xAB, 0x91, 0x94, 0x8C, 0x74, 0x8A, 0xA4, 0x89, 0x82, 0x9E, 0x77, 0x9D, 0x80, 0x90, 0x8D,
        0x7C, 0x83, 0x83,
    },
    {
        0, 0x97, 0x85, 0x8F, 0x87, 0x70, 0x8D, 0x9E, 0x83, 0x7E, 0x93, 0x71, 0x98, 0x7C, 0x88, 0x88,
        0x77, 0x7E, 0x7E,
    },
    {
        0, 0xAC, 0x92, 0x95, 0x8D, 0x75, 0x90, 0xA5, 0x8A, 0x83, 0x9F, 0x78, 0x9E, 0x81, 0x91, 0x8E,
        0x7D, 0x84, 0x84,
    },
    {
        0, 0xAD, 0x93, 0x96, 0x8E, 0x76, 0x91, 0xA6, 0x8B, 0x84, 0xA0, 0x79, 0x9F, 0x82, 0x92, 0x8F,
        0x7E, 0x85, 0x85,
    },
    {
        0, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
        0x2A, 0x2A, 0x2A,
    },
    {
        0, 0xBD, 0x9E, 0xA5, 0x9C, 0x9E, 0xA4, 0xBB, 0x97, 0x99, 0xB5, 0x86, 0xAD, 0xA2, 0x9D, 0x9E,
        0x8B, 0x90, 0x90,
    },
    {
        0, 0xBE, 0x9F, 0xA6, 0x9D, 0x9F, 0xA5, 0xBC, 0x98, 0x9A, 0xB6, 0x87, 0xAE, 0xA3, 0x9E, 0x9F,
        0x8C, 0x91, 0x91,
    },
    {
        0, 0xC5, 0xA4, 0xAA, 0xA4, 0xAA, 0xA9, 0xBF, 0x9B, 0x9D, 0xB9, 0x8C, 0xB1, 0xA5, 0xA4, 0xA3,
        0x8F, 0x94, 0x94,
    },
};

typedef struct {
    int flag;
    u_short hitno;
    u_char damage;
} THROW_HIT;

typedef struct {
    int flag;
    int x;
    int y;
    int pad[5];
} CREATE_POS;

extern int ThrowDist;
extern int throwflag;
extern u_int ThrowRes[2];
extern CHARACTER_WORK *PlayerWork;
extern u_short CommandThrowDamageNoTb[][19];
void EndtoStand(CHARACTER_WORK *offset);

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
int IsThrow(CHARACTER_WORK *offset);
u_int IsLeft(CHARACTER_WORK *offset);
void Tension_Add(CHARACTER_WORK *offset, int val);
void FaintClear(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, u_int no);
void SetActNoNext(CHARACTER_WORK *offset, u_int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
int IsActEnd(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int gravity);
int SetChangeFrameAnime(CHARACTER_WORK *offset, u_short no);
int PlayerDamage(CHARACTER_WORK *offset, int damage, void *arg);
void RequestHit(CHARACTER_WORK *offset, int flag, int hitno, CHARACTER_WORK *enemy);
void OrScreenFlag(u_int flag);
void SetThrowDamageStatusWithOutScreen(CHARACTER_WORK *offset);
void SetThrowDamageStatus(CHARACTER_WORK *offset);

void EnemyThrowResDisp(CHARACTER_WORK *offset) {
}

extern int ThrowDist;

void SetThrowDist(int dist) {
    ThrowDist = dist;
}

int GetThrowDist(void) {
    return ThrowDist;
}

int EnemyThrowCheck(CHARACTER_WORK *offset) {
    if (offset->actst & 0x808330) {
        return 1;
    }
    return offset->ply->notThrowTime != 0;
}

extern u_int commandthrowenableflag[];

int GetCommandThrowEnableFlag(int chara, int no) {
    return (commandthrowenableflag[chara - 1] >> (no - 1)) & 1;
}

extern short ThrowRange[];
int EnemyThrowCheck(CHARACTER_WORK *offset);
int IsThrow(CHARACTER_WORK *offset);

int PlayerThrowFlagSet(void) {
    int i;
    CHARACTER_WORK *offset;
    CHARACTER_WORK *enemy;

    throwflag = 0;
    for (i = 0; i < 2; i++) {
        offset = &PlayerWork[i];
        enemy = i ? &PlayerWork[0] : &PlayerWork[1];
        if (offset->DamColliFlag == 0 && enemy->DamColliFlag == 0 && !EnemyThrowCheck(enemy)) {
            throwflag |= 1 << (i + 2);
            if (IsThrow(offset)) {
                if (ThrowRange[offset->idno] < ThrowDist) {
                    ThrowRes[offset->padid] |= 0x20;
                } else {
                    throwflag |= 1 << i;
                }
            }
        }
    }
    return throwflag == 2;
}

int SL_CommandThrowCheck(CHARACTER_WORK *offset);
int KY_CommandThrowCheck(CHARACTER_WORK *offset);
int MY_CommandThrowCheck(CHARACTER_WORK *offset);
int PO_CommandThrowCheck(CHARACTER_WORK *offset);
int CH_CommandThrowCheck(CHARACTER_WORK *offset);
int ZT_CommandThrowCheck(CHARACTER_WORK *offset);
int JM_CommandThrowCheck(CHARACTER_WORK *offset);

int IsCommandThrow(CHARACTER_WORK *offset) {
    if (offset->ply->ArtsSeal) {
        return 0;
    }
    switch (offset->idno) {
    case 1:
        return SL_CommandThrowCheck(offset);
    case 2:
        return KY_CommandThrowCheck(offset);
    case 3:
        return MY_CommandThrowCheck(offset);
    case 6:
        return PO_CommandThrowCheck(offset);
    case 8:
        return ZT_CommandThrowCheck(offset);
    case 7:
        return CH_CommandThrowCheck(offset);
    case 12:
        return JM_CommandThrowCheck(offset);
    }
    return 0;
}

extern short CommandThrowRange[];
extern int CommandThrowid[2];
int IsCommandThrow(CHARACTER_WORK *offset);
int GetCommandThrowEnableFlag(int chara, int no);

int PlayerCommandThrowFlagSet(void) {
    int i;
    CHARACTER_WORK *offset;
    CHARACTER_WORK *enemy;

    for (i = 0; i < 2; i++) {
        offset = &PlayerWork[i];
        enemy = i ? &PlayerWork[0] : &PlayerWork[1];
        if ((~throwflag >> (i + 2)) & 1) continue;
        if ((CommandThrowid[i] = IsCommandThrow(offset)) == 0) continue;
        if (!GetCommandThrowEnableFlag(CommandThrowid[i], enemy->idno)) continue;
        if (CommandThrowRange[CommandThrowid[i]] < ThrowDist) {
            ThrowRes[offset->padid] |= 0x40;
            continue;
        }
        throwflag |= 1 << (i + 4);
        throwflag &= 0xfffffffc;
    }
    if (throwflag & 3) {
        return -1;
    }
    return throwflag == 0x20;
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void SearchCharacterExecute(CHARACTER_WORK *offset, int a, int b, int c);
void PlayerPoisonClear(CHARACTER_WORK *offset);

void PlayerThrowSetSub(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;

    enemy = GetEnemyPlayer(offset);
    SearchCharacterExecute(enemy, 0x4d, 0, 0x16);
    SearchCharacterExecute(enemy, 0x4d, 0, 0x13);
    PlayerPoisonClear(offset);
    if (enemy->idno == 7) {
        enemy->ply->PersonalWork[1].w[0] = 0;
    }
    offset->ply->inertia = 0;
    enemy->ply->inertia = 0;
    offset->actst |= 0x4000;
    enemy->actst |= 0x4020;
    offset->ply->JumpFlag &= ~2;
    enemy->ply->JumpFlag &= ~2;
    enemy->ply->GuardPoint -= 6;
    enemy->DamParam->FaintPoint = 8;
    enemy->DamParam->DamageHosei = -1;
    enemy->ply->HitCount = 0;
    enemy->ply->MutekiTime = 0;
    offset->GuardSt = 0;
    enemy->GuardSt = 0;
}

extern u_short ThrowActno[];
extern u_short ThrowDamageNoTb[][19];
void PlayerThrowSetSub(CHARACTER_WORK *offset);

int PlayerThrowCheck(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    int pad;

    enemy = GetEnemyPlayer(offset);
    if (offset->actst & 0x10) {
        return 0;
    }
    if ((throwflag & 3) == 3) {
        return 0;
    }
    pad = offset->padid;
    if (offset->CommandFlag & 0x2000) {
        return 0;
    }
    if (!((throwflag >> pad) & 1)) {
        if (IsThrow(offset)) {
            EnemyThrowResDisp(offset);
        }
        return 0;
    }
    if (IsLeft(offset)) {
        offset->dirflag = !offset->dirflag;
    }
    if (enemy->actno == 0x46) {
        FaintClear(enemy);
    }
    SetActNo(offset, ThrowActno[offset->idno]);
    SetActNo(enemy, ThrowDamageNoTb[offset->idno][enemy->idno]);
    Tension_Add(offset, 400);
    PlayerThrowSetSub(offset);
    return 1;
}

extern int CommandThrowid[2];
extern u_short CommandThrowActno[];
extern u_short CommandThrowDamageNoTb[][19];
extern u_short CommandThrowTension[];
void DirectionDirectChange(CHARACTER_WORK *offset);
void PlayerThrowSetSub(CHARACTER_WORK *offset);

void PlayerCommandThrowSet(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    int pad;

    enemy = GetEnemyPlayer(offset);
    pad = offset->padid;
    DirectionDirectChange(offset);
    if (enemy->actno == 0x46) {
        FaintClear(enemy);
    }
    SetActNo(offset, CommandThrowActno[CommandThrowid[pad]]);
    SetActNo(enemy, CommandThrowDamageNoTb[CommandThrowid[pad]][enemy->idno]);
    Tension_Add(offset, CommandThrowTension[CommandThrowid[pad]]);
    PlayerThrowSetSub(offset);
}

int IsCommandThrow(CHARACTER_WORK *offset);
void PlayerCommandThrowSet(CHARACTER_WORK *offset);

int PlayerCommandThrowCheck(CHARACTER_WORK *offset) {
    if (offset->actst & 0x10) {
        return 0;
    }
    if ((throwflag & 0x30) == 0x30) {
        return 0;
    }
    if (!((throwflag >> (offset->padid + 4)) & 1)) {
        if (IsCommandThrow(offset)) {
            EnemyThrowResDisp(offset);
        }
        return 0;
    }
    PlayerCommandThrowSet(offset);
    return 1;
}

void PlayerCommandThrowSet(CHARACTER_WORK *offset);

int CH_HW4_ThrowCheck(CHARACTER_WORK *offset) {
    if ((throwflag >> (offset->padid + 4)) & 1) {
        PlayerCommandThrowSet(offset);
        return 1;
    }
    return 0;
}

void PlayerCommandThrowSet(CHARACTER_WORK *offset);

int JM_SHW1_ThrowCheck(CHARACTER_WORK *offset) {
    if ((throwflag >> (offset->padid + 4)) & 1) {
        PlayerCommandThrowSet(offset);
        return 1;
    }
    return 0;
}

extern int BattleStatus;
void PlayerCommandThrowSet(CHARACTER_WORK *offset);
void SetBattleTimeStopFlag(int padid);

int MY_ItigekiThrowCheck(CHARACTER_WORK *offset) {
    if (!((throwflag >> (offset->padid + 4)) & 1) || BattleStatus != 2) {
        return 0;
    }
    PlayerCommandThrowSet(offset);
    SetBattleTimeStopFlag(offset->padid);
    return 1;
}

void SetThrowDamageStatusWithOutScreen(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x4060;
    offset->ply->JumpFlag &= ~4;
}

void SetThrowDamageStatus(CHARACTER_WORK *offset) {
    OrScreenFlag(0x80);
    SetThrowDamageStatusWithOutScreen(offset);
}

void CmnAct_ThrowDamageByKR(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 0x24;
        offset->actst |= 0x02000000;
        offset->ply->DownFlag |= 2;
        SetYSpeed(offset, 1500, 110);
        SetXSpeed(offset, -100);
        offset->ply->JumpFlag |= 2;
        offset->actst |= 0x20;
        SetActNoNext(offset, 0x3004e);
    }
}

void CmnAct_ThrowDamageByJS(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnimeSameActno(offset);
    OrScreenFlag(0x84);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 0x24;
        offset->actst |= 0x02000000;
        offset->ply->DownFlag |= 2;
        SetYSpeed(offset, 2000, 110);
        SetXSpeed(offset, -100);
        offset->ply->JumpFlag |= 2;
        offset->actst |= 0x20;
        SetActNoNext(offset, 0x3004c);
    }
}

void CmnAct_JSVADamege(CHARACTER_WORK *offset) {
    u_short anime[19] = {
        0x00, 0xcb, 0xab, 0xb0, 0xab, 0xb3, 0xaf, 0xc5, 0xa5, 0xa5,
        0xca, 0x9c, 0xd2, 0xb1, 0xb5, 0xac, 0x99, 0x99, 0x99,
    };

    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, anime[offset->idno]);
    if (IsActEnd(offset)) {
        PlayerDamage(offset, 60, 0);
        offset->ply->DownTimer = 60;
        offset->ply->DownFlag |= 2;
        SetXSpeed(offset, -400);
        SetYSpeed(offset, 400, 70);
        SetActNoNext(offset, 0x30033);
    }
}

void CmnAct_ThrowDamageBySL(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->InstSt & 0x40) {
        offset->ply->DownTimer = 0x1c;
        SetYSpeed(offset, -1300, 110);
        SetXSpeed(offset, -800);
        offset->ply->JumpFlag |= 2;
        offset->actst |= 0x20;
        SetActNoNext(offset, 0x30031);
    }
}

void CmnAct_ThrowDamageBySLEX(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 0x24;
        offset->actst |= 0x02000000;
        offset->ply->DownFlag |= 2;
        SetYSpeed(offset, 2000, 110);
        SetXSpeed(offset, -100);
        offset->ply->JumpFlag |= 2;
        offset->actst |= 0x20;
        SetActNoNext(offset, 0x30033);
    }
}

void CmnAct_ThrowDamageByKY(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        SetActNoNext(offset, 0x30000);
    }
}

void CmnAct_ThrowDamageByKYEX(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        SetActNoNext(offset, 0x30000);
    }
}

void CmnAct_ThrowDamageByPO(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (offset->Mark) {
        offset->ply->DownTimer = 0x20;
        SetYSpeed(offset, -1600, 110);
        SetXSpeed(offset, 1200);
        offset->Mark = 0;
        offset->actst |= 0x20;
        SetActNoNext(offset, 0x30043);
    }
}

void CmnAct_ThrowDamageByJM(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -2000, 100);
        SetXSpeed(offset, 300);
        SetActNoNext(offset, 0x30044);
    }
}

void CmnAct_ThrowDamageByML(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -1300, 110);
        SetXSpeed(offset, 100);
        SetActNoNext(offset, 0x30031);
    }
}

void RequestHit(CHARACTER_WORK *offset, int flag, int hitno, CHARACTER_WORK *enemy);
void SetHitParam(CHARACTER_WORK *offset, int type, THROW_HIT *hit);

int ThrowDamageInterrupt(CHARACTER_WORK *offset, THROW_HIT *hit, CHARACTER_WORK *enemy) {
    PlayerDamage(offset, hit->damage, hit);
    RequestHit(offset, hit->flag, hit->hitno, enemy);
    SetHitParam(offset, 3, hit);
    offset->Localid = 100;
    return 1;
}

int ThrowDamageInterrupt(CHARACTER_WORK *offset, THROW_HIT *hit, CHARACTER_WORK *enemy);
void SetJumpControl(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreate(CHARACTER_WORK *offset, int no);
void SetParentPosX(CHARACTER_WORK *offset, int x);
void SetParentPosY(CHARACTER_WORK *offset, int y);

void CmnAct_ThrowDamageByVE(CHARACTER_WORK *offset) {
    CHARACTER_WORK *obj;
    u_int st;

    SetThrowDamageStatus(offset);
    offset->actst |= 0x02000000;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Localid = 0;
    }
    if (offset->HitStopTime != 0) {
        return;
    }
    if (offset->Localid == 100) {
        offset->yspeed = 0;
        offset->gravity = 0;
        offset->actst |= 0x20;
        SetXSpeed(offset, -2500);
        SetJumpControl(offset);
        st = offset->actst;
        if (st & 0x2800) {
            offset->actst = st | 8;
            offset->yspeed = -1500;
            offset->ply->DownTimer = 30;
            offset->gravity = 110;
            SetXSpeed(offset, 1000);
            SetActNoNext(offset, 0x30031);
            return;
        }
    } else if (offset->Localid > 30) {
        offset->ply->DownTimer = 16;
        offset->actst |= 8;
        offset->yspeed = -1500;
        offset->gravity = 110;
        SetXSpeed(offset, -500);
        SetActNoNext(offset, 0x30031);
        return;
    } else if (offset->Localid != 0) {
        PlayerDamage(offset, 2, NULL);
        offset->ply->DamageInterrupt = ThrowDamageInterrupt;
        offset->yspeed = -50;
        offset->gravity = 0;
        offset->actst |= 0x20;
        SetJumpControl(offset);
        offset->Localid++;
    } else if (offset->Mark) {
        obj = ObjectCreate(offset, 0x36);
        offset->ply->Child = obj;
        obj->parentFlag = 0x80;
        SetParentPosX(obj, 0);
        SetParentPosY(obj, 0);
        offset->Mark = 0;
    }
}

int VEItigekiDamageInterrupt(CHARACTER_WORK *offset, THROW_HIT *hit, CHARACTER_WORK *enemy) {
    if (PlayerDamage(offset, hit->damage, hit)) {
        return 0;
    }
    RequestHit(offset, hit->flag, hit->hitno, enemy);
    return 1;
}

typedef struct {
    int x;
    int y;
} BALL_POS;

CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *offset, int id, int actno);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *offset, int id, int actno);
int GetXMove(CHARACTER_WORK *offset, int x);

void SetVEItigekiBallTarget(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    CHARACTER_WORK *obj;
    int i;
    BALL_POS *p;
    int no;
    BALL_POS pos[8] = {
        { 6400, 0 }, { 3200, -1600 }, { 3200, 1600 }, { 0, -3200 },
        { 0, 3200 }, { -3200, -1600 }, { -3200, 1600 }, { -6400, 0 },
    };

    p = pos;
    no = 11;
    for (i = 8; i-- != 0; p++, no++) {
        obj = SearchObjectWorkActno(enemy, 0x32, no);
        if (obj == NULL) {
            obj = SearchEffectWorkActno(enemy, 0x32, no);
        }
        if (obj != NULL) {
            obj->HitBackx = offset->posx + GetXMove(offset, p->x);
            obj->RHitBackx = offset->posy + p->y;
        }
    }
}

int VEItigekiDamageInterrupt(CHARACTER_WORK *offset, THROW_HIT *hit, CHARACTER_WORK *enemy);
void SetVEItigekiBallTarget(CHARACTER_WORK *offset);
float G_GetDist(float x, float y);
int g_roundmin(int val, int min);
int g_roundmax(int val, int max);
CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *offset, int id, CREATE_POS *pos, int actno);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *offset, int id, CREATE_POS *pos, int actno);

void CmnAct_ITIGEKIDamageByVE(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    CREATE_POS pos;
    float dy;
    float dx;
    float dist;
    float spd;
    int v;

    enemy = GetEnemyPlayer(offset);
    offset->ply->DamageInterrupt = VEItigekiDamageInterrupt;
    offset->CommandFlag = 0xc05f;
    offset->actst = (offset->actst & ~0x2000) | 0x40;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Localid = 0;
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
    }
    if (enemy->actno == 0x6b) {
        dx = enemy->ply->PersonalWork[1].w[0] * 100 - offset->posx;
        dy = enemy->ply->PersonalWork[1].w[1] * 100 - offset->posy;
        dist = G_GetDist(dx, dy);
        spd = G_GetDist(offset->xspeed, offset->yspeed);
        if (dist < 300.0f) {
            SetXSpeed(offset, 0);
            SetYSpeed(offset, 0, 0);
            if (offset->Localid == 0) {
                pos.flag = 1;
                pos.x = offset->posx;
                pos.y = offset->posy;
                ObjectCreate_ActNo(enemy, 0x32, &pos, 11);
                EffectCreate_ActNo(enemy, 0x32, &pos, 12);
                EffectCreate_ActNo(enemy, 0x32, &pos, 13);
                EffectCreate_ActNo(enemy, 0x32, &pos, 14);
                EffectCreate_ActNo(enemy, 0x32, &pos, 15);
                EffectCreate_ActNo(enemy, 0x32, &pos, 16);
                EffectCreate_ActNo(enemy, 0x32, &pos, 17);
                EffectCreate_ActNo(enemy, 0x32, &pos, 18);
                offset->Localid = 1;
            }
        } else {
            if (dist < 2000.0f) {
                v = g_roundmin(spd * 0.9 - 20.0, 100);
            } else {
                v = g_roundmax(spd + 80.0f, 600);
            }
            spd = v;
            offset->xspeed = spd * (dx / dist);
            offset->yspeed = spd * (dy / dist);
        }
    } else {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, 0, 0);
    }
    SetVEItigekiBallTarget(offset);
    if (enemy->Mark == 1) {
        offset->actst |= 0x400000;
    } else {
        offset->actst &= ~0x400000;
    }
}

void CmnAct_ThrowDamageByMY(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnimeSameActno(offset);
    if (IsActEnd(offset)) {
        offset->yspeed = -2400;
        offset->ply->DownTimer = 60;
        offset->gravity = 110;
        SetXSpeed(offset, 800);
        SetActNoNext(offset, 0x30043);
    }
}

void CmnAct_ThrowDamageByMYEX(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->yspeed = -2500;
        offset->ply->DownTimer = 60;
        offset->gravity = 100;
        SetXSpeed(offset, 700);
        SetActNoNext(offset, 0x30044);
    }
}

extern u_short CommandThrowDamageNoTb[][19];

void SetMYITThrowDamageActNoJump(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[16][offset->idno]);
}

void CmnAct_ThrowDamageByMYIT(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, offset->actno);
}

void CmnAct_ThrowDamageByMYITJump(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->yspeed = 1000;
        offset->ply->DownTimer = 60;
        offset->gravity = 100;
        offset->ply->DownFlag |= 0x210;
        SetXSpeed(offset, 0);
        SetActNoNext(offset, 0x30049);
    }
}

void CmnAct_ThrowDamageByZT(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -1800, 110);
        SetXSpeed(offset, -800);
        SetActNoNext(offset, 0x30031);
    }
}

int IsActTimeOver(CHARACTER_WORK *offset, int time);

void CmnAct_ThrowDamageByZTEX(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActTimeOver(offset, 24)) {
        offset->actst &= ~8;
    }
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetXSpeed(offset, -800);
        SetYSpeed(offset, -1800, -1);
        SetActNoNext(offset, 0x30031);
    }
}

void CmnAct_ThrowDamageByCH(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    offset->ply->pactst |= 0x1000;
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = -1;
        SetXSpeed(offset, 0);
        offset->actst |= 0x100;
        SetActNoNext(offset, 0x30032);
    }
}

void CmnAct_ThrowDamageByJY(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 40;
        SetYSpeed(offset, -2500, 110);
        SetXSpeed(offset, -300);
        SetActNoNext(offset, 0x30044);
    }
}

void SetFAThrowNAMEDamageActNo(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[11][offset->idno]);
}

void CmnAct_ThrowDamageByFA(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    EndtoStand(offset);
}

void CmnAct_ThrowDamageByFA_NAME(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    EndtoStand(offset);
}

void CmnAct_ThrowDamageByDZ(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -1800, 110);
        SetXSpeed(offset, -500);
        SetActNoNext(offset, 0x30044);
    }
}

void SetPOEXThrowDamageActNoJump(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[7][offset->idno]);
}

void SetPOEXThrowDamageActNoLand(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[8][offset->idno]);
}

void CmnAct_ThrowDamageByPOEX(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
}

void CmnAct_ThrowDamageByPOEXJump(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
}

void CmnAct_ThrowDamageByPOEXLand(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetXSpeed(offset, 700);
        SetYSpeed(offset, -800, 55);
        SetActNoNext(offset, 0x30031);
    }
}

void SetPOEX2ThrowDamageActNo(CHARACTER_WORK *offset) {
    SetActNo(offset, CommandThrowDamageNoTb[9][offset->idno]);
}

void SetPOEX2ThrowDamageActNoCeilingLand(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[10][offset->idno]);
}

void SetPOEX2ThrowDamageActNoDrop(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[12][offset->idno]);
}

void SetPOEX2ThrowDamageActNoLand(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[13][offset->idno]);
}

void CmnAct_ThrowDamageByPOEX2Jump(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
}

void CmnAct_ThrowDamageByPOEX2CeilingLand(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
}

void CmnAct_ThrowDamageByPOEX2Drop(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
}

void CmnAct_ThrowDamageByPOEX2Land(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, CommandThrowDamageNoTb[8][offset->idno]);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetXSpeed(offset, 700);
        SetYSpeed(offset, -800, 55);
        SetActNoNext(offset, 0x30031);
    }
}

void CmnAct_ThrowDamageByCHEX(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->yspeed = -1800;
        offset->ply->DownTimer = 60;
        offset->gravity = 110;
        SetXSpeed(offset, 0);
        SetActNoNext(offset, 0x30034);
    }
}

void TimetoAction(CHARACTER_WORK *offset, int no, int time);

void CmnAct_SHWDamageByFA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, offset->actno);
    offset->actst = (offset->actst & ~0x2010) | 0x4060;
    TimetoAction(offset, 0, 600);
}

void CmnAct_SHWDamageByFA2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);

    offset->CommandFlag = 0xc05f;
    offset->actst = (offset->actst & ~0x2000) | 0x4060;
    SetChangeFrameAnime(offset, offset->actno);
    if (enemy->Mark == 1) {
        offset->actst |= 0x400000;
    } else {
        offset->actst &= ~0x400000;
    }
    if (IsActEnd(offset)) {
        SetXSpeed(offset, 0);
        SetYSpeed(offset, -1400, 110);
        offset->ply->DownTimer = 60;
        SetActNoNext(offset, 0x30031);
    }
}

void CmnAct_DieByFA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, offset->actno);
    offset->yspeed = 0;
    offset->actst = (offset->actst & ~0x2000) | 0x400040;
}

extern u_short FADieNoTb[];
void EndtoAction(CHARACTER_WORK *offset, u_int no);

void CmnAct_DmpByFA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnime(offset, offset->actno);
    offset->actst = (offset->actst & ~0x2000) | 0x4040;
    offset->yspeed = 0;
    EndtoAction(offset, FADieNoTb[offset->idno]);
}

void CmnAct_ThrowDamageByBK(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = -1;
        SetYSpeed(offset, -1350, 100);
        SetXSpeed(offset, -1500);
        SetActNoNext(offset, 0x30031);
    }
}

void CmnAct_ThrowDamageByAX(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 0x24;
        offset->ply->DownFlag |= 2;
        SetYSpeed(offset, 1900, 110);
        SetXSpeed(offset, -300);
        offset->ply->JumpFlag |= 2;
        offset->actst |= 0x20;
        SetActNoNext(offset, 0x30033);
    }
}

void CmnAct_ThrowDamageByTS(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -1000, 70);
        SetXSpeed(offset, 700);
        SetActNoNext(offset, 0x30044);
    }
}

void CmnAct_ThrowDamageByAN(CHARACTER_WORK *offset) {
    SetThrowDamageStatus(offset);
    SetChangeFrameAnime(offset, offset->actno);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -1800, 110);
        SetXSpeed(offset, -500);
        SetActNoNext(offset, 0x30044);
    }
}

void CmnAct_ItigekiDamegeByBK(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x20;
    SetChangeFrameAnimeSameActno(offset);
    if (IsActEnd(offset)) {
        offset->ply->DownTimer = 60;
        SetYSpeed(offset, -1500, 110);
        SetXSpeed(offset, -100);
        SetActNoNext(offset, 0x30031);
    }
}

void CmnAct_AXTenhousekiDamege(CHARACTER_WORK *offset) {
    u_short anime[19] = {
        0x00, 0xb7, 0x9b, 0x9f, 0x96, 0x9a, 0x9f, 0xae, 0x93, 0x91,
        0xb1, 0x82, 0xaa, 0x8b, 0x9a, 0x9a, 0x88, 0x8d, 0x8d,
    };

    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, anime[offset->idno]);
    if (IsActEnd(offset)) {
        PlayerDamage(offset, 60, 0);
        offset->ply->DownTimer = 60;
        SetXSpeed(offset, -900);
        SetYSpeed(offset, -1000, 70);
        SetActNoNext(offset, 0x30031);
    }
}

CHARACTER_WORK *EffectCreate(CHARACTER_WORK *offset, int id, CREATE_POS *pos, int no);

void CmnAct_DieByJY(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy;
    CREATE_POS pos;

    offset->posx = 0;
    offset->posy = 0;
    enemy = offset->ply->Enemy;
    offset->dirflag = 0;
    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x4020;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        enemy->attackst |= 0x80000;
        pos.flag = 1;
        pos.x = offset->posx;
        pos.y = offset->posy;
        EffectCreate(enemy, 0x47, &pos, 1);
    }
}

void CmnAct_CutByJY(CHARACTER_WORK *offset) {
    THROW_HIT hit;

    offset->CommandFlag = 0xc05f;
    offset->ply->pactst |= 0x800;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        hit.flag = 0x6000000;
        PlayerDamage(offset, 1, &hit);
    }
}

void EndtoAction(CHARACTER_WORK *offset, u_int no);

void CmnAct_DmgByANHW3(CHARACTER_WORK *offset) {
    SetThrowDamageStatusWithOutScreen(offset);
    SetChangeFrameAnime(offset, offset->actno);
    EndtoAction(offset, 8);
}

void SetDZCADamageActNo(CHARACTER_WORK *offset) {
    SetActNoNext(offset, CommandThrowDamageNoTb[17][offset->idno]);
}

void SetJumpControl(CHARACTER_WORK *offset);

void CmnAct_Casket(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
}

void CmnAct_DieByTS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x20;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->idno == 5 || offset->idno == 8 || offset->idno == 12 || offset->idno == 13 || offset->idno == 16) {
            if (offset->dirflag) {
                offset->dirflag = 0;
            } else {
                offset->dirflag = 1;
            }
        }
    }
    offset->xspeed = 0;
    offset->yspeed = 0;
    offset->actst = (offset->actst & ~0x2000) | 0x400040;
}

void CmnAct_Die2ByTS(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xc05f;
    offset->actst |= 0x20;
    if (SetChangeFrameAnime(offset, offset->actno)) {
        if (offset->idno == 5 || offset->idno == 8 || offset->idno == 12 || offset->idno == 13 || offset->idno == 16) {
            if (offset->dirflag) {
                offset->dirflag = 0;
            } else {
                offset->dirflag = 1;
            }
        }
    }
    offset->yspeed = 0;
    offset->actst = (offset->actst & ~0x2000) | 0x40;
    offset->xspeed = 0;
    if (offset->InstSt & 0xc8) {
        offset->actst |= 0x10000;
    }
}
