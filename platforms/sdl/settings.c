/* The port's settings panel (settings.h). */
#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confirm.h"
#include "unlock.h"

#define W 640
#define H 448
#define HOLD_FIELDS 60         /* Select held a second opens it */
#define BANNER_FIELDS 600      /* the first launch's line: ten seconds */
#define CONFIRM_ROWS 2         /* 1P's and 2P's confirm button */
#define ROWS (CONFIRM_ROWS + 5) /* then the unlocks: everything, characters, colours, gallery, challenges */

/* the unlock rows: what each is, and what it gives, in plain words (the HUD font has letters and digits) */
static const struct {
    int items;
    const char *name, *what;
} UNLOCKS[5] = {
    { UNLOCK_ALL, "EVERYTHING", "ALL OF THE BELOW AT ONCE" },
    { UNLOCK_CHARACTERS, "CHARACTERS", "TESTAMENT DIZZY KLIFF JUSTICE GG AND EX" },
    { UNLOCK_COLOURS, "COLOURS", "EVERY COLOUR FOR EVERY CHARACTER" },
    { UNLOCK_GALLERY, "GALLERY", "EVERY PICTURE THE STORY AND THE ENDINGS" },
    { UNLOCK_CHALLENGES, "CHALLENGES", "EVERY GALLERY CHALLENGE CLEARED" },
};

struct Settings {
    Input *in;
    Config *cfg;
    const Prompts *font;
    CoreHost *host;
    int asked;                 /* unlocks asked for, not yet taken by the host */
    int waiting;               /* asked for, and not made yet (not at the title or in OPTIONS) */
    int open, row;
    int hold[2];
    int banner;                /* fields left to show the first launch's line */
    uint16_t prev[2];          /* last tick's buttons, for presses */
    int shown;                 /* what the overlay shows: 0 nothing, 1 the banner, 2 the panel */
    uint32_t drawn_key;        /* what it was drawn for */
    uint8_t *rgba;
};

Settings *settings_new(Input *in, Config *cfg, const Prompts *font, CoreHost *h)
{
    Settings *s = calloc(1, sizeof(*s));

    if (!s || !(s->rgba = malloc((size_t)W * H * 4))) {
        free(s);
        return NULL;
    }
    s->in = in;
    s->cfg = cfg;
    s->font = font;
    s->host = h;
    if (!config_get(cfg, "confirm") && !config_get(cfg, "p1.confirm")) {
        /* the first launch: say once that X confirms, and how to change it */
        s->banner = BANNER_FIELDS;
        config_set(cfg, "confirm", confirm_name(input_confirm(in, 0)));
        if (config_save(cfg) != 0) {
            fprintf(stderr, "ggx: cannot save %s\n", config_path(cfg));
        }
    }
    return s;
}

void settings_free(Settings *s)
{
    if (s) {
        free(s->rgba);
        free(s);
    }
}

int settings_is_open(const Settings *s)
{
    return s->open;
}

void settings_open(Settings *s)
{
    s->open = 1;
    s->row = 0;
    s->banner = 0;
    /* what is held now doesn't count as a press in the panel */
    s->prev[0] = s->prev[1] = 0xFFFF;
    fprintf(stderr, "ggx: settings\n");
}

void settings_close(Settings *s)
{
    s->open = 0;
    s->hold[0] = s->hold[1] = 0;
    if (config_save(s->cfg) != 0) {
        fprintf(stderr, "ggx: cannot save %s\n", config_path(s->cfg));
    }
}

int settings_watch(Settings *s, const PadInput pads[2])
{
    int p;

    if (s->banner > 0) {
        s->banner--;
    }
    for (p = 0; p < 2; p++) {
        s->hold[p] = pads[p].buttons & PAD_SELECT ? s->hold[p] + 1 : 0;
        if (s->hold[p] >= HOLD_FIELDS) {
            settings_open(s);
            return 1;
        }
    }
    return 0;
}

