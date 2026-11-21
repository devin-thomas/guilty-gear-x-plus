/* Replays (replays.h defines the format). */
#include "replays.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "mc_card.h"
#include "save_file.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

#define HEADER_SIZE 128u
#define RECORD_SIZE 15u
#define MAX_DIRS 16

typedef struct {
    char name[SAVE_NAME_MAX];
    uint16_t mode;
    uint32_t size;
    uint8_t *data;          /* NULL: not kept (the disc's), zeros on playback */
} RpFile;

typedef struct {
    char name[SAVE_NAME_MAX];
    uint16_t mode;
    int nfiles;
    RpFile files[SAVE_MAX_FILES];
} RpDir;

typedef struct {
    int present;
    int ndirs;
    RpDir dirs[MAX_DIRS];
} RpCard;

typedef struct {
    uint32_t field;
    uint64_t hash;
} RpCheck;

typedef struct {
    uint32_t field, kind;
} RpMark;

struct Replay {
    RpBuild build;
    int unlocks;
    RpCard card[2];
    SessionInput *in;
    uint32_t n, cap;
    RpCheck *checks;
    uint32_t nchecks, capc;
    RpMark *marks;
    int nmarks, capm;
    /* recording */
    Session *s;
    int broken;             /* a field ran out of order (a rollback): the record isn't the session's */
    int in_fight;
};

static const char *release_name(uint32_t r)
{
    return r == 1 ? "KR (SLPM-645.25)" : r == 2 ? "JP (SLPS-201.37)" : "an unknown release";
}

