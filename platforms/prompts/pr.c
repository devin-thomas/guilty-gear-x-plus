/* Button prompts (pr.h). */
#include "pr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gs_mem.h"

#define FONT_W 64
#define FONT_H 64
#define CELL_W 12
#define CELL_H 16
#define COLS 5
#define BYTES (FONT_W * FONT_H / 2)

/* Where KEY CONFIG draws its symbols (opt_cfg.c disp_activebtn: x =
   padid * 256 + 256, y = row * 24 + 138); the help lines are lower down. */
#define PER_SIDE_BELOW_Y 400
#define FONT_PANEL_W 640
#define FONT_PANEL_H 448
#define SIDE_SPLIT_X 384

struct Prompts {
    uint8_t digits[BYTES], symbols[BYTES];
    uint8_t remap[16];       /* digits' CLUT index -> symbols' */
    uint32_t clut[16];       /* the symbols picture's CLUT (RGBA8, GS alpha), for pr_text */
    uint64_t hash;           /* the symbols picture's upload */
    uint32_t letters[2];     /* per player, packed: one byte per button */
    uint32_t help[2];        /* the same for the help lines */
    int shared;
    uint8_t out[BYTES];
};

/* The cell (in its picture) a letter or the symbols are drawn from. */
static int cell_of(char c, int *pic, int *col, int *row)
{
    int i;

    if (c >= PR_SYMBOL(PR_CIRCLE) && c <= PR_SYMBOL(PR_TRIANGLE)) {
        *pic = 1, i = 16 + c - PR_SYMBOL(PR_CIRCLE);   /* the symbols' own cells, after Z */
    } else if (c >= '0' && c <= '9') {
        *pic = 0, i = c - '0';
    } else if (c >= 'A' && c <= 'J') {
        *pic = 0, i = 10 + c - 'A';
    } else if (c >= 'K' && c <= 'Z') {
        *pic = 1, i = c - 'K';
    } else {
        return -1;
    }
    *col = i % COLS;
    *row = i / COLS;
    return 0;
}

int pr_has_letter(char c)
{
    int pic, col, row;

    return cell_of(c, &pic, &col, &row) == 0;
}

static int dist(uint32_t a, uint32_t b)
{
    int d = 0, k;

    for (k = 0; k < 32; k += 8) {
        int x = (int)((a >> k) & 0xFF) - (int)((b >> k) & 0xFF);

        d += x * x;
    }
    return d;
}

Prompts *pr_open(const TlRawPicture *digits, const TlRawPicture *symbols, char *err, size_t cap)
{
    Prompts *p;
    int i, j;

    if (digits->psm != GS_PSMT4 || symbols->psm != GS_PSMT4 || digits->w != FONT_W || digits->h != FONT_H ||
        symbols->w != FONT_W || symbols->h != FONT_H) {
        snprintf(err, cap, "the HUD font's pictures are not 64x64 PSMT4");
        return NULL;
    }
    if (!(p = calloc(1, sizeof(*p)))) {
        snprintf(err, cap, "out of memory");
        return NULL;
    }
    memcpy(p->digits, digits->texels, BYTES);
    memcpy(p->symbols, symbols->texels, BYTES);
    p->hash = symbols->hash;
    memcpy(p->clut, symbols->clut, sizeof(p->clut));
    for (i = 0; i < 16; i++) {
        /* the same colour in the other CLUT (the two hold the same
           colours, in another order), else the nearest */
        int best = 0, bd = -1;

        for (j = 0; j < 16; j++) {
            int d = dist(digits->clut[i], symbols->clut[j]);

            if ((symbols->clut[j] >> 24) == 0 && (digits->clut[i] >> 24) != 0) {
                continue;
            }
            if (bd < 0 || d < bd) {
                bd = d, best = j;
            }
        }
        p->remap[i] = (uint8_t)best;
    }
    return p;
}

void pr_free(Prompts *p)
{
    free(p);
}

