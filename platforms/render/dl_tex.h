/* The renderer's texture cache (PORT-008).
 *
 * A texture is identified by its content: the format, the size and a hash
 * of the texels as the GS holds them, so a texture the game uploads again
 * every frame (the PXF pool is refilled each frame) is decoded and sent to
 * the GPU once. Indexed textures are kept as indices; their CLUT goes in
 * the frame's CLUT atlas (dl_frame.h), so a palette swap needs no new
 * texture. dl_gpu.c owns the GPU side of each entry.
 */
#ifndef GGX_DL_TEX_H
#define GGX_DL_TEX_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t key;
    uint16_t w, h;
    uint8_t kind;          /* DL_TEX_RGBA (4 bytes a texel) or DL_TEX_INDEX (1) */
    uint8_t max_alpha;     /* the largest texel alpha (RGBA only) */
    uint8_t *pixels;       /* waiting for upload; freed once uploaded */
    void *gpu;             /* the GPU texture, or NULL */
    uint32_t last_used;    /* frame serial */
    uint8_t live;
} DlTex;

typedef struct {
    DlTex *e;
    uint32_t n, cap;
    int32_t *map;          /* open addressing: key -> entry, -1 empty */
    uint32_t map_cap;
    /* GPU textures of evicted entries, for dl_gpu.c to release */
    void **dead;
    uint32_t ndead, cap_dead;
} DlTexCache;

void dl_tex_init(DlTexCache *c);
void dl_tex_free(DlTexCache *c);
/* The entry for key, or -1. */
int32_t dl_tex_find(DlTexCache *c, uint64_t key);
/* A new entry for key with room for its pixels (filled by the caller). */
int32_t dl_tex_add(DlTexCache *c, uint64_t key, uint16_t w, uint16_t h, uint8_t kind);
/* Drop entries not used in the last `age` frames (their GPU textures go to
   `dead`). */
void dl_tex_evict(DlTexCache *c, uint32_t serial, uint32_t age);

uint64_t dl_hash64(const void *data, size_t n, uint64_t seed);

#endif
