#include "generated/g_graph.h"

void SpriteIDInc(CHARACTER_WORK *offset)
{
    offset->SpriteId = offset->SpriteIdIdx;
    offset->SpriteIdIdx++;
    if (offset->SpriteIdIdx >= offset->SpriteIdMax) {
        offset->SpriteIdIdx = 0;
    }
}

void InitAfterImage(void);

void G_GraphicInit(void)
{
    InitAfterImage();
}

void DrawParent(CHARACTER_WORK *offset)
{
    CHARACTER_WORK *poffset;
    int posx;
    int posy;
    int trans;

    poffset = offset->parentWork;
    if (poffset) {
        trans = offset->trans;
        posx = poffset->posx;
        poffset->posx = offset->posx;
        posy = poffset->posy;
        poffset->posy = offset->posy;
        poffset->trans = offset->trans;
        GraphicExecute_Player(poffset, -1);
        poffset->posx = posx;
        poffset->posy = posy;
        offset->trans = trans;
    }
}

extern CHARACTER_WORK *PlayerWork;
extern CHARACTER_WORK *ObjectWork;
extern CHARACTER_WORK *EffectWork;
extern CHARACTER_WORK *CockpitWork;
extern GAME_INFOMATION g_info;
float GetObjScale(void);
void gCurrentColorClear(void);
void PXF_PalReset(void);
int IsDemoBattle(void);

void GraphicExecuteRoot(void)
{
    int i;
    CHARACTER_WORK *offset;

    CScale = GetObjScale();
    gCurrentColorClear();
    for (i = 0; i < 2; i++) {
        PXF_PalReset();
        offset = &PlayerWork[i];
        if (offset && (offset->actst & 9) == 9) {
            GraphicExecute_Player(offset, i);
        }
    }
    gCurrentColorClear();
    for (i = 0; i < 48; i++) {
        PXF_PalReset();
        offset = &ObjectWork[i];
        if (offset && (offset->actst & 9) == 9) {
            if (offset->graphicfunc) {
                ((void (*)(CHARACTER_WORK *))offset->graphicfunc)(offset);
                gCurrentColorClear();
            } else {
                GraphicExecute(offset);
            }
        }
    }
    for (i = 0; i < 160; i++) {
        PXF_PalReset();
        offset = &EffectWork[i];
        if (offset && (offset->actst & 9) == 9) {
            if (offset->graphicfunc) {
                ((void (*)(CHARACTER_WORK *))offset->graphicfunc)(offset);
                gCurrentColorClear();
            } else {
                GraphicExecute(offset);
            }
        }
    }
    gCurrentColorClear();
    if (!(g_info.BattleControlFlag & 2) && !IsDemoBattle()) {
        for (i = 0; i < 16; i++) {
            PXF_PalReset();
            offset = &CockpitWork[i];
            if (offset && (offset->actst & 1)) {
                if (offset->graphicfunc) {
                    ((void (*)(CHARACTER_WORK *))offset->graphicfunc)(offset);
                } else {
                    GraphicExecute(offset);
                }
            }
        }
        PXF_PalReset();
    }
}

static NLpoint2 SpriteXYSv __attribute__((aligned(16)));
static NLpoint2 CorePos __attribute__((aligned(16)));
void SetBlendingMode(short mode);
void nlDispStripSetPalette(int d);
void gSetObjColorOffset(unsigned int val);
unsigned int gStripGetFadeColor(unsigned int color);
void gSpriteDrawUVAngle(NL_SPR_ARG *sprite, float *core, u_short angle);
void gSpriteDrawUV(NL_SPR_ARG *sprite);
void gSpriteDraw(NL_SPR_ARG *sprite);
void gColPush(unsigned int color);
void gColPop(void);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
char Set_GCLVRectNext(CHARACTER_WORK *offset);

