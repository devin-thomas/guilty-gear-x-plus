/* The text layer: English in place of the game's text pictures (PORT-014).
 *
 * Guilty Gear X Plus draws its Japanese and Korean text as textures on the
 * disc (story lines, win quotes, gallery captions, move lists); its string
 * fonts draw Latin letters only, and its menus are English already. So the
 * English is a texture substitution in the renderer, outside the Core
 * (ADR-007): the Core loads, uploads and draws the original pictures as
 * ever, and the renderer draws an English picture of the same format, size
 * and palette wherever an original's upload arrives (DlSubst, dl_stream.h).
 *
 * Translation files (LANG-FORMAT.md) name each picture by disc file and its
 * place in the file's offset tables. tl_bind reads those pictures from the
 * player's disc, as PXF_Entry and PXF_SendTexture would upload them, and
 * keys each by the content hash dl_stream gives the upload. Each English
 * picture is drawn the first time its original is uploaded.
 */
#ifndef GGX_TL_H
#define GGX_TL_H

#include <stddef.h>
#include <stdint.h>

typedef struct TextLayer TextLayer;

enum { TL_KR = 1, TL_JP = 2 };

/* Reads count 2048-byte sectors of the release's disc from lsn. 0 on success. */
typedef int (*TlRead)(void *ctx, uint32_t lsn, uint32_t count, void *dst);

/* Loads a set of translation files (lang_path and what it includes) for
   release, and the fonts from font_dir. NULL with a message in err. */
TextLayer *tl_open(int release, const char *lang_path, const char *font_dir, char *err, size_t cap);
void tl_free(TextLayer *t);

/* Finds every translated picture on the disc: CDDATA.BIN at cddata_lsn,
   its file table `seekpos` (the boot ELF's, as the game reads it: nfiles+1
   pairs of sector offset and compressed size). Pictures that aren't on
   this disc, or aren't text, are reported in err (one line each) and
   skipped. Returns the number of pictures bound, or -1. */
int tl_bind(TextLayer *t, TlRead read, void *ctx, uint32_t cddata_lsn, const uint8_t *seekpos, uint32_t nfiles,
            char *err, size_t cap);

/* One indexed picture of a game file, as PXF_SendTexture uploads it: the
   path names it through the file's offset tables ("067", "003/001"), as
   translation files do. For other layers that draw from the game's own
   pictures (PORT-017's button letters). 0 on success; free texels. */
typedef struct {
    int psm;
    uint32_t w, h;
    uint8_t *texels;         /* upload format */
    uint32_t clut[256];      /* RGBA8, the GS's alpha */
    uint64_t hash;           /* as dl_stream hashes the upload */
} TlRawPicture;
int tl_read_picture(TlRead read, void *ctx, uint32_t cddata_lsn, const uint8_t *seekpos, uint32_t nfiles, uint32_t fid,
                    const char *path, TlRawPicture *out);

/* The English picture for an upload, in the upload's own format (PSMT4
   packed two texels a byte, low nibble first, or PSMT8), or NULL. Matches
   DlSubst. */
const uint8_t *tl_subst(void *t, uint64_t upload_hash, int psm, uint32_t w, uint32_t h, const uint8_t *texels,
                        uint32_t variant);

/* How the English fitted its picture. */
enum { TL_FIT = 0, TL_FIT_SMALLER = 1, TL_FIT_NOT = 2 };

/* Each bound picture, for tools: the original's texels and CLUT (RGBA,
   the GS's alpha) and the English ones. Index 0 .. tl_count()-1. */
typedef struct {
    uint32_t fid;
    char path[24];
    int psm;
    uint32_t w, h;
    const uint8_t *orig;     /* upload format */
    const uint8_t *english;  /* upload format, drawn on demand */
    const uint32_t *clut;    /* 16 or 256 entries */
    int review;              /* marked '?' */
    int fit;                 /* TL_FIT_* */
    const char *text;
} TlPicture;
uint32_t tl_count(const TextLayer *t);
int tl_picture(TextLayer *t, uint32_t i, TlPicture *out);

#endif
