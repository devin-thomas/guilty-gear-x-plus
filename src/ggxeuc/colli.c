#include "ggxeuc/objtypes.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;
} SCREEN_RECT;

typedef struct {
    char pad[0x18];
    u_int flag;
} SCR_STP;

extern SCR_STP *scr_stp;
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
SCREEN_RECT *GetScreenRect(void);
extern CHARACTER_WORK *PlayerWork;
extern CHARACTER_WORK *ObjectWork;
extern int TotalCounter;
typedef struct {
    int flag;
    int x;
    int y;
    int pad[5];
} CREATE_POS;

void Char2ScreenCollision(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    SCREEN_RECT *rect;
    int d;

    if (scr_stp->flag & 0x20) return;
    if (offset->SpriteFlag & 1) return;
    if (offset->ply != NULL && (offset->ply->pactst & 0x1000)) return;
    rect = GetScreenRect();
    offset->actst &= ~0x2000;
    if (offset->posx <= rect->x + 0x6a4) {
        d = rect->x + 0x6a4 - offset->posx;
        offset->posx = rect->x + 0x6a4;
        offset->actst |= 0x2000;
        if (offset->actst & 0x4000) {
            enemy->posx += d;
        }
    }
    if (offset->posx >= rect->x + rect->w - 0x6a4) {
        d = rect->x + rect->w - 0x6a4 - offset->posx;
        offset->posx = rect->x + rect->w - 0x6a4;
        offset->actst |= 0x2000;
        if (offset->actst & 0x4000) {
            enemy->posx += d;
        }
    }
}

void Char2WorldCollision(CHARACTER_WORK *offset) {
    CHARACTER_WORK *enemy = GetEnemyPlayer(offset);
    int d;

    if (scr_stp->flag & 0x2000) return;
    if (offset->SpriteFlag & 1) return;
    if (offset->ply != NULL && (offset->ply->pactst & 0x1000)) return;
    offset->actst &= ~0x800;
    if (offset->posx <= -0x12110) {
        d = -0x12110 - offset->posx;
        offset->posx = -0x12110;
        offset->actst |= 0x800;
        if (offset->actst & 0x4000) {
            enemy->posx += d;
        }
    }
    if (offset->posx >= 0x12110) {
        d = 0x12110 - offset->posx;
        offset->posx = 0x12110;
        offset->actst |= 0x800;
        if (offset->actst & 0x4000) {
            enemy->posx += d;
        }
    }
    if (offset->posy >= 0) {
        offset->posy = 0;
    }
    if (offset->posy <= -0x3a980) {
        offset->posy = -0x3a980;
    }
}

/* matched by Codex */
int GetCenterX(CHARACTER_WORK *offset) {
    int x = offset->posx;
    if (x <= -46000) return -46000 - x;
    if (x >= 46000) return 46000 - x;
    return 0;
}

void CollisionDirectionFilp(CHARACTER_WORK *offset, COLLISION *src, COLLISION *dst) {
    float sx, sy;
    int x;
    u_int attr;

    if (offset->scale > 0) {
        sx = offset->scale / 1000.0f;
    } else {
        sx = 1.0f;
    }
    if (offset->scaleY > 0) {
        sy = offset->scaleY / 1000.0f;
    } else {
        sy = sx;
    }
    if (offset->dirflag == 1) {
        x = -src->x - src->w;
        attr = src->attr;
    } else {
        x = src->x;
        attr = src->attr;
    }
    dst->x = x * sx;
    dst->y = src->y * sy;
    dst->attr = attr;
    dst->w = src->w * sx;
    dst->h = src->h * sy;
}

int IsSpJudjeFlag(u_int flag, u_int val);
int IsBattleDecision(void);
void SetBattleTimeStopFlag(int padid);