void settings_update(Settings *s, const PadInput pads[2])
{
    uint16_t press = 0;
    int p;

    for (p = 0; p < 2; p++) {
        press |= (uint16_t)(pads[p].buttons & ~s->prev[p]);
        s->prev[p] = pads[p].buttons;
    }
    if (press & PAD_START) {
        settings_close(s);
        return;
    }
    if (press & PAD_UP) {
        s->row = (s->row + ROWS - 1) % ROWS;
    }
    if (press & PAD_DOWN) {
        s->row = (s->row + 1) % ROWS;
    }
    if (s->row >= CONFIRM_ROWS && (press & (PAD_RIGHT | PAD_CROSS | PAD_CIRCLE))) {
        /* unlock: only ever upwards */
        int items = UNLOCKS[s->row - CONFIRM_ROWS].items;

        s->asked |= items;
        s->waiting |= items;
        fprintf(stderr, "ggx: unlock %s\n", UNLOCKS[s->row - CONFIRM_ROWS].name);
        return;
    }
    if (s->row < CONFIRM_ROWS && (press & (PAD_LEFT | PAD_RIGHT | PAD_CROSS | PAD_CIRCLE))) {
        int c = input_confirm(s->in, s->row);

        if (press & PAD_CROSS) {
            c = CONFIRM_X;
        } else if (press & PAD_CIRCLE) {
            c = CONFIRM_CIRCLE;
        } else {
            c = c == CONFIRM_X ? CONFIRM_CIRCLE : CONFIRM_X;
        }
        input_set_confirm(s->in, s->row, c, 1);
        fprintf(stderr, "ggx: player %d confirms with %s\n", s->row + 1, c == CONFIRM_X ? "X" : "Circle");
    }
}

int settings_take_unlocks(Settings *s)
{
    int a = s->asked;

    s->asked = 0;
    return a;
}

static void box(uint8_t *rgba, int x0, int y0, int x1, int y1, uint8_t a)
{
    int x, y;

    for (y = y0; y < y1; y++) {
        for (x = x0; x < x1; x++) {
            uint8_t *o = rgba + 4 * ((size_t)y * W + x);

            o[0] = o[1] = o[2] = 0;
            o[3] = a;
        }
    }
}

/* text centred on the picture */
static void centre(const Prompts *f, uint8_t *rgba, int y, const char *t, int bright)
{
    pr_text(f, rgba, (W - 12 * (int)strlen(t)) / 2, y, t, 1, bright);
}

const uint8_t *settings_overlay(Settings *s, int *changed)
{
    int show = s->open ? 2 : s->banner > 0 ? 1 : 0;
    int st = s->open && s->host ? unlock_state(s->host) : 0;
    uint32_t key;

    s->waiting &= ~st;
    key = (uint32_t)show | (uint32_t)s->row << 4 | (uint32_t)input_confirm(s->in, 0) << 8 |
          (uint32_t)input_confirm(s->in, 1) << 9 | (uint32_t)st << 12 | (uint32_t)s->waiting << 16;

    *changed = key != s->drawn_key || show != s->shown;
    s->shown = show;
    s->drawn_key = key;
    if (!show || !s->font) {
        return NULL;
    }
    if (!*changed) {
        return s->rgba;
    }
    memset(s->rgba, 0, (size_t)W * H * 4);
    if (show == 1) {
        box(s->rgba, 0, 384, W, 432, 0x60);
        centre(s->font, s->rgba, 390, "X CONFIRMS IN MENUS", 256);
        centre(s->font, s->rgba, 410, "F9 OR HOLD SELECT FOR SETTINGS", 200);
    } else {
        int r;

        box(s->rgba, 64, 32, W - 64, 416, 0x70);
        centre(s->font, s->rgba, 44, "SETTINGS", 256);
        centre(s->font, s->rgba, 76, "CONFIRM IN MENUS WITH", 220);
        for (r = 0; r < CONFIRM_ROWS; r++) {
            int c = input_confirm(s->in, r), y = 100 + 24 * r, sel = r == s->row;

            pr_text(s->font, s->rgba, 168, y, r ? "2P" : "1P", 1, sel ? 256 : 110);
            pr_text(s->font, s->rgba, 264, y, "X", 1, c == CONFIRM_X ? (sel ? 256 : 190) : 70);
            pr_text(s->font, s->rgba, 336, y, "CIRCLE", 1, c == CONFIRM_CIRCLE ? (sel ? 256 : 190) : 70);
        }
        centre(s->font, s->rgba, 164, "UNLOCKS", 220);
        for (r = 0; r < 5; r++) {
            int it = UNLOCKS[r].items, y = 188 + 24 * r, sel = CONFIRM_ROWS + r == s->row;
            const char *v = (st & it) == it ? "UNLOCKED" : (s->waiting & it) ? "AT THE TITLE" : "LOCKED";

            pr_text(s->font, s->rgba, 112, y, UNLOCKS[r].name, 1, sel ? 256 : 110);
            pr_text(s->font, s->rgba, 336, y, v, 1, (st & it) == it ? (sel ? 256 : 190) : sel ? 200 : 90);
        }
        if (s->row >= CONFIRM_ROWS) {
            centre(s->font, s->rgba, 320, UNLOCKS[s->row - CONFIRM_ROWS].what, 200);
            centre(s->font, s->rgba, 344, "X UNLOCKS  IT STAYS UNLOCKED", 150);
        }
        centre(s->font, s->rgba, 384, "START CLOSES", 150);
    }
    return s->rgba;
}
