#include "generated/nllib.h"
#include "generated/sin.h"

static float sin;
static float cos;

#ifdef GGX_PORT
#include "port/vu0.h"
#endif

void nlUnitMatrix(void) {
#ifndef GGX_PORT
    __asm__ __volatile__(
        ".set noreorder\n"
        "vmulw.xyzw  $vf1, $vf0, $vf0w\n"
        "vmulx.xyzw  $vf3, $vf0, $vf0x\n"
        "vmulx.xyzw  $vf4, $vf0, $vf0x\n"
        "vmr32.xyzw  $vf2, $vf1\n"
        "sqc2        $vf1, 0x30(%0)\n"
        "vaddw.y     $vf3, $vf3, $vf0w\n"
        "vaddw.x     $vf4, $vf4, $vf0w\n"
        "sqc2        $vf2, 0x20(%0)\n"
        "sqc2        $vf3, 0x10(%0)\n"
        "sqc2        $vf4, 0x0(%0)\n"
        ".set reorder\n"
        : : "r"(&mat));
#else
    {
        float *m = (float *)&mat;
        int i;

        for (i = 0; i < 16; i++) {
            m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        }
    }
#endif
}

void nlRotateAX(u_short r) {
    sin = nlSin(r);
    cos = nlCos(r);
#ifndef GGX_PORT
    __asm__ __volatile__(
        ".set noreorder\n"
        "ctc2        %0, $vi21\n"
        "ctc2        %1, $vi22\n"
        "lqc2        $vf5, 0x10(%2)\n"
        "lqc2        $vf6, 0x20(%2)\n"
        "vmulaq.xyzw ACC, $vf5, Q\n"
        "vmaddi.xyzw $vf2, $vf6, I\n"
        "vmulaq.xyzw ACC, $vf6, Q\n"
        "vmsubi.xyzw $vf3, $vf5, I\n"
        "sqc2        $vf2, 0x10(%2)\n"
        "sqc2        $vf3, 0x20(%2)\n"
        ".set reorder\n"
        : : "r"(*(int *)&sin), "r"(*(int *)&cos), "r"(&mat));
#else
    ggx_port_vu0_rotate((float *)&mat + 4, (float *)&mat + 8, sin, cos);
#endif
}

void nlRotateAY(u_short r) {
    sin = nlSin(r);
    cos = nlCos(r);
#ifndef GGX_PORT
    __asm__ __volatile__(
        ".set noreorder\n"
        "ctc2        %0, $vi21\n"
        "ctc2        %1, $vi22\n"
        "lqc2        $vf4, 0x0(%2)\n"
        "lqc2        $vf6, 0x20(%2)\n"
        "vmulaq.xyzw ACC, $vf4, Q\n"
        "vmsubi.xyzw $vf2, $vf6, I\n"
        "vmulaq.xyzw ACC, $vf6, Q\n"
        "vmaddi.xyzw $vf3, $vf4, I\n"
        "sqc2        $vf2, 0x0(%2)\n"
        "sqc2        $vf3, 0x20(%2)\n"
        ".set reorder\n"
        : : "r"(*(int *)&sin), "r"(*(int *)&cos), "r"(&mat));
#else
    ggx_port_vu0_rotate((float *)&mat + 8, (float *)&mat + 0, sin, cos);
#endif
}

void nlRotateAZ(u_short r) {
    sin = nlSin(r);
    cos = nlCos(r);
#ifndef GGX_PORT
    __asm__ __volatile__(
        ".set noreorder\n"
        "ctc2        %0, $vi21\n"
        "ctc2        %1, $vi22\n"
        "lqc2        $vf4, 0x0(%2)\n"
        "lqc2        $vf5, 0x10(%2)\n"
        "vmulaq.xyzw ACC, $vf4, Q\n"
        "vmaddi.xyzw $vf2, $vf5, I\n"
        "vmulaq.xyzw ACC, $vf5, Q\n"
        "vmsubi.xyzw $vf3, $vf4, I\n"
        "sqc2        $vf2, 0x0(%2)\n"
        "sqc2        $vf3, 0x10(%2)\n"
        ".set reorder\n"
        : : "r"(*(int *)&sin), "r"(*(int *)&cos), "r"(&mat));
#else
    ggx_port_vu0_rotate((float *)&mat + 0, (float *)&mat + 4, sin, cos);
#endif
}

void nlCalcPoint(NL_POINT3 *p1, NL_POINT3 *p2) {
#ifndef GGX_PORT
    __asm__ __volatile__(
        ".set noreorder\n"
        "lqc2         $vf1, 0x0(%0)\n"
        "lqc2         $vf3, 0x0(%1)\n"
        "lqc2         $vf2, 0x0(%2)\n"
        "vmulax.xyzw  ACC, $vf2, $vf1x\n"
        "lqc2         $vf2, 0x10(%2)\n"
        "vmadday.xyzw ACC, $vf2, $vf1y\n"
        "lqc2         $vf2, 0x20(%2)\n"
        "vmaddaz.xyzw ACC, $vf2, $vf1z\n"
        "lqc2         $vf2, 0x30(%2)\n"
        "vmaddw.xyzw  $vf1, $vf2, $vf0w\n"
        "vmove.xyz    $vf3, $vf1\n"
        "sqc2         $vf3, 0x0(%1)\n"
        ".set reorder\n"
        : : "r"(p1), "r"(p2), "r"(&mat));
#else
    {
        float *m = (float *)&mat;
        float x = p1->x, y = p1->y, z = p1->z;

        p2->x = m[0] * x + m[4] * y + m[8] * z + m[12];
        p2->y = m[1] * x + m[5] * y + m[9] * z + m[13];
        p2->z = m[2] * x + m[6] * y + m[10] * z + m[14];
    }
#endif
}

void chk_select_reset(int side) {
}

void trapa(int t) {
}

extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern SYS_COND sc;
extern CHARANGE_CTRL *gl_info;
extern story_t story;
int get_ggmode(int side);
int Random(void);

void Read_Playdat(int size)
{
    int no;

    for (no = 0; no < 2; no++) {
        if ((g_info.mode & 0x4000) && sc.replay == 2) {
            if (get_ggmode(no)) {
                userinfo.gguse[no] = get_ggmode(no);
            } else {
                userinfo.gguse[no] = 0;
            }
        } else if (g_info.mode & 0x8000) {
            if (gl_info->ggmode[no] > 0) {
                userinfo.gguse[no] = gl_info->ggmode[no];
            } else {
                userinfo.gguse[no] = 0;
            }
        } else if (g_info.mode & 0x2000) {
            userinfo.gguse[no] = story.gguse[no];
        } else if (userinfo.ggmode[no] > 0) {
            userinfo.gguse[no] |= 1;
        } else if (userinfo.ggmode[no] == -1) {
            userinfo.gguse[no] |= Random() & 1;
        } else {
            userinfo.gguse[no] &= ~1;
        }
    }
}
