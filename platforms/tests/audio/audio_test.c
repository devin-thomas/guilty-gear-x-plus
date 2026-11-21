/* ggx-audio-test (PORT-011): the sound driver and the SPU2 model without a disc.
 *
 *   ggx-audio-test <scratch dir>
 *
 * The SPU2 model, through its registers, against the rules it implements:
 * ADPCM decoding and its filters and the decoded-block reuse, the envelope
 * phases (the decay step that halves the BGM's voices), direct and sliding
 * volumes, loop flags and ENDX, and reverb that stays bounded and is
 * silent when the game's settings turn it off. Its bit-exactness against
 * PCSX2 is ggx-spu2-replay's (AUDIO.md, SPU2 lockstep). Then
 * the driver: an effect uploaded and keyed on and its end, and BGM streamed
 * from a made-up disc through every refill of the ring, looped and
 * stopped. Then the stream file, written and read back.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snd_stream.h"
#include "sndrv.h"
#include "spu2.h"

static int failures;

#define CHECK(cond, ...)                                  \
    do {                                                  \
        if (!(cond)) {                                    \
            failures++;                                   \
            fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__);                 \
            fprintf(stderr, "\n");                        \
        }                                                 \
    } while (0)

/* An ADPCM block: header (shift, filter), flags, 28 nibbles. */
static void block(uint8_t *b, int shift, int filter, int flags, const int8_t *nib)
{
    int i;

    b[0] = (uint8_t)(shift | filter << 4);
    b[1] = (uint8_t)flags;
    for (i = 0; i < 14; i++) {
        b[2 + i] = (uint8_t)((nib[2 * i] & 0xf) | (nib[2 * i + 1] & 0xf) << 4);
    }
}

static void tick_n(Spu2 *s, int n, int16_t *last)
{
    int16_t o[2] = { 0, 0 };

    while (n-- > 0) {
        spu2_tick(s, o);
    }
    if (last) {
        last[0] = o[0];
        last[1] = o[1];
    }
}

/* register addresses: a voice's parameter (VOLL 0, VOLR 2, PITCH 4, ADSR1 6, ADSR2 8, ENVX A, VOLXL C),
   its address registers (SSA 0, LSA 4, NAX 8; high word first), a core's own */
static uint32_t vreg(int core, int voice, uint32_t r)
{
    return (uint32_t)core * 0x400 + (uint32_t)voice * 0x10 + r;
}

static void vaddr(Spu2 *s, int core, int voice, uint32_t r, uint32_t words)
{
    uint32_t a = (uint32_t)core * 0x400 + 0x1c0 + (uint32_t)voice * 12 + r;

    spu2_write(s, a, (uint16_t)(words >> 16));
    spu2_write(s, a + 2, (uint16_t)words);
}

static void creg(Spu2 *s, int core, uint32_t r, uint16_t v)
{
    spu2_write(s, (uint32_t)core * 0x400 + r, v);
}

static void cvol(Spu2 *s, int core, uint32_t r, uint16_t v)
{
    spu2_write(s, 0x760 + (uint32_t)core * 0x28 + r, v);
}

static void key_on(Spu2 *s, int core, int voice)
{
    creg(s, core, 0x1a0, (uint16_t)(1u << voice));
}

/* A model with the mixing the driver sets up: both cores' voices to their dry mix, the main volumes at
   half, core 0's output into core 1. A voice keyed on starts at the next sample. */
static Spu2 *setup(void)
{
    Spu2 *s = malloc(sizeof(*s));
    int c;

    spu2_reset(s);
    for (c = 0; c < 2; c++) {
        cvol(s, c, 0, 0x3fff);
        cvol(s, c, 2, 0x3fff);
    }
    creg(s, 0, 0x198, 0xc00);
    creg(s, 1, 0x198, 0xc0c);
    return s;
}

static const int16_t *decoded(Spu2 *s, uint32_t words)
{
    return s->blocks[words / 8].pcm;
}

