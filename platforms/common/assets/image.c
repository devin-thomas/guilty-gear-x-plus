#include "image.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libchdr/chd.h>

#include "fsys.h"

/* raw sectors read per fread when an image's sectors aren't 2048 bytes */
#define BATCH 64

struct DiscImage {
    char format[48];
    uint32_t sectors;
    /* ISO, BIN: LSN 0 starts at byte start, each sector holding its user
       data at data_offset */
    FILE *f;
    uint64_t start;
    uint32_t sector_bytes;    /* 2048, 2336 or 2352 */
    uint32_t data_offset;
    uint8_t *batch;           /* BATCH raw sectors */
    /* CHD: sector lsn is unit first_unit + lsn, its user data at data_offset */
    chd_file *chd;
    uint32_t unit_bytes;
    uint32_t units_per_hunk;
    uint32_t first_unit;
    uint8_t *hunk;
    uint32_t hunk_num;        /* the hunk in hunk, or UINT32_MAX */
};

static const uint8_t SYNC[12] = { 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0 };

static int fail(char *err, size_t cap, const char *fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 3, 4)))
#endif
    ;

static int fail(char *err, size_t cap, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(err, cap, fmt, ap);
    va_end(ap);
    return -1;
}

static int read_at(FILE *f, uint64_t off, void *dst, size_t n)
{
    return fs_seek(f, off) == 0 && fread(dst, 1, n, f) == n ? 0 : -1;
}

/* ISO or BIN: the layout is set by the caller (a CUE sheet) or found from
   the content: a raw image has the sync pattern at sector 16, its volume
   descriptor */
static int open_plain(DiscImage *img, const char *path, uint32_t sector_bytes, uint64_t start, const char *mode,
                      char *err, size_t cap)
{
    uint8_t raw[2352];
    int64_t size;

    img->f = fs_open(path, "rb");
    if (!img->f) {
        return fail(err, cap, "cannot open %s", path);
    }
    size = fs_size(img->f);
    if (size < 0) {
        return fail(err, cap, "cannot read %s", path);
    }
    if (!sector_bytes) {
        sector_bytes = 2048;
        if (size % 2352 == 0 && read_at(img->f, 16 * 2352ull, raw, sizeof(raw)) == 0 && !memcmp(raw, SYNC, 12)) {
            sector_bytes = 2352;
        }
    }
    img->sector_bytes = sector_bytes;
    img->start = start;
    if (sector_bytes == 2352) {
        /* Mode 1 keeps its user data after a 16-byte header, Mode 2 Form 1
           after the 8-byte subheader too */
        if (read_at(img->f, start + 16 * 2352ull, raw, sizeof(raw)) != 0 || memcmp(raw, SYNC, 12) != 0) {
            return fail(err, cap, "%s has no raw CD sectors where its CUE sheet says", path);
        }
        img->data_offset = raw[15] == 2 ? 24 : 16;
        snprintf(img->format, sizeof(img->format), "BIN (%s)",
                 mode ? mode : raw[15] == 2 ? "MODE2/2352" : "MODE1/2352");
    } else if (sector_bytes == 2336) {
        img->data_offset = 8;
        snprintf(img->format, sizeof(img->format), "%s", "BIN (MODE2/2336)");
    } else {
        img->data_offset = 0;
        snprintf(img->format, sizeof(img->format), "%s", mode ? "BIN (MODE1/2048)" : "ISO");
    }
    if ((uint64_t)size < start) {
        return fail(err, cap, "%s is shorter than its CUE sheet says", path);
    }
    img->sectors = (uint32_t)(((uint64_t)size - start) / sector_bytes);
    if (sector_bytes != 2048) {
        img->batch = malloc((size_t)BATCH * sector_bytes);
        if (!img->batch) {
            return fail(err, cap, "out of memory");
        }
    }
    return 0;
}

