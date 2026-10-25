#include "generated/objwork.h"
static short SpriteId;
static short SpriteIdBG;
static const short idtb[] = { 1, 2, 3 };
extern CHARACTER_WORK *PlayerWork;
extern CHARACTER_WORK *ObjectWork;
extern CHARACTER_WORK *EffectWork;
extern CHARACTER_WORK *CockpitWork;
extern BG_WORK *BgWork;
extern DAMAGEPARAM *HitParamWork;
extern DAMAGEPARAM *DamageParamWork;
extern int EffectLiveCount;
extern int KunsyouLiveCount;
extern int CellTopAddrByIdno[144];
extern int GclTopAddrByIdno[144];
extern int PalTopAddrByIdno[144];
extern int InstTbAddrByIdno[144];
extern u_short FrameBufferIdByIdno[144];
extern u_char FrameBufferPalIdByIdno[144];
extern void (*SearchExecProcAddr)();
short int GetSpriteIdTop(short unsigned int max);
short int GetSpriteIdTopBG(short unsigned int max);
void InitSpriteIdBG(void);
int chk_douchar_ex(int idno, int side);
int SetCellTopAndGclTop(short int side, u_short idno, u_int *cell, u_int *patt);
int SetPalTop(short int side, u_short idno, u_int *pal);
int SetInstTb(short int side, u_short idno, u_int *inst);
char *GetCommonPaletteAddr(CHARACTER_WORK *offset, int no);
unsigned int *GetInstTbAddr(short int side, short unsigned int idno);
int CharacterWork_InitialSub(CHARACTER_WORK *offset, short unsigned int idno, short int side);
CHARACTER_WORK *CharacterWork_Initial(unsigned int InitId, short unsigned int idno);
void CharacterSpriteFree(CHARACTER_WORK *offset);
CHARACTER_WORK *SearchEffectWork(CHARACTER_WORK *off, short unsigned int idno);
CHARACTER_WORK *SearchEffectWorkLocalid_IgnorePadid(CHARACTER_WORK *off, short unsigned int idno, int Localid);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
CHARACTER_WORK *SearchCockpitObjectWork(CHARACTER_WORK *off, short unsigned int idno);
CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off);
CHARACTER_WORK *SearchObjectWorkActnoLocalid(CHARACTER_WORK *off, short unsigned int idno, int actno, int Localid);
CHARACTER_WORK *SearchObjectWorkActnowithFuncExec(CHARACTER_WORK *off, short unsigned int idno, int actno, void (*Func)());
CHARACTER_WORK *SearchBall(CHARACTER_WORK *off);
void EliminateTransfaObject(CHARACTER_WORK *off, unsigned char padid);
void SetSearchExecProcAddr(void *addr);
CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno);
void DeleteEffectByItigeki(CHARACTER_WORK *offset);
void EliminateRoundEnd(void);
void EliminateChildEffect(CHARACTER_WORK *off);
int GetBGObjCnt(void);
BG_WORK *BgWork_Initial(short int idno, BGINIT_ARG *arg);
void AllBgWork_Clear(void);
int CharacterWork_Clear(CHARACTER_WORK *offset);
int AllCharacterWorkInitial(void);
void AllCharacterWorkRelease(void);
void ObjectKill(CHARACTER_WORK *offset);
void LoadFrameBufferObject(short int idno);
void set_gcltop(int i, int id);
void set_celltop(int i, int id);

short int GetSpriteIdTop(short unsigned int max)
{
    short id = SpriteId;
    SpriteId += max;
    return id;
}

short int GetSpriteIdTopBG(short unsigned int max)
{
    short id = SpriteIdBG;
    SpriteIdBG += max;
    return id;
}

void InitSpriteIdBG(void)
{
    SpriteIdBG = 0x180;
}

extern short int UseCharacter1p2p[];
extern USER_SETTING userinfo;
int chk_douchar_ex(int idno, int side)
{
    if (idno < 19 && side > 0 && UseCharacter1p2p[0] == UseCharacter1p2p[1]) {
        if ((userinfo.gguse[0] & 2) != (userinfo.gguse[1] & 2)) return 1;
    }
    return 0;
}

