#include "generated/profile.h"

void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void dfree(void *mem);
int VsCpuCtrl(void);
void JobChange(int job);
char *FileReadSelect(int fid, int chan);
extern short int UseCharacter1p2p[];
extern SYS_COND sc;
extern int SubJob;
extern int SubJobCount;
int FileReadCheck(void);
void pack_allfree(void);
void sound_load_Common(void);
void Read_Playdat(int size);
int GetCPUCharacterFlag(void);
void LinkAddrTb(u_int *addr);
int IST_Load(int *texadr, int sprid, int palid);
void dcfont_init(void);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void FadeInit(void);
void BGM_Request(int no, int loop);
void set_bgmvol(void);
void JobNext(void);
extern int rannyuuflag;
int IsRannyuu(void);
int IsCancel(void);
void DispCreditMsg(void);
void InitCancelFlag(void);
void EnterFight_Main(void);
unsigned int gStripGetFadeColor(unsigned int color);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);

void ProfileLoad(void) {
    if (ProfileFile) dfree(ProfileFile);
    ProfileFile = FileReadSelect(0xa3, 1);
}

void ProfileInit(void) {
    int player;
    int *p;
    int i;
    if (FileReadCheck()) {
        pack_allfree();
        sound_load_Common();
        Read_Playdat(0x100);
        player = GetCPUCharacterFlag() != 0;
        p = (int *)ProfileFile;
        LinkAddrTb((u_int *)p);
        p = (int *)*p;
        LinkAddrTb((u_int *)p);
        for (i = 0; *p; i++, p++) {
            if (i == UseCharacter1p2p[player] + 1 || i == 0 || i == 1 || i == 20) {
                IST_Load(p, i + 0x20, -1);
            }
        }
        p = (int *)ProfileFile;
        for (i = 0; i < UseCharacter1p2p[player]; i++) p++;
        p = (int *)*p;
        LinkAddrTb((u_int *)p);
        IST_Load(p, 0x42, -1);
        dcfont_init();
        CockpitLoadInit();
        CreditFontLoad(20);
        FadeInit();
        BGM_Request(0x18, 1);
        set_bgmvol();
        SubJob = 0;
        sc.grpvar[3] = 0;
        SubJobCount = 0;
        JobNext();
    }
}

void ProfileEnd(void) {
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    dfree(ProfileFile);
    ProfileFile = 0;
    JobChange(VsCpuCtrl());
}

