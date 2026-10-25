#include "generated/file.h"

int sceCdInit(int init_mode);
int sceCdMmode(int media);

void FileReset(void) {
    sceCdInit(0);
    sceCdMmode(1);
}

void FileInit(void) {
    int i;

    comp_enable = -1;
    FileBufPointR = 0;
    FileBufPointW = 0;
    FileStatus = 0;
    FileSelectedAddr[0] = mmalloc(0x870000, "PLAYER1");
    FileSelectedAddr[1] = mmalloc(0x870000, "PLAYER2");
    FileSelectedAddr[2] = mmalloc(0x282000, "BG");
    FileSelectedAddr[3] = mmalloc(0xBE000, "VOICE1");
    FileSelectedAddr[4] = mmalloc(0xBE000, "VOICE2");
    FileSelectedAddr[5] = mmalloc(0xC800, "SKILLREF1");
    FileSelectedAddr[6] = mmalloc(0xC800, "SKILLREF2");
    for (i = 0; i < 7; i++) {
        FileSelected[i] = -1;
        debugf("FileMalloc %x\n", FileSelectedAddr[i]);
    }
}


int func_001BBF50(sceCdlFILE *fp, char *name);

void MakeFileTable(void) {
    sceCdlFILE fp;

    func_001BBF50(&fp, "\\CDDATA.BIN;1");
    CdDataTop = fp.lsn;
}


int GetFileTop(int fid) {
    return seekpos[fid][0] + CdDataTop;
}

int GetFileSize(int fid) {
    if (seekpos[fid][1] == 0) {
        return (seekpos[fid + 1][0] - seekpos[fid][0]) << 11;
    }
    return seekpos[fid][1] + 0x800;
}

void debugf(char *format, ...);

void FileRead(int fid, void *adr) {
    FILE_STR *p;

    debugf("File Requested %d > %x\n", fid, adr);
    p = &FileBuf[FileBufPointW];
    p->id = fid;
    p->buf = adr;
    FileBufPointW = (FileBufPointW + 1) % 10;
}


void FileReload(int id) {
    FILE_STR *src;
    FILE_STR *dst;

    src = &FileBuf[id];
    dst = &FileBuf[FileBufPointW];
    dst->id = src->id;
    FileBufPointW = (FileBufPointW + 1) % 10;
}


char *FileReadMem(int fid) {
    char *p;

    p = dmalloc(GetFileSize(fid), "FileMem");
    if (p) {
        FileRead(fid, p);
    }
    return p;
}


void FileUncompress(void)
{
    FILE_STR *p;
    u_char *src;
    u_char *dst;
    u_int code;
    int size;
    int count;
    int i;
    int offset;
    int num;

    if (comp_enable < 0) {
        return;
    }
    p = &FileBuf[comp_enable];
    count = 0;
    size = p->unc_size;
    src = p->unc_src;
    dst = p->unc_dst;
    if (size > 0) {
    do {
        if (count > 0x40000) {
            p->unc_src = src;
            p->unc_dst = dst;
            p->unc_size = size;
            return;
        }
        if (dst >= src) {
            debugf("Uncompress Danger\n");
            FileReload(comp_enable);
            break;
        }
        if (*src & 0x80) {
            code = (src[0] << 8) | src[1];
            num = ((code & 0x7800) >> 11) + 3;
            offset = (code & 0x7FF) + 1;
            if (num > size) {
                num = size;
            }
            src += 2;
            for (i = 0; i < num; i++) {
                *dst = *(dst - offset);
                dst++;
            }
            size -= num;
            count += num;
        } else {
            num = *src;
            if (num >= size) {
                num = size - 1;
            }
            src++;
            for (i = 0; i <= num; i++) {
                *dst++ = *src++;
            }
            size -= num + 1;
            count += num + 1;
        }
    } while (size > 0);
    }
    comp_enable = -1;
}


int IOPLock(void);
void IOPUnLock(void);
int SnDrvSeVWait(void);
void SnDrvSeVLoadCD(int chan, int top, int size);
int sceCdSync(int mode);
int sceCdDiskReady(int mode);
int sceCdRead(u_int lbn, u_int sectors, void *buf, sceCdRMode *mode);
int sceCdGetError(void);
int sceCdStop(void);

