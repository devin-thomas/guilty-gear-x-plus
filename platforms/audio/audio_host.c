#include "audio_host.h"

#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "session.h"
#include "snd_stream.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

#if defined(VERSION_KR)
#define RELEASE_ID 1u
#else
#define RELEASE_ID 2u
#endif

/* The most a field can be: 255 vblanks (the trace's cap) of 801 frames. */
#define MAX_FIELD_FRAMES (255u * 801u)

/* One field's sound: the calls and disc reads the game made, in order, as bytes. */
typedef struct {
    uint8_t *b;
    size_t n, cap;
    uint32_t field;
} FieldLog;

enum { EV_RPC = 1, EV_CD = 2 };

struct AudioHost {
    CoreHost *h;
    SndLog *log;
    Sndrv *drv;
    FILE *wav;
    uint64_t wav_frames;
    int err;
    uint64_t vblanks;      /* the timeline: PS2 vblanks so far */
    uint64_t frames;       /* samples rendered so far */
    int16_t *buf;          /* the last field's samples */
    uint32_t buf_frames;
    int play;

    /* Following a rollback session (audio_host_follow). The driver that plays
       (drv) hears each field once, as it first runs. base is the driver as
       of the first field not yet confirmed (base_field), with the vblank
       count then; log[] holds each field since, as it was played. */
    int follow;
    AudioWatch watch;
    Sndrv *base;
    uint32_t base_field;
    uint64_t base_vblanks;
    FieldLog *log_ring;     /* fields base_field .. played - 1, by field % LOG_RING */
    uint32_t played;        /* fields before this one have played */
    FieldLog cur;           /* the field running */
    int resim;              /* running fields again after a rollback: they don't play */
    uint32_t first_diff;    /* the first field run again whose sound changed, or NONE */
    int16_t *scratch;       /* samples rendered and not played */
    AudioRollbackStats st;
};

#define LOG_RING 64
#define NONE UINT32_MAX

static int log_put(FieldLog *l, const void *p, size_t n)
{
    if (l->n + n > l->cap) {
        size_t cap = l->cap ? l->cap : 256;
        uint8_t *b;

        while (cap < l->n + n) {
            cap *= 2;
        }
        if (!(b = realloc(l->b, cap))) {
            return -1;
        }
        l->b = b;
        l->cap = cap;
    }
    memcpy(l->b + l->n, p, n);
    l->n += n;
    return 0;
}

static void log_event(FieldLog *l, uint8_t type, uint32_t x, uint32_t y, const uint8_t *data)
{
    uint8_t h[9];

    h[0] = type;
    memcpy(h + 1, &x, 4);
    memcpy(h + 5, &y, 4);
    log_put(l, h, 9);
    if (type == EV_RPC && data && y) {
        log_put(l, data, y);
    }
}

/* Feeds a field's calls to a driver, in the order the game made them. */
static void log_feed(Sndrv *d, const FieldLog *l)
{
    size_t i = 0;

    while (i + 9 <= l->n) {
        uint32_t x, y;

        memcpy(&x, l->b + i + 1, 4);
        memcpy(&y, l->b + i + 5, 4);
        if (l->b[i] == EV_RPC) {
            sndrv_command(d, x, y ? l->b + i + 9 : NULL, y);
            i += 9 + y;
        } else {
            sndrv_game_read(d, x, y);
            i += 9;
        }
    }
}

static uint64_t log_hash(const FieldLog *l)
{
    return XXH3_64bits(l->b ? l->b : (const uint8_t *)"", l->n);
}

/* samples a field of vblanks ends at, counting from vblank 0 */
static uint64_t frames_at(uint64_t vblanks)
{
    return vblanks * 4004 / 5;
}

static void on_rpc(void *ctx, uint32_t field, uint32_t cmd, const uint8_t *data, uint32_t size)
{
    AudioHost *a = ctx;

    if (a->log && !a->resim) {
        snd_log_cmd(a->log, field, cmd, data, size);
    }
    if (a->follow || a->watch.on_final) {
        log_event(&a->cur, EV_RPC, cmd, size, data);
    }
    if (a->drv && !a->resim) {
        sndrv_command(a->drv, cmd, data, size);
    }
}

static void on_cd_read(void *ctx, uint32_t field, uint32_t lsn, uint32_t sectors)
{
    AudioHost *a = ctx;

    if (a->log && !a->resim) {
        snd_log_disc(a->log, field, lsn, sectors);
    }
    if (a->follow || a->watch.on_final) {
        log_event(&a->cur, EV_CD, lsn, sectors, NULL);
    }
    if (a->drv && !a->resim) {
        sndrv_game_read(a->drv, lsn, sectors);
    }
}

static int disc_reader(void *ctx, uint32_t lsn, uint32_t count, void *dst)
{
    return disc_read(&((CoreHost *)ctx)->disc, lsn, count, dst);
}