void GraphicExecute(CHARACTER_WORK *offset)
{
    NL_SPR_ARG sprite;

    CellMain(offset);
    if (offset->CellTop == 0) {
        return;
    }
    if ((offset->SpriteFlag & 0x800) && (g_info.BattleControlFlag & 8)) {
        return;
    }
    SetSpriteStruct(offset, &sprite);
    if (sprite.trnsl <= 0.01) {
        gCurrentColorClear();
        return;
    }
    SetBlendingMode(offset->transmode);
    if (offset->PaletteFlag) {
        nlDispStripSetPalette(offset->padid * 16 + offset->LocalPalette);
    } else {
        PXF_PalReset();
    }
    if (offset->SpriteFlag & 0x800200) {
        gSetObjColorOffset(gStripGetFadeColor(0xC0C0C0));
    }
    if (offset->ColliNo != -1) {
        do {
            SetSpriteStructByVRect(offset, &sprite);
            if (offset->angle) {
                gSpriteDrawUVAngle(&sprite, (float *)&CorePos, offset->angle);
            } else {
                gSpriteDrawUV(&sprite);
            }
        } while (Set_GCLVRectNext(offset) != -1);
    } else {
        gSpriteDraw(&sprite);
    }
    if (offset->SpriteFlag & 0x884200) {
        gSetObjColorOffset(0);
    }
    gCurrentColorClear();
    if (offset->SpriteFlag & 0x200000) {
        gColPush(0);
        SetSpriteStructShadow(offset, &sprite);
        if (offset->ColliNo != -1) {
            do {
                SetSpriteStructByVRectShadow(offset, &sprite);
                gSpriteDrawUV(&sprite);
            } while (Set_GCLVRectNext(offset) != -1);
        } else {
            gSpriteDraw_NaomiLibVersion(&sprite);
        }
        gColPop();
    }
}

extern unsigned int TotalCounter;
CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void JY_CradDraw(NL_SPR_ARG *sprite, int acttimer);
void SpriteDrawSph(NL_SPR_ARG *sprite, int *sphaddr);

void GraphicExecute_Playersub(CHARACTER_WORK *offset, NL_SPR_ARG *sprite)
{
    CHARACTER_WORK *eoffset;
    float f;

    eoffset = GetEnemyPlayer(offset);
    SetSpriteStruct(offset, sprite);
    if (offset->SpriteFlag & 0x10000) {
        f = TotalCounter & 0xF;
        if (TotalCounter & 0x10) {
            f = 16.0f - f;
        }
        f = f * 0.03125;
        sprite->trnsl *= (float)(f + 0.1);
    }
    if (offset->ColliNo != -1) {
        if (offset->PaletteFlag & 2) {
            nlDispStripSetPalette(eoffset->PalId);
        } else {
            nlDispStripSetPalette(offset->PalId);
        }
        do {
            SetSpriteStructByVRect(offset, sprite);
            if (offset->ply->pactst & 0x800) {
                JY_CradDraw(sprite, offset->ActTimer);
            } else if (offset->angle) {
                gSpriteDrawUVAngle(sprite, (float *)&CorePos, offset->angle);
            } else {
                gSpriteDrawUV(sprite);
            }
        } while (Set_GCLVRectNext(offset) != -1);
        return;
    }
    if (offset->PaletteFlag & 2) {
        nlDispStripSetPalette(eoffset->PalId);
    } else {
        nlDispStripSetPalette(offset->PalId);
    }
    SpriteDrawSph(sprite, offset->SphAddr);
}

extern int COMMON_PALETTE;
void PXF_GetSize(int id, int *x, int *y);
void SetPVRMinSize(unsigned short x, unsigned short y);
void SetCurrentPalette(char *src);
int IST_Load(int *texadr, int sprid, int palid);
int *GetSphByIST(char *IstAddr);
void PXF_Free(int id, int pal);
void Set_GCL(CHARACTER_WORK *offset);

