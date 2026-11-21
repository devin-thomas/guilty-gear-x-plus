/* The SPU2 model (spu2.h). */
#include "spu2.h"

#include <string.h>

/* psx-spx, "SPU Voice Interpolation": the hardware's 4-point Gaussian table */
static const int16_t GAUSS[512] = {
    -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001,
    -0x001, -0x001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0001, 0x0001, 0x0001, 0x0002,
    0x0002, 0x0002, 0x0003, 0x0003, 0x0003, 0x0004, 0x0004, 0x0005, 0x0005, 0x0006, 0x0007, 0x0007, 0x0008, 0x0009,
    0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011, 0x0012, 0x0013, 0x0015, 0x0016, 0x0018,
    0x0019, 0x001B, 0x001C, 0x001E, 0x0020, 0x0021, 0x0023, 0x0025, 0x0027, 0x0029, 0x002C, 0x002E, 0x0030, 0x0033,
    0x0035, 0x0038, 0x003A, 0x003D, 0x0040, 0x0043, 0x0046, 0x0049, 0x004D, 0x0050, 0x0054, 0x0057, 0x005B, 0x005F,
    0x0063, 0x0067, 0x006B, 0x006F, 0x0074, 0x0078, 0x007D, 0x0082, 0x0087, 0x008C, 0x0091, 0x0096, 0x009C, 0x00A1,
    0x00A7, 0x00AD, 0x00B3, 0x00BA, 0x00C0, 0x00C7, 0x00CD, 0x00D4, 0x00DB, 0x00E3, 0x00EA, 0x00F2, 0x00FA, 0x0101,
    0x010A, 0x0112, 0x011B, 0x0123, 0x012C, 0x0135, 0x013F, 0x0148, 0x0152, 0x015C, 0x0166, 0x0171, 0x017B, 0x0186,
    0x0191, 0x019C, 0x01A8, 0x01B4, 0x01C0, 0x01CC, 0x01D9, 0x01E5, 0x01F2, 0x0200, 0x020D, 0x021B, 0x0229, 0x0237,
    0x0246, 0x0255, 0x0264, 0x0273, 0x0283, 0x0293, 0x02A3, 0x02B4, 0x02C4, 0x02D6, 0x02E7, 0x02F9, 0x030B, 0x031D,
    0x0330, 0x0343, 0x0356, 0x036A, 0x037E, 0x0392, 0x03A7, 0x03BC, 0x03D1, 0x03E7, 0x03FC, 0x0413, 0x042A, 0x0441,
    0x0458, 0x0470, 0x0488, 0x04A0, 0x04B9, 0x04D2, 0x04EC, 0x0506, 0x0520, 0x053B, 0x0556, 0x0572, 0x058E, 0x05AA,
    0x05C7, 0x05E4, 0x0601, 0x061F, 0x063E, 0x065C, 0x067C, 0x069B, 0x06BB, 0x06DC, 0x06FD, 0x071E, 0x0740, 0x0762,
    0x0784, 0x07A7, 0x07CB, 0x07EF, 0x0813, 0x0838, 0x085D, 0x0883, 0x08A9, 0x08D0, 0x08F7, 0x091E, 0x0946, 0x096F,
    0x0998, 0x09C1, 0x09EB, 0x0A16, 0x0A40, 0x0A6C, 0x0A98, 0x0AC4, 0x0AF1, 0x0B1E, 0x0B4C, 0x0B7A, 0x0BA9, 0x0BD8,
    0x0C07, 0x0C38, 0x0C68, 0x0C99, 0x0CCB, 0x0CFD, 0x0D30, 0x0D63, 0x0D97, 0x0DCB, 0x0E00, 0x0E35, 0x0E6B, 0x0EA1,
    0x0ED7, 0x0F0F, 0x0F46, 0x0F7F, 0x0FB7, 0x0FF1, 0x102A, 0x1065, 0x109F, 0x10DB, 0x1116, 0x1153, 0x118F, 0x11CD,
    0x120B, 0x1249, 0x1288, 0x12C7, 0x1307, 0x1347, 0x1388, 0x13C9, 0x140B, 0x144D, 0x1490, 0x14D4, 0x1517, 0x155C,
    0x15A0, 0x15E6, 0x162C, 0x1672, 0x16B9, 0x1700, 0x1747, 0x1790, 0x17D8, 0x1821, 0x186B, 0x18B5, 0x1900, 0x194B,
    0x1996, 0x19E2, 0x1A2E, 0x1A7B, 0x1AC8, 0x1B16, 0x1B64, 0x1BB3, 0x1C02, 0x1C51, 0x1CA1, 0x1CF1, 0x1D42, 0x1D93,
    0x1DE5, 0x1E37, 0x1E89, 0x1EDC, 0x1F2F, 0x1F82, 0x1FD6, 0x202A, 0x207F, 0x20D4, 0x2129, 0x217F, 0x21D5, 0x222C,
    0x2282, 0x22DA, 0x2331, 0x2389, 0x23E1, 0x2439, 0x2492, 0x24EB, 0x2545, 0x259E, 0x25F8, 0x2653, 0x26AD, 0x2708,
    0x2763, 0x27BE, 0x281A, 0x2876, 0x28D2, 0x292E, 0x298B, 0x29E7, 0x2A44, 0x2AA1, 0x2AFF, 0x2B5C, 0x2BBA, 0x2C18,
    0x2C76, 0x2CD4, 0x2D33, 0x2D91, 0x2DF0, 0x2E4F, 0x2EAE, 0x2F0D, 0x2F6C, 0x2FCC, 0x302B, 0x308B, 0x30EA, 0x314A,
    0x31AA, 0x3209, 0x3269, 0x32C9, 0x3329, 0x3389, 0x33E9, 0x3449, 0x34A9, 0x3509, 0x3569, 0x35C9, 0x3629, 0x3689,
    0x36E8, 0x3748, 0x37A8, 0x3807, 0x3867, 0x38C6, 0x3926, 0x3985, 0x39E4, 0x3A43, 0x3AA2, 0x3B00, 0x3B5F, 0x3BBD,
    0x3C1B, 0x3C79, 0x3CD7, 0x3D35, 0x3D92, 0x3DEF, 0x3E4C, 0x3EA9, 0x3F05, 0x3F62, 0x3FBD, 0x4019, 0x4074, 0x40D0,
    0x412A, 0x4185, 0x41DF, 0x4239, 0x4292, 0x42EB, 0x4344, 0x439C, 0x43F4, 0x444C, 0x44A3, 0x44FA, 0x4550, 0x45A6,
    0x45FC, 0x4651, 0x46A6, 0x46FA, 0x474E, 0x47A1, 0x47F4, 0x4846, 0x4898, 0x48E9, 0x493A, 0x498A, 0x49D9, 0x4A29,
    0x4A77, 0x4AC5, 0x4B13, 0x4B5F, 0x4BAC, 0x4BF7, 0x4C42, 0x4C8D, 0x4CD7, 0x4D20, 0x4D68, 0x4DB0, 0x4DF7, 0x4E3E,
    0x4E84, 0x4EC9, 0x4F0E, 0x4F52, 0x4F95, 0x4FD7, 0x5019, 0x505A, 0x509A, 0x50DA, 0x5118, 0x5156, 0x5194, 0x51D0,
    0x520C, 0x5247, 0x5281, 0x52BA, 0x52F3, 0x532A, 0x5361, 0x5397, 0x53CC, 0x5401, 0x5434, 0x5467, 0x5499, 0x54CA,
    0x54FA, 0x5529, 0x5558, 0x5585, 0x55B2, 0x55DE, 0x5609, 0x5632, 0x565B, 0x5684, 0x56AB, 0x56D1, 0x56F6, 0x571B,
    0x573E, 0x5761, 0x5782, 0x57A3, 0x57C3, 0x57E2, 0x57FF, 0x581C, 0x5838, 0x5853, 0x586D, 0x5886, 0x589E, 0x58B5,
    0x58CB, 0x58E0, 0x58F4, 0x5907, 0x5919, 0x592A, 0x593A, 0x5949, 0x5958, 0x5965, 0x5971, 0x597C, 0x5986, 0x598F,
    0x5997, 0x599E, 0x59A4, 0x59A9, 0x59AD, 0x59B0, 0x59B2, 0x59B3,
};

