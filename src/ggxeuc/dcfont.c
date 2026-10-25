#include "generated/dcfont.h"
void PXF_SpriteClear(void);
void FadeInit(void);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void PXF_SysSpriteClear(void);
int PXF_GetSysSprite(void);
int GetFileSize(int fid);
void *mmalloc(int size, char *name);
void FileRead(int fid, void *adr);
void LinkAddrTb(u_int *addr);
int IST_Load(int *texadr, int sprid, int palid);
void dfree(void *mem);
char *FileReadSelect(int fid, int chan);
void gSpriteDrawPS(NL_SPR_ARG *sprite);
static DCFONT_DATA df[3] = {
    { 32.0f, 32.0f, 30.0f, 22.0f, 59, 8, { 4.0f, 6.0f } },
    { 32.0f, 32.0f, 18.0f, 10.0f, 60, 8, { 4.0f, 6.0f } },
    { 24.0f, 24.0f, 20.0f, 10.0f, 61, 10, { 3.0f, 4.0f } },
};
void gSpriteDrawPS(NL_SPR_ARG *sprite);
void gSpriteDrawPS(NL_SPR_ARG *sprite);

void DCFontLoad(void)
{
    if (DCFontFile == 0) {
        DCFontFile = mmalloc(GetFileSize(0x9A), "DCFontFile");
        FileRead(0x9A, DCFontFile);
    }
}

void dcfont_set_spno(void)
{
    int n;

    PXF_SysSpriteClear();
    for (n = 0; n < 5; n++) {
        fi.font[n].spno = PXF_GetSysSprite();
    }
}

void dcfont_init(void)
{
    u_int *p;
    int n;

    dcfont_set_spno();
    p = (u_int *)DCFontFile;
    LinkAddrTb(p);
    for (n = 0; p[n]; n++) {
        IST_Load((int *)&p[n], fi.font[n].spno, -1);
    }
}

void FontDrawDC(char *str, float x, float y, float z, int alpha, int id)
{
    float u;
    float v;
    int flg;
    int idx = 0;
    NL_SPR_ARG sprite;
    float flen[3][26] = { 31.0f, 30.0f, 30.0f, 32.0f, 28.0f, 27.0f, 31.0f, 34.0f, 12.0f, 26.0f, 31.0f, 28.0f, 34.0f, 32.0f, 33.0f, 30.0f, 33.0f, 32.0f, 29.0f, 31.0f, 31.0f, 33.0f, 32.0f, 31.0f, 30.0f, 30.0f, 20.0f, 19.0f, 18.0f, 21.0f, 17.0f, 16.0f, 26.0f, 19.0f, 8.0f, 16.0f, 20.0f, 16.0f, 26.0f, 22.0f, 26.0f, 18.0f, 26.0f, 20.0f, 17.0f, 17.0f, 20.0f, 22.0f, 28.0f, 20.0f, 17.0f, 19.0f, 22.0f, 20.0f, 19.0f, 20.0f, 19.0f, 19.0f, 20.0f, 21.0f, 9.0f, 20.0f, 20.0f, 20.0f, 23.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 21.0f, 20.0f, 22.0f, 20.0f, 20.0f, 20.0f };
    float nflen[3][11] = { 25.0f, 22.0f, 26.0f, 27.0f, 26.0f, 27.0f, 26.0f, 25.0f, 26.0f, 30.0f, 30.0f, 21.0f, 15.0f, 19.0f, 18.0f, 21.0f, 18.0f, 19.0f, 16.0f, 20.0f, 23.0f, 24.0f, 20.0f, 21.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 22.0f, 20.0f };

    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.zm_x = df[id].xsize;
    sprite.zm_y = df[id].ysize;
    sprite.ang = 0;
    while (*str) {
        flg = 0;
        if ((unsigned char)(*str - '0') < 10) {
            u = *str - '0';
            v = df[id].pos[0];
            idx = u;
            while (df[id].xnum <= u) {
                u -= df[id].xnum;
                v += 1.0f;
            }
        } else if ((unsigned char)(*str - 'A') < 26) {
            flg = 1;
            u = *str - 'A';
            v = 0;
            idx = u;
            while (df[id].xnum <= u) {
                u -= df[id].xnum;
                v += 1.0f;
            }
        } else if (*str == '-') {
            u = 0;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == '+') {
            u = 1.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == '/') {
            u = 2.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == ':') {
            u = 3.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == '(') {
            u = 4.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == ')') {
            u = 5.0f;
            idx = 10;
            v = df[id].pos[1];
        } else {
            goto next;
        }
        sprite.sprno = fi.font[id].spno;
        u *= df[id].xsize;
        v *= df[id].ysize;
        sprite.u0 = u;
        sprite.v0 = v;
        sprite.u1 = u + df[id].xsize;
        sprite.v1 = v + df[id].ysize;
        gSpriteDrawPS(&sprite);
    next:
        if (flg) {
            sprite.x += flen[id][idx];
        } else if (*str == ' ') {
            sprite.x += df[id].space;
        } else {
            sprite.x += nflen[id][idx];
        }
        str++;
    }
}

