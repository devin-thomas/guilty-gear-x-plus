/* ggx's input layer (input.h). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confirm.h"
#include "input.h"
#include "macro.h"

#define MAX_BIND 8
/* a player's binding slots: the sixteen PS2 buttons (pad.h bit indices), then the macros (macro.h) */
#define NSLOTS (PAD_NBUTTONS + MACRO_COUNT)
#define MAX_DEV 16
#define STICK_ON 16384      /* half way */
#define TRIGGER_ON 8192     /* a quarter */
#define CAPTURE_AXIS 20000  /* remapping takes an axis past 60% */
#define CAPTURE_GAP_NS 300000000ull

enum { B_NONE, B_KEY, B_PAD_BUTTON, B_PAD_AXIS, B_JOY_BUTTON, B_JOY_HAT, B_JOY_AXIS };
/* what a remap replaces: a button's bindings of the same class */
enum { C_KEY, C_PAD, C_JOY };

typedef struct {
    uint8_t kind;
    int8_t dir;        /* an axis' sign; a hat's SDL_HAT_* bit */
    int16_t code;      /* scancode, gamepad button or axis, joystick button, hat or axis */
} Bind;

typedef struct {
    SDL_JoystickID id;
    SDL_Gamepad *gp;     /* a gamepad SDL has a mapping for, or */
    SDL_Joystick *joy;   /* a joystick without one (an arcade stick, say) */
    int player;
    InputFamily family;
    uint64_t used;       /* when it was last used (Input.serial) */
} Device;

struct Input {
    Config *cfg;
    Bind bind[2][NSLOTS][MAX_BIND];
    MacroKeys game_keys[2];   /* the game's KEY CONFIG, for the macros */
    int confirm[2];           /* CONFIRM_X or CONFIRM_CIRCLE, per player */
    int menu[2];              /* the game is asking that pad to confirm or cancel (menu.h) */
    ConfirmLatch latch[2];
    int analog[2];
    int socd;            /* 1: hitbox (left+right neutral, up+down up) */
    int rumble;
    int forced;          /* -1: auto */
    InputFamily last;
    uint64_t serial;     /* uses so far, for which device each player used last */
    uint64_t key_used[2];
    int last_player;
    Device dev[MAX_DEV];
    int ndev;
    bool keys[SDL_SCANCODE_COUNT];
    uint8_t act[2][2];
    int remap_player, remap_step;
    uint64_t remap_at;
    char prompt[128];
};

/* the order remapping asks for the buttons in, as slots: pad.h bit indices, then the macros */
static const int REMAP_ORDER[] = { 12, 14, 15, 13, 7, 6, 4, 5, 2, 3, 0, 1, 11, 8, 9, 10, 16, 17, 18, 19 };

/* the gamepad buttons by position; SDL's own names ("a", "dpup") are read too */
static const struct {
    const char *name;
    SDL_GamepadButton b;
} PAD_NAMES[] = {
    { "south", SDL_GAMEPAD_BUTTON_SOUTH },
    { "east", SDL_GAMEPAD_BUTTON_EAST },
    { "west", SDL_GAMEPAD_BUTTON_WEST },
    { "north", SDL_GAMEPAD_BUTTON_NORTH },
    /* the back buttons, as the Steam Deck and Steam Controller label them */
    { "r4", SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1 },
    { "l4", SDL_GAMEPAD_BUTTON_LEFT_PADDLE1 },
    { "r5", SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2 },
    { "l5", SDL_GAMEPAD_BUTTON_LEFT_PADDLE2 },
};

/* The defaults. The game's own KEY CONFIG uses Square, X, Triangle, Circle, R1 (RESPECT) and L1 (MEMORY), and
   it reads L2 and R2 at the character select, so those keep their usual places. It never reads L3 or R3, so the
   stick clicks are the macros', with the back buttons (the Steam Deck's, an Elite pad's) and a DualSense's
   touchpad. */
static const char *const DEFAULTS[NSLOTS] = {
    /* L2 */ "key:E, pad:lefttrigger+, joy:b6",
    /* R2 */ "key:R, pad:righttrigger+, joy:b7",
    /* L1 */ "key:Q, pad:leftshoulder, joy:b4",
    /* R1 */ "key:W, pad:rightshoulder, joy:b5",
    /* Triangle */ "key:S, pad:north, joy:b3",
    /* Circle */ "key:X, pad:east, joy:b2",
    /* X */ "key:Z, pad:south, joy:b1",
    /* Square */ "key:A, pad:west, joy:b0",
    /* Select */ "key:Right Shift, pad:back, joy:b8",
    /* L3 */ "none",
    /* R3 */ "none",
    /* Start */ "key:Return, pad:start, joy:b9",
    /* Up */ "key:Up, pad:dpup, pad:lefty-, joy:h0.up, joy:a1-",
    /* Right */ "key:Right, pad:dpright, pad:leftx+, joy:h0.right, joy:a0+",
    /* Down */ "key:Down, pad:dpdown, pad:lefty+, joy:h0.down, joy:a1+",
    /* Left */ "key:Left, pad:dpleft, pad:leftx-, joy:h0.left, joy:a0-",
    /* P AND K */ "key:D, pad:leftstick, pad:l4",
    /* S AND HS */ "key:F, pad:rightstick, pad:r4",
    /* P K S */ "key:C, pad:touchpad, pad:l5",
    /* P K S HS */ "key:V, pad:r5",
};