static uint32_t pack(const char letters[PR_NBUTTONS])
{
    uint32_t v = 0;
    int b;

    for (b = 0; b < PR_NBUTTONS; b++) {
        char c = letters[b];

        /* a button's own symbol in its own cell is no change */
        if (c == PR_SYMBOL(b) || !pr_has_letter(c)) {
            c = 0;
        }
        v |= (uint32_t)(uint8_t)c << (8 * b);
    }
    return v;
}

void pr_set(Prompts *p, int player, const char letters[PR_NBUTTONS])
{
    p->letters[player & 1] = pack(letters);
}

void pr_set_help(Prompts *p, int player, const char letters[PR_NBUTTONS])
{
    p->help[player & 1] = pack(letters);
}

void pr_set_shared(Prompts *p, int player)
{
    p->shared = player & 1;
}

uint32_t pr_variant(void *ctx, uint64_t upload_hash, int x, int y)
{
    Prompts *p = ctx;

    if (upload_hash != p->hash) {
        return 0;
    }
    return y < PER_SIDE_BELOW_Y ? p->letters[x >= SIDE_SPLIT_X] : p->help[p->shared];
}

static int texel(const uint8_t *t, int x, int y)
{
    int i = y * FONT_W + x;

    return (t[i / 2] >> (i & 1 ? 4 : 0)) & 0xF;
}

static void put(uint8_t *t, int x, int y, int v)
{
    int i = y * FONT_W + x;

    t[i / 2] = (uint8_t)(i & 1 ? (t[i / 2] & 0x0F) | v << 4 : (t[i / 2] & 0xF0) | v);
}

const uint8_t *pr_subst(void *ctx, uint64_t upload_hash, int psm, uint32_t w, uint32_t h, const uint8_t *texels,
                        uint32_t variant)
{
    Prompts *p = ctx;
    int b, x, y;

    if (upload_hash != p->hash || !variant || psm != GS_PSMT4 || w != FONT_W || h != FONT_H) {
        return NULL;
    }
    /* dl_stream decodes it at once, so one buffer does */
    memcpy(p->out, texels, BYTES);
    for (b = 0; b < PR_NBUTTONS; b++) {
        char c = (char)(variant >> (8 * b));
        int pic, col, row;
        /* the symbols' cells: the last row, after Z */
        int dx = (1 + b) * CELL_W, dy = 3 * CELL_H;

        if (!c || cell_of(c, &pic, &col, &row) != 0) {
            continue;
        }
        for (y = 0; y < CELL_H; y++) {
            for (x = 0; x < CELL_W; x++) {
                int v = pic ? texel(p->symbols, col * CELL_W + x, row * CELL_H + y)
                            : p->remap[texel(p->digits, col * CELL_W + x, row * CELL_H + y)];

                put(p->out, dx + x, dy + y, v);
            }
        }
    }
    return p->out;
}

int pr_text(const Prompts *p, uint8_t *rgba, int x, int y, const char *s, int scale, int bright)
{
    int n = 0;

    for (; *s; s++, x += CELL_W * scale, n++) {
        int pic, col, row, cx, cy;
        char c = *s >= 'a' && *s <= 'z' ? (char)(*s - 'a' + 'A') : *s;

        if (cell_of(c, &pic, &col, &row) != 0) {
            continue;    /* a space, or a character the font hasn't */
        }
        for (cy = 0; cy < CELL_H * scale; cy++) {
            for (cx = 0; cx < CELL_W * scale; cx++) {
                int tx = col * CELL_W + cx / scale, ty = row * CELL_H + cy / scale;
                int v = pic ? texel(p->symbols, tx, ty) : p->remap[texel(p->digits, tx, ty)];
                uint32_t k = p->clut[v];
                int px = x + cx, py = y + cy;
                uint8_t *o;

                if (!(k >> 24) || px < 0 || py < 0 || px >= FONT_PANEL_W || py >= FONT_PANEL_H) {
                    continue;
                }
                o = rgba + 4 * ((size_t)py * FONT_PANEL_W + px);
                o[0] = (uint8_t)((k & 0xFF) * bright / 256);
                o[1] = (uint8_t)(((k >> 8) & 0xFF) * bright / 256);
                o[2] = (uint8_t)(((k >> 16) & 0xFF) * bright / 256);
                o[3] = 0x80;
            }
        }
    }
    return n;
}