void CellMain(CHARACTER_WORK *offset)
{
    int x;
    int y;
    int *p;
    int i;

    if (offset->ImageNo == offset->LastImageNo) {
        Set_GCL(offset);
        offset->corey += offset->Imageh - 0x100;
    } else if (offset->FBSpriteIdTop) {
        offset->SpriteId = offset->ImageNo + offset->FBSpriteIdTop;
        Set_GCL(offset);
        PXF_GetSize(offset->SpriteId, &x, &y);
        offset->Imagew = x;
        offset->corey += y - 0x100;
        offset->Imageh = y;
        offset->LastImageNo = offset->ImageNo;
        return;
    } else {
        if (offset->CellTop == 0) {
            return;
        }
        SpriteIDInc(offset);
        if (offset->TexSizeMax >> 4) {
            SetPVRMinSize(1 << (offset->TexSizeMax & 0xF), 1 << (offset->TexSizeMax >> 4));
        } else {
            SetPVRMinSize(1 << offset->TexSizeMax, 1 << offset->TexSizeMax);
        }
        if (COMMON_PALETTE) {
            SetCurrentPalette(offset->CommonPalette);
        } else {
            p = (int *)((int *)offset->CellTop)[offset->ImageNo];
            if (p) {
                SetCurrentPalette((char *)p);
            } else {
                SetCurrentPalette(offset->CommonPalette);
            }
        }
        if (offset->PaletteFlag & 1) {
            IST_Load(&((int *)offset->CellTop)[offset->ImageNo], offset->SpriteIdTop + offset->SpriteId, -1);
        } else {
            IST_Load(&((int *)offset->CellTop)[offset->ImageNo], offset->SpriteIdTop + offset->SpriteId, offset->PalId);
        }
        offset->SphAddr = GetSphByIST((char *)((int *)offset->CellTop)[offset->ImageNo]);
        Set_GCL(offset);
        PXF_GetSize(offset->SpriteIdTop + offset->SpriteId, &x, &y);
        offset->Imagew = x;
        offset->corey += y - 0x100;
        offset->LastImageNo = offset->ImageNo;
        offset->Imageh = y;
    }
    if (offset->SpriteFlag & 0x100) {
        offset->SpriteFlag &= ~0x100;
        for (i = 0; i < offset->SpriteIdMax; i++) {
            if (offset->SpriteId != i) {
                PXF_Free(offset->SpriteIdTop + i, 0);
            }
        }
    }
}

float ObjWorld2ScreenX(float x);
float ObjWorld2ScreenY(float y);
float GetChaseTrnsl(int id);
float GetBGFade(void);
void gCurrentColorSet(float val);
char Set_GCLVRect(CHARACTER_WORK *offset);

void SetSpriteStruct(CHARACTER_WORK *offset, NL_SPR_ARG *sprite)
{
    float LocalScale;
    float LocalScaleY;
    float posx;
    float posy;
    int spflag;
    float fade;

    LocalScale = CScale;
    posy = offset->posy;
    spflag = offset->SpriteFlag;
    posx = offset->posx;
    if (offset->scale > 0) {
        LocalScale *= offset->scale / 1000.0f;
    }
    if (offset->scaleY > 0) {
        LocalScaleY = CScale * offset->scaleY / 1000.0f;
    } else {
        LocalScaleY = LocalScale;
    }
    if (offset->ply) {
        posx += offset->ply->offx;
    }
    if (spflag & 1) {
        if (offset->scale > 0) {
            LocalScale = offset->scale / 1000.0f;
        } else {
            LocalScale = 1.0f;
        }
        LocalScaleY = LocalScale;
        if (offset->scaleY > 0) {
            LocalScaleY = offset->scaleY / 1000.0f;
        }
        sprite->attr = 0xD;
        sprite->x = posx / 100.0f + offset->corex * LocalScale;
        sprite->y = posy / 100.0f + offset->corey * LocalScaleY;
        sprite->z = offset->priority + 1.0f;
    } else {
        CorePos.x = ObjWorld2ScreenX(posx);
        if (spflag & 8) {
            CorePos.y = posy / 100.0f;
        } else {
            CorePos.y = ObjWorld2ScreenY(posy);
        }
        if (offset->dirflag == 1) {
            sprite->attr = 0x1F;
            sprite->x = CorePos.x - offset->corex * LocalScale;
        } else {
            sprite->attr = 0xD;
            sprite->x = CorePos.x + offset->corex * LocalScale;
        }
        sprite->y = CorePos.y + offset->corey * LocalScaleY;
        sprite->z = offset->priority;
    }
    if (offset->FBSpriteIdTop) {
        sprite->sprno = offset->SpriteId;
    } else {
        sprite->sprno = offset->SpriteIdTop + offset->SpriteId;
    }
    if (Set_GCLVRect(offset) == -1) {
        sprite->zm_x = LocalScale;
        sprite->u0 = 0;
        sprite->v0 = 0;
        sprite->u1 = 1.0f;
        sprite->v1 = 1.0f;
        sprite->zm_y = LocalScaleY;
    } else {
        SpriteXYSv.x = sprite->x;
        SpriteXYSv.y = sprite->y;
    }
    sprite->ang = 0;
    sprite->listType = 1;
    sprite->trnsl = offset->trans / 255.0f;
    if (spflag & 0x800) {
        if (g_info.BattleControlFlag & 0xC0) {
            sprite->trnsl = 0.0f;
        } else {
            sprite->trnsl *= GetChaseTrnsl(2);
        }
        fade = GetBGFade();
        gCurrentColorSet(fade);
        if (fade == 0.0f) {
            sprite->trnsl = 0.0f;
        }
    }
}

