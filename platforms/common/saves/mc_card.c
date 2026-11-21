#include "mc_card.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fsys.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

#define MAX_DIRS 16
#define MAX_LOCKED 16
#define CLUSTER 1024u

struct McCard {
    char store[1024];         /* empty: no store */
    FILE *msg;
    int ndirs;
    SaveDir dirs[MAX_DIRS];
    /* directories in the store this build can't read: never shown, never overwritten */
    int nlocked;
    char locked[MAX_LOCKED][SAVE_NAME_MAX];
    int next;
    char ext[SAVE_MAX_EXTENSIONS][SAVE_EXT_ID_MAX];
    uint32_t ext_flags[SAVE_MAX_EXTENSIONS];
    uint32_t changes;         /* counts every change to what the card holds */
    int detached;             /* a session's card: nothing reaches the store, times are zero */
};

static int store_path(const McCard *c, const char *name, char *out, size_t cap)
{
    return save_store_path(out, cap, c->store, name);
}

McCard *mc_card_open(const char *store_dir, uint32_t release, FILE *msg)
{
    McCard *c = calloc(1, sizeof(*c));
    const char *sys = save_system_dir(release);
    int i;

    if (!c) {
        return NULL;
    }
    c->msg = msg;
    if (!store_dir || !sys) {
        return c;
    }
    snprintf(c->store, sizeof(c->store), "%s", store_dir);
    /* the system directory, then the ten replays */
    for (i = -1; i < 10; i++) {
        char name[SAVE_NAME_MAX], path[1100], why[256];
        uint64_t size;
        SaveDir d;
        int rc;

        if (i < 0) {
            snprintf(name, sizeof(name), "%s", sys);
        } else {
            snprintf(name, sizeof(name), "%sRP%d", sys, i);
        }
        if (store_path(c, name, path, sizeof(path)) != 0 || fs_stat(path, &size, NULL) != 0) {
            continue;
        }
        rc = save_read(path, &d, NULL, why, sizeof(why));
        if (rc == SAVE_OK && (strcmp(d.name, name) != 0 || d.release != release)) {
            snprintf(why, sizeof(why), "it holds %s for %s", d.name, save_release_name(d.release));
            save_dir_free(&d);
            rc = SAVE_EBAD;
        }
        if (rc != SAVE_OK) {
            if (msg) {
                fprintf(msg, "ggx: the save %s can't be used: %s. The game won't see it, and won't overwrite it.\n",
                        path, why);
            }
            snprintf(c->locked[c->nlocked++], SAVE_NAME_MAX, "%s", name);
            continue;
        }
        c->dirs[c->ndirs++] = d;
    }
    return c;
}

void mc_card_close(McCard *c)
{
    int i;

    if (!c) {
        return;
    }
    for (i = 0; i < c->ndirs; i++) {
        save_dir_free(&c->dirs[i]);
    }
    free(c);
}

void mc_card_set_extensions(McCard *c, const char *const *ids, const uint32_t *flags, int n)
{
    int i;

    c->next = 0;
    for (i = 0; i < n && i < SAVE_MAX_EXTENSIONS; i++) {
        snprintf(c->ext[i], SAVE_EXT_ID_MAX, "%s", ids[i]);
        c->ext_flags[i] = flags ? flags[i] : 0;
        c->next++;
    }
}

static uint32_t clusters(uint32_t bytes)
{
    return (bytes + CLUSTER - 1) / CLUSTER;
}

/* a directory's clusters: its entries (".", "..", the files) and its files' data */
static int dir_kb(const SaveDir *d)
{
    uint32_t kb = clusters((2 + (uint32_t)d->nfiles) * 512u);
    int i;

    for (i = 0; i < d->nfiles; i++) {
        kb += clusters(d->files[i].size);
    }
    return (int)kb;
}

int mc_card_free_kb(const McCard *c)
{
    int i, kb = MC_CARD_CAPACITY_KB;

    for (i = 0; i < c->ndirs; i++) {
        kb -= dir_kb(&c->dirs[i]);
    }
    return kb > 0 ? kb : 0;
}