/* A slot's name: the PS2 button's, or the macro's as KEY CONFIG writes it. */
static const char *slot_name(int slot)
{
    return slot < PAD_NBUTTONS ? pad_button_label(slot) : macro_name(slot - PAD_NBUTTONS);
}

static const struct {
    const char *name;
    int hat;
} HAT_NAMES[] = { { "up", SDL_HAT_UP }, { "right", SDL_HAT_RIGHT }, { "down", SDL_HAT_DOWN }, { "left", SDL_HAT_LEFT } };

/* ------------------------------------------------------------------ bindings as text */

static int bind_class(const Bind *b)
{
    return b->kind == B_KEY ? C_KEY : (b->kind == B_PAD_BUTTON || b->kind == B_PAD_AXIS) ? C_PAD : C_JOY;
}

static int parse_bind(const char *s, Bind *out)
{
    char *end;
    long n;
    size_t i;

    memset(out, 0, sizeof(*out));
    if (!strncmp(s, "key:", 4)) {
        SDL_Scancode k = SDL_GetScancodeFromName(s + 4);

        if (k == SDL_SCANCODE_UNKNOWN) {
            return -1;
        }
        out->kind = B_KEY;
        out->code = (int16_t)k;
        return 0;
    }
    if (!strncmp(s, "pad:", 4)) {
        const char *p = s + 4;
        size_t len = strlen(p);
        char axis[32];

        for (i = 0; i < sizeof(PAD_NAMES) / sizeof(PAD_NAMES[0]); i++) {
            if (!strcmp(p, PAD_NAMES[i].name)) {
                out->kind = B_PAD_BUTTON;
                out->code = (int16_t)PAD_NAMES[i].b;
                return 0;
            }
        }
        if (len > 1 && len < sizeof(axis) && (p[len - 1] == '+' || p[len - 1] == '-')) {
            SDL_GamepadAxis a;

            memcpy(axis, p, len - 1);
            axis[len - 1] = 0;
            a = SDL_GetGamepadAxisFromString(axis);
            if (a == SDL_GAMEPAD_AXIS_INVALID) {
                return -1;
            }
            out->kind = B_PAD_AXIS;
            out->code = (int16_t)a;
            out->dir = p[len - 1] == '+' ? 1 : -1;
            return 0;
        }
        {
            SDL_GamepadButton b = SDL_GetGamepadButtonFromString(p);

            if (b == SDL_GAMEPAD_BUTTON_INVALID) {
                return -1;
            }
            out->kind = B_PAD_BUTTON;
            out->code = (int16_t)b;
            return 0;
        }
    }
    if (!strncmp(s, "joy:", 4) && (s[4] == 'b' || s[4] == 'h' || s[4] == 'a')) {
        n = strtol(s + 5, &end, 10);
        if (end == s + 5 || n < 0 || n > 255) {
            return -1;
        }
        out->code = (int16_t)n;
        if (s[4] == 'b' && !*end) {
            out->kind = B_JOY_BUTTON;
            return 0;
        }
        if (s[4] == 'a' && (end[0] == '+' || end[0] == '-') && !end[1]) {
            out->kind = B_JOY_AXIS;
            out->dir = end[0] == '+' ? 1 : -1;
            return 0;
        }
        if (s[4] == 'h' && end[0] == '.') {
            for (i = 0; i < 4; i++) {
                if (!strcmp(end + 1, HAT_NAMES[i].name)) {
                    out->kind = B_JOY_HAT;
                    out->dir = (int8_t)HAT_NAMES[i].hat;
                    return 0;
                }
            }
        }
    }
    return -1;
}

