#include "generated/bg.h"

void BG_Move(BG_WORK *offset) {
}

extern RECTL ScreenBG;
extern SCREEN_STRUCT *scr_stp;

void bgprecalc(int scrollrate)
{
    float rate = scrollrate;

    Bgpos_precalc->scr_x = ScreenBG.x * rate / 1000.0f;
    Bgpos_precalc->scr_y = ScreenBG.y * rate / 1000.0f;
    Bgpos_precalc->scale = 1.0f - (1.0f - scr_stp->stScreenScaleBG) * rate / 1000.0f;
}

int BGWorld2ScreenX(int x)
{
    x -= Bgpos_precalc->scr_x;
    x *= Bgpos_precalc->scale;
    x += 32000;
    return x / 100;
}

extern float D_5CAC0C; /* RODATA_EXTERN 0x5cac0c: 44000.0f */

int BGWorld2ScreenY(int y)
{
    y -= Bgpos_precalc->scr_y;
    y *= Bgpos_precalc->scale;
    y += D_5CAC0C;
    return y / 100;
}

void gStripDraw_BGFast(NL_SPR_ARG *sprite);
void gStripDraw_BGFastSkipY_TRS(NL_SPR_ARG *sprite);
void gStripDraw_BGFastSkipY_OPQ(NL_SPR_ARG *sprite);
void gStripDraw_BGFastReverse(NL_SPR_ARG *sprite);
void gWaveStripDraw(NL_SPR_ARG *sprite, int sz);
void PXF_GetSize(int id, int *x, int *y);
void BG_SpriteSetR(BG_WORK *offset, NL_SPR_ARG *sprite, int count)
{
    for (; count != 0; count--, offset++) {
        if (BG_SpriteSetSub(offset, sprite)) {
            sprite->listType = offset->semitrans;
            gStripDraw_BGFastReverse(sprite);
        }
    }
}

void BG_SpriteSetW(BG_WORK *offset, NL_SPR_ARG *sprite, int count)
{
    int x;
    int y;

    for (; count != 0; count--, offset++) {
        if (BG_SpriteSetSub(offset, sprite)) {
            sprite->listType = offset->semitrans;
            PXF_GetSize(sprite->sprno, &x, &y);
            gWaveStripDraw(sprite, x);
        }
    }
}

void BG_SpriteSetC(BG_WORK *offset, NL_SPR_ARG *sprite, int count)
{
    sprite->listType = 1;
    for (; count != 0; count--, offset++) {
        if (BG_SpriteSetSub(offset, sprite)) {
            if (bgex.sety) {
                gStripDraw_BGFast(sprite);
                bgex.sety = 0;
            } else {
                gStripDraw_BGFastSkipY_TRS(sprite);
            }
        }
    }
}

void BG_SpriteSetCR(BG_WORK *offset, NL_SPR_ARG *sprite, int count)
{
    sprite->listType = 1;
    for (; count != 0; count--, offset++) {
        if (BG_SpriteSetSub(offset, sprite)) {
            gStripDraw_BGFastReverse(sprite);
        }
    }
}

void BG_SpriteSetCW(BG_WORK *offset, NL_SPR_ARG *sprite, int count)
{
    int x;
    int y;

    sprite->listType = 1;
    for (; count != 0; count--, offset++) {
        if (BG_SpriteSetSub(offset, sprite)) {
            PXF_GetSize(sprite->sprno, &x, &y);
            gWaveStripDraw(sprite, x);
        }
    }
}

void BG_SpriteSet(BG_WORK *offset, NL_SPR_ARG *sprite, int count)
{
    for (; count != 0; offset++, count--) {
        if (BG_SpriteSetSub(offset, sprite)) {
            if (bgex.sety) {
                sprite->listType = offset->semitrans;
                gStripDraw_BGFast(sprite);
                bgex.sety = 0;
            } else if (offset->semitrans == 0) {
                gStripDraw_BGFastSkipY_OPQ(sprite);
            } else {
                gStripDraw_BGFastSkipY_TRS(sprite);
            }
        }
    }
}

int IsChaseEffectDraw(void);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FLine(float x1, float y1, float x2, float y2, float z, unsigned int color1);
int Random(void);
extern GAME_INFOMATION g_info;

