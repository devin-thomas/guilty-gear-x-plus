/* Rollback over a session (rollback.h). */
#include "rollback.h"

#include <stdlib.h>
#include <string.h>

/* states: one before each of the last max_rollback + 1 frames, and one spare */
#define RING (RB_MAX_ROLLBACK + 2)
/* hashes: every frame not yet confirmed, and the ones confirmed since the caller last looked */
#define HRING 32
#define NONE UINT32_MAX

typedef struct {
    uint32_t frame;
    uint64_t state, session, regions[CORE_REGION_COUNT];
} Hashes;

struct Rollback {
    Session *s;
    RollbackConfig cfg;
    uint32_t frame;          /* the next frame to run */
    uint32_t confirmed;

    /* inputs by frame, grown as needed */
    uint32_t cap;
    PadInput *local, *remote, *used;   /* used: the remote input frame f last ran with */
    uint8_t *known;          /* the remote input for f has arrived */
    uint32_t local_n;        /* local inputs for frames before this one are known */
    int64_t contig;          /* every remote input up to this frame has arrived */
    uint32_t first_wrong;    /* the first frame run with a prediction since contradicted, or NONE */
    uint32_t last_run;       /* the frame run last (or running) */

    SessionState *ring[RING];
    uint32_t ring_frame[RING];
    Hashes hashes[HRING];
    RollbackStats st;

    double (*seconds)(void);
    void (*timed)(void *ctx, int load, double us);
    void *clock_ctx;
};

static int grow(Rollback *r, uint32_t n)
{
    uint32_t cap = r->cap ? r->cap : 1024, i;

    if (n <= r->cap) {
        return 0;
    }
    while (cap < n) {
        cap *= 2;
    }
#define GROW(p)                                                  \
    do {                                                         \
        void *q = realloc((p), (size_t)cap * sizeof(*(p)));      \
        if (!q) {                                                \
            return -1;                                           \
        }                                                        \
        (p) = q;                                                 \
    } while (0)
    GROW(r->local);
    GROW(r->remote);
    GROW(r->used);
    GROW(r->known);
#undef GROW
    for (i = r->cap; i < cap; i++) {
        pad_clear(&r->local[i]);
        pad_clear(&r->remote[i]);
        pad_clear(&r->used[i]);
        r->known[i] = 0;
    }
    r->cap = cap;
    return 0;
}

Rollback *rb_new(Session *s, const RollbackConfig *cfg)
{
    Rollback *r = calloc(1, sizeof(*r));
    int i;

    if (!r || cfg->local < 0 || cfg->local > 1 || cfg->delay < 0 || cfg->delay > RB_MAX_DELAY ||
        cfg->max_rollback < 1 || cfg->max_rollback > RB_MAX_ROLLBACK) {
        free(r);
        return NULL;
    }
    r->s = s;
    r->cfg = *cfg;
    r->first_wrong = NONE;
    if (grow(r, 1024) != 0) {
        rb_free(r);
        return NULL;
    }
    /* the first delay frames: nothing pressed, on both sides */
    r->local_n = (uint32_t)cfg->delay;
    for (i = 0; i < cfg->delay; i++) {
        r->known[i] = 1;
    }
    r->contig = cfg->delay - 1;
    for (i = 0; i < RING; i++) {
        /* each state's first save is whole; made now, every later one copies only what changed */
        if (!(r->ring[i] = session_state_new(s)) || session_save(s, r->ring[i]) != 0) {
            rb_free(r);
            return NULL;
        }
        r->ring_frame[i] = NONE;
    }
    for (i = 0; i < HRING; i++) {
        r->hashes[i].frame = NONE;
    }
    return r;
}

void rb_free(Rollback *r)
{
    int i;

    if (!r) {
        return;
    }
    for (i = 0; i < RING; i++) {
        session_state_free(r->ring[i]);
    }
    free(r->local);
    free(r->remote);
    free(r->used);
    free(r->known);
    free(r);
}

uint32_t rb_local_next(const Rollback *r)
{
    return r->local_n;
}

uint32_t rb_add_local(Rollback *r, const PadInput *in)
{
    uint32_t f = r->local_n;

    if (grow(r, f + 1) != 0) {
        return NONE;
    }
    r->local[f] = *in;
    r->local_n++;
    return f;
}

void rb_add_remote(Rollback *r, uint32_t frame, const PadInput *in)
{
    if (grow(r, frame + 1) != 0 || r->known[frame]) {
        return;
    }
    r->remote[frame] = *in;
    r->known[frame] = 1;
    while ((uint64_t)(r->contig + 1) < r->cap && r->known[r->contig + 1]) {
        r->contig++;
    }
    if (frame < r->frame && !pad_equal(&r->used[frame], in)) {
        r->st.mispredicted++;
        if (frame < r->first_wrong) {
            r->first_wrong = frame;
        }
    }
}

