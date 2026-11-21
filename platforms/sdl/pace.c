/* Display pacing (pace.h). */
#include "pace.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the lead: the 99th percentile of the recent fields' work, plus this much,
   and never less than the floor; until 16 fields have been measured, the
   first. The floor is what the work's outliers need, not its usual 2-3 ms: in
   a fight on one test PC 1 frame in 500 takes 11-16 ms, mostly creating textures
   for new sprites, and a frame that misses its vblank is a repeated frame.
   Of a full Arcade run's 55,600 frames, 123 took longer than 11 ms, 55 longer
   than 13 ms and 37 longer than a refresh, which no lead can save. */
#define LEAD_MARGIN_US 2000
#define LEAD_FLOOR_US 13000
#define LEAD_FIRST_US 13000

void pace_init(Pace *p, double hz, uint64_t freq, int allow)
{
    uint64_t fixed = p->fixed_lead;
    double k = hz / PACE_FIELD_HZ, n = floor(k + 0.5);

    memset(p, 0, sizeof(*p));
    p->fixed_lead = fixed;
    p->hz = hz;
    p->freq = freq;
    p->k = allow && n >= 1 && fabs(k / n - 1.0) <= 0.005 ? (int)n : 0;
    p->lead = fixed ? fixed : freq * LEAD_FIRST_US / 1000000;
    p->nominal = hz > 0 ? (uint64_t)((double)freq / hz + 0.5) : 0;
}

/* The vblank on the grid of vblank + n * period nearest to t. */
static uint64_t snap(uint64_t t, uint64_t vblank, uint64_t period)
{
    int64_t n = (int64_t)floor(((double)t - (double)vblank) / (double)period + 0.5);

    return (uint64_t)((int64_t)vblank + n * (int64_t)period);
}

/* The first vblank on the grid at or after t (the grid's vblank may be on
   either side of t). */
static uint64_t ceil_vb(uint64_t t, uint64_t vblank, uint64_t period)
{
    return t <= vblank ? vblank - (vblank - t) / period * period
                       : vblank + (t - vblank + period - 1) / period * period;
}

uint64_t pace_start(Pace *p, uint64_t now, uint64_t vblank, uint64_t period)
{
    if (p->target) {
        p->target = snap(p->target, vblank, period);
    }
    /* late: half the lead gone (a wake-up a little late is still in time) */
    if (!p->target || p->retarget || now + p->lead / 2 > p->target) {
        uint64_t t = ceil_vb(now + p->lead, vblank, period);

        if (p->target) {
            /* the field before missed its vblank and shows at the next one
               after it was presented: this one comes k refreshes after that */
            uint64_t after = ceil_vb(p->last_done, vblank, period) + (uint64_t)p->k * period;

            t = t > after ? t : after;
        }
        p->target = t;
        p->retarget = 0;
    }
    if (p->last_start && p->nominal) {
        /* never two fields closer than 0.9 of a field by the mode's rate */
        uint64_t earliest = p->last_start + p->nominal * (uint64_t)p->k * 9 / 10;

        if (p->target - p->lead < earliest) {
            p->target = ceil_vb(earliest + p->lead, vblank, period);
        }
    }
    return p->target - p->lead;
}

const char *pace_sane(const Pace *p, uint64_t period)
{
    static char why[160];

    if (p->nominal && (period > p->nominal + p->nominal / 100 || period < p->nominal - p->nominal / 100)) {
        snprintf(why, sizeof(why), "the vblanks come every %.3f ms, where the display mode says %.3f ms",
                 (double)period * 1000.0 / (double)p->freq, (double)p->nominal * 1000.0 / (double)p->freq);
        return why;
    }
    if (p->nfields >= 120 && p->nlate_recent > 6) {
        snprintf(why, sizeof(why), "%d of the last 120 fields started late", p->nlate_recent);
        return why;
    }
    if (p->nfields >= 120 && p->nbehind_recent > 30) {
        snprintf(why, sizeof(why), "%d of the last 120 frames missed their vblank", p->nbehind_recent);
        return why;
    }
    return NULL;
}

uint64_t pace_present_at(const Pace *p, uint64_t period)
{
    return p->target > period ? p->target - period + period / 16 : 0;
}

static int cmp64(const void *a, const void *b)
{
    uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b;

    return x < y ? -1 : x > y;
}