void RainExecute(void)
{
    RAIN_WORK *offset;
    int i;
    int n;
    int a;

    if (IsChaseEffectDraw()) {
        return;
    }
    if (g_info.BattleControlFlag & 2) {
        return;
    }
    for (i = 0; i < 80; i++) {
        offset = &RainWork[i];
        if (offset->flg == 0) {
            n = Random();
            offset->x1 = n & 0x3FF;
            if (offset->x1 > 680) {
                offset->x1 = n & 0x1FF;
            }
            offset->y1 = -(n & 0x3F);
            if (n & 1) {
                offset->x2 = offset->x1 + (n & 3);
            } else {
                offset->x2 = offset->x1 - (n & 3);
            }
            if (i < 10) {
                offset->flg = 24;
                offset->y2 = offset->y1 - 50;
            } else if (i < 30) {
                offset->flg = 30;
                offset->y2 = offset->y1 - 65;
            } else {
                offset->flg = 38;
                offset->y2 = offset->y1 - 80;
            }
        } else {
            offset->c1 = gStripGetFadeColor(((int)(GetBGFade() * 48.0f) << 24) | 0xFFFFFF);
            polydraw_FLine(offset->x1, offset->y1, offset->x2, offset->y2, 8.0f, offset->c1);

            offset->y1 += offset->flg;
            offset->y2 += offset->flg;
            if (offset->x1 < offset->x2) {
                offset->x1--;
                offset->x2--;
            } else if (offset->x2 < offset->x1) {
                offset->x1++;
                offset->x2++;
            }
            if (offset->y1 > 480) {
                offset->flg = 0;
            }
        }
    }
}

void *dmalloc(int size, char *name);
void dfree(void *mem);

extern char D_5BE718[]; /* RODATA_EXTERN 0x5be718: "RainWork" */

void RainInit(void)
{
    RainWork = dmalloc(0x780, D_5BE718);
}

void RainEnd(void)
{
    if (RainWork) {
        dfree(RainWork);
    }
    RainWork = 0;
}

extern unsigned int *LayerHeaderp; /* STATIC_EXTERN 0x5cb434 */

void BGLayerVisualON(short int id)
{
    unsigned int count;

    count = LayerHeaderp[id];
    if (count) {
        count |= 0x80000000;
    }
    LayerHeaderp[id] = count;
}

void BGLayerVisualOFF(short int id)
{
    LayerHeaderp[id] &= 0xFFF;
}

extern LAYERANIME LayerAnimeTb[8]; /* STATIC_EXTERN 0x5df020 */

void BGLayerAnimetion(void)
{
    LAYERANIME *offset;
    BGSETPACKET_LANIME *p;
    int i;

    for (i = 0; i < 8; i++) {
        offset = &LayerAnimeTb[i];
        if (offset->top == 0) {
            continue;
        }
        if (++offset->time < offset->maxtime) {
            continue;
        }
        p = (BGSETPACKET_LANIME *)offset->addr;
        offset->time = 0;
        offset->addr += 4;
        for (;;) {
            switch (((BGSETPACKET_LANIME *)offset->addr)->id) {
            case 7:
                break;
            default:
                offset->addr = offset->top + 4;
                break;
            case 8:
                BGLayerVisualON(((BGSETPACKET_LANIME *)offset->addr)->time);
                offset->addr += 4;
                continue;
            case 9:
                BGLayerVisualOFF(((BGSETPACKET_LANIME *)offset->addr)->time);
                offset->addr += 4;
                continue;
            }
            break;
        }
        offset->maxtime = p->time;
    }
}

float GetBGFade(void)
{
    return bgex.fade[2];
}

extern int StageNo;

void SetStageNo(int no)
{
    StageNo = no;
}

char *FileReadSelect(int fid, int chan);

void ForeStellBG(void)
{
    BGWorkInit();
    if (StageNo == 0) {
        StageNo = 5;
    }
    BGFile = FileReadSelect(StageNo + 0xF0, 2);
    ReadStageNo = StageNo;
}

void MaySHPEnd(void);

void BG_End(void)
{
    switch (StageNo) {
    case 1:
        RainEnd();
        break;
    case 5:
        MaySHPEnd();
        break;
    }
    if (LayerHeaderp) {
        dfree(LayerHeaderp);
    }
    LayerHeaderp = 0;
}

