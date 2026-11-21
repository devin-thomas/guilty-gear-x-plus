/* The Draw Layer stream (dl_stream.h). */
#include "dl_stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The GS environment GraphInit sets once at boot (graph.c, decoded in
   DRAW-INVENTORY.md) and MakeFullBuffer every frame. The renderer starts
   from it, so a Core whose boot it never saw (a state loaded from PCSX2)
   draws the same; the game's own writes then update it. */
#define INIT_XYOFFSET (0x6C00ull | 0x7900ull << 32)
#define INIT_SCISSOR (639ull << 16 | 447ull << 48)
#define INIT_TEXA 0x8000000000ull
#define INIT_ALPHA 0x44ull
#define INIT_FRAME 0xA008Cull       /* FBP 0x8C (block 0x1180), FBW 10, CT32 */
#define INIT_COLCLAMP 1ull
#define INIT_TEST 0x30000ull        /* ZTE on, ZTST ALWAYS (sceGsSetDefDrawEnv, ztest 0) */

/* The privileged registers GS_runtime_preset writes with no screen offset
   (graph.c): both circuits on, MMOD 1, ALP 0x7F, the display buffer at 0,
   and circuit 2 one line below circuit 1. Offsets from 0x12000000. */
#define PRIV_BYTES 0xF0
#define PRIV_PMODE 0x00
#define PRIV_SMODE2 0x20
#define PRIV_DISPFB(i) (0x70 + 0x20 * (i))
#define PRIV_DISPLAY(i) (0x80 + 0x20 * (i))
#define PRIV_BGCOLOR 0xE0
#define G_DISPLAY(dx, dy, magh, magv, dw, dh) \
    ((uint64_t)(dx) | (uint64_t)(dy) << 12 | (uint64_t)(magh) << 23 | (uint64_t)(magv) << 27 | \
     (uint64_t)(dw) << 32 | (uint64_t)(dh) << 44)
#define INIT_PMODE 0x7F27ull
#define INIT_DISPFB 0x1400ull
/* the output's origin: DISPLAY's DX and DY for the game's default position,
   and the video clocks in one output pixel (2560 a line, NTSC) */
#define ORIGIN_DX 636
#define ORIGIN_DY 50
#define VCK_PER_PIXEL 4

#define TEX0_LOW 0x1FFFFFFFFFull   /* TBP0 .. TFX: what PXF_SendTexture sets */
#define MAX_TEX_LOG2 10

typedef struct {
    uint16_t x, y;
    uint16_t u, v;
    uint8_t r, g, b, a;
} GsVtx;

typedef struct {
    int active;
    int psm;
    uint32_t bp, bw, x, y, w, h;
    size_t need, have;
    size_t start;            /* where its data begins in the arena */
} Transfer;

/* A host-to-local transfer seen this frame, kept so a texture drawn right
   after its upload is read from the upload's linear data. */
typedef struct {
    int psm;
    uint32_t bp, bw, x, y, w, h;
    size_t off, size;        /* in the arena */
    uint64_t hash;
    uint32_t id;
    int clobbered;           /* a later write covered one of its blocks */
} Upload;

/* A frame's uploads and the data they carried. Two are kept, this frame's and
   the last, since a block an upload still owns is written into local memory
   only when something reads it, or when its bank is about to be reused. */
typedef struct {
    Upload *up;
    uint32_t nup, cap_up;
    uint8_t *arena;
    size_t arena_n, arena_cap;
    uint32_t first;          /* the id of its first upload */
} Bank;

typedef struct {
    uint32_t nloop, nreg, ri;
    int flg;
    uint8_t regs[16];
    int half;                /* REGLIST: a register word is pending in the qword's top half */
} GifState;

/* What the last primitive's texture resolved to. */
typedef struct {
    uint64_t tex0, texa;
    uint32_t epoch;
    int32_t tex;
    uint8_t kind, tw, th, max_alpha;
    uint16_t w, h;
    uint32_t variant;        /* the substitution's variant (DlVariant) */
} TexRes;

struct DlStream {
    GsMem mem;
    GsClut clut;
    DlTexCache tex;
    DlStats st;
    int list;

    /* GS registers (context 1 and 2 where the GS has two) */
    uint64_t prim, rgbaq, uv, prmodecont, prmode, texa;
    uint64_t tex0[2], clamp[2], xyoffset[2], scissor[2], alpha[2];
    uint64_t frame[2], test[2], fba[2], colclamp, pabe;
    uint64_t bitbltbuf, trxpos, trxreg;
    uint8_t priv[PRIV_BYTES];      /* the privileged registers, as last read */
    DlSurface surf[DL_MAX_SURFACES];
    uint8_t *surf_init[DL_MAX_SURFACES];
    uint32_t nsurf;
    uint32_t cbp[2];
    GsVtx q[3];
    int nq;
    GifState gif;
    Transfer tr;

    /* uploads: this frame's and the last frame's */
    Bank bank[2];
    int cur;
    uint32_t upload_id;
    uint32_t epoch;          /* local memory writes so far */
    uint32_t clut_epoch;     /* CLUT buffer loads so far */
    uint64_t row_key;        /* what the last CLUT row was read for */
    int32_t row;
    uint8_t row_alpha;
    uint32_t block_writer[16384];   /* the upload that last wrote each block */
    uint32_t block_pos[16384];      /* where in that upload: block column | row << 16 */
    uint8_t block_pending[16384];   /* local memory doesn't hold its writer's data yet */
    uint32_t bp_upload[16384];   /* 1 + the latest upload at that base this frame, or 0 */

    DlFrame frames[2];
    int building;
    int in_frame, have_frame;
    uint32_t serial;
    TexRes res;
    uint8_t *scratch;        /* texels read from local memory */
    DlTap tap;
    void *tap_ctx;
    DlSubst subst;
    void *subst_ctx;
    uint64_t subst_salt;     /* in every upload texture's key: a change of
                                substitution misses the old entries */
    DlVariant variant;
    void *variant_ctx;
};

void dl_stream_set_tap(DlStream *s, DlTap tap, void *ctx)
{
    s->tap = tap;
    s->tap_ctx = ctx;
}

void dl_stream_set_variant(DlStream *s, DlVariant fn, void *ctx)
{
    s->variant = fn;
    s->variant_ctx = ctx;
    s->res.tex = -1;
}

void dl_stream_set_subst(DlStream *s, DlSubst fn, void *ctx)
{
    s->subst = fn;
    s->subst_ctx = ctx;
    s->subst_salt += 0x9E3779B97F4A7C15ull;
    s->res.tex = -1;
}

static void grow(void **p, uint32_t *cap, uint32_t need, size_t elem)
{
    if (need <= *cap) {
        return;
    }
    while (*cap < need) {
        *cap = *cap ? *cap * 2 : 1024;
    }
    *p = realloc(*p, *cap * elem);
    if (!*p) {
        abort();
    }
}

static void put64(uint8_t *p, uint64_t v)
{
    memcpy(p, &v, 8);
}

DlStream *dl_stream_new(void)
{
    DlStream *s = calloc(1, sizeof(DlStream));
    int i;

    if (!s || gs_mem_init(&s->mem) != 0) {
        free(s);
        return NULL;
    }
    dl_tex_init(&s->tex);
    s->list = DL_LIST_OTHER;
    s->bank[0].first = s->bank[1].first = 1;
    s->row = -1;
    s->res.tex = -1;
    s->prmodecont = 1;
    s->texa = INIT_TEXA;
    s->colclamp = INIT_COLCLAMP;
    for (i = 0; i < 2; i++) {
        s->xyoffset[i] = INIT_XYOFFSET;
        s->scissor[i] = INIT_SCISSOR;
        s->alpha[i] = INIT_ALPHA;
        s->frame[i] = INIT_FRAME;
        s->test[i] = INIT_TEST;
        put64(s->priv + PRIV_DISPFB(i), INIT_DISPFB);
        put64(s->priv + PRIV_DISPLAY(i), G_DISPLAY(ORIGIN_DX, ORIGIN_DY + i, 3, 0, 2559, 447));
    }
    put64(s->priv + PRIV_PMODE, INIT_PMODE);
    s->scratch = malloc((size_t)4 << (2 * MAX_TEX_LOG2));
    if (!s->scratch) {
        abort();
    }
    return s;
}

void dl_stream_free(DlStream *s)
{
    int i;

    if (!s) {
        return;
    }
    for (i = 0; i < 2; i++) {
        free(s->frames[i].v);
        free(s->frames[i].b);
    }
    for (i = 0; i < 2; i++) {
        free(s->bank[i].up);
        free(s->bank[i].arena);
    }
    for (i = 0; i < DL_MAX_SURFACES; i++) {
        free(s->surf_init[i]);
    }
    free(s->scratch);
    dl_tex_free(&s->tex);
    gs_mem_free(&s->mem);
    free(s);
}

