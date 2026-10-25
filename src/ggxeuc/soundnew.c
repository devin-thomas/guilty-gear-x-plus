#include "generated/soundnew.h"

extern int IOPLockFlag;

void *memcpy(void *dst, const void *src, unsigned int n);
void SnDrvMain(void);

extern sceSifClientData gCd;
extern u_int sbuff[65536];
int sceSifCallRpc(sceSifClientData *cd, unsigned int rpc_number, unsigned int mode, void *send, int ssize, void *receive, int rsize, sceSifEndFunc end_function, void *end_param);


void SnDrvMain(void)
{
    SND_TRANS_BUF *p;
    int top;
    int size;
    u_int *src;
    u_int *dst;
    int i;

    if (IOPLockFlag) {
        return;
    }
    if (sceSifCheckStatRpc(&gCd)) {
        return;
    }
    if (snd_enable) {
        sceSifCallRpc(&gCd, 0x1F, 0, sbuff, 0x10, sbuff, 0x10, 0, 0);
        bgm_mode = sbuff[0];
    }
    if (se_loading > 0) {
        sceSifCallRpc(&gCd, 0x20, 0, sbuff, 0x10, sbuff, 0x10, 0, 0);
        if (sbuff[0] == 0) {
            se_loading--;
        }
    }
    while (snd_trans_wr != snd_trans_rd) {
        p = &snd_trans_buf[snd_trans_rd];
        memcpy(sbuff, p->data, p->size);
        if (p->cmd == 0xD) {
            size = sbuff[1];
            top = sbuff[2];
            if (size > 0x40000) {
                snd_trans_rd = (snd_trans_rd + 1) % 64;
                continue;
            }
            dst = &sbuff[0x10];
            src = (u_int *)top;
            for (i = 0; i < size / 4; i++) {
                *dst++ = *src++;
            }
            sceSifCallRpc(&gCd, 0x8D, 1, sbuff, size + 0x50, sbuff, 0, 0, 0);
            snd_trans_rd = (snd_trans_rd + 1) % 64;
            return;
        }
        if (p->cmd == 0x12) {
            sceSifCallRpc(&gCd, 0x92, 1, sbuff, p->size, sbuff, 0, 0, 0);
            snd_trans_rd = (snd_trans_rd + 1) % 64;
            return;
        }
        snd_trans_rd = (snd_trans_rd + 1) % 64;
        if (snd_trans_rd == snd_trans_wr) {
            sceSifCallRpc(&gCd, p->cmd | 0x80, 0, sbuff, p->size, sbuff, 0, 0, 0);
        } else {
            sceSifCallRpc(&gCd, p->cmd, 0, sbuff, p->size, sbuff, 0, 0, 0);
        }
    }
}


void SnDrvEntry(int cmd, int *data, int size) {
    SND_TRANS_BUF *p;

    p = &snd_trans_buf[snd_trans_wr];
    p->cmd = cmd;
    p->size = size;
    if (size > 0x20000) {
        return;
    }
    memcpy(p->data, data, size);
    snd_trans_wr = (snd_trans_wr + 1) % 64;
    if (snd_trans_wr == snd_trans_rd) {
        debugf("Sound Trans Overflow\n");
    }
    SnDrvMain();
}


extern sceSifClientData gCd;
extern u_int sbuff[65536];
void sceSifInitRpc(unsigned int mode);
int sceSifBindRpc(sceSifClientData *cd, unsigned int request, unsigned int mode);
int sceSifCallRpc(sceSifClientData *cd, unsigned int rpc_number, unsigned int mode, void *send, int ssize, void *receive, int rsize, sceSifEndFunc end_function, void *end_param);

void SnDrvInit(void) {
    int i;

    sceSifInitRpc(0);
    while (1) {
        if (sceSifBindRpc(&gCd, 0x1010101, 0) < 0) {
            debugf("Error:sceSifBindRpc\n");
            while (1) {
            }
        }
        for (i = 9999; i >= 0; i--) {
        }
        if (gCd.serve != 0) {
            break;
        }
    }
    sceSifCallRpc(&gCd, 0, 0, sbuff, 0, sbuff, 16, 0, 0);
    SnDrvRequestClear();
    snd_enable = 1;
}


void SnDrvBgmKeep(int vol, int loop, int size) {
    int data[2];

    vol_work[0] = vol;
    SnDrvEntry(0x18, vol_work, 8);
    debugf("BGM Keep %d %d\n", loop, size);
    data[0] = loop;
    data[1] = size;
    SnDrvEntry(0x19, data, 8);
    bgm_mode = 1;
}


