/* One frame of GS output, ready for a GPU (PORT-008, PORT-009).
 *
 * dl_stream.c builds it from the game's GIF packets; dl_gpu.c draws it.
 * Primitives are already expanded to triangles in framebuffer pixels, with
 * the GS's own pixel-centre convention folded in (dl_stream.c), and grouped
 * into batches that share a framebuffer, a blend mode, a texture and a CLUT.
 *
 * A frame is what the GS does between two of the game's GraphConv blits:
 * the frame clear, the Draw Layer's primitives and the debug text drawn into
 * the draw buffer, then the blit that copies it to the display buffer, and
 * the display circuits' registers (PMODE, DISPFB, DISPLAY, BGCOLOR) that
 * show the result.
 */
#ifndef GGX_DL_FRAME_H
#define GGX_DL_FRAME_H

#include <stdint.h>

/* A framebuffer's size at 1x: FRAME_1 is 640 wide and the scissor 448 tall
   (graph.c MakeFullBuffer), and the display shows 640x448 (DISPLAY). Every
   surface is this size, and so is the display stage's output. */
#define DL_FB_W 640
#define DL_FB_H 448

typedef struct {
    float x, y;            /* framebuffer pixels at 1x; pixel centres at +0.5 */
    float u, v;            /* texels (FST=1 UV / 16) */
    uint8_t r, g, b, a;    /* RGBAQ as the GS gets it (0x80 = 1.0) */
} DlVertex;

/* How a batch's fragments are blended into the framebuffer: the ALPHA
   register's A, B, C, D selectors and FIX, or none when ABE is off. */
typedef struct {
    uint8_t abe;
    uint8_t a, b, c, d;    /* 0 Cs/As, 1 Cd/Ad, 2 zero/FIX */
    uint8_t fix;
} DlBlend;

enum { DL_TEX_NONE = 0, DL_TEX_RGBA = 1, DL_TEX_INDEX = 2 };

typedef struct {
    uint32_t first, count; /* vertices, three per triangle */
    DlBlend blend;
    int32_t tex;           /* texture cache id, or -1 */
    int32_t clut;          /* CLUT row in the frame's atlas, or -1 */
    uint8_t tex_kind;      /* DL_TEX_* */
    uint8_t tcc, tfx;      /* TEX0 TCC and TFX */
    uint8_t tw, th;        /* log2 of the texture's wrap size (REPEAT) */
    uint16_t tex_w, tex_h; /* the stored texels (<= 1 << tw, 1 << th) */
    uint16_t scissor[4];   /* x0, y0, x1, y1, inclusive, 1x pixels */
    uint8_t gs_blend;      /* one primitive whose As can exceed 1.0: blended
                              by the shader, as the GS does, not the GPU */
    uint8_t surface;       /* the framebuffer drawn into (FRAME) */
    int16_t blit;          /* >= 0: not a draw but the frame's blit[blit],
                              done here in the order the GS got it */
} DlBatch;

/* Framebuffers the GPU holds for GS local memory: the ones FRAME draws
   into, DISPFB shows and blits copy between. A surface starts as what local
   memory held there when it was made (zeros after a boot, PCSX2's frames
   after a state or in a GS dump), and from then on its pixels exist only on
   the GPU (no game texture reads them back: PORT-008). */
#define DL_MAX_SURFACES 4

typedef struct {
    uint32_t bp;           /* base block (FBP * 32) */
    uint8_t bw;            /* width in 64-pixel units */
    uint8_t psm;
    const uint8_t *init;   /* its first contents, RGBA8 at 1x, alpha as the GPU
                              keeps it (0x80 = 1.0 = 255) */
} DlSurface;

/* A local-to-local transfer between two surfaces (GraphConv's), 1x pixels. */
#define DL_MAX_BLITS 8

typedef struct {
    uint8_t src, dst;
    uint16_t sx, sy, dx, dy, w, h;
} DlBlit;

/* One read circuit of the display, resolved from PMODE, DISPFB and DISPLAY.
   The output is 640x448 pixels at 1x, with its origin where DISPLAY's DX,
   DY are the game's default (636, 50): the display's own placement, so the
   user's screen offset (GraphSetOffset) moves the picture as it does on a
   television. */
typedef struct {
    uint8_t on;            /* PMODE EN1 / EN2, and a surface to show */
    uint8_t surface;
    uint16_t dbx, dby;     /* DISPFB: the buffer pixel its picture starts at */
    int16_t x, y;          /* where the picture starts on the output */
    uint16_t w, h;         /* its size there */
    uint8_t magh, magv;    /* DISPLAY: MAGH + 1 clocks, MAGV + 1 lines a pixel */
    uint8_t half;          /* interlaced frame mode (SMODE2 INT and FFMD): every
                              buffer line shows on two output lines */
} DlCircuit;

typedef struct {
    DlCircuit c[2];
    uint8_t mmod;          /* PMODE MMOD: 1 blends circuit 1 at ALP, 0 at its alpha */
    uint8_t slbg;          /* PMODE SLBG: 1 blends over BGCOLOR, 0 over circuit 2 */
    uint8_t alp;
    uint32_t bgcolor;      /* BGCOLOR, RGB8 */
} DlDisplay;

#define DL_CLUT_ROWS 512

typedef struct {
    DlVertex *v;
    uint32_t nv, cap_v;
    DlBatch *b;
    uint32_t nb, cap_b;
    uint32_t clut[DL_CLUT_ROWS][256];   /* RGBA8 rows */
    uint64_t clut_key[DL_CLUT_ROWS];
    uint32_t nclut;
    DlSurface surf[DL_MAX_SURFACES];    /* every surface so far (stable indices) */
    uint32_t nsurf;
    DlBlit blit[DL_MAX_BLITS];
    uint32_t nblit;
    DlDisplay display;
    uint32_t serial;                    /* frames completed so far */
} DlFrame;

#endif
