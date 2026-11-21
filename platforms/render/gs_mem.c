/* The GS's local memory with its swizzled layouts (gs_mem.h). */
#include "gs_mem.h"

#include <stdlib.h>
#include <string.h>

/* The GS's block and column layouts: which block of a page, and which unit
   of a block, a pixel lands in. These are the hardware's tables (the same
   ones PCSX2's GSTables.cpp holds). */
static const uint8_t BLOCK32[4][8] = {
    {0, 1, 4, 5, 16, 17, 20, 21},
    {2, 3, 6, 7, 18, 19, 22, 23},
    {8, 9, 12, 13, 24, 25, 28, 29},
    {10, 11, 14, 15, 26, 27, 30, 31},
};
static const uint8_t BLOCK16[8][4] = {
    {0, 2, 8, 10},
    {1, 3, 9, 11},
    {4, 6, 12, 14},
    {5, 7, 13, 15},
    {16, 18, 24, 26},
    {17, 19, 25, 27},
    {20, 22, 28, 30},
    {21, 23, 29, 31},
};
static const uint8_t COL32[8][8] = {
    {0, 1, 4, 5, 8, 9, 12, 13},
    {2, 3, 6, 7, 10, 11, 14, 15},
    {16, 17, 20, 21, 24, 25, 28, 29},
    {18, 19, 22, 23, 26, 27, 30, 31},
    {32, 33, 36, 37, 40, 41, 44, 45},
    {34, 35, 38, 39, 42, 43, 46, 47},
    {48, 49, 52, 53, 56, 57, 60, 61},
    {50, 51, 54, 55, 58, 59, 62, 63},
};
static const uint8_t COL16[8][16] = {
    {0, 2, 8, 10, 16, 18, 24, 26, 1, 3, 9, 11, 17, 19, 25, 27},
    {4, 6, 12, 14, 20, 22, 28, 30, 5, 7, 13, 15, 21, 23, 29, 31},
    {32, 34, 40, 42, 48, 50, 56, 58, 33, 35, 41, 43, 49, 51, 57, 59},
    {36, 38, 44, 46, 52, 54, 60, 62, 37, 39, 45, 47, 53, 55, 61, 63},
    {64, 66, 72, 74, 80, 82, 88, 90, 65, 67, 73, 75, 81, 83, 89, 91},
    {68, 70, 76, 78, 84, 86, 92, 94, 69, 71, 77, 79, 85, 87, 93, 95},
    {96, 98, 104, 106, 112, 114, 120, 122, 97, 99, 105, 107, 113, 115, 121, 123},
    {100, 102, 108, 110, 116, 118, 124, 126, 101, 103, 109, 111, 117, 119, 125, 127},
};
static const uint8_t COL8[16][16] = {
    {0, 4, 16, 20, 32, 36, 48, 52, 2, 6, 18, 22, 34, 38, 50, 54},
    {8, 12, 24, 28, 40, 44, 56, 60, 10, 14, 26, 30, 42, 46, 58, 62},
    {33, 37, 49, 53, 1, 5, 17, 21, 35, 39, 51, 55, 3, 7, 19, 23},
    {41, 45, 57, 61, 9, 13, 25, 29, 43, 47, 59, 63, 11, 15, 27, 31},
    {96, 100, 112, 116, 64, 68, 80, 84, 98, 102, 114, 118, 66, 70, 82, 86},
    {104, 108, 120, 124, 72, 76, 88, 92, 106, 110, 122, 126, 74, 78, 90, 94},
    {65, 69, 81, 85, 97, 101, 113, 117, 67, 71, 83, 87, 99, 103, 115, 119},
    {73, 77, 89, 93, 105, 109, 121, 125, 75, 79, 91, 95, 107, 111, 123, 127},
    {128, 132, 144, 148, 160, 164, 176, 180, 130, 134, 146, 150, 162, 166, 178, 182},
    {136, 140, 152, 156, 168, 172, 184, 188, 138, 142, 154, 158, 170, 174, 186, 190},
    {161, 165, 177, 181, 129, 133, 145, 149, 163, 167, 179, 183, 131, 135, 147, 151},
    {169, 173, 185, 189, 137, 141, 153, 157, 171, 175, 187, 191, 139, 143, 155, 159},
    {224, 228, 240, 244, 192, 196, 208, 212, 226, 230, 242, 246, 194, 198, 210, 214},
    {232, 236, 248, 252, 200, 204, 216, 220, 234, 238, 250, 254, 202, 206, 218, 222},
    {193, 197, 209, 213, 225, 229, 241, 245, 195, 199, 211, 215, 227, 231, 243, 247},
    {201, 205, 217, 221, 233, 237, 249, 253, 203, 207, 219, 223, 235, 239, 251, 255},
};
static const uint16_t COL4[16][32] = {
    {0, 8, 32, 40, 64, 72, 96, 104, 2, 10, 34, 42, 66, 74, 98, 106, 4, 12, 36, 44, 68, 76, 100, 108, 6, 14, 38, 46, 70, 78, 102, 110},
    {16, 24, 48, 56, 80, 88, 112, 120, 18, 26, 50, 58, 82, 90, 114, 122, 20, 28, 52, 60, 84, 92, 116, 124, 22, 30, 54, 62, 86, 94, 118, 126},
    {65, 73, 97, 105, 1, 9, 33, 41, 67, 75, 99, 107, 3, 11, 35, 43, 69, 77, 101, 109, 5, 13, 37, 45, 71, 79, 103, 111, 7, 15, 39, 47},
    {81, 89, 113, 121, 17, 25, 49, 57, 83, 91, 115, 123, 19, 27, 51, 59, 85, 93, 117, 125, 21, 29, 53, 61, 87, 95, 119, 127, 23, 31, 55, 63},
    {192, 200, 224, 232, 128, 136, 160, 168, 194, 202, 226, 234, 130, 138, 162, 170, 196, 204, 228, 236, 132, 140, 164, 172, 198, 206, 230, 238, 134, 142, 166, 174},
    {208, 216, 240, 248, 144, 152, 176, 184, 210, 218, 242, 250, 146, 154, 178, 186, 212, 220, 244, 252, 148, 156, 180, 188, 214, 222, 246, 254, 150, 158, 182, 190},
    {129, 137, 161, 169, 193, 201, 225, 233, 131, 139, 163, 171, 195, 203, 227, 235, 133, 141, 165, 173, 197, 205, 229, 237, 135, 143, 167, 175, 199, 207, 231, 239},
    {145, 153, 177, 185, 209, 217, 241, 249, 147, 155, 179, 187, 211, 219, 243, 251, 149, 157, 181, 189, 213, 221, 245, 253, 151, 159, 183, 191, 215, 223, 247, 255},
    {256, 264, 288, 296, 320, 328, 352, 360, 258, 266, 290, 298, 322, 330, 354, 362, 260, 268, 292, 300, 324, 332, 356, 364, 262, 270, 294, 302, 326, 334, 358, 366},
    {272, 280, 304, 312, 336, 344, 368, 376, 274, 282, 306, 314, 338, 346, 370, 378, 276, 284, 308, 316, 340, 348, 372, 380, 278, 286, 310, 318, 342, 350, 374, 382},
    {321, 329, 353, 361, 257, 265, 289, 297, 323, 331, 355, 363, 259, 267, 291, 299, 325, 333, 357, 365, 261, 269, 293, 301, 327, 335, 359, 367, 263, 271, 295, 303},
    {337, 345, 369, 377, 273, 281, 305, 313, 339, 347, 371, 379, 275, 283, 307, 315, 341, 349, 373, 381, 277, 285, 309, 317, 343, 351, 375, 383, 279, 287, 311, 319},
    {448, 456, 480, 488, 384, 392, 416, 424, 450, 458, 482, 490, 386, 394, 418, 426, 452, 460, 484, 492, 388, 396, 420, 428, 454, 462, 486, 494, 390, 398, 422, 430},
    {464, 472, 496, 504, 400, 408, 432, 440, 466, 474, 498, 506, 402, 410, 434, 442, 468, 476, 500, 508, 404, 412, 436, 444, 470, 478, 502, 510, 406, 414, 438, 446},
    {385, 393, 417, 425, 449, 457, 481, 489, 387, 395, 419, 427, 451, 459, 483, 491, 389, 397, 421, 429, 453, 461, 485, 493, 391, 399, 423, 431, 455, 463, 487, 495},
    {401, 409, 433, 441, 465, 473, 497, 505, 403, 411, 435, 443, 467, 475, 499, 507, 405, 413, 437, 445, 469, 477, 501, 509, 407, 415, 439, 447, 471, 479, 503, 511},
};

