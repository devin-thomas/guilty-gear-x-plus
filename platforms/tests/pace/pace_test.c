/* Display pacing (sdl/pace.c) on made-up displays: no SDL, window or disc.
 *
 * The display is simulated: vblanks every period from a random phase, and a
 * frame presented before a vblank is shown from that vblank on (a flip at
 * vblank). Each field's work takes 1-5 ms. Paced, every field's frame must be
 * shown at its own vblank, k refreshes after the last one's: none dropped,
 * none held longer. Then the loop as it runs on Windows: the vblanks known
 * only from a thread's late and jittery wake-ups (VGrid), frames presented
 * mailbox, the GPU finishing at times after the vblank: the field rate must
 * stay the display's, with no fields back to back. Then the two-buffer flip
 * queue: one late frame must not leave every later frame late. For contrast
 * the old 59.94 Hz clock on a 60.00 Hz display is simulated too, and drops
 * frames where the two clocks beat.
 *
 *   ggx-pace-test     (exit status 0 when every check passes)
 */
#include <stdio.h>
#include <stdlib.h>

#include "pace.h"

static int failures;

#define CHECK(c)                                                         \
    do {                                                                 \
        if (!(c)) {                                                      \
            fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c); \
            failures++;                                                  \
        }                                                                \
    } while (0)

#define FREQ 1000000000ull   /* the counter in nanoseconds */

static uint64_t rng = 0x9E3779B97F4A7C15ull;

static uint64_t rnd(uint64_t n)
{
    rng ^= rng << 13, rng ^= rng >> 7, rng ^= rng << 17;
    return rng % n;
}

/* the first vblank at or after t, and the last one at or before it */
static uint64_t vb_after(uint64_t t, uint64_t phase, uint64_t period)
{
    return t <= phase ? phase : phase + (t - phase + period - 1) / period * period;
}

static uint64_t vb_before(uint64_t t, uint64_t phase, uint64_t period)
{
    return phase + (t - phase) / period * period;
}

typedef struct {
    int fields, dropped, held_longer, late;
    double latency_ms;       /* field start to its vblank, the mean */
} Run;

/* n fields paced on a display at hz; long_at: a field whose work takes 30 ms */
static Run paced(double hz, int n, int long_at)
{
    Pace p = { 0 };
    uint64_t period = (uint64_t)(FREQ / hz + 0.5), phase = 1000000000ull + rnd(period), now = phase + rnd(period);
    uint64_t prev = 0;
    double lat = 0;
    Run r = { 0 };
    int i;

    pace_init(&p, hz, FREQ, 1);
    for (i = 0; i < n; i++) {
        uint64_t at = pace_start(&p, now, vb_before(now, phase, period), period), shown, work;

        uint64_t ready, hold, worked;

        now = at > now ? at + rnd(200000) : now;   /* the wake-up: up to 0.2 ms late */
        work = i == long_at ? 30000000ull : 1000000ull + rnd(4000000);
        /* presented when done, but not before pace_present_at */
        worked = ready = now + work;
        hold = pace_present_at(&p, period);
        ready = ready > hold ? ready : hold;
        shown = vb_after(ready, phase, period);
        if (prev) {
            uint64_t gap = (shown - prev + period / 2) / period;

            r.dropped += gap == 0;
            r.held_longer += gap > (uint64_t)p.k;
        }
        lat += (double)(shown - now) / 1e6;
        prev = shown;
        pace_done(&p, now, ready, ready - worked, period);
        now = ready;
    }
    r.fields = n;
    r.late = (int)p.late;
    r.latency_ms = lat / n;
    return r;
}

/* The loop as ggx runs it on Windows, against a display at hz: the vblanks
   are known only from a thread's wake-ups (true vblank + 0.05-1.5 ms, now and
   then 4 ms, now and then none), through the VGrid; a frame is presented
   mailbox, so it is shown at the first vblank after its GPU work is done and
   a newer frame replaces one still waiting. gpu_late: the GPU work ends this
   long after the present, 0 for 0.3 ms; stuck: a field whose work takes 30 ms.
   What must hold: one field a k refreshes, never two back to back. */