int GetBGObjCnt(void);
float KakuseiBG_GetFadeVal(void);
CHARACTER_WORK *EffectCreate(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg);
extern unsigned int Pause;
extern u_int TotalCounter;
int Random(void);
void MayBack(int a);
void MaySHPExe(void);
void JMBack(void);
void PoBarExe(void);
void PoEyeExe(void);
void VEEye_Execute(void);

void BGObjCreate(void)
{
    int objcnt;
    GAME_INFOMATION *gp;

    objcnt = GetBGObjCnt();
    gp = &g_info;
    if (gp->BattleControlFlag & 1) {
        bgex.fade[0] -= 0.15f;
        if (bgex.fade[0] < 0.0) {
            bgex.fade[0] = 0.0f;
        }
        bgex.fade[1] -= 0.12f;
        if (bgex.fade[1] < 0.0) {
            bgex.fade[1] = 0.0f;
        }
        bgex.fade[2] -= 0.1f;
        if (bgex.fade[2] < 0.0) {
            bgex.fade[2] = 0.0f;
            gp->BattleControlFlag |= 0x20;
        }
    } else if (KakuseiBG_GetFadeVal() > 0.0f) {
    } else if (gp->BattleControlFlag & 0x10) {
        bgex.fade[0] -= 0.1f;
        if (bgex.fade[0] < 0.0) {
            bgex.fade[0] = 0.0f;
        }
        bgex.fade[1] -= 0.08f;
        if (bgex.fade[1] < 0.0) {
            bgex.fade[1] = 0.0f;
        }
        if (bgex.fade[2] < 0.25f) {
            bgex.fade[2] += 0.06f;
            if (bgex.fade[2] > 0.25f) {
                bgex.fade[2] = 0.25f;
            }
        } else {
            bgex.fade[2] -= 0.06f;
            if (bgex.fade[2] < 0.25f) {
                bgex.fade[2] = 0.25f;
            }
        }
    } else if (gp->BattleControlFlag & 0x100) {
        bgex.fade[0] -= 0.15f;
        if (bgex.fade[0] < 0.0) {
            bgex.fade[0] = 0.0f;
        }
        bgex.fade[1] -= 0.12f;
        if (bgex.fade[1] < 0.0) {
            bgex.fade[1] = 0.0f;
        }
        bgex.fade[2] -= 0.1f;
        if (bgex.fade[2] < 0.0) {
            bgex.fade[2] = 0.0f;
        }
    } else {
        bgex.fade[0] += 0.1f;
        if (bgex.fade[0] > 1.0f) {
            bgex.fade[0] = 1.0f;
        }
        bgex.fade[1] += 0.08f;
        if (bgex.fade[1] > 1.0f) {
            bgex.fade[1] = 1.0f;
        }
        bgex.fade[2] += 0.06f;
        if (bgex.fade[2] > 1.0f) {
            bgex.fade[2] = 1.0f;
        }
    }
    gp->BattleControlFlag &= ~1;
    gp->BattleControlFlag &= ~0x10;
    gp->BattleControlFlag &= ~0x100;
    switch (StageNo) {
    case 1: {
        CREATE_ARG c_arg;

        c_arg.flag = 4;
        c_arg.actno = 0;
        if (Pause == 0) {
            EffectCreate(0, 0x81, &c_arg);
        }
        break;
    }
    case 2:
        if (objcnt < 17 && !(TotalCounter & 0x1F)) {
            CREATE_ARG c_arg2;

            c_arg2.flag = 1;
            c_arg2.x = -200000;
            c_arg2.y = 0;
            EffectCreate(0, 0x80, &c_arg2);
        }
        break;
    case 3:
        if (objcnt < 17 && !(TotalCounter & 0xFF)) {
            CREATE_ARG c_arg;

            c_arg.flag = 0x45;
            c_arg.tx = 60000;
            c_arg.ty = -5000;
            if (Random() & 1) {
                c_arg.x = c_arg.tx + -200000;
            } else {
                c_arg.x = c_arg.tx + 200000;
            }
            c_arg.actno = 12;
            c_arg.y = c_arg.ty + -50000;
            EffectCreate(0, 0x80, &c_arg);
        }
        break;
    case 5:
        if (!(TotalCounter & 7)) {
            CREATE_ARG c_arg;

            c_arg.flag = 1;
            c_arg.x = 31000;
            c_arg.y = -42000;
            if (Pause == 0) {
                EffectCreate(0, 0x80, &c_arg);
            }
        }
        MayBack(0);
        MaySHPExe();
        break;
    case 4:
        if (!(TotalCounter & 0x1F)) {
            CREATE_ARG c_arg;

            c_arg.flag = 1;
            c_arg.y = c_arg.x = 0x19000;
            if (Pause == 0) {
                EffectCreate(0, 0x80, &c_arg);
                c_arg.x = -0x19000;
                EffectCreate(0, 0x80, &c_arg);
            }
        }
        if (!(TotalCounter & 7)) {
            JMBack();
        }
        break;
    case 6:
        PoBarExe();
        PoEyeExe();
        if (objcnt < 11 && !(TotalCounter & 0x3F) && !(Random() & 3)) {
            CREATE_ARG c_arg;
            int r;

            r = Random();
            c_arg.flag = 0x61;
            c_arg.Localid = r & 3;
            if (r & 1) {
                c_arg.x = 80000;
            } else {
                c_arg.x = -80000;
            }
            if (c_arg.Localid & 2) {
                c_arg.y = 3600;
                c_arg.tx = 800;
            } else if (Random() & 1) {
                c_arg.y = -2000;
                c_arg.tx = 650;
            } else {
                c_arg.y = -3800;
                c_arg.tx = 500;
            }
            if (Pause == 0) {
                EffectCreate(0, 0x80, &c_arg);
            }
        }
        break;
    case 7:
        if (!(TotalCounter & 0xF)) {
            VEEye_Execute();
        }
        break;
    case 9:
        if (objcnt < 17 && !(TotalCounter & 0x3F)) {
            CREATE_ARG c_arg;

            c_arg.flag = 1;
            c_arg.x = 0x19000;
            c_arg.y = 2000;
            if (Pause == 0) {
                EffectCreate(0, 0x80, &c_arg);
            }
        }
        break;
    case 10:
        if (objcnt < 17) {
            if (!(TotalCounter & 0xFF)) {
                CREATE_ARG c_arg;

                c_arg.flag = 1;
                c_arg.x = 0x19000;
                c_arg.y = 2000;
                if (Pause == 0) {
                    EffectCreate(0, 0x80, &c_arg);
                }
            }
            if (!(TotalCounter & 0x3F)) {
                CREATE_ARG c_arg;

                c_arg.flag = 1;
                c_arg.x = 0x19000;
                c_arg.y = 2000;
                if (Pause == 0) {
                    EffectCreate(0, 0x82, &c_arg);
                }
            }
        }
        break;
    }
}

