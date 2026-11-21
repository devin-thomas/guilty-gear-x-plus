/* ggx-state-test: save/load, rollback and the State Hash, checked against
 * themselves (PORT-004).
 *
 *   ggx-state-test [--data-dir <dir> | --disc <image>] [--elf <boot elf>] [--warm N]
 *                  [--fields N] [--ring N] [--mash SEED] [--scan]
 *
 * The game boots from the extracted assets in the user data directory (or
 * --data-dir), or with --disc straight from an image, as in ggx-headless.
 *
 * 1. Boots the game and runs --warm fields (default 3000).
 * 2. Saves snapshot S0, then runs --fields fields (default 10,000),
 *    recording the State Hash after each. Meanwhile it rolls back the way
 *    a rollback netcode would: it saves every field into a ring of --ring
 *    snapshots (default 8) and every seventh field loads one 1 to ring-1
 *    fields back and simulates forward again, checking every hash on the
 *    way against the recording.
 * 3. Loads S0 and replays all the fields, checking each hash.
 * 4. Copies S0's bytes into a new snapshot, as if read from a file, loads
 *    that (the full copy, with its State Hash checked) and replays 600
 *    fields. A copy with one bit flipped must be refused.
 * 5. Reports how long each kind of save and load took, the rollback ones
 *    again for the fields spent fighting (GameMain).
 *
 * With --mash, both pads mash buttons (mash.h) instead of sitting idle, so
 * the game leaves its attract loop for menus and fights. With --scan
 * (needs a GGX_STATE_SCAN build), any page outside the state regions the
 * Core wrote or read fails the run (core_state_scan). Exit status 0 if
 * every check passed.
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_host.h"
#include "fsys.h"
#include "mash.h"

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

static CoreHost host;
static uint64_t seed;
static int mash, scan, failures;

/* timings in microseconds, by kind */
typedef struct {
    const char *what;
    double *us;
    size_t n, cap;
} Timing;

static void timed(Timing *t, double us)
{
    if (t->n == t->cap) {
        t->cap = t->cap ? 2 * t->cap : 1024;
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
    double sum = 0;
    size_t i;

    if (!t->n) {
        return;
    }
    qsort(t->us, t->n, sizeof(*t->us), cmp_double);
    for (i = 0; i < t->n; i++) {
        sum += t->us[i];
    }
    printf("  %-34s n=%-6zu median %8.1f us  mean %8.1f  p99 %8.1f  max %8.1f\n", t->what, t->n,
           t->us[t->n / 2], sum / t->n, t->us[(t->n * 99) / 100], t->us[t->n - 1]);
}

static int step(void)
{
    if (mash) {
        mash_input(seed, host.field, host.pads);
    }
    return core_host_field(&host);
}

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
    failures++;
}

static void scan_now(const char *when)
{
    if (scan && core_state_scan(&host, when) != 0) {
        fail("field %u: Core state outside the regions", host.field);
    }
}

static double save(CoreSnapshot *s, Timing *t)
{
    double t0 = seconds(), us;

    if (core_save(&host, s) != 0) {
        fail("field %u: core_save failed", host.field);
    }
    us = (seconds() - t0) * 1e6;
    if (t) {
        timed(t, us);
    }
    return us;
}

static double load(CoreSnapshot *s, Timing *t)
{
    double t0 = seconds(), us;
    int rc = core_load(&host, s);

    if (rc != 0) {
        fail("field %u: core_load refused the snapshot: %s", host.field, core_load_error(rc));
    }
    us = (seconds() - t0) * 1e6;
    if (t) {
        timed(t, us);
    }
    return us;
}

/* in a fight: the game's job is GameMain */
static int fighting(void)
{
    uint32_t job = ee_r32(&host, ggx_sym_JobMode);

    return !strcmp(ggx_func_name(ee_r32(&host, ggx_sym_jobfunc + 4 * job)), "GameMain");
}

/* Replays from the current state to field `to`, checking each field's hash
   against the recording (hashes[f - base]). */
