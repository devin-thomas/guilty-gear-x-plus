/* The GS Path as dl_stream.c interprets it (PORT-009): no GPU, no game data.
 *
 * The packets are built as graph.c, text.c and packet.c build them (frame
 * setup and the clear on list 0, the debug text and the GraphConv blit on
 * list 3) and sent through a DMA chain in a fake scratchpad, with the
 * privileged registers GS_runtime_preset writes for a screen offset. The
 * frame that comes out must hold the clear as a primitive in the draw
 * buffer, the debug text only where it falls inside the scissor, the blit
 * with the offset's source and size, and display circuits placed by the
 * offset; and a copy within plain local memory must land in it.
 *
 *   ggx-gs-path-test     (exit status 0 when every check passes)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dl_stream.h"

static int failures;

#define CHECK(c)                                                         \
    do {                                                                 \
        if (!(c)) {                                                      \
            fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
            failures++;                                                  \
        }                                                                \
    } while (0)

/* A DMA chain being built in the scratchpad: one END tag, then GIF data. */
typedef struct {
    uint8_t spr[16384];
    uint32_t n;      /* qwords after the tag */
} Chain;

static void qword(Chain *c, uint64_t lo, uint64_t hi)
{
    memcpy(c->spr + 16 + 16 * c->n, &lo, 8);
    memcpy(c->spr + 24 + 16 * c->n, &hi, 8);
    c->n++;
}

/* PacketPackRegWithTag: a PACKED A+D register write */
static void ad(Chain *c, uint32_t reg, uint64_t v)
{
    qword(c, 0x1000000000000001ull, 0xE);
    qword(c, v, reg);
}

/* A REGLIST tag with nloop loops of the given registers, then the words */
static void reglist(Chain *c, uint32_t nloop, uint32_t nreg, uint64_t regs, const uint64_t *w, uint32_t nw)
{
    uint32_t i;

    qword(c, nloop | 1ull << 58 | (uint64_t)nreg << 60, regs);
    for (i = 0; i < nw; i += 2) {
        qword(c, w[i], i + 1 < nw ? w[i + 1] : 0);
    }
}

static void send(DlStream *s, Chain *c, int list, const uint8_t *priv)
{
    uint64_t tag = 0x70000000ull | c->n;
    DlMem m;

    memcpy(c->spr, &tag, 8);
    memset(c->spr + 8, 0, 8);
    m.ram = NULL;
    m.spr = c->spr;
    m.priv = priv;
    dl_stream_send(s, &m, 0x80000000u, list);
    c->n = 0;
}

/* graph.c GraphSetOffset, GS_runtime_preset and GraphConv for a screen offset */
typedef struct {
    int dx, dy, sizex, sizey;            /* disp.display and dispoffset */
    uint64_t trxpos, trxreg;             /* GraphConv's blit */
} Offset;

static Offset offset(int x, int y)
{
    Offset o;
    int sx, sy, w, h;

    o.sizey = 0x1BF - abs(y);
    o.sizex = (640 - abs(x)) * 4 - 1;
    o.dx = x > 0 ? x * 4 : 0;
    o.dy = y > 0 ? y : 0;
    sx = x >= 0 ? 0 : -x;
    w = x >= 0 ? 640 : x + 640;
    sy = y >= 0 ? 0 : -y;
    h = y >= 0 ? 448 : y + 448;
    o.trxpos = (uint64_t)sx | (uint64_t)sy << 16;
    o.trxreg = (uint64_t)w | (uint64_t)h << 32;
    return o;
}

static uint64_t g_display(int dx, int dy, int magh, int magv, int dw, int dh)
{
    return (uint64_t)dx | (uint64_t)dy << 12 | (uint64_t)magh << 23 | (uint64_t)magv << 27 | (uint64_t)dw << 32 |
           (uint64_t)dh << 44;
}

static void preset(uint8_t *priv, const Offset *o)
{
    uint64_t v[5] = { 0x7F27, 0x1400, g_display(o->dx + 636, o->dy + 50, 3, 0, o->sizex, o->sizey), 0x1400,
                      g_display(o->dx + 636, o->dy + 51, 3, 0, o->sizex, o->sizey) };

    memset(priv, 0, 8192);
    memcpy(priv + 0x00, &v[0], 8);
    memcpy(priv + 0x70, &v[1], 8);
    memcpy(priv + 0x80, &v[2], 8);
    memcpy(priv + 0x90, &v[3], 8);
    memcpy(priv + 0xA0, &v[4], 8);
}