typedef struct {
    int fields;
    double fields_per_s;
    double min_gap_ms;          /* between field starts, after the first 100 */
    int shown_late;             /* frames shown after the vblank they were for */
    int replaced;               /* frames replaced before they were shown */
    int grid_ahead;             /* times the grid put a vblank after now */
} Loop;

static Loop windows_loop(double hz, int n, uint64_t gpu_late_lo, uint64_t gpu_late_hi, int stuck)
{
    Pace p = { 0 };
    VGrid g = { 0 };
    uint64_t period = (uint64_t)(FREQ / hz + 0.5), phase = 1000000000ull, now = phase;
    uint64_t next_vb = phase, next_wake = 0, pending_vb = 0, first = 0, prev_start = 0;
    double min_gap = 1e9;
    int i;
    Loop r = { 0 };

    pace_init(&p, hz, FREQ, 1);
    g.min_gap = period / 2;   /* as vblank_open sets it, from the display mode */
    /* the thread's first vblanks, before the window shows */
    for (i = 0; i < 12; i++, next_vb += period) {
        vgrid_seen(&g, next_vb + 50000 + rnd(1450000));
    }
    now = next_vb - period;
    for (i = 0; i < n; i++) {
        uint64_t vb, per, at, start, work, ready, worked, hold, done, shown;

        /* the thread's wake-ups up to now, and no later */
        for (;;) {
            if (!next_wake) {
                next_wake = next_vb + 50000 + (rnd(20) == 0 ? 4000000 : rnd(1450000));
            }
            if (next_wake > now) {
                break;
            }
            if (rnd(50)) {
                vgrid_seen(&g, next_wake);
            }
            next_vb += period;
            next_wake = 0;
        }
        if (!vgrid_at(&g, now, &vb, &per)) {
            CHECK(!"the grid went stale");
            break;
        }
        r.grid_ahead += vb > now;
        at = pace_start(&p, now, vb, per);
        start = at > now ? at + rnd(200000) : now;
        if (i >= 100) {
            double gap = (double)(start - prev_start) / 1e6;

            min_gap = gap < min_gap ? gap : min_gap;
        }
        if (i == 100) {
            first = start;
        }
        prev_start = start;
        work = i == stuck ? 30000000ull : 300000ull + rnd(1500000);
        ready = start + work;
        worked = ready;   /* the work's end, before any hold (ggx_main.c measures the same) */
        hold = pace_present_at(&p, per);
        ready = ready > hold ? ready : hold;
        done = ready + (gpu_late_hi ? gpu_late_lo + rnd(gpu_late_hi - gpu_late_lo) : 300000);
        shown = vb_after(done, phase, period);
        if (pending_vb && pending_vb >= shown) {
            r.replaced++;   /* the frame still waiting is replaced by this one */
        }
        /* later than the real vblank the frame was for */
        r.shown_late += shown > vb_after(p.target - period / 2, phase, period);
        pending_vb = shown;
        pace_done(&p, start, done, ready - worked, per);
        now = done;
    }
    r.fields = n;
    r.fields_per_s = (double)(n - 101) * 1e9 / (double)(prev_start - first);
    r.min_gap_ms = min_gap;
    return r;
}

/* The two buffers of a D3D12 swapchain: a frame's GPU work can't end before
   its buffer leaves the screen, which is when the last frame is flipped. A
   frame that misses its vblank (30 ms of GPU work) makes the next wait for that
   flip, so it misses its own, and so on for good. With watch the loop knows
   when each frame's GPU work ended (ggx_main.c waits for its fence) and lets a
   refresh pass; without it the loop knows only when it presented. */
typedef struct {
    int shown_late;       /* frames shown after the vblank they were for */
    int stuck_tail;       /* of those, in the last 1000 fields */
    uint32_t behind;
    double present_to_shown_ms;   /* over the last 1000 fields, the mean */
} Flip;

