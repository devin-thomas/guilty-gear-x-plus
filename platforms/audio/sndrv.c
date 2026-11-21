#include "sndrv.h"

#include <stdlib.h>
#include <string.h>

#include "snd_stream.h"

/* SNDRV.IRX's buffers: the RPC server's argument buffer (gRpcArg), the IOP
   memory it allocates for the voice banks or a kept BGM (bgm_file_buf), and
   the BGM's 16-sector read buffer (BgmWaveBuf), left channel then right. */
#define RPC_ARG_SIZE 0x40000u
#define IOP_BUF_SIZE 0x80000u
#define WAVE_SIZE 0x8000u

/* the effect thread's queue (se_thd_buf): 32 entries of a command, a size and 64 bytes */
typedef struct {
    uint32_t cmd, size;
    uint8_t data[0x40];
} SeEntry;

/* a call waiting for the RPC thread */
typedef struct Rpc {
    struct Rpc *next;
    uint32_t cmd, size;
    uint8_t data[];
} Rpc;

/* where a blocked BGM thread goes on when it wakes */
enum { RESUME_NONE, RESUME_MODE2_READ, RESUME_MODE4_READ, RESUME_STREAM_READ, RESUME_WAIT_INTSW };

struct Sndrv {
    Spu2 spu;
    SndDiscRead disc;
    void *disc_ctx;
    uint8_t rpc_arg[RPC_ARG_SIZE];
    uint8_t iop_buf[IOP_BUF_SIZE];
    uint8_t wave[WAVE_SIZE];
    uint64_t now;          /* samples since the driver started */

    /* the IOP's threads: the RPC thread is waiting for its DMA transfer, and the effect thread for its
       disc read, until these times */
    Rpc *rpc_head, **rpc_tail;
    uint64_t rpc_free, se_free;
    uint32_t dma0_samples; /* the transfer the call running made */
    /* the drive: the sector after its last read, whether it spins at the maximum speed, when it is free */
    uint32_t cd_next;
    int cd_fast;
    uint64_t cd_free;

    /* sndrv.c */
    uint32_t ret;
    /* se.c */
    SeEntry thd[32];
    int thd_read, thd_write;
    uint32_t spu_address, last_spu_address, kon_sw;
    uint32_t se_vol[2];
    int32_t se_loading;
    /* bgm.c */
    int32_t bgm_mode, req_bgm_mode, bgm_dev, req_bgm_dev;
    int32_t bgm_fptop, req_bgm_fptop, bgm_size, req_bgm_size, bgm_fp;
    int32_t bgm_loop, bgm_kon, bgm_switch, bgm_bank, bgm_reserve, bgm_reserveR;
    int32_t bgm_channel, bgm_vol, intsw;
    /* the BGM thread: asleep until wake, then resume */
    uint64_t wake;
    int resume;
    int read_ok;
    /* libsd: a channel-1 transfer whose completion interrupt (bgm_irq) is due at dma1_done */
    int dma1_pending;
    uint64_t dma1_done;
    /* when each channel's transfer ends: LIBSD then sets ATTR's transfer mode back */
    uint64_t dma_attr[2];
};

/* ------------------------------------------------------------------------
 * libsd: the calls SNDRV.IRX makes, as LIBSD.IRX makes them, as writes to
 * the SPU2's registers. An entry is (register << 8) | (voice << 1) | core,
 * with 0x80 marking a core volume and 0x40 a voice address; LIBSD's table
 * gives each register's address on core 0 for voice 0, a voice's
 * parameters are 16 bytes apart and its addresses 12, core 1's registers
 * 0x400 on (its volumes 0x28).
 * ------------------------------------------------------------------------ */

static const uint16_t SD_REG[0x24] = {
    0x000, 0x002, 0x004, 0x006, 0x008, 0x00a, 0x00c, 0x00e,   /* VOLL VOLR PITCH ADSR1 ADSR2 ENVX VOLXL VOLXR */
    0x198,                                                    /* MMIX */
    0x760, 0x762, 0x764, 0x766, 0x768, 0x76a, 0x76c, 0x76e,   /* MVOL EVOL AVOL BVOL, left and right */
    0x770, 0x772,                                             /* MVOLX */
    0x180, 0x184, 0x1a0, 0x1a4, 0x340,                        /* PMON NON KON KOFF ENDX */
    0x188, 0x18c, 0x190, 0x194,                               /* VMIXL VMIXEL VMIXR VMIXER */
    0x2e0, 0x33c, 0x1a8, 0x19c,                               /* ESA EEA TSA IRQA */
    0x1c0, 0x1c4, 0x1c8,                                      /* SSA LSA NAX */
    0x19a,                                                    /* ATTR */
};

static uint32_t sd_reg(uint32_t entry)
{
    uint32_t id = (entry >> 8) & 0xff;

    return id < 0x24 ? SD_REG[id] : 0x7fe;
}

static void sd_set_param(Sndrv *d, uint32_t entry, uint16_t v)
{
    spu2_write(&d->spu, sd_reg(entry) + (entry & 0x3e) * 8 + (entry & 1) * ((entry & 0x80) ? 0x28 : 0x400), v);
}

/* SetSwitch writes a register pair, the low 16 bits and then bits 16-23 into the next register. On a switch
   that is one 24-bit value; SNDRV.IRX also calls it on two volumes and on ESA, where it writes the
   neighbour too. */
static void sd_set_switch(Sndrv *d, uint32_t entry, uint32_t v)
{
    uint32_t a = sd_reg(entry) + (entry & 1) * 0x400;

    spu2_write(&d->spu, a, (uint16_t)v);
    spu2_write(&d->spu, a + 2, (uint16_t)((v >> 16) & 0xff));
}

/* Addresses are bytes; the registers hold words, the high word first, the low one 8-aligned. EEA has only
   its high word. */
static void sd_set_addr(Sndrv *d, uint32_t entry, uint32_t addr)
{
    uint32_t a = sd_reg(entry) + (entry & 1) * 0x400 + (entry & 0x3e) * 6;

    spu2_write(&d->spu, a, (uint16_t)(addr >> 17));
    if ((entry & 0xff00) != 0x1d00) {
        spu2_write(&d->spu, a + 2, (uint16_t)((addr >> 1) & 0xfff8));
    }
}

static uint32_t sd_get_addr(Sndrv *d, uint32_t entry)
{
    uint32_t a = sd_reg(entry) + (entry & 1) * 0x400 + (entry & 0x3e) * 6;

    if ((entry & 0xff00) == 0x1d00) {
        return (uint32_t)spu2_read(&d->spu, a) << 17 | 0x1ffff;
    }
    return (uint32_t)spu2_read(&d->spu, a) << 17 | (uint32_t)spu2_read(&d->spu, a + 2) << 1;
}

