/* ggx-rollback-test: two instances of the game playing each other through
 * rollback, on one machine (PORT-013, ADR-014, SPEC.md section 2.4).
 *
 *   ggx-rollback-test [--data-dir <dir> | --disc <image>] [--elf <boot elf>]
 *                     [--warm N] [--fields N] [--delays 0,1,2] [--max-rollback 8]
 *                     [--mash SEED | --script <file>] [--card1 store|empty|none] [--save-dir <dir>]
 *                     [--audio] [--render] [--net SEED] [--expect-save]
 *
 * Three Cores run side by side in this process, each a session
 * (session.h) with its cards detached:
 *
 *   - R, the reference, runs every field with both players' real inputs,
 *     never predicting. It is also where the inputs come from: --mash SEED
 *     (mash.h, the default seed 1) or a GGScript --script for both pads.
 *   - A and B are the two players' instances, each under rollback
 *     (rollback.h): A's own input is player 1's, B's is player 2's, each
 *     delayed by the run's input delay and sent to the other over a made-up
 *     network whose latency changes as the run goes on (below). Each
 *     predicts the other's input, rolls back when it was wrong, and waits
 *     rather than predict more than --max-rollback fields.
 *
 * R boots and runs --warm fields (default 3000); A boots and runs the same
 * fields, and must reach the same session hash. B never boots: it joins
 * from A's state, copied whole as if sent over the network (the whole-copy
 * path, STATE-HASH.md section 4), as an online opponent would. Then for each
 * input delay in --delays (default 0, 1 and 2), from that same point, A and B
 * play --fields fields (default 10,000) and every field each confirms must
 * have R's State Hash and session hash. A difference stops the run and names
 * the field and the regions that differ.
 *
 * The network: every input is sent once, as soon as it is known, and
 * arrives after a latency in ticks (a tick is one field's time on both
 * instances). The latency's base level changes every 600 ticks through 0, 2,
 * 4, 7 and max-rollback + delay + 1 to + 3 fields, with a field of jitter
 * either way and a spike of 6 more on one input in 200, so inputs
 * arrive out of order and both instances roll back the whole --max-rollback
 * and wait past it. --net picks another schedule. The run fails if neither
 * instance ever rolled back --max-rollback fields, which would mean it never
 * tested that depth.
 *
 * Saves (--card1 store): slot 1 holds the native store's card, as in ggx,
 * but detached, so nothing the game saves under rollback reaches the store:
 * the store's files must be byte for byte the same at the end. With a
 * --script that saves, the save is made inside
 * the session, rolled back across and made again; the cards are in the
 * session hash, so every confirmed field checks them too, and the run
 * reports how many rollbacks put a card back. --expect-save fails the run
 * unless the reference's card ends up holding the release's system
 * directory and some rollback put a card back.
 *
 * Sound (--audio): A's and R's sound drivers play what their Cores send
 * (audio_host.h). A's follows its rollbacks: it plays each field once, as
 * it first runs, and when a rollback changes what a field sent the driver,
 * it is put back to its state at the last confirmed field and fed the
 * fields since as they now are, without playing them again. Every field's
 * sound must then be R's, except fields that played while a sound
 * prediction was wrong, which are counted; and the commands the driver
 * consumed for confirmed fields must be R's, command for command: nothing
 * doubled, nothing lost.
 *
 * Pictures (--render): A's and R's renderers (render/dl_stream.h, no GPU)
 * read what their Cores send the GS. The renderer keeps GS local memory,
 * where the game uploads textures as it decides they are needed, and a
 * rollback doesn't put it back. Whenever A has no prediction outstanding,
 * its last frame (the triangles, the texels each draws, the CLUTs, the
 * display) is compared with R's for the same field, and the run reports
 * how many such frames differed: what a rollback leaves on screen after it
 * has caught up. It doesn't fail the run (ROLLBACK.md section 7).
 *
 * Exit status 0 if every check passed.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_host.h"
#include "core_host.h"
#include "dl_stream.h"
#include "fsys.h"
#include "ggscript.h"
#include "mash.h"
#include "mc_card.h"
#include "replays.h"
#include "rollback.h"
#include "save_file.h"
#include "session.h"
#include "unlock.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

static double seconds(void)
{
#ifdef _WIN32
    LARGE_INTEGER f, c;

    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)f.QuadPart;
#else
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + t.tv_nsec * 1e-9;
#endif
}

static int failures;
static uint64_t card_restores;   /* both instances, every round */

static void fail(const char *fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 1, 2)))
#endif
    ;