int SetCellTopAndGclTop(short int side, u_short idno, u_int *cell, u_int *patt)
{
    if (idno >= 144) {
        debugf("!!!!!SetCellTopAndGclTop Danger %d %d %x %x\n", side, idno, cell, patt);
        return 0;
    }
    if (chk_douchar_ex(idno, side)) idno = 19;
    CellTopAddrByIdno[idno] = (int)cell;
    GclTopAddrByIdno[idno] = (int)patt;
    return 0;
}

void LinkAddrTb(u_int *addr);
int SetPalTop(short int side, u_short idno, u_int *pal)
{
    if (*pal == 0) return 1;
    LinkAddrTb(pal);
    if (chk_douchar_ex(idno, side)) idno = 19;
    PalTopAddrByIdno[idno] = (int)pal;
    return 0;
}

void *GetInstTb(void *in);
int SetInstTb(short int side, u_short idno, u_int *inst)
{
    idno = chk_douchar_ex(idno, side) ? 19 : idno;
    InstTbAddrByIdno[idno] = (int)GetInstTb(inst);
    return 0;
}

char *GetCommonPaletteAddr(CHARACTER_WORK *offset, int no)
{
    unsigned int *pal;
    int idno;

    idno = offset->idno;
    if (chk_douchar_ex(idno, offset->padid)) idno = 19;
    pal = (unsigned int *)PalTopAddrByIdno[idno];
    if (pal) return (char *)pal[no];
    return 0;
}

unsigned int *GetInstTbAddr(short int side, short unsigned int idno)
{
    idno = chk_douchar_ex(idno, side) ? 19 : idno;
    return (unsigned int *)InstTbAddrByIdno[idno];
}

void LinkAddrTb(u_int *addr);
void SetBgObj(CHARACTER_WORK *offset);
void SetPriority(CHARACTER_WORK *offset, int PriAdd);
int CharacterWork_InitialSub(CHARACTER_WORK *offset, short unsigned int idno, short int side)
{
    u_short id;

    offset->idno = idno;
    offset->actst = 1;
    id = chk_douchar_ex(idno, side) ? 19 : idno;
    offset->CellTop = (u_int *)CellTopAddrByIdno[id];
    offset->GclTop = (u_int *)GclTopAddrByIdno[id];
    offset->CommonPalette = GetCommonPaletteAddr(offset, 0);
    offset->actnonext = -1;
    offset->LandBreakActno = -1;
    SetPriority(offset, 0);
    offset->LastCellNo = 0xFFFF;
    offset->LastImageNo = 0xFFFF;
    if (FrameBufferIdByIdno[idno]) {
        offset->FBSpriteIdTop = FrameBufferIdByIdno[idno];
        offset->PalId = FrameBufferPalIdByIdno[idno];
    } else {
        offset->FBSpriteIdTop = 0;
    }
    offset->SpriteId = -1;
    offset->trans = 0xFF;
    offset->LocalPalette = 0;
    offset->PaletteFlag = 0;
    if (idno >= 0x80 && idno < 0x88) SetBgObj(offset);
    if (idno < 0x20) offset->TexSizeMax = 9;
    else if (idno == 0x23) offset->TexSizeMax = 0x98;
    else if (idno == 0x37) offset->TexSizeMax = 8;
    else if (idno == 0x4B) offset->TexSizeMax = 9;
    else if (idno == 0x20) offset->TexSizeMax = 8;
    else if (idno == 0x52) offset->TexSizeMax = 8;
    else if (idno == 0x54) offset->TexSizeMax = 9;
    else offset->TexSizeMax = 3;
    offset->actwork.l = 0;
    if (offset->CellTop) LinkAddrTb(offset->CellTop);
    if (offset->GclTop) LinkAddrTb(offset->GclTop);
    return 0;
}