static void test_adpcm(void)
{
    Spu2 *s = setup();
    uint8_t b[32];
    int8_t nib[28];
    int i;

    /* filter 0, shift 0: each nibble is the sample's top 4 bits */
    for (i = 0; i < 28; i++) {
        nib[i] = (int8_t)((i % 16) - 8);
    }
    block(b, 0, 0, 0, nib);
    block(b + 16, 0, 0, 3, nib);
    spu2_write_ram(s, 0x10000, b, 32);
    vaddr(s, 1, 1, 0, 0x8000);
    spu2_write(s, vreg(1, 1, 4), 0x1000);
    key_on(s, 1, 1);
    tick_n(s, 1, NULL);
    for (i = 0; i < 28; i++) {
        CHECK(decoded(s, 0x8000)[i] == (int16_t)(nib[i] * 4096), "filter 0 sample %d: %d", i, decoded(s, 0x8000)[i]);
    }
    /* filter 1 (60/64 of the last sample), shift 12: x = nibble + (60 * prev + 32) >> 6 */
    memset(nib, 0, sizeof(nib));
    nib[0] = 7;
    block(b, 12, 1, 0, nib);
    spu2_write_ram(s, 0x10000, b, 16);
    key_on(s, 1, 1);
    tick_n(s, 1, NULL);
    {
        int32_t prev = 7;
        CHECK(decoded(s, 0x8000)[0] == 7, "filter 1 first sample %d", decoded(s, 0x8000)[0]);
        for (i = 1; i < 28; i++) {
            prev = (60 * prev + 32) >> 6;
            CHECK(decoded(s, 0x8000)[i] == prev, "filter 1 sample %d: %d, want %d", i, decoded(s, 0x8000)[i], prev);
        }
    }
    /* shift 13-15 shift the nibble on down (and 15 leaves only its sign) */
    memset(nib, 0, sizeof(nib));
    nib[0] = 1;
    nib[1] = -1;
    block(b, 13, 0, 0, nib);
    spu2_write_ram(s, 0x10000, b, 16);
    key_on(s, 1, 1);
    tick_n(s, 1, NULL);
    CHECK(decoded(s, 0x8000)[0] == (4096 >> 13) && decoded(s, 0x8000)[1] == -1, "shift 13: %d %d",
          decoded(s, 0x8000)[0], decoded(s, 0x8000)[1]);
    /* a block entered again from the same two samples is reused; written over, it's decoded again */
    nib[0] = 3;
    block(b, 0, 0, 0, nib);
    s->ram[0x8001] = (uint16_t)(b[2] | b[3] << 8);
    key_on(s, 1, 1);
    tick_n(s, 1, NULL);
    CHECK(decoded(s, 0x8000)[0] == (4096 >> 13), "a block decoded again without a write: %d", decoded(s, 0x8000)[0]);
    spu2_write_ram(s, 0x10000, b, 16);
    key_on(s, 1, 1);
    tick_n(s, 1, NULL);
    CHECK(decoded(s, 0x8000)[0] == 3 * 4096, "a block written over isn't decoded again: %d", decoded(s, 0x8000)[0]);
    free(s);
}