int IsHit(CHARACTER_WORK *a, CHARACTER_WORK *b) {
    u_int flag = a->ActHeader.flag;
    u_short gst;

    if (flag & 0x4000) {
        b->GuardSt &= ~8;
    }
    gst = b->GuardSt;
    if (!(gst & 0x1800) && IsSpJudjeFlag(a->ActHeader.flag, 0x20000000)) {
        if (b->ply == NULL) goto ng;
        if (b->ply->MistTime != 0) {
            gst = b->GuardSt &= 0xfff0;
        }
    }
    if (b->ply == NULL) goto ng;
    if (!(gst & 0x800) || a->ply != NULL) {
        if (!(flag & 0x04000000) && (gst & 0x400)) goto ng;
        if ((gst & 0xc) == 0xc) {
        ng:
            return -2;
        }
        if ((flag & 0x400) && (gst & 1)) goto ng;
        if ((flag & 0x800) && (gst & 2)) goto ng;
        if ((flag & 0x1000) && (gst & 4)) goto ng;
    }
judge:
    if (flag & 0x400000) {
        if (IsBattleDecision()) goto ng;
        SetBattleTimeStopFlag(a->padid);
    }
    if (IsSpJudjeFlag(a->ActHeader.flag, 0x20000000)) {
        b->ply->MistTime = 0;
    }
    return -1;
}

int IsSpJudjeFlag(u_int flag, u_int val) {
    return val == (flag & 0x38000000);
}

CHARACTER_WORK *GetChar(int *no) {
    CHARACTER_WORK *p;

    do {
        if (*no < 2) {
            if (TotalCounter & 1) {
                p = PlayerWork;
                if (*no == 0) p = PlayerWork + 1;
            } else {
                p = &PlayerWork[*no];
            }
        } else if (*no < 50) {
            p = &ObjectWork[*no - 2];
        } else {
            return NULL;
        }
        (*no)++;
    } while (!(p->actst & 1));
    return p;
}

CHARACTER_WORK *GetCharObjPriority(int *no) {
    CHARACTER_WORK *p;

    do {
        if (*no < 48) {
            p = &ObjectWork[*no];
        } else if (*no < 50) {
            if (TotalCounter & 1) {
                p = (*no == 48) ? PlayerWork + 1 : PlayerWork;
            } else {
                p = &PlayerWork[*no - 48];
            }
        } else {
            return NULL;
        }
        (*no)++;
    } while (!(p->actst & 1));
    return p;
}

COLLISION *GetColli(CHARACTER_WORK *offset, int *no, u_int mask) {
    COLLISION *c;
    u_int attr;

    for (;;) {
        if (*no == offset->ColliCnt) {
            if (offset->ExColliAddr == NULL) return NULL;
            c = offset->ExColliAddr;
        } else if (*no > offset->ColliCnt) {
            return NULL;
        } else {
            c = &offset->ColliAddr[*no];
        }
        (*no)++;
        attr = c->attr;
        if ((attr & 0xFF) == 3 || (attr & 0xFF) == 6 || attr == 4 || attr == 5) continue;
        if (attr & mask) return c;
        if (attr == 0 && mask == 1) return c;
    }
}

void GetHitPos(COLLIPOS *pos, CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb) {
    int ax1, ay1, ax2, ay2;
    int bx1, by1, bx2, by2;
    int l, t, r, btm;

    ax1 = a->posx + ca->x * 100;
    ay1 = a->posy + ca->y * 100;
    bx1 = b->posx + cb->x * 100;
    by1 = b->posy + cb->y * 100;
    ax2 = ax1 + ca->w * 100;
    ay2 = ay1 + ca->h * 100;
    bx2 = bx1 + cb->w * 100;
    by2 = by1 + cb->h * 100;
    l = (bx1 < ax1) ? ax1 : bx1;
    t = (by1 < ay1) ? ay1 : by1;
    r = (ax2 < bx2) ? ax2 : bx2;
    btm = (ay2 < by2) ? ay2 : by2;
    pos->x = (l + r) / 2;
    pos->y = (t + btm) / 2;
}

int CheckColliDamOverFlow(CHARACTER_WORK *offset) {
    int i;

    if (offset->DamColliFlag == 0) return 0;
    for (i = 0; i < 1; i++) {
        if (offset->DamColliAddr[i] == NULL) return 0;
    }
    return 1;
}

void HitColliAddrClear(CHARACTER_WORK *offset) {
    int i;
    for (i = 1; i >= 0; i--) offset->HitColliAddr[i] = NULL;
}

void DamColliAddrClear(CHARACTER_WORK *offset) {
    int i;
    for (i = 2; i >= 0; i--) offset->DamColliAddr[i] = NULL;
}

void GetHitPos(COLLIPOS *pos, CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *offset, int id, CREATE_POS *pos);
CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *offset, int id, CREATE_POS *pos, int actno);

