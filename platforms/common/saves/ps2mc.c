#include "ps2mc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fsys.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#define PAGE 512u
#define SPARE 16u
#define CLUSTER 1024u
#define EPC (CLUSTER / 4) /* FAT entries per cluster */
#define ENTRY 512u
#define FAT_FREE 0x7FFFFFFFu
#define FAT_END 0xFFFFFFFFu
#define FAT_USED 0x80000000u
#define DF_EXISTS 0x8000u
#define DF_DIR 0x0020u
#define MAGIC "Sony PS2 Memory Card Format "

struct Ps2Card {
    int folder;
    char path[1024];          /* a folder card's folder */
    uint8_t *img;
    size_t size;
    uint32_t spare;           /* 16 with ECC, else 0 */
    uint32_t pages;
    uint32_t clusters;
    uint32_t alloc_off;       /* the first cluster that FAT entry 0 describes */
    uint32_t alloc_end;       /* FAT entries in use for allocation */
    uint32_t root;            /* the root directory's first cluster, relative to alloc_off */
    uint32_t ifc[32];
};

static uint32_t get16(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8;
}

static uint32_t get32(const uint8_t *p)
{
    return get16(p) | get16(p + 2) << 16;
}

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

/* ------------------------------------------------------------------ ECC */

static int parity(uint32_t b)
{
    b ^= b >> 4;
    b ^= b >> 2;
    b ^= b >> 1;
    return (int)(b & 1);
}

void ps2mc_ecc(const uint8_t *s, uint8_t out[3])
{
    static const uint8_t cp_masks[7] = { 0x55, 0x33, 0x0F, 0x00, 0xAA, 0xCC, 0xF0 };
    uint8_t column = 0x77, line0 = 0x7F, line1 = 0x7F;
    int i, k;

    for (i = 0; i < 128; i++) {
        uint8_t mask = 0;

        for (k = 0; k < 7; k++) {
            mask |= (uint8_t)(parity(s[i] & cp_masks[k]) << k);
        }
        column ^= mask;
        if (parity(s[i])) {
            line0 ^= (uint8_t)~i;
            line1 ^= (uint8_t)i;
        }
    }
    out[0] = column;
    out[1] = line0 & 0x7F;
    out[2] = line1;
}

/* --------------------------------------------------------------- pages */

static uint8_t *page(Ps2Card *c, uint32_t n)
{
    return c->img + (size_t)n * (PAGE + c->spare);
}

static void page_ecc(Ps2Card *c, uint32_t n)
{
    uint8_t *p = page(c, n);
    int q;

    if (!c->spare) {
        return;
    }
    for (q = 0; q < 4; q++) {
        ps2mc_ecc(p + q * 128, p + PAGE + q * 3);
    }
}

/* clusters are absolute here: two pages each */
static void read_cluster(Ps2Card *c, uint32_t n, uint8_t *buf)
{
    memcpy(buf, page(c, n * 2), PAGE);
    memcpy(buf + PAGE, page(c, n * 2 + 1), PAGE);
}

static void write_cluster(Ps2Card *c, uint32_t n, const uint8_t *buf)
{
    memcpy(page(c, n * 2), buf, PAGE);
    memcpy(page(c, n * 2 + 1), buf + PAGE, PAGE);
    page_ecc(c, n * 2);
    page_ecc(c, n * 2 + 1);
}

static uint8_t *cluster_word(Ps2Card *c, uint32_t n, uint32_t index)
{
    return page(c, n * 2 + index * 4 / PAGE) + index * 4 % PAGE;
}

/* the FAT cluster holding entry n, or 0 */
static uint32_t fat_cluster(Ps2Card *c, uint32_t n)
{
    uint32_t ind = n / EPC, ifc;

    if (ind / EPC >= 32) {
        return 0;
    }
    ifc = c->ifc[ind / EPC];
    if (ifc == 0 || ifc >= c->clusters) {
        return 0;
    }
    ifc = get32(cluster_word(c, ifc, ind % EPC));
    return ifc < c->clusters ? ifc : 0;
}

