/* The releases the port supports, and how a disc is recognised as one.
 *
 * A Player Disc is identified by its files: the ISO 9660 tree must hold
 * exactly a release's files, with their sizes, and every file's SHA-256
 * must match (checked as the file is extracted). The table is generated
 * by tools/release_table.py and holds names, LSNs, sizes and hashes only.
 */
#ifndef GGX_RELEASES_H
#define GGX_RELEASES_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *path;     /* in the ISO 9660 tree, without ";1" ("CDDATA.BIN") */
    uint32_t lsn;         /* where the release's own disc has it */
    uint32_t size;
    const char *sha256;
} ReleaseFile;

typedef struct {
    const char *id;       /* "KR" or "JP", as GGX_VERSION */
    const char *serial;   /* "SLPM-645.25" */
    const char *title;    /* "Guilty Gear X Plus (Korea)" */
    const char *boot;     /* the boot ELF's file name */
    const ReleaseFile *files;
    unsigned nfiles;
} Release;

#define GGX_RELEASES 2
extern const Release ggx_releases[GGX_RELEASES];

/* NULL if id names no release. */
const Release *release_by_id(const char *id);
/* The release file at path (ISO 9660 path rules: case-insensitive, a
   leading separator and a ";1" version are optional), or NULL. */
const ReleaseFile *release_file(const Release *r, const char *path);
/* Writes the supported releases, one per line, for refusal messages. */
void release_list(char *buf, size_t cap);

#endif
