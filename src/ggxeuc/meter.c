#include "generated/meter.h"

void MeterInit(void)
{
    *(volatile int *)0x10000010 = 0x83;
}

void MeterReset(void)
{
    meter_color[0] = 0xFF0000;
    *(volatile int *)0x10000000 = 0;
    meter_index = 0;
}

int MeterColorChange(int col)
{
    int old;

    if (meter_index >= 256) {
        return col;
    }
    old = meter_color[meter_index];
    meter_time[meter_index] = *(volatile int *)0x10000000;
    meter_index++;
    meter_color[meter_index] = col;
    return old;
}

u_long *PacketDmaTagCnt(int id);
u_long *PacketGifTag(int id, u_long tag, u_long taglist);
void PacketPointerSet(int id, u_long *ptr);

#define M_XYZ(x, y, z) ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32))
#define M_RGBAQ(r, g, b, a, q) ((u_long)(r) | ((u_long)(g) << 8) | ((u_long)(b) << 16) | ((u_long)(a) << 24) | ((u_long)(q) << 32))

void MeterDraw(void)
{
    int i;
    int time;
    int col;
    int x;
    int y;
    u_long *ptr;

    meter_time[meter_index] = *(volatile int *)0x10000000;
    PacketDmaTagCnt(3);
    ptr = PacketGifTag(3, 0x4400000000000009UL, 0x4410);
    for (i = 0; i < 9; i++) {
        x = i * 0x4B0 + 0x6D40; y = 0x7DC0;
        *ptr++ = 1;
        *ptr++ = M_RGBAQ(0xFF, 0xFF, 0xFF, 0x80, 0);
        *ptr++ = M_XYZ(x, y, 0x9F0D17F);
        *ptr++ = M_XYZ(x, y + 0x40, 0x9F0D17F);
    }
    PacketPointerSet(3, ptr);
    ptr = PacketGifTag(3, (meter_index + 1) | 0x4400000000000000UL, 0x4410);
    time = 0;
    for (i = 0; i <= meter_index; i++) {
        col = meter_color[i];
        if (col >= 0) {
            *ptr++ = 6;
            *ptr++ = M_RGBAQ((col >> 16) & 0xFF, (col >> 8) & 0xFF, col & 0xFF, 0x80, 0);
            x = (time % 262) * 0x2580 / 262 + 0x6D40;
            y = (time / 262) * 64 + 0x7E00;
            if (time / 262 != meter_time[i] / 262) {
                x = 0x6D40;
                y = (time / 262) * 64 + 0x7E7C;
            }
            *ptr++ = M_XYZ(x, y, 0x9F0D17F);
            time = meter_time[i];
            x = (time % 262) * 0x2580 / 262 + 0x6D40;
            y = (time / 262) * 64 + 0x7E3C;
            *ptr++ = M_XYZ(x, y, 0x9F0D17F);
        }
    }
    PacketPointerSet(3, ptr);
}
