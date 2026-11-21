/* The GS stream: what the game sends the GS, read as the renderer's input
 * (PORT-008, PORT-009).
 *
 * The Draw Layer's output is the sorted frame SpriteDisplay sends over GIF
 * list 2: the nl* primitives' packets in sort order, with the PXF manager's
 * texture and CLUT uploads placed right before the packets that use them,
 * and the palette uploads PXF_SendPalette and PXF_SetPaletteMain send on
 * the same list during the frame. The host hands every GIF DMA send to
 * dl_stream_send. It walks the DMA chain in the Core's memory, read only,
 * keeps the GS state those packets set (TEX0, ALPHA, the CLUT buffer, local
 * memory), and turns each primitive into triangles in a DlFrame.
 *
 * List 0 (frame setup and the clear) and list 3 (debug text and the
 * GraphConv blit) are the GS Path (DRAW-INVENTORY.md), interpreted the same
 * way: FRAME and SCISSOR pick where primitives land, the clear sprite and
 * the debug font's sprites are drawn like any primitive, and a
 * local-to-local transfer between framebuffers becomes a GPU copy. The
 * GraphConv blit completes a frame. The display circuits' privileged
 * registers (PMODE, SMODE2, DISPFB1/2, DISPLAY1/2, BGCOLOR), which the game
 * writes straight into memory (GS_runtime_preset), are read when a frame is
 * complete and resolved into the frame's DlDisplay.
 *
 * Framebuffers live on the GPU (DlSurface): FRAME's, DISPFB's and a blit's
 * destination. Everything else in local memory is the CPU model in
 * gs_mem.c, where textures and CLUTs are read.
 *
 * Nothing here writes Core memory, so the Core runs the same with the
 * renderer on or off.
 */
#ifndef GGX_DL_STREAM_H
#define GGX_DL_STREAM_H

#include <stdint.h>

#include "dl_frame.h"
#include "dl_tex.h"
#include "gs_mem.h"

/* The Core's memory, read only. */
typedef struct {
    const uint8_t *ram;   /* EE RAM, 32 MB */
    const uint8_t *spr;   /* the scratchpad, 16 KB */
    const uint8_t *priv;  /* the GS privileged registers at 0x12000000, as
                             the EE maps them (8 KB), or NULL */
} DlMem;

/* DL_LIST_REPLAY: GIF data from a GS dump, whose lists are unknown. The
   frame clear and the GraphConv blit are then told by their content (the
   full-screen FIX-blended sprite, the transfer between framebuffers). */
enum { DL_LIST_SETUP = 0, DL_LIST_DRAW = 2, DL_LIST_TAIL = 3, DL_LIST_OTHER = -1, DL_LIST_REPLAY = -2 };

typedef struct {
    uint64_t sends, prims, triangles, sprites, batches, frames;
    uint64_t uploads, upload_bytes;
    uint64_t uploads_short;     /* uploads whose IMAGE data ended before their size: the
                                   PXF manager's T8 pictures not a multiple of 16 texels */
    uint64_t tex_new, tex_new_texels, tex_from_upload, tex_from_memory, cluts;
    uint64_t alpha_over_one;    /* prims whose As can exceed 1.0 (blended in the shader) */
    uint64_t blend[256];        /* prims by ALPHA_1's selector byte (ABE on) */
    uint64_t no_blend;          /* prims with ABE off */
    uint64_t blocks_written;    /* upload blocks written into local memory late, on a read */
    uint64_t unsupported;       /* prims, transfers or display circuits skipped: STQ
                                   coordinates, other framebuffer formats or sizes,
                                   transfers between a framebuffer and plain memory */
    uint64_t unexpected[256];   /* list-2 writes to registers the Draw Layer never sets */
    uint64_t dma_errors;
    /* the GS Path (PORT-009) */
    uint64_t clears;            /* frame clears (MakeFullBuffer's sprite) */
    uint64_t text_prims;        /* debug text sprites (list 3) */
    uint64_t lines, points;
    uint64_t clipped;           /* prims wholly outside their scissor: nothing to draw */
    uint64_t blits;             /* transfers between framebuffers, done on the GPU */
    uint64_t local_copies;      /* local-to-local transfers within local memory */
    uint64_t displays;          /* frames shown, and of those with both circuits merged */
    uint64_t displays_merged;
    uint64_t gs_unsupported;    /* of `unsupported`, on lists 0 and 3 or in the display */
    uint64_t state_unsupported; /* prims drawn under GS state the renderer doesn't
                                   reproduce: alpha, destination-alpha or depth tests,
                                   PABE, FBA, a frame write mask, no COLCLAMP, fog */
    uint64_t surface_reads;     /* textures read from a framebuffer's blocks (they
                                   hold the CPU model's bytes, not the GPU's pixels) */
    uint64_t surface_writes;    /* image uploads into a framebuffer's blocks */
} DlStats;

