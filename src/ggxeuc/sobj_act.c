#include "generated/sobj_act.h"
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void AddXSpeed(CHARACTER_WORK *offset, short int xspeed);
void SetSemiTrans(CHARACTER_WORK *offset, char trans, short int transmode);
int EndtoAction(CHARACTER_WORK *offset, int actno);

int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, short unsigned int no);
int rand(void);

void SBKObj_Momiji(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 0)) {
        if (Random() & 1) {
            offset->priority = 0x6E;
        } else {
            offset->priority = 0x190;
            offset->scale = 0x2EE;
        }
        offset->posx = (rand() & 0x3FF) * 100;
        offset->gravity = 0;
        offset->posy = -0x19000;
        offset->HitBackx = -((Random() & 0x7F) + 0x80);
        if (offset->HitBackx == 0) {
            offset->xspeed = -10;
        }
        offset->yspeed = (Random() & 0x7F) + 0x80;
        offset->dirflag = 1;
        offset->AdBackx = 0;
    }
    if (offset->posy > 8000) {
        offset->actst |= 0x80000000;
    } else if (offset->AdBackx) {
        offset->xspeed += 2;
        offset->posy += offset->yspeed;
        if (offset->xspeed >= 0) {
            offset->HitBackx = -((Random() & 0x7F) + 0x80);
            offset->yspeed = (Random() & 0x7F) + 0x80;
            offset->AdBackx = 0;
        }
    } else {
        offset->xspeed--;
        offset->posy += offset->yspeed;
        if (offset->xspeed <= offset->HitBackx) {
            offset->yspeed = (Random() & 0x7F) + 0x80;
            offset->AdBackx = 1;
        }
    }
}

void SMLHato_Walk(CHARACTER_WORK *offset) {
    int r;

    SetChangeFrameAnimeSameActno(offset);
    SetXSpeed(offset, 100);
    if (offset->ActTimer < 30) return;
    r = Random() & 0x7F;
    if (r < 4) {
        SetActNoNext(offset, 8);
    } else if (r < 8) {
        if (offset->dirflag == 0) {
            SetActNoNext(offset, 4);
        } else {
            SetActNoNext(offset, 5);
        }
    } else if (r < 24) {
        SetActNoNext(offset, 2);
    }
    if (offset->posx < 42000 && offset->dirflag == 0) {
        SetActNoNext(offset, 4);
    }
}

void SMLHato_Stand(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        switch (Random() & 3) {
        case 0:
            SetActNoNext(offset, 1);
            break;
        case 1:
            SetActNoNext(offset, 3);
            break;
        case 2:
            SetActNoNext(offset, 6);
            break;
        case 3:
            SetActNoNext(offset, 7);
            break;
        }
    }
}

void SMLHato_Action1(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetXSpeed(offset, 0);
    if ((offset->InstSt & 0x80) && !(Random() & 3)) {
        SetActNoNext(offset, 0);
    }
}

void SMLTurnHato_Turn(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0x40) {
        SetActNoNext(offset, 0);
    }
}

void SMLHato_TakeOff(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    AddXSpeed(offset, 10);
    offset->gravity = -5;
    offset->posy += offset->yspeed;
    offset->yspeed += offset->gravity;
    if (offset->ActTimer > 20) {
        SetActNoNext(offset, 9);
    }
}

void SMLHato_FlyUp(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 9);
    AddXSpeed(offset, 50);
    if (offset->xspeed < -1000) {
        offset->xspeed = -1000;
    }
    if (offset->xspeed > 1000) {
        offset->xspeed = 1000;
    }
    offset->gravity = -15;
    offset->posy += offset->yspeed;
    offset->yspeed += offset->gravity;
    if (offset->posy <= -100000) {
        offset->actst |= 0x80000000;
    }
}

int abs(int);

