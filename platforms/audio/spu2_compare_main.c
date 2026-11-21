/* ggx-spu2-compare: the port's sound driver against the game's on PCSX2, at the SPU2 (PORT-011, AUDIO.md,
 * "SPU2 lockstep", end to end).
 *
 *   ggx-spu2-compare <pcsx2 tap> <port tap> [--json <out>] [--max-report N]
 *
 * Both taps hold the same recorded command stream played by a sound driver: PCSX2's by the game's SNDRV.IRX
 * and LIBSD.IRX on its emulated IOP (with the BIOS before them), the port's by platforms/audio/sndrv.c
 * (ggx-snd render --tap). Each is replayed into a model of its own, which turns its writes into what they
 * do: a register taking a new value (a write of the value it holds does nothing, so the IOP's loops that
 * write the same values over and over count once), voices keyed on and off when the next sample applies
 * the latches (a key off counts for the voices it releases), and transfers into sound RAM, runs of words
 * merged into one, with what they wrote. Both taps run from the SPU2's power-on. The comparison starts at the
 * driver's first command: the port's tap marks it, and in PCSX2's it is where the port's first writes after
 * the mark appear. (Before it, PCSX2 has the BIOS and LIBSD's sceSdInit, which the port does in fewer writes:
 * the report counts them.) PCSX2's ticks are shifted to match, up to where the port's stream ends.
 *
 * Then, for each register, the sequence of values it took is compared, and the first that differs is
 * named; so are the key-on and key-off sequences and the transfers. For the events that match, the report
 * gives how far apart their ticks are: the drivers' timing, which the port's IOP model sets (sndrv.c),
 * is reported rather than required. Last, the samples the two produced. Exit status 0 when the values
 * all match, 1 when some differ, 2 if a tap can't be read.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spu2.h"
#include "spu2_tap.h"

/* what a write or a mix did */
enum { EV_REG, EV_KON, EV_KOFF, EV_RAM };

typedef struct {
    uint32_t tick;
    uint8_t kind;
    uint16_t addr;   /* EV_REG: the register; EV_KON, EV_KOFF: the core */
    uint32_t value;  /* EV_REG: the value; EV_KON, EV_KOFF: the voices; EV_RAM: the start word */
    uint32_t words, hash, last_tick;   /* EV_RAM */
} Event;

typedef struct {
    Event *ev;
    size_t n, cap;
} Events;

static void push(Events *e, const Event *x)
{
    if (e->n == e->cap) {
        e->cap = e->cap ? e->cap * 2 : 1 << 16;
        e->ev = realloc(e->ev, e->cap * sizeof(Event));
        if (!e->ev) {
            fprintf(stderr, "ggx-spu2-compare: out of memory\n");
            exit(2);
        }
    }
    e->ev[e->n++] = *x;
}

static uint32_t fnv(uint32_t h, const uint16_t *w, uint32_t n)
{
    uint32_t i;

    for (i = 0; i < n; i++) {
        h = (h ^ (w[i] & 0xff)) * 16777619u;
        h = (h ^ (w[i] >> 8)) * 16777619u;
    }
    return h;
}

/* A tap read into events. Registers whose writes are actions, not values, aren't value events: KON, KOFF
   (keyed events instead), the data port (a transfer), ENDX (clears flags no sound depends on). NAX and
   the current-volume registers read back what the hardware moves, so a write to NAX always counts. */
typedef struct {
    Spu2 s;
    Events out;
    int started;            /* events are kept from the start on */
    Event ram[3];           /* the transfer runs being merged: core 0's, core 1's, others' */
    int have_ram[3], ram_started[3];
    uint32_t tsa_next[2];   /* where each core's transfer goes on */
    long before;            /* register writes before the start */
} Reader;

static int is_action(uint32_t a)
{
    uint32_t r = a & 0x3ff;

    if (a >= 0x760) {
        return 0;
    }
    return (r >= 0x1a0 && r < 0x1a8) || r == 0x1ac || r == 0x340 || r == 0x342;
}

static int is_nax(uint32_t a)
{
    uint32_t r = a & 0x3ff;

    return a < 0x760 && r >= 0x1c0 && r < 0x1c0 + 24 * 12 && (r - 0x1c0) % 12 >= 8;
}