static void replay(const uint64_t *hashes, uint32_t base, uint32_t to, const char *what)
{
    while (host.field < to) {
        uint64_t got;

        if (step() != 0) {
            fail("%s: the Core trapped at field %u", what, host.field);
            return;
        }
        got = core_hash(&host);
        if (got != hashes[host.field - base]) {
            printf("FAIL %s: field %u hash %016llx, recorded %016llx\n", what, host.field,
                   (unsigned long long)got, (unsigned long long)hashes[host.field - base]);
            failures++;
            return;
        }
    }
}

static void usage(void)
{
    fprintf(stderr, "usage: ggx-state-test [--data-dir <dir> | --disc <image>] [--elf <boot elf>] [--warm N]\n"
                    "                      [--fields N] [--ring N] [--mash SEED] [--scan]\n");
    exit(64);
}

int main(int argc, char **argv)
{
    CoreHostConfig cfg = { 0 };
    char pref[1024];
    unsigned long warm = 3000, fields = 10000, ring_n = 8, i;
    CoreSnapshot *s0, *s1, **ring;
    uint64_t *hashes;
    uint32_t base;
    Timing t_first = { "save, first (whole)", 0, 0, 0 }, t_ring = { "save, rollback ring", 0, 0, 0 },
           t_roll = { "load, rollback 1-7 fields", 0, 0, 0 }, t_far = { "load, back 10,000 fields", 0, 0, 0 },
           t_foreign = { "load, foreign (whole, checked)", 0, 0, 0 }, t_hash = { "core_hash after a field", 0, 0, 0 },
           t_ring_fight = { "save, rollback ring, in fights", 0, 0, 0 },
           t_roll_fight = { "load, rollback, in fights", 0, 0, 0 };
    double t0;
    int a;

    cfg.cards[0] = 1;
    for (a = 1; a < argc; a++) {
        if (!strcmp(argv[a], "--scan")) {
            scan = 1;
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
        } else if (!strcmp(argv[a], "--ring")) {
            ring_n = strtoul(argv[++a], NULL, 10);
        } else if (!strcmp(argv[a], "--mash")) {
            mash = 1;
            seed = strtoull(argv[++a], NULL, 0);
        } else {
            usage();
        }
    }
    if ((cfg.disc_path && cfg.data_dir) || ring_n < 2 || !fields) {
        usage();
    }
    if (!cfg.disc_path && !cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            fprintf(stderr, "ggx-state-test: no user data directory: set HOME, or pass --data-dir\n");
            return 1;
        }
        cfg.data_dir = pref;
    }
#ifndef GGX_STATE_SCAN
    if (scan) {
        fprintf(stderr, "ggx-state-test: --scan needs a build configured with -DGGX_STATE_SCAN=ON\n");
        return 64;
    }