void SMLHato_FlapLand(CHARACTER_WORK *offset) {
    int Resist;

    SetChangeFrameAnimeSameActno(offset);
    if (offset->HitBackx - offset->posx >= 0) {
        Resist = (offset->HitBackx - offset->posx) / 2000 + 2;
    } else {
        Resist = (offset->posx - offset->HitBackx) / 2000 + 2;
    }
    AddXSpeed(offset, 50);
    AddXSpeed(offset, -abs(offset->xspeed) / Resist);
    if (offset->RHitBackx - offset->posy >= 0) {
        Resist = (offset->RHitBackx - offset->posy) / 1500 + 2;
    } else {
        Resist = (offset->posy - offset->RHitBackx) / 1500 + 2;
    }
    offset->gravity = 40;
    offset->posy += offset->yspeed;
    offset->yspeed += offset->gravity;
    offset->yspeed -= offset->yspeed / Resist;
    if (offset->posy >= offset->RHitBackx) {
        offset->yspeed = 0;
        offset->posy = offset->RHitBackx;
        SetActNoNext(offset, 0);
    }
}

void SMLHato_FlyDown(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 9);
    SetXSpeed(offset, 1000);
    if (offset->posy >= offset->RHitBackx - 5000) {
        offset->yspeed -= offset->yspeed / 10;
    }
    offset->gravity = 3;
    offset->posy += offset->yspeed;
    offset->yspeed += offset->gravity;
    if (offset->posx < offset->HitBackx) {
        offset->dirflag = 1;
    } else if (offset->posx > offset->HitBackx) {
        offset->dirflag = 0;
    }
    if ((offset->HitBackx - offset->posx >= 0 ? offset->HitBackx - offset->posx : offset->posx - offset->HitBackx) < 30000) {
        SetActNoNext(offset, 0x1000B);
    }
}

extern int StageNo;
void RqHamon(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);

void HamonCreate(CHARACTER_WORK *offset) {
    if (offset->actst & 0x10) return;
    if (StageNo == 3 && !SearchEffectWorkActno(offset, 0x81, 2)) {
        RqHamon(offset);
    }
}

void SetSemiTrans(CHARACTER_WORK *offset, char trans, short int transmode);

void SMYObj_Smoke(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    if (offset->ActTimer == 0) {
        offset->xspeed = 600;
        SetSemiTrans(offset, -1, 0);
        offset->scale = 1000;
    }
    if (offset->InstSt & 0xC8) {
        offset->actst |= 0x80000000;
    } else {
        offset->posx += offset->xspeed;
        offset->xspeed -= 10;
        offset->posy -= 180;
        offset->scale += 200;
        offset->trans -= 5;
    }
}

void SDZObj_Coming(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->ActTimer == 0) {
        offset->RHitBackx = Random() & 1;
        if (offset->RHitBackx) {
            offset->posx = 0x19000;
            offset->HitBackx = ((Random() & 0x1FF) + 300) * 100;
            offset->dirflag = 0;
        } else {
            offset->posx = -0x19000;
            offset->HitBackx = -((Random() & 0x1FF) + 350) * 100;
            offset->dirflag = 1;
        }
        offset->posy = (Random() & 3) * -2400;
        if (offset->posy == 0) {
            offset->posy = -2400;
        }
    }
    if (offset->RHitBackx) {
        offset->posx -= 300;
        if (offset->posx < offset->HitBackx) {
            SetActNoNext(offset, 12);
            offset->Localid = Random() & 0x3F;
        }
    } else {
        offset->posx += 300;
        if (offset->posx > offset->HitBackx) {
            SetActNoNext(offset, 12);
            offset->Localid = Random() & 0x3F;
        }
    }
}

void SDZObj_Run(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->Localid <= 0) {
        SetActNoNext(offset, 12);
        return;
    }
    if (offset->dirflag == 1) {
        offset->posx += 300;
        switch (offset->posy) {
        case -2400:
            if (offset->posx >= -13000) {
                SetActNoNext(offset, 12);
            }
            break;
        case -4800:
            if (offset->posx >= -17000) {
                SetActNoNext(offset, 12);
            }
            break;
        case -7200:
            if (offset->posx >= -20000) {
                SetActNoNext(offset, 12);
            }
            break;
        }
    } else {
        offset->posx -= 300;
        switch (offset->posy) {
        case -2400:
            if (offset->posx <= 21000) {
                SetActNoNext(offset, 12);
            }
            break;
        case -4800:
            if (offset->posx <= 16000) {
                SetActNoNext(offset, 12);
            }
            break;
        case -7200:
            if (offset->posx <= 9500) {
                SetActNoNext(offset, 12);
            }
            break;
        }
    }
    offset->Localid--;
}

