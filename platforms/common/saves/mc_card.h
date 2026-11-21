/* The memory card the libmc shim answers from (PORT-012).
 *
 * A formatted 8 MB PS2 card holding the game's save directories. Backed by
 * the native save store, each directory is read from
 * <store>/<directory>.ggxsave when the card is opened and written back,
 * atomically, whenever the game finishes changing it (a file closed after
 * writing, a delete, a new directory). Without a store it is an empty card
 * that lives in memory for the run.
 *
 * The card is outside the Core, like the disc: snapshots don't hold it
 * (a rollback session keeps it beside them: common/host/session.h).
 * The shim's own libmc state (the current directory, open files) is in EE
 * RAM (shim_mc.c).
 */
#ifndef GGX_MC_CARD_H
#define GGX_MC_CARD_H

#include <stdint.h>
#include <stdio.h>

#include "save_file.h"

/* libmc's results (sceMcRes*) */
enum {
    MC_RES_OK = 0,
    MC_RES_NO_FORMAT = -2,
    MC_RES_FULL = -3,
    MC_RES_NO_ENTRY = -4,     /* not found; for sceMcMkdir, already there */
    MC_RES_DENIED = -5,
    MC_RES_NOT_EMPTY = -6,
};

/* what an empty card reports free, in KB (PORT-007) */
#define MC_CARD_CAPACITY_KB 8000

typedef struct McCard McCard;

/* store_dir NULL: an empty card in memory. release picks the directories
   read from the store (SAVE_RELEASE_*). Messages about saves the card
   can't read go to msg. */
McCard *mc_card_open(const char *store_dir, uint32_t release, FILE *msg);
void mc_card_close(McCard *c);
/* Extensions or mods active in this run: every directory the game writes
   records them (save_dir_touch). */
void mc_card_set_extensions(McCard *c, const char *const *ids, const uint32_t *flags, int n);

int mc_card_free_kb(const McCard *c);
int mc_card_count(const McCard *c);
SaveDir *mc_card_at(McCard *c, int i);
/* The directory's index, or -1. */
int mc_card_find(const McCard *c, const char *name);

int mc_card_mkdir(McCard *c, const char *name);
/* Deletes a directory, which must hold no files. */
int mc_card_rmdir(McCard *c, int dir);
/* Creates an empty file. */
int mc_card_create(McCard *c, int dir, const char *name);
int mc_card_unlink(McCard *c, int dir, int file);
/* Writes len bytes at pos, growing the file. MC_RES_FULL if it doesn't fit. */
int mc_card_write(McCard *c, int dir, int file, uint32_t pos, const uint8_t *data, uint32_t len);
/* Writes the directory to the store (or removes its file if it is gone). */
int mc_card_commit(McCard *c, const char *name);

/* Sessions (PORT-013: rollback, replays). A detached card keeps what it
   holds in memory: nothing the game does reaches the store any more, and
   the times it records are zero, so two instances' cards are the same
   bytes. mc_card_changes counts every change to what the card holds;
   mc_card_assign makes dst hold what src holds (directories, unreadable
   names, extensions; a change, so its count goes up), mc_card_clone is a new detached
   card holding the same; mc_card_hash is a hash of the directories and
   files, names, modes and bytes, without the times. */
void mc_card_detach(McCard *c);
uint32_t mc_card_changes(const McCard *c);
int mc_card_assign(McCard *dst, const McCard *src);
McCard *mc_card_clone(const McCard *src);
uint64_t mc_card_hash(const McCard *c);

#endif
