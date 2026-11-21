/* Unlocks (PORT-020): what play unlocks, given by the host instead.
 *
 * Everything the game saves as unlocked is four fields of userinfo
 * (USER_SETTING, the same on KR and JP; PORT-020):
 *
 *   phase       0-5: Testament (1), Dizzy (2), GG and EX characters, GG
 *               MODE and Gold CPU (3), Kliff (4), Justice (5); with
 *               LimitStatus, which SetLimitStatus derives from it (5 for 5)
 *   usecol[18]  each character's colours: 0x7FF is all eleven
 *   gstart[128] gallery pictures that exist: 0-97 (24-31, the Petit
 *               challenges, only by password)
 *   gcheck[128] gallery pictures viewable: 0-31 the challenges cleared,
 *               40-81 the story pictures, 82-97 the endings
 *
 * The host writes them into the Core's memory, as play or a password would:
 * only upwards (never a lower phase, never a colour taken away), only
 * where the game reads them afresh (the title and OPTIONS, never in a
 * fight), and each write in the run's log (shim_log), with its address and
 * old and new value. Nothing else changes; with nothing unlocked, a run is
 * byte for byte what it was.
 *
 * And the game's own password screen takes GOHAN: an entry that starts
 * with G O H A N (the screen needs a letter in all eight slots, so GOHANAAA
 * if the player just confirms through the rest) unlocks everything. The
 * game compares it with its table, finds nothing and says "failed"; in that
 * same field the host turns it into EX PASSWORD 2's success (passidx 13,
 * "EX PASSWORD 2 INPUT COMPLETED") and makes the writes, EX PASSWORD 2's
 * own (phase 5, LimitStatus 5) among them. The game saves it to the card
 * as it saves any password.
 */
#ifndef GGX_UNLOCK_H
#define GGX_UNLOCK_H

#include "core_host.h"

enum {
    UNLOCK_CHARACTERS = 1,   /* phase 5: every character, GG and EX, GG MODE */
    UNLOCK_COLOURS = 2,      /* usecol: every colour for every character */
    UNLOCK_GALLERY = 4,      /* gstart 0-97, gcheck 40-97: every picture, the story's and the endings */
    UNLOCK_CHALLENGES = 8,   /* gcheck 0-31: every gallery challenge cleared, its picture viewable */
    UNLOCK_ALL = 15
};

/* "all", or item names joined with commas ("characters,colours"): a mask,
   or -1. "none" is 0. */
int unlock_parse(const char *s);
const char *unlock_name(int item);   /* one item: "characters" ... */

/* The items fully unlocked now. */
int unlock_state(CoreHost *h);

/* 1 where the game reads the unlocks afresh: the title and OPTIONS. */
int unlock_safe(CoreHost *h);

/* Raises the items (only where unlock_safe; the caller checks), logging
   each write. Returns the number of writes. */
int unlock_apply(CoreHost *h, int items, const char *why);

/* GOHAN: around each field. unlock_before notes a GOHAN entry the password
   screen is about to check; unlock_after turns the screen's "failed" into
   success and unlocks everything. 1 if it did. */
void unlock_before(CoreHost *h);
int unlock_after(CoreHost *h);

#endif