static void write_pending(DlStream *s, uint32_t b);

GsMem *dl_stream_gs_mem(DlStream *s)
{
    uint32_t i, k;

    /* the caller sees all of it and may write anything: every pending block
       is written first, and no upload is known whole afterwards */
    for (i = 0; i < 16384; i++) {
        if (s->block_pending[i]) {
            write_pending(s, i);
        }
    }
    for (k = 0; k < 2; k++) {
        for (i = 0; i < s->bank[k].nup; i++) {
            s->bank[k].up[i].clobbered = 1;
        }
    }
    s->epoch++;
    return &s->mem;
}

DlTexCache *dl_stream_textures(DlStream *s)
{
    return &s->tex;
}

const DlStats *dl_stream_stats(const DlStream *s)
{
    return &s->st;
}

const DlFrame *dl_stream_frame(DlStream *s)
{
    return s->have_frame ? &s->frames[s->building ^ 1] : NULL;
}

/* ---- framebuffers ------------------------------------------------------ */

static void ensure(DlStream *s, int psm, uint32_t bp, uint32_t bufw, uint32_t x0, uint32_t y0, uint32_t w, uint32_t h);

/* The blocks a surface covers: bw pages across, DL_FB_H / 32 pages down. */
static uint32_t surface_end(const DlSurface *f)
{
    return f->bp + (uint32_t)f->bw * (DL_FB_H / 32) * 32;
}

/* The surface for a framebuffer at block bp, or -1 if the renderer can't
   hold it: anything but a 640-wide CT32 or CT24 buffer, or one overlapping
   another surface. A new one is added when `add`. */
static int surface_of(DlStream *s, uint32_t bp, uint32_t bw, int psm, int add)
{
    DlSurface n;
    uint32_t i;

    for (i = 0; i < s->nsurf; i++) {
        if (s->surf[i].bp == bp) {
            return s->surf[i].bw == bw && (psm == GS_PSMCT32 || psm == GS_PSMCT24) ? (int)i : -1;
        }
    }
    if (!add || bw != DL_FB_W / 64 || (psm != GS_PSMCT32 && psm != GS_PSMCT24) || s->nsurf == DL_MAX_SURFACES) {
        return -1;
    }
    memset(&n, 0, sizeof(n));
    n.bp = bp;
    n.bw = (uint8_t)bw;
    n.psm = (uint8_t)psm;
    for (i = 0; i < s->nsurf; i++) {
        if (bp < surface_end(&s->surf[i]) && s->surf[i].bp < surface_end(&n)) {
            return -1;
        }
    }
    /* its first contents: what local memory holds there */
    s->surf_init[s->nsurf] = malloc((size_t)DL_FB_W * DL_FB_H * 4);
    if (!s->surf_init[s->nsurf]) {
        abort();
    }
    ensure(s, psm, bp, bw, 0, 0, DL_FB_W, DL_FB_H);
    gs_read_rect(&s->mem, psm, bp, bw, 0, 0, DL_FB_W, DL_FB_H, s->surf_init[s->nsurf]);
    for (i = 0; i < DL_FB_W * DL_FB_H; i++) {
        uint8_t *a = s->surf_init[s->nsurf] + 4 * i + 3;

        *a = psm == GS_PSMCT24 ? 255 : (uint8_t)(*a >= 0x80 ? 255 : (*a * 255 + 64) / 128);
    }
    n.init = s->surf_init[s->nsurf];
    s->surf[s->nsurf] = n;
    return (int)s->nsurf++;
}

/* 1 if blocks [lo, hi) overlap a surface's. */
static int in_surface(const DlStream *s, uint32_t lo, uint32_t hi)
{
    uint32_t i;

    for (i = 0; i < s->nsurf; i++) {
        if (lo < surface_end(&s->surf[i]) && s->surf[i].bp < hi) {
            return 1;
        }
    }
    return 0;
}

/* The blocks of a w x h rectangle at (x, y) of a buffer, by whole pages. */
static void rect_blocks(int psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                        uint32_t *lo, uint32_t *hi)
{
    *lo = gs_block_of(psm, bp, bw, x, y) & ~31u;
    *hi = (gs_block_of(psm, bp, bw, x + w - 1, y + h - 1) | 31u) + 1;
}

/* ---- the display ------------------------------------------------------- */

static uint64_t priv64(const DlStream *s, uint32_t off)
{
    uint64_t v;

    memcpy(&v, s->priv + off, 8);
    return v;
}