int CharacterWork_InitialSub(CHARACTER_WORK *offset, short unsigned int idno, short int side);
CHARACTER_WORK *CharacterWork_Initial(unsigned int InitId, short unsigned int idno)
{
    CHARACTER_WORK *offset;
    int i;

    switch (InitId) {
    case 0x10:
        for (i = 0; i < 2; i++) {
            offset = &PlayerWork[i];
            if (!(offset->actst & 1)) {
                CharacterWork_InitialSub(offset, idno, i);
                return offset;
            }
        }
        break;
    case 0x11:
        offset = PlayerWork;
        if (offset->actst & 1) return 0;
        CharacterWork_InitialSub(offset, idno, 0);
        return offset;
    case 0x12:
        offset = PlayerWork + 1;
        if (offset->actst & 1) return 0;
        CharacterWork_InitialSub(offset, idno, 1);
        return offset;
    case 0x20:
        for (i = 0; i < 0x30; i++) {
            offset = &ObjectWork[i];
            if (!(offset->actst & 1)) {
                CharacterWork_InitialSub(offset, idno, -1);
                return offset;
            }
        }
        break;
    case 0x30:
        for (i = 0; i < 0xA0; i++) {
            offset = &EffectWork[i];
            if (!(offset->actst & 1)) {
                CharacterWork_InitialSub(offset, idno, -1);
                return offset;
            }
        }
        break;
    case 0x31:
        for (i = 0; i < 0xA0; i++) {
            offset = &EffectWork[i];
            if (!(offset->actst & 1)) {
                CharacterWork_InitialSub(offset, idno, -1);
                return offset;
            }
        }
        break;
    case 0x40:
        for (i = 0; i < 0x10; i++) {
            offset = &CockpitWork[i];
            if (!(offset->actst & 1)) {
                CharacterWork_InitialSub(offset, idno, -1);
                return offset;
            }
        }
        break;
    }
    return 0;
}

void PXF_Free(int id, int pal);
void CharacterSpriteFree(CHARACTER_WORK *offset)
{
    int i;

    for (i = 0; i < offset->SpriteIdMax; i++) {
        PXF_Free(offset->SpriteIdTop + i, 0);
    }
}

CHARACTER_WORK *SearchEffectWork(CHARACTER_WORK *off, short unsigned int idno)
{
    int i;

    for (i = 0; i < 0xA0; i++) {
        if ((EffectWork[i].actst & 1) && EffectWork[i].idno == idno && EffectWork[i].padid == off->padid) {
            return &EffectWork[i];
        }
    }
    return 0;
}

CHARACTER_WORK *SearchEffectWorkLocalid_IgnorePadid(CHARACTER_WORK *off, short unsigned int idno, int Localid)
{
    CHARACTER_WORK *offset;
    int i;

    offset = EffectWork;
    i = 0xA0;
    while (i--) {
        if ((offset->actst & 1) && offset->idno == idno && offset->Localid == Localid) {
            return offset;
        }
        offset++;
    }
    return 0;
}

CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno)
{
    CHARACTER_WORK *offset;
    int i;

    offset = ObjectWork;
    i = 0x30;
    while (i--) {
        if ((offset->actst & 1) && offset->padid == off->padid && offset->idno == idno) {
            return offset;
        }
        offset++;
    }
    return 0;
}

CHARACTER_WORK *SearchCockpitObjectWork(CHARACTER_WORK *off, short unsigned int idno)
{
    CHARACTER_WORK *offset;
    int i;

    offset = CockpitWork;
    i = 0x10;
    while (i--) {
        if ((offset->actst & 1) && offset->padid == off->padid && offset->idno == idno) {
            return offset;
        }
        offset++;
    }
    return 0;
}

CHARACTER_WORK *GetParentPlayer(CHARACTER_WORK *off)
{
    return &PlayerWork[off->padid];
}

CHARACTER_WORK *SearchObjectWorkActnoLocalid(CHARACTER_WORK *off, short unsigned int idno, int actno, int Localid)
{
    CHARACTER_WORK *offset;
    int i;
    unsigned char padid;

    padid = off->padid;
    offset = ObjectWork;
    for (i = 0; i < 0x30; i++, offset++) {
        if ((offset->actst & 1) && offset->idno == idno && offset->padid == padid && offset->actno == actno && offset->Localid == Localid) {
            return offset;
        }
    }
    return 0;
}

