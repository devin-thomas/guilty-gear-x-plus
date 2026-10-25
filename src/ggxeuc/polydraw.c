#include "generated/polydraw.h"
u_int RGBConv2(u_int rgb);
void nlcDispStrip(int num, NLVERTEX00 *vt);
void nlcDispStripTrnsl(int num, NLVERTEX00 *vt);
void nlDispStrip(int num, NLVERTEX00 *vt);
void nlDispStripTrnsl(int num, NLVERTEX00 *vt);
void polydraw_FLine(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void polydraw_EX(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4);

void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1)
{
    NLVERTEX00 vt[4];

    color1 = RGBConv2(color1);
    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x2;
    vt[1].y = y1;
    vt[1].z = z;
    vt[1].col = color1;
    vt[2].x = x1;
    vt[2].y = y2;
    vt[2].z = z;
    vt[2].col = color1;
    vt[3].x = x2;
    vt[3].y = y2;
    vt[3].z = z;
    vt[3].col = color1;
    if ((color1 >> 24) == 0xFF) {
        nlcDispStrip(4, vt);
    } else {
        nlcDispStripTrnsl(4, vt);
    }
}

void polydraw_FBoxOPQ(float x1, float y1, float x2, float y2, float z, unsigned int color1)
{
    NLVERTEX00 vt[4];

    color1 = RGBConv2(color1);
    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x2;
    vt[1].y = y1;
    vt[1].z = z;
    vt[1].col = color1;
    vt[2].x = x1;
    vt[2].y = y2;
    vt[2].z = z;
    vt[2].col = color1;
    vt[3].x = x2;
    vt[3].y = y2;
    vt[3].z = z;
    vt[3].col = color1;
    nlcDispStrip(4, vt);
}

void polydraw_PBox(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, unsigned int color1)
{
    NLVERTEX00 vt[4];

    color1 = RGBConv2(color1);
    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x2;
    vt[1].y = y2;
    vt[1].z = z;
    vt[1].col = color1;
    vt[2].x = x3;
    vt[2].y = y3;
    vt[2].z = z;
    vt[2].col = color1;
    vt[3].x = x4;
    vt[3].y = y4;
    vt[3].z = z;
    vt[3].col = color1;
    if ((color1 >> 24) == 0xFF) {
        nlcDispStrip(4, vt);
    } else {
        nlcDispStripTrnsl(4, vt);
    }
}

void polydraw_GBox(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4)
{
    NLVERTEX00 vt[4];

    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x1;
    vt[1].y = y2;
    vt[1].z = z;
    vt[1].col = color3;
    vt[2].x = x2;
    vt[2].y = y1;
    vt[2].z = z;
    vt[2].col = color2;
    vt[3].x = x2;
    vt[3].y = y2;
    vt[3].z = z;
    vt[3].col = color4;
    nlDispStripTrnsl(4, vt);
}

void polydraw_GBoxOPQ(float x1, float y1, float x2, float y2, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4)
{
    NLVERTEX00 vt[4];

    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x1;
    vt[1].y = y2;
    vt[1].z = z;
    vt[1].col = color3;
    vt[2].x = x2;
    vt[2].y = y1;
    vt[2].z = z;
    vt[2].col = color2;
    vt[3].x = x2;
    vt[3].y = y2;
    vt[3].z = z;
    vt[3].col = color4;
    nlDispStrip(4, vt);
}

void polydraw_EX(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float z, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4)
{
    NLVERTEX00 vt[4];

    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x2;
    vt[1].y = y2;
    vt[1].z = z;
    vt[1].col = color3;
    vt[2].x = x3;
    vt[2].y = y3;
    vt[2].z = z;
    vt[2].col = color2;
    vt[3].x = x4;
    vt[3].y = y4;
    vt[3].z = z;
    vt[3].col = color4;
    nlDispStripTrnsl(4, vt);
}

void polydraw_FLine(float x1, float y1, float x2, float y2, float z, unsigned int color1)
{
    NLVERTEX00 vt[4];

    color1 = RGBConv2(color1);
    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x2;
    vt[1].y = y2;
    vt[1].z = z;
    vt[1].col = color1;
    vt[2].x = x1 + 1.0f;
    vt[2].y = y1 + 1.0f;
    vt[2].z = z;
    vt[2].col = color1;
    vt[3].x = x2 + 1.0f;
    vt[3].y = y2 + 1.0f;
    vt[3].z = z;
    vt[3].col = color1;
    if ((color1 >> 24) == 0xFF) {
        nlcDispStrip(4, vt);
    } else {
        nlcDispStripTrnsl(4, vt);
    }
}

void polydraw_LBox(float x1, float y1, float x2, float y2, float z, unsigned int color1)
{
    polydraw_FLine(x1, y1, x2 + 1.0f, y1, z, color1);
    polydraw_FLine(x2, y1, x2, y2, z, color1);
    polydraw_FLine(x1, y2, x2, y2, z, color1);
    polydraw_FLine(x1, y1, x1, y2, z, color1);
}

void polydraw_Frm(float x1, float y1, float x2, float y2, float w, float z, unsigned int c1, unsigned int c2)
{
    polydraw_EX(x1 - w, y1 - w, x2 + w, y1 - w, x1, y1, x2, y1, z, c1, c1, c2, c2);
    polydraw_EX(x2, y1, x2 + w, y1 - w, x2, y2, x2 + w, y2 + w, z, c2, c1, c2, c1);
    polydraw_EX(x1, y2, x2, y2, x1 - w, y2 + w, x2 + w, y2 + w, z, c2, c2, c1, c1);
    polydraw_EX(x1 - w, y1 - w, x1, y1, x1 - w, y2 + w, x1, y2, z, c1, c2, c1, c2);
}

void polydraw_WLine(float x1, float y1, float x2, float y2, float z, unsigned int color1)
{
    NLVERTEX00 vt[4];

    color1 = RGBConv2(color1);
    vt[0].x = x1;
    vt[0].y = y1;
    vt[0].z = z;
    vt[0].col = color1;
    vt[1].x = x2;
    vt[1].y = y2;
    vt[1].z = z;
    vt[1].col = color1;
    vt[2].x = x1 + 2.0f;
    vt[2].y = y1 + 2.0f;
    vt[2].z = z;
    vt[2].col = color1;
    vt[3].x = x2 + 2.0f;
    vt[3].y = y2 + 2.0f;
    vt[3].z = z;
    vt[3].col = color1;
    if ((color1 >> 24) == 0xFF) {
        nlcDispStrip(4, vt);
    } else {
        nlcDispStripTrnsl(4, vt);
    }
}
