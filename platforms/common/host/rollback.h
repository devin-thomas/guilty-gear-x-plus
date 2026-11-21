/* Rollback over a session, as a rollback netcode runs it (PORT-013, ADR-014).
 *
 * Each instance runs the whole game: both players' fields, one instance
 * per player. Its own player's input is known at once and, with an input
 * delay of D fields, is for the field D after the next one to run; the
 * other player's comes over the network, later. Until it arrives the
 * instance predicts it (the last input it has from that player) and runs
 * on, saving a SessionState before every field. When the real input
 * arrives and differs from the prediction, it loads the state from before
 * the first wrong field and runs the fields again with what it now knows:
 * a rollback. It never predicts more than max_rollback fields; past that it
 * waits (rb_advance returns 0).
 *
 * A field is confirmed once both players' inputs for it and for every field
 * before it are known and it has run with them: its State Hash is final.
 * Two instances fed the same inputs confirm the same State Hash for every
 * field; ggx-rollback-test (rollback_test.c) checks it against a third
 * instance that never predicts.
 *
 * The networking is the caller's: it sends each input rb_add_local
 * returns, and hands rb_add_remote what arrives. Frames count fields run
 * since rb_new; frame f is Core field rb_new's field + f.
 */
#ifndef GGX_ROLLBACK_H
#define GGX_ROLLBACK_H

#include <stdint.h>

#include "session.h"

#define RB_MAX_ROLLBACK 8
#define RB_MAX_DELAY 8

typedef struct {
    int local;           /* the player this instance's input is: 0 or 1 */
    int delay;           /* input delay, fields (0 to RB_MAX_DELAY) */
    int max_rollback;    /* the most fields it predicts (1 to RB_MAX_ROLLBACK) */
} RollbackConfig;

typedef struct {
    uint64_t rollbacks;          /* loads of an earlier state */
    uint64_t resimulated;        /* fields run again */
    uint64_t predicted;          /* fields run with the remote input predicted */
    uint64_t mispredicted;       /* of those, wrong */
    uint64_t waits;              /* rb_advance calls that had to wait */
    uint32_t depth_max;          /* the most fields one rollback went back */
    uint64_t depth[RB_MAX_ROLLBACK + 1];   /* rollbacks by how many fields they went back */
} RollbackStats;

typedef struct Rollback Rollback;

/* Starts at the session's current field; fills the state ring now, so that
   no save in the per-field path copies a whole state (STATE-HASH.md 4). */
Rollback *rb_new(Session *s, const RollbackConfig *cfg);
void rb_free(Rollback *r);

/* The local player's input for the next frame it is wanted for (frame + D,
   or the first frame not yet given); returns that frame. Frames before D
   are a pad with nothing pressed for both players. */
uint32_t rb_add_local(Rollback *r, const PadInput *in);
/* The frame rb_add_local's next input is for. Inputs may be given up to
   rb_frame + delay. */
uint32_t rb_local_next(const Rollback *r);
/* The remote player's input for a frame, in any order; a frame given twice
   keeps the first. */
void rb_add_remote(Rollback *r, uint32_t frame, const PadInput *in);

/* Rolls back if a remote input contradicted a prediction, then runs the
   next frame unless that would predict more than max_rollback frames.
   1 if a frame ran, 0 if it must wait for the remote player, -1 if the Core
   trapped. */
int rb_advance(Rollback *r);

uint32_t rb_frame(const Rollback *r);       /* frames run */
uint32_t rb_confirmed(const Rollback *r);   /* frames confirmed: every frame before this one */
/* After confirmed frame f (among the last ring's worth): the State Hash,
   the session hash (session.h), and each region's hash (in region id
   order), for naming the region that differs. 0 if f isn't held. */
int rb_hashes(const Rollback *r, uint32_t f, uint64_t *state, uint64_t *session, uint64_t regions[7]);
/* The input frame f ran (or will run) with, when both are known. */
int rb_input(const Rollback *r, uint32_t f, SessionInput *in);
/* The input frame f last ran with, predictions included, for every frame
   up to the last one started (rb_last_run): what a timeline that went
   wrong ran, to reproduce it. */
int rb_ran_with(const Rollback *r, uint32_t f, SessionInput *in);
uint32_t rb_last_run(const Rollback *r);
const RollbackStats *rb_stats(const Rollback *r);
/* Every save and load's time in microseconds, if a clock is given
   (ggx-rollback-test reports them). */
void rb_set_clock(Rollback *r, double (*seconds)(void), void (*timed)(void *ctx, int load, double us), void *ctx);

#endif
