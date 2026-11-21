/* ggx-input-test: the input layer (sdl/input.h) with SDL's virtual
 * controllers (PORT-010). No window, GPU, disc or real controller.
 *
 *   ggx-input-test [<scratch dir>]
 *
 * An Xbox Series pad, a DualSense, a Steam Deck's controls (Valve
 * 28de:1205, tested as the generic SDL gamepad it is) and an arcade stick
 * SDL has no gamepad mapping for are attached, and keyboard events are
 * pushed. It checks the players they go to, each one's buttons, sticks
 * and triggers as PS2 pad state, SOCD, analog mode, F5's swap, remapping
 * and its saved bindings, rumble, and the controller family (ADR-019),
 * live and forced. The config file is written in the scratch directory
 * (default: the current one).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "config.h"
#include "confirm.h"
#include "input.h"

static int failures;

#define CHECK(cond, ...)                                     \
    do {                                                     \
        if (!(cond)) {                                       \
            failures++;                                      \
            fprintf(stderr, "FAIL line %d: ", __LINE__);     \
            fprintf(stderr, __VA_ARGS__);                    \
            fputc('\n', stderr);                             \
        }                                                    \
    } while (0)

static Uint16 rumble_lo, rumble_hi;

static bool SDLCALL on_rumble(void *ud, Uint16 lo, Uint16 hi)
{
    (void)ud;
    rumble_lo = lo;
    rumble_hi = hi;
    return true;
}

static SDL_JoystickID attach(const char *name, Uint16 vid, Uint16 pid, int gamepad)
{
    SDL_VirtualJoystickDesc d;
    SDL_JoystickID id;

    SDL_INIT_INTERFACE(&d);
    d.vendor_id = vid;
    d.product_id = pid;
    d.name = name;
    d.Rumble = on_rumble;
    if (gamepad) {
        d.type = SDL_JOYSTICK_TYPE_GAMEPAD;
        d.naxes = SDL_GAMEPAD_AXIS_COUNT;
        d.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
        d.button_mask = 0x1fffff;   /* the standard buttons and the four paddles */
        d.axis_mask = (1u << SDL_GAMEPAD_AXIS_COUNT) - 1;
    } else {
        /* a DirectInput arcade stick: ten buttons, a hat, two axes, no gamepad mapping */
        d.type = SDL_JOYSTICK_TYPE_ARCADE_STICK;
        d.naxes = 2;
        d.nbuttons = 10;
        d.nhats = 1;
    }
    id = SDL_AttachVirtualJoystick(&d);
    CHECK(id != 0, "attach %s: %s", name, SDL_GetError());
    return id;
}

/* An event from a real controller (a test PC may have one plugged in): the
   test sees only its virtual ones. Removals pass, as a removed virtual
   joystick is no longer known as virtual. */
static int physical(const SDL_Event *e)
{
    SDL_JoystickID id;

    switch (e->type) {
    case SDL_EVENT_GAMEPAD_ADDED:
        id = e->gdevice.which;
        break;
    case SDL_EVENT_JOYSTICK_ADDED:
        id = e->jdevice.which;
        break;
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        id = e->gbutton.which;
        break;
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        id = e->gaxis.which;
        break;
    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
        id = e->jbutton.which;
        break;
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
        id = e->jhat.which;
        break;
    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        id = e->jaxis.which;
        break;
    default:
        return 0;
    }
    return !SDL_IsJoystickVirtual(id);
}

static void pump(Input *in)
{
    SDL_Event e;

    SDL_UpdateJoysticks();
    while (SDL_PollEvent(&e)) {
        if (!physical(&e)) {
            input_event(in, &e);
        }
    }
}

static void key(Input *in, SDL_Scancode k, int down)
{
    SDL_Event e;

    memset(&e, 0, sizeof(e));
    e.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    e.key.scancode = k;
    e.key.down = down != 0;
    input_event(in, &e);
}

static void pads(Input *in, PadInput p[2])
{
    pump(in);
    input_pads(in, p);
}

/* A player's prompt letters (input_prompt_letters) as a string, '.' for none. */
static const char *letters(Input *in, int player)
{
    static char s[2][5];
    char l[4];
    int i;

    input_prompt_letters(in, player, l);
    for (i = 0; i < 4; i++) {
        s[player][i] = l[i] ? l[i] : '.';
    }
    s[player][4] = 0;
    return s[player];
}