static void flush_ram(Reader *rd, int k)
{
    if (rd->have_ram[k]) {
        if (rd->ram_started[k]) {
            push(&rd->out, &rd->ram[k]);
        }
        rd->have_ram[k] = 0;
    }
}

static void on_write(Reader *rd, uint32_t tick, uint32_t addr, uint16_t value)
{
    Event e = { 0 };
    uint32_t r = addr & 0x3fe;

    if (!rd->started) {
        rd->before++;
    }
    addr &= 0x7fe;
    if (!is_action(addr) && (is_nax(addr) || spu2_read(&rd->s, addr) != value) && rd->started) {
        e.tick = tick, e.kind = EV_REG, e.addr = (uint16_t)addr, e.value = value;
        push(&rd->out, &e);
    }
    spu2_write(&rd->s, addr, value);
    if (addr < 0x760 && (r == 0x1a8 || r == 0x1aa)) {
        /* a transfer set up: the core's run ends */
        flush_ram(rd, addr >= 0x400);
        rd->tsa_next[addr >= 0x400] = rd->s.core[addr >= 0x400].tsa;
    }
}

/* A write into sound RAM: core 0's or core 1's transfer, if it goes on where that core's TSA or last words
   left off, merged into its run (a DMA lands in pieces) until TSA is set again or 64 samples (1.3 ms) pass. */
static void on_ram(Reader *rd, uint32_t tick, uint32_t addr, const uint16_t *w, uint32_t n)
{
    int k = addr == rd->tsa_next[0] ? 0 : addr == rd->tsa_next[1] ? 1 : 2;
    Event *x = &rd->ram[k];

    if (rd->have_ram[k] && addr == ((x->value + x->words) & (SPU2_RAM_WORDS - 1)) && tick - x->last_tick <= 64) {
        x->words += n;
        x->hash = fnv(x->hash, w, n);
        x->last_tick = tick;
    } else {
        flush_ram(rd, k);
        x->tick = x->last_tick = tick;
        x->kind = EV_RAM;
        x->addr = (uint16_t)k;
        x->value = addr;
        x->words = n;
        x->hash = fnv(2166136261u, w, n);
        rd->have_ram[k] = 1;
        rd->ram_started[k] = rd->started;
    }
    if (k < 2) {
        rd->tsa_next[k] = (addr + n) & (SPU2_RAM_WORDS - 1);
    }
    spu2_write_words(&rd->s, addr, w, n);
}

/* The latches the next sample applies, as events; then the sample. */
static void mix_to(Reader *rd, uint32_t tick)
{
    int16_t o[2];

    while (rd->s.cycles < tick) {
        int core, i;
        for (core = 0; core < 2 && rd->started; core++) {
            Spu2Core *c = &rd->s.core[core];
            uint32_t koff = 0;
            Event e = { 0 };
            for (i = 0; i < SPU2_VOICES; i++) {
                if ((c->koff >> i & 1) && c->v[i].phase != ENV_OFF && c->v[i].phase != ENV_RELEASE) {
                    koff |= 1u << i;
                }
            }
            e.tick = rd->s.cycles;
            e.addr = (uint16_t)core;
            if (koff) {
                e.kind = EV_KOFF, e.value = koff;
                push(&rd->out, &e);
            }
            if (c->kon & 0xffffff) {
                e.kind = EV_KON, e.value = c->kon & 0xffffff;
                push(&rd->out, &e);
            }
        }
        spu2_tick(&rd->s, o);
    }
}

/* The first writes of the port's stream, and where they are in PCSX2's: the record they start at. */
#define START_WRITES 24

typedef struct {
    uint32_t addr[START_WRITES];
    uint16_t value[START_WRITES];
    int n;
} Start;