static void format_bind(const Bind *b, char *out, size_t cap)
{
    size_t i;

    switch (b->kind) {
    case B_KEY:
        snprintf(out, cap, "key:%s", SDL_GetScancodeName((SDL_Scancode)b->code));
        return;
    case B_PAD_BUTTON:
        for (i = 0; i < sizeof(PAD_NAMES) / sizeof(PAD_NAMES[0]); i++) {
            if (PAD_NAMES[i].b == (SDL_GamepadButton)b->code) {
                snprintf(out, cap, "pad:%s", PAD_NAMES[i].name);
                return;
            }
        }
        snprintf(out, cap, "pad:%s", SDL_GetGamepadStringForButton((SDL_GamepadButton)b->code));
        return;
    case B_PAD_AXIS:
        snprintf(out, cap, "pad:%s%c", SDL_GetGamepadStringForAxis((SDL_GamepadAxis)b->code), b->dir > 0 ? '+' : '-');
        return;
    case B_JOY_BUTTON:
        snprintf(out, cap, "joy:b%d", b->code);
        return;
    case B_JOY_AXIS:
        snprintf(out, cap, "joy:a%d%c", b->code, b->dir > 0 ? '+' : '-');
        return;
    case B_JOY_HAT:
        for (i = 0; i < 4; i++) {
            if (HAT_NAMES[i].hat == b->dir) {
                snprintf(out, cap, "joy:h%d.%s", b->code, HAT_NAMES[i].name);
                return;
            }
        }
        break;
    }
    snprintf(out, cap, "none");
}

static void config_key(int player, int button, char *out, size_t cap)
{
    const char *n = button < PAD_NBUTTONS ? pad_button_label(button) : macro_config_name(button - PAD_NBUTTONS);
    size_t i, len;

    len = (size_t)snprintf(out, cap, "p%d.", player + 1);
    for (i = 0; n[i] && len + 1 < cap; i++) {
        out[len++] = (char)(n[i] >= 'A' && n[i] <= 'Z' ? n[i] + 32 : n[i]);
    }
    out[len] = 0;
}

/* "key:Z, pad:south" into a button's bindings; bad entries are reported */
static void load_binds(Bind *dst, const char *text, const char *key)
{
    char buf[512], *p = buf;
    int n = 0;

    memset(dst, 0, sizeof(Bind) * MAX_BIND);
    snprintf(buf, sizeof(buf), "%s", text);
    while (*p) {
        char *comma = strchr(p, ','), *e;

        if (comma) {
            *comma = 0;
        }
        while (*p == ' ') {
            p++;
        }
        e = p + strlen(p);
        while (e > p && e[-1] == ' ') {
            *--e = 0;
        }
        if (*p && strcmp(p, "none")) {
            if (n < MAX_BIND && parse_bind(p, &dst[n]) == 0) {
                n++;
            } else {
                fprintf(stderr, "ggx: %s: \"%s\" is not a binding (key:<name>, pad:<button or axis+->, "
                                "joy:b<n> / joy:h<n>.up / joy:a<n>+): ignored\n", key, p);
            }
        }
        if (!comma) {
            break;
        }
        p = comma + 1;
    }
}

static void save_binds(Input *in, int player, int button)
{
    char key[32], val[512] = "", one[64];
    int i;

    for (i = 0; i < MAX_BIND; i++) {
        if (in->bind[player][button][i].kind == B_NONE) {
            continue;
        }
        format_bind(&in->bind[player][button][i], one, sizeof(one));
        if (val[0]) {
            strncat(val, ", ", sizeof(val) - strlen(val) - 1);
        }
        strncat(val, one, sizeof(val) - strlen(val) - 1);
    }
    config_key(player, button, key, sizeof(key));
    config_set(in->cfg, key, val[0] ? val : "none");
}

/* ------------------------------------------------------------------ devices */

int input_on_steam_deck(void)
{
    const char *e = SDL_getenv("SteamDeck");

    if (e && !strcmp(e, "1")) {
        return 1;
    }
#ifdef __linux__
    {
        FILE *f = fopen("/sys/devices/virtual/dmi/id/product_name", "r");
        char name[64] = "";

        if (f) {
            if (!fgets(name, sizeof(name), f)) {
                name[0] = 0;
            }
            fclose(f);
        }
        return !strncmp(name, "Jupiter", 7) || !strncmp(name, "Galileo", 7);
    }
#else
    return 0;
#endif
}

InputFamily input_gamepad_family(SDL_GamepadType type, Uint16 vendor, Uint16 product, int on_deck)
{
    const char *name = SDL_GetGamepadStringForType(type);

    /* newer SDLs have a Steam type (SDL_GAMEPAD_TYPE_STEAM); by name, so older ones build too */
    if (name && !strcmp(name, "steam")) {
        return INPUT_FAMILY_STEAM;
    }
    if (vendor == 0x28de) {
        /* Valve: the Steam Deck's controls, the Steam Controller (wired, its dongle, Bluetooth) */
        if (product == 0x1205 || product == 0x1102 || product == 0x1142 || product == 0x1106) {
            return INPUT_FAMILY_STEAM;
        }
        /* Steam Input's virtual gamepad on a Deck: the Deck's own controls, shown to the game as an Xbox pad */
        if (product == 0x11ff && on_deck && type != SDL_GAMEPAD_TYPE_PS3 && type != SDL_GAMEPAD_TYPE_PS4 &&
            type != SDL_GAMEPAD_TYPE_PS5) {
            return INPUT_FAMILY_STEAM;
        }
    }
    switch (type) {
    case SDL_GAMEPAD_TYPE_PS3:
    case SDL_GAMEPAD_TYPE_PS4:
    case SDL_GAMEPAD_TYPE_PS5:
        return INPUT_FAMILY_PLAYSTATION;
    case SDL_GAMEPAD_TYPE_XBOX360:
    case SDL_GAMEPAD_TYPE_XBOXONE:
        return INPUT_FAMILY_XBOX;
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
        return INPUT_FAMILY_NINTENDO;
    default:
        return INPUT_FAMILY_OTHER;
    }
}

