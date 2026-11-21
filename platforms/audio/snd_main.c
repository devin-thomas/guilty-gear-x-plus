/* ggx-snd: the sound command stream without the Core (PORT-011).
 *
 *   ggx-snd dump <stream.ggxs>
 *   ggx-snd render <stream.ggxs> <out.wav> [--data-dir <dir> | --disc <image>] [--mute bgm|se | --solo <voice>]
 *                 [--tap <file>]
 *
 * dump lists the commands, one line each, with the field and the vblank
 * they came at. render plays the stream through the sound driver into a
 * WAV file (48 kHz, 16-bit stereo), as ggx-replay --audio-out does while
 * replaying: the result is the same, byte for byte. The driver reads the
 * BGM and the voice banks from the player's disc (the extracted assets, by
 * default). --mute silences core 0's voices (the BGM) or core 1's (the
 * effects and voices), and --solo plays only one of core 1's voices (each
 * effect plays on one, in turn), for the clip analysis's stems. --tap
 * records the driver's SPU2 register writes and transfers into sound RAM,
 * each at its SPU2 tick, and the samples, in the format of the recorder's
 * taps of PCSX2 (a patch to its SPU2, not in this repository), from
 * the SPU2's power-on: ggx-spu2-replay replays it, ggx-spu2-compare puts
 * it beside PCSX2's.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assets.h"
#include "disc.h"
#include "fsys.h"
#include "iso9660.h"
#include "releases.h"
#include "snd_stream.h"
#include "sndrv.h"

static int disc_reader(void *ctx, uint32_t lsn, uint32_t count, void *dst)
{
    return disc_read(ctx, lsn, count, dst);
}

static int usage(void)
{
    fprintf(stderr, "usage: ggx-snd dump <stream.ggxs>\n"
                    "       ggx-snd render <stream.ggxs> <out.wav> [--data-dir <dir> | --disc <image>] [--mute bgm|se | --solo <voice>]\n"
                    "                [--tap <file>]\n");
    return 2;
}

static void put32(uint8_t *p, uint32_t v)
{
    memcpy(p, &v, 4);
}

/* --tap: the driver's register writes and transfers, as the recorder writes PCSX2's */
typedef struct {
    FILE *events, *pcm;
    int marked;
} Tap;

static void tap_u32(FILE *f, uint32_t v)
{
    uint8_t b[4];

    put32(b, v);
    fwrite(b, 1, 4, f);
}

static void tap_write(void *ctx, const Spu2 *s, uint32_t addr, uint16_t value)
{
    Tap *t = ctx;
    uint8_t b[2] = { (uint8_t)value, (uint8_t)(value >> 8) };

    fputc('W', t->events);
    tap_u32(t->events, s->cycles);
    tap_u32(t->events, 0x1f900000u + addr);
    fwrite(b, 1, 2, t->events);
}

static void tap_ram(void *ctx, const Spu2 *s, uint32_t addr, const uint16_t *words, uint32_t n)
{
    Tap *t = ctx;
    uint32_t i;

    fputc('M', t->events);
    tap_u32(t->events, s->cycles);
    tap_u32(t->events, addr);
    tap_u32(t->events, n);
    for (i = 0; i < n; i++) {
        uint8_t b[2] = { (uint8_t)words[i], (uint8_t)(words[i] >> 8) };
        fwrite(b, 1, 2, t->events);
    }
}

static void tap_sample(void *ctx, const Spu2 *s, const int16_t out[2])
{
    Tap *t = ctx;
    uint8_t b[4] = { (uint8_t)out[0], (uint8_t)((uint16_t)out[0] >> 8), (uint8_t)out[1], (uint8_t)((uint16_t)out[1] >> 8) };

    (void)s;
    fwrite(b, 1, 4, t->pcm);
}

static int tap_open(Tap *t, const char *path)
{
    char pcm[1100];

    snprintf(pcm, sizeof(pcm), "%s.pcm", path);
    if (!(t->events = fopen(path, "wb")) || !(t->pcm = fopen(pcm, "wb"))) {
        return -1;
    }
    return 0;
}

