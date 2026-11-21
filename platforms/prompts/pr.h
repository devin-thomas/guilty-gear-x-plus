/* Button prompts: the letter on the player's button in place of the game's
 * PS2 symbols (ADR-019, PORT-017).
 *
 * The game draws its Circle, X, Square and Triangle with FontDraw, as
 * four 12x16 cells of the HUD font's picture 0x44 (CockpitFile, beside the
 * letters K to Z): KEY CONFIG (1P and 2P columns), and the SOUND TEST and
 * gallery viewer's help lines.
 * The prompt layer redraws those four cells with letters from the same
 * font (A to J and the digits are in picture 0x43, K to Z in 0x44), read
 * from the player's disc, their colours carried into 0x44's CLUT. It is a
 * texture substitution in the renderer (DlSubst, DlVariant), outside the
 * Core, as the English text is. The face-button diamonds the Training
 * displays draw are not touched: their colours and places are the meaning.
 *
 * Letters are given per player, in the order Circle, X, Square,
 * Triangle; 0 keeps that button's symbol, and PR_SYMBOL(b) draws button
 * b's symbol instead. KEY CONFIG's two columns take each player's letters
 * (pr_set): the buttons a fight uses. The help lines take the help letters
 * (pr_set_help) of the player set with pr_set_shared (the last device
 * used): where X confirms (PORT-019) they show the X button in Circle's
 * place and the other way round, so "o-PLAY" reads "x-PLAY" on a
 * PlayStation pad and "A-PLAY" on an Xbox one.
 */
#ifndef GGX_PR_H
#define GGX_PR_H

#include <stddef.h>
#include <stdint.h>

#include "tl.h"

typedef struct Prompts Prompts;

enum { PR_CIRCLE, PR_CROSS, PR_SQUARE, PR_TRIANGLE, PR_NBUTTONS };

/* A letter code for button b's own symbol (PR_CIRCLE ... PR_TRIANGLE). */
#define PR_SYMBOL(b) ((char)(1 + (b)))

/* The HUD font's pictures 0x43 (digits, A to J) and 0x44 (K to Z and the
   four symbols), as tl_read_picture reads them: CockpitFile, 0x99. */
enum { PR_FONT_FILE = 0x99 };
#define PR_FONT_DIGITS "067"
#define PR_FONT_SYMBOLS "068"

/* NULL with a message in err if the pictures aren't the HUD font's. The
   pictures stay the caller's. */
Prompts *pr_open(const TlRawPicture *digits, const TlRawPicture *symbols, char *err, size_t cap);
void pr_free(Prompts *p);

/* A player's letters (Circle, X, Square, Triangle; 0: the symbol), and
   the player the shared screens follow. Each change of letters is a new
   variant, so takes one decode of the font picture. */
void pr_set(Prompts *p, int player, const char letters[PR_NBUTTONS]);
void pr_set_help(Prompts *p, int player, const char letters[PR_NBUTTONS]);
void pr_set_shared(Prompts *p, int player);

/* 1 if the font draws c (A to Z, 0 to 9, or a PR_SYMBOL). */
int pr_has_letter(char c);

/* Writes s in the HUD font into a 640x448 RGBA8 picture (GS alpha) at x, y,
   each pixel scale times, its colours times bright / 256: the port's own
   screens (PORT-019's settings) in the game's lettering. A to Z, 0 to 9 and
   the PR_SYMBOLs; anything else is a space, 12 pixels wide (times scale). */
int pr_text(const Prompts *p, uint8_t *rgba, int x, int y, const char *s, int scale, int bright);

/* The renderer's side: DlVariant and DlSubst (dl_stream.h). */
uint32_t pr_variant(void *p, uint64_t upload_hash, int x, int y);
const uint8_t *pr_subst(void *p, uint64_t upload_hash, int psm, uint32_t w, uint32_t h, const uint8_t *texels,
                        uint32_t variant);

#endif