void SDZObj_Standing(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 10);
    if (offset->Localid <= 0) {
        switch (Random() & 7) {
        case 0:
            SetActNoNext(offset, 3);
            offset->actnoSv = 8;
            offset->Localid = (Random() & 0xFF) + 0x100;
            break;
        case 1:
            SetActNoNext(offset, 7);
            offset->Localid = (Random() & 0xFF) + 0x100;
            break;
        case 2:
            SetActNoNext(offset, 4);
            offset->Localid = (Random() & 0xFF) + 0x80;
            break;
        case 3:
            SetActNoNext(offset, 11);
            break;
        case 4:
            SetActNoNext(offset, 13);
            offset->actnoSv = 2;
        case 5:
            SetActNoNext(offset, 3);
            offset->actnoSv = 1;
            offset->Localid = (Random() & 0x3F) + 0x40;
            break;
        case 6:
            SetActNoNext(offset, 13);
            offset->actnoSv = 2;
        case 7:
            SetActNoNext(offset, 4);
            offset->Localid = (Random() & 0xFF) + 0x80;
            break;
        }
    } else {
        offset->Localid--;
    }
}

void SDZObj_Sit(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, offset->actnoSv);
    }
}

void SDZObj_Mozo(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 3);
    if (offset->Localid <= 0) {
        if (Random() & 1) {
            offset->Localid = Random() & 0x3F;
            SetActNoNext(offset, 2);
        } else {
            offset->actnoSv = 6;
            offset->Localid = Random() & 0x3F;
            SetActNoNext(offset, 5);
        }
    } else {
        offset->Localid--;
    }
}

void SDZObj_EatSub(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 4);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, offset->actnoSv);
    }
}

void SDZObj_Eat(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 5);
    if (offset->Localid <= 0) {
        offset->Localid = Random() & 0x3F;
        SetActNoNext(offset, 4);
    }
    offset->Localid--;
}

void SDZObj_Smel(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 6);
    if (offset->Localid == 0) {
        offset->Localid = Random() & 0x3F;
        SetActNoNext(offset, 2);
    }
    offset->Localid--;
}

void SDZObj_SleepSub(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 7);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 9);
    }
}

void SDZObj_Sleep(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 8);
    if (offset->Localid == 0) {
        SetActNoNext(offset, 10);
    }
    offset->Localid--;
}

void SDZObj_WakeUp(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 9);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 12);
    }
}

void SDZObj_StandUp(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    if (offset->InstSt & 0xC8) {
        offset->Localid = Random() & 0x3F;
        SetActNoNext(offset, 2);
    }
}

void SDZObj_Turn(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 10);
    if (offset->dirflag == 1) {
        offset->dirflag = 0;
    } else {
        offset->dirflag = 1;
    }
    if (offset->actnoSv == 14) {
        SetActNoNext(offset, 3);
    } else {
        SetActNoNext(offset, offset->actnoSv);
    }
    offset->Localid = 10;
}

void SDZObj_GoHomeSub(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 10);
    offset->actnoSv = 14;
    if (offset->RHitBackx) {
        if (offset->dirflag == 0) {
            SetActNoNext(offset, 13);
        } else {
            SetActNoNext(offset, 3);
        }
    } else {
        if (offset->dirflag == 1) {
            SetActNoNext(offset, 13);
        } else {
            SetActNoNext(offset, 3);
        }
    }
}

void SDZObj_GoHome(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->RHitBackx) {
        offset->posx += 300;
        if (offset->posx > 0x19000) {
            offset->actst |= 0x80000000;
        }
    } else {
        offset->posx -= 300;
        if (offset->posx < -0x19000) {
            offset->actst |= 0x80000000;
        }
    }
}

int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void SDZObj_InDzHand1(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    offset->priority = poffset->priority - 1;
    SetChangeFrameAnime(offset, 11);
    SetParentPosExNo(offset, 0);
    if (poffset->Mark == 1) {
        SetActNoNext(offset, 16);
    }
}

int SetParentPosExNo(CHARACTER_WORK *offset, int no);

void SDZObj_InDzHand2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    SetChangeFrameAnime(offset, 12);
    offset->priority = poffset->priority - 1;
    SetParentPosExNo(offset, 0);
    if (poffset->Mark == 2) {
        SetActNoNext(offset, 17);
    }
}

