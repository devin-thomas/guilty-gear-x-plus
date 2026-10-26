/* sceGsSetDefDispEnv, Korean disc (SDK 2.5 libgraph, graph003.o): NEAR, 12 words off.
 *
 * Not linked. The KR function stays in the split; this is the closest C found so
 * far, for whoever picks it up. To try it, put it in the VERSION_KR block of
 * src/sdk/sce/graph_setdefdispenv.c, after _GetGsDxDyOffset.
 *
 * What 2.5 adds to the JP (2.3) version: the kernel's display offsets (syscall
 * 128, _GetGsDxDyOffset) on consoles whose ROM is newer than 2001-06-08 and not in
 * NTSC/PAL mode, added into DX/DY, and the 480p mode 0x50. The KR string reads
 * "... for 0x%x!!".
 *
 * Status (KR, 884 of 888 bytes compiled): the prologue, the offset setup, both
 * interlaced arms, the 0x50 arm and the epilogue match. The two non-interlaced
 * arms (omode 2 and 3, which cross-jump into one tail) differ:
 * - in the arm block the target issues `addiu a3,s2,-1; lw a1,0(sp); dsll32
 *   a3,a3,12; lw v1,4(sp)`, so h-1 and its shift go out before the loads; here the
 *   loads come first;
 * - the shared tail ORs (ddx | dh) and (magh | dw) and then combines them; with
 *   that OR tree in the source, the registers come out wrong, and the tree below
 *   gets the registers but ORs the terms one by one.
 * About 5,000 variants were compiled (statement orders, which values are named
 * locals, every OR tree over the five fields, and empty-asm operand forms like the
 * JP version's); none beat 12. The interlaced arms needed `ox` computed on its own
 * (fold otherwise moves the 0x27C out), and the 0x50 arm needed magh as a named
 * local (fold otherwise reassociates the constant 1 << 23 across the OR chain).
 */
void sceGsSetDefDispEnv(sceGsDispEnv *disp, short psm, short w, short h, short dx, short dy)
{
    sceGsGParam *gp = sceGsGetGParam();
    int off[4];

    if (gp->omode != 2 && gp->omode != 3 && checkModelVersion()) {
        _GetGsDxDyOffset(gp->omode, &off[0], &off[1], &off[2], &off[3]);
    } else {
        off[0] = off[1] = off[2] = off[3] = 0;
    }
    disp->pmode = 0x66;
    if (gp->inter != 0) {
        if (gp->ffmd != 0) {
            disp->smode2 = 3;
        } else {
            disp->smode2 = 1;
        }
    } else {
        disp->smode2 = 2;
    }
    disp->dispfb = ((s_long128)(psm & 0xF) << 15) | ((s_long128)(((w + 0x3F) >> 6) & 0x3F) << 9);

    if (gp->omode == 2) {
        if (gp->inter == 1) {
            s_long128 ddy = (s_long128)((dy + off[1] + 0x32) & 0xFFF) << 12;
            int q = (w + 0x9FF) / w;
            s_long128 dw = (s_long128)(q * w - 1) << 32;
            s_long128 ox = (s_long128)off[0] + 0x27C;
            s_long128 ddx = ((s_long128)(dx * q) + ox) & 0xFFF;
            s_long128 magh = (s_long128)(q - 1) << 23;
            s_long128 display;

            if (gp->ffmd) {
                display = magh | dw | (ddx | (s_long128)(h * 2 - 1) << 44) | ddy;
            } else {
                display = magh | dw | (ddx | (s_long128)(h - 1) << 44) | ddy;
            }
            disp->display = display;
        } else {
            int q = (w + 0x9FF) / w;
            s_long128 dh = (s_long128)(h - 1) << 44;
            s_long128 ox = (s_long128)off[0] + 0x27C;
            s_long128 ddy = (s_long128)((dy + off[1] + 0x19) & 0xFFF) << 12;

            disp->display = ((((s_long128)(q - 1) << 23 | (((s_long128)(dx * q) + ox) & 0xFFF)) |
                              (s_long128)(q * w - 1) << 32) | dh) | ddy;
        }
    } else if (gp->omode == 3) {
        if (gp->inter == 1) {
            s_long128 ddy = (s_long128)((dy + off[1] + 0x48) & 0xFFF) << 12;
            int q = (w + 0x9FF) / w;
            s_long128 dw = (s_long128)(q * w - 1) << 32;
            s_long128 ox = (s_long128)off[0] + 0x290;
            s_long128 ddx = ((s_long128)(dx * q) + ox) & 0xFFF;
            s_long128 magh = (s_long128)(q - 1) << 23;
            s_long128 display;

            if (gp->ffmd) {
                display = magh | dw | (ddx | (s_long128)(h * 2 - 1) << 44) | ddy;
            } else {
                display = magh | dw | (ddx | (s_long128)(h - 1) << 44) | ddy;
            }
            disp->display = display;
        } else {
            int q = (w + 0x9FF) / w;
            s_long128 dh = (s_long128)(h - 1) << 44;
            s_long128 ox = (s_long128)off[0] + 0x290;
            s_long128 ddy = (s_long128)((dy + off[1] + 0x24) & 0xFFF) << 12;

            disp->display = ((((s_long128)(q - 1) << 23 | (((s_long128)(dx * q) + ox) & 0xFFF)) |
                              (s_long128)(q * w - 1) << 32) | dh) | ddy;
        }
    } else if (gp->omode == 0x50) {
        s_long128 dh = (s_long128)(h - 1) << 44;
        s_long128 magh = (s_long128)1 << 23;
        s_long128 ex = (s_long128)(dx * 2) + 0xE8;

        disp->display = (dh | (s_long128)(w * 2 - 1) << 32) |
                        (((((s_long128)((0x2D0 - w) / 2 * 2) + off[0]) + ex) & 0xFFF) | magh) |
                        (s_long128)((dy + off[1] + 0x23) & 0xFFF) << 12;
    } else {
        printf("sceGsDefDispEnv:Not support displaymode for 0x%x!!\n", gp->omode);
    }
    disp->bgcolor = 0;
}