/* the next token of a CUE line: a word, or a "quoted string" */
static const char *token(const char *p, char *out, size_t cap)
{
    size_t n = 0;

    while (*p == ' ' || *p == '\t') {
        p++;
    }
    if (*p == '"') {
        for (p++; *p && *p != '"'; p++) {
            if (n + 1 < cap) {
                out[n++] = *p;
            }
        }
        if (*p == '"') {
            p++;
        }
    } else {
        for (; *p && *p != ' ' && *p != '\t'; p++) {
            if (n + 1 < cap) {
                out[n++] = *p;
            }
        }
    }
    out[n] = 0;
    return p;
}

static int is_cue(const uint8_t *head, size_t n)
{
    static const char *const words[] = { "FILE", "REM", "CATALOG", "TITLE", "PERFORMER", "CDTEXTFILE", "SONGWRITER" };
    size_t i = 0, w;

    if (n >= 3 && !memcmp(head, "\xef\xbb\xbf", 3)) {
        i = 3;
    }
    while (i < n && isspace(head[i])) {
        i++;
    }
    for (w = 0; w < sizeof(words) / sizeof(words[0]); w++) {
        size_t len = strlen(words[w]);

        if (i + len < n && !memcmp(head + i, words[w], len) && isspace(head[i + len])) {
            return 1;
        }
    }
    return 0;
}

/* Track 1 of a CUE sheet: the BIN it is in, its sector size, and where it
   starts (INDEX 01). */
static int open_cue(DiscImage *img, const char *path, char *err, size_t cap)
{
    FILE *f = fs_open(path, "rb");
    char *text, *line, *next;
    char file[512] = "", bin[1024], word[64], arg[512];
    const char *slash1, *slash2, *dir_end;
    size_t dir_len;
    int64_t size;
    int in_track = 0, have_track = 0;
    uint32_t sector_bytes = 0, frames = 0, m, s, fr;
    char mode[16] = "";

    if (!f) {
        return fail(err, cap, "cannot open %s", path);
    }
    size = fs_size(f);
    if (size <= 0 || size > 65536 || fs_seek(f, 0) != 0) {
        fclose(f);
        return fail(err, cap, "%s is not a CUE sheet", path);
    }
    text = malloc((size_t)size + 1);
    if (!text || fread(text, 1, (size_t)size, f) != (size_t)size) {
        free(text);
        fclose(f);
        return fail(err, cap, "cannot read %s", path);
    }
    fclose(f);
    text[size] = 0;
    for (line = text; line && *line; line = next) {
        const char *p;

        next = strpbrk(line, "\r\n");
        if (next) {
            *next++ = 0;
        }
        p = token(line, word, sizeof(word));
        if (!strcmp(word, "FILE") && !have_track) {
            token(p, file, sizeof(file));
        } else if (!strcmp(word, "TRACK")) {
            if (have_track) {
                break;     /* only track 1 holds the PS2's data */
            }
            p = token(p, arg, sizeof(arg));
            token(p, mode, sizeof(mode));
            in_track = have_track = 1;
            if (!strcmp(mode, "MODE1/2048")) {
                sector_bytes = 2048;
            } else if (!strcmp(mode, "MODE2/2336")) {
                sector_bytes = 2336;
            } else if (!strcmp(mode, "MODE1/2352") || !strcmp(mode, "MODE2/2352")) {
                sector_bytes = 2352;
            } else {
                free(text);
                return fail(err, cap, "%s: track 1 is %s, not a data track the port reads (MODE1/2048, MODE1/2352, "
                            "MODE2/2336 or MODE2/2352)", path, mode);
            }
        } else if (!strcmp(word, "INDEX") && in_track) {
            p = token(p, arg, sizeof(arg));
            if (!strcmp(arg, "01")) {
                token(p, arg, sizeof(arg));
                if (sscanf(arg, "%u:%u:%u", &m, &s, &fr) != 3) {
                    free(text);
                    return fail(err, cap, "%s: bad INDEX 01 \"%s\"", path, arg);
                }
                frames = (m * 60 + s) * 75 + fr;
            }
        }
    }
    free(text);
    if (!have_track || !file[0]) {
        return fail(err, cap, "%s names no FILE and TRACK", path);
    }
    /* the BIN is beside the sheet unless the sheet gives a full path */
    slash1 = strrchr(path, '/');
    slash2 = strrchr(path, '\\');
    dir_end = !slash1 ? slash2 : !slash2 ? slash1 : slash1 > slash2 ? slash1 : slash2;
    dir_len = dir_end ? (size_t)(dir_end - path + 1) : 0;
    if (file[0] == '/' || file[0] == '\\' || (file[0] && file[1] == ':')) {
        dir_len = 0;
    }
    if (dir_len + strlen(file) >= sizeof(bin)) {
        return fail(err, cap, "%s: the path of its BIN is too long", path);
    }
    memcpy(bin, path, dir_len);
    strcpy(bin + dir_len, file);
    return open_plain(img, bin, sector_bytes, (uint64_t)frames * sector_bytes, mode, err, cap);
}