/* Per format: the page size in pixels, the units (pixels) per block and in
   the whole memory, and the unit offset of each pixel within a page. */
typedef struct {
    uint32_t pw, ph, upb, units;
    uint16_t *off;   /* ph * pw */
} Layout;

static Layout L32, L16, L8, L4;
static int layouts_ready;

static void layout_init(Layout *l, uint32_t pw, uint32_t ph, uint32_t upb, const uint8_t *block, uint32_t bcols,
                        uint32_t cw, uint32_t ch, const uint8_t *col8, const uint16_t *col16)
{
    uint32_t x, y;

    l->pw = pw;
    l->ph = ph;
    l->upb = upb;
    l->units = GS_MEM_SIZE / 256 * upb;
    l->off = malloc(sizeof(uint16_t) * pw * ph);
    if (!l->off) {
        abort();
    }
    for (y = 0; y < ph; y++) {
        for (x = 0; x < pw; x++) {
            uint32_t b = block[(y / ch) * bcols + x / cw];
            uint32_t c = col16 ? col16[(y % ch) * cw + x % cw] : col8[(y % ch) * cw + x % cw];

            l->off[y * pw + x] = (uint16_t)(b * upb + c);
        }
    }
}

static void layouts_init(void)
{
    if (layouts_ready) {
        return;
    }
    layout_init(&L32, 64, 32, 64, &BLOCK32[0][0], 8, 8, 8, &COL32[0][0], NULL);
    layout_init(&L16, 64, 64, 128, &BLOCK16[0][0], 4, 16, 8, &COL16[0][0], NULL);
    layout_init(&L8, 128, 64, 256, &BLOCK32[0][0], 8, 16, 16, &COL8[0][0], NULL);
    layout_init(&L4, 128, 128, 512, &BLOCK16[0][0], 4, 32, 16, NULL, &COL4[0][0]);
    layouts_ready = 1;
}