/* GraphBegin's list 0 (MakeFullBuffer), with GraphInit's environment first */
static void graph_begin(DlStream *s, Chain *c, uint32_t back_color, int init)
{
    uint64_t w[4] = { 0x146, back_color, 0x79006C00, 0x95009400 };

    if (init) {
        ad(c, 0x18, 0x6C00ull | 0x7900ull << 32);   /* XYOFFSET_1 */
        ad(c, 0x46, 1);                             /* COLCLAMP */
        ad(c, 0x47, 0x30000);                       /* TEST_1: ZTST ALWAYS */
        ad(c, 0x42, 0x44);
        ad(c, 0x3B, 0x8000000000ull);
    }
    ad(c, 0x4C, 0xA008C);
    ad(c, 0x40, 0x01BF0000027F0000ull);
    ad(c, 0x42, 0x80000000A8ull);
    reglist(c, 1, 4, 0x5510, w, 4);
    send(s, c, 0, NULL);
}

/* text.c TextOpen and TextPutC for len cells from cell (x, y) */
static void text(Chain *c, uint32_t x, uint32_t y, uint32_t len)
{
    uint64_t w[4 * 8];
    uint32_t i;

    ad(c, 0x00, 0x156);
    ad(c, 0x42, 0x8000000044ull);
    ad(c, 0x06, 0x301460059D40A301ull);
    ad(c, 0x01, 0x7F | 0x7F << 8 | 0x7F << 16 | 0x80ull << 24);
    for (i = 0; i < len; i++) {
        uint64_t px = ((x + i) << 8) + 0x6C00, py = (y << 8) + 0x7900;

        w[4 * i + 0] = 0;
        w[4 * i + 1] = px | py << 16 | 0x9F0D17Full << 32;
        w[4 * i + 2] = 0x80 | 0x80ull << 16;
        w[4 * i + 3] = (px + 0x100) | (py + 0x100) << 16 | 0x9F0D17Full << 32;
    }
    reglist(c, len, 4, 0x5353, w, 4 * len);
}

/* GraphEnd's GraphConv on list 3 */
static void graph_conv(DlStream *s, Chain *c, const Offset *o, const uint8_t *priv)
{
    uint64_t w[8] = { 0xA0000000A1180ull, 0x50, o->trxpos, 0x51, o->trxreg, 0x52, 2, 0x53 };

    qword(c, 0x1000000000000004ull, 0xE);
    qword(c, w[0], w[1]);
    qword(c, w[2], w[3]);
    qword(c, w[4], w[5]);
    qword(c, w[6], w[7]);
    ad(c, 0x42, 0x8000000064ull);
    send(s, c, 3, priv);
}

static void check_frame(DlStream *s, int x, int y, uint32_t back)
{
    static Chain c;
    uint8_t priv[8192];
    Offset o = offset(x, y);
    const DlFrame *f;
    const DlBatch *b;
    const DlBlit *k;
    uint32_t i;

    preset(priv, &o);
    graph_begin(s, &c, back, 1);
    text(&c, 40, 5, 4);   /* main's JobMode: pixel 640, outside the scissor */
    send(s, &c, 3, NULL);
    graph_conv(s, &c, &o, priv);
    f = dl_stream_frame(s);
    CHECK(f != NULL);
    if (!f) {
        return;
    }
    /* the clear: one untextured sprite over 640x448 in the draw buffer */
    CHECK(f->nb == 2);
    b = &f->b[0];
    CHECK(b->blit < 0 && b->count == 6 && b->tex < 0);
    CHECK(b->blend.abe && b->blend.a == 0 && b->blend.b == 2 && b->blend.c == 2 && b->blend.d == 2 &&
          b->blend.fix == 0x80);
    CHECK(f->surf[b->surface].bp == 0x1180 && f->surf[b->surface].bw == 10);
    CHECK(f->v[0].x == 0.5f && f->v[0].y == 0.5f && f->v[4].x == 640.5f && f->v[4].y == 448.5f);
    CHECK(f->v[0].r == (back & 0xFF) && f->v[0].g == ((back >> 8) & 0xFF) && f->v[0].b == ((back >> 16) & 0xFF));
    /* the blit: draw buffer to display buffer, from the offset's source */
    b = &f->b[1];
    CHECK(b->blit == 0 && f->nblit == 1);
    k = &f->blit[0];
    CHECK(f->surf[k->src].bp == 0x1180 && f->surf[k->dst].bp == 0);
    CHECK(k->sx == (o.trxpos & 0x7FF) && k->sy == ((o.trxpos >> 16) & 0x7FF) && k->dx == 0 && k->dy == 0);
    CHECK(k->w == (o.trxreg & 0xFFF) && k->h == ((o.trxreg >> 32) & 0xFFF));
    /* the display: both circuits on the display buffer, the second a line lower */
    CHECK(f->display.mmod == 1 && f->display.slbg == 0 && f->display.alp == 0x7F && f->display.bgcolor == 0);
    for (i = 0; i < 2; i++) {
        const DlCircuit *d = &f->display.c[i];

        CHECK(d->on && f->surf[d->surface].bp == 0 && d->dbx == 0 && d->dby == 0);
        CHECK(d->x == (x > 0 ? x : 0) && d->y == (y > 0 ? y : 0) + (int)i);
        CHECK(d->w == 640 - abs(x) && d->h == 448 - abs(y));
        CHECK(d->magh == 3 && d->magv == 0 && !d->half);
    }
}

