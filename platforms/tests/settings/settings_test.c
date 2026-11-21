/* ggx-settings-test: the port's settings panel (sdl/settings.h) driven by
 * pad presses, as a player would (PORT-019, PORT-020). No window, disc or
 * Core: the input layer's confirm setting, the HUD font and the unlock
 * state are stubbed here.
 *
 *   - a fresh config shows the first-launch line once, and saves confirm;
 *   - the rows: 1P and 2P confirm, then the Unlocks;
 *   - X on EVERYTHING asks for every unlock, Right on COLOURS for the
 *     colours; an unlock is asked for once, never taken back;
 *   - Left on 1P's row switches its confirm button, saved;
 *   - Start closes the panel.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confirm.h"
#include "settings.h"
#include "unlock.h"

static int failures;

#define CHECK(c, ...)                                                                                                  \
    do {                                                                                                               \
        if (!(c)) {                                                                                                    \
            fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__);                                                       \
            fprintf(stderr, __VA_ARGS__);                                                                              \
            fprintf(stderr, "\n");                                                                                     \
            failures++;                                                                                                \
        }                                                                                                              \
    } while (0)

/* ---- stubs for what the panel reads and sets ---- */

static int confirm_of[2] = { CONFIRM_X, CONFIRM_X };
static int unlocked;      /* what the stubbed Core holds */

int input_confirm(const Input *in, int player)
{
    (void)in;
    return confirm_of[player & 1];
}

void input_set_confirm(Input *in, int player, int confirm, int keep)
{
    (void)in;
    (void)keep;
    confirm_of[player & 1] = confirm;
}

int pr_text(const Prompts *p, uint8_t *rgba, int x, int y, const char *s, int scale, int bright)
{
    (void)p, (void)rgba, (void)x, (void)y, (void)scale, (void)bright;
    return (int)strlen(s);
}

int unlock_state(CoreHost *h)
{
    (void)h;
    return unlocked;
}

/* ---- the test ---- */

static void press(Settings *s, uint16_t b)
{
    PadInput p[2];

    memset(p, 0, sizeof(p));
    p[0].buttons = b;
    settings_update(s, p);
    p[0].buttons = 0;
    settings_update(s, p);
}

int main(int argc, char **argv)
{
    char path[1024];
    Config *cfg;
    Settings *s;
    PadInput idle[2];
    int changed, i;

    snprintf(path, sizeof(path), "%s/settings_test.cfg", argc > 1 ? argv[1] : ".");
    remove(path);
    cfg = config_load(path);
    s = settings_new(NULL, cfg, NULL, (CoreHost *)&unlocked);
    CHECK(s != NULL, "settings_new");
    if (!s) {
        return 1;
    }
    /* first launch: the line shows, and the config now has confirm */
    settings_overlay(s, &changed);
    CHECK(changed, "first launch: nothing to show");
    CHECK(!strcmp(config_get_or(cfg, "confirm", ""), "x"), "first launch didn't save confirm = x");
    memset(idle, 0, sizeof(idle));
    for (i = 0; i < 700; i++) {
        settings_watch(s, idle);
    }
    settings_overlay(s, &changed);
    CHECK(changed, "the first-launch line didn't go");
    settings_overlay(s, &changed);
    CHECK(!changed, "an overlay changed with nothing new");

    /* Select held a second opens it */
    idle[1].buttons = PAD_SELECT;
    for (i = 0; i < 59; i++) {
        CHECK(!settings_watch(s, idle), "opened before a second, at %d", i);
    }
    CHECK(settings_watch(s, idle), "Select held a second didn't open it");
    CHECK(settings_is_open(s), "not open");
    idle[1].buttons = 0;
    press(s, 0);               /* what was held at opening doesn't count */

    /* 1P's confirm: Left switches it */
    press(s, PAD_LEFT);
    CHECK(confirm_of[0] == CONFIRM_CIRCLE && confirm_of[1] == CONFIRM_X, "1P Left: %d %d", confirm_of[0], confirm_of[1]);
    press(s, PAD_CROSS);
    CHECK(confirm_of[0] == CONFIRM_X, "1P X picks X");

    /* down to EVERYTHING; X asks for all of it, once */
    press(s, PAD_DOWN);
    press(s, PAD_DOWN);
    CHECK(settings_take_unlocks(s) == 0, "unlocks asked for while moving");
    press(s, PAD_CROSS);
    CHECK(settings_take_unlocks(s) == UNLOCK_ALL, "EVERYTHING didn't ask for everything");
    CHECK(settings_take_unlocks(s) == 0, "taken twice");
    CHECK(confirm_of[0] == CONFIRM_X && confirm_of[1] == CONFIRM_X, "an unlock row changed confirm");

    /* COLOURS (two rows down): Right asks for the colours only; Left (lock again) does nothing */
    press(s, PAD_DOWN);
    press(s, PAD_DOWN);
    press(s, PAD_LEFT);
    CHECK(settings_take_unlocks(s) == 0, "Left on an unlock row asked for something");
    press(s, PAD_RIGHT);
    CHECK(settings_take_unlocks(s) == UNLOCK_COLOURS, "COLOURS didn't ask for the colours");

    /* the panel redraws when what is unlocked changes */
    settings_overlay(s, &changed);
    unlocked = UNLOCK_COLOURS;
    settings_overlay(s, &changed);
    CHECK(changed, "the panel didn't redraw for a new unlock");

    /* back up to 2P's row (three up from COLOURS); X there picks X and unlocks nothing */
    for (i = 0; i < 3; i++) {
        press(s, PAD_UP);
    }
    press(s, PAD_CROSS);
    CHECK(settings_take_unlocks(s) == 0 && confirm_of[1] == CONFIRM_X, "2P's row");

    /* Start closes it, and saves */
    press(s, PAD_START);
    CHECK(!settings_is_open(s), "Start didn't close it");
    config_free(cfg);
    cfg = config_load(path);
    CHECK(!strcmp(config_get_or(cfg, "confirm", ""), "x"), "the saved config lost confirm");
    config_free(cfg);
    settings_free(s);
    remove(path);
    printf("ggx-settings-test: %s (%d failed)\n", failures ? "FAILED" : "ok", failures);
    return failures != 0;
}
