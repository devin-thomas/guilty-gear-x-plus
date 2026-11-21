/* PS2 pad state as libpad delivers it (PORT-010).
 *
 * The game reads both pads once a field with scePadRead (pad.c
 * PadReadMain) and decodes the buttons from bytes 2-3, active low. Its
 * mode byte (byte 1) decides the rest:
 *
 *   0x41  a digital pad, or a DualShock 2 with its ANALOG light off: the
 *         directions come from the d-pad bits;
 *   0x73  a DualShock 2 in analog mode: the game ignores the d-pad bits and
 *         takes the directions from the left stick (bytes 6 and 7) past
 *         0x30 / 0xD0.
 *
 * pad_encode writes what libpad writes, as the PS2's own reads show in the
 * Lockstep Traces (the PS2 in PCSX2 gives 0x41, scePadRead returns 32, and
 * bytes 4-7 of a digital read are zero). Nothing here depends on SDL, so the
 * headless host, the trace tools and ggx share it.
 */
#ifndef GGX_PAD_H
#define GGX_PAD_H

#include <stdint.h>

/* The button bits, active high, as bytes 2-3 of a read (byte 2 high). */
enum {
    PAD_L2 = 0x0001,
    PAD_R2 = 0x0002,
    PAD_L1 = 0x0004,
    PAD_R1 = 0x0008,
    PAD_TRIANGLE = 0x0010,
    PAD_CIRCLE = 0x0020,
    PAD_CROSS = 0x0040,
    PAD_SQUARE = 0x0080,
    PAD_SELECT = 0x0100,
    PAD_L3 = 0x0200,
    PAD_R3 = 0x0400,
    PAD_START = 0x0800,
    PAD_UP = 0x1000,
    PAD_RIGHT = 0x2000,
    PAD_DOWN = 0x4000,
    PAD_LEFT = 0x8000,
};

#define PAD_NBUTTONS 16
#define PAD_DIRS (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)

enum { PAD_MODE_DIGITAL = 0, PAD_MODE_ANALOG = 1 };

#define PAD_STICK_CENTRE 0x80

/* One field's input for one port. It is the field's input record
   (ADR-014): given for every field, never carried over in Core state. */
typedef struct {
    uint16_t buttons;     /* bytes 2-3, active high here */
    uint8_t analog[4];    /* rx, ry, lx, ly (bytes 4-7 in analog mode); 0x80 is centred */
    uint8_t mode;         /* PAD_MODE_DIGITAL (0x41) or PAD_MODE_ANALOG (0x73) */
} PadInput;

/* A centred, digital pad with nothing pressed. */
void pad_clear(PadInput *p);
/* The same input (padding aside). */
int pad_equal(const PadInput *a, const PadInput *b);

/* scePadRead's 32 bytes for in, and its return value. */
uint32_t pad_encode(const PadInput *in, uint8_t out[32]);

/* libpad's actuator alignment request (scePadSetActAlign), as the PS2 runs
   it in every Lockstep Trace. The game makes one for each port when it
   first sees the pad at boot, and at the start of every fight (GameInit).
   libpad works through it on the IOP, vblank by vblank: counting from the
   vblank count when it was made, scePadGetReqState says busy (2) from the
   next vblank until it completes, scePadRead fails in its last three
   vblanks, and scePadSetActDirect returns 0 in the first two of those. It
   takes five vblanks when made during the field's pad reads (before the
   port's scePadSetActDirect: the boot), six when made after them (GameInit).
   The pads are read once a field, so in play, where a field is a vblank,
   that is 4 or 5 fields of input the game drops. The first read after
   scePadPortOpen fails too (field 1). The pads are always connected, as in
   the recorder. */
typedef struct {
    uint32_t call;   /* the vblank count when the request was made */
    uint32_t done;   /* the vblank count it completes at; 0: no request yet */
} PadRequest;

void pad_request(PadRequest *r, uint32_t vblank, int after_actuators);
/* field: the field running (1 = the first after the boot); vblank: the
   Core's vblank count now */
int pad_read_ok(const PadRequest *r, uint32_t field, uint32_t vblank);
uint32_t pad_req_state(const PadRequest *r, uint32_t vblank);   /* 0 complete, 2 busy */
uint32_t pad_act_direct(const PadRequest *r, uint32_t vblank);  /* scePadSetActDirect's return */

/* The names GGScript uses (the gg-layers PCSX2 fork's): "Cross", "L1",
   "Up"... pad_button_by_name also takes "X" for Cross. */
const char *pad_button_name(int index);    /* index 0-15: the bit 1 << index */
int pad_button_by_name(const char *name);  /* the bit, or 0 */
/* What a person reads (PORT-019): the same, but X, never Cross. */
const char *pad_button_label(int index);

#endif
