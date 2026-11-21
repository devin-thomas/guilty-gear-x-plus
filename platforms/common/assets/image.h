/* The player's disc image, read as 2048-byte user-data sectors.
 *
 * Formats, recognised by content rather than by extension:
 *   ISO       2048-byte sectors;
 *   BIN       raw 2352-byte sectors (Mode 1, or Mode 2 Form 1 as PS2 CDs
 *             are), with or without its CUE sheet;
 *   CUE       the sheet naming a BIN: MODE1/2048, MODE1/2352, MODE2/2336
 *             or MODE2/2352 for track 1;
 *   CHD       MAME's compressed format (libchdr), either a CD (chdman
 *             createcd: its first track) or a DVD (chdman createdvd).
 */
#ifndef GGX_IMAGE_H
#define GGX_IMAGE_H

#include <stddef.h>
#include <stdint.h>

typedef struct DiscImage DiscImage;

/* 0 on success. On failure err says why, for the player. */
int disc_image_open(DiscImage **out, const char *path, char *err, size_t errcap);
void disc_image_close(DiscImage *img);
/* "ISO", "BIN (MODE2/2352)", "CHD (CD, MODE2_RAW)", "CHD (DVD)", ... */
const char *disc_image_format(const DiscImage *img);
/* The sectors the image holds. */
uint32_t disc_image_sectors(const DiscImage *img);
/* Reads count 2048-byte user-data sectors from lsn. 0 on success. Its
   signature is a SectorRead (iso9660.h). */
int disc_image_read(void *img, uint32_t lsn, uint32_t count, void *dst);

#endif