static const Layout *layout_of(int psm)
{
    switch (psm) {
    case GS_PSMCT32:
    case GS_PSMCT24:
        return &L32;
    case GS_PSMCT16:
        return &L16;
    case GS_PSMT8:
        return &L8;
    case GS_PSMT4:
        return &L4;
    }
    return NULL;
}

/* The unit address (word, half, byte or nibble) of pixel x, y. */
static uint32_t unit_addr(const Layout *l, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y)
{
    uint32_t per_row = l->pw == 64 ? bw : bw >> 1;
    uint32_t page = (y / l->ph) * per_row + x / l->pw;

    return (bp * l->upb + page * 32 * l->upb + l->off[(y % l->ph) * l->pw + x % l->pw]) & (l->units - 1);
}

int gs_mem_init(GsMem *g)
{
    layouts_init();
    g->m = calloc(1, GS_MEM_SIZE);
    return g->m ? 0 : -1;
}

void gs_mem_free(GsMem *g)
{
    free(g->m);
    g->m = NULL;
}

void gs_mem_load(GsMem *g, const uint8_t *image)
{
    memcpy(g->m, image, GS_MEM_SIZE);
}

int gs_psm_supported(int psm)
{
    return layout_of(psm) != NULL;
}

int gs_psm_bits(int psm)
{
    switch (psm) {
    case GS_PSMCT32:
        return 32;
    case GS_PSMCT24:
        return 24;
    case GS_PSMCT16:
        return 16;
    case GS_PSMT8:
        return 8;
    case GS_PSMT4:
        return 4;
    }
    return 0;
}