void SetLandBreakActno(CHARACTER_WORK *offset, unsigned int no);

void SDZObj_DzPut(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    offset->priority = poffset->priority - 1;
    SetChangeFrameAnime(offset, 13);
    SetLandBreakActno(offset, 18);
}

void OutSideEliminate(CHARACTER_WORK *offset);

void SDZObj_DzBye(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset = offset->parentWork;

    offset->priority = poffset->priority - 1;
    SetChangeFrameAnime(offset, 14);
    if (offset->Mark) {
        offset->Mark = 0;
        offset->dirflag = ~offset->dirflag & 1;
    }
    OutSideEliminate(offset);
}

void SDZObj2_Root(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->ActTimer == 0) {
        offset->Localid = (Random() & 0xFF) + 0x80;
    } else if (offset->Localid) {
        offset->Localid--;
    } else {
        offset->Localid = Random() & 0xFF;
        offset->actnoSv = Random() & 3;
        SetActNoNext(offset, 4);
    }
}

void SDZObj2_Jmp(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, offset->actnoSv);
    }
}

void SDZObj2_Swing(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    if (offset->Localid) {
        offset->Localid--;
    } else {
        SetActNoNext(offset, 4);
        offset->actnoSv = 0;
        offset->Localid = (Random() & 0xFF) + 0x80;
    }
}

void SDZObj2_Smel(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 3);
    if (offset->Localid) {
        offset->Localid--;
    } else {
        SetActNoNext(offset, 0);
        offset->Localid = (Random() & 0xFF) + 0x80;
        if (Random() & 1) {
            offset->dirflag = 1;
        } else {
            offset->dirflag = 0;
        }
    }
}

void SDZObj2_Fly(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 4);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
        offset->Localid = (Random() & 0xFF) + 0x80;
        if (Random() & 1) {
            offset->dirflag = 1;
        } else {
            offset->dirflag = 0;
        }
    }
}

void SetJumpControl(CHARACTER_WORK *offset);
void SetSemiTrans(CHARACTER_WORK *offset, char trans, short int transmode);

void SDZObj3_Create(CHARACTER_WORK *offset) {
    short s[4] = {450, 350, 250, 150};
    short p[4] = {0, 300, 465, 465};
    short y[4] = {0, -100, -120, -120};
    int i;

    if (SetChangeFrameAnime(offset, 0)) {
        SetSemiTrans(offset, 0, 1);
        offset->posx = (Random() & 0x3FF) * 100;
        if (Random() & 1) {
            offset->posx = -offset->posx;
        }
        offset->yspeed = -((Random() & 0x3F) + 0x10);
        i = Random() & 3;
        offset->AdBackx = 0;
        offset->scale = s[i];
        offset->priority = p[i];
        offset->posy = y[i] * 100;
        offset->RHitBackx = (Random() & 0x1FF) + 0x400;
        offset->attackst = offset->scale;
    }
    if (!(offset->AdBackx & 2)) {
        offset->xspeed++;
        if (offset->xspeed >= 40 && !(Random() & 7)) {
            offset->AdBackx |= 2;
        }
    } else {
        offset->xspeed--;
        if (offset->xspeed < -39 && !(Random() & 7)) {
            offset->AdBackx &= ~2;
        }
    }
    if (!(offset->AdBackx & 1)) {
        offset->trans++;
        if (offset->trans > 200) {
            offset->AdBackx |= 1;
        }
    } else {
        if (offset->ActTimer > offset->RHitBackx) {
            offset->trans--;
            if (offset->trans == 0) {
                offset->actst |= 0x80000000;
            }
        }
    }
    SetJumpControl(offset);
}

void SetSemiTrans(CHARACTER_WORK *offset, char trans, short int transmode);