void SetColliSousaiFlag(CHARACTER_WORK *atk, COLLISION *ca, CHARACTER_WORK *dam, COLLISION *cd) {
    COLLIPOS pos;
    CREATE_POS cp;
    CHARACTER_WORK *p;
    int i;

    for (i = 1; i >= 0; i--) atk->HitColliAddr[i] = NULL;
    if (atk->parentFlag & 0x80) {
        p = atk->parentWork;
        for (i = 1; i >= 0; i--) p->HitColliAddr[i] = NULL;
    }
    for (i = 1; i >= 0; i--) dam->HitColliAddr[i] = NULL;
    if (dam->parentFlag & 0x80) {
        p = dam->parentWork;
        for (i = 1; i >= 0; i--) p->HitColliAddr[i] = NULL;
    }
    atk->HitColliFlag = -10;
    atk->HitColliAddr[0] = dam;
    if (atk->parentFlag & 0x80) ((CHARACTER_WORK *)atk->parentWork)->HitColliFlag = -10;
    dam->HitColliFlag = -10;
    dam->HitColliAddr[0] = atk;
    if (dam->parentFlag & 0x80) ((CHARACTER_WORK *)dam->parentWork)->HitColliFlag = -10;
    GetHitPos(&pos, atk, ca, dam, cd);
    cp.flag = 1;
    cp.x = pos.x;
    cp.y = pos.y;
    EffectCreate(atk, 0x75, &cp);
    EffectCreate_ActNo(atk, 0x75, &cp, 1);
}

void GetHitPos(COLLIPOS *pos, CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb);

void SetBallHitFlag(CHARACTER_WORK *atk, COLLISION *ca, CHARACTER_WORK *dam, COLLISION *cd) {
    int i;

    for (i = 1; i >= 0; i--) atk->HitColliAddr[i] = NULL;
    atk->DamColliFlag = -12;
    i = 2;
    if (dam->idno == 0x22 && dam->actno == 0x1B) dam->HitColliFlag = 20;
    GetHitPos(&atk->DamColliPos[i], atk, ca, dam, cd);
}

int World2ScreenX(int x);
int World2ScreenY(int y);

int JudgeColliRect(CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb) {
    int ax1, ay1, ax2, ay2;
    int bx1, by1, bx2, by2;

    ax1 = World2ScreenX(a->posx) + ca->x;
    ay1 = World2ScreenY(a->posy) + ca->y;
    ax2 = ax1 + ca->w;
    ay2 = ay1 + ca->h;
    bx1 = World2ScreenX(b->posx) + cb->x;
    by1 = World2ScreenY(b->posy) + cb->y;
    bx2 = bx1 + cb->w;
    by2 = by1 + cb->h;
    if (ax1 < bx2 && ay1 < by2 && bx1 < ax2 && by1 < ay2) return 1;
    return 0;
}

int GetCollisionTop(CHARACTER_WORK *offset);
int IsSpJudjeFlag(u_int flag, u_int val);
COLLISION *GetColli(CHARACTER_WORK *offset, int *no, u_int mask);
CHARACTER_WORK *GetChar(int *no);
void CollisionDirectionFilp(CHARACTER_WORK *offset, COLLISION *src, COLLISION *dst);
int JudgeColliRect(CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb);
void SetColliSousaiFlag(CHARACTER_WORK *atk, COLLISION *ca, CHARACTER_WORK *dam, COLLISION *cd);

