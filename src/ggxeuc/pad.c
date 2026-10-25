#include "generated/pad.h"

int scePadInit(int mode);
int scePadPortOpen(int port, int slot, void *addr);
void debugf(char *format, ...);
extern unsigned int Pause;

void PadReadInit(void)
{
    if (scePadInit(0) <= 0) {
        debugf("Pad Init Fatal Error");
    }
    scePadPortOpen(0, 0, pad_read_buf[0]);
    scePadPortOpen(1, 0, pad_read_buf[1]);
    pad_actuator[0].small = 0;
    Pause = 1;
    pad_actuator[0].large = 0;
    pad_actuator[1].small = 0;
    pad_actuator[1].large = 0;
}


int scePadSetActAlign(int port, int slot, const unsigned char *data);

void PadInitActuator(int pad)
{
    u_char tbl[6] = { 0, 1, 0xff, 0xff, 0xff, 0xff };

    scePadSetActAlign(pad, 0, tbl);
}


void PadSetActuator(int pad, int sel, int time) {
    if (sel == 0) {
        pad_actuator[pad].small = time;
    } else {
        pad_actuator[pad].large = time;
    }
}

extern SYS_COND sc;
int SnDrvRpcCheck(void);
int scePadRead(int port, int slot, unsigned char *rdata);
int scePadGetReqState(int port, int slot);
int scePadSetActDirect(int port, int slot, const unsigned char *data);

void PadReadMain(void)
{
    u_char data[32];
    u_short d;

    pads[0].old = pads[0].data;
    pads[0].oldtrg = pads[0].trg;
    d = pads[0].data;
    if (!SnDrvRpcCheck()) {
        if (scePadRead(0, 0, data) && data[0] == 0 && scePadGetReqState(0, 0) == 0) {
            d = ~((data[2] << 8) | data[3]);
            switch (data[1] & 0xF0) {
            case 0x50:
            case 0x70:
                if (sc.connect[0] != 1) {
                    d = 0;
                    PadInitActuator(0);
                } else {
                    d &= 0xFFF;
                    if (data[6] < 0x30) {
                        d |= 0x8000;
                    }
                    if (data[6] > 0xD0) {
                        d |= 0x2000;
                    }
                    if (data[7] < 0x30) {
                        d |= 0x1000;
                    }
                    if (data[7] > 0xD0) {
                        d |= 0x4000;
                    }
                }
                sc.connect[0] = 1;
                break;
            case 0x40:
                if (sc.connect[0] != 1) {
                    d = 0;
                    PadInitActuator(0);
                }
                sc.connect[0] = 1;
                break;
            default:
                goto disc0;
            }
        } else {
            if (scePadGetReqState(0, 0) == 0) {
                d = 0;
            disc0:
                sc.connect[0] = 0;
            } else {
                d = 0;
            }
        }
    }
    pads[0].trg = d & ~pads[0].data;
    pads[0].offtrg = ~d & pads[0].data;
    pads[0].data = d;
    pads[1].old = pads[1].data;
    pads[1].oldtrg = pads[1].trg;
    d = pads[1].data;
    if (!SnDrvRpcCheck()) {
        if (scePadRead(1, 0, data) && data[0] == 0 && scePadGetReqState(1, 0) == 0) {
            d = ~((data[2] << 8) | data[3]);
            switch (data[1] & 0xF0) {
            case 0x50:
            case 0x70:
                if (sc.connect[1] != 1) {
                    d = 0;
                    PadInitActuator(1);
                } else {
                    d &= 0xFFF;
                    if (data[6] < 0x30) {
                        d |= 0x8000;
                    }
                    if (data[6] > 0xD0) {
                        d |= 0x2000;
                    }
                    if (data[7] < 0x30) {
                        d |= 0x1000;
                    }
                    if (data[7] > 0xD0) {
                        d |= 0x4000;
                    }
                }
                sc.connect[1] = 1;
                break;
            case 0x40:
                if (sc.connect[1] != 1) {
                    d = 0;
                    PadInitActuator(1);
                }
                sc.connect[1] = 1;
                break;
            default:
                goto disc1;
            }
        } else {
            if (scePadGetReqState(1, 0) == 0) {
                d = 0;
            disc1:
                sc.connect[1] = 0;
            } else {
                d = 0;
            }
        }
    }
    pads[1].trg = d & ~pads[1].data;
    pads[1].offtrg = ~d & pads[1].data;
    pads[1].data = d;
    if (pads[0].trg) {
        pads[0].repeat = pads[0].data;
        pads[0].count = 0;
    } else {
        pads[0].count++;
        if (pads[0].count < 20 || ((pads[0].count - 20) & 3)) {
            pads[0].repeat = 0;
        } else {
            pads[0].repeat = pads[0].data;
        }
    }
    if (pads[1].trg) {
        pads[1].repeat = pads[1].data;
        pads[1].count = 0;
    } else {
        pads[1].count++;
        if (pads[1].count < 20 || ((pads[1].count - 20) & 3)) {
            pads[1].repeat = 0;
        } else {
            pads[1].repeat = pads[1].data;
        }
    }
    pad.data = pads[0].data | pads[1].data;
    pad.trg = pads[0].trg | pads[1].trg;
    pad.repeat = pads[0].repeat | pads[1].repeat;
    if (pad_actuator[0].small) {
        data[0] = 0x80;
        pad_actuator[0].small--;
    } else {
        data[0] = 0;
    }
    if (pad_actuator[0].large) {
        data[1] = 0xC0;
        pad_actuator[0].large--;
    } else {
        data[1] = 0;
    }
    scePadSetActDirect(0, 0, data);
    if (pad_actuator[1].small) {
        data[0] = 0x80;
        pad_actuator[1].small--;
    } else {
        data[0] = 0;
    }
    if (pad_actuator[1].large) {
        data[1] = 0xC0;
        pad_actuator[1].large--;
    } else {
        data[1] = 0;
    }
    scePadSetActDirect(1, 0, data);
}