void STSObj_Create(CHARACTER_WORK *offset) {
    int i;

    SetChangeFrameAnime(offset, 1);
    if (offset->ActTimer == 0) {
        i = Random();
        offset->posx = (Random() & 0x3FF) * 100;
        offset->posy = -(Random() & 0x1FF) * 100;
        if (i & 1) {
            offset->posx = -offset->posx;
        }
        offset->scale = i & 0x3FF;
        if (offset->scale < 200) {
            offset->scale += 200;
        } else if (offset->scale > 1000) {
            offset->scale = 1000;
        }
        offset->Localid = offset->scale;
        SetSemiTrans(offset, 0, 1);
        offset->yspeed = (i & 3) * 10;
        offset->xspeed = (i & 3) * 25;
        if (offset->xspeed == 0) {
            offset->xspeed = 40;
        }
        if (Random() & 1) {
            offset->dirflag = 1;
            offset->xspeed = -offset->xspeed;
        }
        if (Random() & 1) {
            offset->yspeed = -offset->yspeed;
        }
    }
    if (offset->trans >= 0x99) {
        SetActNoNext(offset, 0x10003);
        offset->RHitBackx = (Random() & 0x1FF) + 0x1FF;
        offset->actnoSv = 20;
    } else {
        offset->trans++;
        offset->posy += offset->yspeed;
    }
}

void STSObj_Flying(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    SetSemiTrans(offset, -0x67, 1);
    offset->scale = offset->Localid;
    if (offset->actnoSv) {
        offset->yspeed = (Random() & 3) * 10;
        offset->HitBackx = (Random() & 7) * 25;
        if (offset->HitBackx == 0) {
            offset->HitBackx = 125;
        }
        if (offset->dirflag == 0) {
            offset->dirflag = 1;
            offset->HitBackx = -offset->HitBackx;
        } else {
            offset->dirflag = 0;
        }
        if (Random() & 1) {
            offset->yspeed = -offset->yspeed;
        }
        offset->actnoSv = (Random() & 0x7F) + 0x7F;
        SetActNoNext(offset, 0x10002);
    } else {
        offset->actnoSv--;
    }
}

int abs(int);

void STSObj_Moving(CHARACTER_WORK *offset) {
    if (abs(offset->xspeed) > 100) {
        SetChangeFrameAnime(offset, 1);
    } else {
        SetChangeFrameAnime(offset, 0);
    }
    SetSemiTrans(offset, -0x67, 1);
    offset->scale = offset->Localid;
    if ((offset->xspeed < 0 ? -offset->xspeed : offset->xspeed) > 20 || offset->actnoSv != 0) {
        offset->posy += offset->yspeed;
        if (offset->actnoSv == 0) {
            if (offset->dirflag) {
                offset->xspeed++;
            } else {
                offset->xspeed--;
            }
        } else {
            offset->actnoSv--;
            if (offset->dirflag) {
                if (offset->xspeed > offset->HitBackx) {
                    offset->xspeed--;
                }
            } else {
                if (offset->xspeed < offset->HitBackx) {
                    offset->xspeed++;
                }
            }
        }
        if (--offset->RHitBackx == 0) {
            SetActNoNext(offset, 0x10004);
            offset->attackst = offset->trans;
        }
    } else {
        SetActNoNext(offset, 0x10003);
    }
}

void STSObj_Moved(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    SetSemiTrans(offset, -0x67, 1);
    offset->scale = offset->Localid;
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 1);
        offset->actnoSv = (Random() & 0x3F) + 0x3F;
    }
}

void STSObj_Delete(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    SetSemiTrans(offset, offset->attackst, 1);
    offset->trans--;
    offset->scale = offset->Localid;
    offset->attackst = offset->trans;
    if (offset->trans == 0) {
        offset->actst |= 0x80000000;
    }
    offset->posy += offset->yspeed;
}

void STSObj2_Main(CHARACTER_WORK *offset) {
    SetSemiTrans(offset, 0x3C, 1);
    SetChangeFrameAnime(offset, 0);
}

void SJMObj_Steam1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->ActTimer == 0) {
        SetSemiTrans(offset, 0, 1);
        offset->scale = 1000;
        if (offset->posx == -0x19000) {
            SetActNoNext(offset, 1);
        }
        offset->posx = 0x639C;
        offset->posy = -0xCB20;
        offset->xspeed = (Random() & 3) * 25;
        if (Random() & 1) {
            offset->xspeed = -offset->xspeed;
        }
        offset->AdBackx = 0;
        offset->yspeed = 100;
    }
    offset->posy -= offset->yspeed;
    if (offset->AdBackx) {
        if (offset->trans == 0) {
            offset->actst |= 0x80000000;
        } else {
            offset->trans--;
            offset->scale += 20;
        }
    } else {
        offset->trans += 10;
        if (offset->trans >= 200) {
            offset->AdBackx = 1;
        }
    }
}