void SousaiCollisionExecute(int mode) {
    COLLISION hc;
    COLLISION dc;
    int hitno, damno, objno, charno;
    CHARACTER_WORK *atk;
    CHARACTER_WORK *dam;
    CHARACTER_WORK *p;
    COLLISION *hcol;
    COLLISION *dcol;
    int a, b;

    charno = 0;
    objno = 0;
    hitno = 0;
    damno = 0;
    while ((atk = GetChar(&charno)) != NULL) {
        GetCollisionTop(atk);
        if (atk->idno < 0x20) {
            if (atk->HitColliFlag) continue;
            if (atk->DamColliFlag) continue;
            if (IsSpJudjeFlag(atk->ActHeader.flag, 0x08000000)) continue;
            if (IsSpJudjeFlag(atk->ActHeader.flag, 0x10000000)) continue;
        } else {
            if (!(atk->parentFlag & 0x80)) continue;
            p = atk->parentWork;
            if (p->HitColliFlag) continue;
            if (p->DamColliFlag) continue;
        }
        if (atk->actst & 0x40) continue;
        hitno = 0;
        if (!GetColli(atk, &hitno, 1)) continue;
        objno = 0;
        while ((dam = GetChar(&objno)) != NULL) {
            if (atk == dam) continue;
            if ((atk->actst & 2) && (dam->actst & 2)) continue;
            if ((atk->actst & 4) && (dam->actst & 4)) continue;
            if (dam->idno < 0x20) {
                if (dam->HitColliFlag) continue;
                if (dam->DamColliFlag) continue;
                if (IsSpJudjeFlag(dam->ActHeader.flag, 0x08000000)) continue;
                if (IsSpJudjeFlag(dam->ActHeader.flag, 0x10000000)) continue;
            } else {
                if (!(dam->parentFlag & 0x80)) continue;
                p = dam->parentWork;
                if (p->HitColliFlag) continue;
                if (p->DamColliFlag) continue;
            }
            if (dam->actst & 0x40) continue;
            damno = 0;
            if (!GetColli(dam, &damno, 1)) continue;
            a = (atk->ActHeader.lvflag >> 6) & 1;
            b = (dam->ActHeader.lvflag >> 6) & 1;
            if (mode) {
                if (!a) continue;
                if (!b) continue;
            }
            hitno = 0;
            while ((hcol = GetColli(atk, &hitno, 1)) != NULL) {
                damno = 0;
                while ((dcol = GetColli(dam, &damno, 1)) != NULL) {
                    CollisionDirectionFilp(atk, hcol, &hc);
                    CollisionDirectionFilp(dam, dcol, &dc);
                    if (JudgeColliRect(atk, &hc, dam, &dc)) {
                        SetColliSousaiFlag(atk, &hc, dam, &dc);
                        goto next;
                    }
                }
            }
        }
    next:;
    }
}

int GetCollisionTop(CHARACTER_WORK *offset);
COLLISION *GetColli(CHARACTER_WORK *offset, int *no, u_int mask);
CHARACTER_WORK *GetChar(int *no);
void CollisionDirectionFilp(CHARACTER_WORK *offset, COLLISION *src, COLLISION *dst);
int JudgeColliRect(CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb);
void SetBallHitFlag(CHARACTER_WORK *atk, COLLISION *ca, CHARACTER_WORK *dam, COLLISION *cd);
float nlSqrt(float x);

void BallCollisionExecute(void) {
    COLLISION hc;
    COLLISION dc;
    int hitno, damno, objno, charno;
    CHARACTER_WORK *atk;
    CHARACTER_WORK *dam;
    COLLISION *hcol;
    COLLISION *dcol;
    float speed;

    charno = 0;
    objno = 0;
    hitno = 0;
    damno = 0;
    while ((atk = GetChar(&charno)) != NULL) {
        GetCollisionTop(atk);
        if (!(atk->attackst & 0x8000)) continue;
        hitno = 0;
        if (!GetColli(atk, &hitno, 1)) continue;
        objno = 0;
        while ((dam = GetChar(&objno)) != NULL) {
            if (atk == dam) continue;
            if ((atk->actst & 2) && (dam->actst & 4)) continue;
            if ((atk->actst & 4) && (dam->actst & 2)) continue;
            if (!(dam->attackst & 0x4000)) continue;
            damno = 0;
            if (!GetColli(dam, &damno, 1)) continue;
            hitno = 0;
            while ((hcol = GetColli(atk, &hitno, 2)) != NULL) {
                damno = 0;
                while ((dcol = GetColli(dam, &damno, 1)) != NULL) {
                    CollisionDirectionFilp(atk, hcol, &hc);
                    CollisionDirectionFilp(dam, dcol, &dc);
                    if (JudgeColliRect(atk, &hc, dam, &dc)) {
                        SetBallHitFlag(atk, &hc, dam, &dc);
                        if (dam->ply) {
                            atk->Localid = dam->ply->PersonalWork[0].ub[0];
                            atk->CommandFlag = dam->ply->PersonalWork[0].ub[3];
                            atk->HitPoint = dam->ply->PersonalWork[0].ub[2];
                            atk->dirflag = dam->dirflag;
                            goto next;
                        } else {
                            speed = nlSqrt(dam->xspeed * dam->xspeed + dam->yspeed * dam->yspeed);
                            if (TotalCounter & 1) {
                                atk->Localid = dam->Localid + 16;
                                dam->Localid = dam->Localid - 16;
                            } else {
                                atk->Localid = dam->Localid - 16;
                                dam->Localid = dam->Localid + 16;
                            }
                            atk->HitPoint = speed / 100.0f;
                        }
                        atk->dirflag = dam->dirflag;
                        goto next;
                    }
                }
            }
        }
    next:;
    }
}