static void test_envelope(void)
{
    Spu2 *s = setup();
    Spu2Voice *v = &s->core[1].v[1];
    uint8_t b[16];
    int8_t nib[28] = { 0 };
    int i, peak = 0;

    block(b, 0, 0, 3, nib);   /* one block, looping */
    spu2_write_ram(s, 0x10000, b, 16);
    vaddr(s, 1, 1, 0, 0x8000);
    vaddr(s, 1, 1, 4, 0x8000);
    spu2_write(s, vreg(1, 1, 4), 0xeb3);
    /* the BGM's voices: linear attack at the top rate, decay shift 0, sustain level 0xF, sustain at the
       slowest rate */
    spu2_write(s, vreg(1, 1, 6), 0x000f);
    spu2_write(s, vreg(1, 1, 8), 0x1fc0);
    key_on(s, 1, 1);
    for (i = 0; i < 64; i++) {
        tick_n(s, 1, NULL);
        if (spu2_read(s, vreg(1, 1, 0xa)) > peak) {
            peak = spu2_read(s, vreg(1, 1, 0xa));
        }
    }
    CHECK(peak == 0x7fff, "BGM attack peak %04x", peak);
    CHECK(v->phase == ENV_SUSTAIN && v->env == 0x3fff,
          "BGM voice: one decay step from 7FFFh before sustain should leave 3FFFh, have phase %d level %04x", v->phase,
          v->env);
    /* the slowest rate still steps, once every 0x8000 samples (PCSX2; AUDIO.md, SPU2 lockstep) */
    tick_n(s, 0x8000 - 64 - 1, NULL);
    CHECK(v->env == 0x3fff, "BGM sustain moved early, to %04x", v->env);
    tick_n(s, 0x8000, NULL);
    CHECK(v->env == 0x3fff + 4, "BGM sustain after 0x8000 samples: %04x", v->env);
    /* the effects' voices: decay shift 15 doesn't step at once, so they sustain at the top */
    spu2_write(s, vreg(1, 1, 6), 0x81ff);
    spu2_write(s, vreg(1, 1, 8), 0xdfe1);
    key_on(s, 1, 1);
    tick_n(s, 64, NULL);
    CHECK(v->phase == ENV_SUSTAIN && v->env == 0x7fff, "effect sustain: phase %d level %04x", v->phase, v->env);
    /* key off: exponential release at shift 1 reaches zero, and the voice stops */
    creg(s, 1, 0x1a4, 1u << 1);
    tick_n(s, 4800, NULL);
    CHECK(v->phase == ENV_OFF && v->env == 0, "after release: phase %d level %04x", v->phase, v->env);
    free(s);
}

static void test_volume_and_loop(void)
{
    Spu2 *s = setup();
    Spu2Voice *v = &s->core[1].v[1];
    uint8_t b[48];
    int8_t nib[28];
    int16_t o[2];
    int i;

    for (i = 0; i < 28; i++) {
        nib[i] = 4;   /* a constant 0x4000 */
    }
    block(b, 0, 0, 0, nib);
    block(b + 16, 0, 0, 1, nib);   /* end and mute */
    block(b + 32, 0, 0, 0, nib);
    spu2_write_ram(s, 0x10000, b, 48);
    vaddr(s, 1, 1, 0, 0x8000);
    spu2_write(s, vreg(1, 1, 4), 0x1000);
    spu2_write(s, vreg(1, 1, 6), 0x00ff);   /* linear attack to 7FFFh; decay shift 15 holds it near the top */
    spu2_write(s, vreg(1, 1, 8), 0x1fc0);
    spu2_write(s, vreg(1, 1, 0), 0x3fff);
    spu2_write(s, vreg(1, 1, 2), 0x2000);
    key_on(s, 1, 1);
    tick_n(s, 30, o);
    CHECK(spu2_read(s, vreg(1, 1, 0xc)) == 0x7ffe && spu2_read(s, vreg(1, 1, 0xe)) == 0x4000, "direct volumes %04x %04x",
          spu2_read(s, vreg(1, 1, 0xc)), spu2_read(s, vreg(1, 1, 0xe)));
    CHECK(o[0] > 0x3e00 && o[0] < 0x4000 && o[1] > 0x1f00 && o[1] < 0x2000, "output %d %d", o[0], o[1]);
    CHECK(!(s->core[1].endx & 2), "ENDX before the end");
    tick_n(s, 60, o);
    CHECK(s->core[1].endx & 2, "ENDX at the end");
    CHECK(v->phase == ENV_OFF && o[0] == 0 && o[1] == 0, "end and mute: phase %d output %d %d", v->phase, o[0], o[1]);
    /* slides: at rate 7Fh one never moves (unlike an envelope's); at 0, linear increase steps 7 << 11 a sample */
    spu2_write(s, vreg(1, 2, 0), 0x8000 | 0x7f);
    tick_n(s, 0x10000, NULL);
    CHECK(s->core[1].v[2].vol[0].value == 0, "a slide at rate 7Fh moved, to %d", s->core[1].v[2].vol[0].value);
    spu2_write(s, vreg(1, 2, 0), 0x8000);
    tick_n(s, 2, NULL);
    CHECK(s->core[1].v[2].vol[0].value == 2 * (7 << 11), "the fastest linear slide: %d", s->core[1].v[2].vol[0].value);
    free(s);
}