static void dev_used(Input *in, Device *d)
{
    d->used = ++in->serial;
    in->last_player = d->player;
}

/* A key counts for the players it is bound for. */
static void key_used(Input *in, SDL_Scancode sc)
{
    int p, b, i;

    for (p = 0; p < 2; p++) {
        for (b = 0; b < NSLOTS; b++) {
            for (i = 0; i < MAX_BIND; i++) {
                if (in->bind[p][b][i].kind == B_KEY && in->bind[p][b][i].code == (int16_t)sc) {
                    in->key_used[p] = ++in->serial;
                    in->last_player = p;
                    b = NSLOTS;
                    break;
                }
            }
        }
    }
}

static Device *find_dev(Input *in, SDL_JoystickID id)
{
    int i;

    for (i = 0; i < in->ndev; i++) {
        if (in->dev[i].id == id) {
            return &in->dev[i];
        }
    }
    return NULL;
}

static int pick_player(Input *in, SDL_JoystickID id)
{
    char guid[64], key[32];
    int p, i, used[2] = { 0, 0 };

    SDL_GUIDToString(SDL_GetJoystickGUIDForID(id), guid, sizeof(guid));
    for (p = 0; p < 2; p++) {
        snprintf(key, sizeof(key), "p%d.device", p + 1);
        if (!SDL_strcasecmp(config_get_or(in->cfg, key, ""), guid)) {
            return p;
        }
    }
    for (i = 0; i < in->ndev; i++) {
        used[in->dev[i].player] = 1;
    }
    return !used[0] ? 0 : !used[1] ? 1 : 0;
}

static void add_device(Input *in, SDL_JoystickID id, int gamepad)
{
    Device *d;

    if (find_dev(in, id) || in->ndev >= MAX_DEV) {
        return;
    }
    d = &in->dev[in->ndev];
    memset(d, 0, sizeof(*d));
    if (gamepad) {
        d->gp = SDL_OpenGamepad(id);
    } else {
        d->joy = SDL_OpenJoystick(id);
    }
    if (!d->gp && !d->joy) {
        fprintf(stderr, "ggx: cannot open a controller: %s\n", SDL_GetError());
        return;
    }
    d->id = id;
    d->player = pick_player(in, id);
    d->family = gamepad ? input_gamepad_family(SDL_GetGamepadType(d->gp), SDL_GetGamepadVendor(d->gp),
                                               SDL_GetGamepadProduct(d->gp), input_on_steam_deck())
                        : INPUT_FAMILY_OTHER;
    in->ndev++;
    if (in->ndev == 1 && in->last == INPUT_FAMILY_KEYBOARD) {
        in->last = d->family;
    }
    fprintf(stderr, "ggx: player %d: %s (%s, %s)\n", d->player + 1,
            d->gp ? SDL_GetGamepadName(d->gp) : SDL_GetJoystickName(d->joy), d->gp ? "gamepad" : "joystick",
            input_family_name(d->family));
}

static void remove_device(Input *in, SDL_JoystickID id)
{
    Device *d = find_dev(in, id);

    if (!d) {
        return;
    }
    if (d->gp) {
        SDL_CloseGamepad(d->gp);
    } else {
        SDL_CloseJoystick(d->joy);
    }
    *d = in->dev[--in->ndev];
}

/* ------------------------------------------------------------------ the layer */

