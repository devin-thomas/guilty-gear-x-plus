/* The SPU2's registers (PORT-011): what an IOP write to 0x1F900000 + addr
 * does to the model, and what a read gives. Each core's 0x400 bytes hold
 * its 24 voices' parameters (16 bytes each), its switches, the voices'
 * addresses (12 bytes each) and its reverb's addresses; the cores'
 * volumes and reverb volumes sit at 0x760 and 0x788, SPDIF at 0x7C0.
 *
 * Writes behave as on PCSX2 2.8.2: an address register is two words, the
 * high one first, stored as written (a voice's SSA, LSA and NAX keep 20
 * bits, 8-aligned, NAX then pointing past the block header); a 24-bit
 * switch is two words, each replacing its half; KON and KOFF hold what was
 * written until the next sample applies them; writing ENDX clears its
 * half; MMIX gates the mixes; registers with no effect read back as
 * written. */
#include "spu2.h"

static uint32_t set_hi(uint32_t old, uint16_t v)
{
    return (old & 0xffff) | (uint32_t)v << 16;
}

static uint32_t set_lo(uint32_t old, uint16_t v)
{
    return (old & 0xffff0000u) | v;
}

/* the reverb's address registers from 0x2E4, in order, as Spu2Core.rev indices */
static const uint8_t REV_ADDR[22] = {
    REV_APF1_SIZE, REV_APF2_SIZE, REV_SAME_L_DST, REV_SAME_R_DST, REV_COMB1_L, REV_COMB1_R, REV_COMB2_L,
    REV_COMB2_R, REV_SAME_L_SRC, REV_SAME_R_SRC, REV_DIFF_L_DST, REV_DIFF_R_DST, REV_COMB3_L, REV_COMB3_R,
    REV_COMB4_L, REV_COMB4_R, REV_DIFF_L_SRC, REV_DIFF_R_SRC, REV_APF1_L_DST, REV_APF1_R_DST, REV_APF2_L_DST,
    REV_APF2_R_DST,
};

/* Which reverb address a core's register at r sets. On core 1, PCSX2 2.8.2 has DIFF_L_SRC (0x724) and
   DIFF_R_SRC (0x728) the other way round, core 0 and the hardware's register map not: the model does as PCSX2
   (AUDIO.md, SPU2 lockstep; SNDRV.IRX writes zero to both). */
static int rev_addr_reg(int core, uint32_t r)
{
    int i = REV_ADDR[(r - 0x2e4) / 4];

    if (core == 1 && (i == REV_DIFF_L_SRC || i == REV_DIFF_R_SRC)) {
        return i == REV_DIFF_L_SRC ? REV_DIFF_R_SRC : REV_DIFF_L_SRC;
    }
    return i;
}

/* and its volumes from 0x774 (core 0) / 0x79C (core 1) */
static const uint8_t REV_VOL[10] = {
    REV_IIR_VOL, REV_COMB1_VOL, REV_COMB2_VOL, REV_COMB3_VOL, REV_COMB4_VOL, REV_WALL_VOL, REV_APF1_VOL,
    REV_APF2_VOL, REV_IN_L, REV_IN_R,
};

static void spdif_mode(Spu2 *s)
{
    uint16_t out = s->spdif[0], mode = s->spdif[3];

    if (out & 4) {
        s->play_mode = 8;      /* PCM direct */
    } else if (out & 0x100) {
        s->play_mode = (mode & 2) ? 2 : 4;   /* bypass: core 0's input isn't mixed; bitstream: nor its output */
    } else {
        s->play_mode = (out & 0x20) ? 1 : 0;
    }
}

static void core_ext_write(Spu2 *s, int core, uint32_t r, uint16_t v)
{
    Spu2Core *c = &s->core[core];

    switch (r / 2) {
    case 0: case 1: spu2_vol_set(&c->mvol[r / 2], v); break;
    case 2: case 3: c->evol[r / 2 - 2] = (int16_t)v; break;
    case 4: case 5: c->avol[r / 2 - 4] = (int16_t)v; break;
    case 6: case 7: c->bvol[r / 2 - 6] = (int16_t)v; break;
    case 8: case 9: break;   /* MVOLX: not writable */
    default: c->rev[REV_VOL[r / 2 - 10]] = v; break;
    }
}

