/* The port's settings panel (PORT-019): the host's own screen over the
 * game's, in the game's HUD font (prompts/pr.h pr_text), drawn as the GPU
 * overlay (dl_gpu_set_overlay). It holds the settings the game itself has
 * no screen for: which button confirms in menus, per player (X or Circle;
 * sdl/input.h, common/input/confirm.h), and the Unlocks (PORT-020,
 * common/host/unlock.h): everything, or the characters, the colours, the
 * gallery's pictures and its challenges, each saying what it gives. An
 * unlock only ever raises what the save holds, so a row once unlocked stays
 * so. It is made at once at the title or in OPTIONS, where the game reads
 * them afresh; anywhere else it waits for the title.
 *
 * It opens with F9, or by holding Select (Back, View) for a second, on any
 * pad: the Steam Deck has no F9. While it is open the Core holds, as it
 * does while a player remaps, and the game gets no input. Up and Down pick
 * a row, Left and Right (or X, Circle) a value; Start, F9 or Escape closes
 * it and saves the config.
 *
 * On the first launch (no "confirm" in the config yet) a line over the
 * title says that X confirms and how to open the panel, for a few seconds.
 */
#ifndef GGX_SETTINGS_H
#define GGX_SETTINGS_H

#include <stdint.h>

#include "config.h"
#include "core_host.h"
#include "input.h"
#include "pr.h"

typedef struct Settings Settings;

/* font may be NULL (no HUD font on the disc): the panel works, unseen. The
   host is read for what is unlocked. */
Settings *settings_new(Input *in, Config *cfg, const Prompts *font, CoreHost *h);
void settings_free(Settings *s);

int settings_is_open(const Settings *s);
void settings_open(Settings *s);
/* Closes it and saves the config. */
void settings_close(Settings *s);

/* Once a field the game runs, with the pads as the players hold them (no
   confirm swap): counts Select held, and opens the panel after a second.
   Returns 1 if it opened (the field's input should not reach the game). */
int settings_watch(Settings *s, const PadInput pads[2]);

/* Once a tick while open, with the pads: moves, changes, closes. */
void settings_update(Settings *s, const PadInput pads[2]);

/* The unlocks asked for since the last call (UNLOCK_* mask), for the host
   to make (unlock_apply) where the game reads them afresh. */
int settings_take_unlocks(Settings *s);

/* The overlay to show now (640x448 RGBA8, GS alpha), or NULL for none.
   Changes only when what it shows does; *changed says so. */
const uint8_t *settings_overlay(Settings *s, int *changed);

#endif