/* at the SPU2's power-on, before the driver's first write */
static void tap_attach(Spu2 *s, void *arg)
{
    Tap *t = arg;
    uint32_t a;

    if (!t->events) {
        return;
    }
    fwrite("GGSPU2T", 1, 8, t->events);
    tap_u32(t->events, 1);
    fputc('S', t->events);
    tap_u32(t->events, s->cycles);
    for (a = 0; a < 0x800; a += 2) {
        uint16_t v = spu2_read(s, a);
        uint8_t b[2] = { (uint8_t)v, (uint8_t)(v >> 8) };
        fwrite(b, 1, 2, t->events);
    }
    s->on_write = tap_write;
    s->on_ram = tap_ram;
    s->on_sample = tap_sample;
    s->on_ctx = t;
}

static void tap_close(Tap *t, const Spu2 *s)
{
    if (t->events) {
        fputc('E', t->events);
        tap_u32(t->events, s->cycles);
        fclose(t->events);
    }
    if (t->pcm) {
        fclose(t->pcm);
    }
}

static int dump(const char *path)
{
    char err[512], line[256];
    uint32_t release;
    uint64_t vb = 0;
    SndEvent ev;
    SndReader *r = snd_read_open(path, &release, err, sizeof(err));
    int rc;

    if (!r) {
        fprintf(stderr, "ggx-snd: %s\n", err);
        return 2;
    }
    printf("stream of %s\n", release == 1 ? "KR" : "JP");
    while ((rc = snd_read_next(r, &ev)) > 0) {
        if (ev.tag == 'F') {
            vb += ev.vblanks;
        } else if (ev.tag == 'R') {
            printf("f%06u vb%06llu (the game reads lsn %u, %u sectors)\n", ev.field, (unsigned long long)vb, ev.lsn,
                   ev.sectors);
        } else {
            snd_cmd_describe(ev.cmd, ev.data, ev.size, line, sizeof(line));
            printf("f%06u vb%06llu %s\n", ev.field, (unsigned long long)vb, line);
        }
    }
    snd_read_close(r);
    return rc < 0 ? 1 : 0;
}