/* the reverb registers' addresses, in Spu2Core.rev order: work-area offsets at 0x2E4 on (high, low), volumes
   at 0x774 on */
static void rev_set(Spu2 *s, int core, int i, uint32_t v)
{
    static const uint16_t ADDR[32] = { 0x2e4, 0x2e8, 0x774, 0x776, 0x778, 0x77a, 0x77c, 0x77e, 0x780, 0x782, 0x2ec,
                                       0x2f0, 0x2f4, 0x2f8, 0x2fc, 0x300, 0x304, 0x308, 0x30c, 0x310, 0x314, 0x318,
                                       0x31c, 0x320, 0x324, 0x328, 0x32c, 0x330, 0x334, 0x338, 0x784, 0x786 };
    uint32_t a = ADDR[i];

    if (a >= 0x774) {
        cvol(s, core, a - 0x760, (uint16_t)v);
    } else {
        creg(s, core, a, (uint16_t)(v >> 16));
        creg(s, core, a + 2, (uint16_t)v);
    }
}

static void test_reverb(void)
{
    Spu2 *s = setup();
    uint8_t b[32];
    int8_t nib[28] = { 7 };
    double before = 0, tail = 0, late = 0;
    int16_t o[2];
    int i, bad = 0;

    block(b, 0, 0, 0, nib);
    memset(nib, 0, sizeof(nib));
    block(b + 16, 0, 0, 3, nib);
    spu2_write_ram(s, 0x10000, b, 32);
    vaddr(s, 0, 0, 0, 0x8000);
    vaddr(s, 0, 0, 4, 0x8008);
    spu2_write(s, vreg(0, 0, 4), 0x1000);
    spu2_write(s, vreg(0, 0, 6), 0x00ff);
    spu2_write(s, vreg(0, 0, 8), 0x1fc0);
    spu2_write(s, vreg(0, 0, 0), 0x3fff);
    spu2_write(s, vreg(0, 0, 2), 0x3fff);
    /* the game's settings: reverb off, return volume zero. Nothing reverberates */
    key_on(s, 0, 0);
    for (i = 0; i < 9600; i++) {
        spu2_tick(s, o);
        if (i > 400) {
            before += fabs((double)o[0]);
        }
    }
    CHECK(before == 0, "reverb off: %f after the impulse", before);
    /* a room: a 0x8000-word work area and moderate settings, written to and returned */
    creg(s, 0, 0x18c, 1);
    creg(s, 0, 0x194, 1);
    creg(s, 0, 0x198, 0xf00);
    creg(s, 0, 0x19a, 0x80);
    creg(s, 0, 0x2e0, 0xe);
    creg(s, 0, 0x2e2, 0x8000);
    cvol(s, 0, 4, 0x6000);
    cvol(s, 0, 6, 0x6000);
    {
        static const uint16_t room[32] = { 0x007d, 0x005b, 0x6d80, 0x54b8, 0xbed0, 0x0000, 0x0000, 0xba80, 0x5800,
                                           0x5300, 0x04d6, 0x0333, 0x03f0, 0x0227, 0x0374, 0x01ef, 0x0334, 0x01b5,
                                           0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x01b4,
                                           0x0136, 0x00b8, 0x005c, 0x8000, 0x8000 };
        for (i = 0; i < 32; i++) {
            rev_set(s, 0, i, room[i]);
        }
    }
    key_on(s, 0, 0);
    for (i = 0; i < 96000; i++) {
        spu2_tick(s, o);
        if (o[0] == 32767 || o[0] == -32768) {
            bad++;
        }
        if (i > 4800 && i < 24000) {
            tail += fabs((double)o[0]);
        }
        if (i > 72000) {
            late += fabs((double)o[0]);
        }
    }
    CHECK(tail > 1000, "reverb on: no tail after the dry sound (%f)", tail);
    CHECK(late < tail / 4, "reverb on: the tail doesn't decay (%f, then %f)", tail, late);
    CHECK(bad == 0, "reverb on: %d clipped samples", bad);
    free(s);
}

