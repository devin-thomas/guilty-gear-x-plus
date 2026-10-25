#include "generated/ampack.h"

extern PAD pads[2];

int iodat_chk(int stat) {
    if (pads[0].data & stat) {
        return 1;
    }
    if (pads[1].data & stat) {
        return 2;
    }
    return 0;
}

extern PAD pads[2];

int iotrg_chk(int stat) {
    if (pads[0].trg & stat) {
        return 1;
    }
    if (pads[1].trg & stat) {
        return 2;
    }
    return 0;
}
