/* Drawing the English picture in place of an original (LANG-FORMAT.md,
 * Layout). The original is read for its letters' index and its lines;
 * the English is set to match them, one bit a pixel, in the original's
 * format, size and palette. */
#include <stdlib.h>
#include <string.h>

#include "tl_internal.h"

#define MAX_LINES 32
#define MAX_CHARS 512
#define MARGIN 6         /* pixels kept clear at the picture's edges */
#define MIN_PX 9
#define DEFAULT_PX 16    /* the English's size, where the entry gives none */

typedef struct {
    int top, bottom, left, right;   /* inclusive */
} Line;

typedef struct {
    int bx0, by0, bx1, by1;         /* the part replaced (the entry's area), inclusive */
    int paper, ink;                 /* the background's and the letters' index */
    Line line[MAX_LINES];
    int nline;
} Original;

typedef struct {
    uint8_t *buf;
    const TlEntry *e;
    int ink;
} Canvas;

static int texel(const TlEntry *e, uint32_t x, uint32_t y)
{
    size_t i = (size_t)y * e->w + x;

    return e->psm == 0x13 ? e->orig[i] : (e->orig[i >> 1] >> ((i & 1) * 4)) & 15;
}

static void set_texel(uint8_t *buf, const TlEntry *e, int x, int y, int v)
{
    size_t i;

    if (x < 0 || y < 0 || (uint32_t)x >= e->w || (uint32_t)y >= e->h) {
        return;
    }
    i = (size_t)y * e->w + (size_t)x;
    if (e->psm == 0x13) {
        buf[i] = (uint8_t)v;
    } else {
        buf[i >> 1] = (uint8_t)((buf[i >> 1] & (i & 1 ? 0x0F : 0xF0)) | (v & 15) << ((i & 1) * 4));
    }
}

static void plot(void *ctx, int x, int y)
{
    Canvas *c = ctx;

    set_texel(c->buf, c->e, x, y, c->ink);
}

/* Within the area: the background is the most common index, the letters
   the next. Rows with letters, joined across a gap of one empty row, are
   its lines (the Japanese lines are as little as two rows apart). */
static void analyse(const TlEntry *e, Original *o)
{
    uint32_t count[256] = { 0 };
    int i, x, y, in = 0;

    o->bx0 = 0;
    o->by0 = 0;
    o->bx1 = (int)e->w - 1;
    o->by1 = (int)e->h - 1;
    if (e->area[2] >= 0) {
        o->bx0 = e->area[0] < o->bx1 ? e->area[0] : o->bx1;
        o->by0 = e->area[1] < o->by1 ? e->area[1] : o->by1;
        o->bx1 = e->area[2] < o->bx1 ? e->area[2] : o->bx1;
        o->by1 = e->area[3] < o->by1 ? e->area[3] : o->by1;
    }
    for (y = o->by0; y <= o->by1; y++) {
        for (x = o->bx0; x <= o->bx1; x++) {
            count[texel(e, x, y)]++;
        }
    }
    o->paper = 0;
    for (i = 1; i < 256; i++) {
        if (count[i] > count[o->paper]) {
            o->paper = i;
        }
    }
    o->ink = o->paper == 0 ? 1 : 0;
    for (i = 0; i < 256; i++) {
        if (i != o->paper && count[i] > count[o->ink]) {
            o->ink = i;
        }
    }
    o->nline = 0;
    for (y = o->by0; y <= o->by1; y++) {
        int left = -1, right = -1;

        for (x = o->bx0; x <= o->bx1; x++) {
            if (texel(e, (uint32_t)x, (uint32_t)y) != o->paper) {
                if (left < 0) {
                    left = x;
                }
                right = x;
            }
        }
        if (left >= 0) {
            Line *l = o->nline ? &o->line[o->nline - 1] : NULL;

            if (l && (in || y - l->bottom <= 2)) {
                if (left < l->left) {
                    l->left = left;
                }
                if (right > l->right) {
                    l->right = right;
                }
                l->bottom = y;
            } else if (o->nline < MAX_LINES) {
                l = &o->line[o->nline++];
                l->top = l->bottom = y;
                l->left = left;
                l->right = right;
            }
            in = 1;
        } else {
            in = 0;
        }
    }
}

static uint32_t utf8(const char **s)
{
    const uint8_t *p = (const uint8_t *)*s;
    uint32_t c = *p++;
    int more = c >= 0xF0 ? 3 : c >= 0xE0 ? 2 : c >= 0xC0 ? 1 : 0;

    if (more) {
        c &= 0x3F >> more;
        while (more-- && (*p & 0xC0) == 0x80) {
            c = c << 6 | (*p++ & 0x3F);
        }
    }
    *s = (const char *)p;
    return c;
}