void *smalloc(int size, char *name);
void LinkAddrTb(u_int *addr);
void InitSpriteIdBG(void);
extern char *bgset_datap; /* STATIC_EXTERN 0x5cc75c */

void BG_LoadTextureInit(void)
{
    BG_LOADTEXTURECONTROL *p;
    unsigned int *addr;

    p = &BGLoad_Work;
    p->orgaddr = (unsigned int *)BGFile;
    LayerHeaderp = smalloc(0x400, "LayerHeaderp");
    {
        int i;

        for (i = 255; i >= 0; i--) {
            LayerHeaderp[i] = 0;
        }
    }
    LayerHeaderp[255] = 0xFFFFFFFF;
    p->paltop = 0x3F;
    p->num = 0;
    LinkAddrTb(p->orgaddr);
    addr = (unsigned int *)*p->orgaddr;
    LinkAddrTb(addr);
    bgset_datap = (char *)*addr;
    {
        unsigned int *orgaddr;
        int i;

        orgaddr = p->orgaddr + 2;
        i = 0;
        while (*orgaddr) {
            StageObjAddr[i] = (int *)orgaddr[-1];
            orgaddr++;
            i++;
        }
    }
    p->addr = addr + 1;
    InitSpriteIdBG();
}

void BGWorkInit(void)
{
    BG_LOADTEXTURECONTROL *p;

    Bgpos_precalc = smalloc(sizeof(BGPOS_PRECALC), "Bgpos_precalc");
    p = &BGLoad_Work;
    BGLoad_Work.IsVQ = dmalloc(0x280, "IsVQ");
    p->HeightOffset = dmalloc(0xA00, "HeightOffset");
    p->Size = dmalloc(0xA00, "Size");
    p->sprtidtop = -1;
    p->addr = 0;
    p->orgaddr = 0;
    p->paltop = 0;
}

short int GetSpriteIdTopBG(short unsigned int max);
int IST_Load(int *texadr, int sprid, int palid);