static int find_start(const char *path, const Start *want, long *index, uint32_t *tick)
{
    Spu2Tap tap;
    Spu2TapRecord r;
    uint32_t ring_addr[32], ring_tick[32];
    uint16_t ring_value[32];
    long ring_index[32], seen = 0, at = 0;
    int rc;

    if (spu2_tap_open(&tap, path) != 0) {
        fprintf(stderr, "ggx-spu2-compare: %s is not an SPU2 tap this reads\n", path);
        return -1;
    }
    while ((rc = spu2_tap_next(&tap, &r)) > 0) {
        int k, ok = 1;
        at++;
        if (r.tag != 'W' || (r.addr >> 16) != 0x1f90) {
            continue;
        }
        ring_addr[seen % 32] = r.addr & 0x7fe;
        ring_value[seen % 32] = r.value;
        ring_tick[seen % 32] = r.tick;
        ring_index[seen % 32] = at;
        seen++;
        for (k = 0; k < want->n && ok; k++) {
            long w = seen - want->n + k;
            ok = w >= 0 && ring_addr[w % 32] == want->addr[k] && ring_value[w % 32] == want->value[k];
        }
        if (ok) {
            *index = ring_index[(seen - want->n) % 32];
            *tick = ring_tick[(seen - want->n) % 32];
            spu2_tap_close(&tap);
            return 0;
        }
    }
    spu2_tap_close(&tap);
    fprintf(stderr, "ggx-spu2-compare: %s never makes the port's first writes\n", path);
    return -1;
}

/* Reads a tap into events, from record start (counting from 1) on, or with start 0 from its mark ('K', the
   driver's first command); the model runs from the SPU2's power-on either way. first: the first writes from
   there, kept; end: the last tick. */
static int read_tap(const char *path, Reader *rd, long start, Start *first, uint32_t *start_tick, uint32_t *end)
{
    Spu2Tap tap;
    Spu2TapRecord r;
    long at = 0;
    int rc;

    if (spu2_tap_open(&tap, path) != 0) {
        fprintf(stderr, "ggx-spu2-compare: %s is not an SPU2 tap this reads\n", path);
        return -1;
    }
    spu2_reset(&rd->s);
    rd->started = 0;
    while ((rc = spu2_tap_next(&tap, &r)) > 0) {
        if (++at == start || (start == 0 && r.tag == 'K')) {
            rd->started = 1;
            *start_tick = r.tick;
        }
        mix_to(rd, r.tick);
        if (r.tag == 'S' || r.tag == 'K') {
            if (r.tag == 'K' && first) {
                first->n = 0;   /* the first writes to look for are the driver's, after the mark */
            }
            continue;
        }
        if (r.tag == 'M') {
            on_ram(rd, r.tick, r.addr, r.data, r.words);
            continue;
        }
        if ((r.addr >> 16) != 0x1f90) {
            continue;
        }
        if (first && first->n < START_WRITES) {
            first->addr[first->n] = r.addr & 0x7fe;
            first->value[first->n++] = r.value;
        }
        on_write(rd, r.tick, r.addr, r.value);
    }
    *end = r.tick ? r.tick : rd->s.cycles;
    flush_ram(rd, 0);
    flush_ram(rd, 1);
    flush_ram(rd, 2);
    spu2_tap_close(&tap);
    if (rc < 0) {
        fprintf(stderr, "ggx-spu2-compare: %s is cut short or bad\n", path);
        return -1;
    }
    return 0;
}

/* tick differences, for a distribution */
typedef struct {
    int64_t *d;
    size_t n, cap;
} Deltas;

static void delta(Deltas *x, int64_t d)
{
    if (x->n == x->cap) {
        x->cap = x->cap ? x->cap * 2 : 1024;
        x->d = realloc(x->d, x->cap * sizeof(int64_t));
    }
    x->d[x->n++] = d;
}

static int cmp64(const void *a, const void *b)
{
    int64_t x = *(const int64_t *)a, y = *(const int64_t *)b;

    return x < y ? -1 : x > y;
}

static void report_deltas(FILE *f, const char *what, Deltas *x)
{
    if (!x->n) {
        fprintf(f, "  %-12s no events matched\n", what);
        return;
    }
    qsort(x->d, x->n, sizeof(int64_t), cmp64);
    fprintf(f, "  %-12s %7zu matched: port minus PCSX2, in samples: min %lld, 5%% %lld, median %lld, 95%% %lld, max %lld "
               "(median %.2f ms)\n",
            what, x->n, (long long)x->d[0], (long long)x->d[x->n / 20], (long long)x->d[x->n / 2],
            (long long)x->d[x->n - 1 - x->n / 20], (long long)x->d[x->n - 1], x->d[x->n / 2] / 48.0);
}