int main(void)
{
    DlStream *s = dl_stream_new();
    static Chain c;
    const DlStats *st;
    const DlFrame *f;
    uint8_t priv[8192];
    Offset o;
    int i;

    if (!s) {
        return 2;
    }
    /* frames with the screen offsets Options > Adjust allows (-15 to 15) */
    check_frame(s, 0, 0, 0);
    check_frame(s, 9, 6, 0x00112233);
    check_frame(s, -15, -15, 0x00FFFFFF);
    check_frame(s, 15, -7, 0);

    /* the debug text inside the scissor: four textured sprites in the draw buffer */
    o = offset(0, 0);
    preset(priv, &o);
    graph_begin(s, &c, 0, 0);
    text(&c, 0, 5, 4);
    send(s, &c, 3, NULL);
    graph_conv(s, &c, &o, priv);
    f = dl_stream_frame(s);
    CHECK(f && f->nb == 3 && f->b[1].count == 24 && f->b[1].tex >= 0 && f->b[1].tex_kind == DL_TEX_INDEX);
    CHECK(f && f->b[1].clut >= 0 && f->b[1].tcc == 1 && f->b[1].tfx == 0 && f->b[1].tw == 7 && f->b[1].th == 6);

    /* a PCSX2 dump's registers replace the display of the frame last shown */
    o = offset(-4, 3);
    preset(priv, &o);
    dl_stream_priv(s, priv);
    f = dl_stream_frame(s);
    CHECK(f && f->display.c[0].x == 0 && f->display.c[0].y == 3 && f->display.c[0].w == 636);
    /* circuit 2 off, and SLBG: the frame's display says so */
    priv[0] = 0x25 | 0x80;
    dl_stream_priv(s, priv);
    CHECK(f->display.c[0].on && !f->display.c[1].on && f->display.slbg);

    /* a copy within plain local memory: an 8x8 CT32 image at block 0x3000,
       copied to 0x3100, lands there */
    {
        uint32_t img[64], out[64];
        uint64_t w[8] = { (0x3000ull | 1ull << 16) << 32, 0x50, 0, 0x51, 8 | 8ull << 32, 0x52, 0, 0x53 };

        for (i = 0; i < 64; i++) {
            img[i] = 0x01020304u * (uint32_t)(i + 1);
        }
        graph_begin(s, &c, 0, 0);
        qword(&c, 0x1000000000000004ull, 0xE);
        for (i = 0; i < 8; i += 2) {
            qword(&c, w[i], w[i + 1]);
        }
        qword(&c, 16 | 2ull << 58, 0);   /* IMAGE, 16 qwords */
        for (i = 0; i < 16; i++) {
            uint64_t lo, hi;

            memcpy(&lo, &img[4 * i], 8);
            memcpy(&hi, &img[4 * i + 2], 8);
            qword(&c, lo, hi);
        }
        w[0] = 0x3000ull | 1ull << 16 | (0x3100ull | 1ull << 16) << 32;
        w[2] = 0;
        w[6] = 2;
        qword(&c, 0x1000000000000004ull, 0xE);
        for (i = 0; i < 8; i += 2) {
            qword(&c, w[i], w[i + 1]);
        }
        send(s, &c, 2, NULL);
        gs_read_rect(dl_stream_gs_mem(s), GS_PSMCT32, 0x3100, 1, 0, 0, 8, 8, out);
        CHECK(memcmp(img, out, sizeof(img)) == 0);
    }

    /* a line and a point (MeterDraw's primitives) become quads */
    {
        uint64_t w[6] = { 0x02, 0xFF, 0x7900ull << 16 | 0x6C00, 0x7900ull << 16 | 0x6D00 };
        uint32_t nv;

        f = dl_stream_frame(s);
        graph_begin(s, &c, 0, 0);
        reglist(&c, 1, 4, 0x5510, w, 4);   /* PRIM LINE, RGBAQ, XYZ2, XYZ2 */
        w[0] = 0x00;
        reglist(&c, 1, 3, 0x510, w, 3);    /* PRIM POINT, RGBAQ, XYZ2 */
        send(s, &c, 2, NULL);
        o = offset(0, 0);
        preset(priv, &o);
        graph_conv(s, &c, &o, priv);
        f = dl_stream_frame(s);
        nv = f ? f->nv : 0;
        CHECK(nv == 6 + 6 + 6);
        CHECK(f && f->v[6].x == 0.5f && f->v[6].y == 0.0f && f->v[7].x == 16.5f && f->v[7].y == 0.0f);
        CHECK(f && f->v[12].x == 0.5f && f->v[12].y == 0.5f && f->v[16].x == 1.5f && f->v[16].y == 1.5f);
    }

    /* a T8 picture sent as PXF_SendTexture sends it, w * h / 16 qwords: at
       12x10 that is 7 qwords, 8 texels short of the 120. What came is the
       picture, drawn straight after; the 8 that never came keep what local
       memory held (Testament's 300x350 pictures, before which the whole
       picture was dropped) */
    {
        uint8_t old[120], pic[120], want[120], got[120];
        uint64_t w[8] = { (0x3000ull | 1ull << 16 | 0x13ull << 24) << 32, 0x50, 0, 0x51, 12 | 10ull << 32, 0x52, 0,
                          0x53 };
        uint64_t uv[4] = { 0, 0x7900ull << 16 | 0x6C00, 12 << 4 | 10ull << 20, (0x7900ull + 0xA0) << 16 | 0x6CC0 };
        const DlTexCache *tc;
        const DlTex *t;

        for (i = 0; i < 120; i++) {
            old[i] = 0xEE;
            pic[i] = (uint8_t)(i * 7 + 1);
            want[i] = i < 112 ? pic[i] : old[i];
        }
        gs_write_rect(dl_stream_gs_mem(s), GS_PSMT8, 0x3000, 1, 0, 0, 12, 10, old);
        graph_begin(s, &c, 0, 0);
        qword(&c, 0x1000000000000004ull, 0xE);
        for (i = 0; i < 8; i += 2) {
            qword(&c, w[i], w[i + 1]);
        }
        qword(&c, 120 / 16 | 1ull << 15 | 2ull << 58, 0);   /* IMAGE, 7 qwords */
        for (i = 0; i < 7; i++) {
            uint64_t lo, hi;

            memcpy(&lo, &pic[16 * i], 8);
            memcpy(&hi, &pic[16 * i + 8], 8);
            qword(&c, lo, hi);
        }
        /* TEX0_1: T8, TW 4, TH 4, TCC, CLUT CT32 at 0x3100 loaded; a sprite */
        ad(&c, 0x06, 0x3000 | 1ull << 14 | 0x13ull << 20 | 4ull << 26 | 4ull << 30 | 1ull << 34 | 0x3100ull << 37 |
                         1ull << 61);
        ad(&c, 0x00, 0x156);
        reglist(&c, 1, 4, 0x5353, uv, 4);
        send(s, &c, 2, NULL);
        o = offset(0, 0);
        preset(priv, &o);
        graph_conv(s, &c, &o, priv);
        f = dl_stream_frame(s);
        tc = dl_stream_textures(s);
        t = f && f->nb == 3 && f->b[1].tex >= 0 ? &tc->e[f->b[1].tex] : NULL;
        CHECK(t && t->w == 12 && t->h == 10 && t->kind == DL_TEX_INDEX);
        CHECK(t && t->pixels && memcmp(t->pixels, want, sizeof(want)) == 0);
        gs_read_rect(dl_stream_gs_mem(s), GS_PSMT8, 0x3000, 1, 0, 0, 12, 10, got);
        CHECK(memcmp(got, want, sizeof(want)) == 0);
        CHECK(dl_stream_stats(s)->uploads_short == 1);
    }

    st = dl_stream_stats(s);
    CHECK(st->gs_unsupported == 0 && st->unsupported == 0 && st->state_unsupported == 0);
    CHECK(st->clipped == 16 && st->text_prims == 4 && st->lines == 1 && st->points == 1);
    CHECK(st->local_copies == 1 && st->surface_reads == 0 && st->surface_writes == 0);
    if (failures) {
        dl_stream_print_stats(s, stderr);
    }
    dl_stream_free(s);
    printf("gs path: %s (%d failed)\n", failures ? "FAIL" : "ok", failures);
    return failures ? 1 : 0;
}