uint32_t gs_block_of(int psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y)
{
    const Layout *l = layout_of(psm);

    layouts_init();
    return l ? unit_addr(l, bp, bw, x & 2047, y & 2047) / l->upb : 0;
}

/* The unit addresses of w pixels of row y from x0: one row at a time, so the
   page and the layout row are worked out once. */
static void row_addrs(const Layout *l, uint32_t bp, uint32_t bw, uint32_t x0, uint32_t y, uint32_t w, uint32_t *out)
{
    uint32_t per_row = l->pw == 64 ? bw : bw >> 1, shift = l->pw == 64 ? 6 : 7, page = 32 * l->upb;
    uint32_t base = bp * l->upb + (y / l->ph) * per_row * page, mask = l->units - 1, k, x;
    const uint16_t *off = l->off + (y % l->ph) * l->pw;

    for (k = 0, x = x0; k < w; k++, x = (x + 1) & 2047) {
        out[k] = (base + (x >> shift) * page + off[x & (l->pw - 1)]) & mask;
    }
}

void gs_write_rect(GsMem *g, int psm, uint32_t bp, uint32_t bw, uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
                   const uint8_t *data)
{
    gs_write_sub(g, psm, bp, bw, x0, y0, w, h, data, w, 0, 0);
}

void gs_write_sub(GsMem *g, int psm, uint32_t bp, uint32_t bw, uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
                  const uint8_t *data, uint32_t stride, uint32_t sx, uint32_t sy)
{
    const Layout *l = layout_of(psm);
    uint32_t a[2048], y, k, i;

    if (!l || w > 2048) {
        return;
    }
    for (y = y0; y < y0 + h; y++) {
        i = (sy + y - y0) * stride + sx;
        row_addrs(l, bp, bw, x0 & 2047, y & 2047, w, a);
        switch (psm) {
        case GS_PSMCT32:
            for (k = 0; k < w; k++, i++) {
                memcpy(g->m + 4 * a[k], data + 4 * i, 4);
            }
            break;
        case GS_PSMCT24:
            for (k = 0; k < w; k++, i++) {
                memcpy(g->m + 4 * a[k], data + 3 * i, 3);
            }
            break;
        case GS_PSMCT16:
            for (k = 0; k < w; k++, i++) {
                memcpy(g->m + 2 * a[k], data + 2 * i, 2);
            }
            break;
        case GS_PSMT8:
            for (k = 0; k < w; k++, i++) {
                g->m[a[k]] = data[i];
            }
            break;
        case GS_PSMT4:
            for (k = 0; k < w; k++, i++) {
                uint8_t v = (uint8_t)((data[i >> 1] >> ((i & 1) * 4)) & 0xF);
                uint8_t *m = g->m + (a[k] >> 1);

                *m = (a[k] & 1) ? (uint8_t)((*m & 0x0F) | (v << 4)) : (uint8_t)((*m & 0xF0) | v);
            }
            break;
        }
    }
}

