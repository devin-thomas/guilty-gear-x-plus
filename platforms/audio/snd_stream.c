#include "snd_stream.h"

#include <stdlib.h>
#include <string.h>

#include "fsys.h"

static const char MAGIC[8] = { 'G', 'G', 'X', 'S', 'N', 'D', 0, 0 };

const char *snd_cmd_name(uint32_t cmd)
{
    switch (cmd & 0x7f) {
    case SND_INIT: return "init";
    case SND_MVOL: return "mvolume";
    case SND_SE_STOP: return "se_stop";
    case SND_SE_ALLSTOP: return "se_allstop";
    case SND_SE_VOLUME: return "se_volume";
    case SND_SE_TRANS: return "se_trans";
    case SND_SE_AGAIN: return "se_again";
    case SND_SE_LOAD_PC: return "se_load_pc";
    case SND_SE_LOAD_CD: return "se_load_cd";
    case SND_SE_BANK: return "se_bank";
    case SND_BGM_LOAD_CD: return "bgm_load_cd";
    case SND_BGM_LOAD_PC: return "bgm_load_pc";
    case SND_BGM_START: return "bgm_start";
    case SND_BGM_STOP: return "bgm_stop";
    case SND_BGM_VOLUME: return "bgm_volume";
    case SND_BGM_KEEP: return "bgm_keep";
    case SND_BGM_STATUS: return "bgm_status";
    case SND_SE_STATUS: return "se_status";
    default: return NULL;
    }
}

static uint32_t arg(const uint8_t *data, uint32_t size, uint32_t i)
{
    uint32_t v = 0;

    if (data && size >= 4 * (i + 1)) {
        memcpy(&v, data + 4 * i, 4);
    }
    return v;
}

void snd_cmd_describe(uint32_t cmd, const uint8_t *data, uint32_t size, char *buf, size_t cap)
{
    const char *name = snd_cmd_name(cmd);
    const char *last = (cmd & SND_LAST) ? " (last)" : "";
    uint32_t a0 = arg(data, size, 0), a1 = arg(data, size, 1), a2 = arg(data, size, 2);

    if (!name) {
        snprintf(buf, cap, "0x%02x (ignored) %u bytes%s", cmd, size, last);
        return;
    }
    switch (cmd & 0x7f) {
    case SND_MVOL: snprintf(buf, cap, "%s 0x%x%s", name, a0, last); break;
    case SND_SE_STOP: snprintf(buf, cap, "%s voice %u%s", name, a0, last); break;
    case SND_SE_VOLUME: snprintf(buf, cap, "%s 0x%x 0x%x%s", name, a0, a1, last); break;
    case SND_SE_TRANS:
        snprintf(buf, cap, "%s voice %u, %u bytes from EE 0x%08x%s", name, a0, a1, a2, last);
        break;
    case SND_SE_AGAIN: snprintf(buf, cap, "%s voice %u%s", name, a0, last); break;
    case SND_SE_LOAD_CD: snprintf(buf, cap, "%s bank %u, lsn %u, %u bytes%s", name, a0, a1, a2, last); break;
    case SND_SE_BANK: snprintf(buf, cap, "%s voice %u, bank %u, effect %u%s", name, a0, a1, a2, last); break;
    case SND_BGM_LOAD_CD: snprintf(buf, cap, "%s lsn %u, %u sectors%s", name, a0, a1, last); break;
    case SND_BGM_START: snprintf(buf, cap, "%s loop %u%s", name, a0, last); break;
    case SND_BGM_VOLUME: snprintf(buf, cap, "%s 0x%x, %s%s", name, a0, a1 ? "mono" : "stereo", last); break;
    case SND_BGM_KEEP: snprintf(buf, cap, "%s loop %u, %u bytes%s", name, a0, a1, last); break;
    default: snprintf(buf, cap, "%s%s", name, last); break;
    }
}

struct SndLog {
    FILE *f;
    int err;
};

static void put(SndLog *l, const void *p, size_t n)
{
    if (n && fwrite(p, 1, n, l->f) != n) {
        l->err = 1;
    }
}