void pace_done(Pace *p, uint64_t start, uint64_t done, uint64_t hold, uint64_t period)
{
    uint64_t worked = done - hold;

    p->work[p->iwork] = worked > start ? worked - start : 0;
    p->iwork = (p->iwork + 1) % PACE_WORK_N;
    if (p->nwork < PACE_WORK_N) {
        p->nwork++;
    }
    if (!p->fixed_lead && p->nwork >= 16) {
        uint64_t w[PACE_WORK_N], lead, floor_ = p->freq * LEAD_FLOOR_US / 1000000;

        memcpy(w, p->work, sizeof(uint64_t) * (size_t)p->nwork);
        qsort(w, (size_t)p->nwork, sizeof(uint64_t), cmp64);
        lead = w[p->nwork * 99 / 100] + p->freq * LEAD_MARGIN_US / 1000000;
        lead = lead < floor_ ? floor_ : lead;
        /* never more than most of a field (k refreshes): past that it would
           start before the last field's frame was shown */
        p->lead = lead > period * (uint64_t)p->k * 7 / 8 ? period * (uint64_t)p->k * 7 / 8 : lead;
    }
    {
        /* late: it started with less than half its lead left */
        int late = start + p->lead / 2 > p->target, behind = done > p->target;

        p->late += (uint32_t)late;
        p->nlate_recent += late - p->late_recent[p->ilate];
        p->late_recent[p->ilate] = (uint8_t)late;
        p->behind += (uint32_t)behind;
        p->nbehind_recent += behind - p->behind_recent[p->ilate];
        p->behind_recent[p->ilate] = (uint8_t)behind;
        p->retarget = behind;
        p->ilate = (p->ilate + 1) % 120;
        p->nfields++;
    }
    p->last_start = start;
    p->last_done = done;
    p->target += (uint64_t)p->k * period;
}

static void vgrid_fit(VGrid *g, uint64_t t)
{
    double x = floor((double)(t - g->t0) / (double)g->period + 0.5), y = (double)(t - g->t0), d;

    g->n += 1, g->sx += x, g->sy += y, g->sxx += x * x, g->sxy += x * y;
    d = g->n * g->sxx - g->sx * g->sx;
    if (g->n >= 8 && d > 0) {
        g->period = (uint64_t)((g->n * g->sxy - g->sx * g->sy) / d + 0.5);
    }
}

void vgrid_seen(VGrid *g, uint64_t t)
{
    int i;

    if (g->period == 0) {
        if (g->nfirst && g->min_gap && t - g->first[g->nfirst - 1] < g->min_gap) {
            return;
        }
        g->first[g->nfirst++] = t;
        if (g->nfirst < 8) {
            return;
        } else {
            uint64_t d[7];

            for (i = 0; i < 7; i++) {
                d[i] = g->first[i + 1] - g->first[i];
            }
            qsort(d, 7, sizeof(uint64_t), cmp64);
            /* to number the first vblanks: the display mode's rate where it
               is known (a median of jittery gaps can be off by enough to
               misnumber them, and the fit then runs away), else the median gap */
            g->period = g->min_gap ? g->min_gap * 2 : d[3];
            g->t0 = g->first[0];
            for (i = 0; i < 7; i++) {
                vgrid_fit(g, g->first[i]);
                g->wake[g->iwake++] = g->first[i];
                g->nwake++;
            }
        }
    } else if (t <= g->latest || (g->min_gap && t - g->latest < g->min_gap)) {
        return;   /* a second wake-up for one vblank */
    }
    vgrid_fit(g, t);
    g->latest = t;
    g->wake[g->iwake] = t;
    g->iwake = (g->iwake + 1) % VGRID_N;
    if (g->nwake < VGRID_N) {
        g->nwake++;
    }
    {
        /* each wake-up put back on the grid at the latest one: the earliest */
        uint64_t best = t;

        for (i = 0; i < g->nwake; i++) {
            uint64_t w = g->wake[i], n = (t - w + g->period / 2) / g->period, on = w + n * g->period;

            best = on < best ? on : best;
        }
        g->last = best;
    }
}

int vgrid_at(const VGrid *g, uint64_t now, uint64_t *vblank, uint64_t *period)
{
    if (g->period == 0 || (now > g->latest && now - g->latest > 8 * g->period)) {
        return 0;   /* not known yet, or no vblank for a while (a display asleep) */
    }
    *period = g->period;
    *vblank = now >= g->last ? g->last + (now - g->last) / g->period * g->period
                             : g->last - (g->last - now + g->period - 1) / g->period * g->period;
    return 1;
}