typedef struct DlStream DlStream;

DlStream *dl_stream_new(void);
void dl_stream_free(DlStream *s);
GsMem *dl_stream_gs_mem(DlStream *s);
DlTexCache *dl_stream_textures(DlStream *s);

/* One GIF DMA send of a chain starting at the EE address tag (bit 31: in
   the scratchpad), from packet list `list`. */
void dl_stream_send(DlStream *s, const DlMem *mem, uint32_t tag, int list);

/* Told of every block of GIF data a DMA send carries, in order, before it
   is read: what the GS would receive. ggx --gs-dump writes them into a
   PCSX2 GS dump, so PCSX2 can draw the port's own frames for comparison. */
typedef void (*DlTap)(void *ctx, const uint8_t *qwords, uint32_t count);
void dl_stream_set_tap(DlStream *s, DlTap tap, void *ctx);

/* Asked for each texture decoded from an upload, by the upload's content
   hash (its texels as sent), PSM and size, with those texels and the
   variant DlVariant chose: texels in the upload's own format and size to
   draw instead, or NULL for the upload's. PORT-014's English text replaces
   the game's text pictures this way, outside the Core, and PORT-017's
   button prompts redraw the icon cells of a font. Setting it (or NULL)
   makes every texture decode again. */
typedef const uint8_t *(*DlSubst)(void *ctx, uint64_t upload_hash, int psm, uint32_t w, uint32_t h,
                                  const uint8_t *texels, uint32_t variant);
void dl_stream_set_subst(DlStream *s, DlSubst fn, void *ctx);

/* Asked for each primitive drawn from an upload, with the upload's hash
   and where the primitive starts (framebuffer pixels): which variant of
   the substitution to draw it with, 0 for the plain one. A variant is
   decoded once and cached under its own key, so one upload can be drawn
   two ways in a frame (PORT-017: KEY CONFIG's 1P and 2P columns), and a
   change of variant costs one decode. */
typedef uint32_t (*DlVariant)(void *ctx, uint64_t upload_hash, int x, int y);
void dl_stream_set_variant(DlStream *s, DlVariant fn, void *ctx);

/* Raw GIF PATH3 data (whole qwords), as a PCSX2 GS dump records it; a tag
   may continue from the previous call. For tests against PCSX2. */
void dl_stream_gif(DlStream *s, const uint8_t *data, size_t qwords, int list);

/* The privileged registers as a PCSX2 GS dump records them before a vsync
   (GSPrivRegSet, laid out as the EE maps them: 8 KB). They become the
   display of the last completed frame, which that vsync shows, and of the
   frames after it. Until the first, the renderer assumes what
   GS_runtime_preset writes with no screen offset. */
void dl_stream_priv(DlStream *s, const uint8_t *regs);

/* The last frame the GS Path displayed (GraphConv), or NULL before the
   first. Valid until the next dl_stream_send or dl_stream_gif. */
const DlFrame *dl_stream_frame(DlStream *s);
const DlStats *dl_stream_stats(const DlStream *s);
/* Write the statistics, one per line. */
void dl_stream_print_stats(const DlStream *s, void *file);

#endif
