/* ggx-spu2-replay: an SPU2 tap from PCSX2, replayed into the port's SPU2 model (PORT-011).
 *
 *   ggx-spu2-replay <tap> [--pcm <samples>] [--out <port.pcm>] [--stages <dump> [--ram <dump>]] [--max-report N]
 *
 * The tap (spu2_tap.h) holds, from the SPU2's power-on, every register the IOP wrote and every write into sound
 * RAM from outside the mixer, each with the SPU2 tick it came at, and beside it (<tap>.pcm, or <tap minus
 * .zst>.pcm.zst) every sample pair PCSX2's SPU2 mixed, before PCSX2's own output stage. The replay starts the
 * model from power-on, as PCSX2 starts its SPU2, applies each record before the sample after its tick, and
 * compares every sample with PCSX2's, bit for bit. Exit status 0 if every sample is the same; 1 if not, with the
 * first differing tick, its values and how many samples differ; 2 if the tap can't be read. --out writes the
 * model's samples.
 *
 * --stages <dump>: the recorder's stage dump for a window of ticks (GG_SPU2_TAP_STAGES): each voice's sample,
 * envelope, volumes and decoder, each core's mixes and reverb. The replay compares the model's with them and
 * names the first that differs: the tick, the core, the voice or stage, and the values. --ram <dump>: sound RAM
 * at the window's last tick, compared word by word.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spu2.h"
#include "spu2_tap.h"

static const char *VOICE_FIELD[11] = { "sample after the envelope", "out left", "out right", "envelope", "phase",
                                      "volume left", "volume right", "NAX", "pitch counter", "FIFO read",
                                      "FIFO write" };
static const char *CORE_FIELD[8] = { "dry left", "dry right", "wet left", "wet right", "reverb left", "reverb right",
                                     "out left", "out right" };
static const char *REVERB_FIELD[12] = { "reverb in", "reverb same", "reverb diff", "reverb apf1", "reverb apf2",
                                        "reverb out", "reverb same address", "reverb diff address",
                                        "reverb apf1 address", "reverb apf2 address", "reverb diff source",
                                        "reverb diff previous" };

static int32_t s32r(FILE *f)
{
    uint8_t b[4] = { 0, 0, 0, 0 };

    if (fread(b, 1, 4, f) != 4) {
        return 0;
    }
    return (int32_t)((uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24);
}

/* the stage dump (GGTap.cpp in the patch), read in step with the model */
typedef struct {
    FILE *f;
    uint32_t tick;    /* the tick whose records are next, or 0 at the end */
    int reported;
    long checked;
    const char *ram;  /* PCSX2's sound RAM at the window's end, to compare */
} Stages;

static void compare_ram(const Stages *st, const Spu2 *s)
{
    static uint16_t ram[SPU2_RAM_WORDS];
    FILE *f = fopen(st->ram, "rb");
    uint32_t a, n = 0;

    if (!f || fread(ram, 2, SPU2_RAM_WORDS, f) != SPU2_RAM_WORDS) {
        fprintf(stderr, "ggx-spu2-replay: cannot read %s\n", st->ram);
        if (f) {
            fclose(f);
        }
        return;
    }
    fclose(f);
    for (a = 0; a < SPU2_RAM_WORDS; a++) {
        if (ram[a] != s->ram[a] && n++ < 4) {
            fprintf(stderr, "  sound RAM %05x: port %04x, PCSX2 %04x\n", a, s->ram[a], ram[a]);
        }
    }
    fprintf(stderr, "ggx-spu2-replay: at tick %u, %u words of sound RAM differ\n", s->cycles, n);
}

/* Compares n of the dump's values with the model's; on the first difference, names it and shows them all. */
static void compare(Stages *st, int core, int voice, const char *const *names, const int32_t *mine, int n)
{
    int32_t theirs[12];
    int k, first = -1;

    for (k = 0; k < n; k++) {
        theirs[k] = s32r(st->f);
        if (theirs[k] != mine[k] && first < 0) {
            first = k;
        }
    }
    if (first < 0 || st->reported) {
        return;
    }
    st->reported = 1;
    if (voice >= 0) {
        fprintf(stderr, "ggx-spu2-replay: the first stage to differ: tick %u, core %d voice %d, %s: port %d, PCSX2 %d\n",
                st->tick, core, voice, names[first], mine[first], theirs[first]);
    } else {
        fprintf(stderr, "ggx-spu2-replay: the first stage to differ: tick %u, core %d, %s: port %d, PCSX2 %d\n",
                st->tick, core, names[first], mine[first], theirs[first]);
    }
    for (k = 0; k < n; k++) {
        fprintf(stderr, "  %-26s port %d, PCSX2 %d\n", names[k], mine[k], theirs[k]);
    }
}