int mc_card_count(const McCard *c)
{
    return c->ndirs;
}

SaveDir *mc_card_at(McCard *c, int i)
{
    return i >= 0 && i < c->ndirs ? &c->dirs[i] : NULL;
}

int mc_card_find(const McCard *c, const char *name)
{
    int i;

    for (i = 0; i < c->ndirs; i++) {
        if (!strcmp(c->dirs[i].name, name)) {
            return i;
        }
    }
    return -1;
}

static int is_locked(const McCard *c, const char *name)
{
    int i;

    for (i = 0; i < c->nlocked; i++) {
        if (!strcmp(c->locked[i], name)) {
            return 1;
        }
    }
    return 0;
}

static void stamp(const McCard *c, uint8_t t[8])
{
    /* host time, for the store and an exported card only: the Core never sees it (shim_mc.c). A session's
       card keeps zero, so two instances' cards hold the same bytes. */
    if (c->detached) {
        memset(t, 0, 8);
    } else {
        save_ps2_time((int64_t)time(NULL), t);
    }
}

int mc_card_mkdir(McCard *c, const char *name)
{
    SaveDir *d;

    if (mc_card_find(c, name) >= 0) {
        return MC_RES_NO_ENTRY;
    }
    if (is_locked(c, name) || strlen(name) >= SAVE_NAME_MAX) {
        return MC_RES_DENIED;
    }
    if (c->ndirs >= MAX_DIRS || mc_card_free_kb(c) < 1) {
        return MC_RES_FULL;
    }
    d = &c->dirs[c->ndirs++];
    c->changes++;
    save_dir_init(d, name);
    stamp(c, d->created);
    memcpy(d->modified, d->created, 8);
    return MC_RES_OK;
}

int mc_card_rmdir(McCard *c, int dir)
{
    if (dir < 0 || dir >= c->ndirs) {
        return MC_RES_NO_ENTRY;
    }
    if (c->dirs[dir].nfiles) {
        return MC_RES_NOT_EMPTY;
    }
    c->changes++;
    save_dir_free(&c->dirs[dir]);
    memmove(&c->dirs[dir], &c->dirs[dir + 1], sizeof(c->dirs[0]) * (size_t)(c->ndirs - dir - 1));
    c->ndirs--;
    return MC_RES_OK;
}

int mc_card_create(McCard *c, int dir, const char *name)
{
    SaveDir *d = mc_card_at(c, dir);
    uint8_t t[8];

    if (!d) {
        return MC_RES_NO_ENTRY;
    }
    if (d->nfiles >= SAVE_MAX_FILES || mc_card_free_kb(c) < 1) {
        return MC_RES_FULL;
    }
    stamp(c, t);
    c->changes++;
    return save_dir_put(d, name, SAVE_MODE_FILE, t, t, NULL, 0) == 0 ? MC_RES_OK : MC_RES_DENIED;
}

int mc_card_unlink(McCard *c, int dir, int file)
{
    SaveDir *d = mc_card_at(c, dir);

    if (!d || file < 0 || file >= d->nfiles) {
        return MC_RES_NO_ENTRY;
    }
    c->changes++;
    save_dir_remove(d, file);
    stamp(c, d->modified);
    return MC_RES_OK;
}

int mc_card_write(McCard *c, int dir, int file, uint32_t pos, const uint8_t *data, uint32_t len)
{
    SaveDir *d = mc_card_at(c, dir);
    SaveFile *f;
    uint32_t end;

    if (!d || file < 0 || file >= d->nfiles) {
        return MC_RES_NO_ENTRY;
    }
    f = &d->files[file];
    end = pos + len;
    if (end < pos) {
        return MC_RES_FULL;
    }
    if (end > f->size) {
        uint8_t *grown;

        if ((int)(clusters(end) - clusters(f->size)) > mc_card_free_kb(c)) {
            return MC_RES_FULL;
        }
        grown = realloc(f->data, end);
        if (!grown) {
            return MC_RES_FULL;
        }
        memset(grown + f->size, 0, end - f->size);
        f->data = grown;
        f->size = end;
    }
    if (len) {
        memcpy(f->data + pos, data, len);
    }
    c->changes++;
    stamp(c, f->modified);
    memcpy(d->modified, f->modified, 8);
    return MC_RES_OK;
}

