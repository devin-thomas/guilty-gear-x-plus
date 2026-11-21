/* The EE-FPU Model: see ee_fpu.h for the rules. Integer arithmetic only, so
 * the result never depends on the host's floating point. It is compiled into
 * the Core (as wasm, then C) and natively for tests/fpu. */
#include "ee_fpu.h"

#define SIGN 0x80000000u
#define EXP 0x7f800000u
#define FRAC 0x007fffffu
#define FMAX 0x7f7fffffu
#define ONE 0x00800000u     /* a significand's implicit one */

/* an operand as the FPU reads it (PCSX2's fpuDouble) */
static uint32_t operand(uint32_t f)
{
    switch (f & EXP) {
    case 0:
        return f & SIGN;
    case EXP:
        return (f & SIGN) | FMAX;
    default:
        return f;
    }
}

static int exponent(uint32_t f)
{
    return (int)((f >> 23) & 0xff);
}

static uint32_t significand(uint32_t f)
{
    return (f & FRAC) | ONE;
}

/* PCSX2's interpreter: the exact result, chopped */
/* a result: biased exponent e, 24-bit significand already truncated toward
   zero. Too large clamps to the largest finite value; below 2^-126 flushes
   to a signed zero. */
static uint32_t pack(uint32_t sign, int e, uint32_t sig)
{
    if (e >= 255)
        return sign | FMAX;
    if (e <= 0)
        return sign;
    return sign | ((uint32_t)e << 23) | (sig & FRAC);
}

/* m >> n, with any bit shifted out ORed into bit 0 */
static uint64_t shift_right_jam(uint64_t m, int n)
{
    if (n == 0)
        return m;
    if (n >= 64)
        return m != 0;
    return (m >> n) | ((m << (64 - n)) != 0);
}

/* a + b for two operands. The significands sit at bits 55..32, so 32 bits
   below them carry what alignment shifts out; truncating the exact sum then
   only needs the sticky bit, for a subtraction. */
static uint32_t add_core(uint32_t a, uint32_t b)
{
    uint32_t t, sign;
    uint64_t ma, mb, m;
    int e;

    if (exponent(a) == 0) {
        if (exponent(b) == 0)
            return a & b & SIGN;    /* -0 + -0 is -0; any other pair of zeros is +0 */
        return b;
    }
    if (exponent(b) == 0)
        return a;
    if ((a & ~SIGN) < (b & ~SIGN)) {
        t = a;
        a = b;
        b = t;
    }
    sign = a & SIGN;
    e = exponent(a);
    ma = (uint64_t)significand(a) << 32;
    mb = shift_right_jam((uint64_t)significand(b) << 32, e - exponent(b));
    if ((a ^ b) & SIGN) {
        m = ma - mb;
        if (m == 0)
            return 0;               /* x - x is +0 */
    } else {
        m = ma + mb;
        if (m >> 56) {
            m >>= 1;
            e++;
        }
    }
    while (!(m >> 55)) {
        m <<= 1;
        e--;
    }
    return pack(sign, e, (uint32_t)(m >> 32));
}

static uint32_t mul_core(uint32_t a, uint32_t b)
{
    uint32_t sign = (a ^ b) & SIGN;
    uint64_t p;
    int e;

    if (exponent(a) == 0 || exponent(b) == 0)
        return sign;
    p = (uint64_t)significand(a) * significand(b);     /* 2^46 <= p < 2^48 */
    e = exponent(a) + exponent(b) - 127;
    if (p >> 47)
        return pack(sign, e + 1, (uint32_t)(p >> 24));
    return pack(sign, e, (uint32_t)(p >> 23));
}

/* a / b for two operands, b not zero */
static uint32_t div_core(uint32_t a, uint32_t b)
{
    uint32_t sign = (a ^ b) & SIGN;
    uint64_t q;
    int e;

    if (exponent(a) == 0)
        return sign;
    /* the integer quotient truncates, which is what rounding toward zero
       needs: 2^39 < q < 2^41 */
    q = ((uint64_t)significand(a) << 40) / significand(b);
    e = exponent(a) - exponent(b) + 127;
    if (q >> 40)
        return pack(sign, e, (uint32_t)(q >> 17));
    return pack(sign, e - 1, (uint32_t)(q >> 16));
}

/* floor(sqrt(n)) for n < 2^48 */
static uint32_t isqrt(uint64_t n)
{
    uint64_t r = 0, bit = (uint64_t)1 << 48;

    while (bit > n)
        bit >>= 2;
    while (bit) {
        if (n >= r + bit) {
            n -= r + bit;
            r = (r >> 1) + bit;
        } else {
            r >>= 1;
        }
        bit >>= 2;
    }
    return (uint32_t)r;
}

/* sqrt of a positive operand with a non-zero exponent: the value is
   sig * 2^p; n = sig << j with p - j even and 2^46 <= n < 2^48, so the
   integer square root is the truncated 24-bit result */
static uint32_t sqrt_core(uint32_t a)
{
    int p = exponent(a) - 127 - 23;
    int j = (p & 1) ? 23 : 24;
    uint32_t r = isqrt((uint64_t)significand(a) << j);

    return pack(0, 23 + (p - j) / 2 + 127, r);
}

uint32_t ee_fpu_add(uint32_t s, uint32_t t)
{
    return add_core(operand(s), operand(t));
}