/* sceSdSetCoreAttr: a bit of ATTR (reverb writes, IRQ, mute), or the noise clock */
static void sd_set_core_attr(Sndrv *d, uint32_t entry, uint32_t v)
{
    static const uint8_t BIT[5] = { 0, 7, 6, 14, 8 };
    uint32_t a = 0x19a + (entry & 1) * 0x400;
    uint16_t attr = spu2_read(&d->spu, a);

    switch (entry & 0xe) {
    case 8: spu2_write(&d->spu, a, (uint16_t)((attr & 0xc0ff) | (v & 0x3f) << 8)); break;
    case 10: break;   /* SPDIF: SNDRV.IRX doesn't set it */
    default:
        spu2_write(&d->spu, a, (uint16_t)((attr & ~(1u << BIT[(entry & 0xe) / 2])) | (v & 1) << BIT[(entry & 0xe) / 2]));
        break;
    }
}

/* sceSdSetEffectAttr with mode 0, the only one SNDRV.IRX sets, as its writes come on PCSX2: the depths become
   the reverb return volume, then the preset (every reverb register zero but the four destinations, 4), then
   ESA, the work area at the end of the core's reverb area (8 words). */
static void sd_set_effect_attr_off(Sndrv *d, int core, int16_t depth_l, int16_t depth_r)
{
    uint32_t a, c = (uint32_t)core * 0x400, cv = (uint32_t)core * 0x28;

    spu2_write(&d->spu, 0x764 + cv, (uint16_t)depth_l);
    spu2_write(&d->spu, 0x766 + cv, (uint16_t)depth_r);
    for (a = 0x2e4; a < 0x2ec; a += 2) {
        spu2_write(&d->spu, a + c, 0);
    }
    for (a = 0x774; a < 0x784; a += 2) {
        spu2_write(&d->spu, a + cv, 0);
    }
    for (a = 0x2ec; a < 0x33c; a += 2) {
        spu2_write(&d->spu, a + c, (a == 0x2ee || a == 0x2f2 || a == 0x30e || a == 0x312) ? 4 : 0);
    }
    spu2_write(&d->spu, 0x784 + cv, 0);
    spu2_write(&d->spu, 0x786 + cv, 0);
    spu2_write(&d->spu, 0x2e0 + c, spu2_read(&d->spu, 0x33c + c));
    spu2_write(&d->spu, 0x2e2 + c, 0xfff8);
}

/* A DMA transfer from IOP memory into sound RAM, at TSA. The data is there at once; the transfer takes
   SNDRV_DMA_BYTES bytes a sample. Channel 0's caller waits for it (sceSdVoiceTransStatus); channel 1's
   completion interrupt (bgm_irq) runs at the first poll after it. */
static void sd_voice_trans(Sndrv *d, int channel, const uint8_t *src, uint32_t spu_addr, uint32_t size)
{
    uint32_t t = (size + SNDRV_DMA_BYTES - 1) / SNDRV_DMA_BYTES, c = (uint32_t)(channel & 1) * 0x400;
    uint16_t hi = (uint16_t)(spu_addr >> 17), lo = (uint16_t)((spu_addr >> 1) & 0xfff8);

    /* TSA (LIBSD writes it high, low, low, high), then ATTR's transfer mode to DMA write until the transfer
       is done (iop_tick) */
    spu2_write(&d->spu, 0x1a8 + c, hi);
    spu2_write(&d->spu, 0x1aa + c, lo);
    spu2_write(&d->spu, 0x1aa + c, lo);
    spu2_write(&d->spu, 0x1a8 + c, hi);
    spu2_write(&d->spu, 0x19a + c, (uint16_t)((spu2_read(&d->spu, 0x19a + c) & ~0x30u) | 0x20));
    spu2_write_ram(&d->spu, spu_addr, src, size);
    d->dma_attr[channel & 1] = d->now + t;
    if (channel == 1) {
        d->dma1_pending = 1;
        d->dma1_done = d->now + t;
    } else {
        d->dma0_samples += t;
    }
}

/* The drive takes a read: after the one it is busy with, a seek as far as
   the head moves, a change of speed if the reader wants the other one
   (the driver's reads are at the maximum, the game's at the nominal),
   then the sectors. When it will be done, from now. */
static uint64_t drive(Sndrv *d, uint32_t lsn, uint32_t sectors, int fast)
{
    uint64_t start = d->cd_free > d->now ? d->cd_free : d->now;
    uint32_t dist = lsn > d->cd_next ? lsn - d->cd_next : d->cd_next - lsn;

    start += dist < SNDRV_CD_NEAR ? 0 : dist < SNDRV_CD_FAR ? SNDRV_CD_SHORT_SEEK : SNDRV_CD_SEEK;
    if (fast && !d->cd_fast) {
        start += SNDRV_CD_SPEED + (uint64_t)lsn * SNDRV_CD_SPEED_LSN / 100000;
    }
    d->cd_fast = fast;
    d->cd_free = start + SNDRV_CD_START + (uint64_t)sectors * SNDRV_CD_SECTOR;
    d->cd_next = lsn + sectors;
    return d->cd_free - d->now;
}

/* SNDRV.IRX's own reads from the disc (sceCdRead): the data at once, and
   how long until the drive has it (*samples). */
static int cd_read(Sndrv *d, int32_t lsn, int32_t sectors, uint8_t *dst, uint64_t *samples)
{
    if (lsn < 0 || sectors <= 0 || d->disc(d->disc_ctx, (uint32_t)lsn, (uint32_t)sectors, dst) != 0) {
        return 0;
    }
    *samples = drive(d, (uint32_t)lsn, (uint32_t)sectors, 1);
    return 1;
}

/* ------------------------------------------------------------------------
 * se.c: effects and voices on core 1's voices 1-23
 * ------------------------------------------------------------------------ */

static void se_volume(Sndrv *d, uint32_t l, uint32_t r)
{
    int i;

    d->se_vol[0] = l;
    d->se_vol[1] = r;
    for (i = 1; i < 24; i++) {
        sd_set_param(d, (uint32_t)i << 1 | 1, (uint16_t)l);
        sd_set_param(d, (uint32_t)i << 1 | 0x101, (uint16_t)r);
    }
}