Input *input_new(Config *cfg)
{
    Input *in = calloc(1, sizeof(Input));
    const char *v;
    int p, b;

    if (!in) {
        return NULL;
    }
    in->cfg = cfg;
    in->remap_player = -1;
    in->last = INPUT_FAMILY_KEYBOARD;
    for (p = 0; p < 2; p++) {
        char key[32];

        macro_default_keys(&in->game_keys[p]);
        for (b = 0; b < NSLOTS; b++) {
            config_key(p, b, key, sizeof(key));
            v = config_get(cfg, key);
            if (!v && b < PAD_NBUTTONS && (1 << b) == PAD_CROSS) {
                /* written as "p1.cross" before PORT-019 */
                char old[32];

                snprintf(old, sizeof(old), "p%d.cross", p + 1);
                v = config_get(cfg, old);
            }
            load_binds(in->bind[p][b], v ? v : DEFAULTS[b], key);
            if (!v && p == 1) {
                /* player 2 has no keys by default: the keyboard is player 1's */
                Bind *l = in->bind[p][b];
                int i, n = 0;

                for (i = 0; i < MAX_BIND; i++) {
                    if (l[i].kind != B_KEY) {
                        l[n++] = l[i];
                    }
                }
                memset(l + n, 0, sizeof(Bind) * (size_t)(MAX_BIND - n));
            }
        }
        snprintf(key, sizeof(key), "p%d.analog", p + 1);
        in->analog[p] = !strcmp(config_get_or(cfg, key, "off"), "on");
    }
    for (p = 0; p < 2; p++) {
        char key[32];

        snprintf(key, sizeof(key), "p%d.confirm", p + 1);
        v = config_get_or(cfg, key, config_get_or(cfg, "confirm", "x"));
        in->confirm[p] = confirm_parse(v);
        if (in->confirm[p] < 0) {
            fprintf(stderr, "ggx: confirm = %s: not x or circle; using x\n", v);
            in->confirm[p] = CONFIRM_X;
        }
        confirm_reset(&in->latch[p]);
    }
    in->socd = strcmp(config_get_or(cfg, "socd", "hitbox"), "off") != 0;
    in->rumble = strcmp(config_get_or(cfg, "rumble", "on"), "off") != 0;
    v = config_get_or(cfg, "prompts", "auto");
    in->forced = input_family_parse(v);
    if (in->forced < 0 && strcmp(v, "auto")) {
        fprintf(stderr, "ggx: prompts = %s: not auto, playstation, xbox, steam, nintendo, other or keyboard; using auto\n",
                v);
    }
    return in;
}

void input_free(Input *in)
{
    if (!in) {
        return;
    }
    while (in->ndev) {
        remove_device(in, in->dev[0].id);
    }
    free(in);
}

static void remap_prompt(Input *in)
{
    if (in->remap_step >= (int)(sizeof(REMAP_ORDER) / sizeof(REMAP_ORDER[0]))) {
        int b;

        for (b = 0; b < NSLOTS; b++) {
            save_binds(in, in->remap_player, b);
        }
        if (config_save(in->cfg) != 0) {
            fprintf(stderr, "ggx: cannot save %s\n", config_path(in->cfg));
        } else {
            fprintf(stderr, "ggx: player %d's buttons saved in %s\n", in->remap_player + 1, config_path(in->cfg));
        }
        in->remap_player = -1;
        in->prompt[0] = 0;
        return;
    }
    {
        /* what it is bound to now, so a player sees the macros' bindings too */
        const Bind *list = in->bind[in->remap_player][REMAP_ORDER[in->remap_step]];
        char now[96] = "", one[48];
        int i;

        for (i = 0; i < MAX_BIND && list[i].kind != B_NONE; i++) {
            format_bind(&list[i], one, sizeof(one));
            if (strlen(now) + strlen(one) + 3 >= sizeof(now)) {
                break;
            }
            strcat(now, now[0] ? ", " : "");
            strcat(now, one);
        }
        snprintf(in->prompt, sizeof(in->prompt), "Player %d: press %s (now %s; Esc keeps it)", in->remap_player + 1,
                 slot_name(REMAP_ORDER[in->remap_step]), now[0] ? now : "none");
    }
    fprintf(stderr, "ggx: %s\n", in->prompt);
}

/* A remap capture: b replaces the current button's bindings of its class. */
static void remap_take(Input *in, const Bind *b)
{
    Bind *list = in->bind[in->remap_player][REMAP_ORDER[in->remap_step]];
    Bind keep[MAX_BIND];
    int i, n = 0;
    uint64_t now = SDL_GetTicksNS();

    if (now - in->remap_at < CAPTURE_GAP_NS) {
        return;   /* the same press, or a stick still coming back */
    }
    for (i = 0; i < MAX_BIND; i++) {
        if (list[i].kind != B_NONE && bind_class(&list[i]) != bind_class(b)) {
            keep[n++] = list[i];
        }
    }
    keep[n < MAX_BIND ? n++ : MAX_BIND - 1] = *b;
    memset(list, 0, sizeof(Bind) * MAX_BIND);
    memcpy(list, keep, sizeof(Bind) * (size_t)n);
    in->remap_at = now;
    in->remap_step++;
    remap_prompt(in);
}

