/* Button macros (PORT-018): one key or button presses several of the
 * game's buttons together.
 *
 * The game's KEY CONFIG gives each of its functions one PS2 button:
 * PUNCH, KICK, SLASH, HSLASH, RESPECT, MEMORY and the four combinations
 * P AND K, S AND HS, P K S and P K S HS (USER_KEYSET, opt_cfg.c). A PS2 pad
 * has eight buttons for those ten, so a player who wants all four
 * combinations must give up RESPECT and MEMORY. The port's macros are the
 * same four combinations, bound in the input layer instead: a macro
 * presses the PS2 buttons the game's KEY CONFIG has for each of its parts
 * (P AND K presses PUNCH's and KICK's), on the same field, so the game sees
 * exactly what a PS2 pad with those buttons held together sends. Nothing
 * reaches the Core but pad bytes. No SDL here.
 */
#ifndef GGX_MACRO_H
#define GGX_MACRO_H

#include <stdint.h>

enum { MACRO_P_AND_K, MACRO_S_AND_HS, MACRO_P_K_S, MACRO_P_K_S_HS, MACRO_COUNT };

/* The game's PS2 buttons for PUNCH, KICK, SLASH and HSLASH (pad.h bits):
   USER_KEYSET's first four words for that pad. */
typedef struct {
    uint32_t punch, kick, slash, hslash;
} MacroKeys;

/* The game's defaults (opt_cfg.c set_default): Square, X, Triangle,
   Circle. */
void macro_default_keys(MacroKeys *k);

/* The macro's name as KEY CONFIG writes it ("P AND K"), and its config key
   suffix ("p_and_k"). */
const char *macro_name(int m);
const char *macro_config_name(int m);
/* MACRO_* for a config key suffix, or -1. */
int macro_by_config_name(const char *name);

/* The pad buttons (pad.h bits) macro m presses under the game's keys. A
   part the game's KEY CONFIG doesn't have as one of the eight buttons
   (unset, or while the game hasn't loaded its settings) uses the
   default's. */
uint16_t macro_buttons(int m, const MacroKeys *k);

#endif
