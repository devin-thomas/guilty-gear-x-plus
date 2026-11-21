/* The audio layer on a Core host (PORT-011).
 *
 * Attached to a CoreHost, it takes the sound command stream from the SIF
 * shim (CoreHost.snd_rpc) and can record it (a .ggxs stream file,
 * snd_stream.h), play it through the re-implemented sound driver into a WAV
 * file, or both; ggx also plays it on the speakers (sdl/audio_out.c). It
 * only reads what the game sent: nothing here writes Core memory or any
 * state the State Hash covers, and a run with it attached computes exactly
 * what the same run without it computes.
 *
 * The driver reads the BGM and the voice banks from the disc by LSN, as
 * SNDRV.IRX does, through the host's Disc: the extracted assets or the
 * image, which are read-only input.
 */
#ifndef GGX_AUDIO_HOST_H
#define GGX_AUDIO_HOST_H

#include <stdint.h>

#include "core_host.h"
#include "sndrv.h"

typedef struct {
    const char *snd_log;   /* record the stream here */
    const char *wav;       /* render it into this WAV file (48 kHz, 16-bit stereo) */
    int play;              /* render it for a live output too: audio_host_take hands out the samples */
} AudioHostConfig;

typedef struct AudioHost AudioHost;

/* NULL if a file can't be created (the message is on stderr). With nothing
   asked for, returns NULL without attaching anything. */
AudioHost *audio_host_open(CoreHost *h, const AudioHostConfig *cfg);
/* After each field the Core ran (the boot included): the field's length in
   PS2 vblanks. Renders that much sound. */
void audio_host_field(AudioHost *a, uint32_t field, uint32_t vblanks);
/* For a live output: the samples rendered since the last call (interleaved
   stereo), and how many frames. */
const int16_t *audio_host_take(AudioHost *a, uint32_t *frames);
/* Detaches from the host and closes the files; 0 if every write succeeded. */
int audio_host_close(AudioHost *a);

/* Rollback (PORT-013). A session under rollback (rollback.h) runs fields
 * again after it predicted wrong. The driver must not hear a field twice
 * (doubled sound), and a field run again with other sound must not be left
 * out (lost sound). Followed, the layer plays each field once, as it first
 * runs, and keeps each field's calls since the last confirmed field, with
 * a second driver as of that field. When a rollback runs fields again it
 * plays nothing; once it has caught up, it compares what those fields sent
 * with what played. The same: the playing driver already is where it would
 * be. Different: the playing driver becomes a copy of the confirmed one,
 * fed every field since as it is now without playing them, so from the
 * next field it plays what a run that never predicted would play. Only the
 * fields that played while the prediction was wrong sound different, as
 * they did; nothing is played twice. The cost is the second driver, which
 * renders every confirmed field, and the copy (about 11 MB) when a
 * rollback changed the sound.
 *
 * audio_host_follow takes the session's on_field, on_load and on_confirm
 * (session.h): the session then drives audio_host_field. The layer must be
 * playing or writing a WAV (it has a driver). 0 on success. */
typedef struct Session Session;
int audio_host_follow(AudioHost *a, Session *s);

typedef struct {
    uint64_t played;        /* fields played */
    uint64_t resimulated;   /* fields run again, not played */
    uint64_t rollbacks;
    uint64_t rebuilds;      /* rollbacks that changed the sound: the driver put back */
    uint64_t tainted;       /* fields that played what a rollback then changed */
    uint64_t lost;          /* fields the layer couldn't place (a bug if not 0) */
} AudioRollbackStats;
const AudioRollbackStats *audio_host_rollback_stats(const AudioHost *a);

/* For tests (ggx-rollback-test): on_final is told each field's calls once
   they are final (at once without rollback, when confirmed with it), as a
   hash; on_taint, the fields that played sound a rollback then changed
   (first up to end); on_played, each field's samples as they play. */
typedef struct {
    void (*on_final)(void *ctx, uint32_t field, uint64_t calls);
    void (*on_played)(void *ctx, uint32_t field, const int16_t *samples, uint32_t frames);
    void (*on_taint)(void *ctx, uint32_t first, uint32_t end);
    void *ctx;
} AudioWatch;
void audio_host_watch(AudioHost *a, const AudioWatch *w);

#endif
