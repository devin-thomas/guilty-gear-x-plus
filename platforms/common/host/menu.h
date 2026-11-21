/* Where the game is asking to confirm or cancel (PORT-019, common/input/confirm.h).
 *
 * Read from the Core's state, read only, before each field's input:
 *
 * - Fights (GameInit to GameEnd) only while paused (Pause): the pause
 *   menus and Training's menu confirm and cancel; a fight doesn't. And a
 *   story's talk before its fight (story.is_enter: the fighters are held,
 *   and Circle skips a line).
 * - KEY CONFIG (from OPTIONS, or any pause menu) assigns whichever button
 *   a player presses to the row under their cursor, so its rows 0-10
 *   (PUNCH to VIBRATE) are never swapped; DEFAULT and EXIT are, per pad
 *   (Opt_KeyConfig.pos).
 * - The character select (SelectInit to QuickSelectMain) reads every face
 *   button as "choose, in this colour" and has no cancel, so X already
 *   chooses there; it isn't swapped, and each button keeps its colour.
 * - Every other job is a menu, title, story, gallery, result or name
 *   screen: swapped.
 */
#ifndef GGX_MENU_H
#define GGX_MENU_H

#include "core_host.h"

/* 1 if pad's X and Circle swap now under "confirm = x". */
int menu_confirms(CoreHost *h, int pad);

#endif