/* ------------------------------------------------------------------------
 * The driver, on a made-up disc
 * ------------------------------------------------------------------------ */

#define BGM_LSN 1000
#define BGM_SECTORS 0x80   /* 8 chunks of 16 KB, left and right in turn */

static uint8_t *disc_data;

static int fake_read(void *ctx, uint32_t lsn, uint32_t count, void *dst)
{
    (void)ctx;
    if (lsn < BGM_LSN || lsn + count > BGM_LSN + BGM_SECTORS) {
        memset(dst, 0, count * 2048u);
        return 0;
    }
    memcpy(dst, disc_data + (lsn - BGM_LSN) * 2048u, count * 2048u);
    return 0;
}

/* BGM as the game's files lay it out: 16 KB of left, 16 KB of right, each chunk starting with a loop-start
   block and ending with a loop-end one; a square wave whose level counts the chunks, so the order shows */
static void make_bgm(void)
{
    uint32_t chunk, i;

    disc_data = calloc(BGM_SECTORS, 2048);
    for (chunk = 0; chunk < BGM_SECTORS * 2048u / 0x4000; chunk++) {
        for (i = 0; i < 0x4000; i += 16) {
            int8_t nib[28];
            int k;
            for (k = 0; k < 28; k++) {
                nib[k] = (int8_t)(((k / 7) & 1) ? 1 + (int)(chunk / 2) : -1 - (int)(chunk / 2));
            }
            block(disc_data + chunk * 0x4000 + i, 4, 0, i == 0 ? 6 : i == 0x3ff0 ? 3 : 2, nib);
        }
    }
}

static void put32(uint8_t *p, uint32_t v)
{
    memcpy(p, &v, 4);
}

static void cmd(Sndrv *d, uint32_t c, uint32_t a0, uint32_t a1, uint32_t size)
{
    uint8_t buf[16] = { 0 };

    put32(buf, a0);
    put32(buf + 4, a1);
    sndrv_command(d, c, buf, size);
}

static double rms(const int16_t *x, uint32_t frames)
{
    double e = 0;
    uint32_t i;

    for (i = 0; i < frames * 2; i++) {
        e += (double)x[i] * x[i];
    }
    return sqrt(e / (frames * 2.0));
}

