/* Which button confirms in menus (PORT-019).
 *
 * The KR and JP releases confirm with Circle and cancel with X, as Japanese
 * games did. With "confirm = x" (the port's default) the two swap wherever
 * the game is asking to confirm or cancel (common/host/menu.h decides
 * where): the button that is X in a fight confirms. Never in a fight, where
 * X and Circle are attack buttons.
 *
 * The swap acts on the PS2 buttons after the bindings, so only which button
 * sends which PS2 bit changes: the Core sees ordinary pad bytes. A press
 * keeps the meaning it had when it went down until it is released, so a
 * button held across a change of screen (X closing the pause menu, the
 * fight resuming) doesn't turn from Circle into a new press of X. No SDL
 * here.
 */
#ifndef GGX_CONFIRM_H
#define GGX_CONFIRM_H

#include <stdint.h>

enum { CONFIRM_CIRCLE = 0, CONFIRM_X = 1 };

/* "x" or "circle" (case-insensitive) as CONFIRM_*, or -1. */
int confirm_parse(const char *s);
const char *confirm_name(int confirm);

/* One pad's held X and Circle: -1 up, 0 held as itself, 1 held swapped. */
typedef struct {
    int8_t held[2];
} ConfirmLatch;

void confirm_reset(ConfirmLatch *l);

/* A field's buttons (pad.h bits) with X and Circle swapped if swap was on
   when each went down. */
uint16_t confirm_apply(ConfirmLatch *l, uint16_t buttons, int swap);

#endif
