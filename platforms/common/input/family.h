/* The controller family button prompts follow (ADR-019; PORT-010 detects
 * it, PORT-017 draws the prompts). The input layer (sdl/input.h) sets it
 * from the last device used, or a forced one; the renderer and the host
 * read it. It never enters the Core or its state. No SDL here, so the
 * renderer can hold it.
 */
#ifndef GGX_FAMILY_H
#define GGX_FAMILY_H

#include <string.h>

typedef enum {
    INPUT_FAMILY_PLAYSTATION,   /* DualShock 2/3/4, DualSense: the game's own PS2 icons */
    INPUT_FAMILY_XBOX,          /* Xbox 360 / One / Series and other XInput pads */
    INPUT_FAMILY_STEAM,         /* Steam Deck, Steam Controller */
    INPUT_FAMILY_OTHER,         /* other gamepads, joysticks and arcade sticks */
    INPUT_FAMILY_KEYBOARD,
    INPUT_FAMILY_NINTENDO,      /* Switch Pro, Joy-Con */
    INPUT_FAMILY_COUNT
} InputFamily;

static inline const char *input_family_name(InputFamily f)
{
    static const char *const N[] = { "playstation", "xbox", "steam", "other", "keyboard", "nintendo" };

    return (unsigned)f < INPUT_FAMILY_COUNT ? N[f] : "other";
}

/* -1 if name isn't one */
static inline int input_family_parse(const char *name)
{
    int f;

    for (f = 0; f < INPUT_FAMILY_COUNT; f++) {
        if (!strcmp(name, input_family_name((InputFamily)f))) {
            return f;
        }
    }
    return -1;
}

#endif