static void wav_header(FILE *f, uint64_t frames)
{
    uint32_t data = (uint32_t)(frames * 4);
    uint8_t h[44] = { 'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 2, 0,
                      0x80, 0xbb, 0, 0, 0, 0xee, 2, 0, 4, 0, 16, 0, 'd', 'a', 't', 'a', 0, 0, 0, 0 };
    uint32_t riff = data + 36;

    memcpy(h + 4, &riff, 4);
    memcpy(h + 40, &data, 4);
    fwrite(h, 1, sizeof(h), f);
}

AudioHost *audio_host_open(CoreHost *h, const AudioHostConfig *cfg)
{
    AudioHost *a;

    if (!cfg->snd_log && !cfg->wav && !cfg->play) {
        return NULL;
    }
    if (!(a = calloc(1, sizeof(*a))) || !(a->buf = malloc(MAX_FIELD_FRAMES * 4))) {
        free(a);
        return NULL;
    }
    a->h = h;
    a->play = cfg->play;
    if (cfg->snd_log && !(a->log = snd_log_create(cfg->snd_log, RELEASE_ID))) {
        fprintf(stderr, "ggx: cannot write %s\n", cfg->snd_log);
        audio_host_close(a);
        return NULL;
    }
    if (cfg->wav && !(a->wav = fs_open(cfg->wav, "wb"))) {
        fprintf(stderr, "ggx: cannot write %s\n", cfg->wav);
        audio_host_close(a);
        return NULL;
    }
    if (a->wav) {
        wav_header(a->wav, 0);
    }
    if ((cfg->wav || cfg->play) && !(a->drv = sndrv_create(disc_reader, h))) {
        audio_host_close(a);
        return NULL;
    }
    h->snd_rpc = on_rpc;
    h->snd_cd_read = on_cd_read;
    h->snd_ctx = a;
    return a;
}

static void play_field(AudioHost *a, uint32_t field, uint32_t vblanks);
static void follow_field(AudioHost *a, uint32_t field, uint32_t vblanks);

void audio_host_field(AudioHost *a, uint32_t field, uint32_t vblanks)
{
    if (!a) {
        return;
    }
    if (a->follow) {
        follow_field(a, field, vblanks);
        return;
    }
    play_field(a, field, vblanks);
    if (a->watch.on_final) {
        a->watch.on_final(a->watch.ctx, field, log_hash(&a->cur));
    }
    a->cur.n = 0;
}

static void play_field(AudioHost *a, uint32_t field, uint32_t vblanks)
{
    uint64_t end;
    uint32_t n;

    if (a->log) {
        snd_log_field(a->log, field, vblanks);
    }
    if (!a->drv) {
        return;
    }
    /* 48 kHz over 59.94 Hz: 800.8 samples a vblank, whole samples by an exact count */
    a->vblanks += vblanks;
    end = a->vblanks * 4004 / 5;
    n = (uint32_t)(end - a->frames);
    if (n > MAX_FIELD_FRAMES) {
        n = MAX_FIELD_FRAMES;
    }
    a->frames = end;
    /* a live output takes the samples after each field; anything it left is kept if there is room */
    if (!a->play || a->buf_frames + n > MAX_FIELD_FRAMES) {
        a->buf_frames = 0;
    }
    sndrv_render(a->drv, a->buf + 2 * a->buf_frames, n);
    if (a->watch.on_played) {
        a->watch.on_played(a->watch.ctx, field, a->buf + 2 * a->buf_frames, n);
    }
    if (a->wav) {
        if (fwrite(a->buf + 2 * a->buf_frames, 4, n, a->wav) != n) {
            a->err = 1;
        }
        a->wav_frames += n;
    }
    a->buf_frames += n;
}

const int16_t *audio_host_take(AudioHost *a, uint32_t *frames)
{
    *frames = a ? a->buf_frames : 0;
    if (a) {
        a->buf_frames = 0;
    }
    return a ? a->buf : NULL;
}

int audio_host_close(AudioHost *a)
{
    int err;

    if (!a) {
        return 0;
    }
    if (a->h && a->h->snd_ctx == a) {
        a->h->snd_rpc = NULL;
        a->h->snd_cd_read = NULL;
        a->h->snd_ctx = NULL;
    }
    err = a->err;
    if (a->log && snd_log_close(a->log) != 0) {
        err = 1;
    }
    if (a->wav) {
        if (fseek(a->wav, 0, SEEK_SET) == 0) {
            wav_header(a->wav, a->wav_frames);
        }
        err |= fclose(a->wav) != 0;
    }
    if (a->follow) {
        uint32_t i;

        for (i = 0; i < LOG_RING; i++) {
            free(a->log_ring[i].b);
        }
        free(a->log_ring);
        sndrv_free(a->base);
        free(a->scratch);
    }
    free(a->cur.b);
    sndrv_free(a->drv);
    free(a->buf);
    free(a);
    return err ? -1 : 0;
}

/* ---- following a rollback session (PORT-013) ---- */

