#include "disc.h"

#include "iso9660.h"

void disc_close(Disc *d)
{
    asset_store_close(d->store);
    disc_image_close(d->image);
    d->store = NULL;
    d->image = NULL;
}

int disc_read(Disc *d, uint32_t lsn, uint32_t count, void *dst)
{
    return d->store ? asset_store_read(d->store, lsn, count, dst) : disc_image_read(d->image, lsn, count, dst);
}

int disc_find(Disc *d, const char *path, uint32_t *lsn, uint32_t *size)
{
    if (d->store) {
        return asset_store_find(d->store, path, lsn, size);
    }
    return iso_find(disc_image_read, d->image, path, lsn, size);
}