static void se_init(Sndrv *d)
{
    se_volume(d, 0x1fff, 0x1fff);
    d->spu_address = 0xd000;
    sd_set_core_attr(d, 3, 0);
    sd_set_param(d, 0xb81, 0);
    sd_set_param(d, 0xc81, 0);
    sd_set_param(d, 0xd81, 0x7fff);
    sd_set_param(d, 0xe81, 0x7fff);
    sd_set_switch(d, 0x1901, 0);
    sd_set_switch(d, 0x1b01, 0);
    sd_set_switch(d, 0x1801, 0xffffff);
    sd_set_switch(d, 0x1a01, 0xffffff);
    sd_set_param(d, 0x801, 0xc0c);
    sd_set_switch(d, 0x1c01, 0x3f0000);
}

static void se_stop(Sndrv *d, uint32_t ch)
{
    sd_set_param(d, (ch & 0x7fff) << 1 | 1, 0);
    sd_set_param(d, (ch & 0x7fff) << 1 | 0x101, 0);
    sd_set_switch(d, 0x1601, 1u << (ch & 0x1f));
}

static void se_trans(Sndrv *d, uint32_t ch, uint32_t spu_addr)
{
    sd_set_addr(d, (ch & 0x7fff) << 1 | 0x2041, spu_addr);
    d->kon_sw |= 1u << (ch & 0x1f);
    sd_set_addr(d, (ch & 0x7fff) << 1 | 0x2041, spu_addr);
    sd_set_param(d, (ch & 0x7fff) << 1 | 1, (uint16_t)d->se_vol[0]);
    sd_set_param(d, (ch & 0x7fff) << 1 | 0x101, (uint16_t)d->se_vol[1]);
}

