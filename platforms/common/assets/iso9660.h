/* ISO 9660 on any source of 2048-byte sectors: the volume descriptor, path
 * lookup and a listing of every file. */
#ifndef GGX_ISO9660_H
#define GGX_ISO9660_H

#include <stddef.h>
#include <stdint.h>

/* Reads count 2048-byte sectors from lsn; 0 on success. */
typedef int (*SectorRead)(void *src, uint32_t lsn, uint32_t count, void *dst);

typedef struct {
    char path[128];       /* "CDDATA.BIN", "DIR/FILE.EXT": no ";1", '/' between parts */
    uint32_t lsn;
    uint32_t size;
} IsoFile;

/* 0 if sector 16 is a primary volume descriptor; volume_id gets its
   trimmed volume identifier (33 bytes is enough), sectors its size. */
int iso_volume(SectorRead rd, void *src, char *volume_id, uint32_t *sectors);
/* Find a file by path ("\\CDDATA.BIN;1", "SYSTEM.CNF"). 0 on success. */
int iso_find(SectorRead rd, void *src, const char *path, uint32_t *lsn, uint32_t *size);
/* Every file in the tree, in directory order. Returns how many there are
   (only the first max are stored), or -1 if the tree can't be read. */
int iso_list(SectorRead rd, void *src, IsoFile *files, int max);
/* 1 if two paths name the same file: case-insensitive, '/' and '\\' alike,
   a leading separator, a ";1" version and a trailing '.' optional. */
int iso_path_eq(const char *a, const char *b);

#endif
