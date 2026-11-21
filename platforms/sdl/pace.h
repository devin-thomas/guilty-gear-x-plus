/* Display pacing (ADR-013): when the display refreshes at about a whole
 * multiple of the field rate (59.94 Hz within 0.5%: 60, 119.88, 120, 240 Hz),
 * the Core steps once every k vblanks instead of on its 59.94 Hz clock, and
 * each field starts a measured lead before the vblank its frame is for. Every
 * frame then reaches the display in time for exactly one vblank and stays up
 * for k refreshes; none waits behind another in the swapchain, and none is
 * dropped where the Core's clock and the display's beat (every 16.7 s at
 * 60.00 Hz). The game runs at the display's rate, 0.1% fast at 60.00 Hz,
 * which the sound's rate control absorbs.
 *
 * Times are in performance counter ticks. No SDL calls: tests/pace drives it
 * with made-up displays.
 */
#ifndef GGX_PACE_H
#define GGX_PACE_H

#include <stdint.h>

#define PACE_FIELD_HZ 59.94
#define PACE_WORK_N 128

typedef struct {
    int k;                   /* refreshes a field when paced; 0: the 59.94 Hz clock */
    double hz;               /* the display's refresh rate */
    uint64_t freq;           /* counter ticks a second */
    uint64_t target;         /* the vblank the next field's frame is for, or 0 */
    uint64_t lead;           /* a field starts this long before its vblank */
    uint64_t fixed_lead;     /* a lead set by hand, or 0 to measure it */
    uint64_t work[PACE_WORK_N];
    int nwork, iwork;
    uint64_t last_done;      /* when the last field's frame was done, its GPU work included */
    uint32_t late;           /* fields that started after their start time had passed */
    uint32_t behind;         /* frames whose GPU work ended after their vblank: the swapchain was a frame behind */
    int retarget;            /* the next field must be placed again, after last_done */
    uint32_t unshown;        /* fields run without a present, to keep up with 59.94 Hz (ggx_main.c) */
    uint64_t nominal;        /* a refresh by the display mode's rate */
    uint64_t last_start;     /* when the last field started */
    uint8_t late_recent[120];   /* the last 120 fields: 1 if late */
    uint8_t behind_recent[120]; /* the last 120 frames: 1 if behind */
    int ilate, nfields, nlate_recent, nbehind_recent;
} Pace;

/* The display's vblank grid, from the times a thread woke for its vblanks
   (vblank.c). A wake-up comes after its vblank by a varying delay, never
   before it. The period is the slope of a least-squares line through every
   wake-up against its vblank's number since the first, so its error shrinks
   fast as the vblanks add up. The phase is the earliest of the last 64
   wake-ups put back onto the grid, so it sits within the smallest delay of
   the real vblanks and never after a wake-up. */
#define VGRID_N 64

typedef struct {
    uint64_t min_gap;        /* wake-ups closer than this to the last are ignored (half a refresh by the mode) */
    uint64_t t0, period;     /* the first wake-up; the period, 0 until known */
    uint64_t first[8];
    int nfirst;
    double n, sx, sy, sxx, sxy;   /* the least-squares sums: vblank number, ticks since t0 */
    uint64_t wake[VGRID_N];  /* the latest wake-ups */
    int nwake, iwake;
    uint64_t last;           /* a vblank of the grid, at or before the latest wake-up */
    uint64_t latest;         /* the latest wake-up */
} VGrid;

void vgrid_seen(VGrid *g, uint64_t t);

/* The grid's last vblank at or before now, and its period; 0 until the period
   is known, or when no vblank has been seen for 8 periods. */
int vgrid_at(const VGrid *g, uint64_t now, uint64_t *vblank, uint64_t *period);

/* For a display refreshing at hz (0 if unknown); allow 0 keeps the clock. */
void pace_init(Pace *p, double hz, uint64_t freq, int allow);

/* When the next field should start, given the latest vblank and the period.
   Never sooner than 0.9 of a field (k refreshes by the display mode's rate)
   after the last field started, whatever the vblanks say: a field that is
   late is shown late, and never brings the next one forward. */
uint64_t pace_start(Pace *p, uint64_t now, uint64_t vblank, uint64_t period);

/* NULL while pacing by the display makes sense, else why not: the vblanks'
   period disagrees with the display mode's rate by more than 1%, more than 6
   of the last 120 fields started late, or more than 30 of the last 120 frames
   missed their vblank (each one loses a refresh, so a machine that can't make
   its vblanks would run slow where the clock keeps time by dropping frames).
   The caller goes back to the clock. */
const char *pace_sane(const Pace *p, uint64_t period);

/* The earliest time to present the field's frame: just after the vblank
   before its own, so a frame done early isn't shown early (with a lead longer
   than a refresh, at 120 or 240 Hz). */
uint64_t pace_present_at(const Pace *p, uint64_t period);

/* The field that started at start had its frame's GPU work finished at done,
   after being held back hold ticks before presenting: its work sets the lead,
   and the next field is for the vblank k refreshes on. If done is after the
   vblank the frame was for, the next one comes k refreshes after this frame's
   flip instead. On D3D12 the swapchain has two buffers, and a frame's GPU work
   waits for its buffer to leave the screen, which happens at the last frame's
   flip. One frame a vblank late makes every later frame wait for that flip, a
   vblank late too, for good, unless a refresh passes without a new frame. */
void pace_done(Pace *p, uint64_t start, uint64_t done, uint64_t hold, uint64_t period);

#endif