CHARACTER_WORK *SearchObjectWorkActnowithFuncExec(CHARACTER_WORK *off, short unsigned int idno, int actno, void (*Func)())
{
    CHARACTER_WORK *offset;
    int i;
    unsigned char padid;

    padid = off->padid;
    for (i = 0; i < 0x30; i++) {
        offset = &ObjectWork[i];
        if ((offset->actst & 1) && offset->idno == idno && offset->padid == padid && offset->actno == actno) {
            Func(offset);
        }
    }
    return 0;
}

CHARACTER_WORK *SearchBall(CHARACTER_WORK *off)
{
    CHARACTER_WORK *offset;
    int i;

    offset = ObjectWork;
    for (i = 0; i < 0x30; i++, offset++) {
        if ((offset->actst & 1) && (offset->attackst & 0x8000) && offset->idno == 0x22 && offset->padid == off->padid) {
            debugf("SearchBall Found\n");
            return offset;
        }
    }
    debugf("SearchBall Not Found\n");
    return 0;
}

void EliminateTransfaObject(CHARACTER_WORK *off, unsigned char padid)
{
    CHARACTER_WORK *offset;
    int i;

    for (i = 0; i < 0x30; i++) {
        offset = &ObjectWork[i];
        if ((offset->actst & 1) && offset->padid == padid && (offset->parentFlag & 4)) {
            CharacterSpriteFree(offset);
            offset->actst |= 0x80000000;
        }
    }
}

void SetSearchExecProcAddr(void *addr)
{
    SearchExecProcAddr = addr;
}

CHARACTER_WORK *SearchCharacterExecute(CHARACTER_WORK *off, short unsigned int idno, int actno, unsigned int flag)
{
    CHARACTER_WORK *offset;
    int cnt;

    if (flag & 1) {
        offset = ObjectWork - 1;
        cnt = 0x30;
    } else {
        offset = EffectWork - 1;
        cnt = 0xA0;
    }
    while (cnt--) {
        offset++;
        if (!(offset->actst & 1)) continue;
        if (!(flag & 0x40) && offset->padid != off->padid) continue;
        if ((flag & 2) && offset->idno != idno) continue;
        if ((flag & 4) && offset->actno != actno) continue;
        if (flag & 8) {
            if (!(offset->parentFlag & 0x80)) continue;
            if (offset->parentWork != off) continue;
        }
        if ((flag & 0x80) && offset->parentWork != off) continue;
        if ((flag & 0x40) && !(offset->idno >= 0x20 && offset->idno < 0x7E)) continue;
        if (flag & 0x10) {
            offset->actst |= 0x80000000;
        } else if (flag & 0x20) {
            SearchExecProcAddr(offset);
        } else {
            return offset;
        }
    }
    SearchExecProcAddr = 0;
    return 0;
}

CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno)
{
    return SearchCharacterExecute(off, idno, actno, 6);
}

CHARACTER_WORK *SearchObjectWorkActno(CHARACTER_WORK *off, short unsigned int idno, int actno)
{
    return SearchCharacterExecute(off, idno, actno, 7);
}

CHARACTER_WORK *GetEnemyPlayer(CHARACTER_WORK *offset);
void DeleteEffectByItigeki(CHARACTER_WORK *offset)
{
    CHARACTER_WORK *eoffset;

    eoffset = GetEnemyPlayer(offset);
    SearchCharacterExecute(eoffset, 0x52, 0, 0x13);
    SearchCharacterExecute(eoffset, 0x32, 1, 0x13);
    SearchCharacterExecute(eoffset, 0x50, 0, 0x13);
    SearchCharacterExecute(eoffset, 0x44, 0, 0x13);
    SearchCharacterExecute(eoffset, 0x48, 0, 0x13);
    SearchCharacterExecute(eoffset, 0x48, 0, 0x12);
    SearchCharacterExecute(eoffset, 0x21, 0xD, 0x13);
    SearchCharacterExecute(eoffset, 0x34, 0xD, 0x13);
    SearchCharacterExecute(eoffset, 0x34, 0xD, 0x12);
    SearchCharacterExecute(eoffset, 0x2E, 0xD, 0x13);
    SearchCharacterExecute(eoffset, 0x2E, 0xD, 0x12);
    SearchCharacterExecute(eoffset, 0x2B, 1, 0x13);
    SearchCharacterExecute(eoffset, 0x2B, 1, 0x12);
    SearchCharacterExecute(eoffset, 0x4A, 1, 0x13);
    SearchCharacterExecute(eoffset, 0x4A, 1, 0x12);
    SearchCharacterExecute(eoffset, 0x41, 1, 0x13);
    SearchCharacterExecute(eoffset, 0x41, 1, 0x12);
    SearchCharacterExecute(eoffset, 0x55, 0, 0x11);
}