void ProfileMain(void)
{
    static int x = 0;
    static int y = 0;
    static int fx = 0;
    static int py = 0;
    static float h = 0;
    static float fy = 0;
    int color;
    int player;

    player = GetCPUCharacterFlag() != 0;
    if (SubJob == 0) {
        if (SubJobCount == 0) {
            y = 932;
            fx = 640;
            x = 0;
            h = 0.0f;
            fy = 0.0f;
            py = 0;
            rannyuuflag = 0;
            InitCancelFlag();
            SubJobCount++;
            return;
        }
        Read_Playdat(0x80);
        rannyuuflag |= IsRannyuu();
        if (IsCancel()) {
            ProfileEnd();
            return;
        }
        DispCreditMsg();
        if (SubJobCount < 21) {
            x += 16;
            if (x > 320) x = 320;
        }
        if (SubJobCount >= 20 && SubJobCount < 42) {
            y -= 25;
            if (y < 420) y = 420;
        }
        if (SubJobCount >= 73 && SubJobCount < 94) {
            fx -= 32;
            if (fx < 0) fx = 0;
        }
        if (SubJobCount >= 98 && SubJobCount < 110) {
            h = h + 1.0 / 12.0;
            if (h > 1.0) h = 1.0f;
            fy = fy + 1.5;
            if (fy > 18.0) fy = 18.0f;
        }
        if (SubJobCount >= 241) py += 7;
        color = gStripGetFadeColor(0xffffffff);
        polydraw_FBox(0.0f, 0.0f, x, 20.0f, 9.0f, 0xff000000);
        polydraw_FBox(0.0f, 20.0f, x, 21.0f, 7.0f, color);
        polydraw_FBox(640 - x, 0.0f, 640.0f, 20.0f, 9.0f, 0xff000000);
        polydraw_FBox(640 - x, 20.0f, 640.0f, 21.0f, 7.0f, color);
        polydraw_FBox(0.0f, 419.0f, x, 480.0f, 9.0f, 0xff000000);
        polydraw_FBox(0.0f, 420.0f, x, 421.0f, 7.0f, color);
        polydraw_FBox(640 - x, 419.0f, 640.0f, 480.0f, 9.0f, 0xff000000);
        polydraw_FBox(640 - x, 420.0f, 640.0f, 421.0f, 7.0f, color);
        EasySpriteDrawEX(0x20, 0, 0x114, 15.0f, 0x20d, 1.0f);
        EasySpriteDrawEX(UseCharacter1p2p[player] + 0x21, 4, 0xf0, 13.0f, 0x20d, 1.0f);
        EasySpriteDrawEX(0x42, 0x102, y, 13.0f, 0x20d, 1.0f);
        EasySpriteDrawEX(0x21, 0, 0x1e0, 17.0f, 0x20d, 1.0f);
        Profile_FontDraw(fx, py + 0x114, 11.0f, 1.0f, techcode[UseCharacter1p2p[player] - 1][0]);
        Profile_FontDraw(fx, py + 0x138, 11.0f, 1.0f, techcode[UseCharacter1p2p[player] - 1][1]);
        Profile_FontDraw(fx, py + 0x15c, 11.0f, 1.0f, techcode[UseCharacter1p2p[player] - 1][2]);
        Profile_FontDraw(fx, py + 0x180, 11.0f, 1.0f, techcode[UseCharacter1p2p[player] - 1][3]);
        Profile_FontDraw(fx + 8, fy + 276.0f + py, 11.0f, h, cmndcode[UseCharacter1p2p[player] - 1][0]);
        Profile_FontDraw(fx + 8, fy + 312.0f + py, 11.0f, h, cmndcode[UseCharacter1p2p[player] - 1][1]);
        Profile_FontDraw(fx + 8, fy + 348.0f + py, 11.0f, h, cmndcode[UseCharacter1p2p[player] - 1][2]);
        Profile_FontDraw(fx + 8, fy + 384.0f + py, 11.0f, h, cmndcode[UseCharacter1p2p[player] - 1][3]);
        SubJobCount++;
        if (rannyuuflag) {
            if (SubJobCount > 280) SubJobCount = 280;
        }
        if (SubJobCount > 280) {
            SubJobCount = 0;
            SubJob++;
            y = 0;
        }
    } else {
        Read_Playdat(0x80);
        if (IsCancel()) {
            ProfileEnd();
            return;
        }
        color = gStripGetFadeColor(0xffffffff);
        DispCreditMsg();
        polydraw_FBox(0.0f, 0.0f, 640.0f, y, 5.0f, 0xff000000);
        polydraw_GBox(0.0f, 0.0f, 640.0f, y + 48, 5.0f, 0xff000000, 0xff000000, 0x1000000, 0x1000000);
        polydraw_FBox(0.0f, 480 - y, 640.0f, 480.0f, 5.0f, 0xff000000);
        polydraw_GBox(0.0f, 432 - y, 640.0f, 480.0f, 5.0f, 0x1000000, 0x1000000, 0xff000000, 0xff000000);
        polydraw_FBox(0.0f, 0.0f, 640.0f, 20.0f, 9.0f, 0xff000000);
        polydraw_FBox(0.0f, 20.0f, 640.0f, 21.0f, 7.0f, color);
        polydraw_FBox(0.0f, 419.0f, 640.0f, 480.0f, 9.0f, 0xff000000);
        polydraw_FBox(0.0f, 420.0f, 640.0f, 421.0f, 7.0f, color);
        EasySpriteDrawEX(0x20, 0, 0x114, 15.0f, 0x20d, 1.0f);
        EasySpriteDrawEX(UseCharacter1p2p[player] + 0x21, 4, 0xf0, 13.0f, 0x20d, 1.0f);
        EasySpriteDrawEX(0x42, 0x102, 0x1a4, 13.0f, 0x20d, 1.0f);
        EasySpriteDrawEX(0x21, 0, 0x1e0, 17.0f, 0x20d, 1.0f);
        y = y + 15.0;
        if (++SubJobCount >= 18) ProfileEnd();
    }
    if (rannyuuflag) EnterFight_Main();
}