static int render(const char *path, const char *out, const char *data_dir, const char *disc_path, const uint32_t mute[2],
                  const char *tap_path)
{
    char err[512], pref[1024];
    uint32_t release;
    uint64_t vb = 0, frames = 0, written = 0;
    SndEvent ev;
    SndReader *r = snd_read_open(path, &release, err, sizeof(err));
    Disc disc = { 0 };
    Sndrv *d;
    Tap tap = { 0 };
    FILE *f;
    int16_t *buf;
    uint8_t h[44] = { 'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 2, 0,
                      0x80, 0xbb, 0, 0, 0, 0xee, 2, 0, 4, 0, 16, 0, 'd', 'a', 't', 'a', 0, 0, 0, 0 };
    int rc;

    if (!r) {
        fprintf(stderr, "ggx-snd: %s\n", err);
        return 2;
    }
    if (disc_path) {
        const Release *found;
        if (disc_image_open(&disc.image, disc_path, err, sizeof(err)) != 0 ||
            !(found = assets_identify(disc.image, disc_path, err, sizeof(err)))) {
            fprintf(stderr, "ggx-snd: %s\n", err);
            return 2;
        }
        if (strcmp(found->id, release == 1 ? "KR" : "JP") != 0) {
            fprintf(stderr, "ggx-snd: %s is %s; the stream is %s's\n", disc_path, found->id, release == 1 ? "KR" : "JP");
            return 2;
        }
    } else {
        if (!data_dir) {
            if (fs_pref_dir(pref, sizeof(pref)) != 0) {
                fprintf(stderr, "ggx-snd: no user data directory: pass --data-dir\n");
                return 2;
            }
            data_dir = pref;
        }
        if (asset_store_open(&disc.store, data_dir, release_by_id(release == 1 ? "KR" : "JP"), stderr) != 0) {
            return 2;
        }
    }
    if (tap_path && tap_open(&tap, tap_path) != 0) {
        fprintf(stderr, "ggx-snd: cannot write %s\n", tap_path);
        return 2;
    }
    if (!(f = fs_open(out, "wb")) || !(d = sndrv_create_attached(disc_reader, &disc, tap_attach, &tap)) ||
        !(buf = malloc(255u * 801u * 4))) {
        fprintf(stderr, "ggx-snd: cannot write %s\n", out);
        return 2;
    }
    sndrv_spu2(d)->mute[0] = mute[0];
    sndrv_spu2(d)->mute[1] = mute[1];
    fwrite(h, 1, sizeof(h), f);
    while ((rc = snd_read_next(r, &ev)) > 0) {
        if (ev.tag == 'C') {
            if (tap.events && !tap.marked) {
                /* the driver's first command: where ggx-spu2-compare starts comparing */
                fputc('K', tap.events);
                tap_u32(tap.events, sndrv_spu2(d)->cycles);
                tap.marked = 1;
            }
            sndrv_command(d, ev.cmd, ev.data, ev.size);
        } else if (ev.tag == 'R') {
            sndrv_game_read(d, ev.lsn, ev.sectors);
        } else if (ev.tag == 'F') {
            /* the same clock as audio_host.c: 800.8 samples a vblank */
            uint64_t end, n;
            vb += ev.vblanks;
            end = vb * 4004 / 5;
            n = end - frames;
            frames = end;
            while (n) {
                uint32_t k = n > 255u * 801u ? 255u * 801u : (uint32_t)n;
                sndrv_render(d, buf, k);
                fwrite(buf, 4, k, f);
                written += k;
                n -= k;
            }
        }
    }
    put32(h + 4, (uint32_t)(written * 4 + 36));
    put32(h + 40, (uint32_t)(written * 4));
    fseek(f, 0, SEEK_SET);
    fwrite(h, 1, sizeof(h), f);
    fclose(f);
    free(buf);
    tap_close(&tap, sndrv_spu2(d));
    sndrv_free(d);
    snd_read_close(r);
    disc_close(&disc);
    if (rc < 0) {
        fprintf(stderr, "ggx-snd: %s is cut short\n", path);
        return 1;
    }
    fprintf(stderr, "ggx-snd: %s: %.1f s\n", out, written / 48000.0);
    return 0;
}

int main(int argc, char **argv)
{
    const char *data_dir = NULL, *disc = NULL, *tap = NULL;
    uint32_t mute[2] = { 0, 0 };
    int i;

    if (argc >= 3 && !strcmp(argv[1], "dump")) {
        return dump(argv[2]);
    }
    if (argc < 4 || strcmp(argv[1], "render") != 0) {
        return usage();
    }
    for (i = 4; i < argc; i++) {
        if (i + 1 >= argc) {
            return usage();
        }
        if (!strcmp(argv[i], "--data-dir")) {
            data_dir = argv[++i];
        } else if (!strcmp(argv[i], "--tap")) {
            tap = argv[++i];
        } else if (!strcmp(argv[i], "--disc")) {
            disc = argv[++i];
        } else if (!strcmp(argv[i], "--mute")) {
            i++;
            if (!strcmp(argv[i], "bgm")) {
                mute[0] = 0xffffff;
            } else if (!strcmp(argv[i], "se")) {
                mute[1] = 0xffffff;
            } else {
                return usage();
            }
        } else if (!strcmp(argv[i], "--solo")) {
            int v = atoi(argv[++i]);
            if (v < 0 || v >= SPU2_VOICES) {
                return usage();
            }
            mute[0] = 0xffffff;
            mute[1] = 0xffffff & ~(1u << v);
        } else {
            return usage();
        }
    }
    return render(argv[2], argv[3], data_dir, disc, mute, tap);
}
