/* An SPU2 tap (PORT-011): what the recorder's
 * patch to PCSX2 2.8.2 (not in this repository)
 * writes, and ggx-snd render --tap for the port's driver. Little-endian:
 *
 *   "GGSPU2T\0", u32 version (1), then records:
 *   'S' u32 tick, 0x400 u16: every register as read when the tap opened
 *   'W' u32 tick, u32 address (0x1F900000 on), u16 value: a register write
 *   'M' u32 tick, u32 word address, u32 words, the words: a write into sound RAM from outside the mixer
 *   'K' u32 tick: a mark, the sound driver's first command (the port's taps only)
 *   'E' u32 ticks: the end
 *
 * A record's tick is the SPU2's samples mixed when it came, so it acts
 * before the sample after. Beside the tap, <tap>.pcm holds each sample
 * mixed, s16 left and right. Either may be zstd-compressed (.zst), which
 * zstd's tool, on the PATH, decompresses. */
#ifndef GGX_SPU2_TAP_H
#define GGX_SPU2_TAP_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    int tag;              /* 'S', 'W', 'M', 'K', 'E' */
    uint32_t tick, addr, words;
    uint16_t value;
    const uint16_t *data; /* 'S': 0x400 registers; 'M': the words */
} Spu2TapRecord;

typedef struct {
    FILE *f;
    int piped;
    uint16_t *buf;
} Spu2Tap;

/* Opens a tap, or its samples (raw: no header), compressed or not. */
FILE *spu2_tap_file(const char *path, int *piped);
void spu2_tap_file_close(FILE *f, int piped);
/* 0, or -1 if it can't be read or isn't a tap. */
int spu2_tap_open(Spu2Tap *t, const char *path);
/* The next record: 1, or 0 at the end ('E', or the file's end), or -1 if it is cut short or bad. */
int spu2_tap_next(Spu2Tap *t, Spu2TapRecord *r);
void spu2_tap_close(Spu2Tap *t);

#endif
