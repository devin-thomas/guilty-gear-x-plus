/* A session: the Core and everything the host does to it (session.h). */
#include "session.h"

#include <stdlib.h>
#include <string.h>

#include "mc_card.h"
#include "unlock.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

struct SessionState {
    CoreSnapshot *core;
    uint32_t field;
    int unlock_pending;
    McCard *card[2];
    uint32_t card_changes[2];   /* the live card's change count when this state was made equal to it */
};

int session_input_equal(const SessionInput *a, const SessionInput *b)
{
    return pad_equal(&a->pads[0], &b->pads[0]) && pad_equal(&a->pads[1], &b->pads[1]) && a->unlock == b->unlock;
}

void session_init(Session *s, CoreHost *h, int unlocks)
{
    memset(s, 0, sizeof(*s));
    s->h = h;
    s->unlock_pending = unlocks;
}

void session_detach_cards(Session *s)
{
    mc_card_detach(s->h->card[0]);
    mc_card_detach(s->h->card[1]);
    s->detached = 1;
}

int session_field(Session *s, const SessionInput *in)
{
    CoreHost *h = s->h;
    SessionInput rec = *in;
    int rc;

    rec.unlock |= (uint8_t)s->unlock_asked;
    s->unlock_asked = 0;
    s->unlock_pending |= rec.unlock;
    if (s->on_input) {
        s->on_input(s->on_input_ctx, h->field, &rec);
    }
    h->pads[0] = in->pads[0];
    h->pads[1] = in->pads[1];
    /* the unlocks (PORT-020), where the game reads them afresh; and GOHAN on the password screen */
    if (s->unlock_pending && unlock_safe(h)) {
        unlock_apply(h, s->unlock_pending, "asked for");
        s->unlock_pending = 0;
    }
    unlock_before(h);
    rc = core_host_field(h);
    if (rc == 0) {
        unlock_after(h);
        if (s->on_field) {
            s->on_field(s->on_ctx, h->field - 1);
        }
    }
    return rc;
}

void session_unlock(Session *s, int items, const char *why)
{
    s->unlock_asked |= items;
    s->unlock_pending |= items;
    if (s->unlock_pending && unlock_safe(s->h)) {
        unlock_apply(s->h, s->unlock_pending, why);
        s->unlock_pending = 0;
    }
}

uint64_t session_hash(Session *s)
{
    uint8_t b[36];
    uint64_t v[3];
    int i, k;

    v[0] = core_hash(s->h);
    v[1] = mc_card_hash(s->h->card[0]);
    v[2] = mc_card_hash(s->h->card[1]);
    memcpy(b, "GGXS", 4);
    for (i = 0; i < 3; i++) {
        for (k = 0; k < 8; k++) {
            b[4 + 8 * i + k] = (uint8_t)(v[i] >> (8 * k));
        }
    }
    for (k = 0; k < 4; k++) {
        b[28 + k] = (uint8_t)((uint32_t)s->unlock_pending >> (8 * k));
    }
    memset(b + 32, 0, 4);
    return XXH3_64bits(b, sizeof(b));
}

SessionState *session_state_new(Session *s)
{
    SessionState *st = calloc(1, sizeof(*st));
    int i;

    if (!st || !(st->core = core_snapshot_new(s->h))) {
        free(st);
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        if (!(st->card[i] = mc_card_clone(s->h->card[i]))) {
            session_state_free(st);
            return NULL;
        }
        st->card_changes[i] = mc_card_changes(s->h->card[i]);
    }
    return st;
}

void session_state_free(SessionState *st)
{
    if (st) {
        core_snapshot_free(st->core);
        mc_card_close(st->card[0]);
        mc_card_close(st->card[1]);
        free(st);
    }
}

int session_save(Session *s, SessionState *st)
{
    int i;

    if (core_save(s->h, st->core) != 0) {
        return -1;
    }
    st->field = s->h->field;
    st->unlock_pending = s->unlock_pending;
    for (i = 0; i < 2; i++) {
        McCard *live = s->h->card[i];

        /* the count only goes up, so an equal one means the card is as this state holds it */
        if (mc_card_changes(live) != st->card_changes[i]) {
            if (mc_card_assign(st->card[i], live) != 0) {
                return -1;
            }
            st->card_changes[i] = mc_card_changes(live);
        }
    }
    return 0;
}

int session_load(Session *s, SessionState *st)
{
    int rc = core_load(s->h, st->core), i;

    if (rc != 0) {
        return rc;
    }
    s->unlock_pending = st->unlock_pending;
    s->unlock_asked = 0;
    for (i = 0; i < 2; i++) {
        McCard *live = s->h->card[i];

        if (mc_card_changes(live) != st->card_changes[i]) {
            if (mc_card_assign(live, st->card[i]) != 0) {
                return -1;
            }
            st->card_changes[i] = mc_card_changes(live);
            s->card_restores++;
        }
    }
    if (s->on_load) {
        s->on_load(s->on_ctx, s->h->field);
    }
    return 0;
}

int session_state_copy(SessionState *dst, const SessionState *src)
{
    const CoreSnapshot *c = src->core;
    int i;

    /* the bytes alone, as a file or another instance would hand them over: core_load checks them whole */
    if (core_snapshot_set_data(dst->core, core_snapshot_data(c), core_snapshot_size(c)) != 0) {
        return -1;
    }
    dst->field = src->field;
    dst->unlock_pending = src->unlock_pending;
    for (i = 0; i < 2; i++) {
        if (mc_card_assign(dst->card[i], src->card[i]) != 0) {
            return -1;
        }
        /* no live card is as this one holds it yet */
        dst->card_changes[i] = UINT32_MAX;
    }
    return 0;
}

uint32_t session_state_field(const SessionState *st)
{
    return st->field;
}

const CoreSnapshot *session_state_snapshot(const SessionState *st)
{
    return st->core;
}

void session_confirmed(Session *s, uint32_t field)
{
    if (s->on_confirm) {
        s->on_confirm(s->on_ctx, field);
    }
}