void BG_LoadTextureExecute(void)
{
    BG_LOADTEXTURECONTROL *p;
    int size;
    int ysize;
    int sprtid;

    p = &BGLoad_Work;
    if (p->addr == 0 || p->num == -1) {
        return;
    }
    while (*p->addr) {
        sprtid = GetSpriteIdTopBG(1);
        if (p->sprtidtop == -1) {
            p->sprtidtop = sprtid;
        }
        p->IsVQ[p->num] = IST_Load((int *)p->addr, sprtid, -1);
        PXF_GetSize(sprtid, &size, &ysize);
        for (size = 8; size < ysize; size <<= 1);
        p->HeightOffset[p->num] = size - ysize;
        p->addr++;
        p->Size[p->num] = size;
        p->num++;
    }
    p->num = -1;
}

BG_WORK *BgWork_Initial(short int idno, BGINIT_ARG *arg);

void BG_LoadTexture(void)
{
    BG_LOADTEXTURECONTROL *p;
    BG_WORK *offset;
    BGINIT_ARG arg;
    int senterx;
    int sentery;
    int layer;
    int scroll;
    int priority;
    int semitrans;
    BGSETPACKET_IMAGE *imagep;
    int w;
    int h;
    BG_WORK *animeoffset;
    unsigned int actst;
    int i;

    p = &BGLoad_Work;
    imagep = 0;
    sentery = 0;
    priority = 0;
    senterx = 0;
    semitrans = 0;
    actst = 0;
    offset = 0;
    BG_LoadTextureExecute();
    layer = 0;
    scroll = 0;
    animeoffset = 0;
    for (i = 7; i >= 0; i--) {
        LayerAnimeTb[i].top = 0;
    }
    for (;;) {
        switch (*bgset_datap) {
        case -1:
            return;
        case 1:
            imagep = (BGSETPACKET_IMAGE *)bgset_datap;
            arg.SpriteIdTop = p->sprtidtop + imagep->imageno;
            PXF_GetSize(arg.SpriteIdTop, &w, &h);
            if (actst & 0x80000) {
                arg.posx = (-senterx - imagep->x - w) * 100;
            } else {
                arg.posx = (imagep->x - senterx) * 100;
            }
            arg.posy = -(sentery + imagep->y + p->HeightOffset[imagep->imageno]) * 100;
            arg.priority = priority;
            if (p->HeightOffset[imagep->imageno]) {
                arg.priority = priority + 10.0f;
            }
            arg.scrollrate = scroll;
            offset = BgWork_Initial(layer, &arg);
            offset->semitrans = semitrans;
            if (semitrans == 2) {
                if (actst & 0x100000) {
                    offset->semitrans = 1;
                } else {
                    offset->semitrans = 1;
                }
            }
            offset->Size = p->Size[imagep->imageno];
            offset->actst |= actst;
            if (p->IsVQ[imagep->imageno] == 0) {
                offset->actst |= 0x20000;
            }
            offset->dbg_centerx = senterx;
            offset->dbg_centery = sentery;
            if (actst & 0x80000) {
                offset->dbg_imagex = -imagep->x - w;
            } else {
                offset->dbg_imagex = imagep->x;
            }
            offset->dbg_imagey = imagep->y + p->HeightOffset[imagep->imageno];
            bgset_datap += sizeof(BGSETPACKET_IMAGE);
            break;
        case 2:
            animeoffset = 0;
            senterx = ((BGSETPACKET_LAYER *)bgset_datap)->senterx;
            semitrans = 2;
            sentery = ((BGSETPACKET_LAYER *)bgset_datap)->sentery;
            priority = ((BGSETPACKET_LAYER *)bgset_datap)->priority * 10;
            layer = ((BGSETPACKET_LAYER *)bgset_datap)->layerno;
            actst = 0;
            scroll = ((BGSETPACKET_LAYER *)bgset_datap)->scroll;
            bgset_datap += sizeof(BGSETPACKET_LAYER);
            break;
        case 3:
            bgset_datap += 4;
            semitrans = 0;
            break;
        case 4:
            bgset_datap += 4;
            semitrans = 1;
            break;
        case 10:
            actst |= 0x40000;
            bgset_datap += 4;
            break;
        case 12:
            actst |= 0x100000;
            bgset_datap += 4;
            break;
        case 11:
            actst |= 0x80000;
            bgset_datap += 4;
            break;
        case 5:
            if (animeoffset == 0) {
                BGSETPACKET_ANIME *anime = (BGSETPACKET_ANIME *)bgset_datap;

                arg.SpriteIdTop = p->sprtidtop + anime->imageno;
                arg.posx = (anime->x - senterx) * 100;
                arg.posy = -(sentery + anime->y + p->HeightOffset[anime->imageno]) * 100;
                arg.priority = priority;
                if (p->HeightOffset[anime->imageno]) {
                    arg.priority = priority + 10.0f;
                }
                arg.scrollrate = scroll;
                animeoffset = BgWork_Initial(layer, &arg);
                animeoffset->semitrans = semitrans;
                offset->dbg_centerx = senterx;
                offset->dbg_centery = sentery;
                animeoffset->AnimeAddrTop = bgset_datap;
                animeoffset->AnimeAddr = bgset_datap;
                offset->dbg_imagex = imagep->x;
                offset->dbg_imagey = imagep->y + p->HeightOffset[imagep->imageno];
            }
            bgset_datap += sizeof(BGSETPACKET_ANIME);
            break;
        case 6:
            for (i = 0; i < 8; i++) {
                if (LayerAnimeTb[i].top == 0) {
                    LayerAnimeTb[i].top = bgset_datap;
                    LayerAnimeTb[i].addr = bgset_datap + 4;
                    LayerAnimeTb[i].maxtime = 0;
                    LayerAnimeTb[i].time = 0;
                    break;
                }
            }
            bgset_datap += 4;
            break;
        case 7:
        case 8:
        case 9:
            bgset_datap += 4;
            break;
        }
    }
}