static uint16_t core_ext_read(const Spu2 *s, int core, uint32_t r)
{
    const Spu2Core *c = &s->core[core];

    switch (r / 2) {
    case 0: case 1: return c->mvol[r / 2].reg;
    case 2: case 3: return (uint16_t)c->evol[r / 2 - 2];
    case 4: case 5: return (uint16_t)c->avol[r / 2 - 4];
    case 6: case 7: return (uint16_t)c->bvol[r / 2 - 6];
    case 8: case 9: return (uint16_t)c->mvol[r / 2 - 8].value;
    default: return (uint16_t)c->rev[REV_VOL[r / 2 - 10]];
    }
}

static void voice_write(Spu2Voice *v, uint32_t r, uint16_t value)
{
    switch (r / 2) {
    case 0: case 1: spu2_vol_set(&v->vol[r / 2], value); break;
    case 2: v->pitch = value; break;
    case 3: v->adsr1 = value; spu2_adsr_set(v); break;
    case 4: v->adsr2 = value; spu2_adsr_set(v); break;
    case 5: v->env = value; break;
    default: break;   /* VOLX: no effect */
    }
}

static void addr_write(Spu2Voice *v, uint32_t k, uint16_t value)
{
    uint32_t hi = (uint32_t)(value & 0xf) << 16, lo = value & 0xfff8u;

    switch (k) {
    case 0: v->ssa = hi | (v->ssa & 0xfff8); break;
    case 1: v->ssa = (v->ssa & 0xf0000) | lo; break;
    case 2: v->lsa = hi | (v->lsa & 0xfff8); v->loop_mode = 1; break;
    case 3: v->lsa = (v->lsa & 0xf0000) | lo; v->loop_mode = 1; break;
    case 4: v->nax = hi | (v->nax & 0xfff8) | 1; break;
    default: v->nax = (v->nax & 0xf0000) | lo | 1; break;
    }
}

void spu2_write(Spu2 *s, uint32_t addr, uint16_t v)
{
    int core, i;
    uint32_t r, hiw;
    Spu2Core *c;

    addr &= 0x7fe;
    if (s->on_write) {
        s->on_write(s->on_ctx, s, addr, v);
    }
    if (addr >= 0x760 && addr < 0x7b0) {
        core = addr >= 0x788;
        core_ext_write(s, core, addr - (core ? 0x788u : 0x760u), v);
        return;
    }
    if (addr >= 0x7b0) {
        if (addr >= 0x7c0 && addr < 0x7ce) {
            s->spdif[(addr - 0x7c0) / 2] = v;
            spdif_mode(s);
        }
        return;
    }
    core = addr >= 0x400;
    r = addr & 0x3ff;
    c = &s->core[core];
    hiw = r & 2;
    if (r < 0x180) {
        voice_write(&c->v[r / 0x10], r & 0xf, v);
        return;
    }
    if (r >= 0x1c0 && r < 0x1c0 + 24 * 12) {
        addr_write(&c->v[(r - 0x1c0) / 12], (r - 0x1c0) % 12 / 2, v);
        return;
    }
    if (r >= 0x2e4 && r < 0x2e4 + 22 * 4) {
        uint32_t *g = &c->rev[rev_addr_reg(core, r)];
        *g = hiw ? set_lo(*g, v) : set_hi(*g, v);
        return;
    }
    switch (r) {
    case 0x180: case 0x182:
        c->pmon = hiw ? set_hi(c->pmon, v) : set_lo(c->pmon, v);
        for (i = hiw ? 16 : 1; i < (hiw ? 24 : 16); i++) {
            c->v[i].modulated = v >> (i & 15) & 1;
        }
        break;
    case 0x184: case 0x186:
        c->non = hiw ? set_hi(c->non, v) : set_lo(c->non, v);
        for (i = hiw ? 16 : 0; i < (hiw ? 24 : 16); i++) {
            c->v[i].noise = v >> (i & 15) & 1;
        }
        break;
    case 0x188: case 0x18a: c->vmixl = hiw ? set_hi(c->vmixl, v) : set_lo(c->vmixl, v); break;
    case 0x18c: case 0x18e: c->vmixel = hiw ? set_hi(c->vmixel, v) : set_lo(c->vmixel, v); break;
    case 0x190: case 0x192: c->vmixr = hiw ? set_hi(c->vmixr, v) : set_lo(c->vmixr, v); break;
    case 0x194: case 0x196: c->vmixer = hiw ? set_hi(c->vmixer, v) : set_lo(c->vmixer, v); break;
    case 0x198:
        c->mmix = v;
        c->gates = v & (core ? 0xfff : 0xff0);   /* core 0 has no external input */
        break;
    case 0x19a: c->attr = v; break;
    case 0x19c: c->irqa = set_hi(c->irqa, v); break;
    case 0x19e: c->irqa = set_lo(c->irqa, v); break;
    case 0x1a0: case 0x1a2: c->kon = hiw ? set_hi(c->kon, v) : set_lo(c->kon, v); break;
    case 0x1a4: case 0x1a6: c->koff = hiw ? set_hi(c->koff, v) : set_lo(c->koff, v); break;
    case 0x1a8: c->tsa = set_hi(c->tsa, v); break;
    case 0x1aa: c->tsa = set_lo(c->tsa, v); break;
    case 0x1ac: c->tsa = (c->tsa + 1) & 0xfffff; break;   /* the data port: the caller writes the word into RAM */
    case 0x1b0: c->admas = v; break;
    case 0x2e0: c->esa = set_hi(c->esa, v); break;
    case 0x2e2: c->esa = set_lo(c->esa, v); break;
    case 0x33c: c->eea = set_hi(c->eea, v); break;
    case 0x33e: break;
    case 0x340: c->endx &= 0xff0000; break;
    case 0x342: c->endx &= 0xffff; break;
    case 0x344: break;   /* STATX */
    default: c->raw[r / 2] = v; break;
    }
}