/* the ADPCM prediction filters; 5-15 predict 0 */
static const int32_t FILTER[16][2] = { { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 } };

/* The reverb's resampling filter, 39 taps (odd ones zero but the middle), padded to 40, the SIMD width.
   Upsampling uses it doubled, the middle tap saturating at 0x7FFF. */
static const int16_t FIR_DOWN[40] = {
    -1, 0, 2, 0, -10, 0, 35, 0, -103, 0, 266, 0, -616, 0, 1332, 0, -2960, 0, 10246, 16384,
    10246, 0, -2960, 0, 1332, 0, -616, 0, 266, 0, -103, 0, 35, 0, -10, 0, 2, 0, -1, 0,
};
static const int16_t FIR_UP[40] = {
    -2, 0, 4, 0, -20, 0, 70, 0, -206, 0, 532, 0, -1232, 0, 2664, 0, -5920, 0, 20492, 32767,
    20492, 0, -5920, 0, 2664, 0, -1232, 0, 532, 0, -206, 0, 70, 0, -20, 0, 4, 0, -2, 0,
};

/* the noise generator's tables (psx-spx, "SPU Noise Generator") */
static const uint8_t NOISE_BIT[64] = {
    1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1,
};
static const uint16_t NOISE_FREQ[5] = { 0, 84, 140, 180, 210 };