void SetSpriteStructShadow(CHARACTER_WORK *offset, NL_SPR_ARG *sprite)
{
    float LocalScale;
    float LocalScaleY;
    float posx;
    float posy;
    int spflag;
    float fade;

    LocalScale = CScale;
    posy = offset->posy;
    spflag = offset->SpriteFlag;
    posx = offset->posx;
    if (offset->scale > 0) {
        LocalScale *= offset->scale / 1000.0f;
    }
    if (offset->scaleY > 0) {
        LocalScaleY = CScale * offset->scaleY / 1000.0f;
    } else {
        LocalScaleY = LocalScale;
    }
    if (offset->ply) {
        posx += offset->ply->offx;
    }
    if (spflag & 1) {
        if (offset->scale > 0) {
            LocalScale = offset->scale / 1000.0f;
        } else {
            LocalScale = 1.0f;
        }
        LocalScaleY = LocalScale;
        if (offset->scaleY > 0) {
            LocalScaleY = offset->scaleY / 1000.0f;
        }
        sprite->attr = 0xD;
        sprite->x = posx / 100.0f + offset->corex * LocalScale;
        sprite->y = posy / 100.0f + offset->corey * LocalScaleY;
        sprite->z = offset->priority + 1.0f;
    } else {
        CorePos.x = ObjWorld2ScreenX(posx);
        if (spflag & 8) {
            CorePos.y = posy / 100.0f;
        } else {
            CorePos.y = ObjWorld2ScreenY(posy);
        }
        if (offset->dirflag == 1) {
            sprite->attr = 0x37;
            sprite->x = CorePos.x - offset->corex * LocalScale;
        } else {
            sprite->attr = 0x25;
            sprite->x = CorePos.x + offset->corex * LocalScale;
        }
        sprite->y = ObjWorld2ScreenY(-posy / 6.0f) - offset->corey * LocalScaleY / 6.0f;
        sprite->z = offset->priority + 10.0f;
    }
    if (offset->FBSpriteIdTop) {
        sprite->sprno = offset->SpriteId;
    } else {
        sprite->sprno = offset->SpriteIdTop + offset->SpriteId;
    }
    if (Set_GCLVRect(offset) == -1) {
        sprite->zm_x = LocalScale;
        sprite->u0 = 0;
        sprite->v0 = 0;
        sprite->u1 = 1.0f;
        sprite->v1 = 1.0f;
        sprite->zm_y = LocalScaleY;
    } else {
        SpriteXYSv.x = sprite->x;
        SpriteXYSv.y = sprite->y;
    }
    sprite->listType = 1;
    sprite->ang = 0;
    sprite->zm_y = sprite->zm_y / 6.0f;
    sprite->trnsl = offset->trans / 255.0f;
    if (spflag & 0x800) {
        if (g_info.BattleControlFlag & 0xC0) {
            sprite->trnsl = 0.0f;
        } else {
            sprite->trnsl *= GetChaseTrnsl(2);
        }
        fade = GetBGFade();
        gCurrentColorSet(fade);
        if (fade == 0.0f) {
            sprite->trnsl = 0.0f;
        }
    }
}

