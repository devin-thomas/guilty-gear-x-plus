/* SNDRV.IRX, the game's IOP sound driver, re-implemented (PORT-011, ADR-011).
 *
 * The driver is three source files on the PS2 (its symbols name them):
 * sndrv.c takes the RPC calls, se.c plays effects and voices, bgm.c streams
 * music. This is the same driver, command for command, on the SPU2 model
 * (spu2.h) through the libsd calls it makes, with the IOP's threads made
 * into steps of one timeline:
 *
 *   - the RPC thread, the IOP's highest priority, runs the calls in order,
 *     each when the last one's DMA transfer is done; the effect thread it
 *     starts drains its queue as soon as the RPC thread waits again, so an
 *     effect is keyed on once its upload to sound RAM has finished;
 *   - the BGM thread, the lowest priority, polls every SNDRV_POLL samples,
 *     as the PS2's spins between the others: it streams the music from the
 *     disc into SPU2 RAM half a buffer at a time as the voices play it;
 *   - transfers and disc reads take time (SNDRV_DMA_BYTES, SNDRV_CD_*):
 *     a thread waiting for one sleeps, and a transfer's completion
 *     interrupt runs at the first poll after it.
 *
 * audio/README.md explains the commands and how each part was derived from
 * the driver's code.
 */
#ifndef GGX_SNDRV_H
#define GGX_SNDRV_H

#include <stdint.h>

#include "spu2.h"

/* How often the BGM thread looks at the voices, in 48 kHz samples. */
#define SNDRV_POLL 32
/* SPU2 DMA: bytes moved a sample (a word every 24 IOP cycles, PCSX2's rate). */
#define SNDRV_DMA_BYTES 64
/* The disc drive, in samples (audio/README.md, Time; measured on PCSX2
   2.8.2, whose drive model these are): a sector's read; no seek within
   SNDRV_CD_NEAR sectors of the drive's last read, a short seek within
   SNDRV_CD_FAR, a long one beyond; each read's start; and a change of
   spindle speed from the nominal speed the game's own reads use to the
   maximum the driver's use, longer the further out the head is (PCSX2
   scales it by the change in RPM): SNDRV_CD_SPEED plus SNDRV_CD_SPEED_LSN
   / 100000 samples a sector of LSN, fitted to 311 ms at LSN 92282 (KR) and
   344 ms at 163416 (JP). */
#define SNDRV_CD_SECTOR 39
#define SNDRV_CD_START 293
#define SNDRV_CD_NEAR 8
#define SNDRV_CD_FAR 4371
#define SNDRV_CD_SHORT_SEEK 1440
#define SNDRV_CD_SEEK 4800
#define SNDRV_CD_SPEED 12874
#define SNDRV_CD_SPEED_LSN 2227
/* The BGM thread's byte-by-byte copy of 32 KB of a kept BGM: 26 unoptimised
   instructions a byte, which PCSX2's IOP runs in 19 ms (about 22 cycles a
   byte at 36.9 MHz). */
#define SNDRV_IOP_COPY 912

/* Reads count 2048-byte sectors from lsn: 0 on success. */
typedef int (*SndDiscRead)(void *ctx, uint32_t lsn, uint32_t count, void *dst);

typedef struct Sndrv Sndrv;

Sndrv *sndrv_create(SndDiscRead read, void *ctx);
/* The same, calling attach on the SPU2 at its power-on, before the driver's first write: a tool's hooks
   (Spu2.on_write ...) then see every write the driver makes (ggx-snd render --tap). */
Sndrv *sndrv_create_attached(SndDiscRead read, void *ctx, void (*attach)(Spu2 *s, void *arg), void *arg);
void sndrv_free(Sndrv *d);
/* Makes dst the same driver as src, at the same time, with the same calls
   waiting and the same SPU2 (hooks and disc reader included): a rollback
   puts the driver back this way (audio_host.h). 0 on success. */
int sndrv_copy(Sndrv *dst, const Sndrv *src);
/* An RPC call from the game, at the current time: queued for the RPC
   thread. (The driver's answers, the 0x1F and 0x20 statuses, never reach
   the port's Core: see audio/README.md.) */
void sndrv_command(Sndrv *d, uint32_t cmd, const uint8_t *data, uint32_t size);
/* The game read the disc itself (sceCdRead, nominal speed), at the current
   time: the drive is busy with it, then left after it. */
void sndrv_game_read(Sndrv *d, uint32_t lsn, uint32_t sectors);
/* Runs the driver and the SPU2 for frames samples: interleaved stereo. */
void sndrv_render(Sndrv *d, int16_t *out, uint32_t frames);
/* For tests and tools. */
Spu2 *sndrv_spu2(Sndrv *d);

#endif