void EliminateRoundEnd(void)
{
    SearchCharacterExecute(0, 0, 0, 0x51);
    SearchCharacterExecute(0, 0, 0, 0x50);
}

void EliminateChildEffect(CHARACTER_WORK *off)
{
    SearchCharacterExecute(0, 0, 0, 0x58);
}

int GetBGObjCnt(void)
{
    CHARACTER_WORK *offset;
    int i;
    int cnt;

    cnt = 0;
    offset = EffectWork;
    i = 0xA0;
    while (i--) {
        if ((offset->actst & 1) && (offset->SpriteFlag & 0x800)) cnt++;
        offset++;
    }
    return cnt;
}

BG_WORK *BgWork_Initial(short int idno, BGINIT_ARG *arg)
{
    int i;

    for (i = 0; i < 640; i++) {
        if (!(BgWork[i].actst & 1)) {
            BgWork[i].actst = 9;
            BgWork[i].CellNo = 0;
            BgWork[i].LastCellNo = 0;
            BgWork[i].idno = idno;
            BgWork[i].CellAddr = arg->CellAddr;
            BgWork[i].SpriteId = arg->SpriteIdTop;
            BgWork[i].posx = arg->posx;
            BgWork[i].posy = arg->posy;
            BgWork[i].priority = arg->priority;
            BgWork[i].scrollrate = arg->scrollrate;
            BgWork[i].AnimeAddrTop = 0;
            BgWork[i].AnimeAddr = 0;
            BgWork[i].frame = 0;
            return &BgWork[i];
        }
    }
    return 0;
}

void AllBgWork_Clear(void)
{
    int i;

    for (i = 0; i < 640; i++) {
        BgWork[i].actst = 0;
        BgWork[i].CellAddr = 0;
        BgWork[i].CellNo = 0;
        BgWork[i].LastCellNo = 0;
        BgWork[i].SpriteId = 0;
        BgWork[i].posx = 0;
        BgWork[i].posy = 0;
        BgWork[i].priority = 0;
        BgWork[i].scrollrate = 0;
    }
}

void ExCollisionClear(CHARACTER_WORK *offset);
int CharacterWork_Clear(CHARACTER_WORK *offset)
{
    int i;
    int *p;

    ExCollisionClear(offset);
    CharacterSpriteFree(offset);
    p = (int *)offset;
    for (i = 0; i < sizeof(CHARACTER_WORK); i += 4) {
        *p++ = 0;
    }
    return 0;
}