void SJMObj_Steam2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->ActTimer == 0) {
        SetSemiTrans(offset, 0, 1);
        offset->scale = 1500;
        offset->posy = -0x5DC0;
        offset->posx = 0x1B58;
        offset->xspeed = 150;
        offset->yspeed = 100;
        offset->AdBackx = 0;
    }
    offset->posy -= offset->yspeed;
    if (offset->AdBackx) {
        if (offset->trans == 0) {
            offset->actst |= 0x80000000;
        } else {
            offset->scale += 10;
            if (offset->ActTimer & 1) {
                offset->trans--;
            }
        }
    } else {
        offset->trans += 10;
        if (offset->trans >= 100) {
            offset->AdBackx = 1;
        }
    }
}

void SVEObj_Jirou(CHARACTER_WORK *offset) {
    short s[10] = {600, 800, 800, 800, 700, 600, 500, 400, 350, 300};

    if (offset->ActTimer == offset->Localid * 6) {
        SetChangeFrameAnime(offset, 0);
        offset->scale = s[offset->HitBackx];
        SetSemiTrans(offset, -6, 1);
    }
}

void SSLObj_Loop1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    SetSemiTrans(offset, -0x38, 1);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, Random() & 1);
    }
}

void SSLObj_Loop2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    SetSemiTrans(offset, -0x38, 1);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
    }
}

void SSLObj3_1(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    if (offset->ActTimer == 0) {
        offset->priority = 480;
    }
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, Random() & 3);
    }
}

void SSLObj3_2(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    if (offset->ActTimer == 0) {
        offset->priority = 480;
    }
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, Random() & 3);
    }
}

void SSLObj3_3(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    if (offset->ActTimer == 0) {
        offset->priority = 480;
    }
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, Random() & 3);
    }
}

void SSLObj3_4(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 3);
    if (offset->ActTimer == 0) {
        offset->priority = 480;
    }
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, Random() & 3);
    }
}

void SSLObj4_Loop(CHARACTER_WORK *offset) {
    SetSemiTrans(offset, Random(), 1);
    SetChangeFrameAnime(offset, 0);
}

void SAXObj_BUTAPriority(CHARACTER_WORK *offset) {
    offset->priority = 470;
    offset->priority2 = 480;
}

void SAXObj_BUTAPriority(CHARACTER_WORK *offset);

void SAXObj_Siting(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 0)) {
        offset->Localid = (Random() & 0x1FF) + 0xFF;
        SAXObj_BUTAPriority(offset);
    }
    if (--offset->Localid <= 0) {
        SetActNoNext(offset, 3);
    }
}

void SAXObj_BUTAPriority(CHARACTER_WORK *offset);

void SAXObj_Standup(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 3)) {
        SAXObj_BUTAPriority(offset);
    }
    if (EndtoAction(offset, 1)) {
        offset->Localid = (Random() & 0x3FF) + 0x1FF;
    }
}

void SAXObj_BUTAPriority(CHARACTER_WORK *offset);

void SAXObj_Sitdown(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 2)) {
        SAXObj_BUTAPriority(offset);
    }
    if (EndtoAction(offset, 0)) {
        offset->Localid = (Random() & 0x1FF) + 0xFF;
    }
}

void SAXObj_BUTAPriority(CHARACTER_WORK *offset);

void SAXObj_Walk(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 1)) {
        SAXObj_BUTAPriority(offset);
        if (Random() & 0x3F) {
            SetXSpeed(offset, -50);
        } else {
            SetXSpeed(offset, 50);
        }
    }
    if (--offset->Localid <= 0) {
        SetActNoNext(offset, 2);
        offset->xspeed = 0;
        return;
    }
    offset->RHitBackx++;
    if (offset->RHitBackx > 180 && !(offset->RHitBackx & 0x1F) && !(Random() & 0xF)) {
        SetActNoNext(offset, 4);
    }
    if (offset->RHitBackx > 60) {
        if (offset->posx <= -36000) {
            SetActNoNext(offset, 4);
        } else if (offset->posx >= 36000) {
            SetActNoNext(offset, 4);
        }
    }
}

