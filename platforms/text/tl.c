/* The text layer (tl.h): translation files, finding their pictures on the
 * disc, and the substitution the renderer asks for. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dl_tex.h"
#include "fsys.h"
#include "tl_internal.h"

#define SECTOR 2048u
#define MAX_INCLUDE 8

struct TextLayer {
    int release;
    TlEntry *e;
    uint32_t n, cap;
    int32_t *map;            /* open addressing on the upload hash, -1 empty */
    uint32_t map_cap;
    TlFonts *fonts;
    int default_font;
};

typedef struct {
    TextLayer *t;
    char *err;
    size_t cap;
    int lang_seen;
    /* the group settings */
    unsigned releases;
    uint32_t fid;
    int have_fid, font, rows, align, size;
    int area[4];
    int depth;
} Parse;

static void fail(char *err, size_t cap, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(err, cap, fmt, ap);
    va_end(ap);
}

/* Appends one line to err (bind's report). */
static void note(char *err, size_t cap, const char *fmt, ...)
{
    size_t len = strlen(err);
    va_list ap;

    if (len + 2 >= cap) {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(err + len, cap - len, fmt, ap);
    va_end(ap);
    len = strlen(err);
    if (len + 1 < cap) {
        err[len] = '\n';
        err[len + 1] = 0;
    }
}

static char *read_text(const char *path)
{
    FILE *f = fs_open(path, "rb");
    int64_t n = f ? fs_size(f) : -1;
    char *buf = n >= 0 ? malloc((size_t)n + 1) : NULL;

    if (buf && (fs_seek(f, 0) != 0 || fread(buf, 1, (size_t)n, f) != (size_t)n)) {
        free(buf);
        buf = NULL;
    }
    if (buf) {
        buf[n] = 0;
    }
    if (f) {
        fclose(f);
    }
    return buf;
}

static char *trim(char *s)
{
    char *end;

    while (*s == ' ' || *s == '\t') {
        s++;
    }
    end = s + strlen(s);
    while (end > s && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r')) {
        *--end = 0;
    }
    return s;
}

/* "019-003": up to 6 levels of three digits. */
static int valid_path(const char *p)
{
    int n = 0;

    for (;;) {
        if (!(p[0] >= '0' && p[0] <= '9' && p[1] >= '0' && p[1] <= '9' && p[2] >= '0' && p[2] <= '9')) {
            return 0;
        }
        p += 3;
        if (++n > 6) {
            return 0;
        }
        if (!*p) {
            return 1;
        }
        if (*p++ != '-') {
            return 0;
        }
    }
}

static int add_entry(Parse *ps, const char *where, char *id, char *text)
{
    TextLayer *t = ps->t;
    TlEntry *e;
    char *slash;
    int review = 0;

    if (*id == '?') {
        review = 1;
        id++;
    }
    slash = strchr(id, '/');
    if (!ps->releases) {
        fail(ps->err, ps->cap, "%s: an entry before any group's `releases`", where);
        return -1;
    }
    if (!(ps->releases & (unsigned)t->release)) {
        return 0;
    }
    if (t->n == t->cap) {
        TlEntry *ne;

        t->cap = t->cap ? t->cap * 2 : 256;
        ne = realloc(t->e, sizeof(TlEntry) * t->cap);
        if (!ne) {
            fail(ps->err, ps->cap, "out of memory");
            return -1;
        }
        t->e = ne;
    }
    e = &t->e[t->n];
    memset(e, 0, sizeof(*e));
    if (slash) {
        *slash = 0;
        e->fid = (uint32_t)strtoul(id, NULL, 10);
        id = slash + 1;
    } else if (ps->have_fid) {
        e->fid = ps->fid;
    } else {
        fail(ps->err, ps->cap, "%s: no `file` for this group, so the id needs one (<fid>/<path>)", where);
        return -1;
    }
    if (!valid_path(id) || strlen(id) >= sizeof(e->path)) {
        fail(ps->err, ps->cap, "%s: `%s` is not a picture path (like 000 or 019-003)", where, id);
        return -1;
    }
    strcpy(e->path, id);
    e->font = ps->font;
    e->rows = ps->rows;
    e->align = ps->align;
    e->size = ps->size;
    memcpy(e->area, ps->area, sizeof(e->area));
    e->review = review;
    e->text = malloc(strlen(text) + 1);
    if (!e->text) {
        fail(ps->err, ps->cap, "out of memory");
        return -1;
    }
    strcpy(e->text, text);
    t->n++;
    return 0;
}

static int parse_file(Parse *ps, const char *path);

static int statement(Parse *ps, const char *path, const char *where, char *line)
{
    char *arg, *sp;

    if (*line == '[') {
        if (line[strlen(line) - 1] != ']') {
            fail(ps->err, ps->cap, "%s: a group is `[name]`", where);
            return -1;
        }
        ps->releases = 0;
        ps->have_fid = 0;
        ps->font = TL_FONT_DEFAULT;
        ps->rows = 0;
        ps->align = TL_ALIGN_AUTO;
        ps->size = 0;
        ps->area[2] = -1;
        return 0;
    }
    if ((*line >= '0' && *line <= '9') || *line == '?') {
        sp = strchr(line, ' ');
        if (!sp) {
            fail(ps->err, ps->cap, "%s: an entry with no English", where);
            return -1;
        }
        *sp = 0;
        return add_entry(ps, where, line, trim(sp + 1));
    }
    sp = strchr(line, ' ');
    arg = sp ? trim(sp + 1) : line + strlen(line);
    if (sp) {
        *sp = 0;
    }
    if (!strcmp(line, "lang")) {
        if (strcmp(arg, "en")) {
            fail(ps->err, ps->cap, "%s: only `lang en` is known", where);
            return -1;
        }
        ps->lang_seen = 1;
    } else if (!strcmp(line, "include")) {
        char inc[1024];
        const char *slash = strrchr(path, '/'), *bslash = strrchr(path, '\\');
        size_t dir;

        if (bslash > slash) {
            slash = bslash;
        }
        dir = slash ? (size_t)(slash - path + 1) : 0;
        if (dir + strlen(arg) >= sizeof(inc)) {
            fail(ps->err, ps->cap, "%s: path too long", where);
            return -1;
        }
        Parse outer = *ps;
        int rc;

        memcpy(inc, path, dir);
        strcpy(inc + dir, arg);
        /* an included file starts with no group, and its groups end with it */
        ps->releases = 0;
        ps->have_fid = 0;
        ps->area[2] = -1;
        ps->depth++;
        rc = parse_file(ps, inc);
        outer.lang_seen = ps->lang_seen;
        *ps = outer;
        return rc;
    } else if (!strcmp(line, "releases")) {
        char *tok = strtok(arg, " ");

        ps->releases = 0;
        for (; tok; tok = strtok(NULL, " ")) {
            if (!strcmp(tok, "KR")) {
                ps->releases |= TL_KR;
            } else if (!strcmp(tok, "JP")) {
                ps->releases |= TL_JP;
            } else {
                fail(ps->err, ps->cap, "%s: releases are KR and JP", where);
                return -1;
            }
        }
    } else if (!strcmp(line, "file")) {
        ps->fid = (uint32_t)strtoul(arg, NULL, 10);
        ps->have_fid = 1;
    } else if (!strcmp(line, "font")) {
        if (!strcmp(arg, "serif")) {
            ps->font = TL_FONT_SERIF;
        } else if (!strcmp(arg, "sans")) {
            ps->font = TL_FONT_SANS;
        } else if (!strcmp(arg, "serif-bold")) {
            ps->font = TL_FONT_SERIF | TL_BOLD;
        } else if (!strcmp(arg, "sans-bold")) {
            ps->font = TL_FONT_SANS | TL_BOLD;
        } else {
            fail(ps->err, ps->cap, "%s: fonts are serif, sans, serif-bold and sans-bold", where);
            return -1;
        }
    } else if (!strcmp(line, "layout")) {
        if (!strcmp(arg, "block")) {
            ps->rows = 0;
        } else if (!strncmp(arg, "rows ", 5) && atoi(arg + 5) > 0) {
            ps->rows = atoi(arg + 5);
        } else {
            fail(ps->err, ps->cap, "%s: layouts are `block` and `rows <pitch>`", where);
            return -1;
        }
    } else if (!strcmp(line, "align")) {
        if (!strcmp(arg, "auto")) {
            ps->align = TL_ALIGN_AUTO;
        } else if (!strcmp(arg, "left")) {
            ps->align = TL_ALIGN_LEFT;
        } else if (!strcmp(arg, "center")) {
            ps->align = TL_ALIGN_CENTER;
        } else {
            fail(ps->err, ps->cap, "%s: align is auto, left or center", where);
            return -1;
        }
    } else if (!strcmp(line, "size")) {
        ps->size = atoi(arg);
    } else if (!strcmp(line, "area")) {
        int *a = ps->area;

        if (sscanf(arg, "%d %d %d %d", &a[0], &a[1], &a[2], &a[3]) != 4 || a[0] < 0 || a[1] < 0 || a[2] < a[0] ||
            a[3] < a[1]) {
            fail(ps->err, ps->cap, "%s: area is x0 y0 x1 y1, inclusive", where);
            return -1;
        }
    } else {
        fail(ps->err, ps->cap, "%s: unknown statement `%s`", where, line);
        return -1;
    }
    return 0;
}

static int parse_file(Parse *ps, const char *path)
{
    char *buf, *line, *next, where[1100];
    int lineno = 0, have_version = 0, rc = 0;

    if (ps->depth > MAX_INCLUDE) {
        fail(ps->err, ps->cap, "%s: includes nest too deep", path);
        return -1;
    }
    buf = read_text(path);
    if (!buf) {
        fail(ps->err, ps->cap, "cannot read %s", path);
        return -1;
    }
    for (line = buf; line && rc == 0; line = next) {
        char *s;

        next = strchr(line, '\n');
        if (next) {
            *next++ = 0;
        }
        lineno++;
        s = trim(line);
        if (lineno == 1 && (uint8_t)s[0] == 0xEF && (uint8_t)s[1] == 0xBB && (uint8_t)s[2] == 0xBF) {
            s += 3;   /* a UTF-8 byte order mark */
        }
        if (!*s || *s == '#') {
            continue;
        }
        snprintf(where, sizeof(where), "%s:%d", path, lineno);
        if (!have_version) {
            if (strcmp(s, "ggx-lang 1")) {
                fail(ps->err, ps->cap, "%s: not a ggx-lang 1 file", where);
                rc = -1;
            }
            have_version = 1;
            continue;
        }
        rc = statement(ps, path, where, s);
    }
    free(buf);
    return rc;
}

TextLayer *tl_open(int release, const char *lang_path, const char *font_dir, char *err, size_t cap)
{
    TextLayer *t = calloc(1, sizeof(TextLayer));
    Parse ps;

    if (!t) {
        fail(err, cap, "out of memory");
        return NULL;
    }
    t->release = release;
    /* each disc's own lettering: the Korean gothic is set heavy */
    t->default_font = release == TL_KR ? TL_FONT_SANS | TL_BOLD : TL_FONT_SERIF;
    memset(&ps, 0, sizeof(ps));
    ps.t = t;
    ps.err = err;
    ps.cap = cap;
    ps.font = TL_FONT_DEFAULT;
    ps.area[2] = -1;
    if (parse_file(&ps, lang_path) != 0 || !(t->fonts = tl_fonts_load(font_dir, err, cap))) {
        tl_free(t);
        return NULL;
    }
    if (!ps.lang_seen) {
        fail(err, cap, "%s: no `lang` statement", lang_path);
        tl_free(t);
        return NULL;
    }
    return t;
}

void tl_free(TextLayer *t)
{
    uint32_t i;

    if (!t) {
        return;
    }
    for (i = 0; i < t->n; i++) {
        free(t->e[i].text);
        free(t->e[i].orig);
        free(t->e[i].english);
    }
    free(t->e);
    free(t->map);
    tl_fonts_free(t->fonts);
    free(t);
}

/* ---- the disc ---------------------------------------------------------- */

static uint32_t rd32(const uint8_t *p)
{
    return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)(p[0] | p[1] << 8);
}