static Flip flip_queue(double hz, int n, int stall, int watch)
{
    Pace p = { 0 };
    uint64_t period = (uint64_t)(FREQ / hz + 0.5), phase = 1000000000ull, now = phase, last_shown = 0;
    double total = 0;
    int i;
    Flip r = { 0 };

    pace_init(&p, hz, FREQ, 1);
    for (i = 0; i < n; i++) {
        uint64_t at = pace_start(&p, now, vb_before(now, phase, period), period), start, cpu, done, shown, target;

        start = at > now ? at + rnd(200000) : now;
        target = p.target;
        cpu = start + 1000000ull + rnd(1000000);
        done = cpu + (i == stall ? 30000000ull : 300000ull);
        done = done > last_shown ? done : last_shown + 300000;   /* its buffer, off the screen */
        shown = vb_after(done, phase, period);
        r.shown_late += shown > target + period / 2;
        if (i >= n - 1000) {
            r.stuck_tail += shown > target + period / 2;
            total += (double)(shown - cpu) / 1e6;
        }
        pace_done(&p, start, watch ? done : cpu, 0, period);
        last_shown = shown;
        now = watch ? done : cpu;
    }
    r.behind = p.behind;
    r.present_to_shown_ms = total / 1000;
    return r;
}

/* the old clock: a field every 1/59.94 s, presented when done */
static Run clock_run(double hz, int n)
{
    uint64_t period = (uint64_t)(FREQ / hz + 0.5), field = (uint64_t)(FREQ / 59.94 + 0.5);
    uint64_t phase = 1000000000ull, start = phase + rnd(period), prev = 0;
    Run r = { 0 };
    int i;

    for (i = 0; i < n; i++, start += field) {
        uint64_t t = start + rnd(200000), shown = vb_after(t + 1000000ull + rnd(4000000), phase, period);

        if (prev) {
            r.dropped += shown == prev;
            r.held_longer += shown - prev > period + period / 2;
        }
        prev = shown;
    }
    r.fields = n;
    return r;
}