uint16_t spu2_read(Spu2 *s, uint32_t addr)
{
    int core;
    uint32_t r, hiw;
    const Spu2Core *c;

    addr &= 0x7fe;
    if (addr >= 0x760 && addr < 0x7b0) {
        core = addr >= 0x788;
        return core_ext_read(s, core, addr - (core ? 0x788u : 0x760u));
    }
    if (addr >= 0x7c0 && addr < 0x7ce) {
        return s->spdif[(addr - 0x7c0) / 2];
    }
    if (addr >= 0x7b0) {
        return 0;
    }
    core = addr >= 0x400;
    r = addr & 0x3ff;
    c = &s->core[core];
    hiw = r & 2;
    if (r < 0x180) {
        const Spu2Voice *v = &c->v[r / 0x10];
        switch ((r & 0xf) / 2) {
        case 0: case 1: return v->vol[(r & 0xf) / 2].reg;
        case 2: return v->pitch;
        case 3: return v->adsr1;
        case 4: return v->adsr2;
        case 5: return (uint16_t)v->env;
        default: return (uint16_t)v->vol[(r & 0xf) / 2 - 6].value;
        }
    }
    if (r >= 0x1c0 && r < 0x1c0 + 24 * 12) {
        const Spu2Voice *v = &c->v[(r - 0x1c0) / 12];
        uint32_t k = (r - 0x1c0) % 12 / 2, a = k < 2 ? v->ssa : k < 4 ? v->lsa : v->nax;
        return (uint16_t)((k & 1) ? a : a >> 16);
    }
    if (r >= 0x2e4 && r < 0x2e4 + 22 * 4) {
        uint32_t g = c->rev[rev_addr_reg(core, r)];
        return (uint16_t)(hiw ? g : g >> 16);
    }
#define HALF(x) ((uint16_t)(hiw ? (x) >> 16 : (x)))
    switch (r) {
    case 0x180: case 0x182: return HALF(c->pmon);
    case 0x184: case 0x186: return HALF(c->non);
    case 0x188: case 0x18a: return HALF(c->vmixl);
    case 0x18c: case 0x18e: return HALF(c->vmixel);
    case 0x190: case 0x192: return HALF(c->vmixr);
    case 0x194: case 0x196: return HALF(c->vmixer);
    case 0x198: return c->mmix;
    case 0x19a: return c->attr;
    case 0x19c: return (uint16_t)(c->irqa >> 16);
    case 0x19e: return (uint16_t)c->irqa;
    case 0x1a0: case 0x1a2: return HALF(c->kon);
    case 0x1a4: case 0x1a6: return HALF(c->koff);
    case 0x1a8: return (uint16_t)(c->tsa >> 16);
    case 0x1aa: return (uint16_t)c->tsa;
    case 0x1b0: return c->admas;
    case 0x2e0: return (uint16_t)(c->esa >> 16);
    case 0x2e2: return (uint16_t)c->esa;
    case 0x33c: return (uint16_t)(c->eea >> 16);
    case 0x33e: return (uint16_t)c->eea;
    case 0x340: case 0x342: return HALF(c->endx);
    case 0x344: return c->statx;
    default: return c->raw[r / 2];
    }
#undef HALF
}