void SetSpriteStructByVRect(CHARACTER_WORK *offset, NL_SPR_ARG *sprite)
{
    float zx;
    float zy;
    float LocalScale;
    float LocalScaleY;
    COLLISION *VRectAddr;
    char *offp;
    int offx;
    int offy;
    int Imagew;
    int Imageh;

    LocalScaleY = CScale;
    LocalScale = LocalScaleY;
    VRectAddr = &offset->ColliAddr[offset->ColliNo];
    offp = (char *)VRectAddr + 10;
    PXF_GetSize(sprite->sprno, &Imagew, &Imageh);
    if (offset->SpriteFlag & 1) {
        LocalScale = 1.0f;
    }
    if (offset->scale > 0) {
        LocalScale *= offset->scale / 1000.0f;
    }
    if (offset->scaleY > 0) {
        LocalScaleY *= offset->scaleY / 1000.0f;
    } else {
        LocalScaleY = LocalScale;
    }
    offx = offp[0];
    offy = offp[1];
    offx *= 8;
    offy *= 8;
    zx = VRectAddr->w;
    zy = VRectAddr->h;
    sprite->zm_x = zx * LocalScale;
    sprite->zm_y = zy * LocalScaleY;
    if (offset->dirflag == 1 && (~offset->SpriteFlag & 1)) {
        sprite->x = SpriteXYSv.x - (VRectAddr->x + offx) * LocalScale;
    } else {
        sprite->x = SpriteXYSv.x + (VRectAddr->x + offx) * LocalScale;
    }
    sprite->y = SpriteXYSv.y + (VRectAddr->y + VRectAddr->h - offset->Imageh + offy) * LocalScaleY;
    sprite->u0 = (float)VRectAddr->x / Imagew;
    sprite->v0 = -((float)VRectAddr->y + VRectAddr->h) / Imageh;
    sprite->u1 = ((float)VRectAddr->x + VRectAddr->w) / Imagew;
    sprite->v1 = -(float)VRectAddr->y / Imageh;
    if (sprite->u0 < 0.0) {
        sprite->u1 = sprite->u1 - sprite->u0;
        sprite->u0 = 0;
    }
    if ((unsigned char)VRectAddr->attr == 6) {
        sprite->z = offset->priority2;
        sprite->trnsl = offset->trans2 / 255.0f;
        SetBlendingMode(offset->transmode2);
        if (offset->SpriteFlag & 0x10) {
            float temp;

            temp = sprite->v0;
            sprite->v0 = sprite->v1;
            sprite->v1 = temp;
        }
    } else {
        sprite->z = offset->priority;
        sprite->trnsl = offset->trans / 255.0f;
        SetBlendingMode(offset->transmode);
    }
}