/* file.c FileUncompress: an LZ77 of literal runs and 2-byte copies. */
static uint8_t *uncompress(const uint8_t *src, size_t n, uint32_t size)
{
    uint8_t *out = malloc(size ? size : 1);
    size_t i = 0, o = 0;

    if (!out) {
        return NULL;
    }
    while (o < size) {
        if (i >= n) {
            free(out);
            return NULL;
        }
        if (src[i] & 0x80) {
            uint32_t code, num, off, k;

            if (i + 1 >= n) {
                free(out);
                return NULL;
            }
            code = (uint32_t)src[i] << 8 | src[i + 1];
            num = ((code & 0x7800) >> 11) + 3;
            off = (code & 0x7FF) + 1;
            i += 2;
            if (off > o) {
                free(out);
                return NULL;
            }
            if (num > size - o) {
                num = size - (uint32_t)o;
            }
            for (k = 0; k < num; k++, o++) {
                out[o] = out[o - off];
            }
        } else {
            size_t num = (size_t)src[i] + 1;

            if (num > size - o) {
                num = size - o;
            }
            i++;
            if (i + num > n) {
                free(out);
                return NULL;
            }
            memcpy(out + o, src + i, num);
            i += num;
            o += num;
        }
    }
    return out;
}

typedef struct {
    uint8_t *buf;
    size_t size;
} GameFile;

