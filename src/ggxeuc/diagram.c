#include "generated/diagram.h"

void HighScore_EntryName(SCOREDATA_BYCHARACTER *p, int idno, int rank, char *name)
{
    HighScore_EntryNameSub(&p[idno], rank, name);
}

char *HighScore_GetName(SCOREDATA_BYCHARACTER *p, int idno, int rank)
{
    p[idno].dat[rank].name[3] = 0;
    return p[idno].dat[rank].name;
}

unsigned int HighScore_GetScore(SCOREDATA_BYCHARACTER *p, int idno, int rank)
{
    return p[idno].dat[rank].score;
}

int HighScore_Entry(SCOREDATA_BYCHARACTER *p, int idno, unsigned int score)
{
    return HighScore_EntrySub(&p[idno], score);
}

void Diagram_Clear(char *src)
{
    int i;

    for (i = 0; i < 0x990; i++) {
        *src++ = 0xAA;
    }
}

void HighScore_EntryNameSub(SCOREDATA_BYCHARACTER *p, int rank, char *src)
{
    char *dst;
    int i;

    dst = p->dat[rank].name;
    i = 3;
    while (i--) {
        *dst++ = *src++;
    }
    *dst = 0;
}

char *strcpy(char *dst, char *src);

void ClrPassWord(USER_BACKUP *p)
{
    strcpy(p->password, "AAAAAAAA");
}

int strcmp(const char *a, const char *b);
extern USER_SETTING userinfo;

int CheckPassword(char *m)
{
    return strcmp(userinfo.UserBackUp.password, m) == 0;
}

void BackupromInit(void)
{
    strcpy(userinfo.UserBackUp.name, "KYKISKE");
    userinfo.UserBackUp.id = 0xE5E6E7E8;
    Diagram_Clear(userinfo.UserBackUp.diagram);
    HighScore_Clear(userinfo.UserBackUp.score);
    ClrPassWord(&userinfo.UserBackUp);
}

int BauckUpChkChank(void)
{
    char *p;

    p = userinfo.UserBackUp.name;
    if (strcmp(p, "KYKISKE")) {
        return 1;
    }
    return *(int *)(p - 4) != 0xE5E6E7E8;
}

void HighScore_Clear(SCOREDATA_BYCHARACTER *p)
{
    int i;

    for (i = 0; i < 19; i++) {
        HighScore_ClearSub(p);
        p++;
    }
}

void Diagram_SetResult(char *src, int win, int lose)
{
    int result;

    result = 1;
    if (win == lose) {
        return;
    }
    if (win > lose) {
        result = 0;
    }
    Diagram_WriteResult(Diagram_GetAccessPointer(src, win, lose), result);
}

void HighScore_ClearSub(SCOREDATA_BYCHARACTER *p)
{
    int i;
    SCOREDATA *d;

    d = p->dat;
    for (i = 0; i < 5; i++) {
        d->score = 50000 - i * 10000;
        d->name[0] = 'G';
        d->name[1] = 'G';
        d->name[2] = 'X';
        d->name[3] = 0;
        d++;
    }
}

int Diagram_GetWinnerCount(char *src)
{
    int cnt;
    int i;
    int j;
    unsigned char dat;
    unsigned char *p;

    p = src;
    cnt = 0;
    if (src == 0) {
        return 64;
    }
    for (i = 0; i < 16; i++) {
        dat = *p++;
        for (j = 0; j < 8; j++) {
            cnt += dat & 1;
            dat >>= 1;
        }
    }
    return cnt;
}

int HighScore_EntrySub(SCOREDATA_BYCHARACTER *p, unsigned int score)
{
    SCOREDATA *offset;
    int i;
    int j;
    SCOREDATA *src;

    offset = p->dat;
    for (i = 0; i < 5; i++) {
        if (offset->score < score) {
            src = &p->dat[3];
            for (j = 3; j >= i; j--) {
                src[1] = src[0];
                src--;
            }
            offset->score = score;
            offset->name[0] = ' ';
            offset->name[1] = ' ';
            offset->name[2] = ' ';
            offset->name[3] = 0;
            return i;
        }
        offset++;
    }
    return -1;
}

int Diagram_GetScore(int cnt)
{
    if (cnt >= 104) {
        cnt = 9;
    } else if (cnt >= 89) {
        cnt = 8;
    } else if (cnt >= 79) {
        cnt = 7;
    } else if (cnt >= 71) {
        cnt = 6;
    } else if (cnt >= 58) {
        cnt = 5;
    } else if (cnt >= 50) {
        cnt = 4;
    } else if (cnt >= 40) {
        cnt = 3;
    } else if (cnt >= 25) {
        cnt = 2;
    } else {
        cnt = 1;
    }
    return cnt;
}

void Diagram_WriteResult(char *src, int result)
{
    int i;
    unsigned char *p;
    unsigned char dat;

    p = src;
    if (src == 0) {
        return;
    }
    for (i = 0; i < 16; i++, p++) {
        dat = *p >> 1;
        *p = dat;
        if ((i == 15 && result) || (i != 15 && (p[1] & 1))) {
            *p = dat | 0x80;
        } else if (i != 15) {
            *p = dat;
        }
    }
}

char *Diagram_GetAccessPointer(char *p, int A, int B)
{
    int index;
    int i;
    int temp;

    index = 0;
    A--;
    B--;
    if (A == B) {
        return 0;
    }
    if (B < A) {
        temp = B;
        B = A;
        A = temp;
    }
    for (i = A; i != 0; i--) {
        index += 18 - i;
    }
    index += B - A - 1;
    return p + index * 16;
}

void PasswordDecode(unsigned short *src, char *dst)
{
    char key[9] = "\x8b\x90\x94\x96\x92\x9a\x94\x96";
    unsigned short *from;
    char *to;
    int i;

    from = src;
    to = dst;
    for (i = 0; i < 8; i++) {
        *to++ = *from++ / (unsigned short)~key[i];
        if (*from == 0) {
            break;
        }
    }
    *to = 0;
}

int IsLimitPassword(void)
{
    unsigned short testa[9] = { 0x2528, 0x2241, 0x202F, 0x1AA9, 0x2136, 0x2124, 0x1E83, 0x1B7B, 0 };
    unsigned short dizzy[9] = { 0x22E4, 0x1FA7, 0x22B1, 0x2067, 0x2357, 0x1F2B, 0x2387, 0x20D0, 0 };
    char buff[16];

    PasswordDecode(dizzy, buff);
    if (CheckPassword(buff)) {
        return 2;
    }
    PasswordDecode(testa, buff);
    return CheckPassword(buff) != 0;
}