CHARACTER_WORK *GetChar(int *no);

u_int GetOtherAttackFlag(CHARACTER_WORK *offset) {
    int no = 0;
    u_int flag = 0;
    CHARACTER_WORK *p;
    int d;

    while ((p = GetChar(&no)) != NULL) {
        if (p->actst & 0x40) continue;
        if (!(p->ActHeader.flag & 0x2000)) continue;
        if (offset == p) continue;
        if ((offset->actst & 6) != 6) {
            if ((offset->actst & 2) && (p->actst & 2)) continue;
            if ((offset->actst & 4) && (p->actst & 4)) continue;
        }
        d = offset->posx - p->posx;
        if (offset->dirflag == 1) d = -d;
        if (d > 40000) continue;
        if (d < -10000) continue;
        if (p->ActHeader.flag & 0x400) {
            flag |= 1;
            if (p->ActHeader.flag & 0x20) flag |= 0x100;
        }
        if (p->ActHeader.flag & 0x800) flag |= 2;
        if (p->ActHeader.flag & 0x1000) flag |= 4;
    }
    return flag;
}

CHARACTER_WORK *GetChar(int *no);

u_int GetOtherAttackFlagNoDist(CHARACTER_WORK *offset) {
    int no = 0;
    u_int flag = 0;
    CHARACTER_WORK *p;
    int d;

    while ((p = GetChar(&no)) != NULL) {
        if (p->actst & 0x40) continue;
        if (!(p->ActHeader.flag & 0x2000)) continue;
        if (offset == p) continue;
        if ((offset->actst & 6) != 6) {
            if ((offset->actst & 2) && (p->actst & 2)) continue;
            if ((offset->actst & 4) && (p->actst & 4)) continue;
        }
        d = offset->posx - p->posx;
        if (offset->dirflag == 1) d = -d;
        if (d < -10000) continue;
        if (p->ActHeader.flag & 0x400) {
            flag |= 1;
            if (p->ActHeader.flag & 0x20) flag |= 0x100;
        }
        if (p->ActHeader.flag & 0x800) flag |= 2;
        if (p->ActHeader.flag & 0x1000) flag |= 4;
    }
    return flag;
}

CHARACTER_WORK *GetChar(int *no);

u_int GetOtherAttackGFlag(CHARACTER_WORK *offset) {
    int no = 0;
    u_int flag = 0;
    CHARACTER_WORK *p;
    int d;

    while ((p = GetChar(&no)) != NULL) {
        if (!(p->ActHeader.flag & 0x2000)) continue;
        if (offset == p) continue;
        if ((offset->actst & 6) != 6) {
            if ((offset->actst & 2) && (p->actst & 2)) continue;
            if ((offset->actst & 4) && (p->actst & 4)) continue;
        }
        d = offset->posx - p->posx;
        if (offset->dirflag == 1) d = -d;
        if (d > 60000) continue;
        if (d < -10000) continue;
        if (p->ActHeader.flag & 0x400) {
            flag |= 1;
            if (p->ActHeader.flag & 0x20) flag |= 0x100;
        }
        if (p->ActHeader.flag & 0x800) flag |= 2;
        if (p->ActHeader.flag & 0x1000) flag |= 4;
    }
    return flag;
}

COLLISION *Get_GCLColli(CHARACTER_WORK *offset, int no);
void SetThrowDist(int dist);
extern int ColliTouchFlag;
extern short Sky_PosColliBaseHeightByIdno[];
extern short Sky_PosColliWidthByIdno[];
extern short Sky_PosColliHeightByIdno[];
extern short Sit_PosColliWidthByIdno[];
extern short Sit_PosColliHeightByIdno[];
extern short Stand_PosColliWidthByIdno[];
extern short Stand_PosColliHeightByIdno[];

