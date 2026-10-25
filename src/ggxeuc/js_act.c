#include "generated/js_act.h"

void SetBlendingMode(short int mode);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);

void JSTokorotenDraw(int x, int y, int speed, int size, unsigned int color, unsigned int color2, int vec) {
    NLVERTEX00 vertexu[4];
    NLVERTEX00 *vp;

    SetBlendingMode(1);
    switch (vec) {
    case 0:
        vp = vertexu;
        vp->x = x - size / 2;
        vp->y = y - size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[1];
        vp->x = x - size / 2;
        vp->y = y + size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[2];
        vp->x = x;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color;
        vp = &vertexu[3];
        vp->x = x + size / 2;
        vp->y = y + size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        nlDispStripTrnsl(4, vertexu);
        vp = vertexu;
        vp->x = x - size / 2;
        vp->y = y - size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[1];
        vp->x = x + size / 2 + speed * 2;
        vp->y = y - size / 2 - speed * 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[2];
        vp->x = x;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color;
        vp = &vertexu[3];
        vp->x = x + size / 2;
        vp->y = y + size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        nlDispStripTrnsl(4, vertexu);
        break;
    case 1:
        vp = vertexu;
        vp->x = x + size / 2;
        vp->y = y - size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[1];
        vp->x = x + size / 2;
        vp->y = y + size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[2];
        vp->x = x;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color;
        vp = &vertexu[3];
        vp->x = x - size / 2;
        vp->y = y + size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        nlDispStripTrnsl(4, vertexu);
        vp = vertexu;
        vp->x = x + size / 2;
        vp->y = y - size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[1];
        vp->x = x - size / 2 - speed * 2;
        vp->y = y - size / 2 - speed * 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[2];
        vp->x = x;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color;
        vp = &vertexu[3];
        vp->x = x - size / 2;
        vp->y = y + size / 2;
        vp->z = 300.0f;
        vp->col = color2;
        nlDispStripTrnsl(4, vertexu);
        break;
    case 2:
        vp = vertexu;
        vp->x = x - size * 1.4142135f * 0.5f;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[1];
        vp->x = x;
        vp->y = y - size * 1.4142135f * 0.5f;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[2];
        vp->x = x;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color;
        vp = &vertexu[3];
        vp->x = x + size * 1.4142135f * 0.5f;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color2;
        nlDispStripTrnsl(4, vertexu);
        vp = vertexu;
        vp->x = x - size * 1.4142135f * 0.5f;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[1];
        vp->x = x;
        vp->y = y + size * 1.4142135f * 0.5f + speed * 2;
        vp->z = 300.0f;
        vp->col = color2;
        vp = &vertexu[2];
        vp->x = x;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color;
        vp = &vertexu[3];
        vp->x = x + size * 1.4142135f * 0.5f;
        vp->y = y;
        vp->z = 300.0f;
        vp->col = color2;
        nlDispStripTrnsl(4, vertexu);
        break;
    }
    SetBlendingMode(0);
}

void JSTokorotenDraw(int x, int y, int speed, int size, unsigned int color, unsigned int color2, int vec);
int rand(void);
extern unsigned int Pause;
extern int JSTokorotenFlag;
extern int JSTokorotenVec;
extern int JSTokorotenSpeed;

