/* ggx's input layer (PORT-010): SDL3 keyboards, gamepads and joysticks
 * (arcade sticks) to the two PS2 pads, as libpad delivers them
 * (common/input/pad.h).
 *
 * - Players. Each player has bindings for the sixteen PS2 buttons: keys,
 *   which belong to that player, and gamepad or joystick inputs, which apply
 *   to the devices assigned to that player. A device goes to player 1 when
 *   it connects, then player 2; a third and later go to player 1 too, and F5
 *   swaps the two players' devices. "pN.device = <SDL GUID>" pins one.
 * - Gamepads are SDL gamepads, by position (south, east...), so an Xbox
 *   pad, a DualSense, a Switch Pro and the Steam Deck's own controls all
 *   map the same way: south X, east Circle, west Square, north
 *   Triangle, the shoulders L1 R1, the triggers L2 R2, the d-pad and the
 *   left stick (past half way) the directions, back Select, start Start,
 *   the stick clicks L3 R3; the Deck's back buttons L4 R4 L5 R5 are L1 R1
 *   L2 R2. A joystick SDL has no gamepad mapping for (many arcade sticks)
 *   uses the DirectInput order PS2 adapters use: buttons 0-9 Square,
 *   X, Circle, Triangle, L1, R1, L2, R2, Select, Start; the first hat
 *   and the first two axes are the directions.
 * - Analog. Each pad is digital (0x41) unless "pN.analog = on" or F7 / F8
 *   (like the DualShock 2's ANALOG button) switch it to analog (0x73); then
 *   the game reads its directions from the left stick. A device without a
 *   stick (a keyboard, a d-pad, a hat) moves the left stick to its ends
 *   while the stick itself is centred, so it still steers.
 * - SOCD: "socd = hitbox" (the default) makes left+right neutral and
 *   up+down up, which a d-pad can't press; "socd = off" passes them on.
 * - Remapping: F3 (player 1) or F4 (player 2) asks for each button in turn
 *   (input_remap_prompt; ggx shows it in the window title). The next key,
 *   button, hat or axis pressed replaces that button's binding of the same
 *   kind (keyboard, gamepad or joystick); Escape keeps it. The bindings are
 *   saved in the config (config.h) as "p1.x = key:Z, pad:a" (PORT-019; "p1.cross" is still read).
 * - Rumble: the motors the game asks for (scePadSetActDirect), on the
 *   player's gamepads, unless "rumble = off".
 * - The controller family (ADR-019, for PORT-017's button prompts): the
 *   family of the last device used, live, or "prompts = <family>" to force
 *   one. Read only, and never part of the Core's state.
 * - Macros (PORT-018, common/input/macro.h): P AND K, S AND HS, P K S and
 *   P K S HS, bound like buttons ("p1.p_and_k = key:D, pad:leftstick") and
 *   asked for after the buttons when remapping. A macro presses the PS2
 *   buttons the game's own KEY CONFIG has for its parts, on the same field
 *   (input_set_game_keys), so all four can be bound at once and RESPECT and
 *   MEMORY ("p1.r1 = none") left off.
 * - Confirm (PORT-019, common/input/confirm.h): "confirm = x" (the
 *   default) or "circle", or per player "p1.confirm". With x, X and Circle
 *   swap on that player's pad wherever the game asks to confirm or cancel
 *   (input_set_menu, from common/host/menu.h), never in a fight.
 * - Prompt letters (PORT-017): per player, the letter on the button bound
 *   to Circle, X, Square and Triangle on the device that player used
 *   last: SDL's label on a gamepad (A, B, X, Y), the key on a keyboard.
 */
#ifndef GGX_INPUT_H
#define GGX_INPUT_H

#include <SDL3/SDL.h>

#include "config.h"
#include "family.h"
#include "macro.h"
#include "pad.h"

typedef struct Input Input;

/* The bindings and settings come from cfg (kept; may be NULL for the
   defaults). SDL must be initialised with SDL_INIT_GAMEPAD. */
Input *input_new(Config *cfg);
void input_free(Input *in);

/* Every SDL event goes through here. Returns 1 if the input layer used it
   (device changes, remapping). */
int input_event(Input *in, const SDL_Event *e);

/* Both pads as they are now. While remapping, nothing is pressed. */
void input_pads(Input *in, PadInput pads[2]);

/* The game's motors (CoreHost.actuators), once a field. */
void input_rumble(Input *in, const uint8_t act[2][2]);

/* The analog mode of a player's pad: toggled, or set. */
void input_toggle_analog(Input *in, int player);
int input_analog(const Input *in, int player);
/* Swaps the players' devices. */
void input_swap_players(Input *in);

/* Remapping one player's buttons; the prompt is NULL once it is over. */
void input_remap_start(Input *in, int player);
const char *input_remap_prompt(const Input *in);

/* Whether the game is asking this player's pad to confirm or cancel now
   (menu_confirms), set before each field's input_pads. */
void input_set_menu(Input *in, int player, int menu);
/* A player's confirm button (CONFIRM_X or CONFIRM_CIRCLE). Set with keep,
   it also goes in the config as "pN.confirm" (the caller saves it);
   without, it holds for this run only (ggx --confirm). */
int input_confirm(const Input *in, int player);
void input_set_confirm(Input *in, int player, int confirm, int keep);

/* The game's KEY CONFIG for a player's pad (USER_KEYSET's PUNCH, KICK,
   SLASH and HSLASH buttons), which the macros press. Until set, the game's
   defaults. */
void input_set_game_keys(Input *in, int player, const MacroKeys *k);

/* Forces a family for the prompts (input_family_parse's value), or -1 for
   auto, over the config's "prompts" for this run (ggx --prompts). */
void input_force_family(Input *in, int family);
/* The family to show prompts for: the forced one, or the last device's. */
InputFamily input_family(const Input *in);
/* The letters a player's Circle, X, Square and Triangle show (in that
   order; prompts/pr.h): the label of the gamepad button bound to each on
   the player's last device, or the bound key's letter or digit on the
   keyboard; 0 where there is none (a PlayStation pad's shapes, a
   joystick, a trigger, a key like Enter). With a forced family, that
   family's labels for the bound gamepad buttons (PlayStation and other:
   none). */
void input_prompt_letters(const Input *in, int player, char out[4]);
/* The player whose device was used last (0 or 1). */
int input_last_player(const Input *in);

/* A gamepad's family from what SDL knows of it (exported for the tests). */
InputFamily input_gamepad_family(SDL_GamepadType type, Uint16 vendor, Uint16 product, int steam_virtual);

/* Running on a Steam Deck (Steam's SteamDeck=1, or the Deck's DMI name). */
int input_on_steam_deck(void);

/* A line per connected device, for --input-info and the log. */
void input_describe(const Input *in, FILE *out);

#endif