static const char *reg_name(uint32_t a, char *buf, size_t size)
{
    uint32_t r = a & 0x3ff;
    int core = a >= 0x400 && a < 0x760;
    static const char *VP[8] = { "VOLL", "VOLR", "PITCH", "ADSR1", "ADSR2", "ENVX", "VOLXL", "VOLXR" };
    static const char *VA[6] = { "SSA hi", "SSA lo", "LSA hi", "LSA lo", "NAX hi", "NAX lo" };

    if (a >= 0x760 && a < 0x7b0) {
        static const char *EXT[20] = { "MVOLL", "MVOLR", "EVOLL", "EVOLR", "AVOLL", "AVOLR", "BVOLL", "BVOLR",
                                       "MVOLXL", "MVOLXR", "IIR_VOL", "COMB1_VOL", "COMB2_VOL", "COMB3_VOL",
                                       "COMB4_VOL", "WALL_VOL", "APF1_VOL", "APF2_VOL", "IN_COEF_L", "IN_COEF_R" };
        int c = a >= 0x788;
        snprintf(buf, size, "core %d %s", c, EXT[(a - (c ? 0x788 : 0x760)) / 2]);
    } else if (a >= 0x7b0) {
        snprintf(buf, size, "%03x", a);
    } else if (r < 0x180) {
        snprintf(buf, size, "core %d voice %u %s", core, r / 16, VP[(r & 15) / 2]);
    } else if (r >= 0x1c0 && r < 0x1c0 + 24 * 12) {
        snprintf(buf, size, "core %d voice %u %s", core, (r - 0x1c0) / 12, VA[(r - 0x1c0) % 12 / 2]);
    } else {
        snprintf(buf, size, "core %d %03x", core, r);
    }
    return buf;
}

/* a tap's samples (<tap>.pcm, or .pcm.zst beside a .zst tap) */
static size_t read_pcm(const char *tap, int16_t **out)
{
    char path[1100];
    size_t n = strlen(tap), got = 0, cap = 1 << 22;
    int piped;
    FILE *f;

    if (n > 4 && !strcmp(tap + n - 4, ".zst")) {
        snprintf(path, sizeof(path), "%.*s.pcm.zst", (int)(n - 4), tap);
    } else {
        snprintf(path, sizeof(path), "%s.pcm", tap);
    }
    if (!(f = spu2_tap_file(path, &piped))) {
        return 0;
    }
    *out = malloc(cap * 4);
    for (;;) {
        size_t k;
        if (got == cap) {
            cap *= 2;
            *out = realloc(*out, cap * 4);
        }
        k = fread(*out + 2 * got, 4, cap - got, f);
        if (!k) {
            break;
        }
        got += k;
    }
    spu2_tap_file_close(f, piped);
    return got;
}

/* what differs, for the JSON (the CI's expectations) */
static char differ[256][64];
static int ndiffer;

static void note(const char *what, const char *name)
{
    if (ndiffer < 256) {
        snprintf(differ[ndiffer++], sizeof(differ[0]), "%s: %s", what, name);
    }
}

static int64_t median(Deltas *x)
{
    if (!x->n) {
        return 0;
    }
    qsort(x->d, x->n, sizeof(int64_t), cmp64);
    return x->d[x->n / 2];
}

static int cmp_reg(const void *a, const void *b)
{
    const Event *x = *(const Event *const *)a, *y = *(const Event *const *)b;

    if (x->addr != y->addr) {
        return x->addr < y->addr ? -1 : 1;
    }
    return x < y ? -1 : x > y;   /* in order */
}

/* one kind's events from a list, in order */
static size_t select_kind(const Events *e, int kind, int addr, const Event ***out)
{
    size_t i, n = 0;

    *out = malloc((e->n + 1) * sizeof(Event *));
    for (i = 0; i < e->n; i++) {
        if (e->ev[i].kind == kind && (addr < 0 || e->ev[i].addr == addr)) {
            (*out)[n++] = &e->ev[i];
        }
    }
    return n;
}