static void fail(const char *fmt, ...)
{
    va_list ap;

    printf("FAIL ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    fflush(stdout);
    failures++;
}

/* ---- timings ---- */

typedef struct {
    const char *what;
    double *us;
    size_t n, cap;
} Timing;

static void timed(Timing *t, double us)
{
    if (t->n == t->cap) {
        t->cap = t->cap ? 2 * t->cap : 4096;
        t->us = realloc(t->us, t->cap * sizeof(*t->us));
        if (!t->us) {
            exit(1);
        }
    }
    t->us[t->n++] = us;
}

static int cmp_double(const void *a, const void *b)
{
    double x = *(const double *)a, y = *(const double *)b;

    return x < y ? -1 : x > y;
}

static void report(Timing *t)
{
    size_t over = 0, i;

    if (!t->n) {
        return;
    }
    qsort(t->us, t->n, sizeof(*t->us), cmp_double);
    for (i = 0; i < t->n; i++) {
        over += t->us[i] > 1000.0;
    }
    printf("    %-24s n=%-7zu median %7.1f us  p99 %7.1f  max %8.1f  over 1 ms: %zu\n", t->what, t->n,
           t->us[t->n / 2], t->us[(t->n * 99) / 100], t->us[t->n - 1], over);
    t->n = 0;
}

/* the slowest saves and loads, with the field and the game's job: which screens pass 1 ms */
#define SLOWEST 4

typedef struct {
    double us;
    uint32_t field;
    const char *job;
} Slow;

typedef struct {
    Timing save, load;
    CoreHost *h;
    Slow slow[2][SLOWEST];   /* by time */
    Slow most[2][SLOWEST];   /* by bytes copied (us holds the bytes) */
    double bytes[2];
    uint64_t n[2];
    double fight_bytes[2], fight_us[2];   /* the most in a fight (GameMain) */
} Clock;

static void keep(Slow *s, double v, CoreHost *h)
{
    int i, j;

    for (i = 0; i < SLOWEST && s[i].us >= v; i++) {
    }
    if (i < SLOWEST) {
        for (j = SLOWEST - 1; j > i; j--) {
            s[j] = s[j - 1];
        }
        s[i].us = v;
        s[i].field = h->field;
        s[i].job = ggx_func_name(ee_r32(h, ggx_sym_jobfunc + 4 * ee_r32(h, ggx_sym_JobMode)));
    }
}

static void on_timed(void *ctx, int load, double us)
{
    Clock *c = ctx;
    double bytes = (double)core_state_last_copy(c->h);

    timed(load ? &c->load : &c->save, us);
    keep(c->slow[load], us, c->h);
    keep(c->most[load], bytes, c->h);
    c->bytes[load] += bytes;
    c->n[load]++;
    if (!strcmp(ggx_func_name(ee_r32(c->h, ggx_sym_jobfunc + 4 * ee_r32(c->h, ggx_sym_JobMode))), "GameMain")) {
        c->fight_bytes[load] = bytes > c->fight_bytes[load] ? bytes : c->fight_bytes[load];
        c->fight_us[load] = us > c->fight_us[load] ? us : c->fight_us[load];
    }
}

static void report_slowest(Clock *c, const char *who)
{
    int k, i;

    for (k = 0; k < 2; k++) {
        printf("    %s: slowest %s:", who, k ? "loads" : "saves");
        for (i = 0; i < SLOWEST && c->slow[k][i].us > 0; i++) {
            printf(" %.0f us (field %u, %s)", c->slow[k][i].us, c->slow[k][i].field, c->slow[k][i].job);
        }
        printf("\n");
        printf("    %s: %s copy %.0f KB on average; the most:", who, k ? "loads" : "saves",
               c->n[k] ? c->bytes[k] / c->n[k] / 1024 : 0.0);
        for (i = 0; i < SLOWEST && c->most[k][i].us > 0; i++) {
            printf(" %.0f KB (field %u, %s)", c->most[k][i].us / 1024, c->most[k][i].field, c->most[k][i].job);
        }
        printf("; in fights at most %.0f KB (%.0f us)\n", c->fight_bytes[k] / 1024, c->fight_us[k]);
    }
    memset(c->slow, 0, sizeof(c->slow));
    memset(c->most, 0, sizeof(c->most));
    c->bytes[0] = c->bytes[1] = 0;
    c->n[0] = c->n[1] = 0;
    c->fight_bytes[0] = c->fight_bytes[1] = c->fight_us[0] = c->fight_us[1] = 0;
}

/* ---- sound ---- */

/* each field's sound in A and R, from the round's first field: the samples as they played, and the calls once
   final */
typedef struct {
    uint32_t base, n;
    uint64_t *played, *final;
    uint8_t *tainted;
} SoundLog;

static AudioHost *audio_a, *audio_r;
static SoundLog sound_a, sound_r;

static void on_played(void *ctx, uint32_t field, const int16_t *samples, uint32_t frames)
{
    SoundLog *l = ctx;

    if (field >= l->base && field - l->base < l->n) {
        l->played[field - l->base] = XXH3_64bits(samples, (size_t)frames * 4) | 1;
    }
}

static void on_final(void *ctx, uint32_t field, uint64_t calls)
{
    SoundLog *l = ctx;

    if (field >= l->base && field - l->base < l->n) {
        if (l->final[field - l->base]) {
            /* a field's calls are final once */
            l->final[field - l->base] = 2;
        } else {
            l->final[field - l->base] = calls | 4;
        }
    }
}

static void on_taint(void *ctx, uint32_t first, uint32_t end)
{
    SoundLog *l = ctx;
    uint32_t f;

    for (f = first; f < end; f++) {
        if (f >= l->base && f - l->base < l->n) {
            l->tainted[f - l->base] = 1;
        }
    }
}

static int sound_watch(AudioHost *a, SoundLog *l, uint32_t base, uint32_t n)
{
    AudioWatch w = { on_final, on_played, on_taint, l };

    l->base = base;
    l->n = n;
    l->played = calloc(n, sizeof(*l->played));
    l->final = calloc(n, sizeof(*l->final));
    l->tainted = calloc(n, 1);
    if (!l->played || !l->final || !l->tainted) {
        return -1;
    }
    audio_host_watch(a, &w);
    return 0;
}

/* A's sound against R's, for the fields both confirmed */
static void sound_check(uint32_t fields)
{
    const AudioRollbackStats *st = audio_host_rollback_stats(audio_a);
    uint32_t f, differ = 0, tainted = 0, bad = 0, bad_calls = 0;

    for (f = 0; f < fields; f++) {
        if (!sound_r.final[f] || !sound_a.final[f] || sound_a.final[f] != sound_r.final[f]) {
            bad_calls++;
            if (!bad++) {
                fail("sound: field %u: the calls the driver heard for it %s", sound_a.base + f,
                     sound_a.final[f] == 2 ? "were given to it twice" : !sound_a.final[f] ? "never reached it"
                                                                       : "are not the reference's");
            }
            continue;
        }
        tainted += sound_a.tainted[f];
        if (sound_a.played[f] != sound_r.played[f]) {
            differ++;
            if (!sound_a.tainted[f] && !bad++) {
                fail("sound: field %u played other samples than the reference's, with no prediction to blame",
                     sound_a.base + f);
            }
        }
    }
    if (st->lost) {
        fail("sound: %llu fields the layer couldn't place", (unsigned long long)st->lost);
    }
    printf("  sound (A against R): %u fields; every field's calls reached A's driver once, as R's did%s;\n"
           "    %llu rollbacks, %llu changed the sound and put the driver back; %u fields played while a sound "
           "prediction was wrong, %u of them sounding other than R's; every other field's samples R's\n",
           fields, bad_calls ? " (NOT)" : "", (unsigned long long)st->rollbacks, (unsigned long long)st->rebuilds, tainted,
           differ);
}

/* ---- pictures ---- */

typedef struct {
    CoreHost *h;
    DlStream *s;
} Screen;

static Screen screen_a, screen_r;
static uint64_t *picture_r;      /* R's picture after each frame of the round */
static uint32_t picture_base, picture_n;

static void on_gif(void *ctx, uint32_t tag)
{
    Screen *k = ctx;
    uint32_t a = tag & 0x7FFFFFFFu;
    DlMem m;
    int i, list = DL_LIST_OTHER;

    for (i = 0; i < 4; i++) {
        uint32_t p = ee_r32(k->h, ggx_sym_handle_list + 4 * (uint32_t)i);

        if (p && p + 8 <= EE_RAM_SIZE && (ee_r32(k->h, p) == a || ee_r32(k->h, p + 4) == a)) {
            list = i;
            break;
        }
    }
    m.ram = k->h->mem;
    m.spr = k->h->mem + EE_SPR_BASE;
    m.priv = k->h->mem + 0x12000000u;
    dl_stream_send(k->s, &m, tag, list);
}

static int screen_open(Screen *k, CoreHost *h)
{
    k->h = h;
    if (!(k->s = dl_stream_new())) {
        return -1;
    }
    h->gif_send = on_gif;
    h->gif_ctx = k;
    return 0;
}

/* What the last frame shows, independent of the texture cache's numbering: every triangle, each batch with the
   texels it draws (their cache key) and its CLUT row's colours, the surfaces, blits and the display. */
static uint64_t picture(Screen *k)
{
    const DlFrame *f = k->s ? dl_stream_frame(k->s) : NULL;
    DlTexCache *t;
    XXH3_state_t x;
    uint32_t i;

    if (!f) {
        return 0;
    }
    t = dl_stream_textures(k->s);
    XXH3_64bits_reset(&x);
    XXH3_64bits_update(&x, f->v, (size_t)f->nv * sizeof(*f->v));
    for (i = 0; i < f->nb; i++) {
        DlBatch b = f->b[i];
        uint64_t key = b.tex >= 0 && (uint32_t)b.tex < t->n ? t->e[b.tex].key : 0;

        XXH3_64bits_update(&x, &key, 8);
        if (b.clut >= 0 && b.clut < DL_CLUT_ROWS) {
            XXH3_64bits_update(&x, f->clut[b.clut], sizeof(f->clut[0]));
        }
        b.tex = 0;
        b.clut = 0;
        XXH3_64bits_update(&x, &b, sizeof(b));
    }
    XXH3_64bits_update(&x, f->blit, (size_t)f->nblit * sizeof(f->blit[0]));
    XXH3_64bits_update(&x, &f->display, sizeof(f->display));
    return XXH3_64bits_digest(&x);
}

/* ---- the reference, and where the inputs come from ---- */

typedef struct {
    uint64_t state, session, regions[CORE_REGION_COUNT];
} Hashes;

static CoreHost host_r, host_a, host_b;
static Session sess_r, sess_a, sess_b;
static uint64_t seed = 1;
static GgScript *script;

/* R's run: the inputs of every frame of the round (from the round's first field), and the hashes after it */
typedef struct {
    uint32_t base;          /* R's field at frame 0 */
    int delay;              /* frames before it are nothing pressed */
    uint32_t n, cap;
    SessionInput *in;
    Hashes *h;
} Reference;

static void real_input(uint32_t field, SessionInput *in)
{
    memset(in, 0, sizeof(*in));
    pad_clear(&in->pads[0]);
    pad_clear(&in->pads[1]);
    if (script) {
        ggs_play_to(script, (uint64_t)host_r.vsyncs + 1, in->pads);
    } else {
        mash_input(seed, field, in->pads);
    }
}

static void region_hashes(CoreHost *h, uint64_t out[CORE_REGION_COUNT])
{
    int i;

    for (i = 0; i < CORE_REGION_COUNT; i++) {
        out[i] = core_region_hash(h, (uint32_t)i + 1);
    }
}

/* Runs R up to and including frame f. */
static int reference_to(Reference *ref, uint32_t f)
{
    while (ref->n <= f) {
        SessionInput in;
        Hashes *hs;

        if (ref->n == ref->cap) {
            ref->cap = ref->cap ? 2 * ref->cap : 16384;
            ref->in = realloc(ref->in, ref->cap * sizeof(*ref->in));
            ref->h = realloc(ref->h, ref->cap * sizeof(*ref->h));
            if (!ref->in || !ref->h) {
                return -1;
            }
        }
        real_input(host_r.field, &in);
        if (ref->n < (uint32_t)ref->delay) {
            pad_clear(&in.pads[0]);
            pad_clear(&in.pads[1]);
        }
        if (session_field(&sess_r, &in) != 0) {
            return -1;
        }
        audio_host_field(audio_r, host_r.field - 1, 1);
        if (screen_r.s && ref->n < picture_n) {
            picture_r[ref->n] = picture(&screen_r);
        }
        ref->in[ref->n] = in;
        hs = &ref->h[ref->n];
        hs->state = core_hash(&host_r);
        hs->session = session_hash(&sess_r);
        region_hashes(&host_r, hs->regions);
        ref->n++;
    }
    return 0;
}

/* ---- the network ---- */

typedef struct {
    uint64_t at;            /* the tick it arrives */
    uint32_t frame;
    PadInput in;
} Packet;

typedef struct {
    Packet *p;
    size_t n, cap;
} Wire;

static uint64_t net_seed = 0x6767782d6e6574ull;

static uint32_t latency(uint64_t tick, int dir, uint32_t frame, int delay, int max_rollback)
{
    static const int base[5] = { 0, 2, 4, 7, -1 };
    uint64_t r = mash_mix(net_seed ^ mash_mix(((uint64_t)frame << 1 | (uint64_t)dir) ^ 0x9e3779b97f4a7c15ull));
    int b = base[(tick / 600) % 5], l;

    if (b < 0) {
        /* past the rollback window: a rollback of the whole window, and waits */
        b = max_rollback + delay + 1 + (int)((tick / 600 / 5) % 3);
    }
    l = b + (int)(r % 3) - 1;
    if ((r >> 8) % 200 == 0) {
        l += 6;
    }
    return l < 0 ? 0 : (uint32_t)l;
}

static int wire_send(Wire *w, uint64_t at, uint32_t frame, const PadInput *in)
{
    if (w->n == w->cap) {
        w->cap = w->cap ? 2 * w->cap : 256;
        w->p = realloc(w->p, w->cap * sizeof(*w->p));
        if (!w->p) {
            return -1;
        }
    }
    w->p[w->n].at = at;
    w->p[w->n].frame = frame;
    w->p[w->n].in = *in;
    w->n++;
    return 0;
}

static void deliver(Wire *w, uint64_t tick, Rollback *to)
{
    size_t i, k = 0;

    for (i = 0; i < w->n; i++) {
        if (w->p[i].at <= tick) {
            rb_add_remote(to, w->p[i].frame, &w->p[i].in);
        } else {
            w->p[k++] = w->p[i];
        }
    }
    w->n = k;
}

/* ---- one round: A and B from the join, at one input delay ---- */

static const char *const REGION_NAMES[CORE_REGION_COUNT] = { "ram", "spr", "eeio", "vumem", "gspriv", "own", "host" };

static int check_frame(const char *who, Rollback *rb, Reference *ref, uint32_t f)
{
    uint64_t state, sess, regions[CORE_REGION_COUNT];
    SessionInput in;
    const Hashes *want;
    int i;

    if (!rb_hashes(rb, f, &state, &sess, regions)) {
        fail("%s: frame %u is confirmed but its hashes are gone", who, f);
        return -1;
    }
    if (reference_to(ref, f) != 0) {
        fail("the reference trapped at frame %u", f);
        return -1;
    }
    want = &ref->h[f];
    if (!rb_input(rb, f, &in) || !session_input_equal(&in, &ref->in[f])) {
        fail("%s: frame %u ran with other inputs than the players gave", who, f);
        return -1;
    }
    if (state != want->state || sess != want->session) {
        char what[256] = "";
        size_t o = 0;

        for (i = 0; i < CORE_REGION_COUNT; i++) {
            if (regions[i] != want->regions[i]) {
                o += (size_t)snprintf(what + o, sizeof(what) - o, " %s", REGION_NAMES[i]);
            }
        }
        if (state == want->state) {
            snprintf(what + o, sizeof(what) - o, " (the Core agrees: the session's cards or unlocks differ)");
        }
        fail("%s: field %u (frame %u): State Hash %016llx, session %016llx; the reference's %016llx, %016llx;"
             " differing:%s", who, ref->base + f + 1, f, (unsigned long long)state, (unsigned long long)sess,
             (unsigned long long)want->state, (unsigned long long)want->session, what);
        return -1;
    }
    return 0;
}

static void print_stats(const char *who, const Rollback *rb, const Session *s)
{
    const RollbackStats *st = rb_stats(rb);
    int d;

    printf("  %s: %u fields run, %u confirmed; %llu predicted, %llu wrong; %llu rollbacks, %llu fields run again; "
           "%llu waits; deepest rollback %u; cards put back %llu\n", who, rb_frame(rb), rb_confirmed(rb),
           (unsigned long long)st->predicted, (unsigned long long)st->mispredicted, (unsigned long long)st->rollbacks,
           (unsigned long long)st->resimulated, (unsigned long long)st->waits, st->depth_max,
           (unsigned long long)s->card_restores);
    printf("    rollbacks by depth:");
    for (d = 1; d <= RB_MAX_ROLLBACK; d++) {
        printf(" %d:%llu", d, (unsigned long long)st->depth[d]);
    }
    printf("\n");
}

static const char *script_path;
static CoreHostConfig host_cfg;
static unsigned long warm_fields;
static int unlocks_asked;
static const char *trap_replay = "rollback-trap.ggxreplay";

/* An instance's Core trapped, in a timeline the reference never ran: the same
   boot, warm-up and that timeline's inputs in a fourth Core, recorded as a
   replay that ends with the trapping field (ggx-headless --play reproduces
   it). */
static void write_trap_replay(const char *who, const Rollback *rb)
{
    static CoreHost hx;
    Session sx;
    Replay *rec;
    SessionInput in;
    uint32_t f, last = rb_last_run(rb);
    unsigned long i;
    char why[512];
    int trapped = 0;

    if (script || core_host_open(&hx, &host_cfg) != 0) {
        return;
    }
    session_init(&sx, &hx, unlocks_asked);
    session_detach_cards(&sx);
    if (!(rec = rp_record(&sx, unlocks_asked, NULL, 0)) || core_host_boot(&hx) != 0) {
        return;
    }
    for (i = 0; i < warm_fields && !trapped; i++) {
        mash_input(seed, hx.field, in.pads);
        in.unlock = 0;
        trapped = session_field(&sx, &in) != 0;
    }
    for (f = 0; f <= last && !trapped; f++) {
        rb_ran_with(rb, f, &in);
        trapped = session_field(&sx, &in) != 0;
    }
    if (!trapped) {
        printf("  %s's timeline run again in a fresh Core did not trap\n", who);
    } else if (rp_write(rec, trap_replay, why, sizeof(why)) == 0) {
        printf("  %s's timeline traps in a fresh Core too, at field %u; written as %s\n", who, hx.field, trap_replay);
    }
    rp_free(rec);
    core_host_close(&hx);
}

static int round_at(int delay, int max_rollback, uint32_t fields, SessionState *start_r, SessionState *start_a,
                    SessionState *join)
{
    Reference ref;
    RollbackConfig ca = { 0, delay, max_rollback }, cb = { 1, delay, max_rollback };
    Rollback *ra = NULL, *rbb = NULL;
    Wire to_a = { 0 }, to_b = { 0 };
    static Clock clock_a, clock_b;
    uint32_t checked_a = 0, checked_b = 0, pictures_seen = 0, pictures = 0, pictures_wrong = 0, pictures_first = 0;
    uint64_t tick = 0, card_a0, card_b0;
    double t0;
    int rc = -1;

    memset(&ref, 0, sizeof(ref));
    printf("delay %d, rollback up to %d fields, %u fields:\n", delay, max_rollback, fields);
    fflush(stdout);
    if (session_load(&sess_r, start_r) != 0 || session_load(&sess_a, start_a) != 0) {
        fail("the round's start state did not load");
        return -1;
    }
    if (script) {
        char err[512];
        PadInput skip[2];

        ggs_free(script);
        if (!(script = ggs_load(script_path, err, sizeof(err)))) {
            fail("%s", err);
            return -1;
        }
        ggs_play_to(script, host_r.vsyncs, skip);
    }
    /* B joins: A's state, whole, as another machine would receive it */
    if (session_state_copy(join, start_a) != 0) {
        fail("copying A's state for B");
        return -1;
    }
    t0 = seconds();
    if (session_load(&sess_b, join) != 0) {
        fail("B could not load A's state");
        return -1;
    }
    printf("  B joined from A's state at field %u: whole copy, checked, %.2f ms\n", host_b.field,
           (seconds() - t0) * 1e3);
    if (session_hash(&sess_b) != session_hash(&sess_a) || session_hash(&sess_a) != session_hash(&sess_r)) {
        fail("at the join: A %016llx, B %016llx, R %016llx", (unsigned long long)session_hash(&sess_a),
             (unsigned long long)session_hash(&sess_b), (unsigned long long)session_hash(&sess_r));
        return -1;
    }
    /* the join put B's cards in place: count what rollbacks put back from here */
    card_a0 = sess_a.card_restores;
    card_b0 = sess_b.card_restores;
    ref.base = host_r.field;
    ref.delay = delay;
    if (audio_a) {
        /* A's sound follows its rollbacks from here; R's plays as it always does */
        if (audio_host_follow(audio_a, &sess_a) != 0 || sound_watch(audio_a, &sound_a, host_a.field, fields) != 0 ||
            sound_watch(audio_r, &sound_r, host_r.field, fields) != 0) {
            fail("the sound layer");
            return -1;
        }
    }
    if (screen_a.s) {
        free(picture_r);
        picture_n = fields + RB_MAX_ROLLBACK + RB_MAX_DELAY + 4;
        if (!(picture_r = calloc(picture_n, sizeof(*picture_r)))) {
            return -1;
        }
        picture_base = host_a.field;
    }
    if (!(ra = rb_new(&sess_a, &ca)) || !(rbb = rb_new(&sess_b, &cb))) {
        fail("rb_new");
        goto out;
    }
    clock_a.save.what = "A: save";
    clock_a.load.what = "A: load (rollback)";
    clock_a.h = &host_a;
    clock_b.save.what = "B: save";
    clock_b.load.what = "B: load (rollback)";
    clock_b.h = &host_b;
    rb_set_clock(ra, seconds, on_timed, &clock_a);
    rb_set_clock(rbb, seconds, on_timed, &clock_b);
    t0 = seconds();
    while ((checked_a < fields || checked_b < fields) && !failures) {
        Rollback *who[2] = { ra, rbb };
        Wire *out[2] = { &to_b, &to_a };
        int p;

        for (p = 0; p < 2; p++) {
            Rollback *x = who[p];

            while (rb_local_next(x) <= rb_frame(x) + (uint32_t)delay) {
                uint32_t f = rb_local_next(x);

                if (reference_to(&ref, f) != 0) {
                    fail("the reference trapped at frame %u", f);
                    goto out;
                }
                rb_add_local(x, &ref.in[f].pads[p]);
                if (wire_send(out[p], tick + latency(tick, p, f, delay, max_rollback), f, &ref.in[f].pads[p]) != 0) {
                    goto out;
                }
            }
        }
        deliver(&to_a, tick, ra);
        deliver(&to_b, tick, rbb);
        if (rb_advance(ra) < 0) {
            fail("tick %llu: A's Core trapped running frame %u", (unsigned long long)tick, rb_last_run(ra));
            write_trap_replay("A", ra);
            goto out;
        }
        if (rb_advance(rbb) < 0) {
            fail("tick %llu: B's Core trapped running frame %u", (unsigned long long)tick, rb_last_run(rbb));
            write_trap_replay("B", rbb);
            goto out;
        }
        if (screen_a.s && rb_confirmed(ra) == rb_frame(ra) && rb_frame(ra) && rb_frame(ra) <= fields &&
            rb_frame(ra) != pictures_seen) {
            /* nothing predicted: A's last frame is the reference's */
            uint32_t f = rb_frame(ra) - 1;

            pictures_seen = rb_frame(ra);
            if (reference_to(&ref, f) != 0) {
                goto out;
            }
            pictures++;
            if (picture(&screen_a) != picture_r[f]) {
                pictures_wrong++;
                if (!pictures_first) {
                    pictures_first = picture_base + f + 1;
                }
            }
        }
        while (checked_a < rb_confirmed(ra) && checked_a < fields) {
            if (check_frame("A", ra, &ref, checked_a++) != 0) {
                goto out;
            }
        }
        while (checked_b < rb_confirmed(rbb) && checked_b < fields) {
            if (check_frame("B", rbb, &ref, checked_b++) != 0) {
                goto out;
            }
        }
        if (++tick > 20ull * fields + 1000) {
            fail("no progress after %llu ticks: A confirmed %u, B %u", (unsigned long long)tick, rb_confirmed(ra),
                 rb_confirmed(rbb));
            goto out;
        }
    }
    printf("  %u fields confirmed by both, every State Hash and session hash the reference's (%llu ticks, %.1f s)\n",
           fields, (unsigned long long)tick, seconds() - t0);
    sess_a.card_restores -= card_a0;
    sess_b.card_restores -= card_b0;
    print_stats("A", ra, &sess_a);
    print_stats("B", rbb, &sess_b);
    card_restores += sess_a.card_restores + sess_b.card_restores;
    sess_a.card_restores += card_a0;
    sess_b.card_restores += card_b0;
    report(&clock_a.save);
    report(&clock_a.load);
    report(&clock_b.save);
    report(&clock_b.load);
    report_slowest(&clock_a, "A");
    report_slowest(&clock_b, "B");
    if (audio_a) {
        sound_check(fields);
    }
    if (screen_a.s) {
        uint32_t f, changed = 0;

        for (f = 1; f < fields; f++) {
            changed += picture_r[f] != picture_r[f - 1];
        }
        printf("  pictures (A against R): %u frames shown with nothing predicted, %u of them other than R's%s", pictures,
               pictures_wrong, pictures_wrong ? " (the first after field " : "");
        if (pictures_wrong) {
            printf("%u)", pictures_first);
        }
        printf("; R's picture changed in %u of %u fields\n", changed, fields - 1);
    }
    if (rb_stats(ra)->depth_max < (uint32_t)max_rollback && rb_stats(rbb)->depth_max < (uint32_t)max_rollback) {
        fail("neither instance rolled back %d fields: the run didn't test that depth", max_rollback);
        goto out;
    }
    rc = 0;
out:
    rb_free(ra);
    rb_free(rbb);
    free(to_a.p);
    free(to_b.p);
    free(ref.in);
    free(ref.h);
    return rc;
}

/* every file in the store, by name and contents: the store must not change */
static uint64_t store_hash(const char *dir)
{
    static const char *const names[] = { "BKSLPM-64525GGX", "BISLPS-20137GGX" };
    uint64_t h = 0;
    int r, i;

    for (r = 0; r < 2; r++) {
        for (i = -1; i < 10; i++) {
            char name[64], path[1200];
            FILE *f;

            if (i < 0) {
                snprintf(name, sizeof(name), "%s", names[r]);
            } else {
                snprintf(name, sizeof(name), "%sRP%d", names[r], i);
            }
            if (save_store_path(path, sizeof(path), dir, name) != 0 || !(f = fs_open(path, "rb"))) {
                continue;
            }
            {
                uint8_t buf[65536];
                size_t n;
                XXH3_state_t x;

                XXH3_64bits_reset(&x);
                XXH3_64bits_update(&x, name, strlen(name));
                while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
                    XXH3_64bits_update(&x, buf, n);
                }
                h = mash_mix(h ^ XXH3_64bits_digest(&x));
            }
            fclose(f);
        }
    }
    return h;
}