static void put32(SndLog *l, uint32_t v)
{
    uint8_t b[4] = { (uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24) };
    put(l, b, 4);
}

SndLog *snd_log_create(const char *path, uint32_t release)
{
    SndLog *l = calloc(1, sizeof(*l));

    if (!l || !(l->f = fs_open(path, "wb"))) {
        free(l);
        return NULL;
    }
    put(l, MAGIC, 8);
    put32(l, SND_STREAM_VERSION);
    put32(l, release);
    return l;
}

void snd_log_cmd(SndLog *l, uint32_t field, uint32_t cmd, const uint8_t *data, uint32_t size)
{
    put(l, "C", 1);
    put32(l, field);
    put32(l, cmd);
    put32(l, data ? size : 0);
    put(l, data, data ? size : 0);
}

void snd_log_disc(SndLog *l, uint32_t field, uint32_t lsn, uint32_t sectors)
{
    put(l, "R", 1);
    put32(l, field);
    put32(l, lsn);
    put32(l, sectors);
}

void snd_log_field(SndLog *l, uint32_t field, uint32_t vblanks)
{
    put(l, "F", 1);
    put32(l, field);
    put32(l, vblanks);
}

int snd_log_close(SndLog *l)
{
    int err;

    put(l, "E", 1);
    err = l->err | (fclose(l->f) != 0);
    free(l);
    return err ? -1 : 0;
}

struct SndReader {
    FILE *f;
    uint8_t *buf;
    uint32_t cap;
};

static int get32(FILE *f, uint32_t *v)
{
    uint8_t b[4];

    if (fread(b, 1, 4, f) != 4) {
        return -1;
    }
    *v = (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
    return 0;
}

SndReader *snd_read_open(const char *path, uint32_t *release, char *err, size_t cap)
{
    SndReader *r = calloc(1, sizeof(*r));
    char magic[8];
    uint32_t version;

    if (!r || !(r->f = fs_open(path, "rb"))) {
        snprintf(err, cap, "cannot read %s", path);
        free(r);
        return NULL;
    }
    if (fread(magic, 1, 8, r->f) != 8 || memcmp(magic, MAGIC, 8) != 0 || get32(r->f, &version) != 0 ||
        get32(r->f, release) != 0) {
        snprintf(err, cap, "%s is not a sound command stream", path);
        snd_read_close(r);
        return NULL;
    }
    if (version != SND_STREAM_VERSION) {
        snprintf(err, cap, "%s is stream version %u; this reads version %u", path, version, SND_STREAM_VERSION);
        snd_read_close(r);
        return NULL;
    }
    return r;
}

int snd_read_next(SndReader *r, SndEvent *ev)
{
    int tag = fgetc(r->f);

    memset(ev, 0, sizeof(*ev));
    if (tag == 'E') {
        ev->tag = 'E';
        return 0;
    }
    if (tag == 'F') {
        ev->tag = 'F';
        return get32(r->f, &ev->field) || get32(r->f, &ev->vblanks) ? -1 : 1;
    }
    if (tag == 'R') {
        ev->tag = 'R';
        return get32(r->f, &ev->field) || get32(r->f, &ev->lsn) || get32(r->f, &ev->sectors) ? -1 : 1;
    }
    if (tag != 'C' || get32(r->f, &ev->field) || get32(r->f, &ev->cmd) || get32(r->f, &ev->size)) {
        return -1;
    }
    ev->tag = 'C';
    if (ev->size > r->cap) {
        uint8_t *b = realloc(r->buf, ev->size);
        if (!b) {
            return -1;
        }
        r->buf = b;
        r->cap = ev->size;
    }
    if (ev->size && fread(r->buf, 1, ev->size, r->f) != ev->size) {
        return -1;
    }
    ev->data = ev->size ? r->buf : NULL;
    return 1;
}

void snd_read_close(SndReader *r)
{
    if (r) {
        if (r->f) {
            fclose(r->f);
        }
        free(r->buf);
        free(r);
    }
}