static int read_game_file(TlRead read, void *ctx, uint32_t cddata_lsn, const uint8_t *seekpos, uint32_t fid,
                          GameFile *out)
{
    uint32_t start = rd32(seekpos + 8 * fid), end = rd32(seekpos + 8 * (fid + 1));
    uint32_t packed = rd32(seekpos + 8 * fid + 4);
    uint8_t *raw;

    out->buf = NULL;
    if (end <= start || end - start > 0x4000) {
        return -1;
    }
    raw = malloc((size_t)(end - start) * SECTOR);
    if (!raw || read(ctx, cddata_lsn + start, end - start, raw) != 0) {
        free(raw);
        return -1;
    }
    if (packed == 0) {
        out->buf = raw;
        out->size = (size_t)(end - start) * SECTOR;
        return 0;
    }
    out->buf = uncompress(raw, (size_t)(end - start) * SECTOR, packed);
    out->size = packed;
    free(raw);
    return out->buf ? 0 : -1;
}

/* The offset of the picture at path: each level is an offset table that
   LinkAddrTb patches (offsets from the table's start, ended by
   0xFFFFFFFF). */
static int walk(const GameFile *f, const char *path, size_t *at)
{
    size_t base = 0;

    for (;;) {
        uint32_t idx = (uint32_t)atoi(path), k, off;

        for (k = 0; k <= idx; k++) {
            if (base + 4 * k + 4 > f->size || rd32(f->buf + base + 4 * k) == 0xFFFFFFFFu) {
                return -1;
            }
        }
        off = rd32(f->buf + base + 4 * idx);
        if (off >= f->size - base) {
            return -1;
        }
        base += off;
        path += 3;
        if (!*path) {
            *at = base;
            return 0;
        }
        path++;
    }
}

