#include "generated/sin.h"

double sin(double x);
double sqrt(double x);
double atan2(double y, double x);

void SinMake(void)
{
    int i;

    for (i = 0; i < 0x10000; i++) {
        SinTable[i] = sin(i / 32768.0f * 3.141592653589793) * 16384.0;
    }
}

int Sin(unsigned short r)
{
    return SinTable[r];
}

int Cos(unsigned short r)
{
    return SinTable[(unsigned short)(r + 0x4000)];
}

float nlSin(unsigned short r)
{
    return SinTable[r] / 16384.0f;
}

float G_Sin(unsigned short r)
{
    return SinTable[r] / 16384.0f;
}

float nlCos(unsigned short r)
{
    return SinTable[(unsigned short)(r + 0x4000)] / 16384.0f;
}

float G_Cos(unsigned short r)
{
    return SinTable[(unsigned short)(r + 0x4000)] / 16384.0f;
}

int Sqrt(unsigned long src)
{
    return sqrt((float)src);
}

float nlSqrt(float src)
{
    return sqrt(src);
}

int nlArcTan2(int x, int y)
{
    return atan2((float)x, (float)y) * 32768.0 / 3.141592653589793;
}