void ObjLinkSub(u_int *p, int pid, int cnt);
void MaySHPInit(void);
void Gray_Exe(void);
extern BG_WORK *BgWork;
extern int BGBeingCount; /* STATIC_EXTERN 0x5cc760 */

void BG_Init(void)
{
    bgex.fade[0] = 1.0f;
    bgex.fade[1] = 1.0f;
    bgex.fade[2] = 1.0f;
    BG_LoadTextureInit();
    BG_LoadTexture();
    switch (StageNo) {
    case 1: {
        CREATE_ARG c_arg;

        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        ObjLinkSub((u_int *)StageObjAddr[1], -2, 0x81);
        c_arg.flag = 1;
        c_arg.x = (Random() & 0x7F) * 100;
        if (Random() & 1) {
            c_arg.x = -c_arg.x;
        }
        c_arg.y = -3000;
        EffectCreate(0, 0x80, &c_arg);
        c_arg.flag = 5;
        c_arg.x = -44000;
        c_arg.y = -31600;
        c_arg.actno = 1;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.actno = 4;
        c_arg.x = -44000;
        c_arg.y = -4000;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.actno = 2;
        c_arg.x = -30800;
        c_arg.y = -34800;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.actno = 5;
        c_arg.x = -30800;
        c_arg.y = -2000;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.actno = 3;
        c_arg.x = -24000;
        c_arg.y = -32000;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.actno = 5;
        c_arg.x = -24000;
        c_arg.y = -6000;
        EffectCreate(0, 0x81, &c_arg);
        RainInit();
        break;
    }
    case 3:
        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        ObjLinkSub((u_int *)StageObjAddr[1], -2, 0x81);
        break;
    case 5:
        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        MaySHPInit();
        MayBack(1);
        break;
    case 6: {
        CREATE_ARG c_arg;

        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        ObjLinkSub((u_int *)StageObjAddr[1], -2, 0x81);
        c_arg.flag = 1;
        c_arg.x = -40000;
        c_arg.y = -17000;
        EffectCreate(0, 0x81, &c_arg);
        break;
    }
    case 7:
        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        {
            int x[10] = { -242, -59, 124, 251, 335, 380, 338, 276, 198, 106 };
            int y[10] = { -170, -166, -173, -168, -168, -190, -206, -215, -222, -225 };
            CREATE_ARG c_arg;
            int i;

            c_arg.flag = 0x65;
            for (i = 0; i < 10; i++) {
                c_arg.actno = 0;
                c_arg.Localid = i & 3;
                c_arg.x = x[i] * 100;
                c_arg.y = y[i] * 100;
                c_arg.tx = i;
                EffectCreate(0, 0x80, &c_arg);
                c_arg.actno = i + 1;
                EffectCreate(0, 0x80, &c_arg);
            }
            c_arg.actno = 11;
            c_arg.x = 500;
            c_arg.y = -21200;
            EffectCreate(0, 0x80, &c_arg);
        }
        break;
    case 8: {
        CREATE_ARG c_arg;

        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        ObjLinkSub((u_int *)StageObjAddr[1], -2, 0x81);
        ObjLinkSub((u_int *)StageObjAddr[2], -2, 0x82);
        ObjLinkSub((u_int *)StageObjAddr[3], -2, 0x83);
        c_arg.flag = 1;
        c_arg.x = 31600;
        c_arg.y = -26000;
        EffectCreate(0, 0x80, &c_arg);
        c_arg.x = -31600;
        c_arg.y = -26000;
        EffectCreate(0, 0x80, &c_arg);
        c_arg.x = 0;
        c_arg.y = -48200;
        EffectCreate(0, 0x82, &c_arg);
        c_arg.x = 15000;
        c_arg.y = -50000;
        EffectCreate(0, 0x83, &c_arg);
        c_arg.x = -15000;
        c_arg.y = -50000;
        EffectCreate(0, 0x83, &c_arg);
        break;
    }
    case 2:
    case 4:
    case 9:
        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        break;
    case 10: {
        CREATE_ARG c_arg;

        ObjLinkSub((u_int *)StageObjAddr[0], -2, 0x80);
        ObjLinkSub((u_int *)StageObjAddr[1], -2, 0x81);
        ObjLinkSub((u_int *)StageObjAddr[2], -2, 0x82);
        c_arg.flag = 1;
        c_arg.x = 30400;
        c_arg.y = -12500;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.x = 35600;
        c_arg.y = -12000;
        EffectCreate(0, 0x81, &c_arg);
        break;
    }
    case 11: {
        unsigned int *boaddr;
        CREATE_ARG c_arg;

        boaddr = (unsigned int *)StageObjAddr[0];
        ObjLinkSub(boaddr, -2, 0x80);
        ObjLinkSub(boaddr, -2, 0x81);
        ObjLinkSub((u_int *)StageObjAddr[1], -2, 0x82);
        c_arg.flag = 1;
        c_arg.x = 62500;
        c_arg.y = -8800;
        EffectCreate(0, 0x80, &c_arg);
        c_arg.flag = 1;
        c_arg.x = 16800;
        c_arg.y = -3600;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.x = 11100;
        c_arg.y = -3100;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.x = -55000;
        c_arg.y = -2700;
        EffectCreate(0, 0x81, &c_arg);
        c_arg.flag = 1;
        c_arg.x = -64000;
        c_arg.y = -24600;
        EffectCreate(0, 0x82, &c_arg);
        break;
    }
    }
    BGBeingCount = 0;
    {
        BG_WORK *offset;

        offset = BgWork;
        while (offset->actst & 1) {
            LayerHeaderp[offset->idno]++;
            if (++BGBeingCount >= 640) {
                break;
            }
            offset = &BgWork[BGBeingCount];
        }
    }
    {
        int i;

        for (i = 0; i < 256; i++) {
            if (LayerHeaderp[i]) {
                LayerHeaderp[i] |= 0x80000000;
            }
        }
    }
    if (StageNo == 3) {
        Gray_Exe();
    }
}

