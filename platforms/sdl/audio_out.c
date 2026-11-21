#include "audio_out.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

/* about three fields queued; more than half a second is dropped (after a stall, or the developer build's
   fast mode) */
#define TARGET_FRAMES 2400
#define MAX_FRAMES 24000

struct AudioOut {
    SDL_AudioStream *s;
    float ratio;
    float level;   /* the queue, smoothed over about a second */
    uint32_t puts, dry, cleared;   /* fields queued, times the queue was found empty, times it was dropped */
    int low, high;                 /* the queue's range once it has settled */
    double sum_level, sum_ratio;   /* to report their means */
    uint32_t n_sum;
};

AudioOut *audio_out_open(void)
{
    const SDL_AudioSpec spec = { SDL_AUDIO_S16LE, 2, 48000 };
    AudioOut *o;

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        fprintf(stderr, "ggx: no sound: %s\n", SDL_GetError());
        return NULL;
    }
    if (!(o = calloc(1, sizeof(*o)))) {
        return NULL;
    }
    o->s = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!o->s) {
        fprintf(stderr, "ggx: no sound: %s\n", SDL_GetError());
        free(o);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return NULL;
    }
    o->ratio = 1.0f;
    o->level = TARGET_FRAMES;
    SDL_ResumeAudioStreamDevice(o->s);
    return o;
}

void audio_out_put(AudioOut *o, const int16_t *samples, uint32_t frames)
{
    int queued;
    float want;

    if (!o || !frames) {
        return;
    }
    queued = SDL_GetAudioStreamQueued(o->s) / 4;
    if (o->puts++ > 300) {
        /* past the boot, which fills the queue from nothing */
        o->dry += queued == 0;
        o->low = !o->n_sum || queued < o->low ? queued : o->low;
        o->high = queued > o->high ? queued : o->high;
        o->sum_level += queued;
        o->sum_ratio += o->ratio;
        o->n_sum++;
    }
    if (queued > MAX_FRAMES) {
        o->cleared++;
        SDL_ClearAudioStream(o->s);
        queued = 0;
        o->level = TARGET_FRAMES;
    }
    SDL_PutAudioStreamData(o->s, samples, (int)(frames * 4));
    /* the queue rises by a field at each put and falls as the device plays, so it is smoothed before it sets
       the rate: the two clocks differ by parts per million, and the correction stays far below hearing */
    o->level += ((float)queued - o->level) / 60.0f;
    want = 1.0f + (o->level - TARGET_FRAMES) / (float)TARGET_FRAMES * 0.002f;
    want = want < 0.995f ? 0.995f : want > 1.005f ? 1.005f : want;
    if (want != o->ratio) {
        o->ratio = want;
        SDL_SetAudioStreamFrequencyRatio(o->s, want);
    }
}

void audio_out_report(const AudioOut *o)
{
    if (o && o->n_sum) {
        fprintf(stderr, "ggx: sound: ran dry %u times, overflowed %u times; queue %d-%d frames, mean %.0f; rate %.5f\n",
                o->dry, o->cleared, o->low, o->high, o->sum_level / o->n_sum, o->sum_ratio / o->n_sum);
    }
}

void audio_out_close(AudioOut *o)
{
    if (o) {
        SDL_DestroyAudioStream(o->s);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        free(o);
    }
}
