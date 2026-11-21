/* The Player Disc's assets (ADR-004): identifying the disc, extracting its
 * files into the user data directory, and serving the game's reads from
 * them.
 *
 * The extracted files are the disc's files, byte for byte, in
 * <data dir>assets/<kr|jp>/, with a manifest.txt written last:
 *
 *     ggx-assets 1
 *     release KR SLPM-645.25 Guilty Gear X Plus (Korea)
 *     image CHD (CD, MODE2_RAW) <the image extracted from>
 *     file <lsn> <size> <mtime> <sha256> <path>      (one per file)
 *
 * The hashes are the disc's own (releases.c); the mtime is the extracted
 * file's, so a later run can tell whether a file has changed without
 * hashing it.
 */
#ifndef GGX_ASSETS_H
#define GGX_ASSETS_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "image.h"
#include "releases.h"

/* The release whose files the image's ISO 9660 tree holds (same names and
   sizes; the hashes are checked as the files are extracted), or NULL with
   a refusal for the player in why: what the image is and which releases
   the port supports. */
const Release *assets_identify(DiscImage *img, const char *path, char *why, size_t cap);

/* "<data dir>assets/kr/" for KR. 0 on success. */
int assets_dir(char *buf, size_t cap, const char *data_dir, const Release *r);

typedef struct {
    const Release *release;
    unsigned kept;         /* already extracted and intact */
    unsigned extracted;    /* missing, changed or corrupted, and extracted again */
} ExtractResult;

/* Identifies the image at path and extracts its release into data_dir.
   A file already there is kept only if its SHA-256 is the release's; any
   other is extracted again, checked against that hash, and moved into
   place, so re-running is idempotent and repairs a damaged extraction. The
   manifest is written once every file is in place. Progress and any
   refusal go to msg. 0 on success. */
int assets_extract(const char *path, const char *data_dir, ExtractResult *res, FILE *msg);

/* What a player's screen needs from an extraction (ggx's first run,
   sdl/firstrun.h): every callback may be NULL, and is called on the thread
   that extracts. */
typedef struct {
    void *ctx;
    /* once the image is identified: the release, and the bytes to read */
    void (*begin)(void *ctx, const Release *r, uint64_t total);
    /* as the bytes are read (and checked): the file, the bytes done of all */
    void (*progress)(void *ctx, const char *file, uint64_t done, uint64_t total);
    /* nonzero stops the extraction (it fails; what is in place stays whole) */
    int (*stop)(void *ctx);
    /* the refusal or failure, as written to msg without its "ggx: " */
    char *why;
    size_t why_cap;
} ExtractWatch;

/* assets_extract, watched. */
int assets_extract_watched(const char *path, const char *data_dir, ExtractResult *res, FILE *msg,
                           const ExtractWatch *w);

/* 0 if data_dir holds a manifest of r's extraction (whole or not: see
   asset_store_open). image gets the disc image it was extracted from, if
   that still exists, or "". */
int assets_manifest_image(const char *data_dir, const Release *r, char *image, size_t cap);

/* The extracted assets of one release, read as the disc's sectors at the
   release's own LSNs. */
typedef struct AssetStore AssetStore;

/* Opens r's extraction in data_dir. It must have a manifest for r, and
   every file must have its manifest size; a file whose mtime differs from
   the manifest's is hashed and must still be intact. Refusals go to msg.
   0 on success. */
int asset_store_open(AssetStore **out, const char *data_dir, const Release *r, FILE *msg);
void asset_store_close(AssetStore *s);
/* A file's LSN and size, by ISO 9660 path. 0 on success. */
int asset_store_find(AssetStore *s, const char *path, uint32_t *lsn, uint32_t *size);
/* Reads count 2048-byte sectors from lsn, as the disc holds them (the end
   of a file's last sector reads as zeros, as on the disc). Every sector
   must be in a file. 0 on success; a SectorRead (iso9660.h). */
int asset_store_read(void *s, uint32_t lsn, uint32_t count, void *dst);

/* The SHA-256 of the file at path, as hex. 0 on success. */
int assets_hash_file(const char *path, char hex[65]);

#endif