int BG_SpriteSetSub(BG_WORK *offset, NL_SPR_ARG *sprite)
{
    float x;
    float y;
    int xsize;
    int ysize;
    BGEX *p;

    p = &bgex;
    y = offset->posy;
    x = offset->posx;
    PXF_GetSize(offset->SpriteId, &xsize, &ysize);
    if (p->lasty != y) {
        sprite->y = BGWorld2ScreenY(y);
        sprite->zm_y = BGWorld2ScreenY(y + ysize * 100);
        p->lasty = y;
        if (sprite->zm_y < 0.0f) {
            p->lastskip = 1;
            return 0;
        }
        if (sprite->y > 480.0f) {
            p->lastskip = 1;
            return 0;
        }
        p->lastskip = 0;
        p->sety = 1;
    } else if (p->lastskip) {
        return 0;
    }
    sprite->x = BGWorld2ScreenX(x);
    if (sprite->x > 640.0f) {
        return 0;
    }
    sprite->zm_x = BGWorld2ScreenX(x + xsize * 100);
    if (sprite->zm_x < 0.0f) {
        return 0;
    }
    sprite->sprno = offset->SpriteId;
    return 1;
}

int KakuseiFinish(void);
int IsChaseFade(void);
float GetChaseTrnsl(int id);
void gCurrentColorSet(float val);
void gCurrentColorClear(void);
void gStripDraw_Set_z_transl(float z, float trnsl);
void SetBlendingMode(short int mode);
void MagicSquare_Execute(BG_WORK *offset);
void AnotherSpace_Execute(BG_WORK *offset);
void MayCloud_Execute(BG_WORK *offset);
void MayCloudR_Execute(BG_WORK *offset);
void MayPropeller_Execute(BG_WORK *offset);
extern BG_WORK *BgWork;
extern u_int TotalCounter;
extern int StageNo;
extern int BGBeingCount; /* STATIC_EXTERN 0x5cc760 */