void PXF_GetSize(int id, int *x, int *y);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);

void Profile_FontDraw(int x, int y, float z, float h, char *str)
{
    NL_SPR_ARG sprite;
    char *p;
    float u0 = 0.0f, u1, v1 = 1.0f, v0 = u0;
    int xs, ys;

    p = str;
    PXF_GetSize(0x34, &xs, &ys);
    sprite.sprno = 0x34;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_y = h * 24.0 / (float)ys;
    sprite.ang = 0;
    sprite.listType = 1;
    sprite.trnsl = 1.0f;
    sprite.attr = 0xd;
    for (; *p; p++) {
        if (*p < '0' || *p > 'p') {
            v0 = 0.0f;
            u0 = v0;
        } else {
            if (*p < '8') {
                v0 = 0.0f;
                u0 = (*p - '0') * 24;
            } else if (*p < 'A') {
                v0 = 24.0f;
                u0 = (*p - '8') * 24;
            } else if (*p < 'I') {
                v0 = 48.0f;
                u0 = (*p - 'A') * 24;
            } else if (*p < 'Q') {
                v0 = 72.0f;
                u0 = (*p - 'I') * 24;
            } else if (*p < 'Y') {
                v0 = 96.0f;
                u0 = (*p - 'Q') * 24;
            } else if (*p < 'a') {
                v0 = 120.0f;
                u0 = (*p - 'Y') * 24;
            } else if (*p < 'i') {
                v0 = 144.0f;
                u0 = (*p - 'a') * 24;
            } else if (*p < 'k') {
                v0 = 168.0f;
                u0 = (*p - 'i') * 24;
            } else if (*p == 'k') {
                u0 = 82.0f;
                v0 = 120.0f;
            } else if (*p == 'l') {
                u0 = 108.0f;
                v0 = 120.0f;
            } else if (*p == 'm') {
                u0 = 52.0f;
                v0 = 120.0f;
            }
        }
        if (*p == 'k') {
            u1 = u0 + 26.0;
            sprite.zm_x = 26.0 / (float)xs;
        } else if (*p == 'l') {
            u1 = u0 + 46.0;
            sprite.zm_x = 46.0 / (float)xs;
        } else if (*p == 'm') {
            u1 = u0 + 16.0;
            sprite.zm_x = 16.0 / (float)xs;
        } else {
            u1 = u0 + 24.0;
            sprite.zm_x = 24.0 / (float)xs;
        }
        v1 = v0 + 24.0;
        sprite.u0 = u0 / xs;
        sprite.u1 = u1 / xs;
        sprite.v0 = (v0 + (1.0 - h) * 24.0) / (float)ys;
        sprite.v1 = v1 / ys;
        if (*p != 'p') gSpriteDraw_NaomiLibVersion(&sprite);
        if (*p == 'k') sprite.x = sprite.x + 24.0;
        else if (*p == 'l') sprite.x = sprite.x + 46.0;
        else if (*p == 'j') sprite.x = sprite.x + 24.0;
        else if (*p == 'm') sprite.x = sprite.x + 16.0;
        else if (*p == 'p') sprite.x = sprite.x + 12.0;
        else sprite.x = sprite.x + 18.0;
    }
}