/* a gamepad's joystick, for setting its virtual state */
static SDL_Joystick *joy_of(SDL_JoystickID id)
{
    SDL_Joystick *j = SDL_GetJoystickFromID(id);

    CHECK(j != NULL, "no joystick %u", (unsigned)id);
    return j;
}

int main(int argc, char **argv)
{
    char path[1024];
    const char *dir = argc > 1 ? argv[1] : ".";
    Config *cfg;
    Input *in;
    PadInput p[2];
    SDL_JoystickID xbox, ds, deck, stick, sw;
    SDL_Joystick *jx, *jd, *jk, *js;

    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "0");   /* only the virtual controllers */
    if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_EVENTS)) {
        fprintf(stderr, "ggx-input-test: SDL: %s\n", SDL_GetError());
        return 1;
    }
    snprintf(path, sizeof(path), "%s/ggx-input-test.cfg", dir);
    remove(path);
    cfg = config_load(path);
    in = input_new(cfg);
    CHECK(in != NULL, "input_new");

    /* the families SDL's types and IDs give */
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_PS5, 0x054c, 0x0ce6, 0) == INPUT_FAMILY_PLAYSTATION, "DualSense");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_PS3, 0x054c, 0x0268, 0) == INPUT_FAMILY_PLAYSTATION, "DualShock 3");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_XBOXONE, 0x045e, 0x0b12, 0) == INPUT_FAMILY_XBOX, "Xbox Series");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_XBOX360, 0x045e, 0x028e, 0) == INPUT_FAMILY_XBOX, "Xbox 360");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_STANDARD, 0x28de, 0x1205, 0) == INPUT_FAMILY_STEAM, "Steam Deck");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_STANDARD, 0x28de, 0x1142, 0) == INPUT_FAMILY_STEAM, "Steam Controller");
    /* Steam Input's virtual pad: the Deck's own controls on a Deck; whatever it says it is elsewhere */
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_XBOX360, 0x28de, 0x11ff, 1) == INPUT_FAMILY_STEAM, "Deck via Steam Input");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_XBOX360, 0x28de, 0x11ff, 0) == INPUT_FAMILY_XBOX, "Steam Input elsewhere");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_PS4, 0x28de, 0x11ff, 1) == INPUT_FAMILY_PLAYSTATION,
          "a DualShock 4 through Steam Input on a Deck");
    CHECK(input_gamepad_family(SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO, 0x057e, 0x2009, 0) == INPUT_FAMILY_NINTENDO, "Switch Pro");

    /* the keyboard: player 1's default keys */
    CHECK(input_family(in) == INPUT_FAMILY_KEYBOARD, "family before any device: %s", input_family_name(input_family(in)));
    key(in, SDL_SCANCODE_Z, 1);
    key(in, SDL_SCANCODE_LEFT, 1);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_CROSS | PAD_LEFT) && p[0].mode == PAD_MODE_DIGITAL, "keys: %04x", p[0].buttons);
    CHECK(p[1].buttons == 0, "player 2 has no keys: %04x", p[1].buttons);
    /* prompt letters (PORT-017), circle cross square triangle: the keys X Z A S; player 2 has none */
    CHECK(!strcmp(letters(in, 0), "XZAS"), "keyboard letters: %s", letters(in, 0));
    CHECK(!strcmp(letters(in, 1), "...."), "player 2's letters: %s", letters(in, 1));
    key(in, SDL_SCANCODE_RIGHT, 1);
    key(in, SDL_SCANCODE_UP, 1);
    key(in, SDL_SCANCODE_DOWN, 1);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_CROSS | PAD_UP), "SOCD: left+right neutral, up+down up: %04x", p[0].buttons);
    key(in, SDL_SCANCODE_Z, 0);
    key(in, SDL_SCANCODE_LEFT, 0);
    key(in, SDL_SCANCODE_RIGHT, 0);
    key(in, SDL_SCANCODE_UP, 0);
    key(in, SDL_SCANCODE_DOWN, 0);

    /* three gamepads and an arcade stick: players 1, 2, then 1 */
    xbox = attach("Xbox Series (virtual)", 0x045e, 0x0b12, 1);
    pump(in);
    ds = attach("DualSense (virtual)", 0x054c, 0x0ce6, 1);
    pump(in);
    deck = attach("Steam Deck (virtual)", 0x28de, 0x1205, 1);
    pump(in);
    stick = attach("Arcade stick (virtual)", 0x0f0d, 0x0011, 0);
    pump(in);
    input_describe(in, stdout);
    CHECK(SDL_IsGamepad(xbox) && SDL_IsGamepad(ds) && SDL_IsGamepad(deck), "the virtual gamepads are gamepads");
    CHECK(!SDL_IsGamepad(stick), "the arcade stick has no gamepad mapping");
    jx = joy_of(xbox), jd = joy_of(ds), jk = joy_of(deck), js = joy_of(stick);

    /* Xbox pad, player 1: positions, the stick past half way, the trigger */
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, true);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, true);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTX, -30000);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFT_TRIGGER, 20000);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_CROSS | PAD_R1 | PAD_LEFT | PAD_L2), "Xbox: %04x", p[0].buttons);
    CHECK(input_family(in) == INPUT_FAMILY_XBOX, "family after the Xbox pad: %s", input_family_name(input_family(in)));
    CHECK(!strcmp(letters(in, 0), "BAXY"), "Xbox letters: %s", letters(in, 0));
    CHECK(input_last_player(in) == 0, "last player after the Xbox pad");
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTX, -10000);   /* inside half way */
    pads(in, p);
    CHECK(p[0].buttons == (PAD_CROSS | PAD_R1 | PAD_L2), "Xbox, stick inside half way: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, false);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, false);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTX, 0);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFT_TRIGGER, SDL_JOYSTICK_AXIS_MIN);   /* a trigger at rest */

    /* macros (PORT-018): L3 is P AND K, R3 S AND HS: both parts on the same field, nothing else */
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_LEFT_STICK, true);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_SQUARE | PAD_CROSS), "P AND K on L3: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_RIGHT_STICK, true);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE | PAD_CIRCLE), "both macros: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_LEFT_STICK, false);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_RIGHT_STICK, false);
    pads(in, p);
    CHECK(p[0].buttons == 0, "macros released together: %04x", p[0].buttons);
    {
        /* the game's KEY CONFIG with P on L2: P AND K follows it */
        MacroKeys k;

        macro_default_keys(&k);
        k.punch = PAD_L2;
        input_set_game_keys(in, 0, &k);
        SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_LEFT_STICK, true);
        pads(in, p);
        CHECK(p[0].buttons == (PAD_L2 | PAD_CROSS), "P AND K with the game's P on L2: %04x", p[0].buttons);
        SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_LEFT_STICK, false);
        macro_default_keys(&k);
        input_set_game_keys(in, 0, &k);
    }
    /* each keyboard macro on its own: D, F, C, V */
    {
        static const struct {
            SDL_Scancode k;
            uint16_t want;
        } KM[] = {
            { SDL_SCANCODE_D, PAD_SQUARE | PAD_CROSS },
            { SDL_SCANCODE_F, PAD_TRIANGLE | PAD_CIRCLE },
            { SDL_SCANCODE_C, PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE },
            { SDL_SCANCODE_V, PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE | PAD_CIRCLE },
        };
        size_t m;

        for (m = 0; m < sizeof(KM) / sizeof(KM[0]); m++) {
            key(in, KM[m].k, 1);
            pads(in, p);
            CHECK(p[0].buttons == KM[m].want, "keyboard macro %d: %04x, not %04x", (int)m, p[0].buttons, KM[m].want);
            key(in, KM[m].k, 0);
            pads(in, p);
        }
    }
    /* the keyboard's macros: D is P AND K, V P K S HS; with a direction, SOCD still applies */
    key(in, SDL_SCANCODE_V, 1);
    key(in, SDL_SCANCODE_LEFT, 1);
    key(in, SDL_SCANCODE_RIGHT, 1);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE | PAD_CIRCLE), "P K S HS on V, left+right: %04x",
          p[0].buttons);
    key(in, SDL_SCANCODE_V, 0);
    key(in, SDL_SCANCODE_LEFT, 0);
    key(in, SDL_SCANCODE_RIGHT, 0);
    /* the Xbox pad again, so it is player 1's last device */
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, true);
    pump(in);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, false);
    pads(in, p);

    /* X confirms in menus (PORT-019, the default): south (X) is Circle there, Circle (east) is X; never in a fight */
    CHECK(input_confirm(in, 0) == CONFIRM_X, "confirm defaults to X");
    input_set_menu(in, 0, 1);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, true);
    pads(in, p);
    CHECK(p[0].buttons == PAD_CIRCLE, "menu, X: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, false);
    pads(in, p);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_EAST, true);
    pads(in, p);
    CHECK(p[0].buttons == PAD_CROSS, "menu, Circle: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_EAST, false);
    input_set_menu(in, 0, 0);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, true);
    pads(in, p);
    CHECK(p[0].buttons == PAD_CROSS, "fight, X: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, false);
    /* "circle": as the PS2 */
    input_set_confirm(in, 0, CONFIRM_CIRCLE, 0);
    input_set_menu(in, 0, 1);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, true);
    pads(in, p);
    CHECK(p[0].buttons == PAD_CROSS, "confirm = circle, menu, X: %04x", p[0].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_SOUTH, false);
    input_set_menu(in, 0, 0);
    input_set_confirm(in, 0, CONFIRM_X, 0);
    pads(in, p);

    /* DualSense, player 2 */
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_EAST, true);
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_START, true);
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_DPAD_DOWN, true);
    pads(in, p);
    CHECK(p[0].buttons == 0 && p[1].buttons == (PAD_CIRCLE | PAD_START | PAD_DOWN), "DualSense: %04x %04x", p[0].buttons,
          p[1].buttons);
    CHECK(input_family(in) == INPUT_FAMILY_PLAYSTATION, "family after the DualSense: %s",
          input_family_name(input_family(in)));
    CHECK(!strcmp(letters(in, 1), "...."), "DualSense letters (the shapes): %s", letters(in, 1));
    CHECK(!strcmp(letters(in, 0), "BAXY"), "player 1 still the Xbox pad: %s", letters(in, 0));
    CHECK(input_last_player(in) == 1, "last player after the DualSense");
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_EAST, false);
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_START, false);
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_DPAD_DOWN, false);

    /* the Steam Deck's controls, player 1 (a third device): the back buttons are macros (PORT-018) */
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_WEST, true);
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1, true);   /* R4 */
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_LEFT_PADDLE2, true);    /* L5 */
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_BACK, true);
    pads(in, p);
    /* west (square), R4 (S AND HS), L5 (P K S), back (select) */
    CHECK(p[0].buttons == (PAD_SQUARE | PAD_TRIANGLE | PAD_CIRCLE | PAD_CROSS | PAD_SELECT), "Deck: %04x", p[0].buttons);
    CHECK(input_family(in) == INPUT_FAMILY_STEAM, "family after the Deck: %s", input_family_name(input_family(in)));
    CHECK(!strcmp(letters(in, 0), "BAXY"), "Deck letters: %s", letters(in, 0));
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_WEST, false);
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1, false);
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_LEFT_PADDLE2, false);
    SDL_SetJoystickVirtualButton(jk, SDL_GAMEPAD_BUTTON_BACK, false);

    /* the arcade stick, a plain joystick: the fourth device, so player 1 */
    SDL_SetJoystickVirtualButton(js, 0, true);
    SDL_SetJoystickVirtualButton(js, 9, true);
    SDL_SetJoystickVirtualHat(js, 0, SDL_HAT_RIGHTUP);
    pads(in, p);
    CHECK(p[0].buttons == (PAD_SQUARE | PAD_START | PAD_RIGHT | PAD_UP), "arcade stick: %04x", p[0].buttons);
    CHECK(input_family(in) == INPUT_FAMILY_OTHER, "family after the stick: %s", input_family_name(input_family(in)));
    CHECK(!strcmp(letters(in, 0), "...."), "arcade stick letters (none): %s", letters(in, 0));
    SDL_SetJoystickVirtualButton(js, 0, false);
    SDL_SetJoystickVirtualButton(js, 9, false);
    SDL_SetJoystickVirtualHat(js, 0, SDL_HAT_CENTERED);
    pads(in, p);
    CHECK(p[0].buttons == 0 && p[1].buttons == 0, "all released: %04x %04x", p[0].buttons, p[1].buttons);

    /* a Switch Pro pad (player 1): Nintendo's letters on the same positions, B at the bottom */
    sw = attach("Switch Pro (virtual)", 0x057e, 0x2009, 1);
    pump(in);
    SDL_SetJoystickVirtualButton(joy_of(sw), SDL_GAMEPAD_BUTTON_EAST, true);
    pump(in);
    pads(in, p);
    CHECK(input_family(in) == INPUT_FAMILY_NINTENDO, "family after the Switch Pro: %s",
          input_family_name(input_family(in)));
    CHECK(!strcmp(letters(in, 0), "ABYX"), "Switch Pro letters: %s", letters(in, 0));
    SDL_SetJoystickVirtualButton(joy_of(sw), SDL_GAMEPAD_BUTTON_EAST, false);
    pump(in);
    /* a key of player 1's: the keyboard again */
    key(in, SDL_SCANCODE_X, 1);
    key(in, SDL_SCANCODE_X, 0);
    CHECK(!strcmp(letters(in, 0), "XZAS"), "back on the keyboard: %s", letters(in, 0));
    SDL_DetachVirtualJoystick(sw);
    pump(in);

    /* F5: the players' devices swap */
    input_swap_players(in);
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_NORTH, true);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_NORTH, true);
    pads(in, p);
    CHECK(p[0].buttons == PAD_TRIANGLE && p[1].buttons == PAD_TRIANGLE, "swapped: %04x %04x", p[0].buttons, p[1].buttons);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_NORTH, false);
    pads(in, p);
    CHECK(p[0].buttons == PAD_TRIANGLE && p[1].buttons == 0, "the DualSense is player 1 now: %04x %04x", p[0].buttons,
          p[1].buttons);
    SDL_SetJoystickVirtualButton(jd, SDL_GAMEPAD_BUTTON_NORTH, false);
    input_swap_players(in);

    /* analog mode (F7): the sticks' bytes, 0x73; a key steers the centred left stick */
    input_toggle_analog(in, 0);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTX, 32767);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTY, -32768);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_RIGHTX, 0);
    pads(in, p);
    CHECK(p[0].mode == PAD_MODE_ANALOG && p[0].analog[2] == 0xFF && p[0].analog[3] == 0x00 && p[0].analog[0] == 0x80,
          "analog sticks: %02x %02x %02x %02x", p[0].analog[0], p[0].analog[1], p[0].analog[2], p[0].analog[3]);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTX, 0);
    SDL_SetJoystickVirtualAxis(jx, SDL_GAMEPAD_AXIS_LEFTY, 0);
    key(in, SDL_SCANCODE_LEFT, 1);
    pads(in, p);
    CHECK(p[0].analog[2] == 0x00 && p[0].analog[3] == 0x80 && (p[0].buttons & PAD_LEFT), "a key in analog mode: %02x %02x",
          p[0].analog[2], p[0].analog[3]);
    key(in, SDL_SCANCODE_LEFT, 0);
    CHECK(p[1].mode == PAD_MODE_DIGITAL, "player 2 stays digital");
    input_toggle_analog(in, 0);
    CHECK(!strcmp(config_get_or(cfg, "p1.analog", ""), "off"), "p1.analog saved");

    /* rumble: the game's motors on player 1's gamepads */
    {
        const uint8_t act[2][2] = { { 0x80, 0xC0 }, { 0, 0 } };

        rumble_lo = rumble_hi = 0;
        input_rumble(in, act);
        pump(in);
        CHECK(rumble_lo == 0xC0 * 257 && rumble_hi == 0xFFFF, "rumble %04x %04x", rumble_lo, rumble_hi);
    }

    /* remapping player 1: Up to J from the keyboard, Down to the Xbox pad's east button, then Escape for the rest */
    input_remap_start(in, 0);
    CHECK(input_remap_prompt(in) && strstr(input_remap_prompt(in), "Up"), "prompt: %s", input_remap_prompt(in));
    SDL_Delay(320);
    key(in, SDL_SCANCODE_J, 1);
    key(in, SDL_SCANCODE_J, 0);
    CHECK(input_remap_prompt(in) && strstr(input_remap_prompt(in), "Down"), "prompt: %s", input_remap_prompt(in));
    SDL_Delay(320);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_EAST, true);
    pump(in);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_EAST, false);
    pump(in);
    {
        int macro_seen = 0, x_seen = 0;

        while (input_remap_prompt(in)) {
            /* the PS2's X is "X", never "Cross" (PORT-019) */
            x_seen |= strstr(input_remap_prompt(in), "press X (") != NULL;
            CHECK(!strstr(input_remap_prompt(in), "Cross"), "prompt says Cross: %s", input_remap_prompt(in));
            /* the macros are asked for after the buttons, with what they are bound to now */
            macro_seen |= strstr(input_remap_prompt(in), "P AND K (now key:D, pad:leftstick, pad:l4") != NULL;
            key(in, SDL_SCANCODE_ESCAPE, 1);
            key(in, SDL_SCANCODE_ESCAPE, 0);
        }
        CHECK(macro_seen, "remapping never asked for P AND K with its bindings");
        CHECK(x_seen, "remapping never asked for X");
        CHECK(config_get(cfg, "p1.x") && !config_get(cfg, "p1.cross"), "saved as p1.x");
    }
    key(in, SDL_SCANCODE_J, 1);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_EAST, true);
    pads(in, p);
    /* J is Up, east is Down (and still Circle); up+down is up */
    CHECK(p[0].buttons == (PAD_UP | PAD_CIRCLE), "remapped: %04x", p[0].buttons);
    key(in, SDL_SCANCODE_J, 0);
    SDL_SetJoystickVirtualButton(jx, SDL_GAMEPAD_BUTTON_EAST, false);
    key(in, SDL_SCANCODE_UP, 1);
    pads(in, p);
    CHECK(p[0].buttons == 0, "the Up key no longer moves: %04x", p[0].buttons);
    key(in, SDL_SCANCODE_UP, 0);
    CHECK(strstr(config_get_or(cfg, "p1.up", ""), "key:J") && strstr(config_get_or(cfg, "p1.up", ""), "pad:dpup"),
          "p1.up = %s", config_get_or(cfg, "p1.up", "(none)"));
    CHECK(!strcmp(config_get_or(cfg, "p1.down", ""), "key:Down, joy:h0.down, joy:a1+, pad:east"), "p1.down = %s",
          config_get_or(cfg, "p1.down", "(none)"));

    /* the saved file, read back */
    input_free(in);
    config_free(cfg);
    cfg = config_load(path);
    config_set(cfg, "prompts", "xbox");
    in = input_new(cfg);
    pump(in);
    key(in, SDL_SCANCODE_J, 1);
    pads(in, p);
    CHECK(p[0].buttons == PAD_UP, "bindings from the file: %04x", p[0].buttons);
    key(in, SDL_SCANCODE_J, 0);
    CHECK(input_family(in) == INPUT_FAMILY_XBOX, "prompts = xbox forces the family");
    CHECK(!strcmp(letters(in, 0), "BAXY") && !strcmp(letters(in, 1), "BAXY"), "forced Xbox letters: %s %s",
          letters(in, 0), letters(in, 1));
    input_force_family(in, INPUT_FAMILY_NINTENDO);
    CHECK(!strcmp(letters(in, 0), "ABYX"), "forced Nintendo letters: %s", letters(in, 0));
    input_force_family(in, INPUT_FAMILY_PLAYSTATION);
    CHECK(!strcmp(letters(in, 0), "...."), "forced PlayStation letters: %s", letters(in, 0));

    /* a config from before PORT-019 says "p1.cross" */
    {
        Config *old = config_load("");
        Input *oin;

        config_set(old, "p1.cross", "key:K");
        oin = input_new(old);
        key(oin, SDL_SCANCODE_K, 1);
        input_pads(oin, p);
        CHECK(p[0].buttons == PAD_CROSS, "p1.cross still read: %04x", p[0].buttons);
        key(oin, SDL_SCANCODE_K, 0);
        input_free(oin);
        config_free(old);
    }

    /* unplugging */
    SDL_DetachVirtualJoystick(deck);
    SDL_DetachVirtualJoystick(stick);
    pump(in);
    input_describe(in, stdout);

    input_free(in);
    config_free(cfg);
    SDL_DetachVirtualJoystick(xbox);
    SDL_DetachVirtualJoystick(ds);
    SDL_Quit();
    remove(path);
    if (failures) {
        fprintf(stderr, "ggx-input-test: %d failures\n", failures);
        return 1;
    }
    printf("ggx-input-test: ok\n");
    return 0;
}
