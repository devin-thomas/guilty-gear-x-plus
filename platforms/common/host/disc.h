/* The disc the Core reads: the player's extracted assets (common/assets/),
 * or, for development, the player's image itself. Both read as the
 * release's disc, by logical sector, with ISO 9660 lookup by path; the
 * Core only ever reads through these calls, and its log is the same from
 * either. */
#ifndef GGX_DISC_H
#define GGX_DISC_H

#include <stdint.h>

#include "assets.h"

typedef struct {
    AssetStore *store;    /* the extracted assets */
    DiscImage *image;     /* --disc: straight from the image */
} Disc;

void disc_close(Disc *d);
/* 0 on success; reads count 2048-byte user-data sectors from lsn. */
int disc_read(Disc *d, uint32_t lsn, uint32_t count, void *dst);
/* Find a file by its ISO 9660 path ("\CDDATA.BIN;1", "SYSTEM.CNF").
   Case-insensitive; a ";1" version is optional. 0 on success. */
int disc_find(Disc *d, const char *path, uint32_t *lsn, uint32_t *size);

#endif