typedef struct {
    uint32_t cp[MAX_CHARS];
    int n;
} Text;

static float width(TlFonts *f, int font, float scale, const uint32_t *cp, int n)
{
    float w = 0;
    int i;

    for (i = 0; i < n; i++) {
        w += tl_font_advance(f, font, scale, cp[i], i + 1 < n ? cp[i + 1] : 0);
    }
    return w;
}

/* Breaks the text into lines no wider than avail: at '|', and otherwise
   between words. Returns the number of lines, or -1 if a word is wider
   than avail or there are too many lines. */
static int wrap(TlFonts *f, int font, float scale, const Text *t, float avail, int start[], int len[])
{
    int n = 0, i = 0;

    while (i <= t->n) {
        int end = i, best = -1;

        /* the longest run from i that fits, ending at a space or a break */
        for (;;) {
            int stop = end;

            while (stop < t->n && t->cp[stop] != ' ' && t->cp[stop] != '|') {
                stop++;
            }
            if (width(f, font, scale, t->cp + i, stop - i) > avail) {
                break;
            }
            best = stop;
            if (stop >= t->n || t->cp[stop] == '|') {
                break;
            }
            end = stop + 1;
        }
        if (best < 0 || n == MAX_LINES) {
            return -1;
        }
        start[n] = i;
        len[n] = best - i;
        n++;
        if (best >= t->n) {
            break;
        }
        i = best + 1;   /* past the space or the break */
    }
    return n;
}

static void draw_line(TlFonts *f, int font, float scale, const Text *t, int start, int len, float x, int baseline,
                      Canvas *c)
{
    int i;

    for (i = start; i < start + len; i++) {
        uint32_t next = i + 1 < start + len ? t->cp[i + 1] : 0;

        tl_font_glyph(f, font, scale, t->cp[i], x, baseline, plot, c);
        x += tl_font_advance(f, font, scale, t->cp[i], next);
    }
}

/* block: the whole text, centred on the original's block, at its pitch. */
static void draw_block(TlFonts *f, int font, TlEntry *e, const Original *o, const Text *t, Canvas *c)
{
    int x0 = o->bx1, x1 = o->bx0, y0 = o->line[0].top, y1 = o->line[o->nline - 1].bottom, i, n = 0;
    int start[MAX_LINES], len[MAX_LINES], align = e->align, top;
    float pitch, px, size, cx, avail, scale = 1;
    TlMetrics m;

    for (i = 0; i < o->nline; i++) {
        x0 = o->line[i].left < x0 ? o->line[i].left : x0;
        x1 = o->line[i].right > x1 ? o->line[i].right : x1;
    }
    if (align == TL_ALIGN_AUTO) {
        align = TL_ALIGN_LEFT;
        for (i = 1; i < o->nline; i++) {
            if (abs(o->line[i].left - o->line[0].left) > 2) {
                align = TL_ALIGN_CENTER;
            }
        }
        if (o->nline == 1 && abs((x0 + x1) - (o->bx0 + o->bx1)) <= 8) {
            align = TL_ALIGN_CENTER;   /* one line, centred in the picture */
        }
    }
    /* one size everywhere, so the lines read alike from picture to picture
       (the originals' lettering is all one size too; their lines can touch,
       so the pitch isn't read from them) */
    px = e->size ? (float)e->size : DEFAULT_PX;
    if (px < MIN_PX) {
        px = MIN_PX;
    }
    pitch = px * 1.5f;
    size = px;
    cx = (float)(x0 + x1 + 1) / 2;
    if (align == TL_ALIGN_LEFT) {
        avail = (float)(o->bx1 + 1) - MARGIN - (float)x0;
    } else {
        float half = cx - (float)o->bx0 < (float)(o->bx1 + 1) - cx ? cx - (float)o->bx0 : (float)(o->bx1 + 1) - cx;

        avail = 2 * half - 2 * MARGIN;
    }
    /* shrink until it fits across and down */
    for (;;) {
        tl_font_metrics(f, font, px, &m);
        scale = m.scale;
        n = wrap(f, font, scale, t, avail, start, len);
        if ((n > 0 && (float)(n - 1) * pitch + m.ascent + m.descent <= (float)(o->by1 - o->by0 - 1)) ||
            px - 1 < MIN_PX) {
            break;
        }
        pitch = pitch * (px - 1) / px;
        px -= 1;
    }
    if (n <= 0 || (float)(n - 1) * pitch + m.ascent + m.descent > (float)(o->by1 - o->by0 - 1)) {
        e->fit = TL_FIT_NOT;
        if (n <= 0) {
            return;
        }
    } else if (px < size) {
        e->fit = TL_FIT_SMALLER;
    }
    /* the block's middle on the original's */
    top = (int)((float)(y0 + y1 + 1) / 2 - ((float)(n - 1) * pitch + m.ascent + m.descent) / 2 + 0.5f);
    if (top + (n - 1) * pitch + m.ascent + m.descent > (float)o->by1) {
        top = (int)((float)o->by1 - (n - 1) * pitch - m.ascent - m.descent);
    }
    if (top < o->by0 + 1) {
        top = o->by0 + 1;
    }
    for (i = 0; i < n; i++) {
        float w = width(f, font, scale, t->cp + start[i], len[i]);
        float x = align == TL_ALIGN_LEFT ? (float)x0 : cx - w / 2;
        int baseline = top + (int)((float)i * pitch + m.ascent + 0.5f);

        draw_line(f, font, scale, t, start[i], len[i], (float)(int)(x + 0.5f), baseline, c);
    }
}