static void put16(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static void put64(uint8_t *p, uint64_t v)
{
    put32(p, (uint32_t)v);
    put32(p + 4, (uint32_t)(v >> 32));
}

static uint32_t get16(const uint8_t *p)
{
    return p[0] | (uint32_t)p[1] << 8;
}

static uint32_t get32(const uint8_t *p)
{
    return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static uint64_t get64(const uint8_t *p)
{
    return get32(p) | (uint64_t)get32(p + 4) << 32;
}

void rp_build_of(CoreHost *h, const char *const *extensions, int n, RpBuild *out)
{
    int i;

    memset(out, 0, sizeof(*out));
#if defined(VERSION_KR)
    out->release = 1;
#else
    out->release = 2;
#endif
    memcpy(out->elf_sha256, h->elf_sha256, 32);
    out->state_hash_version = CORE_STATE_HASH_VERSION;
    out->core_build = core_build_id(h);
    for (i = 0; i < n && i < RP_MAX_EXTENSIONS; i++) {
        snprintf(out->extensions[i], RP_EXT_ID_MAX, "%s", extensions[i]);
    }
    out->n_extensions = i;
}

/* ---- growing ---- */

static int grow(void **p, uint32_t *cap, uint32_t need, size_t size)
{
    uint32_t c = *cap ? *cap : 256;
    void *q;

    if (need <= *cap) {
        return 0;
    }
    while (c < need) {
        c *= 2;
    }
    if (!(q = realloc(*p, (size_t)c * size))) {
        return -1;
    }
    *p = q;
    *cap = c;
    return 0;
}

static int add_input(Replay *r, const SessionInput *in)
{
    if (grow((void **)&r->in, &r->cap, r->n + 1, sizeof(*r->in)) != 0) {
        return -1;
    }
    r->in[r->n++] = *in;
    return 0;
}

static int add_check(Replay *r, uint32_t field, uint64_t hash)
{
    if (grow((void **)&r->checks, &r->capc, r->nchecks + 1, sizeof(*r->checks)) != 0) {
        return -1;
    }
    r->checks[r->nchecks].field = field;
    r->checks[r->nchecks].hash = hash;
    r->nchecks++;
    return 0;
}

static int add_mark(Replay *r, uint32_t field, uint32_t kind)
{
    uint32_t cap = (uint32_t)r->capm;

    if (grow((void **)&r->marks, &cap, (uint32_t)r->nmarks + 1, sizeof(*r->marks)) != 0) {
        return -1;
    }
    r->capm = (int)cap;
    r->marks[r->nmarks].field = field;
    r->marks[r->nmarks].kind = kind;
    r->nmarks++;
    return 0;
}

/* ---- recording ---- */

static void on_input(void *ctx, uint32_t field, const SessionInput *in)
{
    Replay *r = ctx;

    if (field != r->n) {
        r->broken = 1;
        return;
    }
    if (field % RP_CHECK_EVERY == 0 && add_check(r, field, core_hash(r->s->h)) != 0) {
        r->broken = 1;
    }
    if (add_input(r, in) != 0) {
        r->broken = 1;
    }
}

/* What a card holds: the game's data files whole, the rest by name and size. */
static int take_card(RpCard *rc, CoreHost *h, int slot)
{
    McCard *c = h->card[slot];
    int i, j;

    rc->present = h->cards[slot] != 0;
    rc->ndirs = 0;
    if (!rc->present) {
        return 0;
    }
    for (i = 0; i < mc_card_count(c) && i < MAX_DIRS; i++) {
        const SaveDir *d = mc_card_at(c, i);
        RpDir *rd = &rc->dirs[rc->ndirs++];

        memset(rd, 0, sizeof(*rd));
        memcpy(rd->name, d->name, SAVE_NAME_MAX);
        rd->mode = d->mode;
        for (j = 0; j < d->nfiles; j++) {
            const SaveFile *f = &d->files[j];
            RpFile *rf = &rd->files[rd->nfiles++];

            memcpy(rf->name, f->name, SAVE_NAME_MAX);
            rf->mode = f->mode;
            rf->size = f->size;
            /* the game's own data file is named as its directory */
            if (!strcmp(f->name, d->name) && f->size) {
                if (!(rf->data = malloc(f->size))) {
                    return -1;
                }
                memcpy(rf->data, f->data, f->size);
            }
        }
    }
    return 0;
}

Replay *rp_record(Session *s, int unlocks, const char *const *extensions, int n_extensions)
{
    Replay *r = calloc(1, sizeof(*r));
    int i;

    if (!r) {
        return NULL;
    }
    rp_build_of(s->h, extensions, n_extensions, &r->build);
    r->unlocks = unlocks;
    r->s = s;
    for (i = 0; i < 2; i++) {
        if (take_card(&r->card[i], s->h, i) != 0) {
            rp_free(r);
            return NULL;
        }
    }
    if (s->h->field != 0) {
        r->broken = 1;
    }
    s->on_input = on_input;
    s->on_input_ctx = r;
    return r;
}

void rp_mark(Replay *r, uint32_t kind)
{
    if (r && add_mark(r, r->n, kind) != 0) {
        r->broken = 1;
    }
}

int rp_watch(Replay *r, CoreHost *h)
{
    int fight;

    if (!r) {
        return 0;
    }
    fight = !strcmp(ggx_func_name(ee_r32(h, ggx_sym_jobfunc + 4 * ee_r32(h, ggx_sym_JobMode))), "GameMain");
    if (fight == r->in_fight) {
        return 0;
    }
    r->in_fight = fight;
    rp_mark(r, fight ? RP_MARK_FIGHT : RP_MARK_FIGHT_END);
    return fight ? RP_MARK_FIGHT : RP_MARK_FIGHT_END;
}

/* ---- the file ---- */

typedef struct {
    uint8_t *b;
    size_t n, cap;
    int err;
} Buf;

static void out(Buf *o, const void *p, size_t n)
{
    if (o->err) {
        return;
    }
    if (o->n + n > o->cap) {
        size_t cap = o->cap ? o->cap : 4096;
        uint8_t *b;

        while (cap < o->n + n) {
            cap *= 2;
        }
        if (!(b = realloc(o->b, cap))) {
            o->err = 1;
            return;
        }
        o->b = b;
        o->cap = cap;
    }
    if (p) {
        memcpy(o->b + o->n, p, n);
    } else {
        memset(o->b + o->n, 0, n);
    }
    o->n += n;
}

static void out32(Buf *o, uint32_t v)
{
    uint8_t b[4];

    put32(b, v);
    out(o, b, 4);
}

static void out_pad(Buf *o)
{
    out(o, NULL, (4 - o->n % 4) % 4);
}

static void out_varint(Buf *o, uint32_t v)
{
    uint8_t b[5];
    int n = 0;

    do {
        b[n] = (uint8_t)(v & 0x7f);
        v >>= 7;
        if (v) {
            b[n] |= 0x80;
        }
        n++;
    } while (v);
    out(o, b, (size_t)n);
}

static void record_bytes(const SessionInput *in, uint8_t b[RECORD_SIZE])
{
    int p;

    for (p = 0; p < 2; p++) {
        put16(b + 7 * p, in->pads[p].buttons);
        memcpy(b + 7 * p + 2, in->pads[p].analog, 4);
        b[7 * p + 6] = in->pads[p].mode;
    }
    b[14] = in->unlock;
}

static void record_of(const uint8_t b[RECORD_SIZE], SessionInput *in)
{
    int p;

    for (p = 0; p < 2; p++) {
        in->pads[p].buttons = (uint16_t)get16(b + 7 * p);
        memcpy(in->pads[p].analog, b + 7 * p + 2, 4);
        in->pads[p].mode = b[7 * p + 6];
    }
    in->unlock = b[14];
}

static int encode(const Replay *r, uint64_t final_hash, int with_final, uint8_t **outp, size_t *len)
{
    Buf o = { 0 };
    uint8_t h[HEADER_SIZE];
    uint32_t nchecks = r->nchecks + (with_final ? 1 : 0), i, start;
    int k, s, d, f;

    memset(h, 0, sizeof(h));
    memcpy(h, "GGXRPLAY", 8);
    put32(h + 8, RP_FORMAT_VERSION);
    put32(h + 12, r->build.release);
    put32(h + 16, HEADER_SIZE);
    put32(h + 20, r->build.state_hash_version);
    memcpy(h + 24, r->build.elf_sha256, 32);
    put64(h + 56, r->build.core_build);
    put32(h + 64, (uint32_t)r->unlocks);
    put32(h + 68, r->n);
    put32(h + 72, (uint32_t)r->build.n_extensions);
    put32(h + 76, nchecks);
    put32(h + 80, (uint32_t)r->nmarks);
    put32(h + 84, RP_CHECK_EVERY);
    out(&o, h, sizeof(h));
    for (k = 0; k < r->build.n_extensions; k++) {
        uint32_t n = (uint32_t)strlen(r->build.extensions[k]);

        out32(&o, n);
        out(&o, r->build.extensions[k], n);
        out_pad(&o);
    }
    for (s = 0; s < 2; s++) {
        const RpCard *c = &r->card[s];

        out32(&o, (uint32_t)c->present);
        out32(&o, (uint32_t)c->ndirs);
        for (d = 0; d < c->ndirs; d++) {
            const RpDir *rd = &c->dirs[d];
            uint8_t b[4];

            out(&o, rd->name, SAVE_NAME_MAX);
            put16(b, rd->mode);
            put16(b + 2, 0);
            out(&o, b, 4);
            out32(&o, (uint32_t)rd->nfiles);
            for (f = 0; f < rd->nfiles; f++) {
                const RpFile *rf = &rd->files[f];

                out(&o, rf->name, SAVE_NAME_MAX);
                put16(b, rf->mode);
                put16(b + 2, rf->data ? 1 : 0);
                out(&o, b, 4);
                out32(&o, rf->size);
                if (rf->data) {
                    out(&o, rf->data, rf->size);
                    out_pad(&o);
                }
            }
        }
    }
    for (k = 0; k < r->nmarks; k++) {
        out32(&o, r->marks[k].field);
        out32(&o, r->marks[k].kind);
    }
    for (i = 0; i < nchecks; i++) {
        uint8_t b[12];

        if (i < r->nchecks) {
            put32(b, r->checks[i].field);
            put64(b + 4, r->checks[i].hash);
        } else {
            put32(b, r->n);
            put64(b + 4, final_hash);
        }
        out(&o, b, 12);
    }
    /* the inputs: runs of the same record */
    out32(&o, 0);
    start = (uint32_t)o.n;
    for (i = 0; i < r->n;) {
        uint32_t j = i + 1;
        uint8_t b[RECORD_SIZE];

        while (j < r->n && session_input_equal(&r->in[j], &r->in[i])) {
            j++;
        }
        out_varint(&o, j - i);
        record_bytes(&r->in[i], b);
        out(&o, b, RECORD_SIZE);
        i = j;
    }
    if (!o.err) {
        put32(o.b + start - 4, (uint32_t)(o.n - start));
    }
    out_pad(&o);
    {
        uint8_t b[8];

        put64(b, o.err ? 0 : XXH3_64bits(o.b, o.n));
        out(&o, b, 8);
    }
    if (o.err) {
        free(o.b);
        return -1;
    }
    *outp = o.b;
    *len = o.n;
    return 0;
}

int rp_encode(const Replay *r, uint8_t **out, size_t *len)
{
    return encode(r, 0, 0, out, len);
}

int rp_write(Replay *r, const char *path, char *why, size_t cap)
{
    uint8_t *b;
    size_t n;
    char tmp[1100];
    FILE *f;
    int ok;
    uint64_t now = core_hash(r->s->h);
    /* the last check: after the last field (unless one is there already) */
    int with_final = !(r->nchecks && r->checks[r->nchecks - 1].field == r->n);

    if (r->broken) {
        snprintf(why, cap, "the session's fields didn't run in order from the boot, so the record isn't the session");
        return -1;
    }
    if (encode(r, now, with_final, &b, &n) != 0) {
        snprintf(why, cap, "out of memory");
        return -1;
    }
    if (snprintf(tmp, sizeof(tmp), "%s.tmp", path) >= (int)sizeof(tmp)) {
        free(b);
        snprintf(why, cap, "the path is too long");
        return -1;
    }
    if (!(f = fs_open(tmp, "wb"))) {
        free(b);
        snprintf(why, cap, "cannot write %s", tmp);
        return -1;
    }
    ok = fwrite(b, 1, n, f) == n;
    ok &= fclose(f) == 0;
    free(b);
    if (!ok || fs_replace(tmp, path) != 0) {
        fs_remove(tmp);
        snprintf(why, cap, "cannot write %s", path);
        return -1;
    }
    return 0;
}

/* ---- reading ---- */

typedef struct {
    const uint8_t *b;
    size_t n, o;
    int err;
} In;

static const uint8_t *take(In *in, size_t n)
{
    const uint8_t *p;

    if (in->err || n > in->n - in->o) {
        in->err = 1;
        return NULL;
    }
    p = in->b + in->o;
    in->o += n;
    return p;
}

static uint32_t take32(In *in)
{
    const uint8_t *p = take(in, 4);

    return p ? get32(p) : 0;
}

static void take_pad(In *in)
{
    take(in, (4 - in->o % 4) % 4);
}

static uint32_t take_varint(In *in)
{
    uint32_t v = 0;
    int shift = 0;

    for (;;) {
        const uint8_t *p = take(in, 1);

        if (!p || shift > 28) {
            in->err = 1;
            return 0;
        }
        v |= (uint32_t)(*p & 0x7f) << shift;
        if (!(*p & 0x80)) {
            return v;
        }
        shift += 7;
    }
}

Replay *rp_decode(const uint8_t *b, size_t len, char *why, size_t cap)
{
    In in = { b, len, 0, 0 };
    Replay *r;
    uint32_t version, hsize, nchecks, nmarks, next, i, bytes;
    const uint8_t *h;
    int s, d, f;

    if (len < HEADER_SIZE + 8 || memcmp(b, "GGXRPLAY", 8) != 0) {
        snprintf(why, cap, "it isn't a replay file");
        return NULL;
    }
    version = get32(b + 8);
    if (version != RP_FORMAT_VERSION) {
        snprintf(why, cap, "it is replay format version %u, and this build reads version %u", version,
                 RP_FORMAT_VERSION);
        return NULL;
    }
    if (XXH3_64bits(b, len - 8) != get64(b + len - 8)) {
        snprintf(why, cap, "its hash doesn't match its contents (the file is damaged)");
        return NULL;
    }
    if (!(r = calloc(1, sizeof(*r)))) {
        snprintf(why, cap, "out of memory");
        return NULL;
    }
    h = take(&in, HEADER_SIZE);
    hsize = get32(h + 16);
    r->build.release = get32(h + 12);
    r->build.state_hash_version = get32(h + 20);
    memcpy(r->build.elf_sha256, h + 24, 32);
    r->build.core_build = get64(h + 56);
    r->unlocks = (int)get32(h + 64);
    next = get32(h + 72);
    nchecks = get32(h + 76);
    nmarks = get32(h + 80);
    if (hsize < HEADER_SIZE || hsize > len || next > RP_MAX_EXTENSIONS) {
        in.err = 1;
    }
    in.o = hsize;
    for (i = 0; i < next && !in.err; i++) {
        uint32_t n = take32(&in);
        const uint8_t *p = take(&in, n);

        if (p && n < RP_EXT_ID_MAX) {
            memcpy(r->build.extensions[i], p, n);
            r->build.extensions[i][n] = 0;
        } else {
            in.err = 1;
        }
        take_pad(&in);
    }
    r->build.n_extensions = (int)next;
    for (s = 0; s < 2 && !in.err; s++) {
        RpCard *c = &r->card[s];

        c->present = take32(&in) != 0;
        c->ndirs = (int)take32(&in);
        if (c->ndirs < 0 || c->ndirs > MAX_DIRS) {
            in.err = 1;
            break;
        }
        for (d = 0; d < c->ndirs && !in.err; d++) {
            RpDir *rd = &c->dirs[d];
            const uint8_t *p = take(&in, SAVE_NAME_MAX + 4);

            if (!p) {
                break;
            }
            memcpy(rd->name, p, SAVE_NAME_MAX);
            rd->name[SAVE_NAME_MAX - 1] = 0;
            rd->mode = (uint16_t)get16(p + SAVE_NAME_MAX);
            rd->nfiles = (int)take32(&in);
            if (rd->nfiles < 0 || rd->nfiles > SAVE_MAX_FILES) {
                in.err = 1;
                break;
            }
            for (f = 0; f < rd->nfiles && !in.err; f++) {
                RpFile *rf = &rd->files[f];
                const uint8_t *q = take(&in, SAVE_NAME_MAX + 4);

                if (!q) {
                    break;
                }
                memcpy(rf->name, q, SAVE_NAME_MAX);
                rf->name[SAVE_NAME_MAX - 1] = 0;
                rf->mode = (uint16_t)get16(q + SAVE_NAME_MAX);
                rf->size = take32(&in);
                if (get16(q + SAVE_NAME_MAX + 2) & 1) {
                    const uint8_t *data = take(&in, rf->size);

                    if (data && rf->size && (rf->data = malloc(rf->size))) {
                        memcpy(rf->data, data, rf->size);
                    } else if (rf->size) {
                        in.err = 1;
                    }
                    take_pad(&in);
                }
            }
        }
    }
    for (i = 0; i < nmarks && !in.err; i++) {
        uint32_t field = take32(&in), kind = take32(&in);

        if (!in.err && add_mark(r, field, kind) != 0) {
            in.err = 1;
        }
    }
    for (i = 0; i < nchecks && !in.err; i++) {
        const uint8_t *p = take(&in, 12);

        if (p && add_check(r, get32(p), get64(p + 4)) != 0) {
            in.err = 1;
        }
    }
    bytes = take32(&in);
    if (!in.err) {
        size_t end = in.o + bytes;

        if (bytes > len - in.o) {
            in.err = 1;
        }
        while (!in.err && in.o < end) {
            uint32_t run = take_varint(&in), k;
            const uint8_t *p = take(&in, RECORD_SIZE);
            SessionInput rec;

            if (!p || !run || r->n + (uint64_t)run > get32(h + 68)) {
                in.err = 1;
                break;
            }
            memset(&rec, 0, sizeof(rec));
            record_of(p, &rec);
            for (k = 0; k < run && !in.err; k++) {
                if (add_input(r, &rec) != 0) {
                    in.err = 1;
                }
            }
        }
    }
    if (!in.err && r->n != get32(h + 68)) {
        in.err = 1;
    }
    if (in.err) {
        rp_free(r);
        snprintf(why, cap, "it is cut short or malformed");
        return NULL;
    }
    return r;
}

Replay *rp_open(const char *path, char *why, size_t cap)
{
    FILE *f = fs_open(path, "rb");
    uint8_t *b;
    long n;
    Replay *r;

    if (!f) {
        snprintf(why, cap, "cannot read %s", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0 || !(b = malloc((size_t)n + 1)) || fread(b, 1, (size_t)n, f) != (size_t)n) {
        fclose(f);
        snprintf(why, cap, "cannot read %s", path);
        return NULL;
    }
    fclose(f);
    r = rp_decode(b, (size_t)n, why, cap);
    free(b);
    return r;
}

static void hex(const uint8_t *p, int n, char *out)
{
    int i;

    for (i = 0; i < n; i++) {
        snprintf(out + 2 * i, 3, "%02x", p[i]);
    }
}

int rp_compatible(const Replay *r, const RpBuild *b, char *why, size_t cap)
{
    const RpBuild *rb = &r->build;
    int i, j;

    if (rb->release != b->release) {
        snprintf(why, cap, "it was recorded on the %s release, and this build of the port runs %s: play it with a "
                           "build made with -DGGX_VERSION=%s", release_name(rb->release), release_name(b->release),
                 rb->release == 1 ? "KR" : "JP");
        return -1;
    }
    if (memcmp(rb->elf_sha256, b->elf_sha256, 32) != 0) {
        char a[65], c[65];

        hex(rb->elf_sha256, 32, a);
        hex(b->elf_sha256, 32, c);
        snprintf(why, cap, "it was recorded with another boot ELF (SHA-256 %.16s...) than this disc's (%.16s...)", a, c);
        return -1;
    }
    if (rb->state_hash_version != b->state_hash_version) {
        snprintf(why, cap, "its checks are State Hash version %u, and this build computes version %u",
                 rb->state_hash_version, b->state_hash_version);
        return -1;
    }
    if (rb->core_build != b->core_build) {
        snprintf(why, cap, "it was recorded with another build of the port's Core (Core build %016llx; this one is "
                           "%016llx), which may compute other fields from the same inputs: a replay plays only on "
                           "the Core it was recorded with", (unsigned long long)rb->core_build,
                 (unsigned long long)b->core_build);
        return -1;
    }
    for (i = 0; i < rb->n_extensions; i++) {
        for (j = 0; j < b->n_extensions && strcmp(rb->extensions[i], b->extensions[j]); j++) {
        }
        if (j == b->n_extensions) {
            snprintf(why, cap, "it was recorded with the extension \"%s\", which this run doesn't have active",
                     rb->extensions[i]);
            return -1;
        }
    }
    for (j = 0; j < b->n_extensions; j++) {
        for (i = 0; i < rb->n_extensions && strcmp(rb->extensions[i], b->extensions[j]); i++) {
        }
        if (i == rb->n_extensions) {
            snprintf(why, cap, "this run has the extension \"%s\" active, and the replay was recorded without it",
                     b->extensions[j]);
            return -1;
        }
    }
    return 0;
}

void rp_host_config(const Replay *r, int cards[2], const char *ext_ids[RP_MAX_EXTENSIONS], int *n_ext)
{
    int i;

    for (i = 0; i < 2; i++) {
        cards[i] = r->card[i].present ? CORE_CARD_EMPTY : CORE_CARD_NONE;
    }
    for (i = 0; i < r->build.n_extensions; i++) {
        ext_ids[i] = r->build.extensions[i];
    }
    *n_ext = r->build.n_extensions;
}

int rp_start(Replay *r, Session *s)
{
    int i, d, f;

    session_init(s, s->h, r->unlocks);
    session_detach_cards(s);
    for (i = 0; i < 2; i++) {
        McCard *c = s->h->card[i];

        for (d = 0; d < r->card[i].ndirs; d++) {
            const RpDir *rd = &r->card[i].dirs[d];
            int di;

            if (mc_card_mkdir(c, rd->name) != MC_RES_OK || (di = mc_card_find(c, rd->name)) < 0) {
                return -1;
            }
            for (f = 0; f < rd->nfiles; f++) {
                const RpFile *rf = &rd->files[f];
                uint8_t *zeros = NULL;
                int fi, rc;

                if (mc_card_create(c, di, rf->name) != MC_RES_OK ||
                    (fi = save_dir_find(mc_card_at(c, di), rf->name)) < 0) {
                    return -1;
                }
                if (!rf->size) {
                    continue;
                }
                if (!rf->data && !(zeros = calloc(1, rf->size))) {
                    return -1;
                }
                rc = mc_card_write(c, di, fi, 0, rf->data ? rf->data : zeros, rf->size);
                free(zeros);
                if (rc != MC_RES_OK) {
                    return -1;
                }
            }
        }
    }
    return 0;
}

const SessionInput *rp_input(const Replay *r, uint32_t f)
{
    return f < r->n ? &r->in[f] : NULL;
}

uint32_t rp_fields(const Replay *r)
{
    return r->n;
}

int rp_check(const Replay *r, uint32_t f, uint64_t state_hash, char *why, size_t cap)
{
    uint32_t i;

    for (i = 0; i < r->nchecks; i++) {
        if (r->checks[i].field == f) {
            if (r->checks[i].hash == state_hash) {
                return 0;
            }
            snprintf(why, cap, "%s field %u, the State Hash is %016llx; recorded, it was %016llx",
                     f == r->n ? "after the last field," : "before", f, (unsigned long long)state_hash,
                     (unsigned long long)r->checks[i].hash);
            return -1;
        }
    }
    return 0;
}

int rp_marks(const Replay *r)
{
    return r->nmarks;
}

void rp_mark_at(const Replay *r, int i, uint32_t *field, uint32_t *kind)
{
    *field = r->marks[i].field;
    *kind = r->marks[i].kind;
}

const RpBuild *rp_build(const Replay *r)
{
    return &r->build;
}

int rp_unlocks(const Replay *r)
{
    return r->unlocks;
}

int rp_card(const Replay *r, int slot, int *ndirs)
{
    *ndirs = r->card[slot].ndirs;
    return r->card[slot].present;
}

const char *rp_card_dir(const Replay *r, int slot, int dir, int *nfiles)
{
    *nfiles = r->card[slot].dirs[dir].nfiles;
    return r->card[slot].dirs[dir].name;
}

void rp_free(Replay *r)
{
    int s, d, f;

    if (!r) {
        return;
    }
    if (r->s && r->s->on_input_ctx == r) {
        r->s->on_input = NULL;
        r->s->on_input_ctx = NULL;
    }
    for (s = 0; s < 2; s++) {
        for (d = 0; d < r->card[s].ndirs; d++) {
            for (f = 0; f < r->card[s].dirs[d].nfiles; f++) {
                free(r->card[s].dirs[d].files[f].data);
            }
        }
    }
    free(r->in);
    free(r->checks);
    free(r->marks);
    free(r);
}

Replay *rp_synthetic(const RpBuild *build, uint32_t fields)
{
    Replay *r = calloc(1, sizeof(*r));
    uint32_t i;

    if (!r) {
        return NULL;
    }
    r->build = *build;
    r->card[0].present = 1;
    {
        /* a system directory: the game's data file whole, the icon by its size */
        RpDir *d = &r->card[0].dirs[r->card[0].ndirs++];

        snprintf(d->name, sizeof(d->name), "%s", "BKSLPM-64525GGX");
        d->mode = SAVE_MODE_DIR;
        d->nfiles = 2;
        memcpy(d->files[0].name, d->name, SAVE_NAME_MAX);
        d->files[0].mode = SAVE_MODE_FILE;
        d->files[0].size = 0x1615;
        if (!(d->files[0].data = malloc(0x1615))) {
            rp_free(r);
            return NULL;
        }
        for (i = 0; i < 0x1615; i++) {
            d->files[0].data[i] = (uint8_t)(i * 7);
        }
        snprintf(d->files[1].name, sizeof(d->files[1].name), "%s", "icon.bin");
        d->files[1].mode = SAVE_MODE_FILE;
        d->files[1].size = 0xF1D8;
    }
    for (i = 0; i < fields; i++) {
        SessionInput in;

        memset(&in, 0, sizeof(in));
        pad_clear(&in.pads[0]);
        pad_clear(&in.pads[1]);
        in.pads[0].buttons = (uint16_t)((i / 7) * 0x1111u);
        in.pads[1].mode = (uint8_t)(i / 100 % 2);
        if (add_input(r, &in) != 0 || (i % RP_CHECK_EVERY == 0 && add_check(r, i, 0x1234567890abcdefull ^ i) != 0)) {
            rp_free(r);
            return NULL;
        }
    }
    add_mark(r, fields / 2, RP_MARK_FIGHT);
    return r;
}

int rp_dir(char *buf, size_t cap, const char *data_dir)
{
    char pref[1024];
    size_t n;

    if (!data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            return -1;
        }
        data_dir = pref;
    }
    n = strlen(data_dir);
    return fs_join(buf, cap, data_dir, n && (data_dir[n - 1] == '/' || data_dir[n - 1] == '\\') ? "replays" : "/replays",
                   NULL);
}