static int fat_get(Ps2Card *c, uint32_t n, uint32_t *v)
{
    uint32_t fc = n < c->alloc_end ? fat_cluster(c, n) : 0;

    if (!fc) {
        return -1;
    }
    *v = get32(cluster_word(c, fc, n % EPC));
    return 0;
}

static int fat_set(Ps2Card *c, uint32_t n, uint32_t v)
{
    uint32_t fc = n < c->alloc_end ? fat_cluster(c, n) : 0, idx = n % EPC;

    if (!fc) {
        return -1;
    }
    put32(cluster_word(c, fc, idx), v);
    page_ecc(c, fc * 2 + idx * 4 / PAGE);
    return 0;
}

/* The clusters of a chain (relative), at most max; -1 if it is broken. */
static int chain(Ps2Card *c, uint32_t start, uint32_t *out, int max)
{
    int n = 0;
    uint32_t v;

    while (n < max) {
        if (start >= c->alloc_end || fat_get(c, start, &v) != 0 || !(v & FAT_USED)) {
            return -1;
        }
        out[n++] = start;
        if (v == FAT_END) {
            return n;
        }
        start = v & ~FAT_USED;
    }
    return -1;
}

/* Reads length bytes of a chain; buf is malloc'd. */
static int read_chain(Ps2Card *c, uint32_t start, uint32_t length, uint8_t **buf)
{
    uint32_t want = (length + CLUSTER - 1) / CLUSTER, i, v;
    uint8_t *b;

    *buf = NULL;
    if (want == 0) {
        return 0;
    }
    if (want > c->alloc_end || !(b = malloc((size_t)want * CLUSTER))) {
        return -1;
    }
    for (i = 0; i < want; i++) {
        if (start >= c->alloc_end || fat_get(c, start, &v) != 0 || !(v & FAT_USED) || (v == FAT_END && i + 1 < want)) {
            free(b);
            return -1;
        }
        read_cluster(c, start + c->alloc_off, b + (size_t)i * CLUSTER);
        start = v & ~FAT_USED;
    }
    *buf = b;
    return 0;
}

/* ----------------------------------------------------------- open/close */

static int parse_superblock(Ps2Card *c, char *why, size_t cap)
{
    const uint8_t *sb = c->img;
    uint32_t pages_ecc, i;

    if (c->size < PAGE || memcmp(sb, MAGIC, strlen(MAGIC)) != 0) {
        snprintf(why, cap, "it is not a formatted PS2 memory card (no superblock)");
        return -1;
    }
    if (get16(sb + 0x28) != PAGE || get16(sb + 0x2a) != 2) {
        snprintf(why, cap, "it has %u-byte pages and %u pages a cluster; only 512 and 2 are supported",
                 get16(sb + 0x28), get16(sb + 0x2a));
        return -1;
    }
    c->clusters = get32(sb + 0x30);
    c->alloc_off = get32(sb + 0x34);
    c->alloc_end = get32(sb + 0x38);
    c->root = get32(sb + 0x3c);
    for (i = 0; i < 32; i++) {
        c->ifc[i] = get32(sb + 0x50 + i * 4);
    }
    c->pages = c->clusters * 2;
    pages_ecc = (uint32_t)(c->size / (PAGE + SPARE));
    if (c->size == (size_t)c->pages * (PAGE + SPARE) && pages_ecc == c->pages) {
        c->spare = SPARE;
    } else if (c->size == (size_t)c->pages * PAGE) {
        c->spare = 0;
    } else {
        snprintf(why, cap, "its size (%zu bytes) doesn't match the %u clusters its superblock describes", c->size,
                 c->clusters);
        return -1;
    }
    if (c->clusters == 0 || c->alloc_off >= c->clusters || c->alloc_end > c->clusters - c->alloc_off) {
        snprintf(why, cap, "its superblock is damaged");
        return -1;
    }
    return 0;
}

static int is_dir(const char *path)
{
#ifdef _WIN32
    wchar_t w[1024];
    DWORD a;

    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, w, 1024)) {
        return 0;
    }
    a = GetFileAttributesW(w);
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;

    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}