void JSTokorotenExecute(int flag) {
    static TOKOROTEN JSTokoroten[128];
    int i;
    int check;

    if (!(Pause >= 1 && Pause <= 2) && !(Pause >= 0x80 && Pause < 0x90)) {
        switch (flag) {
        case 1:
            for (i = 0; i < 128; i++) {
                JSTokoroten[i].x = (rand() % 1280 - 320) * 100;
                JSTokoroten[i].y = (rand() % 896 - 224) * 100;
                JSTokoroten[i].size = (rand() % 4 + 1) * 2;
                JSTokoroten[i].speed = JSTokorotenSpeed * (JSTokoroten[i].size + 72) / 80000;
                JSTokoroten[i].alfa = (80 - JSTokoroten[i].size) * 255 / 80;
            }
            JSTokorotenFlag = flag + 1;
            break;
        case 2:
            for (i = 0; i < 128; i++) {
                if (JSTokorotenVec == 0) {
                    JSTokoroten[i].x -= JSTokoroten[i].speed * 100;
                    if (JSTokoroten[i].x < -31999) {
                        JSTokoroten[i].x += 128000;
                    }
                } else if (JSTokorotenVec == 1) {
                    JSTokoroten[i].x += JSTokoroten[i].speed * 100;
                    if (JSTokoroten[i].x > 95999) {
                        JSTokoroten[i].x -= 128000;
                    }
                }
                JSTokoroten[i].y += JSTokoroten[i].speed * 100;
                if (JSTokoroten[i].y > 67199) {
                    JSTokoroten[i].y -= 89600;
                }
            }
            break;
        case 3:
            JSTokorotenSpeed = JSTokorotenSpeed * 97 / 100;
            for (i = 0; i < 128; i++) {
                JSTokoroten[i].speed = JSTokorotenSpeed * (JSTokoroten[i].size + 72) / 80000;
                if (JSTokorotenVec == 0) {
                    JSTokoroten[i].x -= JSTokoroten[i].speed * 100;
                    if (JSTokoroten[i].x < -31999) {
                        JSTokoroten[i].x += 128000;
                    }
                } else if (JSTokorotenVec == 1) {
                    JSTokoroten[i].x += JSTokoroten[i].speed * 100;
                    if (JSTokoroten[i].x > 95999) {
                        JSTokoroten[i].x -= 128000;
                    }
                }
                JSTokoroten[i].y += JSTokoroten[i].speed * 100;
                if (JSTokoroten[i].y > 67199) {
                    JSTokoroten[i].y -= 89600;
                }
            }
            break;
        case 4:
            JSTokorotenSpeed = 0;
            for (i = 0; i < 128; i++) {
                JSTokoroten[i].speed = 0;
                if (JSTokoroten[i].x >= 32000) {
                    JSTokoroten[i].x -= (JSTokoroten[i].x - 32000) / 125;
                } else {
                    JSTokoroten[i].x += (32000 - JSTokoroten[i].x) / 125;
                }
                if (JSTokoroten[i].y > 39999) {
                    JSTokoroten[i].y -= (JSTokoroten[i].y - 40000) / 125;
                } else {
                    JSTokoroten[i].y += (40000 - JSTokoroten[i].y) / 125;
                }
            }
            break;
        case 6:
            JSTokorotenSpeed += 1000;
            check = 1;
            for (i = 0; i < 128; i++) {
                JSTokoroten[i].speed = JSTokorotenSpeed * (JSTokoroten[i].size + 72) / 80000;
                if (JSTokoroten[i].alfa > 0) {
                    JSTokoroten[i].alfa -= 8;
                    if (JSTokoroten[i].alfa < 0) {
                        JSTokoroten[i].alfa = 0;
                    }
                    check = 0;
                }
                JSTokoroten[i].y -= JSTokoroten[i].speed * 100;
                if (JSTokoroten[i].y < -22399) {
                    JSTokoroten[i].y += 89600;
                }
            }
            if (check) {
                JSTokorotenFlag = 0;
            }
            break;
        }
    }
    if (flag) {
        for (i = 0; i < 128; i++) {
            JSTokorotenDraw(JSTokoroten[i].x / 100, JSTokoroten[i].y / 100, JSTokoroten[i].speed, JSTokoroten[i].size,
                            (JSTokoroten[i].alfa << 24) | 0xFFFFFF, (JSTokoroten[i].alfa << 24) | 0x80, JSTokorotenVec);
        }
    }
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsGRAY2(CHARACTER_WORK *offset);
int IsGyakuSyoryu(CHARACTER_WORK *offset);
int IsHadou(CHARACTER_WORK *offset);
int IsHaouSyoko(CHARACTER_WORK *offset);
int IsTatsumaki(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int IsTrgAndOffKick(CHARACTER_WORK *offset);
int IsTrgAndOffPunch(CHARACTER_WORK *offset);
int IsTrgAndOffSlash(CHARACTER_WORK *offset);
int IsYoga(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int JS_SpecialAttackCheck(CHARACTER_WORK *offset) {
    if (!(offset->actst & 0x10)) {
        if (IsGRAY2(offset) && GetTensionVal(offset) >= 10000 && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x0208005C);
            return 1;
        }
        if (IsHaouSyoko(offset) && GetTensionVal(offset) >= 5000 && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x0208005B);
            return 1;
        }
        if (IsYoga(offset) && IsTrgAndOffHighSlash(offset)) {
            SetAttackActNo(offset, 0x02080059);
            return 1;
        }
        if (IsGyakuSyoryu(offset)) {
            if (!(offset->ply->ObjFlag & 1) && IsTrgAndOffSlash(offset)) {
                SetAttackActNo(offset, 0x02080063);
                return 1;
            }
            if (IsTrgAndOffKick(offset)) {
                SetAttackActNo(offset, 0x0208005A);
                return 1;
            }
        }
        if (IsTatsumaki(offset) && !(offset->ply->ObjFlag & 1) && IsTrgAndOffSlash(offset)) {
            SetAttackActNo(offset, 0x02080062);
            return 1;
        }
        if (IsHadou(offset) && IsTrgAndOffPunch(offset)) {
            SetAttackActNo(offset, 0x02080057);
            return 1;
        }
    }
    return 0;
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void JS_ExciteCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    SetAttackActNo(offset, 0x02080050);
}

int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);
void set_random_seed_abs(void);
extern GAME_INFOMATION g_info;