void FontDrawDCCenter(char *str, float x, float y, float z, int alpha, int id)
{
    float u;
    float v;
    int flg;
    int xoffset;
    int idx = 0;
    NL_SPR_ARG sprite;
    char *str_temp;
    float flen[3][26] = { 31.0f, 30.0f, 30.0f, 32.0f, 28.0f, 27.0f, 31.0f, 34.0f, 12.0f, 26.0f, 31.0f, 28.0f, 34.0f, 32.0f, 33.0f, 30.0f, 33.0f, 32.0f, 29.0f, 31.0f, 31.0f, 33.0f, 32.0f, 31.0f, 30.0f, 30.0f, 20.0f, 19.0f, 18.0f, 21.0f, 17.0f, 16.0f, 26.0f, 19.0f, 8.0f, 16.0f, 20.0f, 16.0f, 26.0f, 22.0f, 26.0f, 18.0f, 26.0f, 20.0f, 17.0f, 17.0f, 20.0f, 22.0f, 28.0f, 20.0f, 17.0f, 19.0f, 22.0f, 20.0f, 19.0f, 20.0f, 19.0f, 19.0f, 20.0f, 21.0f, 9.0f, 20.0f, 20.0f, 20.0f, 23.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 21.0f, 20.0f, 22.0f, 20.0f, 20.0f, 20.0f };
    float nflen[3][11] = { 25.0f, 22.0f, 26.0f, 27.0f, 26.0f, 27.0f, 26.0f, 25.0f, 26.0f, 30.0f, 30.0f, 21.0f, 15.0f, 19.0f, 18.0f, 21.0f, 18.0f, 19.0f, 16.0f, 20.0f, 23.0f, 24.0f, 20.0f, 21.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 20.0f, 22.0f, 20.0f };

    str_temp = str;
    xoffset = 0;
    for (; *str_temp; str_temp++) {
        flg = 0;
        if ((unsigned char)(*str_temp - '0') < 10) {
            idx = *str_temp - '0';
        } else if ((unsigned char)(*str_temp - 'A') < 26) {
            flg = 1;
            idx = *str_temp - 'A';
        } else if (*str_temp == '-' || *str_temp == '+' || *str_temp == '/' || *str_temp == ':' || *str_temp == '(') {
            idx = 10;
        } else if (*str_temp == ')') {
            idx = 10;
        }
        if (flg) {
            xoffset = xoffset + flen[id][idx];
        } else if (*str_temp == ' ') {
            xoffset = xoffset + df[id].space;
        } else {
            xoffset = xoffset + nflen[id][idx];
        }
    }
    xoffset /= 2;
    sprite.x = x - xoffset;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.zm_x = df[id].xsize;
    sprite.zm_y = df[id].ysize;
    sprite.ang = 0;
    while (*str) {
        flg = 0;
        if ((unsigned char)(*str - '0') < 10) {
            u = *str - '0';
            v = df[id].pos[0];
            idx = u;
            while (df[id].xnum <= u) {
                u -= df[id].xnum;
                v += 1.0f;
            }
        } else if ((unsigned char)(*str - 'A') < 26) {
            flg = 1;
            u = *str - 'A';
            v = 0;
            idx = u;
            while (df[id].xnum <= u) {
                u -= df[id].xnum;
                v += 1.0f;
            }
        } else if (*str == '-') {
            u = 0;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == '+') {
            u = 1.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == '/') {
            u = 2.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == ':') {
            u = 3.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == '(') {
            u = 4.0f;
            idx = 10;
            v = df[id].pos[1];
        } else if (*str == ')') {
            u = 5.0f;
            idx = 10;
            v = df[id].pos[1];
        } else {
            goto next;
        }
        sprite.sprno = fi.font[id].spno;
        u *= df[id].xsize;
        v *= df[id].ysize;
        sprite.u0 = u;
        sprite.v0 = v;
        sprite.u1 = u + df[id].xsize;
        sprite.v1 = v + df[id].ysize;
        gSpriteDrawPS(&sprite);
    next:
        if (flg) {
            sprite.x += flen[id][idx];
        } else if (*str == ' ') {
            sprite.x += df[id].space;
        } else {
            sprite.x += nflen[id][idx];
        }
        str++;
    }
}

