/* ggx's input layer (input.h) on port-sdl-input (game-kit): the devices, players, bindings, SOCD, analog, remap capture
 * and prompt letters are the package's. What stays here is Guilty Gear's: its sixteen PS2 buttons and four macros as
 * data (the layout), the macros' bits from the game's own KEY CONFIG, which button confirms in menus, the PS2 pad's
 * analog byte order, and the rumble motors' scePadSetActDirect shape. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confirm.h"
#include "input.h"
#include "macro.h"

#include "psi/psi.h"

/* a player's binding slots: the sixteen PS2 buttons (pad.h bit indices), then the macros (macro.h) */
#define NSLOTS (PAD_NBUTTONS + MACRO_COUNT)

/* The defaults. The game's own KEY CONFIG uses Square, X, Triangle, Circle, R1 (RESPECT) and L1 (MEMORY), and
   it reads L2 and R2 at the character select, so those keep their usual places. It never reads L3 or R3, so the
   stick clicks are the macros', with the back buttons (the Steam Deck's, an Elite pad's) and a DualSense's
   touchpad. The names are the ones a player reads (X, never Cross) and the config keys, "p1.x"; "p1.cross" is still read. */
static const PsiSlot SLOTS[NSLOTS] = {
    /* the sixteen PS2 buttons, by pad.h bit index */
    { "L2", "l2", NULL, PAD_L2, "key:E, pad:lefttrigger+, joy:b6" },
    { "R2", "r2", NULL, PAD_R2, "key:R, pad:righttrigger+, joy:b7" },
    { "L1", "l1", NULL, PAD_L1, "key:Q, pad:leftshoulder, joy:b4" },
    { "R1", "r1", NULL, PAD_R1, "key:W, pad:rightshoulder, joy:b5" },
    { "Triangle", "triangle", NULL, PAD_TRIANGLE, "key:S, pad:north, joy:b3" },
    { "Circle", "circle", NULL, PAD_CIRCLE, "key:X, pad:east, joy:b2" },
    { "X", "x", "cross", PAD_CROSS, "key:Z, pad:south, joy:b1" },
    { "Square", "square", NULL, PAD_SQUARE, "key:A, pad:west, joy:b0" },
    { "Select", "select", NULL, PAD_SELECT, "key:Right Shift, pad:back, joy:b8" },
    { "L3", "l3", NULL, PAD_L3, "none" },
    { "R3", "r3", NULL, PAD_R3, "none" },
    { "Start", "start", NULL, PAD_START, "key:Return, pad:start, joy:b9" },
    { "Up", "up", NULL, PAD_UP, "key:Up, pad:dpup, pad:lefty-, joy:h0.up, joy:a1-" },
    { "Right", "right", NULL, PAD_RIGHT, "key:Right, pad:dpright, pad:leftx+, joy:h0.right, joy:a0+" },
    { "Down", "down", NULL, PAD_DOWN, "key:Down, pad:dpdown, pad:lefty+, joy:h0.down, joy:a1+" },
    { "Left", "left", NULL, PAD_LEFT, "key:Left, pad:dpleft, pad:leftx-, joy:h0.left, joy:a0-" },
    /* the macros (macro.h): their bits come from the game's KEY CONFIG (input_set_game_keys) */
    { "P AND K", "p_and_k", NULL, 0, "key:D, pad:leftstick, pad:l4" },
    { "S AND HS", "s_and_hs", NULL, 0, "key:F, pad:rightstick, pad:r4" },
    { "P K S", "p_k_s", NULL, 0, "key:C, pad:touchpad, pad:l5" },
    { "P K S HS", "p_k_s_hs", NULL, 0, "key:V, pad:r5" },
};

/* the order remapping asks for the buttons in, as slots: pad.h bit indices, then the macros */
static const int REMAP_ORDER[] = { 12, 14, 15, 13, 7, 6, 4, 5, 2, 3, 0, 1, 11, 8, 9, 10, 16, 17, 18, 19 };

static const PsiLayout LAYOUT = {
    SLOTS, NSLOTS, PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT, REMAP_ORDER, (int)(sizeof(REMAP_ORDER) / sizeof(REMAP_ORDER[0])),
    PAD_CROSS, PAD_CIRCLE,
};

struct Input {
    Config *cfg;
    PsiInput *psi;
    MacroKeys game_keys[2];   /* the game's KEY CONFIG, for the macros */
    int confirm[2];           /* CONFIRM_X or CONFIRM_CIRCLE, per player */
    int menu[2];              /* the game is asking that pad to confirm or cancel (menu.h) */
    uint8_t act[2][2];
};

static int s_only_virtual;

void input_only_virtual(int on)
{
    s_only_virtual = on != 0;
}

int input_on_steam_deck(void)
{
    return psi_on_steam_deck();
}

InputFamily input_gamepad_family(SDL_GamepadType type, Uint16 vendor, Uint16 product, int on_deck)
{
    return (InputFamily)psi_gamepad_family(type, vendor, product, on_deck);
}

/* The macros' bits for a player, under the game's KEY CONFIG. */
static void apply_game_keys(Input *in, int player)
{
    int m;

    for (m = 0; m < MACRO_COUNT; m++) {
        psi_set_slot_mask(in->psi, player, PAD_NBUTTONS + m, macro_buttons(m, &in->game_keys[player]));
    }
}