static int floor_div(int a, int b)
{
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

/* PMODE, SMODE2, DISPFB1/2, DISPLAY1/2 and BGCOLOR as the frame's display:
   where each read circuit's picture lands on the 640x448 output, which
   surface it reads from where, and how the merge circuit mixes them. A
   circuit reading a framebuffer the renderer doesn't hold is left off. */
static void resolve_display(DlStream *s, DlFrame *f)
{
    DlDisplay *d = &f->display;
    uint64_t pmode = priv64(s, PRIV_PMODE), smode2 = priv64(s, PRIV_SMODE2);
    int i;

    memset(d, 0, sizeof(*d));
    d->mmod = (uint8_t)((pmode >> 5) & 1);
    d->slbg = (uint8_t)((pmode >> 7) & 1);
    d->alp = (uint8_t)(pmode >> 8);
    d->bgcolor = (uint32_t)priv64(s, PRIV_BGCOLOR) & 0xFFFFFF;
    for (i = 0; i < 2; i++) {
        DlCircuit *c = &d->c[i];
        uint64_t fb = priv64(s, PRIV_DISPFB(i)), dp = priv64(s, PRIV_DISPLAY(i));
        uint32_t fbw = (uint32_t)(fb >> 9) & 0x3F, dw = (uint32_t)(dp >> 32) & 0xFFF;
        uint32_t dh = (uint32_t)(dp >> 44) & 0x7FF;
        int id;

        if (!((pmode >> i) & 1) || (fbw == 0 && dw == 0 && dh == 0)) {
            continue;   /* off, or on but never set up (as PCSX2 treats it) */
        }
        id = surface_of(s, ((uint32_t)fb & 0x1FF) * 32, fbw, (int)((fb >> 15) & 0x1F), 1);
        if (id < 0) {
            s->st.unsupported++;
            s->st.gs_unsupported++;
            continue;
        }
        c->on = 1;
        c->surface = (uint8_t)id;
        c->dbx = (uint16_t)((fb >> 32) & 0x7FF);
        c->dby = (uint16_t)((fb >> 43) & 0x7FF);
        c->x = (int16_t)floor_div((int)(dp & 0xFFF) - ORIGIN_DX, VCK_PER_PIXEL);
        c->y = (int16_t)((int)((dp >> 12) & 0x7FF) - ORIGIN_DY);
        c->w = (uint16_t)((dw + 1) / VCK_PER_PIXEL);
        c->h = (uint16_t)(dh + 1);
        c->magh = (uint8_t)((dp >> 23) & 0xF);
        c->magv = (uint8_t)((dp >> 27) & 3);
        c->half = (uint8_t)((smode2 & 3) == 3);
    }
    memcpy(f->surf, s->surf, sizeof(s->surf));
    f->nsurf = s->nsurf;
}

void dl_stream_priv(DlStream *s, const uint8_t *regs)
{
    memcpy(s->priv, regs, PRIV_BYTES);
    if (s->have_frame) {
        resolve_display(s, &s->frames[s->building ^ 1]);
    }
}

/* ---- frames ------------------------------------------------------------ */

static void retire(DlStream *s, const Bank *bk);

static void frame_begin(DlStream *s)
{
    DlFrame *f = &s->frames[s->building];
    Bank *bk;

    f->nv = f->nb = f->nclut = f->nblit = 0;
    s->in_frame = 1;
    /* the bank two frames old is reused: what its uploads still own goes
       into local memory first */
    s->cur ^= 1;
    bk = &s->bank[s->cur];
    retire(s, bk);
    bk->nup = 0;
    bk->arena_n = 0;
    bk->first = s->upload_id + 1;
    memset(s->bp_upload, 0, sizeof(s->bp_upload));
    s->res.tex = -1;
    s->res.epoch = s->epoch - 1;
    s->row = -1;
}

static void frame_complete(DlStream *s)
{
    DlFrame *f = &s->frames[s->building];

    if (!s->in_frame) {
        return;
    }
    resolve_display(s, f);
    s->st.displays++;
    if (f->display.c[0].on && f->display.c[1].on && !f->display.slbg) {
        s->st.displays_merged++;
    }
    f->serial = ++s->serial;
    s->building ^= 1;
    s->have_frame = 1;
    s->in_frame = 0;
    s->st.frames++;
}

/* ---- local memory ------------------------------------------------------ */

static void block_dims(int psm, uint32_t *bw, uint32_t *bh)
{
    switch (psm) {
    case GS_PSMCT16:
        *bw = 16, *bh = 8;
        break;
    case GS_PSMT8:
        *bw = 16, *bh = 16;
        break;
    case GS_PSMT4:
        *bw = 32, *bh = 16;
        break;
    default:
        *bw = 8, *bh = 8;
        break;
    }
}

/* The upload with this id, if its bank still holds it, and that bank's data. */
static Upload *find_upload(DlStream *s, uint32_t id, const uint8_t **arena)
{
    int k;

    for (k = 0; k < 2; k++) {
        Bank *bk = &s->bank[k];

        if (id >= bk->first && id < bk->first + bk->nup) {
            *arena = bk->arena;
            return &bk->up[id - bk->first];
        }
    }
    return NULL;
}

/* Write block b's pixels from the upload that owns it into local memory. */
static void write_pending(DlStream *s, uint32_t b)
{
    const uint8_t *arena;
    const Upload *u = find_upload(s, s->block_writer[b], &arena);
    uint32_t bw, bh, px, py, w, h;

    s->block_pending[b] = 0;
    if (!u) {
        return;
    }
    block_dims(u->psm, &bw, &bh);
    px = u->x + (s->block_pos[b] & 0xFFFF) * bw;
    py = u->y + (s->block_pos[b] >> 16) * bh;
    w = u->x + u->w - px < bw ? u->x + u->w - px : bw;
    h = u->y + u->h - py < bh ? u->y + u->h - py : bh;
    gs_write_sub(&s->mem, u->psm, u->bp, u->bw, px, py, w, h, arena + u->off, u->w, px - u->x, py - u->y);
    s->st.blocks_written++;
}

/* Before local memory is read over a rectangle: write its pending blocks. */
static void ensure(DlStream *s, int psm, uint32_t bp, uint32_t bufw, uint32_t x0, uint32_t y0, uint32_t w, uint32_t h)
{
    uint32_t bw, bh, x, y;

    block_dims(psm, &bw, &bh);
    for (y = y0 & ~(bh - 1); y < y0 + h; y += bh) {
        for (x = x0 & ~(bw - 1); x < x0 + w; x += bw) {
            uint32_t b = gs_block_of(psm, bp, bufw, x, y);

            if (s->block_pending[b]) {
                write_pending(s, b);
            }
        }
    }
}

static void retire(DlStream *s, const Bank *bk)
{
    uint32_t b;

    if (!bk->nup) {
        return;
    }
    for (b = 0; b < 16384; b++) {
        if (s->block_pending[b] && s->block_writer[b] >= bk->first && s->block_writer[b] < bk->first + bk->nup) {
            write_pending(s, b);
        }
    }
}

/* Make upload u the writer of the blocks it covers (pending if lazy: its
   data stays in the bank until read). An earlier upload that loses a block
   is no longer whole; one that keeps part of a block it shares with u has
   that block written first. */
static void claim_blocks(DlStream *s, Upload *u, int lazy)
{
    uint32_t bw, bh, x, y, x0, y0;

    block_dims(u->psm, &bw, &bh);
    x0 = u->x & ~(bw - 1);
    y0 = u->y & ~(bh - 1);
    for (y = y0; y < u->y + u->h; y += bh) {
        for (x = x0; x < u->x + u->w; x += bw) {
            uint32_t b = gs_block_of(u->psm, u->bp, u->bw, x, y), prev = s->block_writer[b];
            int whole = x >= u->x && y >= u->y && x + bw <= u->x + u->w && y + bh <= u->y + u->h;
            const uint8_t *arena;
            Upload *p;

            if (s->block_pending[b] && !whole) {
                write_pending(s, b);
            }
            if (prev && prev != u->id && (p = find_upload(s, prev, &arena)) != NULL) {
                p->clobbered = 1;
            }
            s->block_writer[b] = u->id;
            s->block_pos[b] = (x - x0) / bw | ((y - y0) / bh) << 16;
            s->block_pending[b] = (uint8_t)lazy;
        }
    }
}

static void transfer_done(DlStream *s)
{
    Transfer *t = &s->tr;
    Upload *u;
    Bank *bk = &s->bank[s->cur];
    uint32_t bw, bh;
    int lazy;

    t->active = 0;
    grow((void **)&bk->up, &bk->cap_up, bk->nup + 1, sizeof(Upload));
    u = &bk->up[bk->nup++];
    u->psm = t->psm;
    u->bp = t->bp;
    u->bw = t->bw;
    u->x = t->x;
    u->y = t->y;
    u->w = t->w;
    u->h = t->h;
    u->off = t->start;
    u->size = t->need;
    u->hash = dl_hash64(bk->arena + t->start, t->need, (uint64_t)t->psm << 32 | t->w << 16 | t->h);
    u->id = ++s->upload_id;
    u->clobbered = 0;
    /* an upload aligned to its format's blocks is written into local
       memory only if something reads it there: most textures are read from
       their upload's own data, and the swizzle costs */
    block_dims(t->psm, &bw, &bh);
    lazy = t->x % bw == 0 && t->y % bh == 0;
    claim_blocks(s, u, lazy);
    if (!lazy) {
        gs_write_rect(&s->mem, t->psm, t->bp, t->bw, t->x, t->y, t->w, t->h, bk->arena + t->start);
    }
    s->epoch++;
    s->bp_upload[t->bp & 0x3FFF] = bk->nup;
    s->st.uploads++;
    s->st.upload_bytes += t->need;
}

/* Room for t's data at the end of this frame's arena. */
static void transfer_room(DlStream *s, Transfer *t)
{
    Bank *bk = &s->bank[s->cur];

    t->have = 0;
    t->start = bk->arena_n;
    if (bk->arena_n + t->need + 16 > bk->arena_cap) {
        while (bk->arena_n + t->need + 16 > bk->arena_cap) {
            bk->arena_cap = bk->arena_cap ? bk->arena_cap * 2 : (size_t)8 << 20;
        }
        bk->arena = realloc(bk->arena, bk->arena_cap);
        if (!bk->arena) {
            abort();
        }
    }
}

/* Count a failure on the GS Path's lists apart from the Draw Layer's. */
static void unsupported(DlStream *s)
{
    s->st.unsupported++;
    if (s->list != DL_LIST_DRAW) {
        s->st.gs_unsupported++;
    }
}

/* How much of len from pos fits below size. */
static uint32_t clip_len(uint32_t pos, uint32_t len, uint32_t size)
{
    return pos >= size ? 0 : len < size - pos ? len : size - pos;
}

/* Pixels read with gs_read_rect packed as IMAGE data packs them. */
static void pack_pixels(int psm, const uint8_t *raw, uint8_t *out, size_t n)
{
    size_t i;

    if (psm == GS_PSMCT24) {
        for (i = 0; i < n; i++) {
            memcpy(out + 3 * i, raw + 4 * i, 3);
        }
    } else if (psm == GS_PSMT4) {
        memset(out, 0, (n + 1) / 2);
        for (i = 0; i < n; i++) {
            out[i >> 1] |= (uint8_t)((raw[i] & 0xF) << ((i & 1) * 4));
        }
    } else {
        memcpy(out, raw, n * (size_t)gs_psm_bits(psm) / 8);
    }
}

/* A local-to-local transfer (TRXDIR 2). Between framebuffers it is a GPU
   copy, kept in the frame in its place: GraphConv's blit of the draw buffer
   to the display buffer, with the user's screen offset in its source
   position and size. Within plain local memory it is done on the CPU model,
   as an upload of what the source holds. Between the two, or between
   formats, it is not supported. */
static void copy_rect(DlStream *s)
{
    uint64_t b = s->bitbltbuf;
    uint32_t sbp = (uint32_t)b & 0x3FFF, sbw = (uint32_t)(b >> 16) & 0x3F;
    uint32_t dbp = (uint32_t)(b >> 32) & 0x3FFF, dbw = (uint32_t)(b >> 48) & 0x3F;
    int spsm = (int)((b >> 24) & 0x3F), dpsm = (int)((b >> 56) & 0x3F);
    uint32_t sx = (uint32_t)s->trxpos & 0x7FF, sy = (uint32_t)(s->trxpos >> 16) & 0x7FF;
    uint32_t dx = (uint32_t)(s->trxpos >> 32) & 0x7FF, dy = (uint32_t)(s->trxpos >> 48) & 0x7FF;
    uint32_t w = (uint32_t)s->trxreg & 0xFFF, h = (uint32_t)(s->trxreg >> 32) & 0xFFF, lo, hi;
    int src = surface_of(s, sbp, sbw, spsm, 0);

    if (w == 0 || h == 0) {
        return;
    }
    if (src >= 0) {
        int dst = surface_of(s, dbp, dbw, dpsm, 1);
        DlFrame *f;
        DlBlit *k;
        DlBatch *bt;

        if (dst < 0 || spsm != dpsm) {
            unsupported(s);
            return;
        }
        if (!s->in_frame) {
            frame_begin(s);
        }
        f = &s->frames[s->building];
        if (f->nblit == DL_MAX_BLITS) {
            unsupported(s);
            return;
        }
        /* the surfaces are 640x448: what lies past them isn't held */
        w = clip_len(dx, clip_len(sx, w, DL_FB_W), DL_FB_W);
        h = clip_len(dy, clip_len(sy, h, DL_FB_H), DL_FB_H);
        if (w && h) {
            k = &f->blit[f->nblit];
            k->src = (uint8_t)src, k->dst = (uint8_t)dst;
            k->sx = (uint16_t)sx, k->sy = (uint16_t)sy, k->dx = (uint16_t)dx, k->dy = (uint16_t)dy;
            k->w = (uint16_t)w, k->h = (uint16_t)h;
            grow((void **)&f->b, &f->cap_b, f->nb + 1, sizeof(DlBatch));
            bt = &f->b[f->nb++];
            memset(bt, 0, sizeof(*bt));
            bt->first = f->nv;
            bt->tex = bt->clut = -1;
            bt->surface = (uint8_t)dst;
            bt->blit = (int16_t)f->nblit++;
            s->st.blits++;
        }
        return;
    }
    rect_blocks(dpsm, dbp, dbw, dx, dy, w, h, &lo, &hi);
    if (spsm != dpsm || !gs_psm_supported(spsm) || in_surface(s, lo, hi)) {
        unsupported(s);
        return;
    }
    {
        Transfer *t = &s->tr;
        size_t n = (size_t)w * h;
        uint8_t *raw = malloc(n * 4);

        if (!raw) {
            abort();
        }
        ensure(s, spsm, sbp, sbw, sx, sy, w, h);
        gs_read_rect(&s->mem, spsm, sbp, sbw, sx, sy, w, h, raw);
        t->psm = dpsm;
        t->bp = dbp, t->bw = dbw, t->x = dx, t->y = dy, t->w = w, t->h = h;
        t->need = (n * (size_t)gs_psm_bits(dpsm) + 7) / 8;
        transfer_room(s, t);
        pack_pixels(spsm, raw, s->bank[s->cur].arena + t->start, n);
        free(raw);
        t->active = 1;
        t->have = t->need;
        s->bank[s->cur].arena_n = t->start + ((t->need + 15) & ~(size_t)15);
        transfer_done(s);
        s->st.local_copies++;
    }
}

/* On list 3 the transfer is GraphConv's, which completes the frame. One from
   a buffer nothing has drawn into yet (a GS dump that starts mid-frame)
   shows what local memory held. */
static void local_copy(DlStream *s)
{
    copy_rect(s);
    if (s->list == DL_LIST_TAIL || s->list == DL_LIST_REPLAY) {
        if (!s->in_frame) {
            frame_begin(s);
        }
        frame_complete(s);
    }
}

static void transfer_start(DlStream *s, int dir)
{
    Transfer *t = &s->tr;
    int psm = (int)((s->bitbltbuf >> 56) & 0x3F);
    uint32_t lo, hi;

    t->active = 0;
    if (dir == 2) {
        local_copy(s);
        return;
    }
    if (dir != 0) {
        /* local to host: only the debug frame dumps, through
           sceGsExecStoreImage, which is a shim; nothing reads it here */
        return;
    }
    if (!gs_psm_supported(psm)) {
        unsupported(s);
        return;
    }
    t->psm = psm;
    t->bp = (uint32_t)(s->bitbltbuf >> 32) & 0x3FFF;
    t->bw = (uint32_t)(s->bitbltbuf >> 48) & 0x3F;
    t->x = (uint32_t)(s->trxpos >> 32) & 0x7FF;
    t->y = (uint32_t)(s->trxpos >> 48) & 0x7FF;
    t->w = (uint32_t)s->trxreg & 0xFFF;
    t->h = (uint32_t)(s->trxreg >> 32) & 0xFFF;
    t->need = ((size_t)t->w * t->h * (size_t)gs_psm_bits(psm) + 7) / 8;
    if (t->need == 0) {
        return;
    }
    rect_blocks(psm, t->bp, t->bw, t->x, t->y, t->w, t->h, &lo, &hi);
    if (in_surface(s, lo, hi)) {
        /* an image into a framebuffer would have to reach the GPU's copy */
        s->st.surface_writes++;
        unsupported(s);
    }
    transfer_room(s, t);
    t->active = 1;
}

/* A host-to-local transfer whose data stopped short of its size, ended by
   the next register write. The GS writes IMAGE data into local memory as it
   arrives, so what came is written and the pixels that never came keep what
   local memory held. The PXF manager sends a T8 picture as w * h / 16
   qwords, rounded down: a 300x350 one (Testament's) is 8 texels short, and
   waiting for them dropped the whole picture. */
static void transfer_cut(DlStream *s)
{
    Transfer *t = &s->tr;
    size_t n = (size_t)t->w * t->h;
    uint8_t *raw = malloc(n * 4), *packed = malloc(t->need);

    if (!raw || !packed) {
        abort();
    }
    ensure(s, t->psm, t->bp, t->bw, t->x, t->y, t->w, t->h);
    gs_read_rect(&s->mem, t->psm, t->bp, t->bw, t->x, t->y, t->w, t->h, raw);
    pack_pixels(t->psm, raw, packed, n);
    memcpy(s->bank[s->cur].arena + t->start + t->have, packed + t->have, t->need - t->have);
    free(raw);
    free(packed);
    t->have = t->need;
    s->bank[s->cur].arena_n = t->start + ((t->need + 15) & ~(size_t)15);
    transfer_done(s);
    s->st.uploads_short++;
}

/* qwc qwords of IMAGE data for the transfer in progress */
static void image_data(DlStream *s, const uint8_t *q, uint32_t qwc)
{
    Transfer *t = &s->tr;
    size_t n;

    if (!t->active) {
        return;
    }
    n = t->need - t->have < (size_t)qwc * 16 ? t->need - t->have : (size_t)qwc * 16;
    memcpy(s->bank[s->cur].arena + t->start + t->have, q, n);
    t->have += n;
    if (t->have == t->need) {
        s->bank[s->cur].arena_n = t->start + ((t->need + 15) & ~(size_t)15);
        transfer_done(s);
    }
}

/* ---- textures ---------------------------------------------------------- */

static uint8_t texel_alpha_max(const uint8_t *rgba, size_t n)
{
    uint8_t m = 0;
    size_t i;

    for (i = 0; i < n; i++) {
        if (rgba[4 * i + 3] > m) {
            m = rgba[4 * i + 3];
        }
    }
    return m;
}

/* Decode w x h texels of psm, raw (as gs_read_rect gives them, or packed
   as upload data when `packed`), into the cache entry's pixels. */
static void decode(DlStream *s, DlTex *t, int psm, const uint8_t *src, uint32_t stride, int packed)
{
    uint32_t ta0 = (uint32_t)(s->texa & 0xFF), ta1 = (uint32_t)((s->texa >> 32) & 0xFF);
    uint32_t x, y;

    for (y = 0; y < t->h; y++) {
        for (x = 0; x < t->w; x++) {
            size_t i = (size_t)y * stride + x, o = (size_t)y * t->w + x;

            switch (psm) {
            case GS_PSMCT32:
                memcpy(t->pixels + 4 * o, src + 4 * i, 4);
                break;
            case GS_PSMCT24:
                if (packed) {
                    memcpy(t->pixels + 4 * o, src + 3 * i, 3);
                } else {
                    memcpy(t->pixels + 4 * o, src + 4 * i, 3);
                }
                t->pixels[4 * o + 3] = (uint8_t)ta0;
                break;
            case GS_PSMCT16: {
                uint16_t c;
                uint32_t v;

                memcpy(&c, src + 2 * i, 2);
                v = gs_ct16_rgba(c, ta0, ta1);
                memcpy(t->pixels + 4 * o, &v, 4);
                break;
            }
            case GS_PSMT8:
                t->pixels[o] = src[i];
                break;
            case GS_PSMT4:
                t->pixels[o] = packed ? (uint8_t)((src[i >> 1] >> ((i & 1) * 4)) & 0xF) : src[i];
                break;
            }
        }
    }
    if (t->kind == DL_TEX_RGBA) {
        t->max_alpha = texel_alpha_max(t->pixels, (size_t)t->w * t->h);
    }
}

static int indexed(int psm)
{
    return psm == GS_PSMT8 || psm == GS_PSMT4;
}

/* The cache entry for the texture TEX0 describes, decoded now if new: the
   texels of the upload that wrote it this frame when that upload is still
   intact (the usual case: PXF_SendTexture uploads right before the draw),
   otherwise what local memory holds over its whole wrap size. */
static int resolve_texture(DlStream *s, uint64_t tex0, int x, int y, TexRes *r)
{
    uint32_t tbp = (uint32_t)tex0 & 0x3FFF, tbw = (uint32_t)(tex0 >> 14) & 0x3F;
    int psm = (int)((tex0 >> 20) & 0x3F);
    uint32_t tw = (uint32_t)(tex0 >> 26) & 0xF, th = (uint32_t)(tex0 >> 30) & 0xF;
    uint32_t W = 1u << (tw > MAX_TEX_LOG2 ? MAX_TEX_LOG2 : tw), H = 1u << (th > MAX_TEX_LOG2 ? MAX_TEX_LOG2 : th);
    uint32_t idx = s->bp_upload[tbp];
    const Upload *u = idx ? &s->bank[s->cur].up[idx - 1] : NULL;
    uint64_t key, texa = psm == GS_PSMCT16 || psm == GS_PSMCT24 ? s->texa : 0;
    uint32_t lo, hi, variant = u && s->variant ? s->variant(s->variant_ctx, u->hash, x, y) : 0;
    int32_t id;
    DlTex *t;

    if ((tex0 & TEX0_LOW) == (s->res.tex0 & TEX0_LOW) && s->res.epoch == s->epoch && s->res.texa == texa &&
        s->res.tex >= 0 && s->res.variant == variant) {
        *r = s->res;
        return 0;
    }
    if (!gs_psm_supported(psm)) {
        return -1;
    }
    rect_blocks(psm, tbp, tbw, 0, 0, W, H, &lo, &hi);
    if (in_surface(s, lo, hi)) {
        /* the GPU's pixels: local memory holds only what was there before */
        s->st.surface_reads++;
        unsupported(s);
    }
    r->tex0 = tex0;
    r->texa = texa;
    r->variant = variant;
    r->epoch = s->epoch;
    r->kind = indexed(psm) ? DL_TEX_INDEX : DL_TEX_RGBA;
    r->tw = (uint8_t)tw;
    r->th = (uint8_t)th;
    if (u && u->psm == psm && u->bw == tbw && u->x == 0 && u->y == 0 && !u->clobbered) {
        uint32_t w = u->w < W ? u->w : W, h = u->h < H ? u->h : H;

        key = dl_hash64(&u->hash, 8, (uint64_t)psm << 48 | (uint64_t)w << 32 | h << 16 | tbw) ^ texa ^ s->subst_salt ^
              variant * 0xC2B2AE3D27D4EB4Full;
        id = dl_tex_find(&s->tex, key);
        if (id < 0) {
            const uint8_t *orig = s->bank[s->cur].arena + u->off;
            const uint8_t *sub = s->subst ? s->subst(s->subst_ctx, u->hash, psm, u->w, u->h, orig, variant) : NULL;

            id = dl_tex_add(&s->tex, key, (uint16_t)w, (uint16_t)h, r->kind);
            decode(s, &s->tex.e[id], psm, sub ? sub : orig, u->w, 1);
            s->st.tex_new++;
            s->st.tex_new_texels += (uint64_t)w * h;
        }
        s->st.tex_from_upload++;
    } else {
        size_t bytes = (size_t)W * H * (psm == GS_PSMCT16 ? 2 : indexed(psm) ? 1 : 4);

        ensure(s, psm, tbp, tbw, 0, 0, W, H);
        gs_read_rect(&s->mem, psm, tbp, tbw, 0, 0, W, H, s->scratch);
        key = dl_hash64(s->scratch, bytes, (uint64_t)psm << 48 | (uint64_t)W << 32 | H << 16 | 0xFFFF) ^ texa;
        id = dl_tex_find(&s->tex, key);
        if (id < 0) {
            id = dl_tex_add(&s->tex, key, (uint16_t)W, (uint16_t)H, r->kind);
            decode(s, &s->tex.e[id], psm, s->scratch, W, 0);
            s->st.tex_new++;
            s->st.tex_new_texels += (uint64_t)W * H;
        }
        s->st.tex_from_memory++;
    }
    t = &s->tex.e[id];
    t->last_used = s->serial + 1;
    r->tex = id;
    r->w = t->w;
    r->h = t->h;
    r->max_alpha = t->max_alpha;
    s->res = *r;
    return 0;
}

/* The CLUT row of the frame's atlas holding what an indexed texture reads
   from the CLUT buffer now. */
static int32_t clut_row(DlStream *s, uint64_t tex0, uint8_t *max_alpha)
{
    DlFrame *f = &s->frames[s->building];
    int psm = (int)((tex0 >> 20) & 0x3F), cpsm = (int)((tex0 >> 51) & 0xF);
    uint32_t csa = (uint32_t)(tex0 >> 56) & 0x1F, row[256], i, n = psm == GS_PSMT8 ? 256 : 16;
    uint64_t key, want = (tex0 >> 51 | (uint64_t)psm << 13) ^ (uint64_t)s->clut_epoch << 32 ^ s->texa * 31;

    if (s->row >= 0 && s->row_key == want) {
        *max_alpha = s->row_alpha;
        return s->row;
    }
    gs_clut_read(&s->clut, psm, cpsm, csa, s->texa, row);
    key = dl_hash64(row, sizeof(row), 0x434C5554);
    *max_alpha = 0;
    for (i = 0; i < n; i++) {
        if ((row[i] >> 24) > *max_alpha) {
            *max_alpha = (uint8_t)(row[i] >> 24);
        }
    }
    s->row_key = want;
    s->row_alpha = *max_alpha;
    for (i = f->nclut; i-- > 0;) {
        if (f->clut_key[i] == key) {
            return s->row = (int32_t)i;
        }
    }
    if (f->nclut == DL_CLUT_ROWS) {
        s->st.unsupported++;
        return s->row = (int32_t)f->nclut - 1;
    }
    memcpy(f->clut[f->nclut], row, sizeof(row));
    f->clut_key[f->nclut] = key;
    s->st.cluts++;
    return s->row = (int32_t)f->nclut++;
}

/* ---- primitives -------------------------------------------------------- */

static DlVertex *out_vertices(DlFrame *f, uint32_t n)
{
    grow((void **)&f->v, &f->cap_v, f->nv + n, sizeof(DlVertex));
    f->nv += n;
    return &f->v[f->nv - n];
}

static void to_vertex(const DlStream *s, int ctx, const GsVtx *g, DlVertex *o)
{
    uint64_t off = s->xyoffset[ctx];

    /* The GS samples a pixel at its integer coordinate; a GPU at its
       centre. Half a pixel moves one convention onto the other, so a game
       coordinate (whole pixels, as XYConv leaves them) lands on a whole
       framebuffer pixel. */
    o->x = ((float)g->x - (float)(off & 0xFFFF)) / 16.0f + 0.5f;
    o->y = ((float)g->y - (float)((off >> 32) & 0xFFFF)) / 16.0f + 0.5f;
    o->u = g->u / 16.0f;
    o->v = g->v / 16.0f;
    o->r = g->r;
    o->g = g->g;
    o->b = g->b;
    o->a = g->a;
}

static int same_batch(const DlBatch *a, const DlBatch *b)
{
    return !a->gs_blend && !b->gs_blend && a->blit < 0 && b->blit < 0 && a->surface == b->surface &&
           !memcmp(&a->blend, &b->blend, sizeof(a->blend)) && a->tex == b->tex && a->clut == b->clut &&
           a->tex_kind == b->tex_kind && a->tcc == b->tcc && a->tfx == b->tfx && a->tw == b->tw && a->th == b->th &&
           !memcmp(a->scissor, b->scissor, sizeof(a->scissor));
}

/* MakeFullBuffer's clear: an untextured sprite over the whole scissor,
   blended Cs * FIX (graph.c). */
static int is_clear(const DlStream *s, uint64_t p, const GsVtx *v, int n)
{
    uint64_t sc = s->scissor[0];
    int x0, y0, x1, y1;

    if ((p & 0x17) != 6 || n != 2 || (s->alpha[0] & 0xFF) != 0xA8) {
        return 0;
    }
    x0 = ((int)v[0].x - (int)(s->xyoffset[0] & 0xFFFF)) >> 4;
    y0 = ((int)v[0].y - (int)((s->xyoffset[0] >> 32) & 0xFFFF)) >> 4;
    x1 = ((int)v[1].x - (int)(s->xyoffset[0] & 0xFFFF)) >> 4;
    y1 = ((int)v[1].y - (int)((s->xyoffset[0] >> 32) & 0xFFFF)) >> 4;
    return x0 <= (int)(sc & 0x7FF) && y0 <= (int)((sc >> 32) & 0x7FF) && x1 > (int)((sc >> 16) & 0x7FF) &&
           y1 > (int)((sc >> 48) & 0x7FF);
}

/* 0 if the primitive is drawn under GS state the renderer doesn't
   reproduce: an alpha, destination-alpha or depth test that can fail, PABE,
   FBA, a frame write mask, COLCLAMP off, or fog. The game sets none of it
   (DRAW-INVENTORY.md). */
static int state_supported(const DlStream *s, int ctx, uint64_t p)
{
    uint64_t t = s->test[ctx];
    int ate = (int)(t & 1), atst = (int)((t >> 1) & 7), date = (int)((t >> 14) & 1);
    int zte = (int)((t >> 16) & 1), ztst = (int)((t >> 17) & 3);

    return !(ate && atst != 1) && !date && !(zte && ztst != 1) && !(s->pabe & 1) && !(s->fba[ctx] & 1) &&
           (s->frame[ctx] >> 32) == 0 && (s->colclamp & 1) && !((p >> 5) & 1);
}

/* 1 if the primitive covers no pixel inside its scissor (the debug text
   at cell 40 is one: pixel 640 and on). A primitive covers the pixels from
   its vertices' smallest coordinate up to, not including, the largest;
   lines and points get a pixel's margin. */
static int outside_scissor(const DlStream *s, int ctx, const GsVtx *v, int n, int margin)
{
    uint64_t sc = s->scissor[ctx], off = s->xyoffset[ctx];
    int x0 = 1 << 20, y0 = 1 << 20, x1 = -(1 << 20), y1 = -(1 << 20), i;

    for (i = 0; i < n; i++) {
        int x = (int)v[i].x - (int)(off & 0xFFFF), y = (int)v[i].y - (int)((off >> 32) & 0xFFFF);

        x0 = x < x0 ? x : x0, x1 = x > x1 ? x : x1;
        y0 = y < y0 ? y : y0, y1 = y > y1 ? y : y1;
    }
    margin *= 16;
    return x1 + margin <= (int)(sc & 0x7FF) * 16 || x0 - margin > (int)((sc >> 16) & 0x7FF) * 16 ||
           y1 + margin <= (int)((sc >> 32) & 0x7FF) * 16 || y0 - margin > (int)((sc >> 48) & 0x7FF) * 16;
}

/* A quad's corners, 0 1 over 2 3, as two triangles. */
static void out_quad(DlFrame *f, DlBatch *b, const DlVertex c[4])
{
    DlVertex *o = out_vertices(f, 6);

    o[0] = c[0], o[1] = c[1], o[2] = c[2];
    o[3] = c[1], o[4] = c[3], o[5] = c[2];
    b->count += 6;
}

/* A sprite: the rectangle between its two vertices, flat in the second
   vertex's colour, UVs interpolated along each axis. */
static void out_sprite(const DlStream *s, int ctx, DlFrame *f, DlBatch *b, const GsVtx *v)
{
    GsVtx k[4] = { v[0], v[1], v[0], v[1] };
    DlVertex c[4];
    int i;

    k[1].x = v[1].x, k[1].y = v[0].y, k[1].u = v[1].u, k[1].v = v[0].v;
    k[2].x = v[0].x, k[2].y = v[1].y, k[2].u = v[0].u, k[2].v = v[1].v;
    for (i = 0; i < 4; i++) {
        to_vertex(s, ctx, &k[i], &c[i]);
        c[i].r = v[1].r, c[i].g = v[1].g, c[i].b = v[1].b, c[i].a = v[1].a;
    }
    out_quad(f, b, c);
}

/* A line, as a strip one pixel across its minor axis, so it covers the
   pixel nearest the line in each column (or row) from its first vertex up
   to its second. Flat shading takes the second vertex's colour. */
static void out_line(const DlStream *s, int ctx, DlFrame *f, DlBatch *b, const GsVtx *v, int gouraud)
{
    DlVertex a, e, c[4];
    float ox = 0, oy = 0;
    int i;

    to_vertex(s, ctx, &v[0], &a);
    to_vertex(s, ctx, &v[1], &e);
    if (!gouraud) {
        a.r = e.r, a.g = e.g, a.b = e.b, a.a = e.a;
    }
    if ((e.x - a.x) * (e.x - a.x) >= (e.y - a.y) * (e.y - a.y)) {
        oy = 0.5f;
    } else {
        ox = 0.5f;
    }
    c[0] = a, c[1] = e, c[2] = a, c[3] = e;
    for (i = 0; i < 4; i++) {
        float k = i < 2 ? -1.0f : 1.0f;

        c[i].x += k * ox;
        c[i].y += k * oy;
    }
    out_quad(f, b, c);
}

static void draw(DlStream *s, const GsVtx *v, int n)
{
    uint64_t p = s->prmodecont ? s->prim : ((s->prim & 7) | (s->prmode & ~7ull));
    int type = (int)(p & 7), ctx = (int)((p >> 9) & 1), tme = (int)((p >> 4) & 1);
    uint64_t fr;
    DlFrame *f;
    DlBatch nb, *b;
    DlVertex c[3], *o;
    TexRes r;
    uint64_t a;
    uint32_t vmax = 0, as_max;
    int i, surface;

    if (s->list == DL_LIST_SETUP || (s->list == DL_LIST_REPLAY && is_clear(s, p, v, n))) {
        /* MakeFullBuffer's full-screen sprite starts a frame, and clears
           the draw buffer by being drawn like any other primitive */
        frame_begin(s);
        s->st.clears++;
    } else if (!s->in_frame) {
        frame_begin(s);
    }
    if (tme && !((p >> 8) & 1)) {
        /* STQ texture coordinates: nothing the game sends uses them */
        unsupported(s);
        return;
    }
    fr = s->frame[ctx];
    surface = surface_of(s, ((uint32_t)fr & 0x1FF) * 32, (uint32_t)(fr >> 16) & 0x3F, (int)((fr >> 24) & 0x3F), 1);
    if (surface < 0) {
        unsupported(s);
        return;
    }
    if (outside_scissor(s, ctx, v, n, type <= 2)) {
        s->st.clipped++;
        return;
    }
    if (!state_supported(s, ctx, p)) {
        s->st.state_unsupported++;
    }
    f = &s->frames[s->building];
    memset(&nb, 0, sizeof(nb));
    a = s->alpha[ctx];
    nb.blend.abe = (uint8_t)((p >> 6) & 1);
    nb.blend.a = (uint8_t)(a & 3);
    nb.blend.b = (uint8_t)((a >> 2) & 3);
    nb.blend.c = (uint8_t)((a >> 4) & 3);
    nb.blend.d = (uint8_t)((a >> 6) & 3);
    nb.blend.fix = (uint8_t)(a >> 32);
    nb.tex = nb.clut = -1;
    nb.surface = (uint8_t)surface;
    nb.blit = -1;
    nb.scissor[0] = (uint16_t)(s->scissor[ctx] & 0x7FF);
    nb.scissor[2] = (uint16_t)((s->scissor[ctx] >> 16) & 0x7FF);
    nb.scissor[1] = (uint16_t)((s->scissor[ctx] >> 32) & 0x7FF);
    nb.scissor[3] = (uint16_t)((s->scissor[ctx] >> 48) & 0x7FF);
    for (i = 0; i < n; i++) {
        vmax = v[i].a > vmax ? v[i].a : vmax;
    }
    as_max = vmax;
    if (tme) {
        uint64_t tex0 = s->tex0[ctx], off = s->xyoffset[ctx];
        uint8_t amax;

        if (resolve_texture(s, tex0, ((int)v[0].x - (int)(off & 0xFFFF)) >> 4,
                            ((int)v[0].y - (int)((off >> 32) & 0xFFFF)) >> 4, &r) != 0) {
            unsupported(s);
            return;
        }
        nb.tex = r.tex;
        nb.tex_kind = r.kind;
        nb.tw = r.tw;
        nb.th = r.th;
        nb.tex_w = r.w;
        nb.tex_h = r.h;
        nb.tcc = (uint8_t)((tex0 >> 34) & 1);
        nb.tfx = (uint8_t)((tex0 >> 35) & 3);
        amax = r.max_alpha;
        if (r.kind == DL_TEX_INDEX) {
            nb.clut = clut_row(s, tex0, &amax);
        }
        if (nb.tcc) {
            /* the texture function's alpha at most (draw.frag) */
            as_max = nb.tfx == 0 ? (uint32_t)(amax * vmax) >> 7 : nb.tfx == 2 ? amax + vmax : amax;
        }
    }
    if (nb.blend.abe && nb.blend.c == 0 && as_max > 0x80) {
        /* As above 1.0 (the hit sparks' CLUTs go to 0xFF): the GS blends
           with it unclamped, which fixed-function blending can't */
        nb.gs_blend = 1;
        s->st.alpha_over_one++;
    }
    b = f->nb ? &f->b[f->nb - 1] : NULL;
    if (!b || !same_batch(b, &nb) || b->first + b->count != f->nv) {
        grow((void **)&f->b, &f->cap_b, f->nb + 1, sizeof(DlBatch));
        b = &f->b[f->nb++];
        *b = nb;
        b->first = f->nv;
        b->count = 0;
        s->st.batches++;
    }
    s->st.prims++;
    if (s->list == DL_LIST_TAIL) {
        s->st.text_prims++;
    }
    if (nb.blend.abe) {
        s->st.blend[a & 0xFF]++;
    } else {
        s->st.no_blend++;
    }
    switch (type) {
    case 6:
        out_sprite(s, ctx, f, b, v);
        s->st.sprites++;
        return;
    case 0: {
        /* a point: the pixel nearest it, as a one-pixel sprite */
        GsVtx k[2] = { v[0], v[0] };

        k[0].x = (uint16_t)((v[0].x + 8) & ~15), k[0].y = (uint16_t)((v[0].y + 8) & ~15);
        k[1].x = (uint16_t)(k[0].x + 16), k[1].y = (uint16_t)(k[0].y + 16);
        out_sprite(s, ctx, f, b, k);
        s->st.points++;
        return;
    }
    case 1:
    case 2:
        out_line(s, ctx, f, b, v, (int)((p >> 3) & 1));
        s->st.lines++;
        return;
    }
    for (i = 0; i < 3; i++) {
        to_vertex(s, ctx, &v[i], &c[i]);
        if (!((p >> 3) & 1)) {
            /* flat shading: the colour of the vertex that kicked */
            c[i].r = v[2].r, c[i].g = v[2].g, c[i].b = v[2].b, c[i].a = v[2].a;
        }
    }
    o = out_vertices(f, 3);
    o[0] = c[0], o[1] = c[1], o[2] = c[2];
    b->count += 3;
    s->st.triangles++;
}

static void vertex(DlStream *s, uint16_t x, uint16_t y, int kick)
{
    int type = (int)(s->prim & 7), need;
    GsVtx *q;
    GsVtx tri[3];

    if (type == 7) {
        s->nq = 0;
        return;
    }
    q = &s->q[s->nq++];
    q->x = x;
    q->y = y;
    q->u = (uint16_t)(s->uv & 0x3FFF);
    q->v = (uint16_t)((s->uv >> 16) & 0x3FFF);
    q->r = (uint8_t)s->rgbaq;
    q->g = (uint8_t)(s->rgbaq >> 8);
    q->b = (uint8_t)(s->rgbaq >> 16);
    q->a = (uint8_t)(s->rgbaq >> 24);
    need = type == 0 ? 1 : type <= 2 || type == 6 ? 2 : 3;
    if (s->nq < need) {
        return;
    }
    if (kick) {
        if (type == 5) {
            tri[0] = s->q[0], tri[1] = s->q[s->nq - 2], tri[2] = s->q[s->nq - 1];
        } else {
            memcpy(tri, &s->q[s->nq - need], sizeof(GsVtx) * (size_t)need);
        }
        draw(s, tri, need);
    }
    switch (type) {
    case 2:
        s->q[0] = s->q[1];
        s->nq = 1;
        break;
    case 4:
        s->q[0] = s->q[1];
        s->q[1] = s->q[2];
        s->nq = 2;
        break;
    case 5:
        s->q[1] = s->q[2];
        s->nq = 2;
        break;
    default:
        s->nq = 0;
        break;
    }
}

/* ---- registers ---------------------------------------------------------- */

/* What the Draw Layer's packets and uploads set on list 2. */
static int draw_layer_reg(uint32_t addr)
{
    switch (addr) {
    case 0x00: /* PRIM */
    case 0x01: /* RGBAQ */
    case 0x03: /* UV */
    case 0x05: /* XYZ2 */
    case 0x06: /* TEX0_1 */
    case 0x3F: /* TEXFLUSH */
    case 0x42: /* ALPHA_1 */
    case 0x50: /* BITBLTBUF */
    case 0x51: /* TRXPOS */
    case 0x52: /* TRXREG */
    case 0x53: /* TRXDIR */
        return 1;
    }
    return 0;
}

static void set_tex0(DlStream *s, int ctx, uint64_t v)
{
    int psm = (int)((v >> 20) & 0x3F), cld = (int)((v >> 61) & 7);
    uint32_t cbp = (uint32_t)(v >> 37) & 0x3FFF;
    int load = 0;

    s->tex0[ctx] = v;
    if (!indexed(psm)) {
        return;
    }
    switch (cld) {
    case 1:
        load = 1;
        break;
    case 2:
    case 3:
        s->cbp[cld - 2] = cbp;
        load = 1;
        break;
    case 4:
    case 5:
        load = s->cbp[cld - 4] != cbp;
        s->cbp[cld - 4] = cbp;
        break;
    }
    if (load) {
        ensure(s, (int)((v >> 51) & 0xF), cbp, 1, 0, 0, psm == GS_PSMT8 ? 16 : 8, psm == GS_PSMT8 ? 16 : 2);
        gs_clut_load(&s->clut, &s->mem, psm, (int)((v >> 51) & 0xF), cbp, (uint32_t)(v >> 56) & 0x1F);
        s->clut_epoch++;
    }
}

static void write_reg(DlStream *s, uint32_t addr, uint64_t v)
{
    if (s->list == DL_LIST_DRAW && !draw_layer_reg(addr)) {
        s->st.unexpected[addr & 0xFF]++;
    }
    if (s->tr.active && s->tr.have) {
        transfer_cut(s);
    }
    switch (addr) {
    case 0x00:
        s->prim = v & 0x7FF;
        s->nq = 0;
        break;
    case 0x01:
        s->rgbaq = v;
        break;
    case 0x03:
        s->uv = v;
        break;
    case 0x04:
    case 0x05:
    case 0x0C:
    case 0x0D:
        vertex(s, (uint16_t)v, (uint16_t)(v >> 16), addr == 0x04 || addr == 0x05);
        break;
    case 0x06:
    case 0x07:
        set_tex0(s, (int)(addr - 0x06), v);
        break;
    case 0x08:
    case 0x09:
        s->clamp[addr - 0x08] = v;
        break;
    case 0x16:
    case 0x17: {
        /* TEX2: only the format and the CLUT fields */
        uint64_t m = 0x3Full << 20 | 0x7FFFFFFull << 37;

        set_tex0(s, (int)(addr - 0x16), (s->tex0[addr - 0x16] & ~m) | (v & m));
        break;
    }
    case 0x18:
    case 0x19:
        s->xyoffset[addr - 0x18] = v;
        break;
    case 0x1A:
        s->prmodecont = v & 1;
        break;
    case 0x1B:
        s->prmode = v & 0x7FF;
        break;
    case 0x3B:
        s->texa = v;
        break;
    case 0x46:
        s->colclamp = v;
        break;
    case 0x47:
    case 0x48:
        s->test[addr - 0x47] = v;
        break;
    case 0x49:
        s->pabe = v;
        break;
    case 0x4A:
    case 0x4B:
        s->fba[addr - 0x4A] = v;
        break;
    case 0x4C:
    case 0x4D:
        s->frame[addr - 0x4C] = v;
        break;
    case 0x40:
    case 0x41:
        s->scissor[addr - 0x40] = v;
        break;
    case 0x42:
    case 0x43:
        s->alpha[addr - 0x42] = v;
        break;
    case 0x50:
        s->bitbltbuf = v;
        break;
    case 0x51:
        s->trxpos = v;
        break;
    case 0x52:
        s->trxreg = v;
        break;
    case 0x53:
        transfer_start(s, (int)(v & 3));
        break;
    }
}

/* ---- GIF ----------------------------------------------------------------- */

static uint64_t rd64(const uint8_t *p)
{
    uint64_t v;

    memcpy(&v, p, 8);
    return v;
}

static void packed(DlStream *s, uint32_t reg, uint64_t lo, uint64_t hi)
{
    switch (reg) {
    case 0x0:
        write_reg(s, 0x00, lo);
        break;
    case 0x1:
        write_reg(s, 0x01, (lo & 0xFF) | ((lo >> 32) & 0xFF) << 8 | (hi & 0xFF) << 16 | ((hi >> 32) & 0xFF) << 24);
        break;
    case 0x3:
        write_reg(s, 0x03, (lo & 0x3FFF) | ((lo >> 32) & 0x3FFF) << 16);
        break;
    case 0x4:
    case 0x5:
        write_reg(s, ((hi >> 47) & 1) ? reg + 8 : reg, (lo & 0xFFFF) | ((lo >> 32) & 0xFFFF) << 16);
        break;
    case 0xE:
        write_reg(s, (uint32_t)(hi & 0xFF), lo);
        break;
    case 0xF:
    case 0x2:
    case 0xA:
    case 0xB:
        break;
    default:
        /* TEX0_1, TEX0_2, CLAMP_1, CLAMP_2: the register's own format */
        write_reg(s, reg, lo);
        break;
    }
}

static void gif_qword(DlStream *s, const uint8_t *q)
{
    GifState *g = &s->gif;
    uint64_t lo = rd64(q), hi = rd64(q + 8);
    int i;

    if (g->nloop == 0) {
        g->nloop = (uint32_t)(lo & 0x7FFF);
        g->flg = (int)((lo >> 58) & 3);
        if (g->flg == 3) {
            g->flg = 2;
        }
        g->nreg = (uint32_t)((lo >> 60) & 0xF);
        if (g->nreg == 0) {
            g->nreg = 16;
        }
        for (i = 0; i < 16; i++) {
            g->regs[i] = (uint8_t)((hi >> (4 * i)) & 0xF);
        }
        g->ri = 0;
        g->half = 0;
        if (g->flg == 0 && ((lo >> 46) & 1)) {
            write_reg(s, 0x00, (lo >> 47) & 0x7FF);
        }
        return;
    }
    if (g->flg == 0) {
        packed(s, g->regs[g->ri], lo, hi);
        if (++g->ri == g->nreg) {
            g->ri = 0;
            g->nloop--;
        }
        return;
    }
    if (g->flg == 1) {
        /* REGLIST: two 64-bit register values a qword; a loop of an odd
           register count ends mid-qword, and the rest is padding at the end */
        uint64_t w[2] = { lo, hi };

        for (i = 0; i < 2 && g->nloop; i++) {
            uint32_t r = g->regs[g->ri];

            if (r != 0xE && r != 0xF) {
                write_reg(s, r, w[i]);
            }
            if (++g->ri == g->nreg) {
                g->ri = 0;
                g->nloop--;
            }
        }
        return;
    }
    image_data(s, q, 1);
    g->nloop--;
}

static void gif_data(DlStream *s, const uint8_t *p, uint32_t qwc)
{
    GifState *g = &s->gif;
    uint32_t i = 0, n;

    while (i < qwc) {
        if (g->nloop && g->flg == 2) {
            /* IMAGE data: into the transfer in one copy, as much as there is */
            n = qwc - i < g->nloop ? qwc - i : g->nloop;
            image_data(s, p + 16 * i, n);
            g->nloop -= n;
            i += n;
        } else {
            gif_qword(s, p + 16 * i++);
        }
    }
}

void dl_stream_gif(DlStream *s, const uint8_t *data, size_t qwords, int list)
{
    s->list = list;
    s->st.sends++;
    gif_data(s, data, (uint32_t)qwords);
}

/* qwc qwords at an EE address (spr: scratchpad), or NULL if outside memory. */
static const uint8_t *dma_ptr(const DlMem *m, uint32_t addr, int spr, uint32_t qwc)
{
    uint64_t n = (uint64_t)qwc * 16;

    if (spr) {
        addr &= 0x3FF0;
        return addr + n <= 0x4000 ? m->spr + addr : NULL;
    }
    addr &= 0x01FFFFF0;
    return addr + n <= 0x02000000 ? m->ram + addr : NULL;
}

void dl_stream_send(DlStream *s, const DlMem *m, uint32_t tag, int list)
{
    uint32_t addr = tag & 0x7FFFFFF0u, stack[2], steps;
    int spr = (int)(tag >> 31), sp = 0;

    s->list = list;
    s->st.sends++;
    if (m->priv) {
        memcpy(s->priv, m->priv, PRIV_BYTES);
    }
    memset(&s->gif, 0, sizeof(s->gif));
    for (steps = 0; steps < 1u << 20; steps++) {
        const uint8_t *t = dma_ptr(m, addr, spr, 1), *d;
        uint64_t v;
        uint32_t qwc, id, a;
        int aspr;

        if (!t) {
            s->st.dma_errors++;
            return;
        }
        v = rd64(t);
        qwc = (uint32_t)(v & 0xFFFF);
        id = (uint32_t)((v >> 28) & 7);
        a = (uint32_t)(v >> 32) & 0x7FFFFFF0u;
        aspr = (int)(v >> 63);
        switch (id) {
        case 0: /* refe */
        case 3: /* ref */
        case 4: /* refs */
            d = dma_ptr(m, a, aspr, qwc);
            break;
        default:
            d = dma_ptr(m, addr + 16, spr, qwc);
            break;
        }
        if (!d) {
            s->st.dma_errors++;
            return;
        }
        if (s->tap && qwc) {
            s->tap(s->tap_ctx, d, qwc);
        }
        gif_data(s, d, qwc);
        switch (id) {
        case 0:
        case 7: /* end */
            return;
        case 1: /* cnt */
            addr += 16 + 16 * qwc;
            break;
        case 2: /* next */
            addr = a, spr = aspr;
            break;
        case 3:
        case 4:
            addr += 16;
            break;
        case 5: /* call */
            if (sp == 2) {
                s->st.dma_errors++;
                return;
            }
            stack[sp++] = addr + 16 + 16 * qwc;
            addr = a, spr = aspr;
            break;
        case 6: /* ret */
            if (sp == 0) {
                return;
            }
            addr = stack[--sp];
            break;
        }
    }
    s->st.dma_errors++;
}

void dl_stream_print_stats(const DlStream *s, void *file)
{
    FILE *f = file;
    const DlStats *t = &s->st;
    int i;

    fprintf(f, "draw layer: %llu sends, %llu frames, %llu prims (%llu triangles, %llu sprites) in %llu batches\n",
            (unsigned long long)t->sends, (unsigned long long)t->frames, (unsigned long long)t->prims,
            (unsigned long long)t->triangles, (unsigned long long)t->sprites, (unsigned long long)t->batches);
    fprintf(f, "draw layer: %llu uploads (%llu bytes); textures: %llu new (%llu texels), %llu from uploads, %llu "
               "from memory; %llu CLUT rows\n",
            (unsigned long long)t->uploads, (unsigned long long)t->upload_bytes, (unsigned long long)t->tex_new,
            (unsigned long long)t->tex_new_texels, (unsigned long long)t->tex_from_upload,
            (unsigned long long)t->tex_from_memory, (unsigned long long)t->cluts);
    fprintf(f, "draw layer: %llu uploads with less IMAGE data than their size\n",
            (unsigned long long)t->uploads_short);
    fprintf(f, "draw layer: %llu prims with As over 1.0, %llu unsupported, %llu DMA errors\n",
            (unsigned long long)t->alpha_over_one, (unsigned long long)t->unsupported,
            (unsigned long long)t->dma_errors);
    for (i = 0; i < 256; i++) {
        if (t->blend[i]) {
            fprintf(f, "draw layer: %llu prims blended with ALPHA 0x%02x\n", (unsigned long long)t->blend[i], i);
        }
    }
    if (t->no_blend) {
        fprintf(f, "draw layer: %llu prims not blended\n", (unsigned long long)t->no_blend);
    }
    fprintf(f, "draw layer: %llu upload blocks written into local memory on a read\n",
            (unsigned long long)t->blocks_written);
    fprintf(f,
            "gs path: %llu frame clears, %llu debug text sprites, %llu lines, %llu points, %llu prims outside their "
            "scissor\n",
            (unsigned long long)t->clears, (unsigned long long)t->text_prims, (unsigned long long)t->lines,
            (unsigned long long)t->points, (unsigned long long)t->clipped);
    fprintf(f,
            "gs path: %llu blits between framebuffers, %llu copies within local memory; %llu frames shown, %llu with "
            "both circuits merged\n",
            (unsigned long long)t->blits, (unsigned long long)t->local_copies, (unsigned long long)t->displays,
            (unsigned long long)t->displays_merged);
    fprintf(f,
            "gs path: %llu unsupported; %llu prims under GS state not reproduced; %llu texture reads and %llu uploads "
            "in a framebuffer's blocks\n",
            (unsigned long long)t->gs_unsupported, (unsigned long long)t->state_unsupported,
            (unsigned long long)t->surface_reads, (unsigned long long)t->surface_writes);
    for (i = 0; i < 256; i++) {
        if (t->unexpected[i]) {
            fprintf(f, "draw layer: list 2 wrote register 0x%02x %llu times\n", i,
                    (unsigned long long)t->unexpected[i]);
        }
    }
}