static int open_chd(DiscImage *img, const char *path, char *err, size_t cap)
{
    FILE *f = fs_open(path, "rb");
    const chd_header *hd;
    chd_error e;
    char meta[256], type[32] = "", subtype[32], pgtype[32] = "", pgsub[32];
    uint32_t len = 0;
    int track = 0, frames = 0, pregap = 0, postgap = 0;

    if (!f) {
        return fail(err, cap, "cannot open %s", path);
    }
    img->f = f;    /* libchdr reads it but leaves closing it to us */
    e = chd_open_file(f, CHD_OPEN_READ, NULL, &img->chd);
    if (e != CHDERR_NONE) {
        return fail(err, cap, "%s: %s", path, chd_error_string(e));
    }
    hd = chd_get_header(img->chd);
    if (chd_get_metadata(img->chd, CDROM_TRACK_METADATA2_TAG, 0, meta, sizeof(meta) - 1, &len, NULL, NULL) ==
        CHDERR_NONE) {
        meta[len < sizeof(meta) ? len : sizeof(meta) - 1] = 0;
        if (sscanf(meta, "TRACK:%d TYPE:%31s SUBTYPE:%31s FRAMES:%d PREGAP:%d PGTYPE:%31s PGSUB:%31s POSTGAP:%d",
                   &track, type, subtype, &frames, &pregap, pgtype, pgsub, &postgap) != 8) {
            return fail(err, cap, "%s: unreadable track metadata", path);
        }
    } else if (chd_get_metadata(img->chd, CDROM_TRACK_METADATA_TAG, 0, meta, sizeof(meta) - 1, &len, NULL, NULL) ==
               CHDERR_NONE) {
        meta[len < sizeof(meta) ? len : sizeof(meta) - 1] = 0;
        if (sscanf(meta, "TRACK:%d TYPE:%31s SUBTYPE:%31s FRAMES:%d", &track, type, subtype, &frames) != 4) {
            return fail(err, cap, "%s: unreadable track metadata", path);
        }
    }
    if (type[0]) {
        /* a CD: units are frames of sector data plus subcode */
        static const struct { const char *type; uint32_t offset; } TYPES[] = {
            { "MODE1", 0 }, { "MODE2_FORM1", 0 }, { "MODE1_RAW", 16 }, { "MODE2_RAW", 24 },
            { "MODE2", 8 }, { "MODE2_FORM_MIX", 8 },
        };
        size_t i;

        for (i = 0; i < sizeof(TYPES) / sizeof(TYPES[0]) && strcmp(type, TYPES[i].type); i++) {
        }
        if (track != 1 || i == sizeof(TYPES) / sizeof(TYPES[0])) {
            return fail(err, cap, "%s: track %d is %s, not a data track the port reads", path, track, type);
        }
        img->data_offset = TYPES[i].offset;
        /* a pregap chdman stored ("V...") comes before LSN 0 */
        img->first_unit = pgtype[0] == 'V' ? (uint32_t)pregap : 0;
        img->sectors = frames > (int)img->first_unit ? (uint32_t)frames - img->first_unit : 0;
        snprintf(img->format, sizeof(img->format), "CHD (CD, %s)", type);
    } else {
        /* a DVD: units are 2048-byte sectors */
        if (hd->unitbytes != 2048) {
            return fail(err, cap, "%s is a CHD, but not of a CD or DVD", path);
        }
        img->data_offset = 0;
        img->first_unit = 0;
        img->sectors = (uint32_t)(hd->logicalbytes / 2048);
        snprintf(img->format, sizeof(img->format), "%s", "CHD (DVD)");
    }
    img->unit_bytes = hd->unitbytes;
    if (!img->unit_bytes || hd->hunkbytes % img->unit_bytes != 0 || img->data_offset + 2048 > img->unit_bytes) {
        return fail(err, cap, "%s: unsupported CHD layout (%u-byte hunks of %u-byte units)", path, hd->hunkbytes,
                    hd->unitbytes);
    }
    img->units_per_hunk = hd->hunkbytes / img->unit_bytes;
    img->hunk = malloc(hd->hunkbytes);
    img->hunk_num = UINT32_MAX;
    if (!img->hunk) {
        return fail(err, cap, "out of memory");
    }
    return 0;
}