#define FLAG_LOOP_END 1
#define FLAG_LOOP 2
#define FLAG_LOOP_START 4
/* Below this, sound RAM holds the SPU2's own buffers (the cores' outputs, ADMA input): blocks decoded
   from there are never kept for reuse. */
#define DYNAMIC_END 0x2800u

static int32_t clamp16(int32_t v)
{
    return v < -0x8000 ? -0x8000 : v > 0x7fff ? 0x7fff : v;
}

static int32_t clamp(int32_t v, int32_t lo, int32_t hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

/* a volume (or envelope level) applied: 1.15 fixed point, rounded down */
static int32_t apply(int32_t data, int32_t volume)
{
    return (data * volume) >> 15;
}

/* the reverb's products, whose sums can pass 32 bits and wrap */
static int32_t mulw(int32_t a, int32_t b)
{
    return (int32_t)((uint32_t)a * (uint32_t)b) >> 15;
}

/* one step of an envelope or volume slide: how much the counter advances and the level moves */
static void rate(int shift, int step, int decr, int expo, int32_t level, uint32_t *inc, int32_t *delta)
{
    *inc = 0x8000u >> (shift > 11 ? shift - 11 : 0);
    *delta = step * (1 << (shift < 11 ? 11 - shift : 0));
    if (expo) {
        if (!decr && level > 0x6000) {
            *inc >>= 2;
        }
        if (decr) {
            *delta = (int16_t)((*delta * level) >> 15);
        }
    }
}

void spu2_vol_set(Spu2Vol *v, uint16_t reg)
{
    v->reg = reg;
    if (!(reg & 0x8000)) {
        v->value = (int16_t)(reg << 1);
    }
}

/* A volume register with bit 15 set slides: bits 0-1 the step, 2-6 the shift, 12 the phase (toward
   negative), 13 decreasing, 14 exponential. */
static void vol_update(Spu2Vol *v)
{
    int step, shift, phase, decr, expo;
    uint32_t inc;
    int32_t delta;

    if (!(v->reg & 0x8000)) {
        return;
    }
    step = v->reg & 3;
    shift = (v->reg >> 2) & 0x1f;
    phase = (v->reg >> 12) & 1;
    decr = (v->reg >> 13) & 1;
    expo = (v->reg >> 14) & 1;
    rate(shift, decr ? ~(7 - step) : 7 - step, decr, expo, v->value, &inc, &delta);
    if (step != 3 && shift != 0x1f && !inc) {
        inc = 1;
    }
    v->counter += inc;
    if (!(expo && decr) && phase) {
        delta = -delta;
    }
    if (v->counter >= 0x8000) {
        v->counter = 0;
        if (!decr) {
            v->value = clamp16(v->value + delta);
        } else if (expo) {
            v->value = clamp(v->value + delta, 0, 0x7fff);
        } else {
            v->value = phase ? clamp(v->value + delta, -0x8000, 0) : clamp(v->value + delta, 0, 0x7fff);
        }
    }
}

static void voice_stop(Spu2Voice *v)
{
    v->env = 0;
    v->phase = ENV_OFF;
}

/* An envelope's rates from ADSR1 and ADSR2. ADSR1: bits 0-3 the sustain level, 4-7 the decay shift, 8-9 the
   attack step, 10-14 its shift, 15 exponential attack. ADSR2: bits 0-4 the release shift, 5 exponential
   release, 6-7 the sustain step, 8-12 its shift, 14 sustain decreasing, 15 exponential sustain. */
void spu2_adsr_set(Spu2Voice *v)
{
    int sus = 7 - ((v->adsr2 >> 6) & 3);

    v->rate[ENV_ATTACK].decr = 0;
    v->rate[ENV_ATTACK].expo = (int8_t)(v->adsr1 >> 15);
    v->rate[ENV_ATTACK].shift = (int8_t)((v->adsr1 >> 10) & 0x1f);
    v->rate[ENV_ATTACK].step = (int8_t)(7 - ((v->adsr1 >> 8) & 3));
    v->rate[ENV_ATTACK].target = 0x7fff;
    v->rate[ENV_DECAY].decr = 1;
    v->rate[ENV_DECAY].expo = 1;
    v->rate[ENV_DECAY].shift = (int8_t)((v->adsr1 >> 4) & 0xf);
    v->rate[ENV_DECAY].step = -8;
    v->rate[ENV_DECAY].target = ((v->adsr1 & 0xf) + 1) << 11;
    v->rate[ENV_SUSTAIN].decr = (int8_t)((v->adsr2 >> 14) & 1);
    v->rate[ENV_SUSTAIN].expo = (int8_t)(v->adsr2 >> 15);
    v->rate[ENV_SUSTAIN].shift = (int8_t)((v->adsr2 >> 8) & 0x1f);
    v->rate[ENV_SUSTAIN].step = (int8_t)(((v->adsr2 >> 14) & 1) ? ~sus : sus);
    v->rate[ENV_SUSTAIN].target = 0;
    v->rate[ENV_RELEASE].decr = 1;
    v->rate[ENV_RELEASE].expo = (int8_t)((v->adsr2 >> 5) & 1);
    v->rate[ENV_RELEASE].shift = (int8_t)(v->adsr2 & 0x1f);
    v->rate[ENV_RELEASE].step = -8;
    v->rate[ENV_RELEASE].target = 0;
}

/* One envelope step; 0 when the envelope has ended. */
static int adsr_update(Spu2Voice *v)
{
    int decr = v->rate[v->phase].decr;
    int32_t delta;
    uint32_t inc;

    rate(v->rate[v->phase].shift, v->rate[v->phase].step, decr, v->rate[v->phase].expo, v->env, &inc, &delta);
    v->env_counter += inc ? inc : 1;
    if (v->env_counter >= 0x8000) {
        v->env_counter = 0;
        v->env = clamp(v->env + delta, 0, 0x7fff);
    }
    if (v->phase == ENV_SUSTAIN) {
        return v->env != 0;   /* sustain lasts until key off or silence */
    }
    if (decr ? v->env <= v->rate[v->phase].target : v->env >= v->rate[v->phase].target) {
        v->phase++;
    }
    return v->phase <= ENV_RELEASE;
}

void spu2_key_on(Spu2 *s, int core, uint32_t voices)
{
    Spu2Core *c = &s->core[core];
    int i;

    c->endx &= ~voices;
    for (i = 0; i < SPU2_VOICES; i++) {
        Spu2Voice *v = &c->v[i];
        if (!(voices >> i & 1)) {
            continue;
        }
        v->phase = ENV_ATTACK;
        v->env = 0;
        v->env_counter = 0;
        v->loop_mode = 0;
        v->counter = 0;
        v->flags = 0;
        v->nax = v->ssa | 1;
        v->prev[0] = v->prev[1] = 0;
        v->block = -1;
        v->fifo_read = v->fifo_write = 0;
    }
}

void spu2_key_off(Spu2 *s, int core, uint32_t voices)
{
    int i;

    for (i = 0; i < SPU2_VOICES; i++) {
        Spu2Voice *v = &s->core[core].v[i];
        if ((voices >> i & 1) && v->phase != ENV_OFF) {
            v->phase = ENV_RELEASE;
            v->env_counter = 0;
        }
    }
}

static void discard_blocks(Spu2 *s, uint32_t addr, uint32_t n)
{
    uint32_t b;

    for (b = addr / 8; b < (addr + n + 7) / 8; b++) {
        s->blocks[b & (SPU2_RAM_WORDS / 8 - 1)].valid = 0;
    }
}

void spu2_write_words(Spu2 *s, uint32_t addr, const uint16_t *words, uint32_t n)
{
    uint32_t i;

    if (s->on_ram) {
        s->on_ram(s->on_ctx, s, addr, words, n);
    }
    for (i = 0; i < n; i++) {
        s->ram[(addr + i) & (SPU2_RAM_WORDS - 1)] = words[i];
    }
    discard_blocks(s, addr, n);
}

void spu2_write_ram(Spu2 *s, uint32_t addr, const uint8_t *data, uint32_t size)
{
    static uint16_t w[0x8000];
    uint32_t i, k, n;

    for (k = 0; k < size / 2; k += n) {
        n = size / 2 - k < 0x8000 ? size / 2 - k : 0x8000;
        for (i = 0; i < n; i++) {
            w[i] = (uint16_t)(data[2 * (k + i)] | data[2 * (k + i) + 1] << 8);
        }
        spu2_write_words(s, (addr >> 1) + k, w, n);
    }
}

static void decode(const Spu2 *s, uint32_t b, int16_t pcm[28], int32_t prev[2])
{
    const uint16_t *blk = &s->ram[b * 8];
    int header = (int16_t)blk[0], shift = (header & 0xf) + 16, k;
    const int32_t *f = FILTER[(header >> 4) & 0xf];

    for (k = 0; k < 28; k++) {
        uint32_t nib = (uint32_t)(blk[1 + k / 4] >> (4 * (k & 3))) & 0xf;
        int32_t x = ((int32_t)(nib << 28) >> shift) + ((f[0] * prev[0] + f[1] * prev[1] + 32) >> 6);
        pcm[k] = (int16_t)clamp16(x);
        prev[1] = prev[0];
        prev[0] = pcm[k];
    }
}

/* Four of the block's samples into the voice's FIFO, decoding the block on entering it. */
static void fetch(Spu2 *s, Spu2Voice *v)
{
    const int16_t *pcm;
    int i, k;

    if (v->block < 0) {
        Spu2Block *b = &s->blocks[(v->nax & 0xffff8) / 8];
        v->block = (int32_t)((v->nax & 0xffff8) / 8);
        if (b->valid && b->prev[0] == v->prev[0] && b->prev[1] == v->prev[1]) {
            v->prev[0] = b->pcm[27];
            v->prev[1] = b->pcm[26];
        } else {
            if (v->nax >= DYNAMIC_END) {
                b->valid = 1;
                b->prev[0] = (int16_t)v->prev[0];
                b->prev[1] = (int16_t)v->prev[1];
            }
            decode(s, (uint32_t)v->block, b->pcm, v->prev);
        }
    }
    /* the samples at NAX, from the block decoded (which NAX written since may have left) */
    pcm = s->blocks[v->block].pcm;
    k = (int)((v->nax + 7) & 7) * 4;
    for (i = 0; i < 4; i++) {
        v->fifo[(v->fifo_write + i) % 32] = pcm[(k + i) % 28];
    }
}

/* The decoder's step, every sample, the voice playing or not: NAX moves on a word whenever the FIFO has
   room, past the block's header at its end, and to LSA at a block flagged loop-end. */
static void decode_step(Spu2 *s, int core, int i)
{
    Spu2Core *c = &s->core[core];
    Spu2Voice *v = &c->v[i];

    v->flags = (uint8_t)(s->ram[v->nax & 0xffff8] >> 8);
    if ((v->flags & FLAG_LOOP_START) && !v->loop_mode) {
        v->lsa = v->nax & 0xffff8;
    }
    if ((int32_t)(v->fifo_write - v->fifo_read) > 12) {
        return;
    }
    if (v->phase != ENV_OFF) {
        fetch(s, v);
    }
    v->fifo_write += 4;
    v->nax = (v->nax + 1) & 0xfffff;
    if ((v->nax & 7) == 0) {
        if (v->flags & FLAG_LOOP_END) {
            c->endx |= 1u << i;
            v->nax = v->lsa;
            if (!(v->flags & FLAG_LOOP)) {
                voice_stop(v);
            }
        }
        v->nax = (v->nax + 1) & 0xfffff;
        v->block = -1;
    }
}

static int32_t interpolate(const Spu2Voice *v)
{
    int p = (v->counter & 0xff0) >> 4;
    uint32_t r = v->fifo_read;

    return ((GAUSS[0xff - p] * v->fifo[r % 32]) >> 15) + ((GAUSS[0x1ff - p] * v->fifo[(r + 1) % 32]) >> 15) +
           ((GAUSS[0x100 + p] * v->fifo[(r + 2) % 32]) >> 15) + ((GAUSS[p] * v->fifo[(r + 3) % 32]) >> 15);
}

/* One voice's sample, left and right, before the core's voice gates. */
static void voice_tick(Spu2 *s, int core, int i, int32_t out[2])
{
    Spu2Core *c = &s->core[core];
    Spu2Voice *v = &c->v[i];
    int32_t value = 0, pitch;

    vol_update(&v->vol[0]);
    vol_update(&v->vol[1]);
    decode_step(s, core, i);
    out[0] = out[1] = 0;
    if (v->phase != ENV_OFF) {
        value = v->noise ? (int16_t)c->noise_out : interpolate(v);
        if (!adsr_update(v)) {
            voice_stop(v);
        }
        value = apply(value, v->env);
        v->outx = value;
        if (!(s->mute[core] >> i & 1)) {
            out[0] = apply(value, v->vol[0].value);
            out[1] = apply(value, v->vol[1].value);
        }
    }
    pitch = v->pitch;
    if (v->modulated && i > 0) {
        pitch = clamp((pitch * (0x8000 + c->v[i - 1].outx)) >> 15, 0, 0x3fff);
    }
    v->counter += pitch < 0x3fff ? pitch : 0x3fff;
    v->fifo_read += (uint32_t)(v->counter >> 12);
    v->counter &= 0xfff;
    if (s->on_voice) {
        s->on_voice(s->on_ctx, s, core, i, value, out);
    }
    /* voices 1 and 3 show their output in sound RAM */
    if (i == 1 || i == 3) {
        s->ram[(core ? 0xc00 : 0x400) + (i == 3 ? 0x200 : 0) + s->out_pos] = (uint16_t)value;
    }
}

static void noise_update(Spu2Core *c)
{
    int clk = (c->attr >> 8) & 0x3f;
    uint32_t level = (0x8000u >> (clk >> 2)) << 16;

    c->noise_count += 0x10000 + NOISE_FREQ[clk & 3];
    if ((c->noise_count & 0xffff) >= NOISE_FREQ[4]) {
        c->noise_count += 0x10000 - NOISE_FREQ[clk & 3];
    }
    if (c->noise_count >= level) {
        while (c->noise_count >= level) {
            c->noise_count -= level;
        }
        c->noise_out = (c->noise_out << 1) | NOISE_BIT[(c->noise_out >> 10) & 63];
    }
}

/* The resampling filter as PCSX2 runs it with SSE4: each product rounded to 16 bits, then summed in
   eight lanes and pairwise, saturating at every add. */
static int16_t sat(int32_t v)
{
    return (int16_t)clamp16(v);
}

static int32_t fir(const int16_t *x, const int16_t *coef)
{
    int16_t acc[8];
    int j, k;

    for (j = 0; j < 8; j++) {
        acc[j] = (int16_t)((x[j] * coef[j] + 0x4000) >> 15);
    }
    for (k = 8; k < 40; k += 8) {
        for (j = 0; j < 8; j++) {
            acc[j] = sat(acc[j] + ((x[k + j] * coef[k + j] + 0x4000) >> 15));
        }
    }
    return sat(sat(sat(acc[0] + acc[1]) + sat(acc[2] + acc[3])) + sat(sat(acc[4] + acc[5]) + sat(acc[6] + acc[7])));
}

/* A reverb work-area address: the offset from the area's start, moving on one word every two samples */
static uint32_t rev_addr(const Spu2 *s, const Spu2Core *c, uint32_t offset)
{
    uint32_t start = c->esa & 0x3fffff, end = (c->eea & 0x3fffff) | 0xffff;

    return (((s->cycles >> 1) + offset) % (end - start + 1) + start) & 0xfffff;
}

/* The reverb, at 24 kHz: even samples (by Spu2.cycles) compute the left side, odd ones the right. */
static void reverb(Spu2 *s, Spu2Core *c, const int32_t in[2], int32_t out[2])
{
    const uint32_t *r = c->rev;
    int16_t *m = (int16_t *)s->ram;
    int R = s->cycles & 1, k;
    uint32_t pos = c->rev_pos, same_src, same_dst, same_prv, diff_src, diff_dst, diff_prv, comb[4], apf1_src, apf1_dst,
             apf2_src, apf2_dst;
    int32_t x, same, diff, apf1, apf2, o;
#define V(n) ((int32_t)(int16_t)r[n])

    memset(c->rev_last, 0, sizeof(c->rev_last));
    if (c->esa >= c->eea) {
        out[0] = out[1] = 0;
        return;
    }
    for (k = 0; k < 2; k++) {
        c->rev_down[k][pos] = c->rev_down[k][pos | 64] = (int16_t)clamp16(in[k]);
    }
    same_src = rev_addr(s, c, r[R ? REV_SAME_R_SRC : REV_SAME_L_SRC]);
    same_dst = rev_addr(s, c, r[R ? REV_SAME_R_DST : REV_SAME_L_DST]);
    same_prv = rev_addr(s, c, r[R ? REV_SAME_R_DST : REV_SAME_L_DST] - 1);
    diff_src = rev_addr(s, c, r[R ? REV_DIFF_L_SRC : REV_DIFF_R_SRC]);
    diff_dst = rev_addr(s, c, r[R ? REV_DIFF_R_DST : REV_DIFF_L_DST]);
    diff_prv = rev_addr(s, c, r[R ? REV_DIFF_R_DST : REV_DIFF_L_DST] - 1);
    comb[0] = rev_addr(s, c, r[R ? REV_COMB1_R : REV_COMB1_L]);
    comb[1] = rev_addr(s, c, r[R ? REV_COMB2_R : REV_COMB2_L]);
    comb[2] = rev_addr(s, c, r[R ? REV_COMB3_R : REV_COMB3_L]);
    comb[3] = rev_addr(s, c, r[R ? REV_COMB4_R : REV_COMB4_L]);
    apf1_dst = rev_addr(s, c, r[R ? REV_APF1_R_DST : REV_APF1_L_DST]);
    apf1_src = rev_addr(s, c, r[R ? REV_APF1_R_DST : REV_APF1_L_DST] - r[REV_APF1_SIZE]);
    apf2_dst = rev_addr(s, c, r[R ? REV_APF2_R_DST : REV_APF2_L_DST]);
    apf2_src = rev_addr(s, c, r[R ? REV_APF2_R_DST : REV_APF2_L_DST] - r[REV_APF2_SIZE]);

    x = mulw(V(R ? REV_IN_R : REV_IN_L), fir(&c->rev_down[R][(pos - 39) & 63], FIR_DOWN));
    same = mulw(V(REV_IIR_VOL), x + mulw(V(REV_WALL_VOL), m[same_src]) - m[same_prv]) + m[same_prv];
    diff = mulw(V(REV_IIR_VOL), x + mulw(V(REV_WALL_VOL), m[diff_src]) - m[diff_prv]) + m[diff_prv];
    o = mulw(V(REV_COMB1_VOL), m[comb[0]]) + mulw(V(REV_COMB2_VOL), m[comb[1]]) + mulw(V(REV_COMB3_VOL), m[comb[2]]) +
        mulw(V(REV_COMB4_VOL), m[comb[3]]);
    apf1 = o - mulw(V(REV_APF1_VOL), m[apf1_src]);
    o = m[apf1_src] + mulw(V(REV_APF1_VOL), apf1);
    apf2 = o - mulw(V(REV_APF2_VOL), m[apf2_src]);
    o = m[apf2_src] + mulw(V(REV_APF2_VOL), apf2);
    if (c->attr & 0x80) {
        /* the reverb always runs; ATTR bit 7 lets it write its work area */
        m[same_dst] = (int16_t)clamp16(same);
        m[diff_dst] = (int16_t)clamp16(diff);
        m[apf1_dst] = (int16_t)clamp16(apf1);
        m[apf2_dst] = (int16_t)clamp16(apf2);
    }
#undef V
    {
        const int32_t last[12] = { x, same, diff, apf1, apf2, clamp16(o), (int32_t)same_dst, (int32_t)diff_dst,
                                   (int32_t)apf1_dst, (int32_t)apf2_dst, (int32_t)diff_src, (int32_t)diff_prv };
        memcpy(c->rev_last, last, sizeof(last));
    }
    c->rev_up[R][pos] = c->rev_up[R][pos | 64] = (int16_t)clamp16(o);
    c->rev_up[!R][pos] = c->rev_up[!R][pos | 64] = 0;
    pos = c->rev_pos = (pos + 1) & 63;
    out[0] = fir(&c->rev_up[0][(pos - 39) & 63], FIR_UP);
    out[1] = fir(&c->rev_up[1][(pos - 39) & 63], FIR_UP);
}

/* The gates in Spu2Core.gates, MMIX's bits */
enum {
    G_WET_EXT_R = 1 << 0, G_WET_EXT_L = 1 << 1, G_DRY_EXT_R = 1 << 2, G_DRY_EXT_L = 1 << 3,
    G_WET_IN_R = 1 << 4, G_WET_IN_L = 1 << 5, G_DRY_IN_R = 1 << 6, G_DRY_IN_L = 1 << 7,
    G_WET_VOICE_R = 1 << 8, G_WET_VOICE_L = 1 << 9, G_DRY_VOICE_R = 1 << 10, G_DRY_VOICE_L = 1 << 11,
};

static int32_t gate(const Spu2Core *c, int g, int32_t v)
{
    return (c->gates & g) ? v : 0;
}

/* A core's mix of its voices' dry and wet sums, its input and the external input, through its reverb. */
static void core_mix(Spu2 *s, int core, const int32_t dry_in[2], const int32_t wet_in[2], const int32_t in[2],
                     const int32_t ext[2], int32_t out[2])
{
    Spu2Core *c = &s->core[core];
    int32_t dry[2], wet[2], td[2], tw[2], rv[2];
    uint32_t base = core ? 0x1800 : 0x1000;
    int k;

    vol_update(&c->mvol[0]);
    vol_update(&c->mvol[1]);
    noise_update(c);
    for (k = 0; k < 2; k++) {
        dry[k] = clamp16(dry_in[k]);
        wet[k] = clamp16(wet_in[k]);
    }
    s->ram[base + s->out_pos] = (uint16_t)dry[0];
    s->ram[base + 0x200 + s->out_pos] = (uint16_t)dry[1];
    s->ram[base + 0x400 + s->out_pos] = (uint16_t)wet[0];
    s->ram[base + 0x600 + s->out_pos] = (uint16_t)wet[1];
    td[0] = gate(c, G_DRY_IN_L, in[0]) + gate(c, G_DRY_VOICE_L, dry[0]) + gate(c, G_DRY_EXT_L, ext[0]);
    td[1] = gate(c, G_DRY_IN_R, in[1]) + gate(c, G_DRY_VOICE_R, dry[1]) + gate(c, G_DRY_EXT_R, ext[1]);
    tw[0] = gate(c, G_WET_IN_L, in[0]) + gate(c, G_WET_VOICE_L, wet[0]) + gate(c, G_WET_EXT_L, ext[0]);
    tw[1] = gate(c, G_WET_IN_R, in[1]) + gate(c, G_WET_VOICE_R, wet[1]) + gate(c, G_WET_EXT_R, ext[1]);
    reverb(s, c, tw, rv);
    out[0] = td[0] + apply(rv[0], c->evol[0]);
    out[1] = td[1] + apply(rv[1], c->evol[1]);
    if (s->on_core) {
        const int32_t mix[8] = { dry[0], dry[1], wet[0], wet[1], rv[0], rv[1], out[0], out[1] };
        s->on_core(s->on_ctx, s, core, mix);
    }
}

void spu2_tick(Spu2 *s, int16_t out[2])
{
    int32_t in[2][2] = { { 0, 0 }, { 0, 0 } }, dry[2][2] = { { 0, 0 }, { 0, 0 } }, wet[2][2] = { { 0, 0 }, { 0, 0 } };
    int32_t ext[2], o[2], vo[2];
    int core, i, k;

    s->cycles++;
    for (core = 0; core < 2; core++) {
        Spu2Core *c = &s->core[core];
        if (c->koff) {
            spu2_key_off(s, core, c->koff);
            c->koff = 0;
        }
        if (c->kon) {
            spu2_key_on(s, core, c->kon);
            c->kon = 0;
        }
    }
    /* the sound data inputs, from the ADMA buffers */
    for (core = 0; core < 2; core++) {
        if (core == 0 ? !(s->play_mode & 2) : !(s->play_mode & 8)) {
            uint32_t a = 0x2000 + ((uint32_t)core << 10) + s->out_pos;
            in[core][0] = apply((int16_t)s->ram[a], s->core[core].bvol[0]);
            in[core][1] = apply((int16_t)s->ram[a + 0x200], s->core[core].bvol[1]);
        }
    }
    for (core = 0; core < 2; core++) {
        Spu2Core *c = &s->core[core];
        for (i = 0; i < SPU2_VOICES; i++) {
            voice_tick(s, core, i, vo);
            dry[core][0] += (c->vmixl >> i & 1) ? vo[0] : 0;
            dry[core][1] += (c->vmixr >> i & 1) ? vo[1] : 0;
            wet[core][0] += (c->vmixel >> i & 1) ? vo[0] : 0;
            wet[core][1] += (c->vmixer >> i & 1) ? vo[1] : 0;
        }
    }
    ext[0] = ext[1] = 0;
    core_mix(s, 0, dry[0], wet[0], in[0], ext, o);
    for (k = 0; k < 2; k++) {
        ext[k] = (s->play_mode & 4) ? 0 : apply(clamp16(o[k]), s->core[0].mvol[k].value);
    }
    s->ram[0x800 + s->out_pos] = (uint16_t)ext[0];
    s->ram[0xa00 + s->out_pos] = (uint16_t)ext[1];
    for (k = 0; k < 2; k++) {
        ext[k] = apply(ext[k], s->core[1].avol[k]);
    }
    core_mix(s, 1, dry[1], wet[1], in[1], ext, o);
    for (k = 0; k < 2; k++) {
        /* (PCM-direct output, SPDIF mode 8, isn't modelled: it plays nothing of the SPU2's) */
        out[k] = (s->play_mode & 8) ? 0 : (int16_t)clamp16(apply(clamp16(o[k]), s->core[1].mvol[k].value));
    }
    s->out_pos = (s->out_pos + 1) & 0x1ff;
    if (s->on_sample) {
        s->on_sample(s->on_ctx, s, out);
    }
}

void spu2_reset(Spu2 *s)
{
    int core, i;

    memset(s, 0, sizeof(*s));
    /* left by the BIOS: two blocks flagged to loop on themselves, where voices start out */
    for (i = 0; i < 8; i++) {
        s->ram[0x2800 + i] = s->ram[0xe870 + i] = 0x0707;
    }
    for (core = 0; core < 2; core++) {
        Spu2Core *c = &s->core[core];
        c->avol[0] = c->avol[1] = 0x7fff;
        c->bvol[0] = c->bvol[1] = 0x7fff;
        c->mmix = core ? 0xffc : 0xff0;
        /* every source gated in, but core 0's external input (it has none) and core 1's dry one: not
           what MMIX reads as, until it is written */
        c->gates = core ? 0xff3 : 0xff0;
        c->vmixl = c->vmixr = c->vmixel = c->vmixer = 0xffffff;
        c->esa = core ? 0xffff8 : 0xefff8;
        c->eea = core ? 0xfffff : 0xeffff;
        c->irqa = 0x800;
        c->statx = 0x80;
        c->endx = 0xffffff;
        for (i = 0; i < SPU2_VOICES; i++) {
            Spu2Voice *v = &c->v[i];
            v->pitch = 0x3fff;
            v->nax = 0x2801;
            v->ssa = v->lsa = 0x2800;
            v->block = -1;
        }
    }
}
