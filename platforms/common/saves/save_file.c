#include "save_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

/* The original layout (memory.c, memacc.c): the data file is the structure
   and a checksum byte, the icon is IconSize1/IconSize2 bytes of the disc's
   icon, icon.sys is a sceMcIconSys. */
#define SYS_DATA_SIZE (0x1614u + 1)    /* USER_SETTING */
#define REPLAY_DATA_SIZE (0x7094u + 1) /* VS_REPLAY */
#define ICON_SIZE 0xF1D8u
#define ICON_SYS_SIZE 964u
#define ICON_SYS_FNAME_VIEW 0x104      /* sceMcIconSys.FnameView: the icon file's name */

#define MAGIC "GGXSAVE"
#define FIXED_HEADER 80
#define FILE_HEADER 56

static const char *const SYSTEM_DIRS[] = { NULL, "BKSLPM-64525GGX", "BISLPS-20137GGX" };

static void put16(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static void put32(uint8_t *p, uint32_t v)
{
    put16(p, v);
    put16(p + 2, v >> 16);
}

static uint32_t get16(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8;
}

static uint32_t get32(const uint8_t *p)
{
    return get16(p) | get16(p + 2) << 16;
}

static size_t pad4(size_t n)
{
    return (n + 3) & ~(size_t)3;
}

void save_dir_init(SaveDir *d, const char *name)
{
    memset(d, 0, sizeof(*d));
    snprintf(d->name, sizeof(d->name), "%s", name ? name : "");
    d->mode = SAVE_MODE_DIR;
    d->release = save_release_of(d->name);
}

void save_dir_free(SaveDir *d)
{
    int i;

    for (i = 0; i < d->nfiles; i++) {
        free(d->files[i].data);
    }
    d->nfiles = 0;
}

int save_dir_copy(SaveDir *dst, const SaveDir *src)
{
    int i;

    *dst = *src;
    for (i = 0; i < src->nfiles; i++) {
        dst->files[i].data = NULL;
        if (src->files[i].size) {
            dst->files[i].data = malloc(src->files[i].size);
            if (!dst->files[i].data) {
                dst->nfiles = i;
                save_dir_free(dst);
                return -1;
            }
            memcpy(dst->files[i].data, src->files[i].data, src->files[i].size);
        }
    }
    return 0;
}

int save_dir_find(const SaveDir *d, const char *name)
{
    int i;

    for (i = 0; i < d->nfiles; i++) {
        if (!strcmp(d->files[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int save_dir_put(SaveDir *d, const char *name, uint16_t mode, const uint8_t created[8], const uint8_t modified[8],
                 const void *data, uint32_t size)
{
    int i = save_dir_find(d, name);
    uint8_t *copy = NULL;
    SaveFile *f;

    if (strlen(name) >= SAVE_NAME_MAX || (i < 0 && d->nfiles >= SAVE_MAX_FILES)) {
        return -1;
    }
    if (size) {
        copy = malloc(size);
        if (!copy) {
            return -1;
        }
        memcpy(copy, data, size);
    }
    if (i < 0) {
        i = d->nfiles++;
        memset(&d->files[i], 0, sizeof(d->files[i]));
        snprintf(d->files[i].name, sizeof(d->files[i].name), "%s", name);
    }
    f = &d->files[i];
    free(f->data);
    f->mode = mode;
    if (created) {
        memcpy(f->created, created, 8);
    }
    if (modified) {
        memcpy(f->modified, modified, 8);
    }
    f->size = size;
    f->data = copy;
    return 0;
}

void save_dir_remove(SaveDir *d, int index)
{
    if (index < 0 || index >= d->nfiles) {
        return;
    }
    free(d->files[index].data);
    memmove(&d->files[index], &d->files[index + 1], sizeof(d->files[0]) * (size_t)(d->nfiles - index - 1));
    d->nfiles--;
}

int save_dir_touch(SaveDir *d, const char *id, uint32_t flags)
{
    int i;

    for (i = 0; i < d->next; i++) {
        if (!strcmp(d->ext[i].id, id)) {
            /* save-compatible only if it said so every time */
            d->ext[i].flags &= flags;
            return 0;
        }
    }
    if (d->next >= SAVE_MAX_EXTENSIONS || strlen(id) >= SAVE_EXT_ID_MAX || !*id) {
        return -1;
    }
    snprintf(d->ext[d->next].id, sizeof(d->ext[0].id), "%s", id);
    d->ext[d->next++].flags = flags;
    return 0;
}

uint32_t save_release_of(const char *name)
{
    uint32_t r;

    for (r = SAVE_RELEASE_KR; r <= SAVE_RELEASE_JP; r++) {
        size_t n = strlen(SYSTEM_DIRS[r]);

        if (!strncmp(name, SYSTEM_DIRS[r], n) &&
            (name[n] == 0 || (name[n] == 'R' && name[n + 1] == 'P' && name[n + 2] >= '0' && name[n + 2] <= '9' &&
                              name[n + 3] == 0))) {
            return r;
        }
    }
    return 0;
}

const char *save_release_name(uint32_t release)
{
    return release == SAVE_RELEASE_KR ? "KR (SLPM-645.25)" : release == SAVE_RELEASE_JP ? "JP (SLPS-201.37)" : "unknown";
}

const char *save_system_dir(uint32_t release)
{
    return release == SAVE_RELEASE_KR || release == SAVE_RELEASE_JP ? SYSTEM_DIRS[release] : NULL;
}

int save_encode(const SaveDir *d, uint8_t **out, size_t *len)
{
    size_t n = FIXED_HEADER, hdr, o;
    uint8_t *b;
    int i;

    for (i = 0; i < d->next; i++) {
        n += 8 + pad4(strlen(d->ext[i].id));
    }
    hdr = n;
    for (i = 0; i < d->nfiles; i++) {
        n += FILE_HEADER + pad4(d->files[i].size);
    }
    n += 8;
    b = calloc(1, n);
    if (!b) {
        return SAVE_EIO;
    }
    memcpy(b, MAGIC, 8);
    put32(b + 8, SAVE_FORMAT_VERSION);
    put32(b + 12, d->release);
    put32(b + 16, (uint32_t)hdr);
    put32(b + 20, (uint32_t)d->nfiles);
    put32(b + 24, (uint32_t)d->next);
    put16(b + 28, d->mode);
    memcpy(b + 32, d->name, SAVE_NAME_MAX);
    memcpy(b + 64, d->created, 8);
    memcpy(b + 72, d->modified, 8);
    o = FIXED_HEADER;
    for (i = 0; i < d->next; i++) {
        size_t l = strlen(d->ext[i].id);

        put32(b + o, d->ext[i].flags);
        put32(b + o + 4, (uint32_t)l);
        memcpy(b + o + 8, d->ext[i].id, l);
        o += 8 + pad4(l);
    }
    for (i = 0; i < d->nfiles; i++) {
        const SaveFile *f = &d->files[i];

        memcpy(b + o, f->name, SAVE_NAME_MAX);
        put16(b + o + 32, f->mode);
        put32(b + o + 36, f->size);
        memcpy(b + o + 40, f->created, 8);
        memcpy(b + o + 48, f->modified, 8);
        if (f->size) {
            memcpy(b + o + FILE_HEADER, f->data, f->size);
        }
        o += FILE_HEADER + pad4(f->size);
    }
    {
        uint64_t h = XXH3_64bits(b, o);

        put32(b + o, (uint32_t)h);
        put32(b + o + 4, (uint32_t)(h >> 32));
    }
    *out = b;
    *len = n;
    return SAVE_OK;
}

static int bad(char *why, size_t cap, const char *what)
{
    snprintf(why, cap, "%s", what);
    return SAVE_EBAD;
}

/* a NUL-terminated name from a fixed field */
static int name_ok(const uint8_t *p)
{
    return memchr(p, 0, SAVE_NAME_MAX) != NULL && p[0] != 0;
}

int save_decode(const uint8_t *b, size_t len, SaveDir *d, uint32_t *version, char *why, size_t cap)
{
    size_t hdr, o;
    uint32_t v, nfiles, next, i;
    uint64_t h;

    memset(d, 0, sizeof(*d));
    if (len < 16 || memcmp(b, MAGIC, 8) != 0) {
        return bad(why, cap, "not a ggx save file");
    }
    v = get32(b + 8);
    if (version) {
        *version = v;
    }
    if (v != SAVE_FORMAT_VERSION) {
        snprintf(why, cap, "it is save format version %u, and this build reads version %u", v, SAVE_FORMAT_VERSION);
        return v > SAVE_FORMAT_VERSION ? SAVE_ENEWER : SAVE_EBAD;
    }
    if (len < FIXED_HEADER + 8) {
        return bad(why, cap, "the file is cut short");
    }
    h = (uint64_t)get32(b + len - 8) | (uint64_t)get32(b + len - 4) << 32;
    if (h != XXH3_64bits(b, len - 8)) {
        return bad(why, cap, "its hash doesn't match its contents (the file is damaged)");
    }
    hdr = get32(b + 16);
    nfiles = get32(b + 20);
    next = get32(b + 24);
    if (nfiles > SAVE_MAX_FILES || next > SAVE_MAX_EXTENSIONS || hdr > len - 8 || !name_ok(b + 32)) {
        return bad(why, cap, "its header is damaged");
    }
    d->release = get32(b + 12);
    d->mode = (uint16_t)get16(b + 28);
    memcpy(d->name, b + 32, SAVE_NAME_MAX);
    memcpy(d->created, b + 64, 8);
    memcpy(d->modified, b + 72, 8);
    o = FIXED_HEADER;
    for (i = 0; i < next; i++) {
        uint32_t l;

        if (o + 8 > hdr || (l = get32(b + o + 4)) >= SAVE_EXT_ID_MAX || o + 8 + l > hdr) {
            return bad(why, cap, "its extension list is damaged");
        }
        d->ext[i].flags = get32(b + o);
        memcpy(d->ext[i].id, b + o + 8, l);
        d->ext[i].id[l] = 0;
        o += 8 + pad4(l);
    }
    d->next = (int)next;
    o = hdr;
    for (i = 0; i < nfiles; i++) {
        SaveFile *f = &d->files[i];

        if (o + FILE_HEADER > len - 8 || !name_ok(b + o)) {
            save_dir_free(d);
            return bad(why, cap, "its file list is damaged");
        }
        memcpy(f->name, b + o, SAVE_NAME_MAX);
        f->mode = (uint16_t)get16(b + o + 32);
        f->size = get32(b + o + 36);
        memcpy(f->created, b + o + 40, 8);
        memcpy(f->modified, b + o + 48, 8);
        if (f->size > len - 8 - o - FILE_HEADER) {
            save_dir_free(d);
            return bad(why, cap, "its file list is damaged");
        }
        f->data = NULL;
        if (f->size) {
            f->data = malloc(f->size);
            if (!f->data) {
                save_dir_free(d);
                return bad(why, cap, "out of memory");
            }
            memcpy(f->data, b + o + FILE_HEADER, f->size);
        }
        d->nfiles = (int)i + 1;
        o += FILE_HEADER + pad4(f->size);
    }
    return SAVE_OK;
}

int save_write(const char *path, const SaveDir *d, char *why, size_t cap)
{
    char tmp[1100];
    uint8_t *buf;
    size_t len;
    FILE *f;
    int ok;

    if (snprintf(tmp, sizeof(tmp), "%s.tmp", path) >= (int)sizeof(tmp)) {
        snprintf(why, cap, "the path %s is too long", path);
        return SAVE_EIO;
    }
    if (save_encode(d, &buf, &len) != SAVE_OK) {
        snprintf(why, cap, "out of memory");
        return SAVE_EIO;
    }
    f = fs_open(tmp, "wb");
    if (!f) {
        free(buf);
        snprintf(why, cap, "cannot write %s", tmp);
        return SAVE_EIO;
    }
    ok = fwrite(buf, 1, len, f) == len;
    ok = fflush(f) == 0 && ok;
    ok = fclose(f) == 0 && ok;
    free(buf);
    if (!ok || fs_replace(tmp, path) != 0) {
        fs_remove(tmp);
        snprintf(why, cap, "cannot write %s", path);
        return SAVE_EIO;
    }
    return SAVE_OK;
}

int save_read(const char *path, SaveDir *d, uint32_t *version, char *why, size_t cap)
{
    FILE *f = fs_open(path, "rb");
    int64_t n;
    uint8_t *buf;
    int rc;

    memset(d, 0, sizeof(*d));
    if (!f) {
        snprintf(why, cap, "cannot read %s", path);
        return SAVE_EIO;
    }
    n = fs_size(f);
    if (n < 0 || n > 64 * 1024 * 1024 || fs_seek(f, 0) != 0) {
        fclose(f);
        snprintf(why, cap, "cannot read %s", path);
        return SAVE_EIO;
    }
    buf = malloc(n ? (size_t)n : 1);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) {
        free(buf);
        fclose(f);
        snprintf(why, cap, "cannot read %s", path);
        return SAVE_EIO;
    }
    fclose(f);
    rc = save_decode(buf, (size_t)n, d, version, why, cap);
    free(buf);
    return rc;
}

int save_checksum_ok(const uint8_t *data, uint32_t size)
{
    uint8_t sum = 0;
    uint32_t i;

    for (i = 0; i < size; i++) {
        sum = (uint8_t)(sum + data[i]);
    }
    return sum == 1;
}

int save_check_layout(const SaveDir *d, char *why, size_t cap)
{
    uint32_t release = save_release_of(d->name), want;
    const SaveFile *data, *sys, *icon;
    char icon_name[65];
    int i, replay;

    if (!release) {
        snprintf(why, cap, "%s is not one of Guilty Gear X Plus's save directories", d->name);
        return -1;
    }
    replay = strlen(d->name) > strlen(SYSTEM_DIRS[release]);
    i = save_dir_find(d, d->name);
    if (i < 0) {
        snprintf(why, cap, "%s has no data file %s", d->name, d->name);
        return -1;
    }
    data = &d->files[i];
    want = replay ? REPLAY_DATA_SIZE : SYS_DATA_SIZE;
    if (data->size != want) {
        snprintf(why, cap, "its data file %s is %u bytes, and the game's is %u", data->name, data->size, want);
        return -1;
    }
    if (!save_checksum_ok(data->data, data->size)) {
        snprintf(why, cap, "its data file %s fails the game's checksum", data->name);
        return -1;
    }
    i = save_dir_find(d, "icon.sys");
    if (i < 0) {
        snprintf(why, cap, "%s has no icon.sys", d->name);
        return -1;
    }
    sys = &d->files[i];
    if (sys->size != ICON_SYS_SIZE || memcmp(sys->data, "PS2D", 4) != 0) {
        snprintf(why, cap, "its icon.sys is not the game's (%u bytes)", sys->size);
        return -1;
    }
    memcpy(icon_name, sys->data + ICON_SYS_FNAME_VIEW, 64);
    icon_name[64] = 0;
    i = save_dir_find(d, icon_name);
    if (!icon_name[0] || i < 0) {
        snprintf(why, cap, "the icon its icon.sys names (\"%s\") is missing", icon_name);
        return -1;
    }
    icon = &d->files[i];
    if (icon->size != ICON_SIZE) {
        snprintf(why, cap, "its icon %s is %u bytes, and the game's is %u", icon->name, icon->size, ICON_SIZE);
        return -1;
    }
    for (i = 0; i < d->nfiles; i++) {
        const SaveFile *f = &d->files[i];

        if (f != data && f != sys && f != icon) {
            snprintf(why, cap, "it holds a file the game never writes: %s", f->name);
            return -1;
        }
    }
    return 0;
}

int save_check_export(const SaveDir *d, char *why, size_t cap)
{
    uint32_t release = save_release_of(d->name);
    int i;

    if (d->release != SAVE_RELEASE_KR && d->release != SAVE_RELEASE_JP) {
        snprintf(why, cap, "its header names release %u, which is neither KR (1) nor JP (2)", d->release);
        return -1;
    }
    if (release != d->release) {
        snprintf(why, cap, "its header says %s, but the directory %s is %s's", save_release_name(d->release), d->name,
                 release ? save_release_name(release) : "neither release");
        return -1;
    }
    for (i = 0; i < d->next; i++) {
        if (!(d->ext[i].flags & SAVE_EXT_COMPATIBLE)) {
            snprintf(why, cap, "it was written with the extension \"%s\", which is not save-compatible: the PS2 game "
                               "can't represent what it changed", d->ext[i].id);
            return -1;
        }
    }
    return save_check_layout(d, why, cap);
}

int save_store_dir(char *buf, size_t cap, const char *data_dir)
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
    return fs_join(buf, cap, data_dir, n && (data_dir[n - 1] == '/' || data_dir[n - 1] == '\\') ? "saves" : "/saves",
                   NULL);
}

int save_store_path(char *buf, size_t cap, const char *store, const char *dir_name)
{
    return snprintf(buf, cap, "%s/%s.ggxsave", store, dir_name) < (int)cap ? 0 : -1;
}

void save_ps2_time(int64_t t, uint8_t out[8])
{
    /* the PS2 keeps Japan's time on its cards, whatever the console's time zone */
    int64_t days, secs, y, m, doy, mp, era, yoe, doe;

    t += 9 * 3600;
    days = t >= 0 ? t / 86400 : -((-t + 86399) / 86400);
    secs = t - days * 86400;
    /* civil_from_days (H. Hinnant) */
    days += 719468;
    era = (days >= 0 ? days : days - 146096) / 146097;
    doe = days - era * 146097;
    yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    y = yoe + era * 400;
    doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    mp = (5 * doy + 2) / 153;
    m = mp < 10 ? mp + 3 : mp - 9;
    y += m <= 2;
    out[0] = 0;
    out[1] = (uint8_t)(secs % 60);
    out[2] = (uint8_t)(secs / 60 % 60);
    out[3] = (uint8_t)(secs / 3600);
    out[4] = (uint8_t)(doy - (153 * mp + 2) / 5 + 1);
    out[5] = (uint8_t)m;
    put16(out + 6, (uint32_t)y);
}