static void se_load_cd(Sndrv *d, uint32_t bank, int32_t lsn, int32_t size)
{
    uint8_t *dst = d->iop_buf + (bank ? 0x40000 : 0);
    int32_t sectors = (size < 0 ? size + 0x7ff : size) >> 11;
    uint64_t t = 0;

    d->se_loading = 3;
    if (sectors > (int32_t)((IOP_BUF_SIZE - (uint32_t)(dst - d->iop_buf)) >> 11)) {
        /* the PS2 would read on past its allocation */
        sectors = (int32_t)((IOP_BUF_SIZE - (uint32_t)(dst - d->iop_buf)) >> 11);
    }
    d->se_loading = cd_read(d, lsn, sectors, dst, &t) ? 0 : -1;
    /* the effect thread waits for the read (sceCdSync) */
    d->se_free = d->now + t;
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static void wr32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

/* The spot in the effects' ring of sound RAM (0xD000-0x1F0000) for size bytes. */
static uint32_t se_place(Sndrv *d, int32_t size)
{
    if ((int32_t)d->spu_address < 0xd000 || (int32_t)d->spu_address + size > 0x1f0000) {
        d->spu_address = 0xd000;
    }
    d->last_spu_address = d->spu_address;
    return d->spu_address;
}

static void se_push(Sndrv *d)
{
    d->thd_write = (d->thd_write + 1) % 32;
}

/* An RPC command for the effect thread: queues it, and for 0x0D and 0x12
   copies the effect into sound RAM first, in the RPC thread. arg is the
   RPC's argument buffer, size the bytes the call sent. */
static void se_command(Sndrv *d, uint32_t cmd, uint8_t *arg, int32_t size)
{
    SeEntry *e = &d->thd[d->thd_write];

    if ((d->thd_write + 1) % 32 == d->thd_read) {
        return;   /* "PATCH:THD Overflow" */
    }
    if (cmd == SND_SE_TRANS) {
        /* voice, size, EE address; the effect follows at 0x40. The transfer
           is the call's size less the header, rounded down to 64 bytes, and
           its last block is made an end: a one-shot sample. */
        int32_t n = ((size - 1) >> 6) << 6;
        uint8_t *wave = arg + 0x40;

        e->cmd = SND_SE_TRANS;
        memcpy(e->data, arg, 8);
        if (n < 0 || n > 0x12000) {
            return;   /* "PATCH:Illegal Size 1" */
        }
        wr32(e->data + 8, se_place(d, n));
        wave[n - 0xf] = 1;
        wave[n - 0x10] = 0;
        if ((int8_t)wave[0] < 0) {
            return;   /* "Voice CutOff": the transfer is skipped and the entry never queued */
        }
        sd_voice_trans(d, 0, wave, d->spu_address, (uint32_t)n);
        d->spu_address += (uint32_t)n;
        e->size = 0xc;
        se_push(d);
        return;
    }
    if (cmd == SND_SE_BANK) {
        /* voice, bank, effect: a bank is a table of offsets, then the effects */
        uint32_t bank = rd32(arg + 4), num = rd32(arg + 8), top;
        const uint8_t *base = d->iop_buf + (bank ? 0x40000 : 0);
        int32_t n;

        e->cmd = SND_SE_TRANS;
        memcpy(e->data, arg, 4);
        if (num > (IOP_BUF_SIZE - 8) / 4) {
            return;
        }
        n = (int32_t)(rd32(base + num * 4 + 4) - rd32(base + num * 4));
        n = ((n + 0x3f) >> 6) << 6;
        if (n < 0 || n > 0x12000) {
            return;   /* "PATCH:Illegal Size 2" */
        }
        top = rd32(base + num * 4);
        if ((top & 3) || (int32_t)top < 0 || (int32_t)top > 0x80000 || (base - d->iop_buf) + top + (uint32_t)n > IOP_BUF_SIZE) {
            return;   /* "PATCH:Illegal TopAddr" (and, here, past the end of the buffer) */
        }
        wr32(e->data + 8, se_place(d, n));
        sd_voice_trans(d, 0, base + top, d->spu_address, (uint32_t)n);
        d->spu_address += (uint32_t)n;
        e->size = 0xc;
        se_push(d);
        return;
    }
    e->cmd = cmd;
    memset(e->data, 0, sizeof(e->data));
    memcpy(e->data, arg, size > 0x40 ? 0x40 : (size_t)(size < 0 ? 0 : size));
    if (cmd == SND_SE_AGAIN) {
        wr32(e->data + 4, d->last_spu_address);
    }
    e->size = (uint32_t)size;
    se_push(d);
}

/* The effect thread (se_main): runs the queue, unless it is waiting for a
   disc read. */
static void se_main(Sndrv *d)
{
    while (d->thd_read != d->thd_write && d->now >= d->se_free) {
        SeEntry *e = &d->thd[d->thd_read];

        switch (e->cmd) {
        case SND_SE_STOP: se_stop(d, rd32(e->data)); break;
        case SND_SE_ALLSTOP: sd_set_switch(d, 0x1601, 0xffffff); break;
        case SND_SE_VOLUME: se_volume(d, rd32(e->data), rd32(e->data + 4)); break;
        case SND_SE_TRANS: se_trans(d, rd32(e->data), rd32(e->data + 8)); break;
        case SND_SE_AGAIN: se_trans(d, rd32(e->data), rd32(e->data + 4)); break;
        case SND_SE_LOAD_PC: d->se_loading = 0; break;   /* the host's files: not on a player's disc */
        case SND_SE_LOAD_CD: se_load_cd(d, rd32(e->data), (int32_t)rd32(e->data + 4), (int32_t)rd32(e->data + 8)); break;
        case SND_KEY_ON:
            if (d->kon_sw) {
                sd_set_switch(d, 0x1501, d->kon_sw);
                d->kon_sw = 0;
                /* The SPU2 takes KON at its next sample, a second write before it replacing the first. On the
                   IOP the thread's calls take time, and the BGM thread's loop takes its share: key ons come
                   samples apart (AUDIO.md, SPU2 lockstep). Here the thread waits for the next sample. */
                d->se_free = d->now + 1;
                d->thd_read = (d->thd_read + 1) % 32;
                return;
            }
            break;
        default: break;
        }
        d->thd_read = (d->thd_read + 1) % 32;
    }
}

/* ------------------------------------------------------------------------
 * bgm.c: music on core 0's voices 18-21, streamed through two 16 KB rings
 * of sound RAM (left 0x5000-0x9000, right 0x9000-0xD000), each refilled a
 * half at a time from 32 KB reads (16 KB left, then 16 KB right)
 * ------------------------------------------------------------------------ */

static void bgm_vol_clr(Sndrv *d)
{
    static const uint16_t v[] = { 0x24, 0x28, 0x26, 0x2a, 0x124, 0x128, 0x126, 0x12a };
    size_t i;

    for (i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
        sd_set_param(d, v[i], 0);
    }
}

static void bgm_vol_out(Sndrv *d)
{
    uint16_t v = (uint16_t)d->bgm_vol, h = (uint16_t)(d->bgm_vol / 2);

    if (d->bgm_channel == 0) {
        /* stereo: 18 and 20 left, 19 and 21 right; each side is played twice */
        sd_set_param(d, 0x24, v);
        sd_set_param(d, 0x124, 0);
        sd_set_param(d, 0x26, 0);
        sd_set_param(d, 0x126, v);
        sd_set_param(d, 0x28, v);
        sd_set_param(d, 0x128, 0);
        sd_set_param(d, 0x2a, 0);
        sd_set_param(d, 0x12a, v);
    } else {
        /* mono: every voice at half volume on both sides */
        sd_set_param(d, 0x24, h);
        sd_set_param(d, 0x124, h);
        sd_set_param(d, 0x26, h);
        sd_set_param(d, 0x126, h);
        sd_set_param(d, 0x28, h);
        sd_set_param(d, 0x128, h);
        sd_set_param(d, 0x2a, h);
        sd_set_param(d, 0x12a, h);
    }
}

static void bgm_set_ssa(Sndrv *d)
{
    sd_set_addr(d, 0x2064, 0x5000);
    sd_set_addr(d, 0x2066, 0x9000);
    sd_set_addr(d, 0x2068, 0x5000);
    sd_set_addr(d, 0x206a, 0x9000);
}

static void bgm_set_lsa(Sndrv *d)
{
    sd_set_addr(d, 0x2164, 0x5000);
    sd_set_addr(d, 0x2166, 0x9000);
    sd_set_addr(d, 0x2168, 0x5000);
    sd_set_addr(d, 0x216a, 0x9000);
}

/* "BGM Miss Found": the voices wrapped round the ring before their next half
   was filled. Start the stream again from what is in the read buffer. */
static void bgm_miss(Sndrv *d)
{
    d->bgm_dev = d->req_bgm_dev;
    d->bgm_size = d->req_bgm_size;
    bgm_set_lsa(d);
    sd_set_switch(d, 0x1600, 0x3c0000);
    d->bgm_mode = 3;
    d->bgm_kon = 1;
}

/* the voices' NAX: 18, the left, and 19, the right */
static int32_t nax_l(Sndrv *d) { return (int32_t)sd_get_addr(d, 0x2264); }
static int32_t nax_r(Sndrv *d) { return (int32_t)sd_get_addr(d, 0x2266); }

/* The transfer interrupt handler: chains each half's right-channel transfer
   to its left one, and tells the stream when a transfer is done. */
static void bgm_irq(Sndrv *d)
{
    int32_t r;

    if (d->bgm_bank == 1) {
        r = nax_r(d);
        if (r < 0xb001 || r > 0xbfff) {
            d->bgm_bank = 3;
        } else {
            sd_voice_trans(d, 1, d->wave + 0x4000, 0x9000, 0x2000);
            d->bgm_bank = 2;
        }
    } else if (d->bgm_bank == 2) {
        d->bgm_bank = 3;
    } else if (d->bgm_bank == 4) {
        r = nax_r(d);
        if (r < 0xa000) {
            sd_voice_trans(d, 1, d->wave + 0x6000, 0xb000, 0x2000);
            d->bgm_bank = 5;
        } else {
            d->bgm_bank = 6;
        }
    } else if (d->bgm_bank == 5) {
        d->bgm_bank = 6;
    }
    d->intsw = 1;
}

/* the BGM thread blocks: it goes on at resume once the time has passed */
static void bgm_block(Sndrv *d, int resume, uint64_t samples)
{
    d->resume = resume;
    d->wake = d->now + samples;
}

/* The flags bgm_main writes into the last byte of each channel's half of
   the read buffer: 3 while the stream goes on, 1 at the end. */
static void bgm_mark(Sndrv *d, uint8_t v)
{
    d->wave[0x3fff] = v;
    d->wave[0x7fff] = v;
}

static void bgm_mode4_rest(Sndrv *d)
{
    d->bgm_kon = 0;
    d->bgm_switch = 1;
    bgm_set_lsa(d);
    d->bgm_bank = 0;
    d->bgm_reserve = 0;
    d->bgm_reserveR = 0;
    d->bgm_mode++;
}

/* The next 16 sectors are in the read buffer (bank 6 onwards): mark them. */
static void bgm_stream_read_done(Sndrv *d)
{
    if (!d->read_ok) {
        d->bgm_switch = 2;
        sd_set_switch(d, 0x1600, 0x3c0000);
        return;
    }
    if (d->bgm_switch == 2) {
        d->bgm_switch = 1;
        sd_set_switch(d, 0x1500, 0x3c0000);
    }
    if (d->bgm_fp < d->bgm_fptop + d->bgm_size) {
        bgm_mark(d, 3);
    } else if (d->bgm_loop == 0) {
        bgm_mark(d, 1);
        d->bgm_switch = 0;
    } else {
        d->bgm_fp = d->bgm_fptop;
    }
    d->bgm_bank = 0;
}

/* Mode 5, the voices playing: one pass of the refill state machine. */
static void bgm_stream(Sndrv *d)
{
    int32_t l, r, keep;

    if (d->bgm_switch == 0) {
        bgm_vol_clr(d);
        sd_set_switch(d, 0x1600, 0x3c0000);
        d->bgm_mode = 0;
        d->bgm_dev = 0;
        return;
    }
    bgm_vol_out(d);
    if (d->bgm_bank == 0) {
        /* waiting for the left voice to pass 0x7000, so its first half can be refilled */
        r = nax_r(d);
        l = nax_l(d);
        if (l < 0x7001) {
            keep = l;
            if (l < d->bgm_reserve) {
                bgm_miss(d);
                keep = d->bgm_reserve;
            }
            d->bgm_reserve = keep;
            if (r < d->bgm_reserveR) {
                bgm_miss(d);
            } else {
                d->bgm_reserveR = r;
            }
        } else if (nax_l(d) > 0x7000) {
            sd_voice_trans(d, 1, d->wave, 0x5000, 0x2000);
            d->bgm_bank = 1;
        }
        return;
    }
    if (d->bgm_bank < 3) {
        /* the first halves' transfers running */
        r = nax_r(d);
        l = nax_l(d);
        if (l < 0x7000) {
            keep = d->bgm_reserve;
            if (nax_l(d) < 0x7000) {
                d->bgm_bank = 3;   /* "BGM Bank Error" */
            }
        } else {
            keep = l;
            if (l < d->bgm_reserve) {
                bgm_miss(d);
                keep = d->bgm_reserve;
            }
        }
        d->bgm_reserve = keep;
        if (r < d->bgm_reserveR) {
            bgm_miss(d);
        } else {
            d->bgm_reserveR = r;
        }
        return;
    }
    if (d->bgm_bank == 3) {
        /* waiting for the left voice to wrap to its first half, so the second can be refilled */
        r = nax_r(d);
        l = nax_l(d);
        if (l < 0x7000) {
            if (nax_l(d) < 0x7000) {
                sd_voice_trans(d, 1, d->wave + 0x2000, 0x7000, 0x2000);
                d->bgm_bank = 4;
                d->bgm_reserve = 0;
                d->bgm_reserveR = 0;
            }
        } else {
            keep = l;
            if (l < d->bgm_reserve) {
                bgm_miss(d);
                keep = d->bgm_reserve;
            }
            d->bgm_reserve = keep;
            if (r < d->bgm_reserveR) {
                bgm_miss(d);
            } else {
                d->bgm_reserveR = r;
            }
        }
        return;
    }
    if (d->bgm_bank > 5) {
        /* both halves sent: read the next 32 KB */
        if (d->bgm_dev == 1) {
            uint64_t t = 0;
            d->read_ok = cd_read(d, d->bgm_fp, 0x10, d->wave, &t);
            if (!d->read_ok) {
                d->bgm_switch = 2;
                sd_set_switch(d, 0x1600, 0x3c0000);
                return;
            }
            d->bgm_fp += 0x10;
            bgm_block(d, RESUME_STREAM_READ, t);
            return;
        }
        if (d->bgm_dev > 1) {
            memcpy(d->wave, d->iop_buf + (d->bgm_fp & (IOP_BUF_SIZE - 1)), WAVE_SIZE);
            d->bgm_fp += (int32_t)WAVE_SIZE;
            bgm_block(d, RESUME_NONE, SNDRV_IOP_COPY);
            if (d->bgm_size <= d->bgm_fp) {
                if (d->bgm_loop == 0) {
                    bgm_mark(d, 1);
                    d->bgm_switch = 0;
                } else {
                    d->bgm_fp = 0;
                }
            }
        }
        d->bgm_bank = 0;
        return;
    }
    /* banks 4 and 5: the second halves' transfers running */
    r = nax_r(d);
    l = nax_l(d);
    if (l < 0x7001) {
        keep = l;
        if (l < d->bgm_reserve) {
            bgm_miss(d);
            keep = d->bgm_reserve;
        }
    } else {
        keep = d->bgm_reserve;
        if (nax_l(d) > 0x7000) {
            d->bgm_bank = 6;   /* "BGM Bank Error" */
        }
    }
    d->bgm_reserve = keep;
    if (r < d->bgm_reserveR) {
        bgm_miss(d);
    } else {
        d->bgm_reserveR = r;
    }
}

/* One pass of bgm_main's loop. */
static void bgm_main(Sndrv *d)
{
    switch (d->bgm_mode) {
    case 0:
        sd_set_switch(d, 0x1600, 0x3c0000);
        bgm_vol_clr(d);
        if (d->req_bgm_mode != 0) {
            d->bgm_dev = d->req_bgm_dev;
            d->bgm_fptop = d->req_bgm_fptop;
            d->bgm_size = d->req_bgm_size;
            d->bgm_mode = 1;
            d->req_bgm_mode = 0;
        }
        break;
    case 1: {
        static const uint16_t voices[] = { 0x24, 0x26, 0x28, 0x2a };
        size_t i;

        bgm_vol_out(d);
        for (i = 0; i < 4; i++) {
            sd_set_param(d, 0x200u | voices[i], 0xeb3);   /* 44.1 kHz */
        }
        sd_set_param(d, 0x324, 0xf);
        sd_set_param(d, 0x326, 0xf);
        sd_set_param(d, 0x424, 0x1fc0);
        sd_set_param(d, 0x426, 0x1fc0);
        sd_set_param(d, 0x328, 0xf);
        sd_set_param(d, 0x32a, 0xf);
        sd_set_param(d, 0x428, 0x1fc0);
        sd_set_param(d, 0x42a, 0x1fc0);
        sd_set_switch(d, 0x1600, 0x3c0000);
        bgm_set_ssa(d);
        if (d->bgm_dev == 1) {
            d->bgm_fp = d->bgm_fptop;
            d->bgm_mode++;
        } else if (d->bgm_dev > 1) {
            d->bgm_fp = 0;
            if (d->se_loading < 1) {
                d->bgm_mode++;
            }
        }
        /* device 0, a file on the host, is the developers': it stays here as on a PS2 without one */
        break;
    }
    case 2:
        sd_set_switch(d, 0x1600, 0x3c0000);
        if (d->bgm_dev == 1) {
            uint64_t t = 0;
            d->read_ok = cd_read(d, d->bgm_fp, 0x10, d->wave, &t);
            bgm_block(d, RESUME_MODE2_READ, t);
        } else if (d->bgm_dev > 1) {
            memcpy(d->wave, d->iop_buf + (d->bgm_fp & (IOP_BUF_SIZE - 1)), WAVE_SIZE);
            d->bgm_fp += (int32_t)WAVE_SIZE;
            d->bgm_mode++;
            bgm_block(d, RESUME_NONE, SNDRV_IOP_COPY);
        }
        break;
    case 3:
        sd_set_switch(d, 0x1600, 0x3c0000);
        d->bgm_bank = -1;
        d->intsw = 0;
        sd_voice_trans(d, 1, d->wave, 0x5000, WAVE_SIZE);
        d->resume = RESUME_WAIT_INTSW;
        break;
    case 4:
        if (d->bgm_kon != 0) {
            sd_set_switch(d, 0x1500, 0x3c0000);
            bgm_set_ssa(d);
            if (d->bgm_dev > 1) {
                memcpy(d->wave, d->iop_buf + (d->bgm_fp & (IOP_BUF_SIZE - 1)), WAVE_SIZE);
                d->bgm_fp += (int32_t)WAVE_SIZE;
                bgm_mode4_rest(d);
                bgm_block(d, RESUME_NONE, SNDRV_IOP_COPY);
                break;
            }
            if (d->bgm_dev == 1) {
                uint64_t t = 0;
                if (!cd_read(d, d->bgm_fp, 0x10, d->wave, &t)) {
                    break;
                }
                d->bgm_fp += 0x10;
                bgm_block(d, RESUME_MODE4_READ, t);
                break;
            }
            bgm_mode4_rest(d);
        } else if (d->bgm_switch == 0) {
            bgm_vol_clr(d);
            sd_set_switch(d, 0x1600, 0x3c0000);
            d->bgm_mode = 0;
            d->bgm_dev = 0;
        }
        break;
    case 5:
        bgm_stream(d);
        break;
    default:
        break;
    }
}

/* The BGM thread's turn: if it isn't blocked, it goes on where it was, then
   runs one pass. */
static void bgm_poll(Sndrv *d)
{
    if (d->dma1_pending && d->now >= d->dma1_done) {
        d->dma1_pending = 0;
        bgm_irq(d);
    }
    if (d->now < d->wake) {
        return;
    }
    switch (d->resume) {
    case RESUME_MODE2_READ:
        d->resume = RESUME_NONE;
        if (d->read_ok) {
            d->bgm_fp += 0x10;
            d->bgm_mode++;
        }
        return;
    case RESUME_MODE4_READ:
        d->resume = RESUME_NONE;
        bgm_mode4_rest(d);
        return;
    case RESUME_STREAM_READ:
        d->resume = RESUME_NONE;
        bgm_stream_read_done(d);
        return;
    case RESUME_WAIT_INTSW:
        if (!d->intsw) {
            return;
        }
        d->resume = RESUME_NONE;
        d->bgm_bank = 0;
        d->bgm_mode++;
        return;
    default:
        break;
    }
    bgm_main(d);
}

/* ------------------------------------------------------------------------
 * sndrv.c: the module, its RPC server and its init command
 * ------------------------------------------------------------------------ */

static void sndrv_init(Sndrv *d)
{
    int core, i;

    for (core = 0; core < 2; core++) {
        sd_set_switch(d, (uint32_t)core | 0x1300, 0);
        sd_set_switch(d, (uint32_t)core | 0x1400, 0);
        sd_set_param(d, (uint32_t)core | 0x980, 0x3fff);
        sd_set_param(d, (uint32_t)core | 0xa80, 0x3fff);
    }
    /* sndrv_mvolume(0x3fff) */
    sd_set_switch(d, 0x980, 0x3fff);
    sd_set_switch(d, 0xa80, 0x3fff);
    sd_set_param(d, 0xf80, 0);
    sd_set_param(d, 0x1080, 0);
    for (i = 0; i < 24; i++) {
        sd_set_param(d, (uint32_t)i << 1, 0);
        sd_set_param(d, (uint32_t)i << 1 | 0x100, 0);
    }
    for (core = 0; core < 2; core++) {
        for (i = 0; i < 24; i++) {
            uint32_t v = (uint32_t)core | (uint32_t)i << 1;
            sd_set_param(d, v | 0x200, 0x555);   /* 16 kHz */
            sd_set_param(d, v | 0x300, 0x81ff);
            sd_set_param(d, v | 0x400, 0xdfe1);
        }
        /* both passes set core 0's (sceSdSetEffectAttr(0, ...)): mode 0, depth 0 */
        sd_set_effect_attr_off(d, 0, 0, 0);
    }
    sd_set_core_attr(d, 2, 0);
    sd_set_param(d, 0xb80, 0);
    sd_set_param(d, 0xc80, 0);
    sd_set_param(d, 0xd80, 0);
    sd_set_param(d, 0xe80, 0);
    sd_set_switch(d, 0x1900, 0);
    sd_set_switch(d, 0x1b00, 0);
    sd_set_switch(d, 0x1800, 0xffffff);
    sd_set_switch(d, 0x1a00, 0xffffff);
    sd_set_param(d, 0x800, 0xc00);
    sd_set_addr(d, 0x1c00, 0x3f0000);
    se_init(d);
}

/* The SPU2 running with no one listening: sceSdInit's waits, before the driver's clock starts. */
static void sd_wait(Sndrv *d, uint32_t samples)
{
    int16_t o[2];

    while (samples--) {
        spu2_tick(&d->spu, o);
    }
}

/* sceSdInit (LIBSD.IRX), as its writes come on PCSX2 (AUDIO.md, SPU2 lockstep): SPDIF out, each core's
   volumes, ADMA and attributes and every voice keyed off, the reverb areas, the block every voice starts on
   (looping on itself) through core 0's data port, every voice's parameters and start, then every voice keyed
   on and, after a wait, off again; then the cores' attributes, every voice to both mixes, the cores' mixes,
   their volumes zero but core 1's external input, the reverb areas ending at 0xEFFFF and 0xFFFFF. */
static void sd_init(Sndrv *d)
{
    static const uint16_t CORES[][2] = {
        { 0x7c8, 0x200 }, { 0x7c0, 0x20 }, { 0x7c6, 0x800 }, { 0x7c6, 0x900 }, { 0x7c8, 0x200 }, { 0x7ca, 8 },
        { 0x7c0, 0 }, { 0x7c0, 0x8000 },
        { 0x760, 0 }, { 0x762, 0 }, { 0x360, 0 }, { 0x362, 0 }, { 0x1b0, 0 }, { 0x19a, 0 }, { 0x19a, 0x8000 },
        { 0x760, 0 }, { 0x762, 0 }, { 0x1a4, 0xffff }, { 0x1a6, 0xffff },
        { 0x5b0, 0 }, { 0x59a, 0 }, { 0x59a, 0x8000 }, { 0x788, 0 }, { 0x78a, 0 }, { 0x5a4, 0xffff },
        { 0x5a6, 0xffff }, { 0x580, 0 }, { 0x582, 0 }, { 0x584, 0 }, { 0x586, 0 }, { 0x580, 0 }, { 0x582, 0 },
        { 0x584, 0 }, { 0x586, 0 },
        { 0x2e0, 0xe }, { 0x2e2, 0xfff8 }, { 0x6e0, 0xf }, { 0x6e2, 0xfff8 }, { 0x1a8, 0 }, { 0x1aa, 0x2800 },
    };
    static const uint16_t SETUP[][2] = {
        { 0x342, 0 }, { 0x340, 0 },
        { 0x7c0, 0xc032 }, { 0x19a, 0xc000 }, { 0x59a, 0xc001 },
        { 0x188, 0xffff }, { 0x18a, 0xff }, { 0x190, 0xffff }, { 0x192, 0xff }, { 0x18c, 0xffff }, { 0x18e, 0xff },
        { 0x194, 0xffff }, { 0x196, 0xff }, { 0x588, 0xffff }, { 0x58a, 0xff }, { 0x590, 0xffff }, { 0x592, 0xff },
        { 0x58c, 0xffff }, { 0x58e, 0xff }, { 0x594, 0xffff }, { 0x596, 0xff }, { 0x198, 0xff0 }, { 0x598, 0xffc },
        { 0x760, 0 }, { 0x762, 0 }, { 0x788, 0 }, { 0x78a, 0 }, { 0x764, 0 }, { 0x766, 0 }, { 0x78c, 0 },
        { 0x78e, 0 }, { 0x33c, 0xe }, { 0x73c, 0xf }, { 0x768, 0 }, { 0x76a, 0 }, { 0x790, 0x7fff },
        { 0x792, 0x7fff }, { 0x76c, 0 }, { 0x76e, 0 }, { 0x794, 0 }, { 0x796, 0 },
    };
    static const uint16_t LOOP[16] = { 0x707, 0x707, 0x707, 0x707, 0x707, 0x707, 0x707, 0x707 };
    size_t i;
    int v, core;

    for (i = 0; i < sizeof(CORES) / sizeof(CORES[0]); i++) {
        spu2_write(&d->spu, CORES[i][0], CORES[i][1]);
    }
    for (i = 0; i < 16; i++) {
        spu2_write(&d->spu, 0x1ac, LOOP[i]);
        spu2_write_words(&d->spu, 0x2800 + (uint32_t)i, &LOOP[i], 1);
    }
    spu2_write(&d->spu, 0x19a, 0x8010);
    spu2_write(&d->spu, 0x19a, 0x8000);
    for (v = 0; v < SPU2_VOICES; v++) {
        static const uint16_t PARAM[5] = { 0, 0, 0x3fff, 0, 0 };
        for (i = 0; i < 5; i++) {
            for (core = 1; core >= 0; core--) {
                spu2_write(&d->spu, (uint32_t)core * 0x400 + (uint32_t)v * 0x10 + (uint32_t)i * 2, PARAM[i]);
            }
        }
        for (i = 0; i < 2; i++) {
            for (core = 1; core >= 0; core--) {
                spu2_write(&d->spu, (uint32_t)core * 0x400 + 0x1c0 + (uint32_t)v * 12 + (uint32_t)i * 2,
                           i ? 0x2800 : 0);
            }
        }
    }
    spu2_write(&d->spu, 0x5a0, 0xffff);
    spu2_write(&d->spu, 0x1a0, 0xffff);
    spu2_write(&d->spu, 0x5a2, 0xff);
    spu2_write(&d->spu, 0x1a2, 0xff);
    sd_wait(d, 36);
    spu2_write(&d->spu, 0x5a4, 0xffff);
    spu2_write(&d->spu, 0x1a4, 0xffff);
    spu2_write(&d->spu, 0x5a6, 0xff);
    spu2_write(&d->spu, 0x1a6, 0xff);
    sd_wait(d, 37);
    for (i = 0; i < sizeof(SETUP) / sizeof(SETUP[0]); i++) {
        spu2_write(&d->spu, SETUP[i][0], SETUP[i][1]);
    }
}

Sndrv *sndrv_create(SndDiscRead read, void *ctx)
{
    return sndrv_create_attached(read, ctx, NULL, NULL);
}

Sndrv *sndrv_create_attached(SndDiscRead read, void *ctx, void (*attach)(Spu2 *s, void *arg), void *arg)
{
    Sndrv *d = calloc(1, sizeof(*d));

    if (!d) {
        return NULL;
    }
    d->disc = read;
    d->disc_ctx = ctx;
    d->rpc_tail = &d->rpc_head;
    /* start: the module's data, the effect thread's queue, then bgm_all_init and sceSdInit (sedrv_main), from
       the SPU2's power-on */
    d->bgm_fp = -1;
    spu2_reset(&d->spu);
    if (attach) {
        attach(&d->spu, arg);
    }
    /* bgm_all_init, before sceSdInit (whose voice reset undoes its volumes) */
    d->bgm_kon = d->bgm_switch = d->bgm_mode = d->bgm_dev = d->req_bgm_mode = 0;
    d->bgm_vol = 0x3fff;
    d->bgm_channel = 0;
    bgm_vol_out(d);
    sd_wait(d, 24);
    sd_init(d);
    return d;
}

int sndrv_copy(Sndrv *dst, const Sndrv *src)
{
    const Rpc *q;
    Rpc *head = NULL, **tail = &head;

    /* the calls waiting first, so a failure leaves dst as it was */
    for (q = src->rpc_head; q; q = q->next) {
        Rpc *r = malloc(sizeof(*r) + q->size);

        if (!r) {
            while (head) {
                Rpc *n = head->next;
                free(head);
                head = n;
            }
            return -1;
        }
        memcpy(r, q, sizeof(*r) + q->size);
        r->next = NULL;
        *tail = r;
        tail = &r->next;
    }
    while (dst->rpc_head) {
        Rpc *r = dst->rpc_head;
        dst->rpc_head = r->next;
        free(r);
    }
    memcpy(dst, src, sizeof(*dst));
    dst->rpc_head = head;
    dst->rpc_tail = head ? tail : &dst->rpc_head;
    return 0;
}

void sndrv_free(Sndrv *d)
{
    while (d && d->rpc_head) {
        Rpc *r = d->rpc_head;
        d->rpc_head = r->next;
        free(r);
    }
    free(d);
}

/* sedrv_command: the RPC thread runs a call. */
static void sedrv_command(Sndrv *d, uint32_t cmd, const uint8_t *data, uint32_t size)
{
    uint8_t *arg = d->rpc_arg;
    uint32_t c = cmd & 0x7f;

    /* the call's bytes land in the server's buffer over what the last left */
    if (size > RPC_ARG_SIZE) {
        size = RPC_ARG_SIZE;
    }
    if (data && size) {
        memcpy(arg, data, size);
    }
    d->ret = 0;
    d->dma0_samples = 0;
    switch (c) {
    case SND_INIT: sndrv_init(d); break;
    case SND_MVOL:
        sd_set_switch(d, 0x980, rd32(arg));
        sd_set_switch(d, 0xa80, rd32(arg));
        break;
    case SND_SE_STOP: case SND_SE_ALLSTOP: case SND_SE_VOLUME: case SND_SE_TRANS: case SND_SE_AGAIN:
    case SND_SE_BANK:
        se_command(d, c, arg, (int32_t)size);
        break;
    case SND_SE_LOAD_PC: case SND_SE_LOAD_CD:
        d->se_loading = 1;
        se_command(d, c, arg, (int32_t)size);
        break;
    case SND_BGM_LOAD_CD:
        d->req_bgm_dev = 1;
        d->req_bgm_fptop = (int32_t)rd32(arg);
        d->req_bgm_size = (int32_t)rd32(arg + 4);
        d->req_bgm_mode = 1;
        break;
    case SND_BGM_LOAD_PC:
        d->req_bgm_dev = 0;
        d->req_bgm_mode = 1;
        break;
    case SND_BGM_START:
        d->bgm_kon = 1;
        d->bgm_loop = (int32_t)rd32(arg);
        break;
    case SND_BGM_STOP:
        d->bgm_switch = 0;
        d->req_bgm_mode = 0;
        d->bgm_kon = 0;
        break;
    case SND_BGM_VOLUME:
        d->bgm_vol = (int32_t)rd32(arg);
        d->bgm_channel = (int32_t)rd32(arg + 4);
        bgm_vol_out(d);
        break;
    case SND_BGM_KEEP:
        d->bgm_loop = (int32_t)rd32(arg);
        d->req_bgm_dev = 2;
        d->req_bgm_size = (int32_t)rd32(arg + 4);
        d->req_bgm_mode = 1;
        d->bgm_kon = 1;
        d->bgm_fp = 0;
        break;
    case SND_BGM_STATUS:
        if ((d->req_bgm_mode > 0 || d->bgm_mode > 0) && d->bgm_dev < 2) {
            d->ret = 1;
        }
        break;
    case SND_SE_STATUS:
        d->ret = (uint32_t)d->se_loading;
        if (d->se_loading < 0) {
            d->se_loading = 0;
        }
        break;
    default:
        break;
    }
    if (cmd & SND_LAST) {
        se_command(d, SND_KEY_ON, arg, 0);
    }
    /* the call returns once its transfer is done */
    d->rpc_free = d->now + d->dma0_samples;
}

/* in the RPC thread's queue, so it keeps its place among the calls: the game's own disc read */
#define GAME_READ 0xffffffffu

void sndrv_game_read(Sndrv *d, uint32_t lsn, uint32_t sectors)
{
    uint8_t b[8];

    memcpy(b, &lsn, 4);
    memcpy(b + 4, &sectors, 4);
    sndrv_command(d, GAME_READ, b, 8);
}

void sndrv_command(Sndrv *d, uint32_t cmd, const uint8_t *data, uint32_t size)
{
    Rpc *r = malloc(sizeof(*r) + (data ? size : 0));

    if (!r) {
        return;
    }
    r->next = NULL;
    r->cmd = cmd;
    r->size = data ? size : 0;
    if (data && size) {
        memcpy(r->data, data, size);
    }
    *d->rpc_tail = r;
    d->rpc_tail = &r->next;
}

/* The IOP at one sample: the RPC thread takes the next call when the last
   one's transfer is done, and the effect thread runs while the RPC thread
   waits for a call (StartThread readies it; it is the lower priority). */
static void iop_tick(Sndrv *d)
{
    int ch;

    for (ch = 0; ch < 2; ch++) {
        if (d->dma_attr[ch] && d->now >= d->dma_attr[ch]) {
            uint32_t a = 0x19a + (uint32_t)ch * 0x400;
            d->dma_attr[ch] = 0;
            spu2_write(&d->spu, a, (uint16_t)(spu2_read(&d->spu, a) & ~0x30u));
        }
    }
    while (d->rpc_head && d->now >= d->rpc_free) {
        Rpc *r = d->rpc_head;

        d->rpc_head = r->next;
        if (!d->rpc_head) {
            d->rpc_tail = &d->rpc_head;
        }
        if (r->cmd == GAME_READ) {
            (void)drive(d, rd32(r->data), rd32(r->data + 4), 0);   /* the EE's read: the IOP's threads go on */
        } else {
            sedrv_command(d, r->cmd, r->size ? r->data : NULL, r->size);
        }
        free(r);
    }
    if (d->now >= d->rpc_free) {
        se_main(d);
    }
    if (d->now % SNDRV_POLL == 0) {
        bgm_poll(d);
    }
}

void sndrv_render(Sndrv *d, int16_t *out, uint32_t frames)
{
    uint32_t i;

    for (i = 0; i < frames; i++) {
        iop_tick(d);
        spu2_tick(&d->spu, out + 2 * i);
        d->now++;
    }
}

Spu2 *sndrv_spu2(Sndrv *d)
{
    return &d->spu;
}