/* rows: English line n in band n, where the game draws that band alone. */
static void draw_rows(TlFonts *f, int font, TlEntry *e, const Original *o, const Text *t, Canvas *c)
{
    int band = e->rows, nb = (int)e->h / band, i, b = 0, left = (int)e->w;
    int height = 0, lines = 0;
    float px;
    TlMetrics m;

    for (i = 0; i < o->nline; i++) {
        left = o->line[i].left < left ? o->line[i].left : left;
        height += o->line[i].bottom - o->line[i].top + 1;
        lines++;
    }
    if (!lines) {
        left = MARGIN;
        height = band * 2 / 3;
        lines = 1;
    }
    /* the letters as tall as the original's */
    px = e->size ? (float)e->size : (float)height / (float)lines * 0.9f + 0.5f;
    if (px > band - 2) {
        px = (float)(band - 2);
    }
    for (i = 0; i <= t->n && b < nb; b++) {
        int end = i, mid = b * band + band / 2;
        float size = px, avail = (float)e->w - MARGIN - (float)left;
        size_t k;

        while (end < t->n && t->cp[end] != '|') {
            end++;
        }
        /* centre the line on the original's line in this band, if any */
        for (k = 0; k < (size_t)o->nline; k++) {
            if (o->line[k].top >= b * band && o->line[k].bottom < (b + 1) * band) {
                mid = (o->line[k].top + o->line[k].bottom + 1) / 2;
            }
        }
        for (;;) {
            tl_font_metrics(f, font, size, &m);
            if (width(f, font, m.scale, t->cp + i, end - i) <= avail) {
                break;
            }
            if (size - 1 < MIN_PX) {
                e->fit = TL_FIT_NOT;
                break;
            }
            size -= 1;
            e->fit = e->fit > TL_FIT_SMALLER ? e->fit : TL_FIT_SMALLER;
        }
        if (end > i) {
            int baseline = mid + (int)((m.ascent - m.descent) / 2 + 0.5f);

            draw_line(f, font, m.scale, t, i, end - i, (float)left, baseline, c);
        }
        i = end + 1;
    }
    if (i <= t->n) {
        e->fit = TL_FIT_NOT;   /* more lines than bands */
    }
}

uint8_t *tl_draw(TlFonts *f, TlEntry *e, int font)
{
    size_t bytes = e->psm == 0x13 ? (size_t)e->w * e->h : ((size_t)e->w * e->h + 1) / 2;
    uint8_t *buf = malloc(bytes);
    Original o;
    Canvas c;
    Text t;
    const char *s = e->text;
    int x, y;

    if (!buf || !f) {
        free(buf);
        return NULL;
    }
    e->fit = TL_FIT;
    analyse(e, &o);
    memcpy(buf, e->orig, bytes);
    for (y = o.by0; y <= o.by1; y++) {
        for (x = o.bx0; x <= o.bx1; x++) {
            set_texel(buf, e, x, y, o.paper);
        }
    }
    for (t.n = 0; *s && t.n < MAX_CHARS;) {
        t.cp[t.n++] = utf8(&s);
    }
    c.buf = buf;
    c.e = e;
    c.ink = o.ink;
    if (e->rows) {
        draw_rows(f, font, e, &o, &t, &c);
    } else if (o.nline) {
        draw_block(f, font, e, &o, &t, &c);
    } else {
        e->fit = TL_FIT_NOT;   /* an original with no letters: nothing to match */
    }
    return buf;
}