void SAXObj_BUTAPriority(CHARACTER_WORK *offset);

void SAXObj_Trun(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 4)) {
        SAXObj_BUTAPriority(offset);
        offset->RHitBackx = 0;
    }
    EndtoAction(offset, 1);
}

extern RECTL ScreenObj;
int EndtoEliminate(CHARACTER_WORK *offset);

void SAXObj_Rain(CHARACTER_WORK *offset) {
    int ytb[8] = {2000, 1000, 0, -2000, -4000, -6000, -8000, -9000};
    int stb[8] = {1000, 950, 900, 850, 800, 700, 600, 500};
    int atb[8] = {127, 147, 167, 187, 197, 207, 217, 227};
    int z;

    if (SetChangeFrameAnime(offset, 0)) {
        offset->posx = (Random() & 0x1FF) * 100;
        z = Random() & 7;
        offset->posy = ytb[z];
        offset->scale = stb[z];
        if (Random() & 1) {
            offset->posx = -offset->posx;
        }
        offset->posx += ScreenObj.x;
        if (offset->posx > 60000) {
            offset->posx -= 51200;
        }
        offset->trans -= atb[z];
    }
    EndtoEliminate(offset);
}

void SPOObj_Walk(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, 0)) {
        offset->scale = offset->HitBackx;
        if (offset->Localid & 1) {
            offset->dirflag = 1;
            offset->xspeed = -300;
        } else {
            offset->dirflag = 0;
            offset->xspeed = 300;
        }
        offset->RHitBackx = (Random() & 0x7F) + 0x3F;
    }
    if (offset->Localid & 2) {
        offset->priority = 100;
    } else {
        offset->priority = 400;
    }
    if (--offset->RHitBackx == -1) {
        if (Random() & 1) {
            SetActNoNext(offset, 0x10001);
        } else {
            SetActNoNext(offset, 0x10002);
        }
        offset->xspeed = 0;
    }
    if (offset->dirflag == 1) {
        if (offset->posx <= -80000) {
            offset->actst |= 0x80000000;
        }
    } else {
        if (offset->posx >= 80000) {
            offset->actst |= 0x80000000;
        }
    }
}

void SPOObj_Stand(CHARACTER_WORK *offset) {
    if (offset->Localid & 2) {
        offset->priority = 100;
    } else {
        offset->priority = 400;
    }
    if (SetChangeFrameAnime(offset, 1)) {
        offset->scale = offset->HitBackx;
    }
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0x10000);
    }
}

void SPOObj_Stop(CHARACTER_WORK *offset) {
    if (offset->Localid & 2) {
        offset->priority = 100;
    } else {
        offset->priority = 400;
    }
    if (SetChangeFrameAnime(offset, 2)) {
        offset->scale = offset->HitBackx;
    }
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0x10000);
    }
}

void SKRObj_Stand(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        switch (Random() & 3) {
        case 0:
            SetActNoNext(offset, 0);
            break;
        case 1:
            SetActNoNext(offset, 1);
            break;
        case 2:
            if (offset->posx >= 72000) {
                SetActNoNext(offset, 1);
            } else if (offset->dirflag == 0) {
                SetActNoNext(offset, 4);
            } else {
                SetActNoNext(offset, 2);
            }
            break;
        case 3:
            if (offset->posx <= 62500) {
                SetActNoNext(offset, 1);
            } else if (offset->dirflag == 1) {
                SetActNoNext(offset, 5);
            } else {
                SetActNoNext(offset, 2);
            }
            break;
        }
    }
}

void SKRObj_Face(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        if ((Random() & 7) < 6) {
            SetActNoNext(offset, 1);
        } else {
            SetActNoNext(offset, 0);
        }
    }
}

void SKRObj_Walk(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    SetXSpeed(offset, 0);
    SetXSpeed(offset, 50);
    if (offset->posx <= 62500 || offset->posx >= 72000) {
        if (offset->posx >= 72000 && offset->dirflag == 1) {
            SetActNoNext(offset, 3);
        }
        if (offset->posx <= 62500 && offset->dirflag == 0) {
            SetActNoNext(offset, 3);
        }
    } else {
        if (offset->InstSt & 0xC8) {
            if (Random() & 1) {
                SetActNoNext(offset, 3);
            } else {
                SetActNoNext(offset, 2);
            }
        }
    }
}