void CharPosCollisionExecute(void) {
    CHARACTER_WORK *l;
    CHARACTER_WORK *r;
    COLLISION *lc;
    COLLISION *rc;
    int lh, ly, ry, lw, rw, rh;
    int lx, rx, d, side, mid;

    l = PlayerWork;
    r = PlayerWork + 1;
    if (PlayerWork[0].posx > PlayerWork[1].posx) {
        l = PlayerWork + 1;
        r = PlayerWork;
    }
    ly = -l->posy;
    ry = -r->posy;
    if ((l->actst & 0x10) && (~l->actst & 0x1000000)) {
        lw = Sky_PosColliWidthByIdno[l->idno];
        lh = Sky_PosColliHeightByIdno[l->idno];
        ly += Sky_PosColliBaseHeightByIdno[l->idno];
    } else if ((l->actst & 0x400) && (~l->actst & 0x1000000)) {
        lw = Sit_PosColliWidthByIdno[l->idno];
        lh = Sit_PosColliHeightByIdno[l->idno];
    } else {
        lw = Stand_PosColliWidthByIdno[l->idno];
        lh = Stand_PosColliHeightByIdno[l->idno];
    }
    if ((r->actst & 0x10) && (~r->actst & 0x1000000)) {
        rw = Sky_PosColliWidthByIdno[r->idno];
        rh = Sky_PosColliHeightByIdno[r->idno];
        ry += Sky_PosColliBaseHeightByIdno[r->idno];
    } else if ((r->actst & 0x400) && (~r->actst & 0x1000000)) {
        rw = Sit_PosColliWidthByIdno[r->idno];
        rh = Sit_PosColliHeightByIdno[r->idno];
    } else {
        rw = Stand_PosColliWidthByIdno[r->idno];
        rh = Stand_PosColliHeightByIdno[r->idno];
    }
    lc = Get_GCLColli(l, 4);
    rc = Get_GCLColli(r, 4);
    if (lc) {
        if (l->dirflag == 0) lw = (lc->x + lc->w) * 100;
        else lw = -lc->x * 100;
    }
    if (rc) {
        if (r->dirflag == 1) rw = (rc->x + rc->w) * 100;
        else rw = -rc->x * 100;
    }
    lx = l->posx + lw;
    if (l->posx < 0) side = 0; else side = 1;
    rx = r->posx - rw;
    d = rx - lx;
    SetThrowDist(d);
    if ((l->actst | r->actst) & 0x104000) return;
    SetThrowDist(d);
    if (ly < ry) {
        if (ly + lh < ry) return;
    } else {
        if (ry + rh < ly) return;
    }
    if (!(rx < lx)) return;
    ColliTouchFlag |= 1;
    if (l->actst & r->actst & 0x800) {
        if (l->dirflag == r->dirflag) {
            if ((l->actst & 0x10) == (r->actst & 0x10)) {
                if (l->ply->lastposx < 0) goto push_r;
                if (r->ply->lastposx != 0) goto push_r;
                goto push_l;
            }
            if (l->actst & 0x10) goto push_l;
            goto push_r;
        }
        if (side == 0) {
            if (l->dirflag == 1) goto push_r;
            goto push_l;
        }
        if (side == 1) {
            if (l->dirflag == 0) goto push_r;
            goto push_l;
        }
    }
    if (l->actst & 0x800) goto push_r;
    if (r->actst & 0x800) goto push_l;
    mid = (lx + rx) / 2;
    l->posx = mid - lw;
    r->posx = mid + rw;
    return;
push_l:
    if (l->posx < 0) l->posx = r->posx + lw + rw;
    else l->posx = r->posx - lw - rw;
    return;
push_r:
    if (r->posx < 0) r->posx = l->posx + lw + rw;
    else r->posx = l->posx - lw - rw;
}

extern int ColliTouchFlag;

void TouchTimeExecute(void) {
    if (ColliTouchFlag) {
        PlayerWork[0].ply->EnemyTouch++;
        PlayerWork[1].ply->EnemyTouch++;
    } else {
        PlayerWork[0].ply->EnemyTouch = 0;
        PlayerWork[1].ply->EnemyTouch = 0;
    }
    ColliTouchFlag = 0;
}

void SousaiCollisionExecute(int mode);
void ObjCollisionExecute(int mode);
void BallCollisionExecute(void);

