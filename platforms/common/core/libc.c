/* The C library calls the game makes that the decomp has no matched C for.
 *
 * Compiled into the Core module (wasm32), freestanding. newlib's malloc,
 * rand and errno and fdlibm come from src/sdk/ as matched C, so the heap,
 * the random sequence and the math are the PS2's own. What is here has one
 * correct answer for every input, so plain C reproduces newlib 1.8.2:
 * the string and memory functions, abs, and the integer subset of sprintf
 * the game uses (%d with width and zero padding, plus %u %x %X %o %c %s %%).
 *
 * sbrk is libkernl's (src/sdk/sce/glue.c) without the interrupt masking,
 * which only matters on the EE: the break lives in the same PS2 variable
 * and the heap ends where crt0 put the main thread's stack.
 */
#include <stdarg.h>

typedef unsigned int size_t;

/* crt0 (both discs): SetupThread(_gp, 0x1fe0000, 0x20000, ...) and
   SetupHeap(_end, -1), so the heap runs from _end up to the stack. */
#define GGX_END_OF_HEAP 0x01fe0000u

void *memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        while (n--) {
            d[n] = s[n];
        }
    }
    return dst;
}

void *memset(void *dst, int c, size_t n)
{
    unsigned char *d = dst;

    while (n--) {
        *d++ = (unsigned char)c;
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const unsigned char *x = a, *y = b;

    for (; n; n--, x++, y++) {
        if (*x != *y) {
            return *x - *y;
        }
    }
    return 0;
}

size_t strlen(const char *s)
{
    const char *p = s;

    while (*p) {
        p++;
    }
    return p - s;
}

char *strcpy(char *dst, const char *src)
{
    char *d = dst;

    while ((*d++ = *src++) != 0) {
    }
    return dst;
}

char *strcat(char *dst, const char *src)
{
    strcpy(dst + strlen(dst), src);
    return dst;
}

int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int abs(int v)
{
    return v < 0 ? -v : v;
}

/* newlib vfprintf for the integer conversions: flags - 0 + space #, width
   and precision (digits or *), the h, l and ll length modifiers. EE-GCC's
   long is 64 bits; the port builds game code with long as long long. */
static char *put_number(char *out, unsigned long long v, int neg, int base, int upper,
                        int width, int prec, int left, int zero, char sign, int alt)
{
    char digits[24];
    const char *set = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    int n = 0, len, pad;

    while (v) {
        digits[n++] = set[v % base];
        v /= base;
    }
    if (prec < 0) {
        prec = 1;
    } else {
        zero = 0;
    }
    if (alt && base == 8 && prec <= n) {
        prec = n + 1;
    }
    len = (n > prec ? n : prec) + (neg || sign ? 1 : 0) + (alt && base == 16 && n ? 2 : 0);
    pad = width > len ? width - len : 0;
    if (!left && !zero) {
        while (pad-- > 0) {
            *out++ = ' ';
        }
    }
    if (neg) {
        *out++ = '-';
    } else if (sign) {
        *out++ = sign;
    }
    if (alt && base == 16 && n) {
        *out++ = '0';
        *out++ = upper ? 'X' : 'x';
    }
    if (!left && zero) {
        while (pad-- > 0) {
            *out++ = '0';
        }
    }
    for (len = n; len < prec; len++) {
        *out++ = '0';
    }
    while (n) {
        *out++ = digits[--n];
    }
    if (left) {
        while (pad-- > 0) {
            *out++ = ' ';
        }
    }
    return out;
}

int vsprintf(char *buf, const char *fmt, va_list ap)
{
    char *out = buf;

    for (; *fmt; fmt++) {
        int left = 0, zero = 0, alt = 0, width = 0, prec = -1, lng = 0;
        char sign = 0;

        if (*fmt != '%') {
            *out++ = *fmt;
            continue;
        }
        for (fmt++;; fmt++) {
            if (*fmt == '-') {
                left = 1;
            } else if (*fmt == '0') {
                zero = 1;
            } else if (*fmt == '+') {
                sign = '+';
            } else if (*fmt == ' ') {
                if (!sign) {
                    sign = ' ';
                }
            } else if (*fmt == '#') {
                alt = 1;
            } else {
                break;
            }
        }
        if (*fmt == '*') {
            width = va_arg(ap, int);
            if (width < 0) {
                left = 1;
                width = -width;
            }
            fmt++;
        } else {
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt++ - '0');
            }
        }
        if (*fmt == '.') {
            fmt++;
            prec = 0;
            if (*fmt == '*') {
                prec = va_arg(ap, int);
                fmt++;
            } else {
                while (*fmt >= '0' && *fmt <= '9') {
                    prec = prec * 10 + (*fmt++ - '0');
                }
            }
        }
        while (*fmt == 'h' || *fmt == 'l') {
            lng += *fmt++ == 'l';
        }
        switch (*fmt) {
        case 'd':
        case 'i': {
            long long v = lng ? va_arg(ap, long long) : va_arg(ap, int);

            out = put_number(out, v < 0 ? 0 - (unsigned long long)v : (unsigned long long)v,
                             v < 0, 10, 0, width, prec, left, zero, sign, 0);
            break;
        }
        case 'u':
        case 'x':
        case 'X':
        case 'o': {
            unsigned long long v = lng ? va_arg(ap, unsigned long long) : va_arg(ap, unsigned int);
            int base = *fmt == 'u' ? 10 : *fmt == 'o' ? 8 : 16;

            out = put_number(out, v, 0, base, *fmt == 'X', width, prec, left, zero, 0, alt);
            break;
        }
        case 'p':
            out = put_number(out, (unsigned int)va_arg(ap, void *), 0, 16, 0, width, prec,
                             left, zero, 0, 1);
            break;
        case 'c': {
            int pad = width > 1 ? width - 1 : 0;

            while (!left && pad-- > 0) {
                *out++ = ' ';
            }
            *out++ = (char)va_arg(ap, int);
            while (left && pad-- > 0) {
                *out++ = ' ';
            }
            break;
        }
        case 's': {
            const char *s = va_arg(ap, const char *);
            int n, pad;

            if (!s) {
                s = "(null)";
            }
            for (n = 0; s[n] && (prec < 0 || n < prec); n++) {
            }
            pad = width > n ? width - n : 0;
            while (!left && pad-- > 0) {
                *out++ = ' ';
            }
            while (n--) {
                *out++ = *s++;
            }
            while (left && pad-- > 0) {
                *out++ = ' ';
            }
            break;
        }
        case '%':
            *out++ = '%';
            break;
        default:
            /* no float conversions in the game's format strings */
            *out++ = '%';
            if (*fmt) {
                *out++ = *fmt;
            } else {
                fmt--;
            }
            break;
        }
    }
    *out = 0;
    return out - buf;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsprintf(buf, fmt, ap);
    va_end(ap);
    return n;
}

int *__errno(void);
/* glue.c's file static; the decomp names it after its symbol */
extern char *ggx_heap_ptr __asm__("heap_ptr.30");

void *sbrk(int incr)
{
    char *old = ggx_heap_ptr;
    char *new = old + incr;

    if ((char *)GGX_END_OF_HEAP < new) {
        *__errno() = 12;
        return (void *)-1;
    }
    ggx_heap_ptr = new;
    return old;
}