uint32_t ee_fpu_sub(uint32_t s, uint32_t t)
{
    return add_core(operand(s), operand(t) ^ SIGN);
}

uint32_t ee_fpu_mul(uint32_t s, uint32_t t)
{
    return mul_core(operand(s), operand(t));
}

uint32_t ee_fpu_div(uint32_t s, uint32_t t)
{
    if ((t & EXP) == 0)
        return ((s ^ t) & SIGN) | FMAX;
    return div_core(operand(s), operand(t));
}

/* sqrt.s of a negative value is the root of its magnitude; -0 (or a
   negative denormal) gives -0 */
uint32_t ee_fpu_sqrt(uint32_t t)
{
    if ((t & EXP) == 0)
        return t & SIGN;
    return sqrt_core(operand(t) & ~SIGN);
}

/* rsqrt.s is s / sqrt(|t|): the root is rounded first, then divided */
uint32_t ee_fpu_rsqrt(uint32_t s, uint32_t t)
{
    if ((t & EXP) == 0)
        return (t & SIGN) | FMAX;
    return div_core(operand(s), sqrt_core(operand(t) & ~SIGN));
}

uint32_t ee_fpu_madd(uint32_t acc, uint32_t s, uint32_t t)
{
    return add_core(operand(acc), ee_fpu_mul(s, t));
}

uint32_t ee_fpu_msub(uint32_t acc, uint32_t s, uint32_t t)
{
    return add_core(operand(acc), ee_fpu_mul(s, t) ^ SIGN);
}

/* madda.s and msuba.s read ACC as it is, not as an operand. ACC only ever
   holds clamped results, but if it held an infinity the sum is that
   infinity, clamped; a NaN stays a NaN, made quiet. */
static uint32_t acc_special(uint32_t acc)
{
    return (acc & FRAC) ? acc | 0x00400000u : (acc & SIGN) | FMAX;
}

uint32_t ee_fpu_madda(uint32_t acc, uint32_t s, uint32_t t)
{
    if ((acc & EXP) == EXP)
        return acc_special(acc);
    return ee_fpu_madd(acc, s, t);
}

uint32_t ee_fpu_msuba(uint32_t acc, uint32_t s, uint32_t t)
{
    if ((acc & EXP) == EXP)
        return acc_special(acc);
    return ee_fpu_msub(acc, s, t);
}

uint32_t ee_fpu_abs(uint32_t s)
{
    return s & ~SIGN;
}

uint32_t ee_fpu_neg(uint32_t s)
{
    return s ^ SIGN;
}

/* max.s and min.s compare the raw bits as signed integers, reversed when
   both are negative */
uint32_t ee_fpu_max(uint32_t s, uint32_t t)
{
    int32_t a = (int32_t)s, b = (int32_t)t;

    if (a < 0 && b < 0)
        return (uint32_t)(a < b ? a : b);
    return (uint32_t)(a > b ? a : b);
}

uint32_t ee_fpu_min(uint32_t s, uint32_t t)
{
    int32_t a = (int32_t)s, b = (int32_t)t;

    if (a < 0 && b < 0)
        return (uint32_t)(a > b ? a : b);
    return (uint32_t)(a < b ? a : b);
}

static int lt(uint32_t a, uint32_t b)
{
    if (((a | b) & ~SIGN) == 0)
        return 0;
    if ((a ^ b) & SIGN)
        return (a & SIGN) != 0;
    return (a & SIGN) ? a > b : a < b;
}

static int eq(uint32_t a, uint32_t b)
{
    return a == b || ((a | b) & ~SIGN) == 0;
}

int ee_fpu_c_eq(uint32_t s, uint32_t t)
{
    return eq(operand(s), operand(t));
}

int ee_fpu_c_lt(uint32_t s, uint32_t t)
{
    return lt(operand(s), operand(t));
}

int ee_fpu_c_le(uint32_t s, uint32_t t)
{
    uint32_t a = operand(s), b = operand(t);

    return lt(a, b) || eq(a, b);
}

uint32_t ee_fpu_cvt_s_w(int32_t w)
{
    uint32_t sign = w < 0 ? SIGN : 0;
    uint32_t u = w < 0 ? 0u - (uint32_t)w : (uint32_t)w;
    int e = 127 + 31;

    if (u == 0)
        return 0;
    while (!(u & SIGN)) {
        u <<= 1;
        e--;
    }
    return sign | ((uint32_t)e << 23) | ((u >> 8) & FRAC);
}

/* cvt.w.s reads its operand raw: below 2^31 in magnitude it truncates (a zero
   exponent reads as 0); otherwise it saturates by sign, NaN patterns
   included */
int32_t ee_fpu_cvt_w_s(uint32_t s)
{
    int e = exponent(s), sh;
    uint32_t mag;

    if ((s & EXP) > 0x4E800000u)
        return (s & SIGN) ? (int32_t)0x80000000u : 0x7fffffff;
    if (e < 127)
        return 0;
    sh = e - 127 - 23;
    mag = sh >= 0 ? significand(s) << sh : significand(s) >> -sh;
    return (s & SIGN) ? -(int32_t)mag : (int32_t)mag;
}

uint32_t ee_fpu_cvt_s_wu(uint32_t u)
{
    uint32_t t;

    if (!(u & SIGN))
        return ee_fpu_cvt_s_w((int32_t)u);
    t = ee_fpu_cvt_s_w((int32_t)((u >> 1) | (u & 1)));
    return ee_fpu_add(t, t);
}
