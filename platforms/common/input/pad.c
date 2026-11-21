/* PS2 pad state as libpad delivers it (pad.h). */
#include <string.h>

#include "pad.h"

static const char *const NAMES[PAD_NBUTTONS] = {
    "L2", "R2", "L1", "R1", "Triangle", "Circle", "Cross", "Square",
    "Select", "L3", "R3", "Start", "Up", "Right", "Down", "Left",
};

void pad_clear(PadInput *p)
{
    p->buttons = 0;
    memset(p->analog, PAD_STICK_CENTRE, sizeof(p->analog));
    p->mode = PAD_MODE_DIGITAL;
}

int pad_equal(const PadInput *a, const PadInput *b)
{
    return a->buttons == b->buttons && a->mode == b->mode && !memcmp(a->analog, b->analog, sizeof(a->analog));
}

uint32_t pad_encode(const PadInput *in, uint8_t out[32])
{
    memset(out, 0, 32);
    out[0] = 0;   /* success */
    out[2] = (uint8_t)~(in->buttons >> 8);
    out[3] = (uint8_t)~in->buttons;
    if (in->mode == PAD_MODE_ANALOG) {
        out[1] = 0x73;   /* DualShock 2, analog: three halfwords of data */
        memcpy(out + 4, in->analog, 4);
    } else {
        out[1] = 0x41;   /* digital: one halfword */
    }
    return 32;
}

void pad_request(PadRequest *r, uint32_t vblank, int after_actuators)
{
    r->call = vblank;
    r->done = vblank + (after_actuators ? 6 : 5);
}

static int failing(const PadRequest *r, uint32_t vblank)
{
    return r->done && vblank + 3 >= r->done && vblank < r->done;
}

int pad_read_ok(const PadRequest *r, uint32_t field, uint32_t vblank)
{
    return field != 1 && !failing(r, vblank);
}

uint32_t pad_req_state(const PadRequest *r, uint32_t vblank)
{
    return r->done && vblank > r->call && vblank < r->done ? 2 : 0;
}

uint32_t pad_act_direct(const PadRequest *r, uint32_t vblank)
{
    return failing(r, vblank) && vblank + 1 < r->done ? 0 : 1;
}

const char *pad_button_name(int index)
{
    return index >= 0 && index < PAD_NBUTTONS ? NAMES[index] : NULL;
}

const char *pad_button_label(int index)
{
    /* players call it X, not cross */
    return (1 << index) == PAD_CROSS ? "X" : pad_button_name(index);
}

int pad_button_by_name(const char *name)
{
    int i;

    if ((name[0] | 0x20) == 'x' && !name[1]) {
        return PAD_CROSS;
    }
    for (i = 0; i < PAD_NBUTTONS; i++) {
        const char *a = NAMES[i], *b = name;

        while (*a && *b && (*a | 0x20) == (*b | 0x20)) {
            a++, b++;
        }
        if (!*a && !*b) {
            return 1 << i;
        }
    }
    return 0;
}
