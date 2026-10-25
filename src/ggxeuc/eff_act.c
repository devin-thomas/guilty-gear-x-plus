#include "generated/eff_act.h"

void HibanaDraw(CHARACTER_WORK *offset);
void SetHibanaVertex(CHARACTER_WORK *offset);
int InitHibanaVertex(CHARACTER_WORK *offset);
void PolySkyDashEffectDraw(CHARACTER_WORK *offset);
void HamonEffectDraw(CHARACTER_WORK *offset);
void UkemiEffectDraw(CHARACTER_WORK *offset);
void PerfectGuardEffectDraw2(CHARACTER_WORK *offset);
void PerfectGuardEffectDraw(CHARACTER_WORK *offset);
void PerfectGuardDisaappearEffectDraw(CHARACTER_WORK *offset);
void LightningEffectDraw(CHARACTER_WORK *offset);
void LightningCenterEffectDraw(CHARACTER_WORK *offset);
void HitEff172Draw(CHARACTER_WORK *offset);
void HitEff172Draw2(CHARACTER_WORK *offset);
void HitEff173Draw(CHARACTER_WORK *offset);
void HitEff174Draw(CHARACTER_WORK *offset);
void HitEff175Draw(CHARACTER_WORK *offset);
void HitEffSlashSplashDraw(CHARACTER_WORK *offset);
void HitEffHandFightSplashDraw(CHARACTER_WORK *offset);
void HitEffKakuseiSplashDraw(CHARACTER_WORK *offset);
void HitEffKakuseiRingDraw(CHARACTER_WORK *offset);
void HitEffItigekiReadyDraw(CHARACTER_WORK *offset);
void HitEffRomanCancelDraw(CHARACTER_WORK *offset);
void HitEffSDashSplashDraw(CHARACTER_WORK *offset);
void KakuseiEffectDraw(CHARACTER_WORK *offset);
int EndtoEliminate(CHARACTER_WORK *offset);
int TimetoEliminate(CHARACTER_WORK *offset, int time);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
int SetChangeFrameAnimeSameActno(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int Random(void);
float G_Cos(unsigned short r);
float G_Sin(unsigned short r);
int IsHitStop(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
void ParentFlagSuccession(CHARACTER_WORK *poffset, CHARACTER_WORK *offset);
extern unsigned char PG_RTbByIdno[19];
extern unsigned char PG_offyStandTbByIdno[19];
extern unsigned char PG_offySitTbByIdno[19];
extern unsigned char PG_offySkyTbByIdno[19];
void KakuseiEffectDraw2(CHARACTER_WORK *offset);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg, unsigned short actno);
void SetAnten(CHARACTER_WORK *offset, int flag);
void SetForceStopTime(short t1p, short t2p);
void KakuseiEffectDraw3(CHARACTER_WORK *offset);
void KakuseiEffectDraw4(CHARACTER_WORK *offset);

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);

void CMNEFFActDef(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);

void CMNEFFActWithJumpDef(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);

void CMNEFFActWithRandPos(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnime(offset, offset->actno)) {
        offset->parentx += (Random() & 0x3F) - 0x20;
        offset->parenty += (Random() & 0x3F) - 0x20;
    }
    EndtoEliminate(offset);
}

int EndtoEliminate(CHARACTER_WORK *offset);
float G_Cos(unsigned short r);
float G_Sin(unsigned short r);
int GetXMove(CHARACTER_WORK *offset, int val);
int Random(void);
int SetChangeFrameAnime(CHARACTER_WORK *offset, unsigned short no);
void SetJumpControl(CHARACTER_WORK *offset);
void SetXSpeed(CHARACTER_WORK *offset, int xspeed);
void SetYSpeed(CHARACTER_WORK *offset, int yspeed, int grav);
int nlArcTan2(int x, int y);