static void usage(void)
{
    fprintf(stderr, "usage: ggx-rollback-test [--data-dir <dir> | --disc <image>] [--elf <boot elf>] [--warm N]\n"
                    "                         [--fields N] [--delays 0,1,2] [--max-rollback 8]\n"
                    "                         [--mash SEED | --script <file>] [--unlock all|none|<items>]\n"
                    "                         [--card1 store|empty|none] [--save-dir <dir>] [--audio] [--net SEED]\n"
                    "                         [--expect-save] [--render]\n");
    exit(64);
}

int main(int argc, char **argv)
{
    CoreHostConfig cfg = { 0 };
    char pref[1024], save_dir[1100];
    unsigned long warm = 3000, fields = 10000, i;
    const char *delays = "0,1,2", *p, *unlock_opt = "all";
    int max_rollback = RB_MAX_ROLLBACK, unlocks, a, audio = 0, expect_save = 0, render = 0;
    SessionState *start_r, *start_a, *join;
    uint64_t store0 = 0;

    cfg.cards[0] = CORE_CARD_EMPTY;
    for (a = 1; a < argc; a++) {
        if (!strcmp(argv[a], "--audio")) {
            audio = 1;
            continue;
        }
        if (!strcmp(argv[a], "--expect-save")) {
            expect_save = 1;
            continue;
        }
        if (!strcmp(argv[a], "--render")) {
            render = 1;
            continue;
        }
        if (a + 1 >= argc) {
            usage();
        }
        if (!strcmp(argv[a], "--disc")) {
            cfg.disc_path = argv[++a];
        } else if (!strcmp(argv[a], "--data-dir")) {
            cfg.data_dir = argv[++a];
        } else if (!strcmp(argv[a], "--elf")) {
            cfg.elf_path = argv[++a];
        } else if (!strcmp(argv[a], "--warm")) {
            warm = strtoul(argv[++a], NULL, 10);
        } else if (!strcmp(argv[a], "--fields")) {
            fields = strtoul(argv[++a], NULL, 10);
        } else if (!strcmp(argv[a], "--delays")) {
            delays = argv[++a];
        } else if (!strcmp(argv[a], "--max-rollback")) {
            max_rollback = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "--mash")) {
            seed = strtoull(argv[++a], NULL, 0);
        } else if (!strcmp(argv[a], "--script")) {
            script_path = argv[++a];
        } else if (!strcmp(argv[a], "--unlock")) {
            unlock_opt = argv[++a];
        } else if (!strcmp(argv[a], "--net")) {
            net_seed = strtoull(argv[++a], NULL, 0);
        } else if (!strcmp(argv[a], "--card1")) {
            cfg.cards[0] = core_card_kind(argv[++a]);
            if (cfg.cards[0] < 0) {
                usage();
            }
        } else if (!strcmp(argv[a], "--save-dir")) {
            cfg.save_dir = argv[++a];
        } else {
            usage();
        }
    }
    unlocks = unlock_parse(unlock_opt);
    if ((cfg.disc_path && cfg.data_dir) || !fields || max_rollback < 1 || max_rollback > RB_MAX_ROLLBACK ||
        unlocks < 0) {
        usage();
    }
    if (!cfg.disc_path && !cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            fprintf(stderr, "ggx-rollback-test: no user data directory: set HOME, or pass --data-dir\n");
            return 1;
        }
        cfg.data_dir = pref;
    }
    if (!cfg.save_dir) {
        if (save_store_dir(save_dir, sizeof(save_dir), cfg.data_dir) != 0) {
            return 1;
        }
        cfg.save_dir = save_dir;
    }
    if (script_path) {
        char err[512];

        if (!(script = ggs_load(script_path, err, sizeof(err)))) {
            fprintf(stderr, "ggx-rollback-test: %s\n", err);
            return 1;
        }
    }
    if (cfg.cards[0] == CORE_CARD_STORE) {
        store0 = store_hash(cfg.save_dir);
    }
    host_cfg = cfg;
    warm_fields = warm;
    unlocks_asked = unlocks;
    if (core_host_open(&host_r, &cfg) != 0 || core_host_open(&host_a, &cfg) != 0 ||
        core_host_open(&host_b, &cfg) != 0) {
        return 1;
    }
    session_init(&sess_r, &host_r, unlocks);
    session_init(&sess_a, &host_a, unlocks);
    session_init(&sess_b, &host_b, 0);
    session_detach_cards(&sess_r);
    session_detach_cards(&sess_a);
    session_detach_cards(&sess_b);
    if (audio) {
        AudioHostConfig ac = { NULL, NULL, 1 };

        if (strchr(delays, ',')) {
            fprintf(stderr, "ggx-rollback-test: --audio checks one input delay a run\n");
            return 64;
        }
        if (!(audio_a = audio_host_open(&host_a, &ac)) || !(audio_r = audio_host_open(&host_r, &ac))) {
            return 1;
        }
    }
    if (render && (screen_open(&screen_r, &host_r) != 0 || screen_open(&screen_a, &host_a) != 0)) {
        return 1;
    }
    if (core_host_boot(&host_r) != 0 || core_host_boot(&host_a) != 0) {
        return 2;
    }
    audio_host_field(audio_r, 0, 1);
    audio_host_field(audio_a, 0, 1);
    /* R and A boot apart and run the same fields: the same hashes */
    for (i = 0; i < warm; i++) {
        SessionInput in;

        real_input(host_r.field, &in);
        if (session_field(&sess_r, &in) != 0 || session_field(&sess_a, &in) != 0) {
            return 2;
        }
        audio_host_field(audio_r, host_r.field - 1, 1);
        audio_host_field(audio_a, host_a.field - 1, 1);
    }
    if (session_hash(&sess_r) != session_hash(&sess_a)) {
        fail("R and A booted and ran %lu fields apart and differ: %016llx, %016llx", warm,
             (unsigned long long)session_hash(&sess_r), (unsigned long long)session_hash(&sess_a));
        return 1;
    }
    printf("R and A booted apart and ran %lu fields: session hash %016llx, State Hash %016llx (unlocks: %s)\n", warm,
           (unsigned long long)session_hash(&sess_a), (unsigned long long)core_hash(&host_a), unlock_opt);
    start_r = session_state_new(&sess_r);
    start_a = session_state_new(&sess_a);
    join = session_state_new(&sess_b);
    if (!start_r || !start_a || !join || session_save(&sess_r, start_r) != 0 || session_save(&sess_a, start_a) != 0) {
        return 1;
    }
    for (p = delays; *p && !failures;) {
        int d = atoi(p);

        if (d < 0 || d > RB_MAX_DELAY) {
            usage();
        }
        round_at(d, max_rollback, (uint32_t)fields, start_r, start_a, join);
        p = strchr(p, ',');
        if (!p) {
            break;
        }
        p++;
    }
    if (cfg.cards[0] == CORE_CARD_STORE) {
        if (store_hash(cfg.save_dir) != store0) {
            fail("the native save store changed: a save under rollback reached it");
        } else {
            printf("the native save store is unchanged\n");
        }
    }
    printf("cards at the end: R %016llx (%d directories)\n", (unsigned long long)mc_card_hash(host_r.card[0]),
           mc_card_count(host_r.card[0]));
    if (expect_save) {
        static const char *sys =
#if defined(VERSION_KR)
            "BKSLPM-64525GGX";
#else
            "BISLPS-20137GGX";
#endif
        if (mc_card_find(host_r.card[0], sys) < 0) {
            fail("--expect-save: the game saved nothing (no %s on the card)", sys);
        } else if (!card_restores) {
            fail("--expect-save: no rollback crossed the save");
        } else {
            printf("the save was made under rollback: %s on the card, %llu rollbacks put a card back\n", sys,
                   (unsigned long long)card_restores);
        }
    }
    session_state_free(start_r);
    session_state_free(start_a);
    session_state_free(join);
    audio_host_close(audio_a);
    audio_host_close(audio_r);
    dl_stream_free(screen_a.s);
    dl_stream_free(screen_r.s);
    free(picture_r);
    core_host_close(&host_b);
    core_host_close(&host_a);
    core_host_close(&host_r);
    ggs_free(script);
    printf("%s\n", failures ? "FAILED" : "PASSED");
    return failures ? 1 : 0;
}