void SetSpriteStructByVRectShadow(CHARACTER_WORK *offset, NL_SPR_ARG *sprite)
{
    float zx;
    float zy;
    float LocalScale;
    float LocalScaleY;
    COLLISION *VRectAddr;
    char *offp;
    int offx;
    int offy;
    int Imagew;
    int Imageh;

    LocalScaleY = CScale;
    LocalScale = LocalScaleY;
    VRectAddr = &offset->ColliAddr[offset->ColliNo];
    offp = (char *)VRectAddr + 10;
    PXF_GetSize(sprite->sprno, &Imagew, &Imageh);
    if (offset->SpriteFlag & 1) {
        LocalScale = 1.0f;
    }
    if (offset->scale > 0) {
        LocalScale *= offset->scale / 1000.0f;
    }
    if (offset->scaleY > 0) {
        LocalScaleY *= offset->scaleY / 1000.0f;
    } else {
        LocalScaleY = LocalScale;
    }
    offx = offp[0];
    offy = offp[1];
    offx *= 8;
    offy *= 8;
    zx = VRectAddr->w;
    zy = VRectAddr->h;
    sprite->zm_x = zx * LocalScale;
    zy *= LocalScaleY;
    sprite->zm_y = zy / 6.0f;
    if (offset->dirflag == 1 && (~offset->SpriteFlag & 1)) {
        sprite->x = SpriteXYSv.x - (VRectAddr->x + offx) * LocalScale;
    } else {
        sprite->x = SpriteXYSv.x + (VRectAddr->x + offx) * LocalScale;
    }
    sprite->y = SpriteXYSv.y - (VRectAddr->y + VRectAddr->h - offset->Imageh + offy) * (LocalScaleY / 6.0f);
    sprite->u0 = (float)VRectAddr->x / Imagew;
    sprite->v0 = -((float)VRectAddr->y + VRectAddr->h) / Imageh;
    sprite->u1 = ((float)VRectAddr->x + VRectAddr->w) / Imagew;
    sprite->v1 = -(float)VRectAddr->y / Imageh;
    if (sprite->u0 < 0.0) {
        sprite->u1 = sprite->u1 - sprite->u0;
        sprite->u0 = 0;
    }
    if ((unsigned char)VRectAddr->attr == 6) {
        sprite->z = offset->priority2;
        sprite->trnsl = offset->trans2 / 255.0f;
        SetBlendingMode(offset->transmode2);
        if (offset->SpriteFlag & 0x10) {
            float temp;

            temp = sprite->v0;
            sprite->v0 = sprite->v1;
            sprite->v1 = temp;
        }
    } else {
        sprite->z = offset->priority;
        sprite->trnsl = offset->trans / 255.0f;
        SetBlendingMode(offset->transmode);
    }
    sprite->z = (float)offset->priority + 10.0;
}

extern int BattleStatus;
void *memset(void *s, int c, int n);
void FirePalette_Load(unsigned int palid);
void RaiPalette_Load(unsigned int palid);
void PXF_SetPaletteMain(void);
void CurrentPaletteLoad(unsigned int palid, char *PalAddr);
void SaveAfterImage(CHARACTER_WORK *offset);
void PushAfterImage(CHARACTER_WORK *offset);
void PopAfterImage(CHARACTER_WORK *offset);
int SetAfterImage(CHARACTER_WORK *offset, int index);
void gCurrentColorSetARBG(unsigned int arbg);