/* pxf.c read_pix8 with mode & 0x4000: a literal run (d < 0x40: d + 1
   bytes) or a repeat of the last literal byte (d - 0x3D times). */
static int rle8(const uint8_t *src, size_t n, uint8_t *out, size_t want)
{
    size_t i = 0, o = 0;
    uint8_t last = 0;

    while (o < want) {
        uint8_t d;

        if (i >= n) {
            return -1;
        }
        d = src[i++];
        if (!(d & 0xC0)) {
            size_t k = (size_t)d + 1;

            if (i + k > n) {
                return -1;
            }
            if (k > want - o) {
                k = want - o;
            }
            memcpy(out + o, src + i, k);
            o += k;
            i += (size_t)d + 1;
            last = src[i - 1];
        } else {
            size_t k = (size_t)d - 0x3D;

            if (k > want - o) {
                k = want - o;
            }
            memset(out + o, last, k);
            o += k;
        }
    }
    return 0;
}

static uint32_t ct16(uint16_t v)
{
    uint32_t r = (v & 31u) << 3, g = ((v >> 5) & 31u) << 3, b = ((v >> 10) & 31u) << 3;

    return r | g << 8 | b << 16 | (v & 0x8000 ? 0x80u : 0u) << 24;
}

/* The texels and CLUT PXF_SendTexture uploads for the PXF picture at off
   (pxf.c: PXF_Entry's header, PXF_Uncompress for compressed ones). Only
   the indexed formats: every text picture is PSMT4 or PSMT8. */