void BG_Execute(void)
{
    int i;
    BG_WORK *offset;
    NL_SPR_ARG sprite;
    NL_SPR_ARG *p;
    int count;
    unsigned int flags;
    void (*setspfunc)();
    int funcno;
    int ret;

    if (g_info.Flash) {
        return;
    }
    if (g_info.BattleControlFlag & 0xC0) {
        if (KakuseiFinish()) {
            return;
        }
    }
    if (g_info.BattleControlFlag & 8) {
        return;
    }
    sprite.attr = 5;
    sprite.u0 = 0.0f;
    sprite.v0 = 0.0f;
    sprite.u1 = 1.0f;
    sprite.v1 = 1.0f;
    sprite.trnsl = 1.0f;
    sprite.ang = 0;
    bgex.ischase = IsChaseFade();
    BGLayerAnimetion();
    gCurrentColorSet(bgex.fade[0]);
    SetBlendingMode(0);
    p = &sprite;
    offset = BgWork;
    for (i = 0; i < BGBeingCount; i += count, offset += count) {
        flags = LayerHeaderp[offset->idno];
        count = flags & 0xFFFF;
        if (!(flags & 0x80000000)) {
            continue;
        }
        if (offset->scrollrate < 700) {
            ret = 0;
        } else if (offset->scrollrate < 900) {
            ret = 1;
        } else if (offset->scrollrate < 2000) {
            ret = 2;
        } else {
            ret = 0;
        }
        if (offset->idno == 51) {
            ret = 0;
        }
        if (bgex.fade[ret] <= 0.01f) {
            continue;
        }
        gCurrentColorSet(bgex.fade[ret]);
        p->trnsl = GetChaseTrnsl(ret);
        if (p->trnsl <= 0.0f) {
            continue;
        }
        p->z = offset->priority;
        bgprecalc(offset->scrollrate);
        if (offset->actst & 0x100000) {
            SetBlendingMode(1);
            if (p->trnsl > 0.95f) {
                p->trnsl = 0.95f;
            }
        } else {
            SetBlendingMode(0);
        }
        gStripDraw_Set_z_transl(p->z, p->trnsl);
        bgex.lasty = 1;
        if (offset->idno == 50) {
            MagicSquare_Execute(offset);
            continue;
        } else if (offset->idno == 51) {
            MayCloud_Execute(offset);
            MayCloudR_Execute(offset);
            continue;
        } else if (offset->idno == 52) {
            MayPropeller_Execute(offset);
            continue;
        } else if (offset->idno == 53) {
            AnotherSpace_Execute(offset);
            continue;
        } else if (offset->idno == 54) {
            static short int i = 0;
            float t[3] = { 0.0f, 0.3f, 0.6f };

            if (TotalCounter & 1) {
                i = (i + 1) % 3;
            }
            p->trnsl = t[i];
            gStripDraw_Set_z_transl(p->z, p->trnsl);
        }

        if (offset->actst & 0x40000) {
            funcno = 2;
        } else if (offset->actst & 0x80000) {
            funcno = 1;
        } else {
            funcno = 0;
        }
        if (bgex.ischase && p->trnsl < 1.0f) {
            funcno |= 4;
        }
        if (p->trnsl <= 0.0f) {
            continue;
        }
        setspfunc = bgspset_functb[funcno];
        setspfunc(offset, p, count);
    }
    if (StageNo == 1) {
        RainExecute();
    }
    gCurrentColorClear();
}
