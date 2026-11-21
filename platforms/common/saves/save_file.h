/* The port's own save format (PORT-012, ADR-012).
 *
 * A save is one PS2 memory card directory, the way the game writes it
 * through libmc: BKSLPM-64525GGX (KR) or BISLPS-20137GGX (JP) holds the
 * settings and progress, icon.bin and icon.sys, and ...GGXRP0 to ...GGXRP9
 * hold the recorded Versus replays. The native store keeps each directory
 * in a file of its own, <save dir>/<directory>.ggxsave, with a header that
 * records the format version, the release and every extension or mod that
 * wrote it. The files the game wrote are kept byte for byte, so a save can
 * go back onto a PS2 memory card unchanged.
 */
#ifndef GGX_SAVE_FILE_H
#define GGX_SAVE_FILE_H

#include <stddef.h>
#include <stdint.h>

#define SAVE_FORMAT_VERSION 1
#define SAVE_MAX_FILES 16
#define SAVE_MAX_EXTENSIONS 32
#define SAVE_NAME_MAX 32 /* a PS2 directory entry's name field, NUL included */
#define SAVE_EXT_ID_MAX 64

/* the releases, numbered as in Lockstep Traces and snapshots */
enum { SAVE_RELEASE_KR = 1, SAVE_RELEASE_JP = 2 };

/* the PS2 directory entry modes the BIOS's mcman writes */
#define SAVE_MODE_DIR 0x8427u
#define SAVE_MODE_FILE 0x8497u

/* an extension's flags: it declared that what it writes is the PS2 game's own data */
#define SAVE_EXT_COMPATIBLE 1u

typedef struct {
    char name[SAVE_NAME_MAX];
    uint16_t mode;          /* the PS2 directory entry's mode */
    uint8_t created[8];     /* sceMcStDateTime, as on the card: 0, sec, min, hour, day, month, year (u16), JST */
    uint8_t modified[8];
    uint32_t size;
    uint8_t *data;          /* malloc'd; NULL if size is 0 */
} SaveFile;

typedef struct {
    char id[SAVE_EXT_ID_MAX];
    uint32_t flags;
} SaveExtension;

typedef struct {
    uint32_t release;       /* SAVE_RELEASE_*, or 0 if the directory isn't one of the game's */
    char name[SAVE_NAME_MAX];
    uint16_t mode;
    uint8_t created[8];
    uint8_t modified[8];
    int nfiles;
    SaveFile files[SAVE_MAX_FILES];
    int next;
    SaveExtension ext[SAVE_MAX_EXTENSIONS];
} SaveDir;

/* save_read's results */
enum {
    SAVE_OK = 0,
    SAVE_EIO = -1,      /* can't be read */
    SAVE_EBAD = -2,     /* not a save file, cut short, or its hash doesn't match */
    SAVE_ENEWER = -3,   /* a format version this build doesn't read */
};

void save_dir_init(SaveDir *d, const char *name);
void save_dir_free(SaveDir *d);
/* A deep copy. 0 on success. */
int save_dir_copy(SaveDir *dst, const SaveDir *src);
/* The file's index, or -1. */
int save_dir_find(const SaveDir *d, const char *name);
/* Replaces or adds the file (a copy of data). 0 on success. */
int save_dir_put(SaveDir *d, const char *name, uint16_t mode, const uint8_t created[8], const uint8_t modified[8],
                 const void *data, uint32_t size);
void save_dir_remove(SaveDir *d, int index);
/* Records that an extension wrote the directory (once per id). 0 on success. */
int save_dir_touch(SaveDir *d, const char *id, uint32_t flags);

/* The release a game directory name belongs to, or 0. */
uint32_t save_release_of(const char *dir_name);
const char *save_release_name(uint32_t release);
/* The game's system directory for a release ("BKSLPM-64525GGX"), or NULL. */
const char *save_system_dir(uint32_t release);

/* The native file. save_write writes a temporary file beside path and
   renames it over path, so a reader sees the old save or the new one,
   never part of one. Both return SAVE_OK or a SAVE_E* code; why gets a
   sentence on failure. *version gets the file's format version when known. */
int save_write(const char *path, const SaveDir *d, char *why, size_t cap);
int save_read(const char *path, SaveDir *d, uint32_t *version, char *why, size_t cap);
/* The same, to and from memory (save_encode's buffer is malloc'd). */
int save_encode(const SaveDir *d, uint8_t **out, size_t *len);
int save_decode(const uint8_t *buf, size_t len, SaveDir *d, uint32_t *version, char *why, size_t cap);

/* Checks a directory against the original game's save layout: the files the
   game writes, their sizes, the data file's checksum, icon.sys. 0 if it is
   one; otherwise nonzero, and why names the first thing that isn't. */
int save_check_layout(const SaveDir *d, char *why, size_t cap);
/* What export requires (ADR-012): a known release that matches the
   directory, no extension that isn't save-compatible, and the original
   layout. 0 if it may go onto a PS2 memory card; otherwise nonzero, and why
   names the cause. */
int save_check_export(const SaveDir *d, char *why, size_t cap);

/* The game data file's checksum (memory.c MemoryCheckSumCheck): its bytes
   sum to 1. */
int save_checksum_ok(const uint8_t *data, uint32_t size);

/* The native store's directory when none is given: saves/ in the user data
   directory (beside ggx.cfg and the extracted assets), or in data_dir. */
int save_store_dir(char *buf, size_t cap, const char *data_dir);
/* The store's file for a directory. 0, or -1 if it doesn't fit. */
int save_store_path(char *buf, size_t cap, const char *store, const char *dir_name);

/* A sceMcStDateTime for a Unix time, in JST as the PS2 keeps it. */
void save_ps2_time(int64_t unix_seconds, uint8_t out[8]);

#endif
