/* The SPU2, the PS2's sound processor (PORT-011, ADR-011).
 *
 * Two cores of 24 voices share 2 MB of sound RAM. A voice decodes SPU-ADPCM
 * (16-byte blocks of 28 samples) four samples at a time into a small FIFO,
 * steps through it at its pitch with 4-point Gaussian interpolation, and
 * applies an ADSR envelope and a left and right volume. Each core mixes its
 * voices, its inputs and its reverb; core 0's output is core 1's external
 * input, and core 1's is what the PS2 plays, 16-bit stereo at 48 kHz, one
 * sample per spu2_tick.
 *
 * The model is bit-exact to PCSX2 2.8.2's SPU2: the same register semantics, decode pipeline, envelope
 * and volume-slide steps, mixing, clamps and reverb, down to where results
 * are rounded. It is written from observing that behaviour, not from its
 * code. Software drives it through the registers (spu2_write), as the IOP
 * does. Addresses are in 16-bit words, as the registers hold them.
 */
#ifndef GGX_SPU2_H
#define GGX_SPU2_H

#include <stdint.h>

#define SPU2_RAM_WORDS 0x100000u   /* 2 MB */
#define SPU2_VOICES 24
#define SPU2_RATE 48000

enum { ENV_OFF, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE };

/* A volume register and the volume it gives: set directly (bit 15 clear) or
   slid, by the same steps as the envelope's. */
typedef struct {
    uint16_t reg;
    int32_t value;
    uint32_t counter;
} Spu2Vol;

typedef struct {
    /* registers */
    Spu2Vol vol[2];
    uint16_t pitch;
    uint16_t adsr1, adsr2;
    uint32_t ssa, lsa, nax;   /* start, loop and next addresses, words */
    int modulated, noise;     /* PMON, NON */
    /* the decoder: the flags of the block at NAX, the decoded block it reads from, and the FIFO the
       interpolator reads; the pitch counter's bits 12 up step the FIFO, 4-11 are the interpolation index */
    int loop_mode;            /* LSAX was written since the key on: the blocks' loop-start flags don't move it */
    uint8_t flags;
    int32_t block;            /* the decoded block (Spu2.blocks) the voice is in, or -1 */
    int32_t prev[2];
    int16_t fifo[32];
    uint32_t fifo_read, fifo_write;
    int32_t counter;
    /* the envelope; its phases' rates, taken from ADSR1 and ADSR2 when either is written (and zero, which
       ends every phase at once, until one is) */
    struct {
        int8_t decr, expo, shift, step;
        int32_t target;
    } rate[5];
    int phase;                /* ENV_* */
    int32_t env;              /* ENVX */
    uint32_t env_counter;
    int32_t outx;             /* the last output after the envelope, for the next voice's pitch modulation */
} Spu2Voice;

typedef struct {
    Spu2Voice v[SPU2_VOICES];
    uint32_t pmon, non, endx;
    uint32_t vmixl, vmixr, vmixel, vmixer;   /* voices to the dry and wet (reverb) mixes */
    uint32_t kon, koff;       /* written, applied at the next sample (key off first) */
    uint16_t mmix;            /* MMIX as written */
    uint16_t gates;           /* the sources that reach the dry and wet mixes, in MMIX's layout */
    uint16_t attr;            /* bit 7: reverb writes; bits 8-13: the noise clock */
    uint16_t admas, statx;
    Spu2Vol mvol[2];          /* main volume */
    int32_t evol[2];          /* reverb return */
    int32_t avol[2];          /* external input: core 0's output, on core 1 */
    int32_t bvol[2];          /* sound data input (ADMA) */
    uint32_t esa, eea;        /* the reverb work area's start and end, as written */
    uint32_t irqa, tsa;
    uint32_t noise_count, noise_out;
    uint32_t rev[32];         /* reverb registers, REV_*: addresses, and volumes in the low 16 bits */
    int16_t rev_down[2][128]; /* the resampling filters' inputs, each written twice (64 apart) */
    int16_t rev_up[2][128];
    uint32_t rev_pos;
    int32_t rev_last[12];     /* the reverb's last step, for analysis: in, same, diff, apf1, apf2, out, the four
                                 addresses written, 0, 0; zero when it didn't run */
    uint16_t raw[0x200];      /* registers with no effect, as written, for reads */
} Spu2Core;