void SnDrvBgmStart(int vol, int loop) {
    int data[2];

    vol_work[0] = vol;
    data[0] = loop;
    SnDrvEntry(0x18, vol_work, 8);
    debugf("BGM Start %d\n", data[0]);
    SnDrvEntry(0x16, data, 4);
    bgm_mode = 1;
}


extern int IOPLockFlag;

int IOPLock(void) {
    if (IOPLockFlag) {
        debugf("IOPLock (IOPLockFlag > 0 )\n");
        return 0;
    }
    if (SnDrvBgmCheck()) {
        debugf("IOPLock (SnDrvBgmCheck() > 0 )\n");
        SnDrvBgmStop();
        return 1;
    }
    IOPLockFlag = 1;
    return 0;
}


void IOPUnLock(void) {
    IOPLockFlag = 0;
}

int SnDrvBgmCheck(void) {
    return bgm_mode;
}

void SnDrvBgmMode(int mode) {
    vol_work[1] = mode;
    debugf(" SnDrvBgmMode Called %d\n", mode);
    SnDrvEntry(0x18, vol_work, 8);
}


int SnDrvGetBgmMode(void) {
    return vol_work[1];
}

void SnDrvReqClear(void) {
    last_trans = -1;
}

void SnDrvRequestClear(void) {
    snd_trans_rd = 0;
    snd_trans_wr = 0;
}

void SnDrvBgmVol(int vol) {
    vol_work[0] = vol;
    SnDrvEntry(0x18, vol_work, 8);
}

extern sceSifClientData gCd;
int sceSifCheckStatRpc(sceSifClientData *cd);

int SnDrvRpcCheck(void) {
    return sceSifCheckStatRpc(&gCd);
}

void SnDrvAllStop(void) {
    int data[4];

    SnDrvEntry(0xB, data, 0);
}

void SnDrvBgmStop(void) {
    int data[4];

    SnDrvEntry(0x17, data, 0);
}

void SnDrvStop(int chan) {
    int data[4];

    data[0] = chan;
    SnDrvEntry(0xA, data, 4);
}

void SnDrvBgmLoadCD(int top, int size) {
    int data[4];

    data[0] = top;
    data[1] = size;
    SnDrvEntry(0x14, data, 8);
    bgm_mode = 0;
}

void SnDrvBgmLoadPC(char *name) {
    se_loading = 3;
    SnDrvEntry(0x15, (int *)name, 0x40);
    bgm_mode = 0;
}

char *strcpy(char *dst, const char *src);

void SnDrvSeVLoadPC(int chan, char *name) {
    int data[16];

    data[0] = chan;
    se_loading = 3;
    strcpy((char *)&data[1], name);
    SnDrvEntry(0x10, data, 0x40);
}

void SnDrvSeVLoadCD(int chan, int top, int size) {
    int data[4];

    data[0] = chan;
    data[1] = top;
    data[2] = size << 11;
    se_loading = 3;
    SnDrvEntry(0x11, data, 0xC);
}

int SnDrvSeVWait(void) {
    int i;

    for (i = snd_trans_rd; i != snd_trans_wr; i = (i + 1) % 64) {
        if (snd_trans_buf[i].cmd == 0x14 || snd_trans_buf[i].cmd == 0x15) {
            se_loading = 3;
            break;
        }
    }
    return se_loading;
}

void SnDrvSeTrans(char *top, int size, int chan) {
    int data[3];

    if ((int)top == last_trans) {
        data[0] = chan;
        data[1] = 0;
        SnDrvEntry(0xF, data, 8);
    } else {
        data[0] = chan;
        data[1] = size;
        data[2] = (int)top;
        if (se_loading == 0) {
            last_trans = (int)top;
        } else {
            last_trans = -1;
        }
        SnDrvEntry(0xD, data, 0xC);
    }
}

void SnDrvSeVReq(int channel, int fid, int num) {
    int data[3];

    if (last_trans == num * 4 + fid) {
        data[0] = channel;
        data[1] = 0;
        SnDrvEntry(0xF, data, 8);
    } else {
        data[0] = channel;
        data[1] = fid;
        data[2] = num;
        SnDrvEntry(0x12, data, 0xC);
        if (se_loading == 0) {
            last_trans = num * 4 + fid;
        } else {
            last_trans = -1;
        }
    }
}
