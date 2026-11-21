/* A session: the Core and everything the host does to it, field by field
 * (PORT-013).
 *
 * What a field computes depends on more than the Core's state and the pads.
 * The host writes the unlocks into the Core where the game reads them
 * (unlock.h), and the memory cards hold what the game saved (mc_card.h),
 * which the game reads back. A session puts these in one place, so that a
 * rollback restores all of it and a replay records all of it:
 *
 *   - SessionInput is a field's input record (ADR-014): both pads as libpad
 *     delivers them, and the unlock items asked for before the field;
 *   - SessionState is a snapshot: the Core's (core_state.h), the unlocks
 *     asked for and not yet made (they wait for a field where the game
 *     reads them afresh, unlock_safe), and what each memory card holds.
 *
 * session_field runs one field from its input: the unlocks waiting are made
 * if the game is where it reads them, GOHAN on the password screen is
 * checked (unlock_before/after), and the Core runs. ggx, ggx-headless, the
 * rollback test and replays all step the Core this way, so a recorded input
 * plays back to the same State Hashes.
 *
 * Cards. Normally the game's saves reach the native store as it makes them.
 * A session that may roll back, or one replaying, detaches its cards
 * (session_detach_cards): what the game saves then stays in memory, inside
 * the session's state, and never reaches the store, so a mispredicted or
 * replayed field can't write a save the player didn't make. The card is
 * copied into a SessionState only when it changed since that state last
 * held it: games save in menus, so in a fight a save copies nothing.
 */
#ifndef GGX_SESSION_H
#define GGX_SESSION_H

#include <stdint.h>

#include "core_host.h"

typedef struct {
    PadInput pads[2];
    uint8_t unlock;       /* UNLOCK_* items asked for before this field (unlock.h) */
} SessionInput;

/* The same record: both pads (pad_equal) and the unlocks. */
int session_input_equal(const SessionInput *a, const SessionInput *b);

typedef struct Session Session;
typedef struct SessionState SessionState;

struct Session {
    CoreHost *h;
    int unlock_pending;   /* asked for, waiting for unlock_safe */
    int unlock_asked;     /* asked for between fields (session_unlock), for the next field's record */
    int detached;
    uint64_t card_restores;   /* session_load put a card back as it was (a rollback across a save) */
    /* Optional: told of each field's input record, as the field starts (the
       field number, then the record: a replay being recorded). */
    void (*on_input)(void *ctx, uint32_t field, const SessionInput *in);
    void *on_input_ctx;
    /* Optional: told after each field ran (its number), when session_load
       put the Core back at an earlier field (the field it will run next),
       and when a rollback confirmed every field before one: the audio
       layer (audio_host_follow). */
    void (*on_field)(void *ctx, uint32_t field);
    void (*on_load)(void *ctx, uint32_t field);
    void (*on_confirm)(void *ctx, uint32_t field);
    void *on_ctx;
};

/* unlocks: the items asked for from the start (ggx's "unlock =", a
   replay's header) */
void session_init(Session *s, CoreHost *h, int unlocks);
/* From here on what the game saves stays in the session (above). */
void session_detach_cards(Session *s);

/* Runs one field. 0, or nonzero if the Core trapped. */
int session_field(Session *s, const SessionInput *in);
/* Unlocks asked for between fields (the settings panel): made at once if
   the game is where it reads them, otherwise at the first field where it
   is. They go into the next field's input record. */
void session_unlock(Session *s, int items, const char *why);

/* The State Hash (core_hash), then the same with the session's own state:
   the unlocks waiting and each card's contents (mc_card_hash). Two
   instances that agree on the session hash compute the same from here on. */
uint64_t session_hash(Session *s);

SessionState *session_state_new(Session *s);
void session_state_free(SessionState *st);
/* 0 on success. */
int session_save(Session *s, SessionState *st);
/* 0, or core_load's refusal (core_state.h) with nothing changed. */
int session_load(Session *s, SessionState *st);
/* Makes dst hold what src holds, as if received from another instance:
   the Core's snapshot is copied whole and checked when it is loaded
   (STATE-HASH.md section 4). 0 on success. */
int session_state_copy(SessionState *dst, const SessionState *src);
uint32_t session_state_field(const SessionState *st);
const CoreSnapshot *session_state_snapshot(const SessionState *st);

/* Rollback (rollback.h) says when every field before field is final. */
void session_confirmed(Session *s, uint32_t field);

#endif