static void on_voice(void *ctx, const Spu2 *s, int core, int voice, int32_t value, const int32_t out[2])
{
    Stages *st = ctx;
    const Spu2Voice *v = &s->core[core].v[voice];
    int32_t mine[11];

    if (!st->f || st->reported || st->tick != s->cycles) {
        return;
    }
    if (core == 0 && voice == 0 && (uint32_t)s32r(st->f) != st->tick) {
        fprintf(stderr, "ggx-spu2-replay: the stage dump is out of step at tick %u\n", st->tick);
        st->reported = 1;
        return;
    }
    mine[0] = value, mine[1] = out[0], mine[2] = out[1], mine[3] = v->env, mine[4] = v->phase;
    mine[5] = v->vol[0].value, mine[6] = v->vol[1].value, mine[7] = (int32_t)v->nax, mine[8] = v->counter;
    mine[9] = (int32_t)v->fifo_read, mine[10] = (int32_t)v->fifo_write;
    compare(st, core, voice, VOICE_FIELD, mine, 11);
    st->checked++;
}

static void on_core(void *ctx, const Spu2 *s, int core, const int32_t mix[8])
{
    Stages *st = ctx;
    const Spu2Core *c = &s->core[core];
    int16_t buf[2][2][128];
    uint32_t pos = 0, next;
    int side, j;
    long at;

    if (!st->f || st->reported || st->tick != s->cycles) {
        return;
    }
    compare(st, core, -1, CORE_FIELD, mix, 8);
    compare(st, core, -1, REVERB_FIELD, c->rev_last, 12);
    /* the reverb's buffers */
    pos = (uint32_t)s32r(st->f);
    if (fread(buf, 2, 4 * 128, st->f) != 4 * 128) {
        st->tick = 0;
        return;
    }
    if (!st->reported && pos != c->rev_pos) {
        st->reported = 1;
        fprintf(stderr, "ggx-spu2-replay: the first stage to differ: tick %u, core %d, the reverb's position: port %u, "
                        "PCSX2 %u\n", st->tick, core, c->rev_pos, pos);
    }
    for (side = 0; side < 2 && !st->reported; side++) {
        for (j = 0; j < 128 && !st->reported; j++) {
            const char *what = buf[0][side][j] != c->rev_down[side][j] ? "downsampling" :
                               buf[1][side][j] != c->rev_up[side][j] ? "upsampling" : NULL;
            if (what) {
                st->reported = 1;
                fprintf(stderr, "ggx-spu2-replay: the first stage to differ: tick %u, core %d, the reverb's %s "
                                "buffer, side %d, entry %d\n", st->tick, core, what, side, j);
            }
        }
    }
    if (core == 1) {
        /* the dump's next tick */
        uint8_t b[4];
        at = ftell(st->f);
        if (fread(b, 1, 4, st->f) != 4) {
            st->tick = 0;
            if (st->ram) {
                compare_ram(st, s);
            }
        } else {
            next = (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
            st->tick = next;
            fseek(st->f, at, SEEK_SET);
        }
    }
}

typedef struct {
    FILE *ref, *out;
    uint64_t tick, differ, first;
    long max_report;
    int16_t first_port[2], first_ref[2];
    int have_first;
    int ended;
} Run;

/* Mixes the samples up to (not including) tick t, comparing each. */
static void run_to(Spu2 *s, Run *r, uint64_t t)
{
    while (r->tick < t) {
        int16_t o[2], ref[2];
        uint8_t b[4];

        spu2_tick(s, o);
        if (r->out) {
            fwrite(o, 2, 2, r->out);
        }
        if (!r->ended && fread(b, 1, 4, r->ref) == 4) {
            ref[0] = (int16_t)(b[0] | b[1] << 8);
            ref[1] = (int16_t)(b[2] | b[3] << 8);
            if (ref[0] != o[0] || ref[1] != o[1]) {
                if (!r->have_first) {
                    r->have_first = 1;
                    r->first = r->tick;
                    memcpy(r->first_port, o, sizeof(o));
                    memcpy(r->first_ref, ref, sizeof(ref));
                }
                if (r->differ < (uint64_t)r->max_report) {
                    fprintf(stderr, "  tick %llu: port %d %d, PCSX2 %d %d\n", (unsigned long long)r->tick, o[0], o[1],
                            ref[0], ref[1]);
                }
                r->differ++;
            }
        } else {
            r->ended = 1;
        }
        r->tick++;
    }
}

static int usage(void)
{
    fprintf(stderr, "usage: ggx-spu2-replay <tap> [--pcm <samples>] [--out <port.pcm>] [--stages <dump> [--ram <dump>]] "
                    "[--max-report N]\n");
    return 2;
}

int main(int argc, char **argv)
{
    const char *path = NULL, *pcm = NULL, *out = NULL, *stages_path = NULL;
    char pcm_default[1100];
    static Spu2 s;
    Spu2Tap tap;
    Spu2TapRecord rec;
    Stages st = { 0 };
    Run r = { 0 };
    uint32_t writes = 0, mems = 0, end = 0;
    int i, rc, ref_piped = 0;

    r.max_report = 10;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--pcm") && i + 1 < argc) {
            pcm = argv[++i];
        } else if (!strcmp(argv[i], "--out") && i + 1 < argc) {
            out = argv[++i];
        } else if (!strcmp(argv[i], "--stages") && i + 1 < argc) {
            stages_path = argv[++i];
        } else if (!strcmp(argv[i], "--ram") && i + 1 < argc) {
            st.ram = argv[++i];
        } else if (!strcmp(argv[i], "--max-report") && i + 1 < argc) {
            r.max_report = strtol(argv[++i], NULL, 10);
        } else if (argv[i][0] != '-' && !path) {
            path = argv[i];
        } else {
            return usage();
        }
    }
    if (!path) {
        return usage();
    }
    if (!pcm) {
        size_t n = strlen(path);
        if (n > 4 && !strcmp(path + n - 4, ".zst")) {
            snprintf(pcm_default, sizeof(pcm_default), "%.*s.pcm.zst", (int)(n - 4), path);
        } else {
            snprintf(pcm_default, sizeof(pcm_default), "%s.pcm", path);
        }
        pcm = pcm_default;
    }
    if (spu2_tap_open(&tap, path) != 0 || !(r.ref = spu2_tap_file(pcm, &ref_piped))) {
        fprintf(stderr, "ggx-spu2-replay: %s (with %s) is not an SPU2 tap this reads\n", path, pcm);
        return 2;
    }
    if (out && !(r.out = fopen(out, "wb"))) {
        fprintf(stderr, "ggx-spu2-replay: cannot write %s\n", out);
        return 2;
    }
    spu2_reset(&s);
    if (stages_path) {
        uint8_t b[4];
        if (!(st.f = fopen(stages_path, "rb")) || fread(b, 1, 4, st.f) != 4) {
            fprintf(stderr, "ggx-spu2-replay: cannot read %s\n", stages_path);
            return 2;
        }
        st.tick = (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
        fseek(st.f, 0, SEEK_SET);
        s.on_voice = on_voice;
        s.on_core = on_core;
        s.on_ctx = &st;
    }
    while ((rc = spu2_tap_next(&tap, &rec)) > 0) {
        run_to(&s, &r, rec.tick);
        if (rec.tag == 'S') {
            /* PCSX2's registers when the tap opened, at its first mix: the model starts from power-on, as PCSX2
               starts its SPU2, so these only check that start */
            uint32_t a, bad = 0;
            for (a = 0; a < 0x7b0; a += 2) {
                uint32_t rr = a & 0x3ff;
                if (rr == 0x1ac || rr == 0x340 || rr == 0x342 || (rr >= 0x1c8 && rr < 0x2e0 && (rr - 0x1c0) % 12 >= 8)) {
                    continue;   /* the data port; ENDX and NAX, which the first mix has moved */
                }
                if (spu2_read(&s, a) != rec.data[a / 2] && bad++ < 8) {
                    fprintf(stderr, "  power-on: register %03x is %04x, PCSX2 %04x\n", a, spu2_read(&s, a),
                            rec.data[a / 2]);
                }
            }
            if (bad) {
                fprintf(stderr, "ggx-spu2-replay: %s: %u registers differ from PCSX2's at tick %u\n", path, bad, rec.tick);
            }
        } else if (rec.tag == 'K') {
            continue;
        } else if (rec.tag == 'W') {
            if ((rec.addr >> 16) == 0x1f90) {
                spu2_write(&s, rec.addr & 0xffff, rec.value);
            }
            writes++;
        } else {
            spu2_write_words(&s, rec.addr, rec.data, rec.words);
            mems++;
        }
    }
    end = rec.tick;
    spu2_tap_close(&tap);
    if (rc < 0) {
        fprintf(stderr, "ggx-spu2-replay: %s: a bad or cut-short record after tick %llu\n", path, (unsigned long long)r.tick);
        return 2;
    }
    run_to(&s, &r, end);
    if (r.out) {
        fclose(r.out);
    }
    spu2_tap_file_close(r.ref, ref_piped);
    if (st.f) {
        if (!st.reported) {
            fprintf(stderr, "ggx-spu2-replay: every stage the same, %ld voices' samples\n", st.checked);
        }
        fclose(st.f);
    }
    if (r.ended && r.tick > 0) {
        fprintf(stderr, "ggx-spu2-replay: %s: PCSX2's samples end before the tap does\n", path);
        return 1;
    }
    if (r.differ) {
        fprintf(stderr, "ggx-spu2-replay: %s: %llu of %llu samples differ; the first at tick %llu (port %d %d, PCSX2 %d "
                        "%d); %u register writes, %u memory writes\n",
                path, (unsigned long long)r.differ, (unsigned long long)r.tick, (unsigned long long)r.first,
                r.first_port[0], r.first_port[1], r.first_ref[0], r.first_ref[1], writes, mems);
        return 1;
    }
    fprintf(stderr, "ggx-spu2-replay: %s: all %llu samples identical (%u register writes, %u memory writes)\n", path,
            (unsigned long long)r.tick, writes, mems);
    return 0;
}
