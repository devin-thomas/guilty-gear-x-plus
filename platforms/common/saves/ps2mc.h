/* PS2 memory card images (PORT-012): PCSX2's .ps2 and .mcd files and its
 * folder memory cards.
 *
 * A card image is the card's flash, page by page: 512 data bytes, and on
 * cards with ECC (PCSX2's 8 MB .ps2 and .mcd are) 16 spare bytes holding a
 * 3-byte Hamming code for each 128-byte quarter of the page. The file
 * system is the BIOS's (mcman): a superblock in page 0, an indirect FAT
 * whose clusters list the FAT's clusters, the FAT (one 32-bit entry per
 * cluster: bit 31 in use, the low 31 bits the next cluster, 0xFFFFFFFF the
 * end of a chain, 0x7FFFFFFF free), and directories of 512-byte entries
 * starting with the root's. A save is a directory in the root.
 *
 * A folder memory card is a host folder PCSX2 presents as a card: one
 * folder per save, holding its files. Those are read only here.
 */
#ifndef GGX_PS2MC_H
#define GGX_PS2MC_H

#include <stddef.h>
#include <stdint.h>

#include "save_file.h"

typedef struct Ps2Card Ps2Card;

/* Opens a card image (read into memory) or a folder memory card. */
int ps2mc_open(Ps2Card **out, const char *path, char *why, size_t cap);
/* A new 8 MB card with ECC, formatted as the BIOS formats one. */
int ps2mc_create(Ps2Card **out, char *why, size_t cap);
void ps2mc_close(Ps2Card *c);
int ps2mc_is_folder(const Ps2Card *c);

/* The directories in the root (deleted ones and "." and ".." left out). */
int ps2mc_list(Ps2Card *c, char (*names)[SAVE_NAME_MAX], int max);
/* Reads a root directory and its files. d->release is set from its name.
   0, -1 if there is none of that name, -2 if the card is damaged (why). */
int ps2mc_read_dir(Ps2Card *c, const char *name, SaveDir *d, char *why, size_t cap);
/* Writes d as a root directory. If one of that name exists it is replaced
   when replace is set, and refused otherwise. 0, or -1 and why. */
int ps2mc_write_dir(Ps2Card *c, const SaveDir *d, int replace, char *why, size_t cap);
/* Free space in KB (clusters), as the BIOS counts it. */
int ps2mc_free_kb(Ps2Card *c);
/* Writes the image to path through a temporary file and a rename. */
int ps2mc_save(Ps2Card *c, const char *path, char *why, size_t cap);

/* The Hamming code of a 128-byte quarter page, as mcman computes it. */
void ps2mc_ecc(const uint8_t *chunk128, uint8_t out[3]);

#endif