int PXF_GetFreePaletteNum(int num);
void PXF_PaletteClear(void);
void *smalloc(int size, char *name);
int AllCharacterWorkInitial(void)
{
    CHARACTER_WORK *offset;
    int i;

    if (PlayerWork == 0) {
        ObjectWork = smalloc(sizeof(CHARACTER_WORK) * 48, "ObjectWork");
        EffectWork = smalloc(sizeof(CHARACTER_WORK) * 160, "EffectWork");
        BgWork = smalloc(sizeof(BG_WORK) * 640, "BgWork");
        HitParamWork = smalloc(sizeof(DAMAGEPARAM) * 50, "HitParamWork");
        DamageParamWork = smalloc(sizeof(DAMAGEPARAM) * 50, "DamageParamWork");
        PlayerWork = smalloc(sizeof(CHARACTER_WORK) * 2, "PlayerWork");
        CockpitWork = smalloc(sizeof(CHARACTER_WORK) * 16, "CockpitWork");
    }
    SpriteId = 0x20;
    PXF_PaletteClear();
    for (i = 0; i < 2; i++) {
        offset = &PlayerWork[i];
        offset->ExColliAddr = 0;
        offset->SpriteIdMax = 0;
        CharacterWork_Clear(offset);
        offset->SpriteIdMax = 2;
        offset->SpriteIdTop = GetSpriteIdTop(2);
        offset->PalId = PXF_GetFreePaletteNum(16);
        offset->HitParam = &HitParamWork[i];
        offset->DamParam = &DamageParamWork[i];
    }
    for (i = 0; i < 48; i++) {
        offset = &ObjectWork[i];
        offset->ExColliAddr = 0;
        offset->SpriteIdMax = 0;
        CharacterWork_Clear(offset);
        offset->SpriteIdMax = 2;
        offset->SpriteIdTop = GetSpriteIdTop(2);
        offset->HitParam = &HitParamWork[i] + 2;
        offset->DamParam = &DamageParamWork[i] + 2;
    }
    for (i = 0; i < 160; i++) {
        offset = &EffectWork[i];
        offset->ExColliAddr = 0;
        offset->SpriteIdMax = 0;
        CharacterWork_Clear(offset);
        offset->HitParam = 0;
        offset->DamParam = 0;
    }
    for (i = 0; i < 16; i++) {
        offset = &CockpitWork[i];
        offset->ExColliAddr = 0;
        offset->SpriteIdMax = 0;
        CharacterWork_Clear(offset);
        offset->SpriteIdMax = 2;
        offset->SpriteIdTop = GetSpriteIdTop(2);
        offset->HitParam = 0;
        offset->DamParam = 0;
    }
    for (i = 0; i < 144; i++) {
        CellTopAddrByIdno[i] = 0;
        GclTopAddrByIdno[i] = 0;
        PalTopAddrByIdno[i] = 0;
        FrameBufferIdByIdno[i] = 0;
        InstTbAddrByIdno[i] = 0;
    }
    return 0;
}

void dfree(void *mem);
void AllCharacterWorkRelease(void)
{
    if (PlayerWork) {
        dfree(PlayerWork);
        PlayerWork = 0;
        dfree(ObjectWork);
        ObjectWork = 0;
        dfree(EffectWork);
        EffectWork = 0;
        dfree(CockpitWork);
        CockpitWork = 0;
        dfree(BgWork);
        BgWork = 0;
        dfree(HitParamWork);
        HitParamWork = 0;
        dfree(DamageParamWork);
        DamageParamWork = 0;
    }
}

void ObjectKill(CHARACTER_WORK *offset)
{
    DAMAGEPARAM *hpm;
    DAMAGEPARAM *dpm;
    short max, top;
    char pal;

    max = offset->SpriteIdMax;
    top = offset->SpriteIdTop;
    pal = offset->PalId;
    hpm = offset->HitParam;
    dpm = offset->DamParam;
    CharacterWork_Clear(offset);
    offset->SpriteIdMax = max;
    offset->SpriteIdTop = top;
    offset->PalId = pal;
    offset->HitParam = hpm;
    offset->DamParam = dpm;
}

int IST_Load(int *texadr, int sprid, int palid);
void LinkAddrTb(u_int *addr);
void LoadFrameBufferObject(short int idno)
{
    int Max;
    int Cnt;
    int SprtIDTop;
    int PalIDTop;
    int *addr;

    addr = (int *)CellTopAddrByIdno[idno];
    LinkAddrTb((u_int *)addr);
    Max = 0;
    PalIDTop = -1;
    while (addr[Max]) Max++;
    SprtIDTop = GetSpriteIdTop(Max);
    for (Cnt = 0; Cnt < Max; Cnt++) {
        PalIDTop = -1;
        IST_Load(addr + Cnt, SprtIDTop + Cnt, PalIDTop);
    }
    FrameBufferIdByIdno[idno] = SprtIDTop;
    FrameBufferPalIdByIdno[idno] = PalIDTop;
}

void set_gcltop(int i, int id)
{
    PlayerWork[i].GclTop = (u_int *)GclTopAddrByIdno[id];
}

void set_celltop(int i, int id)
{
    PlayerWork[i].CellTop = (u_int *)CellTopAddrByIdno[id];
}