static int load_picture(const GameFile *f, size_t off, TlEntry *e)
{
    uint16_t mode, w, h;
    uint32_t n, clut_bytes, i;
    size_t need, p = off + 16;
    int fmt;

    if (off + 16 > f->size) {
        return -1;
    }
    mode = rd16(f->buf + off);
    w = rd16(f->buf + off + 2);
    h = rd16(f->buf + off + 4);
    fmt = mode & 0xFF;
    if ((fmt != 0x13 && fmt != 0x14) || !w || !h || w > 1024 || h > 1024) {
        return -1;
    }
    n = fmt == 0x13 ? 256 : 16;
    clut_bytes = (mode & 0xF00) == 0 ? n * 4 : (mode & 0xF00) == 0x200 ? n * 2 : 0;
    need = fmt == 0x13 ? (size_t)w * h : ((size_t)w * h + 1) / 2;
    if (p + clut_bytes > f->size) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        /* a 256-entry CLUT is stored in CSM1 order: bits 3 and 4 of the
           index swapped */
        uint32_t at = n == 256 ? ((i & ~0x18u) | (i & 8u) << 1 | (i & 16u) >> 1) : i;

        if (clut_bytes == n * 4) {
            e->clut[i] = rd32(f->buf + p + 4 * at);
        } else if (clut_bytes) {
            e->clut[i] = ct16(rd16(f->buf + p + 2 * at));
        } else {
            e->clut[i] = i * 255 / (n - 1) * 0x010101u | 0x80000000u;
        }
    }
    p += clut_bytes;
    e->orig = malloc(need);
    if (!e->orig) {
        return -1;
    }
    if (mode & 0x4000) {
        if (rle8(f->buf + p, f->size - p, e->orig, need) != 0) {
            return -1;
        }
    } else {
        if (p + need > f->size) {
            return -1;
        }
        memcpy(e->orig, f->buf + p, need);
    }
    e->psm = fmt;
    e->w = w;
    e->h = h;
    /* as dl_stream's transfer_done hashes the upload */
    e->hash = dl_hash64(e->orig, need, (uint64_t)fmt << 32 | (uint64_t)w << 16 | h);
    return 0;
}

int tl_read_picture(TlRead read, void *ctx, uint32_t cddata_lsn, const uint8_t *seekpos, uint32_t nfiles, uint32_t fid,
                    const char *path, TlRawPicture *out)
{
    GameFile f = { NULL, 0 };
    TlEntry e;
    size_t off;
    int rc = -1;

    memset(out, 0, sizeof(*out));
    memset(&e, 0, sizeof(e));
    if (fid < nfiles && read_game_file(read, ctx, cddata_lsn, seekpos, fid, &f) == 0 && walk(&f, path, &off) == 0 &&
        load_picture(&f, off, &e) == 0) {
        out->psm = e.psm;
        out->w = e.w;
        out->h = e.h;
        out->texels = e.orig;
        out->hash = e.hash;
        memcpy(out->clut, e.clut, sizeof(out->clut));
        rc = 0;
    } else {
        free(e.orig);
    }
    free(f.buf);
    return rc;
}

static void map_put(TextLayer *t, uint32_t idx)
{
    uint32_t m = t->map_cap - 1, k = (uint32_t)(t->e[idx].hash ^ t->e[idx].hash >> 32) & m;

    while (t->map[k] >= 0) {
        k = (k + 1) & m;
    }
    t->map[k] = (int32_t)idx;
}