void CollisionExecute(void) {
    SousaiCollisionExecute(1);
    ObjCollisionExecute(0);
    ObjCollisionExecute(1);
    BallCollisionExecute();
    SousaiCollisionExecute(0);
}

void SetColliHitFlag(CHARACTER_WORK *atk, COLLISION *ca, CHARACTER_WORK *dam, COLLISION *cd, int flag) {
    CHARACTER_WORK *p;
    int i;

    if (dam->ply != NULL && (dam->ply->pactst & 0x2000)) {
        if (atk->ply == NULL && (atk->ActHeader.flag2 & 2)) {
            atk->actst &= ~6;
            atk->actst |= dam->actst & 6;
            atk->xspeed = -atk->xspeed;
            atk->dirflag = !atk->dirflag;
            return;
        }
    }
    if (atk->ply == NULL && (atk->ActHeader.flag2 & 4)) {
        if (dam->ply != NULL) {
            dam->ply->MistTime = 180;
            atk->HitColliFlag = -1;
        }
        return;
    }

    if (atk->HitColliFlag == 0) HitColliAddrClear(atk);
    if (atk->parentFlag & 0x80) {
        p = atk->parentWork;
        if (p->HitColliFlag == 0) HitColliAddrClear(p);
    }
    atk->HitColliFlag = -1;
    for (i = 0; i < 2; i++) {
        if (atk->HitColliAddr[i] == NULL) {
            atk->HitColliAddr[i] = dam;
            atk->HitColliFlag = IsHit(atk, dam);
            break;
        }
    }
    if (atk->parentFlag & 0x80) {
        p = atk->parentWork;
        for (i = 0; i < 2; i++) {
            if (p->HitColliAddr[i] == NULL) {
                p->HitColliAddr[i] = dam;
                p->HitColliFlag = IsHit(p, dam);
                break;
            }
        }
    }
    if (flag) return;

    if (dam->DamColliFlag == 0) DamColliAddrClear(dam);
    if (dam->parentFlag & 0x80) {
        p = dam->parentWork;
        if (p->DamColliFlag == 0) DamColliAddrClear(p);
    }
    if (IsSpJudjeFlag(atk->ActHeader.flag, 0x08000000)) dam->DamColliFlag = 10;
    else dam->DamColliFlag = 1;
    for (i = 0; i < 3; i++) {
        if (dam->DamColliAddr[i] == NULL) {
            dam->DamColliAddr[i] = atk;
            dam->DamActHeader[i] = atk->ActHeader;
            GetHitPos(&dam->DamColliPos[i], atk, ca, dam, cd);
            if (i == 0) *dam->DamParam = *atk->HitParam;
            break;
        }
    }
    if (dam->parentFlag & 0x80) {
        p = dam->parentWork;
        if (IsSpJudjeFlag(atk->ActHeader.flag, 0x08000000)) p->DamColliFlag = 10;
        else p->DamColliFlag = 1;
        for (i = 0; i < 3; i++) {
            if (p->DamColliAddr[i] == NULL) {
                p->DamColliAddr[i] = atk;
                p->DamActHeader[i] = atk->ActHeader;
                GetHitPos(&p->DamColliPos[i], atk, ca, dam, cd);
                if (i == 0) *p->DamParam = *atk->HitParam;
                break;
            }
        }
    }
}

int GetCollisionTop(CHARACTER_WORK *offset);
int IsSpJudjeFlag(u_int flag, u_int val);
COLLISION *GetColli(CHARACTER_WORK *offset, int *no, u_int mask);
CHARACTER_WORK *GetChar(int *no);
CHARACTER_WORK *GetCharObjPriority(int *no);
int CheckColliDamOverFlow(CHARACTER_WORK *offset);
int GetBattleTimeStopFlag(int no);
void CollisionDirectionFilp(CHARACTER_WORK *offset, COLLISION *src, COLLISION *dst);
int JudgeColliRect(CHARACTER_WORK *a, COLLISION *ca, CHARACTER_WORK *b, COLLISION *cb);
void SetColliHitFlag(CHARACTER_WORK *atk, COLLISION *ca, CHARACTER_WORK *dam, COLLISION *cd, int flag);

