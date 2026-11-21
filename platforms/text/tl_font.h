/* The text layer's fonts (tl_font.c), drawn one bit a pixel. */
#ifndef GGX_TL_FONT_H
#define GGX_TL_FONT_H

#include <stddef.h>
#include <stdint.h>

enum { TL_FONT_DEFAULT = -1, TL_FONT_SERIF = 0, TL_FONT_SANS = 1, TL_FONTS = 2 };
/* or'ed into a font: emboldened by a pixel (each glyph drawn twice, a
   pixel apart, and a pixel wider), for lettering set heavier than the
   face's regular weight */
#define TL_BOLD 0x100

typedef struct TlFonts TlFonts;

typedef struct {
    float scale;             /* font units to pixels */
    float ascent, descent;   /* pixels above and below the baseline */
} TlMetrics;

typedef void (*TlPlot)(void *ctx, int x, int y);

/* DejaVuSerif.ttf and NotoSans-Regular.ttf from dir. */
TlFonts *tl_fonts_load(const char *dir, char *err, size_t cap);
void tl_fonts_free(TlFonts *f);
/* For a font drawn px pixels to the em. */
void tl_font_metrics(TlFonts *f, int font, float px, TlMetrics *m);
/* A code point's advance, with the kerning before `next` (0: none). */
float tl_font_advance(TlFonts *f, int font, float scale, uint32_t cp, uint32_t next);
/* Plots each pixel of a code point's glyph with its origin at x on the
   baseline row. */
void tl_font_glyph(TlFonts *f, int font, float scale, uint32_t cp, float x, int baseline, TlPlot plot, void *ctx);

#endif