int input_event(Input *in, const SDL_Event *e)
{
    Device *d;
    Bind b;

    memset(&b, 0, sizeof(b));
    switch (e->type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        if (e->key.scancode < SDL_SCANCODE_COUNT) {
            in->keys[e->key.scancode] = e->type == SDL_EVENT_KEY_DOWN;
        }
        if (e->type == SDL_EVENT_KEY_DOWN && !e->key.repeat) {
            in->last = INPUT_FAMILY_KEYBOARD;
            key_used(in, e->key.scancode);
            if (in->remap_player >= 0) {
                if (e->key.scancode == SDL_SCANCODE_ESCAPE) {
                    in->remap_step++;
                    remap_prompt(in);
                } else {
                    b.kind = B_KEY;
                    b.code = (int16_t)e->key.scancode;
                    remap_take(in, &b);
                }
                return 1;
            }
        }
        return 0;
    case SDL_EVENT_GAMEPAD_ADDED:
        add_device(in, e->gdevice.which, 1);
        return 1;
    case SDL_EVENT_JOYSTICK_ADDED:
        if (!SDL_IsGamepad(e->jdevice.which)) {
            add_device(in, e->jdevice.which, 0);
        }
        return 1;
    case SDL_EVENT_GAMEPAD_REMOVED:
    case SDL_EVENT_JOYSTICK_REMOVED:
        remove_device(in, e->type == SDL_EVENT_GAMEPAD_REMOVED ? e->gdevice.which : e->jdevice.which);
        return 1;
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        if ((d = find_dev(in, e->gbutton.which)) != NULL && d->gp) {
            in->last = d->family;
            dev_used(in, d);
            if (in->remap_player >= 0) {
                b.kind = B_PAD_BUTTON;
                b.code = (int16_t)e->gbutton.button;
                remap_take(in, &b);
                return 1;
            }
        }
        return 0;
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        if ((d = find_dev(in, e->gaxis.which)) != NULL && d->gp && (e->gaxis.value > STICK_ON || e->gaxis.value < -STICK_ON)) {
            in->last = d->family;
            dev_used(in, d);
            if (in->remap_player >= 0 && (e->gaxis.value > CAPTURE_AXIS || e->gaxis.value < -CAPTURE_AXIS)) {
                b.kind = B_PAD_AXIS;
                b.code = (int16_t)e->gaxis.axis;
                b.dir = e->gaxis.value > 0 ? 1 : -1;
                remap_take(in, &b);
                return 1;
            }
        }
        return 0;
    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        /* the same events come for a gamepad's joystick: only plain joysticks count here */
        d = find_dev(in, e->type == SDL_EVENT_JOYSTICK_BUTTON_DOWN ? e->jbutton.which
                         : e->type == SDL_EVENT_JOYSTICK_HAT_MOTION ? e->jhat.which : e->jaxis.which);
        if (!d || !d->joy) {
            return 0;
        }
        if (e->type == SDL_EVENT_JOYSTICK_BUTTON_DOWN) {
            b.kind = B_JOY_BUTTON;
            b.code = (int16_t)e->jbutton.button;
        } else if (e->type == SDL_EVENT_JOYSTICK_HAT_MOTION) {
            int h = e->jhat.value;

            if (h == SDL_HAT_CENTERED) {
                return 0;
            }
            b.kind = B_JOY_HAT;
            b.code = (int16_t)e->jhat.hat;
            b.dir = (int8_t)((h & SDL_HAT_UP) ? SDL_HAT_UP : (h & SDL_HAT_DOWN) ? SDL_HAT_DOWN
                             : (h & SDL_HAT_LEFT) ? SDL_HAT_LEFT : SDL_HAT_RIGHT);
        } else {
            if (e->jaxis.value <= CAPTURE_AXIS && e->jaxis.value >= -CAPTURE_AXIS) {
                return 0;
            }
            b.kind = B_JOY_AXIS;
            b.code = (int16_t)e->jaxis.axis;
            b.dir = e->jaxis.value > 0 ? 1 : -1;
        }
        in->last = d->family;
        dev_used(in, d);
        if (in->remap_player >= 0) {
            remap_take(in, &b);
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

static int bind_on(const Input *in, int player, const Bind *b)
{
    int i;

    if (b->kind == B_KEY) {
        return in->keys[b->code];
    }
    for (i = 0; i < in->ndev; i++) {
        const Device *d = &in->dev[i];

        if (d->player != player) {
            continue;
        }
        switch (b->kind) {
        case B_PAD_BUTTON:
            if (d->gp && SDL_GetGamepadButton(d->gp, (SDL_GamepadButton)b->code)) {
                return 1;
            }
            break;
        case B_PAD_AXIS:
            if (d->gp) {
                int v = SDL_GetGamepadAxis(d->gp, (SDL_GamepadAxis)b->code) * b->dir;
                int on = b->code == SDL_GAMEPAD_AXIS_LEFT_TRIGGER || b->code == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
                             ? TRIGGER_ON : STICK_ON;

                if (v > on) {
                    return 1;
                }
            }
            break;
        case B_JOY_BUTTON:
            if (d->joy && SDL_GetJoystickButton(d->joy, b->code)) {
                return 1;
            }
            break;
        case B_JOY_HAT:
            if (d->joy && (SDL_GetJoystickHat(d->joy, b->code) & b->dir)) {
                return 1;
            }
            break;
        case B_JOY_AXIS:
            if (d->joy && SDL_GetJoystickAxis(d->joy, b->code) * b->dir > STICK_ON) {
                return 1;
            }
            break;
        }
    }
    return 0;
}

static uint8_t axis_byte(Sint16 v)
{
    return (uint8_t)(((int)v + 32768) >> 8);
}

void input_pads(Input *in, PadInput pads[2])
{
    int p, b, i;

    for (p = 0; p < 2; p++) {
        PadInput *o = &pads[p];
        const Device *stick = NULL;

        pad_clear(o);
        o->mode = in->analog[p] ? PAD_MODE_ANALOG : PAD_MODE_DIGITAL;
        if (in->remap_player >= 0) {
            continue;
        }
        for (b = 0; b < NSLOTS; b++) {
            for (i = 0; i < MAX_BIND && in->bind[p][b][i].kind != B_NONE; i++) {
                if (bind_on(in, p, &in->bind[p][b][i])) {
                    /* a macro presses its buttons on the same field (macro.h) */
                    o->buttons |= b < PAD_NBUTTONS ? (uint16_t)(1u << b)
                                                   : macro_buttons(b - PAD_NBUTTONS, &in->game_keys[p]);
                    break;
                }
            }
        }
        /* X confirms in menus (confirm.h), never in a fight */
        o->buttons = confirm_apply(&in->latch[p], o->buttons, in->menu[p] && in->confirm[p] == CONFIRM_X);
        if (in->socd) {
            if ((o->buttons & (PAD_LEFT | PAD_RIGHT)) == (PAD_LEFT | PAD_RIGHT)) {
                o->buttons &= (uint16_t)~(PAD_LEFT | PAD_RIGHT);
            }
            if ((o->buttons & (PAD_UP | PAD_DOWN)) == (PAD_UP | PAD_DOWN)) {
                o->buttons &= (uint16_t)~PAD_DOWN;
            }
        }
        if (o->mode != PAD_MODE_ANALOG) {
            continue;
        }
        for (i = 0; i < in->ndev && !stick; i++) {
            if (in->dev[i].player == p) {
                stick = &in->dev[i];
            }
        }
        if (stick && stick->gp) {
            o->analog[0] = axis_byte(SDL_GetGamepadAxis(stick->gp, SDL_GAMEPAD_AXIS_RIGHTX));
            o->analog[1] = axis_byte(SDL_GetGamepadAxis(stick->gp, SDL_GAMEPAD_AXIS_RIGHTY));
            o->analog[2] = axis_byte(SDL_GetGamepadAxis(stick->gp, SDL_GAMEPAD_AXIS_LEFTX));
            o->analog[3] = axis_byte(SDL_GetGamepadAxis(stick->gp, SDL_GAMEPAD_AXIS_LEFTY));
        } else if (stick && SDL_GetNumJoystickAxes(stick->joy) >= 2) {
            o->analog[2] = axis_byte(SDL_GetJoystickAxis(stick->joy, 0));
            o->analog[3] = axis_byte(SDL_GetJoystickAxis(stick->joy, 1));
        }
        /* keys, d-pads and hats steer the left stick while it is centred */
        if (o->analog[2] > 0x60 && o->analog[2] < 0xA0 && o->analog[3] > 0x60 && o->analog[3] < 0xA0) {
            if (o->buttons & PAD_LEFT) {
                o->analog[2] = 0x00;
            } else if (o->buttons & PAD_RIGHT) {
                o->analog[2] = 0xFF;
            }
            if (o->buttons & PAD_UP) {
                o->analog[3] = 0x00;
            } else if (o->buttons & PAD_DOWN) {
                o->analog[3] = 0xFF;
            }
        }
    }
}

void input_rumble(Input *in, const uint8_t act[2][2])
{
    int i;

    for (i = 0; i < in->ndev; i++) {
        Device *d = &in->dev[i];
        const uint8_t *a = act[d->player];

        if (!d->gp || !in->rumble) {
            continue;
        }
        /* small motor: on or off; large: its speed. Renewed while on, stopped when it stops */
        if (a[0] || a[1]) {
            SDL_RumbleGamepad(d->gp, (Uint16)(a[1] * 257), a[0] ? 0xFFFF : 0, 100);
        } else if (in->act[d->player][0] || in->act[d->player][1]) {
            SDL_RumbleGamepad(d->gp, 0, 0, 0);
        }
    }
    memcpy(in->act, act, sizeof(in->act));
}

void input_toggle_analog(Input *in, int player)
{
    char key[32];

    in->analog[player] = !in->analog[player];
    snprintf(key, sizeof(key), "p%d.analog", player + 1);
    config_set(in->cfg, key, in->analog[player] ? "on" : "off");
    fprintf(stderr, "ggx: player %d's pad is %s\n", player + 1, in->analog[player] ? "analog (0x73)" : "digital (0x41)");
}

int input_analog(const Input *in, int player)
{
    return in->analog[player];
}

void input_swap_players(Input *in)
{
    int i;

    for (i = 0; i < in->ndev; i++) {
        in->dev[i].player ^= 1;
    }
    input_describe(in, stderr);
}

void input_remap_start(Input *in, int player)
{
    in->remap_player = player;
    in->remap_step = 0;
    in->remap_at = SDL_GetTicksNS();
    remap_prompt(in);
}

const char *input_remap_prompt(const Input *in)
{
    return in->remap_player >= 0 ? in->prompt : NULL;
}

InputFamily input_family(const Input *in)
{
    return in->forced >= 0 ? (InputFamily)in->forced : in->last;
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
}

void input_force_family(Input *in, int family)
{
    in->forced = family;
}

int input_last_player(const Input *in)
{
    return in->last_player;
}

/* pad.h's bit indices of Circle, X, Square and Triangle */
static const int PROMPT_BITS[4] = { 5, 6, 7, 4 };

static char label_letter(SDL_GamepadButtonLabel l)
{
    switch (l) {
    case SDL_GAMEPAD_BUTTON_LABEL_A: return 'A';
    case SDL_GAMEPAD_BUTTON_LABEL_B: return 'B';
    case SDL_GAMEPAD_BUTTON_LABEL_X: return 'X';
    case SDL_GAMEPAD_BUTTON_LABEL_Y: return 'Y';
    default: return 0;        /* a PlayStation shape, or no label */
    }
}

/* The first binding of a kind for a button, or NULL. */
static const Bind *bound(const Input *in, int player, int bit, int kind)
{
    int i;

    for (i = 0; i < MAX_BIND && in->bind[player][bit][i].kind != B_NONE; i++) {
        if (in->bind[player][bit][i].kind == kind) {
            return &in->bind[player][bit][i];
        }
    }
    return NULL;
}

static char key_letter(const Bind *b)
{
    SDL_Keycode k;

    if (!b) {
        return 0;
    }
    k = SDL_GetKeyFromScancode((SDL_Scancode)b->code, SDL_KMOD_NONE, false);
    if (k >= 'a' && k <= 'z') {
        return (char)(k - 'a' + 'A');
    }
    return k >= '0' && k <= '9' ? (char)k : 0;
}

void input_prompt_letters(const Input *in, int player, char out[4])
{
    const Device *d = NULL;
    SDL_GamepadType forced_type = SDL_GAMEPAD_TYPE_UNKNOWN;
    int keyboard = 0, i, k;

    player &= 1;
    memset(out, 0, 4);
    if (in->forced >= 0) {
        switch ((InputFamily)in->forced) {
        case INPUT_FAMILY_XBOX:
        case INPUT_FAMILY_STEAM:
            forced_type = SDL_GAMEPAD_TYPE_XBOXONE;    /* the Deck's and the Steam Controller's letters are Xbox's */
            break;
        case INPUT_FAMILY_NINTENDO:
            forced_type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
            break;
        case INPUT_FAMILY_KEYBOARD:
            keyboard = 1;
            break;
        default:
            return;
        }
    } else {
        /* the player's device used last; before any, the first they have, else the keyboard */
        for (i = 0; i < in->ndev; i++) {
            if (in->dev[i].player == player && (!d || in->dev[i].used > d->used)) {
                d = &in->dev[i];
            }
        }
        if (!d || in->key_used[player] > d->used) {
            keyboard = 1;
        }
    }
    for (k = 0; k < 4; k++) {
        const Bind *b;

        if (keyboard) {
            out[k] = key_letter(bound(in, player, PROMPT_BITS[k], B_KEY));
        } else if ((b = bound(in, player, PROMPT_BITS[k], B_PAD_BUTTON)) != NULL) {
            SDL_GamepadButton gb = (SDL_GamepadButton)b->code;

            if (forced_type != SDL_GAMEPAD_TYPE_UNKNOWN) {
                out[k] = label_letter(SDL_GetGamepadButtonLabelForType(forced_type, gb));
            } else if (d->gp) {
                out[k] = label_letter(SDL_GetGamepadButtonLabel(d->gp, gb));
            }
        }
    }
}

void input_describe(const Input *in, FILE *out)
{
    int i;

    if (!in->ndev) {
        fprintf(out, "ggx: no controllers: player 1 is the keyboard\n");
    }
    for (i = 0; i < in->ndev; i++) {
        const Device *d = &in->dev[i];
        char guid[64];

        SDL_GUIDToString(SDL_GetJoystickGUIDForID(d->id), guid, sizeof(guid));
        fprintf(out, "ggx: player %d: %s (%s, %s, %04x:%04x, GUID %s)\n", d->player + 1,
                d->gp ? SDL_GetGamepadName(d->gp) : SDL_GetJoystickName(d->joy), d->gp ? "gamepad" : "joystick",
                input_family_name(d->family), SDL_GetJoystickVendorForID(d->id), SDL_GetJoystickProductForID(d->id),
                guid);
    }
}
