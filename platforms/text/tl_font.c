/* The text layer's fonts: stb_truetype (third_party/stb). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_truetype.h"

#include "fsys.h"
#include "tl_font.h"

static const char *const FILES[TL_FONTS] = { "DejaVuSerif.ttf", "NotoSans-Regular.ttf" };

struct TlFonts {
    uint8_t *data[TL_FONTS];
    stbtt_fontinfo info[TL_FONTS];
};

TlFonts *tl_fonts_load(const char *dir, char *err, size_t cap)
{
    TlFonts *f = calloc(1, sizeof(TlFonts));
    int i;

    if (!f) {
        snprintf(err, cap, "out of memory");
        return NULL;
    }
    for (i = 0; i < TL_FONTS; i++) {
        char path[1100];
        FILE *fp;
        int64_t n;

        if (fs_join(path, sizeof(path), dir, "/", FILES[i]) != 0 || !(fp = fs_open(path, "rb"))) {
            snprintf(err, cap, "cannot read the font %s/%s", dir, FILES[i]);
            tl_fonts_free(f);
            return NULL;
        }
        n = fs_size(fp);
        f->data[i] = n > 0 ? malloc((size_t)n) : NULL;
        if (!f->data[i] || fs_seek(fp, 0) != 0 || fread(f->data[i], 1, (size_t)n, fp) != (size_t)n ||
            !stbtt_InitFont(&f->info[i], f->data[i], stbtt_GetFontOffsetForIndex(f->data[i], 0))) {
            fclose(fp);
            snprintf(err, cap, "%s/%s is not a TrueType font", dir, FILES[i]);
            tl_fonts_free(f);
            return NULL;
        }
        fclose(fp);
    }
    return f;
}

void tl_fonts_free(TlFonts *f)
{
    int i;

    if (!f) {
        return;
    }
    for (i = 0; i < TL_FONTS; i++) {
        free(f->data[i]);
    }
    free(f);
}

void tl_font_metrics(TlFonts *f, int font, float px, TlMetrics *m)
{
    const stbtt_fontinfo *fi = &f->info[font & 0xFF];
    int asc, desc, gap;

    m->scale = stbtt_ScaleForMappingEmToPixels(fi, px);
    stbtt_GetFontVMetrics(fi, &asc, &desc, &gap);
    m->ascent = asc * m->scale;
    m->descent = -desc * m->scale;
}

static int glyph(const stbtt_fontinfo *fi, uint32_t cp)
{
    int g = stbtt_FindGlyphIndex(fi, (int)cp);

    return g ? g : stbtt_FindGlyphIndex(fi, '?');
}

float tl_font_advance(TlFonts *f, int font, float scale, uint32_t cp, uint32_t next)
{
    const stbtt_fontinfo *fi = &f->info[font & 0xFF];
    int g = glyph(fi, cp), adv, lsb;

    stbtt_GetGlyphHMetrics(fi, g, &adv, &lsb);
    if (next) {
        adv += stbtt_GetGlyphKernAdvance(fi, g, glyph(fi, next));
    }
    return adv * scale + (font & TL_BOLD ? 1.0f : 0.0f);
}

/* Coverage at 4x4 samples a pixel, kept where at least half is covered:
   one bit a pixel, as the original lettering is, without the speckle a
   single sample at each pixel's centre gives thin serifs. */
#define OVER 4

void tl_font_glyph(TlFonts *f, int font, float scale, uint32_t cp, float x, int baseline, TlPlot plot, void *ctx)
{
    const stbtt_fontinfo *fi = &f->info[font & 0xFF];
    int g = glyph(fi, cp), x0, y0, x1, y1, w, h, px, py;
    float sub = x - (float)(int)x;
    uint8_t *bmp;

    stbtt_GetGlyphBitmapBoxSubpixel(fi, g, scale * OVER, scale * OVER, sub * OVER, 0, &x0, &y0, &x1, &y1);
    w = x1 - x0;
    h = y1 - y0;
    if (w <= 0 || h <= 0) {
        return;
    }
    bmp = calloc((size_t)w * h, 1);
    if (!bmp) {
        return;
    }
    stbtt_MakeGlyphBitmapSubpixel(fi, bmp, w, h, w, scale * OVER, scale * OVER, sub * OVER, 0, g);
    /* the oversampled box starts at x0, y0 in OVER-times pixels */
    for (py = (int)((y0 >= 0 ? y0 : y0 - OVER + 1) / OVER); py * OVER < y1; py++) {
        for (px = (int)((x0 >= 0 ? x0 : x0 - OVER + 1) / OVER); px * OVER < x1; px++) {
            int sum = 0, sx, sy;

            for (sy = 0; sy < OVER; sy++) {
                int yy = py * OVER + sy - y0;

                if (yy < 0 || yy >= h) {
                    continue;
                }
                for (sx = 0; sx < OVER; sx++) {
                    int xx = px * OVER + sx - x0;

                    if (xx >= 0 && xx < w) {
                        sum += bmp[yy * w + xx];
                    }
                }
            }
            if (sum >= 128 * OVER * OVER / 2 + 1) {
                plot(ctx, (int)x + px, baseline + py);
                if (font & TL_BOLD) {
                    plot(ctx, (int)x + px + 1, baseline + py);
                }
            }
        }
    }
    free(bmp);
}