int tl_bind(TextLayer *t, TlRead read, void *ctx, uint32_t cddata_lsn, const uint8_t *seekpos, uint32_t nfiles,
            char *err, size_t cap)
{
    uint32_t i, j, bound = 0;
    GameFile f = { NULL, 0 };
    uint32_t cur = 0xFFFFFFFFu;
    int file_ok = 0;

    if (cap) {
        err[0] = 0;
    }
    for (i = 0; i < t->n; i++) {
        TlEntry *e = &t->e[i];
        size_t off;

        free(e->orig);
        e->orig = NULL;
        e->psm = 0;
        if (e->fid >= nfiles) {
            note(err, cap, "%u/%s: no such file on this disc", e->fid, e->path);
            continue;
        }
        if (e->fid != cur) {
            free(f.buf);
            cur = e->fid;
            file_ok = read_game_file(read, ctx, cddata_lsn, seekpos, cur, &f) == 0;
            if (!file_ok) {
                note(err, cap, "%u: cannot read the file", cur);
            }
        }
        if (!file_ok) {
            continue;
        }
        if (walk(&f, e->path, &off) != 0 || load_picture(&f, off, e) != 0) {
            free(e->orig);
            e->orig = NULL;
            e->psm = 0;
            note(err, cap, "%u/%s: not an indexed picture", e->fid, e->path);
            continue;
        }
        bound++;
    }
    free(f.buf);
    free(t->map);
    for (t->map_cap = 64; t->map_cap < t->n * 2; t->map_cap *= 2) {
    }
    t->map = malloc(sizeof(int32_t) * t->map_cap);
    if (!t->map) {
        return -1;
    }
    memset(t->map, 0xFF, sizeof(int32_t) * t->map_cap);
    for (i = 0; i < t->n; i++) {
        if (!t->e[i].orig) {
            continue;
        }
        for (j = 0; j < i; j++) {
            if (t->e[j].orig && t->e[j].hash == t->e[i].hash) {
                /* the same picture twice (a file loaded under two ids): the
                   first entry wins */
                break;
            }
        }
        if (j == i) {
            map_put(t, i);
        }
    }
    return (int)bound;
}

static TlEntry *find(TextLayer *t, uint64_t hash)
{
    uint32_t m, k;

    if (!t->map) {
        return NULL;
    }
    m = t->map_cap - 1;
    for (k = (uint32_t)(hash ^ hash >> 32) & m; t->map[k] >= 0; k = (k + 1) & m) {
        if (t->e[t->map[k]].hash == hash) {
            return &t->e[t->map[k]];
        }
    }
    return NULL;
}

static const uint8_t *english(TextLayer *t, TlEntry *e)
{
    if (!e->english) {
        e->english = tl_draw(t->fonts, e, e->font == TL_FONT_DEFAULT ? t->default_font : e->font);
    }
    return e->english;
}

const uint8_t *tl_subst(void *ctx, uint64_t upload_hash, int psm, uint32_t w, uint32_t h, const uint8_t *texels,
                        uint32_t variant)
{
    TextLayer *t = ctx;
    TlEntry *e = find(t, upload_hash);

    (void)texels;
    (void)variant;
    if (!e || e->psm != psm || e->w != w || e->h != h) {
        return NULL;
    }
    return english(t, e);
}

uint32_t tl_count(const TextLayer *t)
{
    return t->n;
}

int tl_picture(TextLayer *t, uint32_t i, TlPicture *out)
{
    TlEntry *e;

    if (i >= t->n || !t->e[i].orig) {
        return -1;
    }
    e = &t->e[i];
    out->fid = e->fid;
    memcpy(out->path, e->path, sizeof(out->path));
    out->psm = e->psm;
    out->w = e->w;
    out->h = e->h;
    out->orig = e->orig;
    out->english = english(t, e);
    out->clut = e->clut;
    out->review = e->review;
    out->fit = e->fit;
    out->text = e->text;
    return 0;
}
