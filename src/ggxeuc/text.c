#include "generated/text.h"

int strlen(const char *s);

void TextPutS(u_int x, u_int y, u_char *str)
{
    TextOpen(strlen(str));
    while (*str) {
        TextPutC(x++, y, *str++);
    }
}

void TextColor(int rgb)
{
    text_color.r = (rgb >> 16) & 0xFF;
    text_color.g = (rgb >> 8) & 0xFF;
    text_color.b = rgb & 0xFF;
}

void TextPutX(u_int x, u_int y, u_long data, u_int num)
{
    TextOpen(num);
    x += num - 1;
    while (num) {
        TextPutC(x, y, "0123456789ABCDEF"[data & 0xF]);
        x--;
        data >>= 4;
        num--;
    }
}

void TextPutD(u_int x, u_int y, long data, u_int num)
{
    int sign;

    TextOpen(num);
    sign = 0;
    x += num - 1;
    if (data < 0) {
        sign = 1;
        data = -data;
        num--;
    }
    do {
        if (num == 0) {
            break;
        }
        num--;
        TextPutC(x, y, data % 10 + '0');
        x--;
    } while ((data /= 10) != 0);
    if (sign > 0) {
        TextPutC(x, y, '-');
    }
    while (num) {
        TextPutC(x, y, ' ');
        num--;
    }
}

void PacketPointerSet(int id, u_long *ptr);
u_long *PacketPointerGet(int id);

void TextPutC(u_int x, u_int y, u_char c)
{
    int u;
    int v;
    u_long *ptr;

    u = (c & 0xF) << 7;
    v = (c >> 4) << 7;
    ptr = PacketPointerGet(3);
    x = (x << 8) + 0x6C00;
    y = (y << 8) + 0x7900;
    *ptr++ = (u_long)u | ((u_long)v << 16);
    *ptr++ = (u_long)x | ((u_long)y << 16) | ((u_long)0x9F0D17F << 32);
    *ptr++ = (u_long)(u + 0x80) | ((u_long)(v + 0x80) << 16);
    *ptr++ = (u_long)(x + 0x100) | ((u_long)(y + 0x100) << 16) | ((u_long)0x9F0D17F << 32);
    PacketPointerSet(3, ptr);
}

u_long *PacketDmaTagCnt(int id);
u_long *PacketGifTag(int id, u_long tag, u_long taglist);
u_long *PacketPackRegsWithTag(int id, u_long *reglist, int num);

#define T_RGBAQ(r, g, b, a, q) ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))

void TextOpen(int len)
{
    u_long font_reg[8] = {
        0, 0x156,
        0x42, 0x8000000044UL,
        6, 0x301460059D40A301UL,
        1, T_RGBAQ(text_color.r / 2, text_color.g / 2, text_color.b / 2, 0x80, 0),
    };

    PacketDmaTagCnt(3);
    PacketPackRegsWithTag(3, font_reg, 4);
    PacketGifTag(3, len | 0x4400000000000000UL, 0x5353);
}

void PacketBegin(void);
void PacketEnd(int wait);
u_long *PacketCopy(int id, u_long *data, int size);

void TextRead(void)
{
    int tbw;
    TEX_HEADER *h;
    u_short *p;
    u_long reglist[8];

    PacketBegin();
    p = font_data;
    PacketDmaTagCnt(3);
    h = (TEX_HEADER *)font_data;
    while (h->size) {
        tbw = h->w / 64;
        if (tbw == 0) {
            tbw = 1;
        }
        reglist[0] = 0x50;
        reglist[1] = ((u_long)(h->dbp + 0x500) << 32) | ((u_long)tbw << 48) | ((u_long)h->format << 56);
        reglist[2] = 0x51;
        reglist[3] = 0;
        reglist[4] = 0x52;
        reglist[5] = (u_long)h->w | ((u_long)h->h << 32);
        reglist[6] = 0x53;
        reglist[7] = 0;
        PacketPackRegsWithTag(3, reglist, 4);
        PacketGifTag(3, ((h->size - 16) / 16) | 0x8000 | ((u_long)2 << 58), 0);
        PacketCopy(3, &h->data, h->size - 16);
        p += h->size / 2;
        h = (TEX_HEADER *)p;
    }
    PacketEnd(1);
}