/* ------------------------------------------------------------------ the layer */

Input *input_new(Config *cfg)
{
    Input *in = calloc(1, sizeof(Input));
    PsiOptions o;
    int p;

    if (!in) {
        return NULL;
    }
    in->cfg = cfg;
    memset(&o, 0, sizeof(o));
    o.layout = &LAYOUT;
    o.name = "ggx";
    o.only_virtual = s_only_virtual;
    o.store.ctx = cfg;
    o.store.get = pset_store_get;
    o.store.set = pset_store_set;
    o.store.save = pset_store_save;
    in->psi = psi_new(&o);
    if (!in->psi) {
        free(in);
        return NULL;
    }
    for (p = 0; p < 2; p++) {
        char key[32];
        const char *v;

        macro_default_keys(&in->game_keys[p]);
        apply_game_keys(in, p);
        snprintf(key, sizeof(key), "p%d.confirm", p + 1);
        v = config_get_or(cfg, key, config_get_or(cfg, "confirm", "x"));
        in->confirm[p] = confirm_parse(v);
        if (in->confirm[p] < 0) {
            fprintf(stderr, "ggx: confirm = %s: not x or circle; using x\n", v);
            in->confirm[p] = CONFIRM_X;
        }
    }
    return in;
}

void input_free(Input *in)
{
    if (!in) {
        return;
    }
    psi_free(in->psi);
    free(in);
}

int input_event(Input *in, const SDL_Event *e)
{
    /* the layer says 1 for a remap capture; devices coming and going are used here too (the title shows the prompt) */
    int used = psi_event(in->psi, e);

    switch (e->type) {
    case SDL_EVENT_GAMEPAD_ADDED:
    case SDL_EVENT_JOYSTICK_ADDED:
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_JOYSTICK_REMOVED:
        return 1;
    default:
        return used;
    }
}

void input_pads(Input *in, PadInput pads[2])
{
    PsiPad s[PSI_PLAYERS];
    int p;

    for (p = 0; p < 2; p++) {
        /* X confirms in menus (confirm.h), never in a fight */
        psi_set_swap(in->psi, p, in->menu[p] && in->confirm[p] == CONFIRM_X);
    }
    psi_pads(in->psi, s);
    for (p = 0; p < 2; p++) {
        pad_clear(&pads[p]);
        pads[p].buttons = (uint16_t)s[p].buttons;
        pads[p].mode = s[p].analog ? PAD_MODE_ANALOG : PAD_MODE_DIGITAL;
        if (s[p].analog) {
            /* the PS2 pad's sticks: rx, ry, lx, ly */
            pads[p].analog[0] = s[p].rx;
            pads[p].analog[1] = s[p].ry;
            pads[p].analog[2] = s[p].lx;
            pads[p].analog[3] = s[p].ly;
        }
    }
}

void input_rumble(Input *in, const uint8_t act[2][2])
{
    int p;

    for (p = 0; p < 2; p++) {
        const uint8_t *a = act[p];

        /* small motor: on or off; large: its speed. Renewed while on, stopped when it stops */
        if (a[0] || a[1]) {
            psi_rumble(in->psi, p, a[0] ? 0xFFFF : 0, (Uint16)(a[1] * 257), 100);
        } else if (in->act[p][0] || in->act[p][1]) {
            psi_rumble(in->psi, p, 0, 0, 0);
        }
    }
    memcpy(in->act, act, sizeof(in->act));
}

void input_toggle_analog(Input *in, int player)
{
    psi_toggle_analog(in->psi, player & 1);
}

int input_analog(const Input *in, int player)
{
    return psi_analog(in->psi, player & 1);
}

void input_swap_players(Input *in)
{
    psi_swap_players(in->psi);
}

void input_remap_start(Input *in, int player)
{
    psi_remap_start(in->psi, player & 1);
}

const char *input_remap_prompt(const Input *in)
{
    return psi_remap_prompt(in->psi);
}

InputFamily input_family(const Input *in)
{
    return (InputFamily)psi_family(in->psi);
}

void input_set_menu(Input *in, int player, int menu)
{
    in->menu[player & 1] = menu != 0;
}

int input_confirm(const Input *in, int player)
{
    return in->confirm[player & 1];
}

void input_set_confirm(Input *in, int player, int confirm, int keep)
{
    char key[32];

    in->confirm[player & 1] = confirm;
    if (keep) {
        snprintf(key, sizeof(key), "p%d.confirm", (player & 1) + 1);
        config_set(in->cfg, key, confirm_name(confirm));
    }
}

void input_set_game_keys(Input *in, int player, const MacroKeys *k)
{
    in->game_keys[player & 1] = *k;
    apply_game_keys(in, player & 1);
}

void input_force_family(Input *in, int family)
{
    psi_force_family(in->psi, family);
}

int input_last_player(const Input *in)
{
    return psi_last_player(in->psi);
}

void input_prompt_letters(const Input *in, int player, char out[4])
{
    /* pad.h's bit indices of Circle, X, Square and Triangle */
    static const int PROMPT_SLOTS[4] = { 5, 6, 7, 4 };

    psi_prompt_letters(in->psi, player & 1, PROMPT_SLOTS, 4, out);
}

void input_describe(const Input *in, FILE *out)
{
    psi_describe(in->psi, out);
}