void gs_read_rect(const GsMem *g, int psm, uint32_t bp, uint32_t bw, uint32_t x0, uint32_t y0, uint32_t w,
                  uint32_t h, void *out)
{
    const Layout *l = layout_of(psm);
    uint32_t a[2048], y, k, i = 0;

    if (!l || w > 2048) {
        return;
    }
    for (y = y0; y < y0 + h; y++) {
        row_addrs(l, bp, bw, x0 & 2047, y & 2047, w, a);
        switch (psm) {
        case GS_PSMCT32:
        case GS_PSMCT24:
            for (k = 0; k < w; k++, i++) {
                memcpy((uint32_t *)out + i, g->m + 4 * a[k], 4);
            }
            break;
        case GS_PSMCT16:
            for (k = 0; k < w; k++, i++) {
                memcpy((uint16_t *)out + i, g->m + 2 * a[k], 2);
            }
            break;
        case GS_PSMT8:
            for (k = 0; k < w; k++, i++) {
                ((uint8_t *)out)[i] = g->m[a[k]];
            }
            break;
        case GS_PSMT4:
            for (k = 0; k < w; k++, i++) {
                ((uint8_t *)out)[i] = (uint8_t)((g->m[a[k] >> 1] >> ((a[k] & 1) * 4)) & 0xF);
            }
            break;
        }
    }
}

uint32_t gs_ct16_rgba(uint16_t c, uint32_t ta0, uint32_t ta1)
{
    uint32_t r = (c & 0x1Fu) << 3, gg = ((c >> 5) & 0x1Fu) << 3, b = ((c >> 10) & 0x1Fu) << 3;

    return r | gg << 8 | b << 16 | (c & 0x8000 ? ta1 : ta0) << 24;
}

/* The CSM1 storage position of palette index i: 8-bit palettes swap index
   bits 3 and 4. */
static uint32_t csm1_pos(uint32_t i, uint32_t n)
{
    return n == 256 ? (i & ~0x18u) | ((i & 0x08) << 1) | ((i & 0x10) >> 1) : i;
}

/* The CLUT buffer offset (in entries) PCSX2 gives a CSA: a CT32 entry takes
   both halves, so only CSA 0-15 address CT32 entries. */
static uint32_t clut_offset(int cpsm, uint32_t csa)
{
    return cpsm == GS_PSMCT16 ? csa << 4 : (csa & 15) << 4;
}

void gs_clut_load(GsClut *c, const GsMem *g, int psm, int cpsm, uint32_t cbp, uint32_t csa)
{
    uint32_t n = psm == GS_PSMT8 ? 256 : 16, w = n == 256 ? 16 : 8, hh = n == 256 ? 16 : 2;
    uint32_t off = clut_offset(cpsm, csa), i;

    if (cpsm == GS_PSMCT16) {
        uint16_t raw[256];

        gs_read_rect(g, GS_PSMCT16, cbp, 1, 0, 0, w, hh, raw);
        for (i = 0; i < n; i++) {
            c->h[(off + i) & 511] = raw[csm1_pos(i, n)];
        }
    } else {
        uint32_t raw[256];

        gs_read_rect(g, GS_PSMCT32, cbp, 1, 0, 0, w, hh, raw);
        for (i = 0; i < n; i++) {
            uint32_t v = raw[csm1_pos(i, n)], k = (off + i) & 255;

            c->h[k] = (uint16_t)v;
            c->h[256 + k] = (uint16_t)(v >> 16);
        }
    }
}

void gs_clut_read(const GsClut *c, int psm, int cpsm, uint32_t csa, uint64_t texa, uint32_t out[256])
{
    uint32_t n = psm == GS_PSMT8 ? 256 : 16, off = clut_offset(cpsm, csa), i;
    uint32_t ta0 = (uint32_t)(texa & 0xFF), ta1 = (uint32_t)((texa >> 32) & 0xFF);

    for (i = 0; i < n; i++) {
        if (cpsm == GS_PSMCT16) {
            out[i] = gs_ct16_rgba(c->h[(off + i) & 511], ta0, ta1);
        } else {
            uint32_t k = (off + i) & 255;

            out[i] = c->h[k] | (uint32_t)c->h[256 + k] << 16;
        }
    }
    for (; i < 256; i++) {
        out[i] = 0;
    }
}