void FileReadMain(void)
{
    FILE_STR *p;
    sceCdRMode mode;
    int err;
    int size;
    char *src;

    FileUncompress();
    p = &FileBuf[FileBufPointR];
    switch (FileStatus) {
    case 0:
        if (FileBufPointR == FileBufPointW) {
            return;
        }
        if (IOPLock()) {
            debugf("IOPLock Waiting\n");
            return;
        }
        if (sceCdSync(1)) {
            debugf("Sync Waiting\n");
            return;
        }
        if ((u_int)((int)p->buf - 1) < 3) {
            IOPUnLock();
            debugf("SE Read Request %x %x %x\n", (int)p->buf - 1, GetFileTop(p->id), seekpos[p->id + 1][0] - seekpos[p->id][0]);
            SnDrvSeVLoadCD((int)p->buf - 1, GetFileTop(p->id), seekpos[p->id + 1][0] - seekpos[p->id][0]);
            FileStatus = 3;
            break;
        }
        mode.trycount = 0;
        mode.spindlctrl = 1;
        mode.datapattern = 0;
        sceCdDiskReady(0);
        if (seekpos[p->id][1] == 0) {
            size = GetFileSize(p->id);
            src = p->buf;
        } else {
            size = seekpos[p->id][1];
            src = (char *)p->buf + GetFileSize(p->id) - ((seekpos[p->id + 1][0] - seekpos[p->id][0]) << 11);
        }
        debugf("Read Request %x %x %x\n", GetFileTop(p->id), size, src);
        if (sceCdRead(GetFileTop(p->id), seekpos[p->id + 1][0] - seekpos[p->id][0], src, &mode) == 0) {
            debugf("File Read Command Error\n");
            return;
        }
        ReadTime = 0;
        FileStatus = 2;
        break;
    case 2:
        if (++ReadTime > 0x708) {
            debugf("Read Error(TimeUp) -> Retry\n");
            goto stop;
        }
        if (sceCdSync(1)) {
            return;
        }
        debugf("File Read Over\n");
        err = sceCdGetError();
        if (err) {
            debugf("Read Error(%d) -> Retry\n", err);
        stop:
            if (sceCdStop()) {
                FileStatus = 4;
            }
            break;
        }
        if (seekpos[p->id][1]) {
            if (comp_enable >= 0) {
                return;
            }
            p->unc_size = seekpos[p->id][1];
            p->unc_dst = p->buf;
            p->unc_src = (u_char *)p->buf + GetFileSize(p->id) - ((seekpos[p->id + 1][0] - seekpos[p->id][0]) << 11);
            comp_enable = FileBufPointR;
        }
        FileBufPointR = (FileBufPointR + 1) % 10;
        IOPUnLock();
        FileStatus = 0;
        break;
    case 3:
        err = SnDrvSeVWait();
        if (err == 0) {
            FileBufPointR = (FileBufPointR + 1) % 10;
            IOPUnLock();
            FileStatus = 0;
            break;
        }
        if (err >= 0) {
            break;
        }
        goto reset;
    case 4:
        debugf("CD HOLDING\n");
        if (sceCdSync(1)) {
            return;
        }
    reset:
        FileReset();
        FileStatus = 0;
        break;
    }
}


int FileReadCheck(void) {
    if (FileStatus == 0 && FileBufPointR == FileBufPointW && comp_enable < 0) {
        return 1;
    }
    return 0;
}


char *strcat(char *dst, const char *src);
int sceSifLoadModule(const char *name, int arglen, const char *args);
extern long long D_5B8400[]; /* RODATA_EXTERN 0x5b8400: "cdrom0:\\" (orig: strcpy(buf, "cdrom0:\\") inlined as an aligned 9-byte copy) */
extern char D_5CAF90[]; /* RODATA_EXTERN 0x5caf90: ";1" (.sdata) */
extern char D_5B8410[]; /* RODATA_EXTERN 0x5b8410: "Module Load %s\n" */
extern char D_5B8420[]; /* RODATA_EXTERN 0x5b8420: "Module Load Error\n" */

void FileIRXLoad(char *fname) {
    char buf[60];

    __builtin_memcpy(buf, D_5B8400, 9);
    strcat(buf, fname);
    strcat(buf, D_5CAF90);
    debugf(D_5B8410, buf);
    if (sceSifLoadModule(buf, 0, 0) < 0) {
        debugf(D_5B8420);
    }
}


char *FileReadSelect(int fid, int no) {
    if (FileSelected[no] != fid) {
        FileSelected[no] = fid;
        FileRead(fid, FileSelectedAddr[no]);
    }
    return FileSelectedAddr[no];
}
