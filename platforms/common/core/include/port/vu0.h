/* VU0 macro-mode helpers for game code built with GGX_PORT. */
#ifndef GGX_PORT_VU0_H
#define GGX_PORT_VU0_H

/* nllib's rotations: with I = sin and Q = cos, vmulaq/vmaddi and vmulaq/vmsubi
   give a <- a*cos + b*sin and b <- b*cos - a*sin on two whole matrix rows. */
static void ggx_port_vu0_rotate(float *a, float *b, float s, float c)
{
    int i;

    for (i = 0; i < 4; i++) {
        float x = a[i], y = b[i];

        a[i] = x * c + y * s;
        b[i] = y * c - x * s;
    }
}

#endif