static double now(const Rollback *r)
{
    return r->seconds ? r->seconds() : 0;
}

/* Runs frame f from the state before it, which it saves first. */
static int run(Rollback *r, uint32_t f)
{
    Session *s = r->s;
    SessionState *st = r->ring[f % RING];
    SessionInput in;
    Hashes *hs;
    double t0 = now(r);
    int i;

    if (session_save(s, st) != 0) {
        return -1;
    }
    if (r->timed) {
        r->timed(r->clock_ctx, 0, (now(r) - t0) * 1e6);
    }
    r->ring_frame[f % RING] = f;
    r->last_run = f;
    memset(&in, 0, sizeof(in));
    in.pads[r->cfg.local] = r->local[f];
    if (r->known[f]) {
        r->used[f] = r->remote[f];
    } else {
        /* the prediction: the last input that arrived in order */
        if (r->contig >= 0) {
            r->used[f] = r->remote[r->contig];
        } else {
            pad_clear(&r->used[f]);
        }
        r->st.predicted++;
    }
    in.pads[1 - r->cfg.local] = r->used[f];
    if (session_field(s, &in) != 0) {
        return -1;
    }
    hs = &r->hashes[f % HRING];
    hs->frame = f;
    hs->session = session_hash(s);
    hs->state = core_hash(s->h);
    for (i = 0; i < CORE_REGION_COUNT; i++) {
        hs->regions[i] = core_region_hash(s->h, (uint32_t)i + 1);
    }
    return 0;
}

static void confirm(Rollback *r)
{
    uint32_t c = (uint64_t)(r->contig + 1) < r->frame ? (uint32_t)(r->contig + 1) : r->frame;

    if (c > r->confirmed && r->first_wrong >= c) {
        r->confirmed = c;
        session_confirmed(r->s, r->s->h->field - (r->frame - c));
    }
}

int rb_advance(Rollback *r)
{
    uint32_t f;

    if (r->first_wrong < r->frame) {
        uint32_t g = r->first_wrong, depth = r->frame - g;
        SessionState *st = r->ring[g % RING];
        double t0 = now(r);

        if (r->ring_frame[g % RING] != g || session_load(r->s, st) != 0) {
            return -1;
        }
        if (r->timed) {
            r->timed(r->clock_ctx, 1, (now(r) - t0) * 1e6);
        }
        r->st.rollbacks++;
        r->st.depth[depth <= RB_MAX_ROLLBACK ? depth : RB_MAX_ROLLBACK]++;
        if (depth > r->st.depth_max) {
            r->st.depth_max = depth;
        }
        for (f = g; f < r->frame; f++) {
            if (run(r, f) != 0) {
                return -1;
            }
            r->st.resimulated++;
        }
        r->first_wrong = NONE;
    }
    confirm(r);
    /* predicting this frame would make one too many */
    if ((int64_t)r->frame - r->contig > r->cfg.max_rollback || r->local_n <= r->frame) {
        r->st.waits++;
        return 0;
    }
    if (grow(r, r->frame + 1) != 0 || run(r, r->frame) != 0) {
        return -1;
    }
    r->frame++;
    confirm(r);
    return 1;
}

uint32_t rb_frame(const Rollback *r)
{
    return r->frame;
}

uint32_t rb_confirmed(const Rollback *r)
{
    return r->confirmed;
}

int rb_hashes(const Rollback *r, uint32_t f, uint64_t *state, uint64_t *session, uint64_t regions[7])
{
    const Hashes *hs = &r->hashes[f % HRING];

    if (f >= r->confirmed || hs->frame != f) {
        return 0;
    }
    if (state) {
        *state = hs->state;
    }
    if (session) {
        *session = hs->session;
    }
    if (regions) {
        memcpy(regions, hs->regions, sizeof(hs->regions));
    }
    return 1;
}

int rb_input(const Rollback *r, uint32_t f, SessionInput *in)
{
    if (f >= r->local_n || f >= r->cap || !r->known[f]) {
        return 0;
    }
    memset(in, 0, sizeof(*in));
    in->pads[r->cfg.local] = r->local[f];
    in->pads[1 - r->cfg.local] = r->remote[f];
    return 1;
}

int rb_ran_with(const Rollback *r, uint32_t f, SessionInput *in)
{
    if (f > r->last_run || f >= r->cap) {
        return 0;
    }
    memset(in, 0, sizeof(*in));
    in->pads[r->cfg.local] = r->local[f];
    in->pads[1 - r->cfg.local] = r->used[f];
    return 1;
}

uint32_t rb_last_run(const Rollback *r)
{
    return r->last_run;
}

const RollbackStats *rb_stats(const Rollback *r)
{
    return &r->st;
}

void rb_set_clock(Rollback *r, double (*seconds)(void), void (*timed)(void *ctx, int load, double us), void *ctx)
{
    r->seconds = seconds;
    r->timed = timed;
    r->clock_ctx = ctx;
}
