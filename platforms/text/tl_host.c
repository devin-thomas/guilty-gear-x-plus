/* The text layer bound to a Core host's disc (tl_host.h). */
#include "tl_host.h"

#include <string.h>

/* file.c's seekpos: {sector offset, packed size} per file, the offsets
   rising, and one more entry after the last file to end it */
#define MAX_FILES 1024

static int read_disc(void *ctx, uint32_t lsn, uint32_t count, void *dst)
{
    return disc_read(ctx, lsn, count, dst);
}

/* CDDATA.BIN's sector and the number of files in the boot ELF's seekpos. */
static int disc_files(CoreHost *h, uint32_t *lsn, uint32_t *nfiles, char *err, size_t cap)
{
    /* read only, straight from the Core's memory (ee_ptr would mark the
       pages changed) */
    const uint8_t *seek = h->mem + ggx_sym_seekpos;
    uint32_t size, n, prev = 0;

    if (disc_find(&h->disc, "\\CDDATA.BIN;1", lsn, &size) != 0) {
        snprintf(err, cap, "the disc has no CDDATA.BIN");
        return -1;
    }
    for (n = 0; n + 1 < MAX_FILES; n++) {
        uint32_t off = seek[8 * n] | (uint32_t)seek[8 * n + 1] << 8 | (uint32_t)seek[8 * n + 2] << 16 |
                       (uint32_t)seek[8 * n + 3] << 24;

        if ((n && off <= prev) || off > size / 2048) {
            break;
        }
        prev = off;
    }
    /* n entries: n - 1 files */
    *nfiles = n ? n - 1 : 0;
    return 0;
}

int tl_bind_host(TextLayer *t, CoreHost *h, char *err, size_t cap)
{
    uint32_t lsn, n;

    if (disc_files(h, &lsn, &n, err, cap) != 0) {
        return -1;
    }
    return tl_bind(t, read_disc, &h->disc, lsn, h->mem + ggx_sym_seekpos, n, err, cap);
}

int tl_host_picture(CoreHost *h, uint32_t fid, const char *path, TlRawPicture *out)
{
    uint32_t lsn, n;
    char err[64];

    if (disc_files(h, &lsn, &n, err, sizeof(err)) != 0) {
        return -1;
    }
    return tl_read_picture(read_disc, &h->disc, lsn, h->mem + ggx_sym_seekpos, n, fid, path, out);
}