int mc_card_commit(McCard *c, const char *name)
{
    char path[1100], why[256];
    int i = mc_card_find(c, name);
    SaveDir *d;

    /* only the game's own directories are kept */
    if (!c->store[0] || c->detached || !save_release_of(name)) {
        return 0;
    }
    if (store_path(c, name, path, sizeof(path)) != 0) {
        return -1;
    }
    if (i < 0) {
        uint64_t size;

        return fs_stat(path, &size, NULL) == 0 ? fs_remove(path) : 0;
    }
    d = &c->dirs[i];
    for (i = 0; i < c->next; i++) {
        save_dir_touch(d, c->ext[i], c->ext_flags[i]);
    }
    if (fs_mkdirs(c->store) != 0 || save_write(path, d, why, sizeof(why)) != SAVE_OK) {
        if (c->msg) {
            fprintf(c->msg, "ggx: could not save %s: %s\n", name, why);
        }
        return -1;
    }
    return 0;
}

/* ---- sessions (PORT-013) ---- */

void mc_card_detach(McCard *c)
{
    c->detached = 1;
}

uint32_t mc_card_changes(const McCard *c)
{
    return c->changes;
}

static void free_dirs(McCard *c)
{
    int i;

    for (i = 0; i < c->ndirs; i++) {
        save_dir_free(&c->dirs[i]);
    }
    c->ndirs = 0;
}

int mc_card_assign(McCard *dst, const McCard *src)
{
    int i;

    if (dst == src) {
        return 0;
    }
    free_dirs(dst);
    for (i = 0; i < src->ndirs; i++) {
        if (save_dir_copy(&dst->dirs[i], &src->dirs[i]) != 0) {
            dst->ndirs = i;
            return -1;
        }
        dst->ndirs = i + 1;
    }
    dst->nlocked = src->nlocked;
    memcpy(dst->locked, src->locked, sizeof(dst->locked));
    dst->next = src->next;
    memcpy(dst->ext, src->ext, sizeof(dst->ext));
    memcpy(dst->ext_flags, src->ext_flags, sizeof(dst->ext_flags));
    /* a change like any other: the count only goes up, so an equal count means nothing changed */
    dst->changes++;
    return 0;
}

McCard *mc_card_clone(const McCard *src)
{
    McCard *c = calloc(1, sizeof(*c));

    if (!c) {
        return NULL;
    }
    c->msg = src->msg;
    c->detached = 1;
    if (mc_card_assign(c, src) != 0) {
        mc_card_close(c);
        return NULL;
    }
    return c;
}

static void hash_u32(XXH3_state_t *x, uint32_t v)
{
    uint8_t b[4] = { (uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24) };

    XXH3_64bits_update(x, b, 4);
}

uint64_t mc_card_hash(const McCard *c)
{
    XXH3_state_t x;
    int i, j;

    XXH3_64bits_reset(&x);
    hash_u32(&x, (uint32_t)c->ndirs);
    for (i = 0; i < c->ndirs; i++) {
        const SaveDir *d = &c->dirs[i];

        XXH3_64bits_update(&x, d->name, SAVE_NAME_MAX);
        hash_u32(&x, d->mode);
        hash_u32(&x, (uint32_t)d->nfiles);
        for (j = 0; j < d->nfiles; j++) {
            const SaveFile *f = &d->files[j];

            XXH3_64bits_update(&x, f->name, SAVE_NAME_MAX);
            hash_u32(&x, f->mode);
            hash_u32(&x, f->size);
            if (f->size) {
                XXH3_64bits_update(&x, f->data, f->size);
            }
        }
    }
    return XXH3_64bits_digest(&x);
}