/* Runs a driver through a field it doesn't play: the same samples as if it did, thrown away. */
static void render_silent(AudioHost *a, Sndrv *d, uint64_t *vblanks, uint32_t field_vblanks)
{
    uint64_t from = frames_at(*vblanks), n;

    *vblanks += field_vblanks;
    n = frames_at(*vblanks) - from;
    if (n > MAX_FIELD_FRAMES) {
        n = MAX_FIELD_FRAMES;
    }
    sndrv_render(d, a->scratch, (uint32_t)n);
}

static void on_session_confirm(void *ctx, uint32_t field);

static void follow_field(AudioHost *a, uint32_t field, uint32_t vblanks)
{
    FieldLog *slot;

    /* a session that never confirms (no rollback): the oldest fields can't be rolled back to past the ring */
    if (field >= a->played && field - a->base_field >= LOG_RING - 1) {
        on_session_confirm(a, field - (LOG_RING - 2));
    }
    if (field < a->base_field || field - a->base_field >= LOG_RING) {
        /* the ring can't hold it: a rollback this layer wasn't told of */
        a->st.lost++;
        a->cur.n = 0;
        return;
    }
    slot = &a->log_ring[field % LOG_RING];
    a->cur.field = field;
    /* a field's vblanks go into its log's last word: the driver's time depends on them */
    log_put(&a->cur, &vblanks, 4);
    if (field >= a->played) {
        /* a new field: it plays, as it would without rollback */
        play_field(a, field, vblanks);
        a->played = field + 1;
        a->st.played++;
    } else {
        /* run again after a rollback: is it what played? */
        a->st.resimulated++;
        if (a->first_diff == NONE && (slot->n != a->cur.n || memcmp(slot->b, a->cur.b, a->cur.n) != 0)) {
            a->first_diff = field;
        }
    }
    {
        FieldLog t = *slot;

        *slot = a->cur;
        a->cur = t;
        a->cur.n = 0;
    }
    if (a->resim && field + 1 == a->played) {
        /* caught up */
        a->resim = 0;
        if (a->first_diff != NONE) {
            uint64_t v = a->base_vblanks;
            uint32_t f;

            /* the driver as it would be had it heard these fields as they are now: from the last confirmed
               field's state, fed every field since, without playing them again */
            if (sndrv_copy(a->drv, a->base) == 0) {
                for (f = a->base_field; f < a->played; f++) {
                    const FieldLog *l = &a->log_ring[f % LOG_RING];
                    uint32_t fv;

                    memcpy(&fv, l->b + l->n - 4, 4);
                    log_feed(a->drv, l);
                    render_silent(a, a->drv, &v, fv);
                }
                a->vblanks = v;
                a->frames = frames_at(v);
            } else {
                a->st.lost++;
            }
            a->st.rebuilds++;
            a->st.tainted += a->played - a->first_diff;
            if (a->watch.on_taint) {
                a->watch.on_taint(a->watch.ctx, a->first_diff, a->played);
            }
            a->first_diff = NONE;
        }
    }
}

static void on_session_field(void *ctx, uint32_t field)
{
    audio_host_field(ctx, field, 1);
}

static void on_session_load(void *ctx, uint32_t field)
{
    AudioHost *a = ctx;

    a->cur.n = 0;
    if (field < a->played) {
        a->resim = 1;
        a->first_diff = NONE;
        a->st.rollbacks++;
    }
}

static void on_session_confirm(void *ctx, uint32_t field)
{
    AudioHost *a = ctx;

    /* the fields before field are final: base hears them */
    while (a->base_field < field && a->base_field < a->played) {
        const FieldLog *l = &a->log_ring[a->base_field % LOG_RING];
        uint32_t fv;

        memcpy(&fv, l->b + l->n - 4, 4);
        log_feed(a->base, l);
        render_silent(a, a->base, &a->base_vblanks, fv);
        if (a->watch.on_final) {
            FieldLog without = *l;

            without.n -= 4;
            a->watch.on_final(a->watch.ctx, a->base_field, log_hash(&without));
        }
        a->base_field++;
    }
}

int audio_host_follow(AudioHost *a, Session *s)
{
    if (!a || !a->drv || a->follow) {
        return -1;
    }
    if (!(a->base = sndrv_create(disc_reader, a->h)) || sndrv_copy(a->base, a->drv) != 0 ||
        !(a->log_ring = calloc(LOG_RING, sizeof(*a->log_ring))) || !(a->scratch = malloc(MAX_FIELD_FRAMES * 4))) {
        return -1;
    }
    a->follow = 1;
    a->base_field = a->played = s->h->field;
    a->base_vblanks = a->vblanks;
    a->first_diff = NONE;
    s->on_field = on_session_field;
    s->on_load = on_session_load;
    s->on_confirm = on_session_confirm;
    s->on_ctx = a;
    return 0;
}

void audio_host_watch(AudioHost *a, const AudioWatch *w)
{
    a->watch = *w;
}

const AudioRollbackStats *audio_host_rollback_stats(const AudioHost *a)
{
    return &a->st;
}
