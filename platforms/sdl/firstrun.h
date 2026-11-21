/* The first run (SPEC section 6, PORT-015): ggx's own screen for getting
 * the player's disc into the user data directory, before the game starts.
 *
 * When this build's release has no extraction in the data directory, ggx
 * shows a screen of its own, in the port's fonts (the game's are on the disc,
 * which isn't extracted yet), drawn at the game's 640x448 and scaled as the
 * game is:
 *
 *   1. It asks for the disc image. X (the pad's south button), Enter or a
 *      click opens SDL3's file dialog; an image can also be dropped on the
 *      window. Circle or Escape quits.
 *   2. The image is identified by its files (assets_identify). A wrong or
 *      modified image is refused with assets.c's message, which names the
 *      supported releases, and the player can choose another.
 *   3. The files are extracted (assets_extract_watched) on a thread, with a
 *      bar, the file and the bytes done; a file whose hash isn't the
 *      release's stops it with the same refusal.
 *   4. The game starts.
 *
 * An extraction that is there but damaged (asset_store_open refuses it) is
 * redone from the image its manifest names, if that is still there, without
 * asking; otherwise the screen asks for the image, saying why.
 *
 * One executable or two (the packaging choice, still open): each build of
 * ggx plays one release (-DGGX_VERSION). If the image is the other
 * release's, the first run looks beside this executable for that release's
 * build, ggx-kr or ggx-jp (.exe on Windows); if it is there, the files are
 * extracted and ggx hands over to it (fr_handoff), with the same arguments.
 * A later launch whose own release isn't extracted but the other's is hands
 * over at once, before any window opens. With one executable only, the
 * other release's image is refused, saying which build plays it. A process
 * that was handed over (GGX_HANDED_OVER in its environment) never hands
 * back.
 *
 * --first-run <image> (with or without --offscreen) runs the screen as if
 * the player had chosen that image, for tests; --dump-dir then gets each
 * screen as first-run-<n>.png.
 */
#ifndef GGX_FIRSTRUN_H
#define GGX_FIRSTRUN_H

#include <stddef.h>
#include <stdint.h>

#include <SDL3/SDL.h>

#include "releases.h"
#include "tl_font.h"

#define FR_W 640
#define FR_H 448

enum {
    FR_PLAY = 0,      /* this build's release is extracted: start the game */
    FR_HANDOFF = 1,   /* the other release's is: hand over to fr->other */
    FR_QUIT = 2,      /* the player quit */
    FR_REFUSED = 3    /* offscreen (--first-run): the image was refused */
};

typedef struct {
    SDL_Window *win;          /* NULL offscreen */
    const char *data_dir;
    const char *fonts_dir;    /* the text layer's fonts (text/fonts beside ggx) */
    const char *image;        /* --first-run: as if chosen; NULL to ask */
    const char *dump_dir;     /* each screen as PNG, or NULL */
    /* shows a 640x448 RGBA picture in the window, scaled as the game's */
    void (*show)(void *ctx, const uint8_t *rgba);
    void *show_ctx;
    char other[1100];         /* out: the other release's build, with FR_HANDOFF */
} FirstRun;

/* What the data directory holds for this build: 1 if the first run is
   needed (own release not extracted, or damaged), 0 if not. With *other
   set, the other release is extracted and its build is beside this one
   (fr_handoff it; 0 is returned too). */
int fr_needed(const char *data_dir, char *other, size_t cap);

/* Runs the first run. FR_*. */
int fr_run(FirstRun *fr);

/* Hands over to the other release's build at path: argv as this process got
   it, without --first-run <image>. Returns only if it couldn't (or, on
   Windows, with the child's exit code once it ends: *code). */
int fr_handoff(const char *path, int argc, char **argv, int *code);

/* The build of release r beside this executable, if it is there: 0. */
int fr_build_of(const Release *r, char *out, size_t cap);

/* The screens, for tests and fr_run: draws screen kind with its text into
   rgba (640x448). fonts from tl_fonts_load. */
enum { FR_SCREEN_ASK, FR_SCREEN_DIALOG, FR_SCREEN_EXTRACT, FR_SCREEN_REFUSED, FR_SCREEN_READY };
typedef struct {
    int kind;
    const char *heading;      /* under the title */
    const char *body;         /* wrapped; '\n' breaks */
    const char *file;         /* FR_SCREEN_EXTRACT: the file being read */
    uint64_t done, total;     /* FR_SCREEN_EXTRACT: bytes */
    const char *confirm;      /* the confirm button's label ("X", "A", "ENTER") */
    const char *back;         /* the back button's ("CIRCLE", "B", "ESC") */
    const char *confirm_does, *back_does;   /* NULL: no prompt */
} FrScreen;
void fr_draw(TlFonts *fonts, const FrScreen *s, uint8_t *rgba);

#endif
