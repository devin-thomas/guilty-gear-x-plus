/* Files by UTF-8 path on every OS, and the per-OS user data directory.
 *
 * Windows' C library takes paths in the ANSI code page, so a player whose
 * user name or disc folder isn't ASCII needs the wide-character calls;
 * these wrap them. Elsewhere they are the POSIX calls.
 */
#ifndef GGX_FSYS_H
#define GGX_FSYS_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

FILE *fs_open(const char *path, const char *mode);
/* 64-bit offsets, also on Windows. 0 on success. */
int fs_seek(FILE *f, uint64_t off);
/* The file's size, or -1. */
int64_t fs_size(FILE *f);
/* 0 if path exists; its size and modification time (seconds) if asked. */
int fs_stat(const char *path, uint64_t *size, int64_t *mtime);
/* Creates path and its parents. 0 on success or if it already exists. */
int fs_mkdirs(const char *path);
/* Renames from to to, replacing to if it exists. 0 on success. */
int fs_replace(const char *from, const char *to);
int fs_remove(const char *path);
/* out = a, then b, then c (either may be NULL). 0, or -1 (and out empty) if
   that doesn't fit in cap. */
int fs_join(char *out, size_t cap, const char *a, const char *b, const char *c);

/* Calls each with the name of every file (not directory) in dir, in no
   order. 0, or -1 if dir can't be read. */
int fs_list(const char *dir, void (*each)(void *ctx, const char *name), void *ctx);

/* The directory the port keeps its data in, with a trailing separator.
   It is what SDL3's SDL_GetPrefPath("", "ggx-plus") returns:
     Windows: %APPDATA%\ggx-plus\
     macOS:   ~/Library/Application Support/ggx-plus/
     Linux:   $XDG_DATA_HOME/ggx-plus/, or ~/.local/share/ggx-plus/
   It is not created. 0 on success. */
int fs_pref_dir(char *buf, size_t cap);

#endif