void SKRObj_WalkEnd(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 3);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
    }
}

void SKRObj_LtoR(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 4);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 2);
    }
}

void SKRObj_RtoL(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 5);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 2);
    }
}

void SKRObj2_Stand(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        switch (Random() & 3) {
        case 0:
            SetActNoNext(offset, 0);
            break;
        case 1:
        case 2:
            SetActNoNext(offset, 1);
            break;
        case 3:
            if (offset->dirflag == 0) {
                SetActNoNext(offset, 4);
            } else {
                SetActNoNext(offset, 5);
            }
            break;
        }
    }
}

void SKRObj2_Face(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        if ((Random() & 7) < 6) {
            SetActNoNext(offset, 1);
        } else {
            SetActNoNext(offset, 0);
        }
    }
}

void SKRObj2_LtoR(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 4);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
    }
}

void SKRObj2_RtoL(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 5);
    SetXSpeed(offset, 0);
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
    }
}

void SKRObj3_Stand(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 0);
    SetXSpeed(offset, 0);
    offset->yspeed = 0;
    if (offset->InstSt & 0xC8) {
        switch (Random() & 3) {
        case 0:
            SetActNoNext(offset, 0);
            break;
        case 1:
        case 2:
            if (offset->dirflag == 0) {
                SetActNoNext(offset, 1);
            } else {
                SetActNoNext(offset, 6);
            }
            break;
        case 3:
            if (offset->dirflag == 0) {
                SetActNoNext(offset, 1);
            } else {
                SetActNoNext(offset, 2);
            }
            break;
        }
    }
}

void SKRObj3_StandLtoR(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 1);
    SetXSpeed(offset, 0);
    offset->yspeed = 0;
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
    }
}

void SKRObj3_StandRtoL(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 2);
    SetXSpeed(offset, 0);
    offset->yspeed = 0;
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
    }
}

void SKRObj3_Fly(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 3);
    offset->yspeed = 0;
    if (offset->xspeed == 0) {
        if (offset->RHitBackx == -256 || offset->RHitBackx == 256) {
            offset->RHitBackx = offset->RHitBackx / 256;
            offset->xspeed = offset->RHitBackx;
        } else if (offset->RHitBackx != 0) {
            if (offset->InstSt & 0xC8) {
                if (offset->dirflag == 1) {
                    SetActNoNext(offset, 5);
                    offset->RHitBackx = 0;
                } else {
                    SetActNoNext(offset, 4);
                    offset->RHitBackx = 0;
                }
            }
        } else {
            offset->RHitBackx = 1;
            offset->xspeed = 1;
        }
    } else if (offset->xspeed >= 120) {
        offset->RHitBackx = -1;
        offset->xspeed--;
    } else if (offset->xspeed < -119) {
        offset->RHitBackx = 1;
        offset->xspeed++;
    } else {
        offset->xspeed += offset->RHitBackx;
    }
}

void SKRObj3_FlyLtoR(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 4);
    SetXSpeed(offset, 0);
    offset->yspeed = 0;
    if (offset->InstSt & 0xC8) {
        if (Random() & 1) {
            SetActNoNext(offset, 7);
        } else {
            SetActNoNext(offset, 3);
            offset->RHitBackx = 256;
        }
    }
}

void SKRObj3_FlyRtoL(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 5);
    SetXSpeed(offset, 0);
    offset->yspeed = 0;
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 3);
        offset->RHitBackx = -256;
    }
}

void SKRObj3_Takeoff(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 6);
    SetXSpeed(offset, 0);
    offset->yspeed = -50;
    offset->posy += offset->yspeed;
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 3);
        offset->yspeed = 0;
        offset->RHitBackx = 0;
    }
}

void SKRObj3_Landing(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, 7);
    SetXSpeed(offset, 0);
    offset->yspeed = 50;
    offset->posy += offset->yspeed;
    if (offset->InstSt & 0xC8) {
        SetActNoNext(offset, 0);
        offset->yspeed = 0;
    }
}