void GraphicExecute_Player(CHARACTER_WORK *offset, int id)
{
    NL_SPR_ARG spritework;
    NL_SPR_ARG *sprite = &spritework;
    int flag;
    int i;
    int ObjUsePalTb[22] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 };

    CellMain(offset);
    if (offset->SpriteFlag & 0x1000) {
        for (i = 0; i < 4; i++) {
            if (!(ObjUsePalTb[offset->idno] & i)) {
                FirePalette_Load(offset->PalId + i);
            }
        }
        PXF_SetPaletteMain();
        offset->SpriteFlag |= 0x400;
    } else if (offset->SpriteFlag & 0x2000) {
        for (i = 0; i < 4; i++) {
            if (!(ObjUsePalTb[offset->idno] & i)) {
                RaiPalette_Load(offset->PalId + i);
            }
        }
        PXF_SetPaletteMain();
        offset->SpriteFlag |= 0x400;
    } else if (offset->SpriteFlag & 0x400) {
        CurrentPaletteLoad(offset->PalId, offset->CommonPalette);
        offset->SpriteFlag &= ~0x400;
    }
    if (offset->SpriteFlag & 0x40000) {
        gColPush(0);
    } else if (offset->SpriteFlag & 0x800200) {
        gSetObjColorOffset(gStripGetFadeColor(0xC0C0C0));
    } else if (offset->SpriteFlag & 0x4000) {
        static unsigned int ocolor[] = { 0, 0x300808, 0x601010, 0x901818, 0xC02020, 0x901818, 0x601010, 0x300808 };

        gSetObjColorOffset(gStripGetFadeColor(ocolor[TotalCounter & 7]));
    } else if (offset->SpriteFlag & 0x80000) {
        static unsigned int ocolor[] = { 0, 0x080830, 0x101060, 0x181890, 0x2020C0, 0x181890, 0x101060, 0x080830 };

        gSetObjColorOffset(gStripGetFadeColor(ocolor[TotalCounter & 7]));
    } else if (offset->SpriteFlag & 0x100000) {
        static unsigned int ocolor[] = { 0, 0x300830, 0x601060, 0x901890, 0xC020C0, 0x901890, 0x601060, 0x300830 };

        gSetObjColorOffset(gStripGetFadeColor(ocolor[TotalCounter & 7]));
    } else if (offset->SpriteFlag & 0x8000) {
        gColPush(0xFFFFFF);
    }
    GraphicExecute_Playersub(offset, sprite);
    if (offset->SpriteFlag & 0x984200) {
        gSetObjColorOffset(0);
    }
    if (offset->SpriteFlag & 0x48000) {
        gColPop();
    }
    if (offset->SpriteFlag & 1) {
        return;
    }
    if (!(offset->SpriteFlag & 0x20000) && offset->idno != 8) {
        gColPush(0);
        SetSpriteStructShadow(offset, sprite);
        if (offset->ColliNo != -1) {
            do {
                SetSpriteStructByVRectShadow(offset, sprite);
                if (!(offset->ply->pactst & 0x800)) {
                    gSpriteDrawUV(sprite);
                }
            } while (Set_GCLVRectNext(offset) != -1);
        } else {
            gSpriteDraw_NaomiLibVersion(sprite);
        }
        gColPop();
    }
    if (offset->HitStopTime == 0) {
        SaveAfterImage(offset);
    }
    if (offset->ply->pactst & 7) {
        static unsigned int fcolor[] = { 0x3060FF, 0x2040FF, 0x1020FF };
        static unsigned int ocolor[] = { 0x60, 0x40, 0x20 };
        int j;

        PushAfterImage(offset);
        offset->trans = 0xFF;
        offset->priority += 10;
        i = 3; j = 0;
        while (SetAfterImage(offset, i)) {
            offset->trans -= offset->trans >> 3;
            offset->priority += 2;
            gCurrentColorSetARBG(fcolor[j]);
            gSetObjColorOffset(ocolor[j]);
            j++;
            GraphicExecute_Playersub(offset, sprite);
            i += 3;
            if (i >= 7) {
                break;
            }
        }
        gCurrentColorClear();
        gSetObjColorOffset(0);
        PopAfterImage(offset);
    }
    flag = 0;
    if (offset->ply->OverKillFlag == 1) {
        if (BattleStatus == 2) flag = 1;
    }
    if (flag) {
        if (GetEnemyPlayer(offset)->ply->OverKillFlag == 1) {
            flag = (offset->padid == (TotalCounter & 1));
        }
        if (flag) {
            static unsigned int fcolor[] = { 0x401010 };
            static unsigned int ocolor[] = { 0x401010 };
            char transmode;
            char transmode2;
            short priority2;
            char trans2;
            float c;

            transmode = offset->transmode;
            transmode2 = offset->transmode2;
            priority2 = offset->priority2;
            trans2 = offset->trans2;
            PushAfterImage(offset);
            offset->transmode = 1;
            offset->transmode2 = 1;
            offset->trans = 0x80;
            if (offset->SpriteFlag & 0x400000) {
                offset->trans2 = 0;
            } else {
                offset->trans2 = 0x80;
            }
            offset->priority2 = offset->priority = offset->priority + 18;
            c = (TotalCounter & 0xF) * 0.003;
            if (TotalCounter & 0x10) {
                c = 0.048 - c;
            }
            if (offset->scale < 0) {
                offset->scale = 1000;
            }
            offset->scale = offset->scale * (c + 1.03);
            SetBlendingMode(1);
            gCurrentColorSetARBG(fcolor[0]);
            gSetObjColorOffset(ocolor[0]);
            GraphicExecute_Playersub(offset, sprite);
            gCurrentColorClear();
            gSetObjColorOffset(0);
            PopAfterImage(offset);
            offset->transmode = transmode;
            offset->transmode2 = transmode2;
            offset->priority2 = priority2;
            offset->trans2 = trans2;
            SetBlendingMode(0);
        }
    }
}