void JS_RespectCommnadCheck(CHARACTER_WORK *offset) {
    if (g_info.mode & 0x4000) {
        set_random_seed_abs();
    }
    SetAttackActNo(offset, 0x02080051);
}

short int GetTensionVal(CHARACTER_WORK *offset);
int IsDouji(CHARACTER_WORK *offset, short unsigned int flag, int oncnt, int doujilimit);
int IsSitRight(CHARACTER_WORK *offset);
int IsStdRight(CHARACTER_WORK *offset);
int SetAttackActNo(CHARACTER_WORK *offset, unsigned int no);

int JS_GuardSpecialAttackCheck(CHARACTER_WORK *offset) {
    if (GetTensionVal(offset) >= 5000 && !(offset->actst & 0x10) && IsDouji(offset, 0xF, 2, 2)) {
        if (IsStdRight(offset) || IsSitRight(offset)) {
            offset->actst |= 0x80;
            SetAttackActNo(offset, 0x03080060);
            return 1;
        }
    }
    return 0;
}

int IsHadoux2(CHARACTER_WORK *offset);
int IsTrgAndOffHighSlash(CHARACTER_WORK *offset);
int ItigekiCommandCheck(CHARACTER_WORK *offset, void **functb, int actITIGEKI, int actOVERKILLMODE);

int JS_NoCancelAttackCheck(CHARACTER_WORK *offset) {
    static void *functb[2] = { IsHadoux2, IsTrgAndOffHighSlash };

    if (!(offset->actst & 0x10)) {
        if (ItigekiCommandCheck(offset, functb, 0x5D, 0x55)) {
            return 1;
        }
    }
    return 0;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetExciteCommnadFlag(CHARACTER_WORK *offset);

void JSAct_LongHair1(CHARACTER_WORK *offset) {
    SetExciteCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x50);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetRespectCommnadFlag(CHARACTER_WORK *offset);

void JSAct_LongHair2(CHARACTER_WORK *offset) {
    SetRespectCommnadFlag(offset);
    SetChangeFrameAnime(offset, 0x51);
    EndtoStand(offset);
}

int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_EntryA_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x52);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_EntryA_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0x53);
    offset->CommandFlag = 0xC05F;
    EndtoStand(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);