void H20_20_Act(CHARACTER_WORK *offset) {
    static int scaletb[10] = { 800, 1000, 1200, 800, 1000, 1200, 1200, 1200, 1200, 800 };

    if (SetChangeFrameAnime(offset, offset->actno)) {
        switch (offset->actno) {
        case 3:
            SetXSpeed(offset, -300 - (Random() & 0xFF));
            SetYSpeed(offset, 50 - (Random() & 0xFF), 20);
            break;
        case 4:
        case 5:
            SetXSpeed(offset, -400 - (Random() & 0x1FF));
            SetYSpeed(offset, 100 - (Random() & 0x1FF), 20);
            break;
        case 6:
            SetXSpeed(offset, (Random() & 0xFF) + 50);
            SetYSpeed(offset, -50 - (Random() & 0xFF), 20);
            break;
        case 7:
        case 8:
            SetXSpeed(offset, (Random() & 0x1FF) + 100);
            SetYSpeed(offset, -100 - (Random() & 0x1FF), 20);
            break;
        case 9:
        case 10:
        case 11: {
            int F;
            int i;
            unsigned short r = (unsigned char)offset->Localid << 8;

            F = (Random() & 0xFF) + 300;
            SetXSpeed(offset, F * G_Sin(r));
            SetYSpeed(offset, F * G_Cos(r), 20);
            i = 3;
            while (i--) {
                offset->posx += offset->xspeed;
                offset->posy += offset->yspeed;
            }
            break;
        }
        case 12:
            SetXSpeed(offset, (Random() & 0x7F) + 120);
            SetYSpeed(offset, -50 - (Random() & 0x7F), 10);
            break;
        }
        offset->scale = scaletb[offset->actno - 3];
        offset->angle = nlArcTan2(GetXMove(offset, -offset->xspeed), -offset->yspeed) - 0x4000;
    }
    if (offset->Mark) {
        offset->gravity = 0;
        offset->xspeed /= 2;
        offset->yspeed /= 2;
        offset->Mark = 0;
    }
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void UkemiEffectDraw(CHARACTER_WORK *offset);

void PLYEFF01Act(CHARACTER_WORK *offset) {
    offset->graphicfunc = UkemiEffectDraw;
    if (offset->ActTimer >= 20) {
        offset->actst |= 0x80000000;
    }
}

void HamonEffectDraw(CHARACTER_WORK *offset);
int TimetoEliminate(CHARACTER_WORK *offset, int time);

void PLYEFFHamonAct(CHARACTER_WORK *offset) {
    offset->graphicfunc = HamonEffectDraw;
    TimetoEliminate(offset, 20);
}

int InitHibanaVertex(CHARACTER_WORK *offset);
void SetHibanaVertex(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int TimetoEliminate(CHARACTER_WORK *offset, int time);
int rand(void);
void HibanaDraw(CHARACTER_WORK *offset);

static void HibanaCnt(CHARACTER_WORK *offset) {
    int i;

    offset->graphicfunc = HibanaDraw;
    TimetoEliminate(offset, 16);
    if (offset->ActTimer >= 16) {
        offset->actst |= 0x80000000;
    }
    if (offset->InstSt & 0x20) {
        if (!InitHibanaVertex(offset)) {
            offset->actst |= 0x80000000;
            return;
        }
        offset->xspeed = (rand() & 0x7FF) - 0x400;
        offset->gravity = 16;
        offset->yspeed = (rand() & 0x7FF) - 0x400;
        i = 16;
        while (i--) {
            SetHibanaVertex(offset);
            offset->posx += offset->xspeed / 16;
            offset->posy += offset->yspeed / 16;
        }
        offset->InstSt &= ~0x20;
    }
    SetJumpControl(offset);
    if (!offset->HitStopTime) {
        SetHibanaVertex(offset);
        offset->xspeed -= offset->xspeed / 20;
    }
    if (!(offset->actst & 0x10)) {
        offset->yspeed = -offset->yspeed - -offset->yspeed / 4;
        offset->xspeed -= offset->xspeed / 4;
    }
}

int InitHibanaVertex(CHARACTER_WORK *offset);
void SetHibanaVertex(CHARACTER_WORK *offset);
void SetJumpControl(CHARACTER_WORK *offset);
int TimetoEliminate(CHARACTER_WORK *offset, int time);
int rand(void);
void HibanaDraw(CHARACTER_WORK *offset);

static void HibanaCnt2(CHARACTER_WORK *offset) {
    int i;

    offset->graphicfunc = HibanaDraw;
    TimetoEliminate(offset, 32);
    if (offset->ActTimer >= 32) {
        offset->actst |= 0x80000000;
    }
    if (offset->InstSt & 0x20) {
        if (!InitHibanaVertex(offset)) {
            offset->actst |= 0x80000000;
            return;
        }
        offset->xspeed = (rand() & 0x7FF) + 0x200;
        offset->gravity = 20;
        offset->yspeed = (rand() & 0x7FF) - 0x600;
        if (offset->dirflag == 1) {
            offset->xspeed = -offset->xspeed;
        }
        i = 16;
        while (i--) {
            SetHibanaVertex(offset);
            offset->posx += offset->xspeed / 16;
            offset->posy += offset->yspeed / 16;
        }
        offset->InstSt &= ~0x20;
    }
    SetJumpControl(offset);
    if (!offset->HitStopTime) {
        SetHibanaVertex(offset);
        offset->xspeed -= offset->xspeed / 40;
    }
    if (!(offset->actst & 0x10)) {
        offset->yspeed = -offset->yspeed - -offset->yspeed / 4;
        offset->xspeed -= offset->xspeed / 4;
    }
}

void CMNEFFActH17_H(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void CMNEFFActH17_V(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void CMNEFFActH172(CHARACTER_WORK *offset) {
    offset->graphicfunc = HitEff172Draw;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void CMNEFFActH173(CHARACTER_WORK *offset) {
    offset->graphicfunc = HitEff173Draw;
    SetChangeFrameAnimeSameActno(offset);
    if (offset->ActTimer >= 11) {
        offset->actst |= 0x80000000;
    }
}

void CMNEFFActH174(CHARACTER_WORK *offset) {
    offset->graphicfunc = HitEff174Draw;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->Localid = Random() & 0xFF;
        offset->scale = (Random() & 0x3F) + 0x40;
        offset->InstSt &= ~0x20;
    }
    if (offset->ActTimer >= 10) {
        offset->actst |= 0x80000000;
    }
}

void CMNEFFActH175(CHARACTER_WORK *offset) {
    offset->graphicfunc = HitEff175Draw;
    SetChangeFrameAnimeSameActno(offset);
    if (offset->ActTimer >= 11) {
        offset->actst |= 0x80000000;
    }
}

void CMNEFFActSlashSplash(CHARACTER_WORK *offset) {
    int r;

    offset->graphicfunc = HitEffSlashSplashDraw;
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    if (SetChangeFrameAnimeSameActno(offset)) {
        unsigned short a = (unsigned char)Random() << 8;

        r = (Random() & 0x1F) * 50 + 600;
        SetXSpeed(offset, G_Sin(a) * r);
        SetYSpeed(offset, G_Cos(a) * r, 0);
        offset->InstSt &= ~0x20;
    }
    if (!IsHitStop(offset)) {
        offset->xspeed -= offset->xspeed / 12;
        offset->yspeed -= offset->yspeed / 12;
    }
    offset->SpriteId = offset->FBSpriteIdTop + 4;
    EndtoEliminate(offset);
}

void CMNEFFActHandFightSplash(CHARACTER_WORK *offset) {
    int r;

    offset->graphicfunc = HitEffHandFightSplashDraw;
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    if (SetChangeFrameAnimeSameActno(offset)) {
        unsigned short a = (unsigned char)Random() << 8;

        r = (Random() & 0x1F) * 50 + 400;
        SetXSpeed(offset, G_Sin(a) * r);
        SetYSpeed(offset, G_Cos(a) * r, 0);
        offset->InstSt &= ~0x20;
    }
    if (!IsHitStop(offset)) {
        offset->xspeed -= offset->xspeed / 10;
        offset->yspeed -= offset->yspeed / 10;
    }
    EndtoEliminate(offset);
}

void CMNEFFActKakuseiSplash(CHARACTER_WORK *offset) {
    int r;
    int i;

    offset->graphicfunc = HitEffKakuseiSplashDraw;
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    if (SetChangeFrameAnimeSameActno(offset)) {
        unsigned short a = (unsigned char)Random() << 8;

        r = (Random() & 0x1F) * 70 + 700;
        SetXSpeed(offset, G_Sin(a) * r);
        SetYSpeed(offset, G_Cos(a) * r, 0);
        offset->InstSt &= ~0x20;
        i = 15;
        while (i--) {
            offset->posx -= offset->xspeed;
            offset->posy -= offset->yspeed;
        }
    }
    EndtoEliminate(offset);
}

void CMNEFFActKakuseiRing(CHARACTER_WORK *offset) {
    offset->graphicfunc = HitEffKakuseiRingDraw;
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->RHitBackx = -0x3800;
    }
    if (!IsHitStop(offset)) {
        offset->HitBackx += 0x300;
        offset->RHitBackx += 0x200;
    }
    EndtoEliminate(offset);
}

void CMNEFFActH17A(CHARACTER_WORK *offset) {
    SetChangeFrameAnime(offset, offset->actno);
    offset->actst |= 0x100000;
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void CMNEFFActH17B(CHARACTER_WORK *offset) {
    offset->graphicfunc = PolySkyDashEffectDraw;
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

void CMNEFFActH17C(CHARACTER_WORK *offset) {
    SetJumpControl(offset);
    offset->actst |= 0x100000;
    SetChangeFrameAnimeSameActno(offset);
    EndtoEliminate(offset);
}

extern unsigned char PG_offySitTbByIdno[19];
extern unsigned char PG_offySkyTbByIdno[19];
extern unsigned char PG_offyStandTbByIdno[19];

void CMNEFFActRomanCancel(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    unsigned short pidno;

    offset->graphicfunc = HitEffRomanCancelDraw;
    if (SetChangeFrameAnimeSameActno(offset)) {
        poffset = offset->parentWork;
        pidno = poffset->idno;
        if (poffset->actst & 0x10) {
            offset->posy -= PG_offySkyTbByIdno[pidno] * 100;
        } else if (poffset->actst & 0x400) {
            offset->posy -= PG_offySitTbByIdno[pidno] * 100;
        } else {
            offset->posy -= PG_offyStandTbByIdno[pidno] * 100;
        }
        offset->HitBackx = (Random() & 0xFF) << 8;
        offset->RHitBackx = (Random() & 0xFF) << 8;
    }
    EndtoEliminate(offset);
}

void CMNEFFActItigekiReady(CHARACTER_WORK *offset) {
    offset->graphicfunc = HitEffItigekiReadyDraw;
    if (SetChangeFrameAnimeSameActno(offset)) {
        offset->angle = Random() << 8;
        offset->scale = 40;
        offset->scaleY = 200;
    }
    if (!IsHitStop(offset)) {
        if (offset->ActTimer < 10) {
            offset->scale += offset->scale / 10;
            offset->scaleY = offset->ActTimer * 40 + 200;
            offset->trans = offset->ActTimer * 20 + 40;
        } else if (offset->ActTimer < 20) {
            offset->scale += offset->scale / 10;
            offset->trans = 255;
            offset->scaleY = (offset->ActTimer - 10) * 30 + 600;
        } else if (offset->ActTimer < 40) {
            offset->scale += offset->scale / 10;
            offset->trans = 255 - (offset->ActTimer - 20) * 12;
        } else {
            offset->actst |= 0x80000000;
        }
    }
}

static void PerfectGuard(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    unsigned short pidno;
    CHARACTER_WORK *offobj;

    offset->graphicfunc = PerfectGuardEffectDraw;
    poffset = offset->parentWork;
    pidno = poffset->idno;
    offset->posx = poffset->posx;
    if (poffset->actst & 0x10) {
        offset->posy = poffset->posy - PG_offySkyTbByIdno[pidno] * 100;
    } else if (poffset->actst & 0x400) {
        offset->posy = poffset->posy - PG_offySitTbByIdno[pidno] * 100;
    } else {
        offset->posy = poffset->posy - PG_offyStandTbByIdno[pidno] * 100;
    }
    if (~poffset->GuardSt & 8) {
        offobj = EffectCreate(offset, 0x74, 0);
        if (offobj) {
            ParentFlagSuccession(offset, offobj);
        }
        offset->actst |= 0x80000000;
        return;
    }
    if (offset->Localid > 0) {
        offset->Localid--;
    }
    if (poffset->GuardSt & 0x20) {
        offset->Localid = 4;
    }
    if (offset->ActTimer == 0) {
        offset->attackst = 0;
        offset->HitBackx = PG_RTbByIdno[pidno];
    } else {
        offset->attackst++;
    }
}

static void PerfectGuard2(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;

    poffset = offset->parentWork;
    offset->graphicfunc = PerfectGuardDisaappearEffectDraw;
    offset->HitBackx = PG_RTbByIdno[poffset->idno];
    TimetoEliminate(offset, 16);
}

static void PLYLIGHTNING(CHARACTER_WORK *offset) {
    offset->graphicfunc = LightningEffectDraw;
    TimetoEliminate(offset, 16);
}

static void PLYLIGHTNING_C(CHARACTER_WORK *offset) {
    offset->graphicfunc = LightningCenterEffectDraw;
    TimetoEliminate(offset, 16);
}

void EFFK25ActDef(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    int t;

    t = (offset->ActTimer + offset->Localid * 8) << 10;
    poffset = offset->parentWork;
    SetChangeFrameAnimeSameActno(offset);
    offset->posx = poffset->posx + G_Sin(t) * 64.0 * 100.0;
    offset->posy = poffset->posy + G_Cos(t) * 16.0 * 100.0 - 16000.0;
}

void EFFK25ActDead(CHARACTER_WORK *offset) {
    SetChangeFrameAnimeSameActno(offset);
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void EFFRYActDef(CHARACTER_WORK *offset) {
    static int angle = 0;

    if (SetChangeFrameAnime(offset, offset->actno)) {
        int r = ((Random() & 0xF) << 8) + 0x1800;
        angle = offset->angle = angle + r;
    }
    EndtoEliminate(offset);
}

float nlCos(unsigned short r);
float nlSin(unsigned short r);

void SlashSpark(CHARACTER_WORK *offset) {
    if (offset->ActTimer == 0) {
        offset->xspeed = nlSin((offset->Localid & 0xFF) << 8) * ((((unsigned short)offset->Localid >> 8) & 0xF) + 2) * 2.0f * 100.0f;
        offset->yspeed = nlCos((offset->Localid & 0xFF) << 8) * ((((unsigned short)offset->Localid >> 8) & 0xF) + 2) * 2.0f * 100.0f;
    }
    offset->posy += offset->yspeed;
    if (offset->ActTimer >= 20) {
        offset->actst |= 0x80000000;
    }
}

static void KAKUSEI(CHARACTER_WORK *offset) {
    unsigned char *p;

    p = (unsigned char *)&offset->Localid;
    offset->graphicfunc = KakuseiEffectDraw;
    SetAnten(offset, 1);
    if (offset->padid == 0) {
        SetForceStopTime(p[0] * 2, p[1] * 2);
    } else {
        SetForceStopTime(p[1] * 2, p[0] * 2);
    }
    EffectCreate_ActNo(offset, 0x77, 0, 1);
    EffectCreate_ActNo(offset, 0x77, 0, 2);
    EffectCreate_ActNo(offset, 0x6F, 0, 9);
    offset->actst |= 0x80000000;
}

static void KAKUSEI2(CHARACTER_WORK *offset) {
    offset->graphicfunc = KakuseiEffectDraw2;
    SetAnten(offset, 1);
    if (offset->ActTimer >= 15) {
        offset->actst |= 0x80000000;
    }
}

static void KAKUSEI3(CHARACTER_WORK *offset) {
    offset->graphicfunc = KakuseiEffectDraw3;
    SetAnten(offset, 1);
    if (!offset->Mark && offset->ActTimer == 30) {
        EffectCreate_ActNo(offset, 0x77, 0, 3);
        offset->Mark = 1;
    }
    if (offset->ActTimer >= 35) {
        offset->actst |= 0x80000000;
    } else if (offset->ActTimer < 11) {
        if (!IsHitStop(offset)) {
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
        }
    } else if (offset->ActTimer < 21) {
        if (!IsHitStop(offset)) {
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
            EffectCreate_ActNo(offset, 0x6F, 0, 8);
        }
    }
}

static void KAKUSEI4(CHARACTER_WORK *offset) {
    offset->graphicfunc = KakuseiEffectDraw4;
    SetAnten(offset, 1);
    if (offset->ActTimer >= 40) {
        offset->actst |= 0x80000000;
    }
}

int abs(int);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
extern short Sit_PosColliHeightByIdno[];
extern short Sit_PosColliWidthByIdno[];
extern short Sky_PosColliBaseHeightByIdno[];
extern short Sky_PosColliHeightByIdno[];
extern short Sky_PosColliWidthByIdno[];
extern short Stand_PosColliHeightByIdno[];
extern short Stand_PosColliWidthByIdno[];

int IsKunsyouHit(CHARACTER_WORK *offset) {
    CHARACTER_WORK *poffset;
    int x;
    int y;
    int w;
    int h;

    poffset = GetParentPlayer(offset);
    x = abs(offset->posx - poffset->posx);
    y = poffset->posy;
    if (poffset->actst & 0x10) {
        h = -Sky_PosColliHeightByIdno[poffset->idno];
        w = Sky_PosColliWidthByIdno[poffset->idno];
        y -= Sky_PosColliBaseHeightByIdno[poffset->idno];
    } else if (poffset->actst & 0x400) {
        h = -Sit_PosColliHeightByIdno[poffset->idno];
        w = Sit_PosColliWidthByIdno[poffset->idno];
    } else {
        h = -Stand_PosColliHeightByIdno[poffset->idno];
        w = Stand_PosColliWidthByIdno[poffset->idno];
    }
    w += 4000;
    h -= 4000;
    y += 5000;
    if (x > w || offset->posy > y) {
        return 0;
    }
    return offset->posy >= y + h;
}

int abs(int);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
int IsActTimeOver(CHARACTER_WORK *offset, int time);
int IsKunsyouHit(CHARACTER_WORK *offset);
int IsLand(CHARACTER_WORK *offset);
void Score_AddByKunsyou(int side, int level);
void Score_AddScorerate(int side, int level);
int Score_GetScorerate(int side);
void SetActNoNext(CHARACTER_WORK *offset, unsigned int no);

void Kunsyou_Level0(CHARACTER_WORK *offset) {
    int dist;

    if (SetChangeFrameAnimeSameActno(offset)) {
        SetYSpeed(offset, -1500, 100);
        SetXSpeed(offset, (Random() & 0x3FF) - 0x200);
    }
    if (!offset->Mark) {
        CHARACTER_WORK *p = GetParentPlayer(offset);

        dist = abs(offset->posx - p->posx);
        if (dist > offset->xspeed * 10) {
            if (offset->posx > p->posx) {
                offset->xspeed -= 20;
            } else {
                offset->xspeed += 20;
            }
        } else {
            offset->xspeed -= offset->xspeed / 10;
        }
    }
    if (IsActTimeOver(offset, 6) && IsKunsyouHit(offset)) {
        Score_AddScorerate(offset->padid, offset->actno);
        Score_AddByKunsyou(offset->padid, offset->actno);
        SetActNoNext(offset, offset->actno * 10 + 6 + Score_GetScorerate(offset->padid));
        return;
    }
    if (IsLand(offset)) {
        if (offset->yspeed > 1000) {
            offset->Mark = 1;
            SetYSpeed(offset, -offset->yspeed / 2, 100);
            offset->xspeed /= 2;
        } else {
            SetYSpeed(offset, 0, 0);
            SetXSpeed(offset, 0);
        }
    }
    SetJumpControl(offset);
    EndtoEliminate(offset);
}

void Score_Level0(CHARACTER_WORK *offset) {
    if (SetChangeFrameAnimeSameActno(offset)) {
        SetYSpeed(offset, -1000, 50);
        offset->scale = 1500;
    }
    SetJumpControl(offset);
    if (offset->yspeed > 0) {
        SetYSpeed(offset, 0, 0);
    }
    TimetoEliminate(offset, 40);
}
