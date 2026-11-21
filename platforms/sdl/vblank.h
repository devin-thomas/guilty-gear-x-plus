/* The vblanks of the display a window is on (ADR-013's display pacing).
 *
 * On Windows a thread waits for each vertical blank of the window's monitor
 * (the kernel's D3DKMTWaitForVerticalBlankEvent for that output, else
 * IDXGIOutput::WaitForVBlank) and keeps the vblank grid (pace.h VGrid), in
 * SDL_GetPerformanceCounter units. Elsewhere
 * there is no source and vblank_open returns NULL, so the window keeps the
 * 59.94 Hz clock: on macOS neither CVDisplayLink nor CADisplayLink gives the
 * vblanks' times (both jitter by several milliseconds, measured on macOS 27),
 * and the window server composes every window anyway.
 */
#ifndef GGX_VBLANK_H
#define GGX_VBLANK_H

#include <stdint.h>
#include <stdio.h>

#include <SDL3/SDL.h>

typedef struct VBlank VBlank;

/* Follows the display the window is on now (reopen it when the window
   moves to another), whose mode says it refreshes at hz, or NULL when this
   platform has no vblank source. With log (--vblank-log), every wake-up is
   written there as "wake,<counter>". */
VBlank *vblank_open(SDL_Window *win, double hz, FILE *log);
void vblank_close(VBlank *v);

/* --fake-vblank, for testing the paced loop where there is no source: a grid
   of vblanks at hz from now, not the display's. */
VBlank *vblank_fake(double hz);

/* NULL while the source is good, else why there is none: on Windows each
   source (the kernel's per-output wait, then DXGI's) is dropped if it
   returns sooner than a quarter of a refresh 50 times in a row. */
const char *vblank_why(VBlank *v);

/* --vblank-source, for testing: "kernel" or "dxgi" tries only that source, and
   "nowait" makes every source return at once, so the check that drops a source
   that doesn't wait can be seen working. */
void vblank_prefer(const char *source);

/* The latest vblank and the period between vblanks, in performance counter
   ticks; 0 until the source has seen enough vblanks to know the period. */
int vblank_get(VBlank *v, uint64_t *last, uint64_t *period);

#endif