int main(void)
{
    Pace p = { 0 };
    Run r;
    size_t i;
    struct {
        double hz;
        int k;
    } rates[] = { { 60.0, 1 }, { 59.94, 1 }, { 60.2, 1 }, { 60.4, 0 }, { 119.88, 2 }, { 120.0, 2 }, { 240.0, 4 },
                  { 144.0, 0 }, { 165.0, 0 }, { 75.0, 0 }, { 100.0, 0 }, { 50.0, 0 }, { 0.0, 0 } };

    /* which displays are paced: a whole multiple of 59.94 Hz within 0.5% */
    for (i = 0; i < sizeof(rates) / sizeof(rates[0]); i++) {
        pace_init(&p, rates[i].hz, FREQ, 1);
        CHECK(p.k == rates[i].k);
        if (p.k != rates[i].k) {
            fprintf(stderr, "  %.2f Hz: k %d, want %d\n", rates[i].hz, p.k, rates[i].k);
        }
    }
    pace_init(&p, 60.0, FREQ, 0);
    CHECK(p.k == 0);

    /* paced: every frame at its own vblank, k refreshes apart */
    for (i = 0; i < 5; i++) {
        double hz = i == 0 ? 60.0 : i == 1 ? 59.94 : i == 2 ? 119.88 : i == 3 ? 120.0 : 240.0;

        r = paced(hz, 20000, -1);
        printf("paced at %6.2f Hz: %d fields, %d dropped, %d held longer, %d late, %.1f ms from a field's start to "
               "its vblank\n", hz, r.fields, r.dropped, r.held_longer, r.late, r.latency_ms);
        CHECK(r.dropped == 0 && r.held_longer == 0 && r.late == 0);
        CHECK(r.latency_ms < 14.0);
    }
    /* a field that takes 30 ms: it misses its vblank, the next starts at the
       first it can make, and the pacing goes on */
    r = paced(60.0, 2000, 1000);
    printf("paced at  60.00 Hz with one 30 ms field: %d dropped, %d held longer, %d late\n", r.dropped,
           r.held_longer, r.late);
    CHECK(r.dropped == 0 && r.held_longer <= 2 && r.late <= 2);

    /* the loop as on Windows: the vblanks only through the VGrid, presented
       mailbox. The field rate must be the display's, with no fields back to
       back, whatever the grid's error and however late the GPU finishes */
    {
        struct {
            double hz;
            uint64_t lo, hi;
            int stuck;
            const char *what;
        } cases[] = {
            { 60.0, 0, 0, -1, "GPU done 0.3 ms after the present" },
            { 60.0, 2000000, 6000000, -1, "GPU done 2-6 ms after the present (past the vblank)" },
            { 60.0, 0, 0, 3000, "one 30 ms field" },
            { 119.88, 0, 0, -1, "GPU done 0.3 ms after the present" },
            { 119.88, 2000000, 6000000, -1, "GPU done 2-6 ms after the present" },
        };

        for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
            Loop l = windows_loop(cases[i].hz, 12000, cases[i].lo, cases[i].hi, cases[i].stuck);
            double want = cases[i].hz / (cases[i].hz > 100 ? 2 : 1);

            printf("loop at %6.2f Hz, %s: %.3f fields/s, the shortest gap %.2f ms, %d shown late, %d replaced, "
                   "the grid ahead of now %d times\n", cases[i].hz, cases[i].what, l.fields_per_s, l.min_gap_ms,
                   l.shown_late, l.replaced, l.grid_ahead);
            CHECK(l.fields_per_s > want * 0.999 && l.fields_per_s < want * 1.001);
            CHECK(l.grid_ahead == 0);
            if (cases[i].stuck < 0) {
                CHECK(l.min_gap_ms > 0.9 * 1000.0 / want);
            } else {
                CHECK(l.min_gap_ms > 0.4 * 1000.0 / want);   /* the field after a long one may start early in its refresh */
            }
            if (!cases[i].lo) {
                CHECK(l.shown_late <= (cases[i].stuck >= 0 ? 2 : 0) && l.replaced == 0);
            }
        }
    }

    /* a frame a vblank late doesn't leave every later frame a vblank late */
    for (i = 0; i < 2; i++) {
        double hz = i ? 119.88 : 60.0;
        Flip w = flip_queue(hz, 6000, 3000, 1), u = flip_queue(hz, 6000, 3000, 0);

        printf("the flip queue at %6.2f Hz, one frame's GPU work 30 ms: watching the fence %d shown late (%u behind), %d in the "
               "last 1000, %.1f ms present to display; not watching %d shown late, %d in the last 1000, %.1f ms\n", hz,
               w.shown_late, w.behind, w.stuck_tail, w.present_to_shown_ms, u.shown_late, u.stuck_tail,
               u.present_to_shown_ms);
        CHECK(w.shown_late <= 2 && w.stuck_tail == 0 && w.behind >= 1 && w.behind <= 2);
        CHECK(w.present_to_shown_ms < 1000.0 / hz);
        if (i == 0) {
            CHECK(u.stuck_tail == 1000);   /* the model does get stuck without the fence */
        }
    }

    /* a machine that can't make its vblanks goes back to the clock, which
       keeps time by dropping frames, rather than running slow */
    {
        Pace q = { 0 };
        uint64_t period = (uint64_t)(FREQ / 60.0 + 0.5), now = 1000000000ull;
        const char *why;
        int f;

        pace_init(&q, 60.0, FREQ, 1);
        for (f = 0; f < 200; f++) {
            uint64_t at = pace_start(&q, now, now - now % period, period), start = at > now ? at : now;

            now = start + 20000000ull;
            pace_done(&q, start, now, 0, period);
        }
        why = pace_sane(&q, period);
        printf("every frame 20 ms: pace_sane: %s\n", why ? why : "(nothing)");
        CHECK(why != NULL);
    }

    /* the grid from wake-ups: never ahead of the latest wake-up, close to the
       real vblanks, and its period to a part in 10,000 */
    {
        VGrid g = { 0 };
        uint64_t period = 16683350, vb = 5000000000ull, at, per, worst = 0;
        int ahead = 0, k;

        for (k = 0; k < 20000; k++, vb += period) {
            uint64_t wake = vb + 50000 + (rnd(20) == 0 ? 4000000 : rnd(1450000));

            if (rnd(50)) {
                vgrid_seen(&g, wake);
            }
            if (k > 50 && vgrid_at(&g, wake, &at, &per)) {
                /* the phase error, around the period */
                uint64_t d = (at > vb ? at - vb : vb - at) % period, err = d < period - d ? d : period - d;

                ahead += at > wake;
                worst = k > 200 && err > worst ? err : worst;
            }
        }
        printf("the grid from wake-ups: %d times ahead of a wake-up, the worst phase error %.2f ms, the period %.4f "
               "ms (true %.4f)\n", ahead, worst / 1e6, per / 1e6, period / 1e6);
        CHECK(ahead == 0 && worst < 2000000 && per > period - period / 10000 && per < period + period / 10000);
    }

    /* the invariant: whatever the vblanks say, two fields never start closer
       than 0.9 of a field by the display mode's rate, and pace_sane says why
       the vblanks can't be trusted. A grid a third of the real period (as if
       WaitForVBlank returned three times a vblank) */
    {
        Pace q = { 0 };
        uint64_t period = (uint64_t)(FREQ / 60.0 + 0.5), bad = period / 3, now = 1000000000ull, prev = 0;
        uint64_t first = 0, start = 0;
        double min_gap = 1e9;
        const char *why;
        int f;

        pace_init(&q, 60.0, FREQ, 1);
        for (f = 0; f < 3000; f++) {
            uint64_t at = pace_start(&q, now, now - now % bad, bad);

            start = at > now ? at : now;
            if (prev) {
                double gap = (double)(start - prev) / 1e6;

                min_gap = gap < min_gap ? gap : min_gap;
            } else {
                first = start;
            }
            prev = start;
            pace_done(&q, start, start + 500000, 0, bad);
            now = start + 500000;
        }
        why = pace_sane(&q, bad);
        printf("vblanks at 3x the real rate: %.3f fields/s, the shortest gap %.2f ms, %u late of 3000; pace_sane: %s\n",
               2999.0 * 1e9 / (double)(start - first), min_gap, q.late, why ? why : "(nothing)");
        CHECK(min_gap >= 0.9 * 1000.0 / 60.0 - 0.01 && 2999.0 * 1e9 / (double)(start - first) <= 60.0 * 1.001);
        CHECK(why != NULL && q.late <= 3000);
    }

    /* the grid from wake-ups that WaitForVBlank might give: a second return
       for one vblank 0.1-3 ms after the first, 3 in 10 vblanks; or the other
       display's vblanks (a 119.88 Hz TV on the same adapter) mixed in. The
       grid ignores wake-ups less than half a refresh apart; whatever is left,
       its period is right, or pace_sane says it isn't */
    {
        uint64_t period = 16666667, tv = 8341675;
        int mode;

        for (mode = 0; mode < 2; mode++) {
            VGrid g = { 0 };
            Pace q = { 0 };
            uint64_t vb = 2000000000ull, tvb = 2000000000ull + 3000000, at, per = 0;
            const char *why;
            int k;

            g.min_gap = period / 2;
            pace_init(&q, 60.0, FREQ, 1);
            for (k = 0; k < 6000; k++, vb += period) {
                uint64_t wake = vb + 50000 + rnd(1450000);

                if (mode == 1) {
                    while (tvb < wake) {
                        vgrid_seen(&g, tvb + 50000 + rnd(500000));
                        tvb += tv;
                    }
                }
                vgrid_seen(&g, wake);
                if (mode == 0 && rnd(10) < 3) {
                    vgrid_seen(&g, wake + 100000 + rnd(2900000));
                }
            }
            vgrid_at(&g, vb, &at, &per);
            why = pace_sane(&q, per);
            printf("the grid with %s: the period %.4f ms (true %.4f); pace_sane: %s\n",
                   mode ? "a 119.88 Hz display's vblanks mixed in" : "second wake-ups", per / 1e6, period / 1e6,
                   why ? why : "(nothing)");
            if (mode == 0) {
                CHECK(per > period - period / 1000 && per < period + period / 1000 && !why);
            } else {
                CHECK((per > period - period / 100 && per < period + period / 100) || why);
            }
        }
    }

    /* the old clock on a 60.00 Hz display: frames lost where the clocks beat */
    r = clock_run(60.0, 20000);
    printf("the 59.94 Hz clock at 60.00 Hz: %d fields, %d never shown, %d held longer\n", r.fields, r.dropped,
           r.held_longer);
    CHECK(r.dropped > 0);

    printf("pace: %s (%d failed)\n", failures ? "FAIL" : "ok", failures);
    return failures ? 1 : 0;
}