int disc_image_open(DiscImage **out, const char *path, char *err, size_t errcap)
{
    DiscImage *img = calloc(1, sizeof(*img));
    uint8_t head[512];
    size_t n = 0;
    FILE *f;
    int rc;

    *out = NULL;
    if (!img) {
        return fail(err, errcap, "out of memory");
    }
    f = fs_open(path, "rb");
    if (!f) {
        free(img);
        return fail(err, errcap, "cannot open %s", path);
    }
    n = fread(head, 1, sizeof(head), f);
    fclose(f);
    if (n >= 8 && !memcmp(head, "MComprHD", 8)) {
        rc = open_chd(img, path, err, errcap);
    } else if (is_cue(head, n)) {
        rc = open_cue(img, path, err, errcap);
    } else {
        rc = open_plain(img, path, 0, 0, NULL, err, errcap);
    }
    if (rc != 0) {
        disc_image_close(img);
        return -1;
    }
    *out = img;
    return 0;
}

void disc_image_close(DiscImage *img)
{
    if (!img) {
        return;
    }
    if (img->chd) {
        chd_close(img->chd);
    }
    if (img->f) {
        fclose(img->f);
    }
    free(img->batch);
    free(img->hunk);
    free(img);
}

const char *disc_image_format(const DiscImage *img)
{
    return img->format;
}

uint32_t disc_image_sectors(const DiscImage *img)
{
    return img->sectors;
}

static int read_chd(DiscImage *img, uint32_t lsn, uint32_t count, uint8_t *out)
{
    uint32_t i;

    for (i = 0; i < count; i++) {
        uint32_t unit = img->first_unit + lsn + i, hunk = unit / img->units_per_hunk;

        if (hunk != img->hunk_num) {
            if (chd_read(img->chd, hunk, img->hunk) != CHDERR_NONE) {
                img->hunk_num = UINT32_MAX;
                return -1;
            }
            img->hunk_num = hunk;
        }
        memcpy(out + (size_t)i * 2048,
               img->hunk + (size_t)(unit % img->units_per_hunk) * img->unit_bytes + img->data_offset, 2048);
    }
    return 0;
}

int disc_image_read(void *src, uint32_t lsn, uint32_t count, void *dst)
{
    DiscImage *img = src;
    uint8_t *out = dst;

    if ((uint64_t)lsn + count > img->sectors) {
        return -1;
    }
    if (img->chd) {
        return read_chd(img, lsn, count, out);
    }
    if (img->sector_bytes == 2048) {
        return read_at(img->f, img->start + (uint64_t)lsn * 2048, out, (size_t)count * 2048);
    }
    while (count) {
        uint32_t n = count < BATCH ? count : BATCH, i;
        uint64_t at = img->start + (uint64_t)lsn * img->sector_bytes;

        if (read_at(img->f, at, img->batch, (size_t)n * img->sector_bytes)) {
            return -1;
        }
        for (i = 0; i < n; i++) {
            memcpy(out + (size_t)i * 2048, img->batch + (size_t)i * img->sector_bytes + img->data_offset, 2048);
        }
        out += (size_t)n * 2048;
        lsn += n;
        count -= n;
    }
    return 0;
}
