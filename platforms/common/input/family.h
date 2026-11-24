/* The controller family button prompts follow (ADR-019; PORT-010 detects
 * it, PORT-017 draws the prompts). The input layer (sdl/input.h) sets it
 * from the last device used, or a forced one; the renderer and the host
 * read it. It never enters the Core or its state. No SDL here, so the
 * renderer can hold it.
 *
 * The enum and its names are port-sdl-input's (game-kit, psi/family.h); these
 * are the names the port has always used for them.
 */
#ifndef GGX_FAMILY_H
#define GGX_FAMILY_H

#include "psi/family.h"

typedef PsiFamily InputFamily;

#define INPUT_FAMILY_PLAYSTATION PSI_FAMILY_PLAYSTATION   /* DualShock 2/3/4, DualSense: the game's own PS2 icons */
#define INPUT_FAMILY_XBOX PSI_FAMILY_XBOX                 /* Xbox 360 / One / Series and other XInput pads */
#define INPUT_FAMILY_STEAM PSI_FAMILY_STEAM               /* Steam Deck, Steam Controller */
#define INPUT_FAMILY_OTHER PSI_FAMILY_OTHER               /* other gamepads, joysticks and arcade sticks */
#define INPUT_FAMILY_KEYBOARD PSI_FAMILY_KEYBOARD
#define INPUT_FAMILY_NINTENDO PSI_FAMILY_NINTENDO         /* Switch Pro, Joy-Con */
#define INPUT_FAMILY_COUNT PSI_FAMILY_COUNT

#define input_family_name psi_family_name
/* -1 if name isn't one */
#define input_family_parse psi_family_parse

#endif