static void test_driver(void)
{
    Sndrv *d;
    int16_t *out = malloc(48000 * 4 * 8);
    uint32_t i, gaps = 0;
    uint8_t se[0x40 + 0x200 + 0x10];
    int8_t nib[28];

    make_bgm();
    d = sndrv_create(fake_read, NULL);
    sndrv_command(d, SND_INIT, NULL, 0);
    /* an effect: voice 1, 512 bytes; the driver turns its last block into an end */
    memset(se, 0, sizeof(se));
    put32(se, 1);
    put32(se + 4, 0x200);
    for (i = 0; i < 28; i++) {
        nib[i] = (i & 4) ? 6 : -6;
    }
    for (i = 0; i < 0x200; i += 16) {
        block(se + 0x40 + i, 2, 0, 0, nib);
    }
    sndrv_command(d, SND_SE_TRANS | SND_LAST, se, sizeof(se));
    sndrv_render(d, out, 4800);
    CHECK(rms(out, 1200) > 100, "the effect doesn't sound (%f)", rms(out, 1200));
    CHECK(rms(out + 2 * 3600, 1200) == 0, "the effect doesn't end (%f)", rms(out + 2 * 3600, 1200));
    CHECK(sndrv_spu2(d)->core[1].endx & 2, "the effect's voice didn't reach its end");

    /* BGM: volume, load, start; eight chunks at 44.1 kHz are about 5.2 s, looped */
    cmd(d, SND_BGM_VOLUME | SND_LAST, 0x2800, 0, 8);
    cmd(d, SND_BGM_LOAD_CD | SND_LAST, BGM_LSN, BGM_SECTORS, 8);
    cmd(d, SND_BGM_START | SND_LAST, 1, 0, 4);
    sndrv_render(d, out, 48000 * 8);
    /* after its start, every 50 ms has sound: the ring was refilled in time */
    for (i = 48000 / 2; i < 48000 * 8; i += 2400) {
        if (rms(out + 2 * i, 2400) < 100) {
            gaps++;
        }
    }
    CHECK(gaps == 0, "the BGM has %u silent 50 ms stretches in 7.5 s", gaps);
    /* stopped: silent within a poll and the release */
    cmd(d, SND_BGM_STOP | SND_LAST, 0, 0, 0);
    sndrv_render(d, out, 9600);
    CHECK(rms(out + 2 * 4800, 4800) == 0, "the BGM doesn't stop (%f)", rms(out + 2 * 4800, 4800));
    sndrv_free(d);
    free(out);
    free(disc_data);
}

static void test_stream(const char *dir)
{
    char path[1024];
    uint8_t data[300];
    uint32_t release, i;
    SndEvent ev;
    SndLog *l;
    SndReader *r;
    char err[256];

    snprintf(path, sizeof(path), "%s/audio-test.ggxs", dir);
    for (i = 0; i < sizeof(data); i++) {
        data[i] = (uint8_t)i;
    }
    l = snd_log_create(path, 2);
    CHECK(l != NULL, "cannot write %s", path);
    if (!l) {
        return;
    }
    snd_log_cmd(l, 0, SND_INIT, NULL, 0);
    snd_log_cmd(l, 7, SND_SE_TRANS | SND_LAST, data, sizeof(data));
    snd_log_field(l, 7, 2);
    CHECK(snd_log_close(l) == 0, "writing the stream failed");
    r = snd_read_open(path, &release, err, sizeof(err));
    CHECK(r && release == 2, "reading it back: %s", r ? "" : err);
    if (!r) {
        return;
    }
    CHECK(snd_read_next(r, &ev) == 1 && ev.tag == 'C' && ev.cmd == SND_INIT && ev.size == 0, "the first record");
    CHECK(snd_read_next(r, &ev) == 1 && ev.tag == 'C' && ev.field == 7 && ev.cmd == (SND_SE_TRANS | SND_LAST) &&
              ev.size == sizeof(data) && !memcmp(ev.data, data, sizeof(data)), "the second record");
    CHECK(snd_read_next(r, &ev) == 1 && ev.tag == 'F' && ev.field == 7 && ev.vblanks == 2, "the field record");
    CHECK(snd_read_next(r, &ev) == 0 && ev.tag == 'E', "the end");
    snd_read_close(r);
    remove(path);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: ggx-audio-test <scratch dir>\n");
        return 2;
    }
    test_adpcm();
    test_envelope();
    test_volume_and_loop();
    test_reverb();
    test_driver();
    test_stream(argv[1]);
    if (failures) {
        fprintf(stderr, "ggx-audio-test: %d failures\n", failures);
        return 1;
    }
    printf("ggx-audio-test: all passed\n");
    return 0;
}