#endif
    if (core_host_open(&host, &cfg) != 0) {
        return 1;
    }
    for (i = 0; i < 2; i++) {
        memset(host.pads[i].analog, 0x80, sizeof(host.pads[i].analog));
    }
    {
        const CoreRegion *g = core_regions(&host);

        printf("regions:");
        for (a = 0; a < CORE_REGION_COUNT; a++) {
            printf(" %s 0x%08x+0x%x", g[a].name, g[a].base, g[a].size);
        }
        printf("\nsnapshot: %zu bytes\n", core_state_size(&host));
    }
    if (core_host_boot(&host) != 0) {
        return 2;
    }
    scan_now("boot");
    for (i = 0; i < warm; i++) {
        if (step() != 0) {
            return 2;
        }
    }
    scan_now("warm-up");

    /* 2: the recording, with rollbacks */
    s0 = core_snapshot_new(&host);
    s1 = core_snapshot_new(&host);
    ring = calloc(ring_n, sizeof(*ring));
    hashes = malloc((fields + 1) * sizeof(*hashes));
    if (!s0 || !s1 || !ring || !hashes) {
        return 1;
    }
    for (i = 0; i < ring_n; i++) {
        ring[i] = core_snapshot_new(&host);
        if (!ring[i]) {
            return 1;
        }
    }
    save(s0, &t_first);
    base = host.field;
    hashes[0] = core_hash(&host);
    if (core_snapshot_hash(s0) != hashes[0]) {
        fail("field %u: S0 records %016llx, core_hash %016llx", host.field, (unsigned long long)core_snapshot_hash(s0),
             (unsigned long long)hashes[0]);
    }
    printf("S0 at field %u: %016llx\n", base, (unsigned long long)hashes[0]);
    for (i = 0; i < ring_n; i++) {
        save(ring[i], &t_first);
    }
    while (host.field < base + fields && !failures) {
        uint32_t f;

        if (step() != 0) {
            fail("field %u: the Core trapped", host.field);
            break;
        }
        f = host.field;
        t0 = seconds();
        hashes[f - base] = core_hash(&host);
        timed(&t_hash, (seconds() - t0) * 1e6);
        t0 = save(ring[f % ring_n], &t_ring);
        if (fighting()) {
            timed(&t_ring_fight, t0);
        }
        if (f % 7 == 0 && f - base >= ring_n) {
            uint32_t back = 1 + (f / 7) % (uint32_t)(ring_n - 1), to = f;

            t0 = load(ring[(f - back) % ring_n], &t_roll);
            if (fighting()) {
                timed(&t_roll_fight, t0);
            }
            if (host.field != f - back || core_hash(&host) != hashes[f - back - base]) {
                fail("field %u: rolling back %u fields restored field %u with another hash", f, back, host.field);
                break;
            }
            while (host.field < to && !failures) {
                replay(hashes, base, host.field + 1, "rollback");
                t0 = save(ring[host.field % ring_n], &t_ring);
                if (fighting()) {
                    timed(&t_ring_fight, t0);
                }
            }
        }
    }
    printf("recorded fields %u-%u: %016llx at the end\n", base + 1, host.field,
           (unsigned long long)hashes[host.field - base]);
    scan_now("recording");

    /* 3: back to S0, and the whole recording again */
    if (!failures) {
        load(s0, &t_far);
        if (host.field != base || core_hash(&host) != hashes[0]) {
            fail("S0: loaded field %u, hash %016llx, recorded %016llx", host.field,
                 (unsigned long long)core_hash(&host), (unsigned long long)hashes[0]);
        } else {
            replay(hashes, base, base + fields, "replay from S0");
            if (!failures) {
                printf("replay from S0: %lu fields, every hash identical\n", fields);
            }
        }
        scan_now("replay");
    }

    /* 4: S0 as a foreign snapshot */
    if (!failures) {
        unsigned long n = fields < 600 ? fields : 600;

        if (core_snapshot_set_data(s1, core_snapshot_data(s0), core_state_size(&host)) != 0) {
            fail("S1: set_data refused S0's bytes");
        }
        load(s1, &t_foreign);
        replay(hashes, base, base + n, "replay from a copy of S0");
        if (!failures) {
            printf("replay from a copy of S0: %lu fields, every hash identical\n", n);
        }
        /* a damaged copy is refused, and the Core is left alone */
        {
            uint8_t *bad = malloc(core_state_size(&host));
            uint64_t before = core_hash(&host);

            memcpy(bad, core_snapshot_data(s0), core_state_size(&host));
            bad[core_state_size(&host) / 2] ^= 1;
            core_snapshot_set_data(s1, bad, core_state_size(&host));
            if (core_load(&host, s1) == 0 || core_hash(&host) != before) {
                fail("a damaged snapshot was loaded");
            } else {
                printf("a damaged copy of S0 is refused\n");
            }
            free(bad);
        }
        scan_now("foreign load");
    }

    printf("timings (%s):\n", mash ? "mashing" : "no input");
    report(&t_first);
    report(&t_ring);
    report(&t_roll);
    report(&t_ring_fight);
    report(&t_roll_fight);
    report(&t_far);
    report(&t_foreign);
    report(&t_hash);
    printf("%s\n", failures ? "FAILED" : "PASSED");
    return failures ? 1 : 0;
}