void MarkDraw(char *str, int x, int y, int z, int alpha)
{
    int u;
    int v;
    NL_SPR_ARG sprite;
    int udat[9] = { 6, 3, 7, 0, 0, 1, 4, 2, 5 };

    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.attr = 0x205;
    sprite.zm_x = 32.0f;
    sprite.zm_y = 32.0f;
    sprite.ang = 0;
    while (*str) {
        if ((unsigned char)(*str - '1') < 9) {
            v = 7;
            u = udat[*str - '1'];
        } else if (*str == 'a') {
            u = 5;
            v = 5;
        } else if (*str == 'b') {
            u = 4;
            v = 5;
        } else if (*str == 'd') {
            u = 3;
            v = 5;
        } else if (*str == 'h') {
            u = 6;
            v = 5;
        } else if (*str == 'o') {
            u = 2;
            v = 5;
        } else if (*str == 'c') {
            u = 3;
            v = 3;
        } else if (*str == 'e') {
            u = 6;
            v = 6;
        } else if (*str == 'i') {
            u = 4;
            v = 3;
        } else if (*str == 'f') {
            u = 7;
            v = 6;
        } else if (*str == 'j') {
            u = 5;
            v = 6;
        } else if (*str == 'l') {
            u = 7;
            v = 5;
        } else if (*str == 'g') {
            u = 6;
            v = 3;
        } else if (*str == 'k') {
            u = 5;
            v = 3;
        } else if (*str == 'm') {
            u = 4;
            v = 6;
        } else if (*str == 'n') {
            u = 7;
            v = 3;
        } else if (*str == 'A') {
            u = 2;
            v = 3;
        } else if (*str == 'B') {
            u = 0;
            v = 6;
        } else if (*str == 'D') {
            u = 1;
            v = 6;
        } else if (*str == 'H') {
            u = 2;
            v = 6;
        } else {
            goto next;
        }
        sprite.sprno = fi.font[1].spno;
        u = u * 32.0f;
        v = v * 32.0f;
        sprite.u0 = u;
        sprite.v0 = v;
        sprite.u1 = sprite.u0 + 32.0f;
        sprite.v1 = sprite.v0 + 32.0f;
        gSpriteDrawPS(&sprite);
    next:
        str++;
        sprite.z = sprite.z - 0.2f;
    }
}

void MessDraw(char *str, int x, int y, int z, int alpha, int spno)
{
    int u;
    int v;
    float xoff;
    float yoff;
    NL_SPR_ARG sprite;

    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.trnsl = alpha;
    sprite.listType = 1;
    sprite.ang = 0;
    sprite.attr = 0x205;
    while (*str) {
        if (*str == 'a') {
            xoff = 608.0f;
            u = 0;
            yoff = 48.0f;
            v = 0;
        } else if (*str == 'b') {
            xoff = 608.0f;
            u = 0;
            yoff = 48.0f;
            v = 1;
        } else if (*str == 'd') {
            xoff = 608.0f;
            u = 0;
            yoff = 48.0f;
            v = 2;
        } else if (*str == 'h') {
            xoff = 640.0f;
            u = 0;
            yoff = 24.0f;
            v = 6;
        } else if (*str == 'o') {
            xoff = 332.0f;
            u = 0;
            yoff = 24.0f;
            v = 7;
        } else if (*str == 'c') {
            xoff = 96.0f;
            u = 0;
            yoff = 24.0f;
            v = 8;
        } else if (*str == 'e') {
            xoff = 96.0f;
            u = 0;
            yoff = 24.0f;
            v = 9;
        } else {
            goto next;
        }
        sprite.zm_x = xoff;
        sprite.zm_y = yoff;
        sprite.sprno = spno;
        u = u * xoff;
        v = v * yoff;
        sprite.u0 = u;
        sprite.v0 = v;
        sprite.u1 = sprite.u0 + xoff;
        sprite.v1 = sprite.v0 + yoff;
        gSpriteDrawPS(&sprite);
    next:
        str++;
        sprite.z = sprite.z - 0.2;
    }
}

int message_init(void)
{
    int n;
    int no;
    u_int *p;

    if (ErrorFile) {
        dfree(ErrorFile);
    }
    p = (u_int *)(ErrorFile = FileReadSelect(0xB8, 1));
    LinkAddrTb(p);
    no = PXF_GetSysSprite();
    for (n = 0; p[n]; n++) {
        IST_Load((int *)&p[n], no + n, -1);
    }
    return no;
}

void load_font(void)
{
    PXF_SpriteClear();
    FadeInit();
    CockpitLoadInit();
    CreditFontLoad(20);
}