int main(int argc, char **argv)
{
    static Reader pcsx2, port;
    const char *paths[2] = { NULL, NULL }, *json = NULL;
    Start first = { { 0 }, { 0 }, 0 };
    uint32_t start_pcsx2 = 0, start_port = 0, end_pcsx2 = 0, end_port = 0;
    long start_index = 0;
    int64_t shift;
    long max_report = 8;
    int i, np = 0, values_differ = 0;
    Deltas d_reg = { 0 }, d_kon = { 0 }, d_koff = { 0 }, d_ram = { 0 };
    FILE *jf = NULL;
    char name[64];

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--json") && i + 1 < argc) {
            json = argv[++i];
        } else if (!strcmp(argv[i], "--max-report") && i + 1 < argc) {
            max_report = strtol(argv[++i], NULL, 10);
        } else if (argv[i][0] != '-' && np < 2) {
            paths[np++] = argv[i];
        } else {
            np = 0;
            break;
        }
    }
    if (np != 2) {
        fprintf(stderr, "usage: ggx-spu2-compare <pcsx2 tap> <port tap> [--json <out>] [--max-report N]\n");
        return 2;
    }
    if (read_tap(paths[1], &port, 0, &first, &start_port, &end_port) != 0 ||
        find_start(paths[0], &first, &start_index, &start_pcsx2) != 0 ||
        read_tap(paths[0], &pcsx2, start_index, NULL, &start_pcsx2, &end_pcsx2) != 0) {
        return 2;
    }
    shift = (int64_t)start_port - (int64_t)start_pcsx2;
    {
        /* PCSX2 ran on after the stream ended: what it did then isn't compared */
        size_t k, n = 0;
        for (k = 0; k < pcsx2.out.n; k++) {
            if ((int64_t)pcsx2.out.ev[k].tick + shift <= (int64_t)end_port) {
                pcsx2.out.ev[n++] = pcsx2.out.ev[k];
            }
        }
        pcsx2.out.n = n;
    }
    printf("ggx-spu2-compare: %s against %s\n", paths[1], paths[0]);
    printf("  the driver starts at PCSX2's tick %u (%.3f s), the port's %u: PCSX2's ticks are shifted by %lld\n", start_pcsx2,
           start_pcsx2 / 48000.0, start_port, (long long)shift);
    printf("  before it: PCSX2 makes %ld register writes (the BIOS, sceSdInit), the port %ld (its sceSdInit)\n",
           pcsx2.before, port.before);
    printf("  the port's stream ends at its tick %u (%.3f s); PCSX2's events after that aren't compared\n", end_port,
           end_port / 48000.0);
    printf("  events: PCSX2 %zu, the port %zu\n", pcsx2.out.n, port.out.n);

    /* registers: each one's values, in order */
    {
        const Event **a = malloc((pcsx2.out.n + 1) * sizeof(Event *)), **b = malloc((port.out.n + 1) * sizeof(Event *));
        size_t na = 0, nb = 0, ia = 0, ib = 0, regs = 0, regs_same = 0;
        long reported = 0;
        for (i = 0; i < (int)pcsx2.out.n; i++) {
            if (pcsx2.out.ev[i].kind == EV_REG) {
                a[na++] = &pcsx2.out.ev[i];
            }
        }
        for (i = 0; i < (int)port.out.n; i++) {
            if (port.out.ev[i].kind == EV_REG) {
                b[nb++] = &port.out.ev[i];
            }
        }
        qsort(a, na, sizeof(*a), cmp_reg);
        qsort(b, nb, sizeof(*b), cmp_reg);
        printf("registers (the values each took):\n");
        while (ia < na || ib < nb) {
            uint16_t r = ia < na && (ib >= nb || a[ia]->addr <= b[ib]->addr) ? a[ia]->addr : b[ib]->addr;
            size_t ja = ia, jb = ib, k = 0;
            int same = 1;
            while (ja < na && a[ja]->addr == r) {
                ja++;
            }
            while (jb < nb && b[jb]->addr == r) {
                jb++;
            }
            for (k = 0; ia + k < ja && ib + k < jb; k++) {
                if (a[ia + k]->value != b[ib + k]->value) {
                    break;
                }
                delta(&d_reg, (int64_t)b[ib + k]->tick - ((int64_t)a[ia + k]->tick + shift));
            }
            same = ia + k == ja && ib + k == jb;
            regs++;
            regs_same += same;
            if (!same) {
                values_differ = 1;
                note("registers", reg_name(r, name, sizeof(name)));
                if (reported++ < max_report) {
                    if (ia + k < ja && ib + k < jb) {
                        printf("  %s: value %zu of %zu/%zu differs: PCSX2 %04x at %u, the port %04x at %u\n",
                               reg_name(r, name, sizeof(name)), k + 1, ja - ia, jb - ib, a[ia + k]->value,
                               a[ia + k]->tick, b[ib + k]->value, b[ib + k]->tick);
                    } else {
                        printf("  %s: the first %zu values agree; PCSX2 sets %zu, the port %zu%s\n",
                               reg_name(r, name, sizeof(name)), k, ja - ia, jb - ib,
                               ia + k < ja ? "" : "");
                        if (ia + k < ja) {
                            printf("    PCSX2's next: %04x at %u\n", a[ia + k]->value, a[ia + k]->tick);
                        } else {
                            printf("    the port's next: %04x at %u\n", b[ib + k]->value, b[ib + k]->tick);
                        }
                    }
                }
            }
            ia = ja;
            ib = jb;
        }
        printf("  %zu of %zu registers took the same values in the same order\n", regs_same, regs);
        free(a);
        free(b);
    }

    /* key on and key off, per core: the voices, in order */
    {
        static const char *KIND[2] = { "key on", "key off" };
        int kind, core;
        printf("keys (the voices keyed, in order):\n");
        for (kind = 0; kind < 2; kind++) {
            for (core = 0; core < 2; core++) {
                const Event **a, **b;
                size_t na = select_kind(&pcsx2.out, kind ? EV_KOFF : EV_KON, core, &a);
                size_t nb = select_kind(&port.out, kind ? EV_KOFF : EV_KON, core, &b), k;
                for (k = 0; k < na && k < nb && a[k]->value == b[k]->value; k++) {
                    delta(kind ? &d_koff : &d_kon, (int64_t)b[k]->tick - ((int64_t)a[k]->tick + shift));
                }
                if (k == na && k == nb) {
                    printf("  core %d %s: all %zu the same\n", core, KIND[kind], na);
                } else {
                    values_differ = 1;
                    snprintf(name, sizeof(name), "core %d %s", core, KIND[kind]);
                    note("keys", name);
                    printf("  core %d %s: the first %zu of %zu/%zu agree", core, KIND[kind], k, na, nb);
                    if (k < na && k < nb) {
                        size_t j;
                        printf("; then PCSX2 %06x at %u, the port %06x at %u", a[k]->value, a[k]->tick, b[k]->value,
                               b[k]->tick);
                        printf("\n    around it (the voices, at PCSX2's tick shifted):");
                        for (j = k >= 3 ? k - 3 : 0; j < k + 4 && (j < na || j < nb); j++) {
                            printf("\n     %3zu  PCSX2 ", j);
                            if (j < na) {
                                printf("%06x at %lld", a[j]->value, (long long)a[j]->tick + shift);
                            }
                            printf("%*s  the port ", 22, "");
                            if (j < nb) {
                                printf("%06x at %u", b[j]->value, b[j]->tick);
                            }
                        }
                    }
                    printf("\n");
                }
                free(a);
                free(b);
            }
        }
    }

    /* transfers into sound RAM, per core: where, how many words, what they held */
    {
        static const char *WHO[3] = { "core 0's", "core 1's", "others (neither core's transfer)" };
        int k;
        printf("transfers into sound RAM (runs of words):\n");
        for (k = 0; k < 3; k++) {
            const Event **a, **b;
            size_t na = select_kind(&pcsx2.out, EV_RAM, k, &a), nb = select_kind(&port.out, EV_RAM, k, &b), j;
            for (j = 0; j < na && j < nb && a[j]->value == b[j]->value && a[j]->words == b[j]->words &&
                        a[j]->hash == b[j]->hash;
                 j++) {
                delta(&d_ram, (int64_t)b[j]->tick - ((int64_t)a[j]->tick + shift));
            }
            if (j == na && j == nb) {
                printf("  %s: all %zu the same\n", WHO[k], na);
            } else {
                values_differ = 1;
                note("transfers", WHO[k]);
                printf("  %s: the first %zu of %zu/%zu agree", WHO[k], j, na, nb);
                if (j < na && j < nb) {
                    printf("; then PCSX2 %u words at %05x (at %u), the port %u words at %05x (at %u)%s", a[j]->words,
                           a[j]->value, a[j]->tick, b[j]->words, b[j]->value, b[j]->tick,
                           a[j]->value == b[j]->value && a[j]->words == b[j]->words ? ", different words" : "");
                }
                printf("\n");
            }
            free(a);
            free(b);
        }
    }

    printf("timing of the events that match:\n");
    report_deltas(stdout, "registers", &d_reg);
    report_deltas(stdout, "key on", &d_kon);
    report_deltas(stdout, "key off", &d_koff);
    report_deltas(stdout, "transfers", &d_ram);
    /* the samples, side by side: at the driver's start, and moved by the key ons' median lag */
    {
        int16_t *pa = NULL, *pb = NULL;
        size_t na = read_pcm(paths[0], &pa), nb = read_pcm(paths[1], &pb);
        int64_t lag[2] = { shift, shift + (d_kon.n ? d_kon.d[d_kon.n / 2] : 0) };
        int k;
        printf("samples (PCSX2's and the port's):\n");
        for (k = 0; k < 2 && na && nb; k++) {
            size_t i, same = 0, both = 0, silent = 0;
            double e = 0, ref = 0;
            for (i = 0; i < na; i++) {
                int64_t j = (int64_t)i + lag[k];
                if (j < (int64_t)start_port || j >= (int64_t)nb || (int64_t)i < (int64_t)start_pcsx2) {
                    continue;
                }
                both++;
                same += pa[2 * i] == pb[2 * j] && pa[2 * i + 1] == pb[2 * j + 1];
                silent += !pa[2 * i] && !pa[2 * i + 1] && !pb[2 * j] && !pb[2 * j + 1];
                e += (double)(pa[2 * i] - pb[2 * j]) * (pa[2 * i] - pb[2 * j]) +
                     (double)(pa[2 * i + 1] - pb[2 * j + 1]) * (pa[2 * i + 1] - pb[2 * j + 1]);
                ref += (double)pa[2 * i] * pa[2 * i] + (double)pa[2 * i + 1] * pa[2 * i + 1];
            }
            printf("  %s: %zu of %zu samples identical (%zu of them silence in both); the difference %.1f dB below "
                   "PCSX2's\n",
                   k ? "moved by the key ons' median lag" : "at the driver's start", same, both, silent,
                   ref > 0 && e > 0 ? 10 * log10(ref / e) : 0.0);
        }
        free(pa);
        free(pb);
    }
    printf("ggx-spu2-compare: the values %s\n", values_differ ? "differ" : "all match");

    if (json && (jf = fopen(json, "w"))) {
        fprintf(jf, "{\"start_pcsx2\": %u, \"start_port\": %u, \"events_pcsx2\": %zu, \"events_port\": %zu,\n",
                start_pcsx2, start_port, pcsx2.out.n, port.out.n);
        fprintf(jf, " \"median_lag\": {\"registers\": %lld, \"key_on\": %lld, \"key_off\": %lld, \"transfers\": %lld},\n",
                (long long)median(&d_reg), (long long)median(&d_kon), (long long)median(&d_koff), (long long)median(&d_ram));
        fprintf(jf, " \"differ\": [");
        for (i = 0; i < ndiffer; i++) {
            fprintf(jf, "%s\"%s\"", i ? ", " : "", differ[i]);
        }
        fprintf(jf, "]}\n");
        fclose(jf);
    }
    return values_differ ? 1 : 0;
}
