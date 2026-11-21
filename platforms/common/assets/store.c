/* The extracted assets, read as the release's disc. */
#include <stdlib.h>
#include <string.h>

#include "assets.h"
#include "fsys.h"

#define MAX_FILES 32

struct AssetStore {
    const Release *r;
    char dir[1024];
    FILE *f[MAX_FILES];    /* opened on first read */
};

void asset_store_close(AssetStore *s)
{
    unsigned i;

    if (!s) {
        return;
    }
    for (i = 0; i < s->r->nfiles; i++) {
        if (s->f[i]) {
            fclose(s->f[i]);
        }
    }
    free(s);
}

/* The manifest's mtime for each of r's files; 0 if it is r's manifest. */
static int read_manifest(const char *dir, const Release *r, int64_t *mtimes)
{
    char path[1100], line[512], id[8], sha[65], name[128];
    unsigned seen = 0, i, lsn, size;
    long long mtime;
    int header = 0, release = 0;
    FILE *f;

    fs_join(path, sizeof(path), dir, "manifest.txt", NULL);
    f = fs_open(path, "rb");
    if (!f) {
        return -1;
    }
    while (fgets(line, sizeof(line), f)) {
        if (!strcmp(line, "ggx-assets 1\n")) {
            header = 1;
        } else if (sscanf(line, "release %7s", id) == 1) {
            release = !strcmp(id, r->id);
        } else if (sscanf(line, "file %u %u %lld %64s %127s", &lsn, &size, &mtime, sha, name) == 5) {
            for (i = 0; i < r->nfiles; i++) {
                const ReleaseFile *rf = &r->files[i];

                if (!strcmp(rf->path, name) && rf->lsn == lsn && rf->size == size && !strcmp(rf->sha256, sha)) {
                    mtimes[i] = (int64_t)mtime;
                    seen |= 1u << i;
                }
            }
        }
    }
    fclose(f);
    return header && release && seen == (1u << r->nfiles) - 1 ? 0 : -1;
}

int asset_store_open(AssetStore **out, const char *data_dir, const Release *r, FILE *msg)
{
    AssetStore *s = calloc(1, sizeof(*s));
    int64_t mtimes[MAX_FILES];
    unsigned i;

    *out = NULL;
    if (!s || r->nfiles > MAX_FILES) {
        free(s);
        return -1;
    }
    s->r = r;
    if (assets_dir(s->dir, sizeof(s->dir), data_dir, r) != 0) {
        free(s);
        return -1;
    }
    if (read_manifest(s->dir, r, mtimes) != 0) {
        fprintf(msg, "ggx: no extracted %s assets in %s. Extract them from your disc first:\n"
                     "  ggx-headless --extract <your %s disc image>\n", r->title, s->dir, r->serial);
        free(s);
        return -1;
    }
    for (i = 0; i < r->nfiles; i++) {
        const ReleaseFile *f = &r->files[i];
        char path[1100], hex[65];
        uint64_t size;
        int64_t mtime;

        fs_join(path, sizeof(path), s->dir, f->path, NULL);
        if (fs_stat(path, &size, &mtime) != 0 || size != f->size ||
            (mtime != mtimes[i] && (assets_hash_file(path, hex) != 0 || strcmp(hex, f->sha256) != 0))) {
            fprintf(msg, "ggx: the extracted %s is missing or damaged. Extract the assets again to repair them:\n"
                         "  ggx-headless --extract <your %s disc image>\n", path, r->serial);
            free(s);
            return -1;
        }
    }
    *out = s;
    return 0;
}

int asset_store_find(AssetStore *s, const char *path, uint32_t *lsn, uint32_t *size)
{
    const ReleaseFile *f = release_file(s->r, path);

    if (!f) {
        return -1;
    }
    *lsn = f->lsn;
    *size = f->size;
    return 0;
}

int asset_store_read(void *src, uint32_t lsn, uint32_t count, void *dst)
{
    AssetStore *s = src;
    uint8_t *out = dst;

    while (count) {
        const ReleaseFile *f = NULL;
        uint32_t i, sectors, n;
        uint64_t off, avail;

        for (i = 0; i < s->r->nfiles; i++) {
            f = &s->r->files[i];
            if (lsn >= f->lsn && lsn - f->lsn < (f->size + 2047u) / 2048) {
                break;
            }
        }
        if (i == s->r->nfiles) {
            return -1;     /* not in a file: nothing the game reads */
        }
        if (!s->f[i]) {
            char path[1100];

            fs_join(path, sizeof(path), s->dir, f->path, NULL);
            s->f[i] = fs_open(path, "rb");
            if (!s->f[i]) {
                return -1;
            }
        }
        sectors = (f->size + 2047) / 2048 - (lsn - f->lsn);
        n = count < sectors ? count : sectors;
        off = (uint64_t)(lsn - f->lsn) * 2048;
        avail = f->size - off < (uint64_t)n * 2048 ? f->size - off : (uint64_t)n * 2048;
        if (fs_seek(s->f[i], off) != 0 || fread(out, 1, (size_t)avail, s->f[i]) != avail) {
            return -1;
        }
        memset(out + avail, 0, (size_t)n * 2048 - (size_t)avail);
        out += (size_t)n * 2048;
        lsn += n;
        count -= n;
    }
    return 0;
}