void JSAct_Lose(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnimeSameActno(offset);
    EndtoDemo(offset);
}

int EndtoDemo(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetWinActFlag(CHARACTER_WORK *offset);

void JSAct_WinA(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetWinActFlag(offset);
    SetChangeFrameAnime(offset, 0x54);
    EndtoDemo(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Throw(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    offset->actst |= 0x40;
    SetChangeFrameAnime(offset, 0x56);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void Tension_Dec(CHARACTER_WORK *offset, int Val);

void JSAct_DA1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst &= ~0x400;
    if (IsActTimeOver(offset, 5)) {
        offset->SpriteFlag &= ~0x200;
    } else {
        offset->SpriteFlag |= 0x200;
    }
    if (SetChangeFrameAnime(offset, 0x60)) {
        Tension_Dec(offset, -4);
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Hissatu5_1(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x62);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Hissatu5_2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x63);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Hissatu4(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x59);
    EndtoStand(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Hissatu3(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5A);
    EndtoSit(offset);
}

int EndtoSit(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Hissatu2(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5B);
    EndtoSit(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Kakusei_GRay(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5C);
    EndtoStand(offset);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void PlayerThrowSetSub(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

int JS_InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt) {
    CHARACTER_WORK *eoffset = GetEnemyPlayer(offset);
    short unsigned int ActNoTbl[19] = {
        0x0, 0xCB, 0xAB, 0xB0, 0xAB, 0xB3, 0xAF, 0xC5, 0xA5, 0xA5,
        0xCA, 0x9C, 0xD2, 0xB1, 0xB5, 0xAC, 0x99, 0x99, 0x99
    };

    if (aoffset->ply == 0) {
        offset->GuardSt &= ~3;
        return 0;
    }
    SetActNoNext(offset, 0x58);
    SetActNoNext(eoffset, ActNoTbl[eoffset->idno]);
    offset->HitStopTime = 60;
    offset->Mark &= ~1;
    eoffset->HitStopTime = 60;
    PlayerThrowSetSub(offset);
    return 1;
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int JS_InterruptProc(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);

void JSAct_Hissatu1_Start(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x57);
    if (offset->Mark) {
        offset->ply->GuardFunc = JS_InterruptProc;
        offset->GuardSt = 0xA01;
    } else {
        offset->GuardSt = 0;
        offset->ply->GuardFunc = 0;
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_Hissatu1_Hit(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    offset->actst |= 0x4000;
    SetChangeFrameAnime(offset, 0x58);
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_OVERKILLMODE(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 0);
    SetChangeFrameAnime(offset, 0x55);
    EndtoStand(offset);
}

void DeleteEffectByItigeki(CHARACTER_WORK *offset);
int EndtoAction(CHARACTER_WORK *offset, int actno);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;
extern int BattleStatus;

void JSAct_ITIGEKI(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    offset->CommandFlag = 0xC05F;
    SetAnten(offset, 1);
    if (SetChangeFrameAnime(offset, 0x5D)) {
        offset->ply->OverKillFlag = 2;
        if (g_info.mode & 0x100) {
            tr_info->itigeki = 1;
        }
    }
    if (!(offset->attackst & 0x80000) || BattleStatus != 2) {
        EndtoAction(offset, 0x5E);
    } else {
        DeleteEffectByItigeki(offset);
        SearchCharacterExecute(eoffset, 0x6D, 0, 0x12);
        SetFlash(1, 0xF01);
        offset->posx = 0;
        eoffset->posx = 0;
        eoffset->ply->MutekiTime = 16;
        SetActNoNext(offset, 0x5F);
        eoffset->actst &= ~0x20;
        g_info.BattleControlFlag |= 8;
    }
}

void DeleteEffectByItigeki(CHARACTER_WORK *offset);
int EndtoStand(CHARACTER_WORK *offset);
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
COLLISION *Get_GCLExNo(CHARACTER_WORK *offset, int no);
void OrScreenFlag(int flag);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetAnten(CHARACTER_WORK *offset, int flag);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
void SetFlash(char time, short unsigned int flag);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
void scrxyset(int x, int y);
extern GAME_INFOMATION g_info;
extern SCREEN_STRUCT *scr_stp;
extern int JSTokorotenFlag;
extern int JSTokorotenVec;
extern int JSTokorotenSpeed;

void JSAct_ITIGEKI2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *eoffset;
    COLLISION *colli;

    eoffset = GetEnemyPlayer(offset);
    SetJumpControl(offset);
    offset->CommandFlag = 0xC05F;
    eoffset->CommandFlag = 0xC05F;
    if (offset->ActTimer >= 32 && offset->ActTimer <= 192) {
        eoffset->AnimeTime = 0;
    }
    SetAnten(offset, 0);
    if (SetChangeFrameAnime(offset, 0x5F)) {
        offset->ply->MutekiTime = 0xFF;
        SetFlash(1, 0xF01);
        if (eoffset->actno != 0x44) {
            SetActNoNext(eoffset, 0x44);
        }
        SetXSpeed(eoffset, 0);
        SetYSpeed(eoffset, 0, 0);
        eoffset->ply->DownTimer = 30000;
        eoffset->SpriteFlag &= ~0x3000;
        OrScreenFlag(0x1000);
        DeleteEffectByItigeki(offset);
        SearchCharacterExecute(eoffset, 0x6D, 0, 0x12);
        SearchCharacterExecute(offset, 0, 0, 0x90);
        SearchCharacterExecute(offset, 0, 0, 0x91);
        SearchCharacterExecute(eoffset, 0, 0, 0x90);
        SearchCharacterExecute(eoffset, 0, 0, 0x91);
        JSTokorotenFlag = 1;
        if (offset->dirflag == 1) {
            JSTokorotenVec = 0;
        } else {
            JSTokorotenVec = 1;
        }
        JSTokorotenSpeed = 80000;
    }
    colli = Get_GCLExNo(offset, 0);
    if (colli && offset->ActTimer <= 192) {
        if (offset->dirflag == 1) {
            eoffset->posx = offset->posx - colli->x * 100;
        } else {
            eoffset->posx = offset->posx + colli->x * 100;
        }
        eoffset->posy = offset->posy + (colli->y + 200) * 100;
    }
    if (offset->ActTimer == 48) {
        JSTokorotenFlag = 3;
    } else if (offset->ActTimer == 194) {
        JSTokorotenFlag = 4;
    } else if (offset->ActTimer == 202) {
        JSTokorotenFlag = 5;
    } else if (offset->ActTimer == 256) {
        JSTokorotenFlag = 6;
        JSTokorotenVec = 2;
        JSTokorotenSpeed = 20000;
    }
    if (offset->posy >= 0 && offset->ActTimer > 192) {
        SetActNoNext(offset, 0);
        g_info.BattleControlFlag &= ~8;
    }
    if (offset->posy < 0 && offset->ActTimer > 192) {
        if (offset->ActTimer >= 194 && offset->ActTimer < 202) {
            scrxyset((scr_stp->stLastPosX1P + scr_stp->stLastPosX2P) / 2, scr_stp->stLastPosY);
            scr_stp->stLastPosY -= 700;
        }
        if (offset->yspeed == 0) {
            OrScreenFlag(0x80);
        } else {
            scr_stp->stScreenFlag &= ~0x80;
        }
    }
    EndtoStand(offset);
}

int EndtoStand(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);

void JSAct_ITIGEKI_Back(CHARACTER_WORK *offset) {
    offset->CommandFlag = 0xC05F;
    SetChangeFrameAnime(offset, 0x5E);
    EndtoStand(offset);
}