void ObjCollisionExecute(int flag) {
    COLLISION aCData;
    COLLISION eCData;
    int aCno, eCno, eno, ano;
    CHARACTER_WORK *aoff;
    CHARACTER_WORK *eoff;
    COLLISION *aColli;
    COLLISION *eColli;
    u_int CheckFlag, EColliFlag;
    int ashotlevel, slv;

    ano = 0;
    eno = 0;
    aCno = 0;
    eCno = 0;
    while ((aoff = GetChar(&ano)) != NULL) {
        GetCollisionTop(aoff);
        if (flag == 1) {
            if (!IsSpJudjeFlag(aoff->ActHeader.flag, 0x08000000)) continue;
            if (aoff->DamColliFlag) continue;
        } else if (IsSpJudjeFlag(aoff->ActHeader.flag, 0x08000000)) continue;
        if (aoff->actst & 0x40) continue;
        if (aoff->HitColliFlag == -10) continue;
        aCno = 0;
        if (!GetColli(aoff, &aCno, 1)) continue;
        CheckFlag = (aoff->ActHeader.lvflag >> 4) & 3;
        if (CheckFlag) {
            if (aoff->HitColliFlag) continue;
        } else {
            EColliFlag = 2; /* dead store (flow deletes it): keeps the then-arm jump, so the reverse-hit block lands between the two eno = 0 copies */
        }
        eno = 0;
        while ((eoff = GetCharObjPriority(&eno)) != NULL) {
            if (aoff == eoff) continue;
            if ((aoff->actst & 2) && (eoff->actst & 2)) continue;
            if ((aoff->actst & 4) && (eoff->actst & 4)) continue;
            if (CheckColliDamOverFlow(eoff)) continue;
            if (flag == 1 && !(eoff->actst & 0x10)) continue;
            if (IsSpJudjeFlag(aoff->ActHeader.flag, 0x10000000)) {
                if (eoff->ply == NULL) continue;
                if (eoff->actst & 0x10) continue;
            } else if (IsSpJudjeFlag(aoff->ActHeader.flag, 0x18000000) && !GetBattleTimeStopFlag(aoff->padid)) {
                if (eoff->actst & 0x20) continue;
            }
            if (aoff->ActHeader.flag & 0x40) {
                if (eoff->ply == NULL) continue;
                if (!(aoff->ActHeader.flag & 0x80000000)) {
                    if (eoff->actst & 0x800010) continue;
                    if (eoff->ply->notThrowTime) continue;
                }
            }
            if (eoff->actst & 0x4000) {
                if (aoff->ply == NULL) continue;
            }
            if (GetBattleTimeStopFlag(eoff->padid)) {
                if (aoff->ply != NULL) continue;
                if (!(aoff->ActHeader.flag2 & 0x10)) continue;
            }
            if (aoff->ply == NULL && (aoff->ActHeader.flag2 & 0x20)) {
                if (GetBattleTimeStopFlag(-1)) continue;
            }
            ashotlevel = CheckFlag;
            eCno = 0;
            if (ashotlevel) {
                slv = (eoff->ActHeader.lvflag >> 4) & 3;
                if (slv == 2) continue;
                if (slv == 1) {
                    if (slv >= ashotlevel) ashotlevel = 0;
                    EColliFlag = 1;
                } else {
                    EColliFlag = 2;
                    ashotlevel = 0;
                }
            } else {
                EColliFlag = 2;
            }
            if (EColliFlag == 2) {
                if (eoff->actst & 0x80) continue;
                if (eoff->actst & 0x20000) continue;
            } else {
                if (eoff->actst & 0x40) continue;
            }
            if (!GetColli(eoff, &eCno, EColliFlag)) continue;
            aCno = 0;
            while ((aColli = GetColli(aoff, &aCno, 1)) != NULL) {
                eCno = 0;
                while ((eColli = GetColli(eoff, &eCno, EColliFlag)) != NULL) {
                    CollisionDirectionFilp(aoff, aColli, &aCData);
                    CollisionDirectionFilp(eoff, eColli, &eCData);
                    if (JudgeColliRect(aoff, &aCData, eoff, &eCData)) {
                        if (ashotlevel) {
                            SetColliHitFlag(eoff, &eCData, aoff, &aCData, 1);
                        } else {
                            SetColliHitFlag(aoff, &aCData, eoff, &eCData, 0);
                        }
                        goto next;
                    }
                }
            }
        }
    next:;
    }
}