/* A block of sound RAM, decoded. The SPU2 keeps one per block, shared by
   every voice that plays it; a voice entering a block whose decode began
   from the same two previous samples reuses it, and one that doesn't
   decodes it again, for every voice reading it. A write to the block's
   RAM from outside the mixer discards it. */
typedef struct {
    uint8_t valid;
    int16_t prev[2];
    int16_t pcm[28];
} Spu2Block;

typedef struct Spu2 {
    uint16_t ram[SPU2_RAM_WORDS];
    Spu2Block blocks[SPU2_RAM_WORDS / 8];
    Spu2Core core[2];
    uint32_t cycles;          /* samples mixed, counting the one being mixed: the reverb runs on half of it */
    uint32_t out_pos;         /* where the mixer writes its outputs into sound RAM (0x000-0x1FF) */
    uint16_t spdif[8];        /* 0x7C0-0x7CF */
    int play_mode;            /* from SPDIF_OUT and SPDIF_MODE: bypasses that skip parts of the mix */
    /* for analysis tools: voices to silence, a bit per voice for each core; everything else runs as before */
    uint32_t mute[2];
    /* told of every register write and RAM transfer (the end-to-end comparison) */
    void (*on_write)(void *ctx, const struct Spu2 *s, uint32_t addr, uint16_t value);
    void (*on_ram)(void *ctx, const struct Spu2 *s, uint32_t addr, const uint16_t *words, uint32_t n);
    /* and of each voice's sample (after the envelope; then left and right) and each core's mixes (dry, wet,
       reverb out and the core's out, left and right), for attributing a difference (ggx-spu2-replay --stages) */
    void (*on_voice)(void *ctx, const struct Spu2 *s, int core, int voice, int32_t value, const int32_t out[2]);
    void (*on_core)(void *ctx, const struct Spu2 *s, int core, const int32_t mix[8]);
    /* and of each sample mixed */
    void (*on_sample)(void *ctx, const struct Spu2 *s, const int16_t out[2]);
    void *on_ctx;
} Spu2;

/* The reverb registers' indices in Spu2Core.rev. */
enum {
    REV_APF1_SIZE, REV_APF2_SIZE, REV_IIR_VOL, REV_COMB1_VOL, REV_COMB2_VOL, REV_COMB3_VOL, REV_COMB4_VOL,
    REV_WALL_VOL, REV_APF1_VOL, REV_APF2_VOL, REV_SAME_L_DST, REV_SAME_R_DST, REV_COMB1_L, REV_COMB1_R,
    REV_COMB2_L, REV_COMB2_R, REV_SAME_L_SRC, REV_SAME_R_SRC, REV_DIFF_L_DST, REV_DIFF_R_DST, REV_COMB3_L,
    REV_COMB3_R, REV_COMB4_L, REV_COMB4_R, REV_DIFF_L_SRC, REV_DIFF_R_SRC, REV_APF1_L_DST, REV_APF1_R_DST,
    REV_APF2_L_DST, REV_APF2_R_DST, REV_IN_L, REV_IN_R
};

/* Power-on, as PCSX2 starts its SPU2: RAM zero but the two loops its BIOS leaves, voices stopped. */
void spu2_reset(Spu2 *s);
/* A write to an SPU2 register, at its offset from 0x1F900000 (only the low 11 bits count), as the IOP
   makes it; and a read (spu2_regs.c). Key on and off take effect at the next sample. */
void spu2_write(Spu2 *s, uint32_t addr, uint16_t value);
uint16_t spu2_read(Spu2 *s, uint32_t addr);
/* Writes words into sound RAM at a word address, as a transfer does. */
void spu2_write_words(Spu2 *s, uint32_t addr, const uint16_t *words, uint32_t n);
/* The same from bytes at a byte address. */
void spu2_write_ram(Spu2 *s, uint32_t addr, const uint8_t *data, uint32_t size);
/* One 48 kHz sample: what core 1 outputs, left then right. */
void spu2_tick(Spu2 *s, int16_t out[2]);

/* for spu2_regs.c */
void spu2_key_on(Spu2 *s, int core, uint32_t voices);
void spu2_key_off(Spu2 *s, int core, uint32_t voices);
void spu2_vol_set(Spu2Vol *v, uint16_t reg);
void spu2_adsr_set(Spu2Voice *v);

#endif
