/* The text layer's parts (tl.h): translation entries, the fonts, drawing. */
#ifndef GGX_TL_INTERNAL_H
#define GGX_TL_INTERNAL_H

#include <stdint.h>

#include "tl.h"
#include "tl_font.h"

enum { TL_ALIGN_AUTO = 0, TL_ALIGN_LEFT = 1, TL_ALIGN_CENTER = 2 };

/* What a translation file says about one picture, and what tl_bind found. */
typedef struct {
    uint32_t fid;
    char path[24];
    int font;               /* TL_FONT_* */
    int rows;               /* 0: block layout; else the band pitch */
    int align;              /* TL_ALIGN_* */
    int size;               /* font pixel size; 0: from the original */
    int area[4];            /* x0, y0, x1, y1 inclusive: the part replaced
                               (x1 < 0: the whole picture) */
    int review;
    char *text;             /* UTF-8; '|' breaks a line */
    /* bound */
    int psm;                /* 0x13 PSMT8 or 0x14 PSMT4 */
    uint32_t w, h;
    uint8_t *orig;          /* upload format */
    uint8_t *english;       /* upload format, once drawn */
    int fit;                /* TL_FIT_*, once drawn */
    uint32_t clut[256];
    uint64_t hash;
} TlEntry;

/* Draws e->text in place of e->orig into a new buffer of the same format,
   and sets e->fit. NULL if out of memory. */
uint8_t *tl_draw(TlFonts *f, TlEntry *e, int font);

#endif