int ps2mc_open(Ps2Card **out, const char *path, char *why, size_t cap)
{
    Ps2Card *c = calloc(1, sizeof(*c));
    FILE *f;
    int64_t n;

    *out = NULL;
    if (!c) {
        snprintf(why, cap, "out of memory");
        return -1;
    }
    if (is_dir(path)) {
        c->folder = 1;
        snprintf(c->path, sizeof(c->path), "%s", path);
        *out = c;
        return 0;
    }
    f = fs_open(path, "rb");
    if (!f) {
        free(c);
        snprintf(why, cap, "cannot read %s", path);
        return -1;
    }
    n = fs_size(f);
    if (n <= 0 || n > 256 * 1024 * 1024 || fs_seek(f, 0) != 0) {
        fclose(f);
        free(c);
        snprintf(why, cap, "%s is not a memory card image", path);
        return -1;
    }
    c->size = (size_t)n;
    c->img = malloc(c->size);
    if (!c->img || fread(c->img, 1, c->size, f) != c->size) {
        fclose(f);
        ps2mc_close(c);
        snprintf(why, cap, "cannot read %s", path);
        return -1;
    }
    fclose(f);
    if (parse_superblock(c, why, cap) != 0) {
        ps2mc_close(c);
        return -1;
    }
    *out = c;
    return 0;
}

void ps2mc_close(Ps2Card *c)
{
    if (c) {
        free(c->img);
        free(c);
    }
}

int ps2mc_is_folder(const Ps2Card *c)
{
    return c->folder;
}

/* --------------------------------------------------------- directories */

typedef struct {
    uint16_t mode;
    uint32_t length;
    uint8_t created[8];
    uint32_t cluster;
    uint32_t dir_entry;
    uint8_t modified[8];
    char name[SAVE_NAME_MAX];
} Entry;

static void entry_decode(const uint8_t *e, Entry *x)
{
    x->mode = (uint16_t)get16(e);
    x->length = get32(e + 4);
    memcpy(x->created, e + 8, 8);
    x->cluster = get32(e + 0x10);
    x->dir_entry = get32(e + 0x14);
    memcpy(x->modified, e + 0x18, 8);
    memcpy(x->name, e + 0x40, SAVE_NAME_MAX);
    x->name[SAVE_NAME_MAX - 1] = 0;
}

static void entry_encode(uint8_t *e, const Entry *x)
{
    memset(e, 0, ENTRY);
    put16(e, x->mode);
    put32(e + 4, x->length);
    memcpy(e + 8, x->created, 8);
    put32(e + 0x10, x->cluster);
    put32(e + 0x14, x->dir_entry);
    memcpy(e + 0x18, x->modified, 8);
    memcpy(e + 0x40, x->name, strlen(x->name));
}

/* A directory's entries (malloc'd), from its first cluster and entry count. */
static int read_entries(Ps2Card *c, uint32_t cluster, uint32_t count, uint8_t **buf)
{
    if (count == 0 || count > 0x10000) {
        return -1;
    }
    return read_chain(c, cluster, count * ENTRY, buf);
}

static int root_entries(Ps2Card *c, uint8_t **buf, uint32_t *count)
{
    uint8_t first[CLUSTER];

    read_cluster(c, c->root + c->alloc_off, first);
    *count = get32(first + 4);
    if (get16(first) & DF_EXISTS && *count >= 2 && read_entries(c, c->root, *count, buf) == 0) {
        return 0;
    }
    return -1;
}

