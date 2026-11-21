/* The GS's 4 MB local memory, with the hardware's swizzled layouts (PORT-008).
 *
 * The Draw Layer's textures and CLUTs reach the GS as image transfers into
 * local memory, and its primitives read them back through TEX0. The
 * renderer keeps the same memory, written and read with the GS's own page,
 * block and column layouts, because the game relies on what that layout
 * makes of its addresses: palette slots overlap (a slot's 256-colour CLUT at
 * 0x2320 + 2*slot spans four blocks), the 16-colour banks sit inside them,
 * and the texture pool is rewritten in the middle of a frame. A PCSX2 GS
 * state loads into it unchanged (`gs_mem_load`).
 *
 * Formats: PSMCT32, PSMCT24, PSMCT16, PSMT8 and PSMT4, which are all the
 * game uses (DRAW-INVENTORY.md). Addresses are in 256-byte blocks (BP) and
 * buffer widths in 64-pixel units (BW), as in the GS registers.
 */
#ifndef GGX_GS_MEM_H
#define GGX_GS_MEM_H

#include <stddef.h>
#include <stdint.h>

#define GS_MEM_SIZE (4u << 20)

enum {
    GS_PSMCT32 = 0x00,
    GS_PSMCT24 = 0x01,
    GS_PSMCT16 = 0x02,
    GS_PSMT8 = 0x13,
    GS_PSMT4 = 0x14,
};

typedef struct {
    uint8_t *m;   /* GS_MEM_SIZE bytes */
} GsMem;

int gs_mem_init(GsMem *g);
void gs_mem_free(GsMem *g);
/* Replace the whole memory with a raw 4 MB image, such as PCSX2's. */
void gs_mem_load(GsMem *g, const uint8_t *image);

/* 1 if the GS can store pixels of this format here. */
int gs_psm_supported(int psm);
/* Bits per pixel of a supported format. */
int gs_psm_bits(int psm);

/* A host-to-local transfer: w x h pixels at (x, y) of the buffer at bp, bw,
   from data packed as the GIF IMAGE data is (32, 24, 16, 8 or 4 bits a
   pixel, rows in order, 4-bit pixels low nibble first). */
void gs_write_rect(GsMem *g, int psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                   const uint8_t *data);

/* The same, from a sub-rectangle of a larger image: the pixels from (sx, sy)
   of data, whose rows are stride pixels apart. */
void gs_write_sub(GsMem *g, int psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                  const uint8_t *data, uint32_t stride, uint32_t sx, uint32_t sy);

/* Read w x h pixels at (x, y) as raw values: uint32_t for PSMCT32 and
   PSMCT24 (24's top byte is whatever memory holds), uint16_t for PSMCT16,
   uint8_t indices for PSMT8 and PSMT4. */
void gs_read_rect(const GsMem *g, int psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y, uint32_t w,
                  uint32_t h, void *out);

/* The block a pixel of a buffer lands in (0-16383), for overlap checks. */
uint32_t gs_block_of(int psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y);

/* The GS's CLUT buffer: 512 16-bit halves. A CT32 entry k is stored as
   halves k and 256 + k (k mod 256), a CT16 entry k as half k (mod 512). */
typedef struct {
    uint16_t h[512];
} GsClut;

/* The CLUT load a TEX0 write with CLD set performs, for an indexed psm (T8
   or T4), with CSM1 (the only mode the game uses): 256 or 16 entries read
   from the 16x16 or 8x2 CPSM image at CBP, at the entry offset CSA gives,
   as PCSX2 does it. */
void gs_clut_load(GsClut *c, const GsMem *g, int psm, int cpsm, uint32_t cbp, uint32_t csa);
/* The n (16 or 256) entries an indexed texture with this CPSM and CSA
   reads, as RGBA8 (bytes R, G, B, A; CT16 alpha from TEXA's TA0/TA1 with
   AEM 0, the game's setting). */
void gs_clut_read(const GsClut *c, int psm, int cpsm, uint32_t csa, uint64_t texa, uint32_t out[256]);

/* A CT16 pixel as RGBA8 under TEXA (AEM 0). */
uint32_t gs_ct16_rgba(uint16_t c, uint32_t texa_ta0, uint32_t texa_ta1);

#endif
