/* Replays (PORT-013): a play session's input record, kept as a file and
 * played back to the same State Hashes on any OS.
 *
 * A replay starts at the boot and holds everything that decides what the
 * Core computes from there (session.h): every field's input record (both
 * pads as libpad delivers them, and the unlocks asked for), the unlocks
 * asked for from the start, what each memory card slot held at the boot,
 * and the extensions or mods active. It also holds the State Hash every
 * RP_CHECK_EVERY fields and after the last, so playback knows the moment it
 * goes astray, and marks: the fields where each fight started and ended,
 * which a player's playback skips to.
 *
 * No game data: the inputs, the game's own save data files the player's
 * card held (settings and progress, as the game wrote them), and hashes.
 * The other files on the card (icon.bin, icon.sys: the disc's) are kept as
 * names and sizes only, and play back as zeros of that size; the game never
 * reads them, and the checkpoints would say if it did.
 *
 * A replay plays only on the build it was recorded with: the same release,
 * boot ELF, State Hash version and Core build (the Core build id, STATE-
 * HASH.md section 1: every OS builds the same Core from the pinned
 * compiler, so a replay recorded on one OS plays on the others), with the
 * same extensions active. rp_compatible refuses any other, naming the
 * cause. This is format version 1.
 */
#ifndef GGX_REPLAYS_H
#define GGX_REPLAYS_H

#include <stddef.h>
#include <stdint.h>

#include "session.h"

#define RP_FORMAT_VERSION 1u
#define RP_CHECK_EVERY 60u
#define RP_MAX_EXTENSIONS 16
#define RP_EXT_ID_MAX 64

/* marks */
enum { RP_MARK_FIGHT = 1, RP_MARK_FIGHT_END = 2 };

/* What a replay must match to play: this build's, or the file's. */
typedef struct {
    uint32_t release;           /* 1 KR (SLPM-645.25), 2 JP (SLPS-201.37) */
    uint8_t elf_sha256[32];
    uint32_t state_hash_version;
    uint64_t core_build;
    int n_extensions;
    char extensions[RP_MAX_EXTENSIONS][RP_EXT_ID_MAX];
} RpBuild;

typedef struct Replay Replay;

/* This host's build, with the extensions active. */
void rp_build_of(CoreHost *h, const char *const *extensions, int n, RpBuild *out);

/* Recording. Starts at the boot (the host's field 0), before the first
   field: takes what the cards hold and the unlocks asked for from the
   start, and from here every field's input through the session's
   on_input (which it takes). */
Replay *rp_record(Session *s, int unlocks, const char *const *extensions, int n_extensions);
/* A mark at the field about to run. */
void rp_mark(Replay *r, uint32_t kind);
/* After each field: marks where a fight (GameMain) started and ended.
   Returns the mark made, or 0. Does nothing for NULL. */
int rp_watch(Replay *r, CoreHost *h);
/* Writes every field so far (atomically: a temporary file renamed over
   path), with the State Hash now as the last check. 0, or -1 with why. */
int rp_write(Replay *r, const char *path, char *why, size_t cap);

/* Playback. NULL, with why, if the file isn't a replay this format reads
   or is damaged. */
Replay *rp_open(const char *path, char *why, size_t cap);
/* 0 if it plays on build; otherwise nonzero, and why names the cause. */
int rp_compatible(const Replay *r, const RpBuild *build, char *why, size_t cap);
/* How the replay's host must be opened: the card slots (CORE_CARD_EMPTY
   or _NONE) and the extensions. */
void rp_host_config(const Replay *r, int cards[2], const char *ext_ids[RP_MAX_EXTENSIONS], int *n_ext);
/* After core_host_open, before the boot: the session as recorded (cards
   detached and holding what they held, the unlocks from the start). */
int rp_start(Replay *r, Session *s);
/* The input of field f (the one that starts at host field f), or NULL past
   the end. */
const SessionInput *rp_input(const Replay *r, uint32_t f);
uint32_t rp_fields(const Replay *r);
/* Checks the State Hash before field f (after field f - 1), and after the
   last field (f = rp_fields): 0 if it agrees or f has no check, -1 if not,
   with why naming the field and both hashes. */
int rp_check(const Replay *r, uint32_t f, uint64_t state_hash, char *why, size_t cap);
/* The marks. */
int rp_marks(const Replay *r);
void rp_mark_at(const Replay *r, int i, uint32_t *field, uint32_t *kind);
const RpBuild *rp_build(const Replay *r);
int rp_unlocks(const Replay *r);
/* The cards as recorded, for ggx-replays info: slot i's directories and
   each directory's name and file count; 0 if the slot had no card. */
int rp_card(const Replay *r, int slot, int *ndirs);
const char *rp_card_dir(const Replay *r, int slot, int dir, int *nfiles);

void rp_free(Replay *r);

/* Encoding and decoding, for the tests: a replay's bytes, and one read
   back from bytes. */
int rp_encode(const Replay *r, uint8_t **out, size_t *len);
Replay *rp_decode(const uint8_t *b, size_t len, char *why, size_t cap);
/* For the tests: a replay of n made-up fields for a build. */
Replay *rp_synthetic(const RpBuild *build, uint32_t fields);

/* The user data directory's replays/ folder (fsys.h), or data_dir's. */
int rp_dir(char *buf, size_t cap, const char *data_dir);

#endif