#ifdef _WIN32
static int list_folder(const char *path, int dirs, char (*names)[SAVE_NAME_MAX], int max)
{
    wchar_t w[1100];
    WIN32_FIND_DATAW fd;
    HANDLE h;
    char pat[1100];
    int n = 0;

    snprintf(pat, sizeof(pat), "%s\\*", path);
    if (!MultiByteToWideChar(CP_UTF8, 0, pat, -1, w, 1100)) {
        return -1;
    }
    h = FindFirstFileW(w, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        return -1;
    }
    do {
        char name[260];
        int isdir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (!WideCharToMultiByte(CP_UTF8, 0, fd.cFileName, -1, name, sizeof(name), NULL, NULL) ||
            isdir != dirs || name[0] == '.' || !strncmp(name, "_pcsx2", 6) || strlen(name) >= SAVE_NAME_MAX) {
            continue;
        }
        if (n < max) {
            snprintf(names[n], SAVE_NAME_MAX, "%s", name);
        }
        n++;
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return n < max ? n : max;
}
#else
static int list_folder(const char *path, int dirs, char (*names)[SAVE_NAME_MAX], int max)
{
    DIR *dp = opendir(path);
    struct dirent *de;
    int n = 0;

    if (!dp) {
        return -1;
    }
    while ((de = readdir(dp)) != NULL) {
        char full[1400];

        if (de->d_name[0] == '.' || !strncmp(de->d_name, "_pcsx2", 6) || strlen(de->d_name) >= SAVE_NAME_MAX) {
            continue;
        }
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        if (is_dir(full) != dirs) {
            continue;
        }
        if (n < max) {
            snprintf(names[n], SAVE_NAME_MAX, "%s", de->d_name);
        }
        n++;
    }
    closedir(dp);
    return n < max ? n : max;
}
#endif

static int name_cmp(const void *a, const void *b)
{
    return strcmp(a, b);
}

int ps2mc_list(Ps2Card *c, char (*names)[SAVE_NAME_MAX], int max)
{
    uint8_t *ents;
    uint32_t count, i;
    int n = 0;

    if (c->folder) {
        n = list_folder(c->path, 1, names, max);
        if (n > 0) {
            qsort(names, (size_t)n, SAVE_NAME_MAX, name_cmp);
        }
        return n;
    }
    if (root_entries(c, &ents, &count) != 0) {
        return -1;
    }
    for (i = 2; i < count && n < max; i++) {
        Entry e;

        entry_decode(ents + i * ENTRY, &e);
        if ((e.mode & DF_EXISTS) && (e.mode & DF_DIR)) {
            snprintf(names[n++], SAVE_NAME_MAX, "%s", e.name);
        }
    }
    free(ents);
    return n;
}

/* the root entry of a directory, or -1 */
static int find_root(Ps2Card *c, const uint8_t *ents, uint32_t count, const char *name, Entry *out)
{
    uint32_t i;

    for (i = 2; i < count; i++) {
        entry_decode(ents + i * ENTRY, out);
        if ((out->mode & DF_EXISTS) && !strcmp(out->name, name)) {
            return (int)i;
        }
    }
    (void)c;
    return -1;
}

static int read_folder_dir(Ps2Card *c, const char *name, SaveDir *d, char *why, size_t cap)
{
    char dir[1100], files[SAVE_MAX_FILES + 1][SAVE_NAME_MAX];
    int n, i;

    snprintf(dir, sizeof(dir), "%s/%s", c->path, name);
    if (!is_dir(dir)) {
        return -1;
    }
    n = list_folder(dir, 0, files, SAVE_MAX_FILES + 1);
    if (n < 0 || n > SAVE_MAX_FILES) {
        snprintf(why, cap, "cannot list %s", dir);
        return -2;
    }
    qsort(files, (size_t)n, SAVE_NAME_MAX, name_cmp);
    save_dir_init(d, name);
    for (i = 0; i < n; i++) {
        char p[1400];
        FILE *f;
        int64_t size, mtime = 0;
        uint8_t *buf, t[8];
        uint64_t st_size;

        f = snprintf(p, sizeof(p), "%s/%s", dir, files[i]) < (int)sizeof(p) ? fs_open(p, "rb") : NULL;
        size = f ? fs_size(f) : -1;
        if (size < 0 || size > 16 * 1024 * 1024 || fs_seek(f, 0) != 0) {
            if (f) {
                fclose(f);
            }
            save_dir_free(d);
            snprintf(why, cap, "cannot read %s", p);
            return -2;
        }
        buf = malloc(size ? (size_t)size : 1);
        if (!buf || fread(buf, 1, (size_t)size, f) != (size_t)size) {
            free(buf);
            fclose(f);
            save_dir_free(d);
            snprintf(why, cap, "cannot read %s", p);
            return -2;
        }
        fclose(f);
        fs_stat(p, &st_size, &mtime);
        save_ps2_time(mtime, t);
        if (i == 0) {
            memcpy(d->created, t, 8);
            memcpy(d->modified, t, 8);
        }
        save_dir_put(d, files[i], SAVE_MODE_FILE, t, t, buf, (uint32_t)size);
        free(buf);
    }
    return 0;
}

int ps2mc_read_dir(Ps2Card *c, const char *name, SaveDir *d, char *why, size_t cap)
{
    uint8_t *ents, *sub;
    uint32_t count, i;
    Entry de;

    memset(d, 0, sizeof(*d));
    if (c->folder) {
        return read_folder_dir(c, name, d, why, cap);
    }
    if (root_entries(c, &ents, &count) != 0) {
        snprintf(why, cap, "its root directory is damaged");
        return -2;
    }
    if (find_root(c, ents, count, name, &de) < 0 || !(de.mode & DF_DIR)) {
        free(ents);
        return -1;
    }
    free(ents);
    if (read_entries(c, de.cluster, de.length, &sub) != 0) {
        snprintf(why, cap, "the directory %s is damaged", name);
        return -2;
    }
    save_dir_init(d, name);
    d->mode = de.mode;
    memcpy(d->created, de.created, 8);
    memcpy(d->modified, de.modified, 8);
    for (i = 2; i < de.length; i++) {
        Entry fe;
        uint8_t *data;

        entry_decode(sub + i * ENTRY, &fe);
        if (!(fe.mode & DF_EXISTS)) {
            continue;
        }
        if ((fe.mode & DF_DIR) || d->nfiles >= SAVE_MAX_FILES) {
            free(sub);
            save_dir_free(d);
            snprintf(why, cap, "the directory %s holds more than a save does", name);
            return -2;
        }
        if (fe.length && read_chain(c, fe.cluster, fe.length, &data) != 0) {
            free(sub);
            save_dir_free(d);
            snprintf(why, cap, "the file %s/%s is damaged", name, fe.name);
            return -2;
        }
        save_dir_put(d, fe.name, fe.mode, fe.created, fe.modified, fe.length ? data : NULL, fe.length);
        if (fe.length) {
            free(data);
        }
    }
    free(sub);
    return 0;
}

int ps2mc_free_kb(Ps2Card *c)
{
    uint32_t i, v;
    int n = 0;

    for (i = 0; i < c->alloc_end; i++) {
        if (fat_get(c, i, &v) == 0 && !(v & FAT_USED)) {
            n++;
        }
    }
    return n;
}

/* --------------------------------------------------------------- writing */

static int alloc_cluster(Ps2Card *c, uint32_t *out)
{
    uint32_t i, v;

    for (i = 0; i < c->alloc_end; i++) {
        if (fat_get(c, i, &v) == 0 && !(v & FAT_USED)) {
            fat_set(c, i, FAT_END);
            *out = i;
            return 0;
        }
    }
    return -1;
}

static void free_chain(Ps2Card *c, uint32_t start)
{
    uint32_t v;
    int guard = 0;

    while (start < c->alloc_end && fat_get(c, start, &v) == 0 && (v & FAT_USED) && guard++ < (int)c->alloc_end) {
        fat_set(c, start, FAT_FREE);
        if (v == FAT_END) {
            return;
        }
        start = v & ~FAT_USED;
    }
}

/* Writes data (len bytes) into new clusters; *first gets the chain's start
   (FAT_END for no data). */
static int write_chain(Ps2Card *c, const uint8_t *data, uint32_t len, uint32_t *first)
{
    uint32_t n = (len + CLUSTER - 1) / CLUSTER, i, prev = 0, cl;
    uint8_t buf[CLUSTER];

    *first = FAT_END;
    for (i = 0; i < n; i++) {
        uint32_t take = len - i * CLUSTER < CLUSTER ? len - i * CLUSTER : CLUSTER;

        if (alloc_cluster(c, &cl) != 0) {
            return -1;
        }
        if (i == 0) {
            *first = cl;
        } else {
            fat_set(c, prev, cl | FAT_USED);
        }
        memset(buf, 0xFF, sizeof(buf));
        memcpy(buf, data + (size_t)i * CLUSTER, take);
        write_cluster(c, cl + c->alloc_off, buf);
        prev = cl;
    }
    return 0;
}

static void now_ps2(uint8_t t[8])
{
    save_ps2_time((int64_t)time(NULL), t);
}

static int zero_time(const uint8_t t[8])
{
    static const uint8_t z[8];

    return !memcmp(t, z, 8);
}

/* writes root entry index (extending the root's chain if it must) */
static int put_root_entry(Ps2Card *c, uint32_t index, const uint8_t *e)
{
    uint32_t cl[4096], want = index / (CLUSTER / ENTRY), nc;
    int n = chain(c, c->root, cl, 4096);
    uint8_t buf[CLUSTER];

    if (n < 0) {
        return -1;
    }
    nc = (uint32_t)n;
    while (want >= nc) {
        uint32_t add;

        if (nc >= 4096 || alloc_cluster(c, &add) != 0) {
            return -1;
        }
        fat_set(c, cl[nc - 1], add | FAT_USED);
        memset(buf, 0, sizeof(buf));
        write_cluster(c, add + c->alloc_off, buf);
        cl[nc++] = add;
    }
    read_cluster(c, cl[want] + c->alloc_off, buf);
    memcpy(buf + index % (CLUSTER / ENTRY) * ENTRY, e, ENTRY);
    write_cluster(c, cl[want] + c->alloc_off, buf);
    return 0;
}

static uint32_t clusters_for(uint32_t bytes)
{
    return (bytes + CLUSTER - 1) / CLUSTER;
}

int ps2mc_write_dir(Ps2Card *c, const SaveDir *d, int replace, char *why, size_t cap)
{
    uint8_t *ents, *dir, e[ENTRY], now[8];
    uint32_t count, need, slot, i, nent = 2 + (uint32_t)d->nfiles, dir_first;
    Entry old, de;
    int found, have;

    if (c->folder) {
        snprintf(why, cap, "export writes card images (.ps2, .mcd); a folder memory card is read only here");
        return -1;
    }
    if (!d->name[0] || strlen(d->name) >= SAVE_NAME_MAX) {
        snprintf(why, cap, "the directory has no name");
        return -1;
    }
    if (root_entries(c, &ents, &count) != 0) {
        snprintf(why, cap, "the card's root directory is damaged");
        return -1;
    }
    found = find_root(c, ents, count, d->name, &old);
    if (found >= 0 && !replace) {
        free(ents);
        snprintf(why, cap, "the card already holds %s (replace it to overwrite it)", d->name);
        return -1;
    }
    /* the space it needs, less what a replaced save frees */
    need = clusters_for(nent * ENTRY);
    for (i = 0; i < (uint32_t)d->nfiles; i++) {
        need += clusters_for(d->files[i].size);
    }
    have = ps2mc_free_kb(c);
    if (found >= 0) {
        uint8_t *sub;

        have += (int)clusters_for(old.length * ENTRY);
        if (read_entries(c, old.cluster, old.length, &sub) == 0) {
            for (i = 2; i < old.length; i++) {
                Entry fe;

                entry_decode(sub + i * ENTRY, &fe);
                if ((fe.mode & DF_EXISTS) && fe.length) {
                    have += (int)clusters_for(fe.length);
                }
            }
            free(sub);
        }
    }
    if ((int)need + 1 > have) {
        free(ents);
        snprintf(why, cap, "the card has %d KB free, and %s needs %u KB", have, d->name, need);
        return -1;
    }
    if (found >= 0) {
        uint8_t *sub;

        if (read_entries(c, old.cluster, old.length, &sub) == 0) {
            for (i = 2; i < old.length; i++) {
                Entry fe;

                entry_decode(sub + i * ENTRY, &fe);
                if ((fe.mode & DF_EXISTS) && fe.length) {
                    free_chain(c, fe.cluster);
                }
            }
            free(sub);
        }
        free_chain(c, old.cluster);
        slot = (uint32_t)found;
    } else {
        /* the first deleted entry, or a new one at the end */
        for (slot = 2; slot < count; slot++) {
            Entry x;

            entry_decode(ents + slot * ENTRY, &x);
            if (!(x.mode & DF_EXISTS)) {
                break;
            }
        }
    }
    now_ps2(now);

    /* the directory's own entries: ".", "..", then the files */
    dir = calloc(clusters_for(nent * ENTRY), CLUSTER);
    if (!dir) {
        free(ents);
        snprintf(why, cap, "out of memory");
        return -1;
    }
    memset(&de, 0, sizeof(de));
    memcpy(de.created, zero_time(d->created) ? now : d->created, 8);
    memcpy(de.modified, zero_time(d->modified) ? now : d->modified, 8);
    {
        Entry dot = de, dotdot;
        Entry root0;

        /* as the BIOS writes them: "." stamped with the directory's creation, ".." with the root's */
        entry_decode(ents, &root0);
        memcpy(dot.modified, dot.created, 8);
        dot.mode = SAVE_MODE_DIR;
        dot.cluster = c->root;
        dot.dir_entry = slot;
        snprintf(dot.name, sizeof(dot.name), ".");
        entry_encode(dir, &dot);
        memset(&dotdot, 0, sizeof(dotdot));
        dotdot.mode = SAVE_MODE_DIR;
        memcpy(dotdot.created, root0.created, 8);
        memcpy(dotdot.modified, root0.created, 8);
        snprintf(dotdot.name, sizeof(dotdot.name), "..");
        entry_encode(dir + ENTRY, &dotdot);
    }
    for (i = 0; i < (uint32_t)d->nfiles; i++) {
        const SaveFile *f = &d->files[i];
        Entry fe;

        memset(&fe, 0, sizeof(fe));
        fe.mode = f->mode ? f->mode : SAVE_MODE_FILE;
        fe.length = f->size;
        memcpy(fe.created, zero_time(f->created) ? de.created : f->created, 8);
        memcpy(fe.modified, zero_time(f->modified) ? de.modified : f->modified, 8);
        snprintf(fe.name, sizeof(fe.name), "%s", f->name);
        if (write_chain(c, f->data, f->size, &fe.cluster) != 0) {
            free(dir);
            free(ents);
            snprintf(why, cap, "the card is full");
            return -1;
        }
        entry_encode(dir + (2 + i) * ENTRY, &fe);
    }
    if (write_chain(c, dir, nent * ENTRY, &dir_first) != 0) {
        free(dir);
        free(ents);
        snprintf(why, cap, "the card is full");
        return -1;
    }
    free(dir);

    de.mode = SAVE_MODE_DIR;
    de.length = nent;
    de.cluster = dir_first;
    snprintf(de.name, sizeof(de.name), "%s", d->name);
    entry_encode(e, &de);
    if (put_root_entry(c, slot, e) != 0) {
        free(ents);
        snprintf(why, cap, "the card's root directory is full");
        return -1;
    }
    if (slot >= count) {
        /* the root's "." counts its entries */
        memcpy(e, ents, ENTRY);
        put32(e + 4, slot + 1);
        put_root_entry(c, 0, e);
    }
    free(ents);
    return 0;
}

/* ---------------------------------------------------------- create/save */

int ps2mc_create(Ps2Card **out, char *why, size_t cap)
{
    /* the BIOS's own format of an 8 MB card, field by field as a card it
       formatted holds them (0x150 on: the card type and flags, then words the
       BIOS keeps there) */
    static const uint8_t sb_tail[0x38] = {
        0x02, 0x2b, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x41, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    };
    const uint32_t clusters = 8192, first_ifc = 8, fat_clusters = 32, alloc_off = 41, alloc_end = 8135;
    Ps2Card *c = calloc(1, sizeof(*c));
    uint8_t *sb, buf[CLUSTER], t[8];
    uint32_t i, n;
    Entry dot, dotdot;

    *out = NULL;
    if (!c) {
        snprintf(why, cap, "out of memory");
        return -1;
    }
    c->spare = SPARE;
    c->pages = clusters * 2;
    c->size = (size_t)c->pages * (PAGE + SPARE);
    c->img = malloc(c->size);
    if (!c->img) {
        free(c);
        snprintf(why, cap, "out of memory");
        return -1;
    }
    memset(c->img, 0xFF, c->size);
    for (i = 0; i < c->pages; i++) {
        page_ecc(c, i);
    }

    sb = page(c, 0);
    memset(sb, 0, 0x150);
    memcpy(sb, MAGIC "1.2.0.0", strlen(MAGIC "1.2.0.0"));
    put16(sb + 0x28, PAGE);
    put16(sb + 0x2a, 2);
    put16(sb + 0x2c, 16);
    put16(sb + 0x2e, 0xFF00);
    put32(sb + 0x30, clusters);
    put32(sb + 0x34, alloc_off);
    put32(sb + 0x38, alloc_end);
    put32(sb + 0x3c, 0);
    put32(sb + 0x40, 1023);
    put32(sb + 0x44, 1022);
    put32(sb + 0x50, first_ifc);
    for (i = 0; i < 32; i++) {
        put32(sb + 0xd0 + i * 4, 0xFFFFFFFFu);
    }
    memcpy(sb + 0x150, sb_tail, sizeof(sb_tail));
    page_ecc(c, 0);
    if (parse_superblock(c, why, cap) != 0) {
        ps2mc_close(c);
        return -1;
    }

    /* the indirect FAT: the FAT's 32 clusters, after it */
    memset(buf, 0xFF, sizeof(buf));
    for (i = 0; i < fat_clusters; i++) {
        put32(buf + i * 4, first_ifc + 1 + i);
    }
    write_cluster(c, first_ifc, buf);
    /* the FAT: everything free, the root's cluster in use, nothing past alloc_end */
    for (n = 0; n < fat_clusters; n++) {
        for (i = 0; i < EPC; i++) {
            uint32_t idx = n * EPC + i;

            put32(buf + i * 4, idx == 0 || idx >= alloc_end ? FAT_END : FAT_FREE);
        }
        write_cluster(c, first_ifc + 1 + n, buf);
    }
    /* the root directory */
    now_ps2(t);
    memset(&dot, 0, sizeof(dot));
    dot.mode = SAVE_MODE_DIR;
    dot.length = 2;
    memcpy(dot.created, t, 8);
    memcpy(dot.modified, t, 8);
    snprintf(dot.name, sizeof(dot.name), ".");
    dotdot = dot;
    dotdot.mode = 0xa426;
    dotdot.length = 0;
    snprintf(dotdot.name, sizeof(dotdot.name), "..");
    memset(buf, 0, sizeof(buf));
    entry_encode(buf, &dot);
    entry_encode(buf + ENTRY, &dotdot);
    write_cluster(c, alloc_off, buf);
    *out = c;
    return 0;
}

int ps2mc_save(Ps2Card *c, const char *path, char *why, size_t cap)
{
    char tmp[1100];
    FILE *f;
    int ok;

    if (c->folder) {
        snprintf(why, cap, "a folder memory card is read only here");
        return -1;
    }
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);
    f = fs_open(tmp, "wb");
    if (!f) {
        snprintf(why, cap, "cannot write %s", tmp);
        return -1;
    }
    ok = fwrite(c->img, 1, c->size, f) == c->size;
    ok = fflush(f) == 0 && ok;
    ok = fclose(f) == 0 && ok;
    if (!ok || fs_replace(tmp, path) != 0) {
        fs_remove(tmp);
        snprintf(why, cap, "cannot write %s", path);
        return -1;
    }
    return 0;
}
