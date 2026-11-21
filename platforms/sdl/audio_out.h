/* ggx's speakers (PORT-011): the sound driver's samples on SDL3 audio.
 *
 * The driver renders each field's 800.8 samples after the field runs
 * (audio/audio_host.h) and they are queued on an SDL audio stream at 48 kHz,
 * 16-bit stereo, the SPU2's own format. The Core's clock is the host's
 * (ADR-013) and the device has its own, so the queue drifts; the stream's
 * playback rate is nudged, from the queue smoothed over a second, to hold
 * about three fields queued (the clocks differ by parts per million).
 * Nothing here reaches the Core.
 */
#ifndef GGX_AUDIO_OUT_H
#define GGX_AUDIO_OUT_H

#include <stdint.h>

typedef struct AudioOut AudioOut;

/* NULL, with the reason on stderr, if there is no audio device: ggx then plays without sound. */
AudioOut *audio_out_open(void);
void audio_out_put(AudioOut *o, const int16_t *samples, uint32_t frames);
/* One stderr line: how often the queue ran dry or overflowed, its range, and the rate it settled on. */
void audio_out_report(const AudioOut *o);
void audio_out_close(AudioOut *o);

#endif
