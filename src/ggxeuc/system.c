#include "generated/system.h"

void heap_alloc(void) {
    HeapTop = mmalloc(0x30000, "HEAP");
    if (HeapTop == 0) {
        debugf("No Heap Area\n");
    }
    HeapNow = HeapTop;
    HeapEnd = HeapTop + 0x30000;
}


void heap_clear(void) {
    debugf("----Heap Area Clear----\n");
    SprNow = (char *)0x70000000;
    HeapNow = HeapTop;
    g_strip_init();
    PolyEffInit();
    Screen_MemKeep();
}



void debugf(char *format, ...);

void *smalloc(int size, char *name) {
    void *adr;

    if (size & 0xF) {
        size = size / 16 * 16 + 16;
    }
    if ((int)(SprNow + size) - 0x70000000 <= 0x4000) {
        adr = SprNow;
        debugf("Keep Spr Memory %s %x\n", name, adr);
        SprNow += size;
    } else {
        adr = HeapNow;
        if (HeapEnd < HeapNow + size) {
            debugf("DMalloc Size Overflow %s\n", name);
            return 0;
        }
        debugf("Keep Normal Memory %s %x\n", name, adr);
        HeapNow += size;
    }
    return adr;
}


void *dmalloc(int size, char *name) {
    char *p;

    if (size & 0xF) {
        size = size / 16 * 16 + 16;
    }
    p = HeapNow;
    if (HeapEnd < HeapNow + size) {
        debugf("DMalloc Size Overflow %s\n", name);
        return 0;
    }
    debugf("Keep Normal Memory %s %x\n", name, p);
    HeapNow += size;
    return p;
}


void *malloc(unsigned int size);

void *mmalloc(int size, char *name) {
    char *p;

    p = malloc(size);
    if (p == 0) {
        debugf("Malloc Size Overflow %s\n", name);
    } else {
        debugf("Malloc Enable(%s) %x-%x\n", name, p, p + size);
    }
    return p;
}


void dfree(void *mem) {
}

void memcopy(int *src, int *dst, int num) {
    for (; num > 0; num -= 4) {
        *dst++ = *src++;
    }
}

int sprintf(char *buf, const char *fmt, ...);

char *G_sprintf(char *buff, char *str, int *valtb) {
    if (buff == 0) {
        buff = tmpbuf;
    }
    sprintf(buff, str, valtb[0]);
    return buff;
}

int sprintf(char *buf, const char *fmt, ...);

char *G_sprintf_arg1(char *str, int arg1) {
    sprintf(tmpbuf, str, arg1);
    return tmpbuf;
}

int sprintf(char *buf, const char *fmt, ...);

char *G_sprintf_arg2(char *str, int arg1, int arg2) {
    sprintf(tmpbuf, str, arg1, arg2);
    return tmpbuf;
}

int sprintf(char *buf, const char *fmt, ...);

char *G_sprintf_arg3(char *str, int arg1, int arg2, int arg3) {
    sprintf(tmpbuf, str, arg1, arg2, arg3);
    return tmpbuf;
}
