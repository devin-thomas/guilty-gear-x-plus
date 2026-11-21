/* The EE's software floating point, for the Core.
 *
 * The EE FPU has no double precision. ee-gcc 2.9 compiles every double
 * operation, and every float/double conversion, to a call into libgcc's
 * fp-bit (dp-bit.o and fp-bit.o, built with NO_DENORMALS and renamed dpadd,
 * dpmul, fptodp, ...), and float to unsigned int to fp-bit's fptoui. The
 * decomp's matched C for those members (src/sdk/gcc/dp_bit.c, fp_bit.c,
 * _floatdisf.c) is compiled into the Core, and tools/fpu_lower.py turns each
 * such operation in the Core into a call here, so the Core runs the same
 * routines the EE runs.
 *
 * Most of these go straight to the matched function. Four matched functions
 * (dpadd, dpdiv, fptodp, dptofp) return through a MIPS register the C never
 * names: the value is whatever their last call left in v0 or f0, which only
 * works on the EE. Each is rebuilt here from the same matched helpers with an
 * explicit return; dpdiv's body is its matched C, unchanged except for that
 * return.
 *
 * Every value is a bit pattern: a double is a uint64_t, a float a uint32_t.
 */
#include <stdint.h>

#include "ee_fpu.h"
#include "ee_softfp.h"

/* fp-bit's unpacked forms, as dp_bit.c and fp_bit.c lay them out */
#define CLASS_SNAN 0
#define CLASS_QNAN 1
#define CLASS_ZERO 2
#define CLASS_NUMBER 3
#define CLASS_INFINITY 4

typedef struct {
    unsigned int class;
    int sign;
    int normal_exp;
    int pad;
    union {
        unsigned long long ll;
        unsigned int l[2];
    } fraction;
} fp_number_type_d;

typedef struct {
    unsigned int class;
    int sign;
    int normal_exp;
    unsigned int fraction;
} fp_number_type;

/* the matched members (the EE's `long` is 64 bits) */
long long __pack_d(void *s);
void __unpack_d(void *in, void *out);
fp_number_type_d *_fpadd_parts(fp_number_type_d *a, fp_number_type_d *b, fp_number_type_d *tmp);
long long dpsub(long long arg_a, long long arg_b);
long long dpmul(long long arg_a, long long arg_b);
int dpcmp(long long arg_a, long long arg_b);
long long litodp(int arg_a);
int dptoli(long long arg_a);
unsigned int dptoul(long long arg_a);
float __pack_f(void *s);
void __unpack_f(void *in, void *out);
unsigned int fptoui(float arg_a);
float __floatdisf(long long u);

static uint32_t float_bits(float f)
{
    union {
        float f;
        uint32_t u;
    } v;

    v.f = f;
    return v.u;
}

static float bits_float(uint32_t u)
{
    union {
        float f;
        uint32_t u;
    } v;

    v.u = u;
    return v.f;
}

uint64_t ee_soft_dpadd(uint64_t a, uint64_t b)
{
    fp_number_type_d x, y, z;

    __unpack_d(&a, &x);
    __unpack_d(&b, &y);
    return (uint64_t)__pack_d(_fpadd_parts(&x, &y, &z));
}

uint64_t ee_soft_dpsub(uint64_t a, uint64_t b)
{
    return (uint64_t)dpsub((long long)a, (long long)b);
}

uint64_t ee_soft_dpmul(uint64_t a, uint64_t b)
{
    return (uint64_t)dpmul((long long)a, (long long)b);
}

/* fp-bit.c's nan() as dp_bit_k.h has it: a zeroed static, class SNAN */
static fp_number_type_d *nan_d(void)
{
    static fp_number_type_d thenan;

    return &thenan;
}

/* dp_bit.c's dpdiv */
uint64_t ee_soft_dpdiv(uint64_t arg_a, uint64_t arg_b)
{
    fp_number_type_d x, y, *p;

    void *r;
    unsigned long long m1, m2, bit, q;

    __unpack_d(&arg_a, &x);
    __unpack_d(&arg_b, &y);
    p = &x;
    if (x.class >= 2)
        goto op2check;
    r = &x;
    goto pack;
op2check:
    if (y.class < 2) {
        r = &y;
        goto pack;
    }
    x.sign = x.sign ^ y.sign;
    if ((x.class ^ 4) == 0)
        goto chk_same;
    if ((x.class ^ 2) != 0)
        goto op1_normal;
chk_same:
    r = &x;
    if (x.class != y.class)
        goto pack;
    r = nan_d();
    goto pack;
op1_normal:
    if ((y.class ^ 4) != 0)
        goto chk_zero;
    x.fraction.ll = 0;
    x.normal_exp = 0;
    r = &x;
    goto pack;
chk_zero:
    if ((y.class ^ 2) != 0)
        goto divide;
    x.class = 4;
    r = &x;
    goto pack;
divide:
    m1 = x.fraction.ll;
    m2 = y.fraction.ll;
    x.normal_exp = x.normal_exp - y.normal_exp;
    if (m1 < m2) {
        x.normal_exp = x.normal_exp - 1;
        m1 <<= 1;
    }
    q = 0;
    bit = 0x1000000000000000ULL;
    do {
        if (m1 >= m2) {
            q |= bit;
            m1 -= m2;
        }
        bit >>= 1;
        m1 <<= 1;
    } while (bit != 0);
    if ((q & 0xFF) != 0x80) {
        p->fraction.ll = q;
    } else {
        if (q & 0x100) {
            q += 0x80;
        } else {
            q = (m1 != 0) ? (q + 0x80) : q;
        }
        p->fraction.ll = q;
    }
    r = p;
pack:
    return (uint64_t)__pack_d(r);
}

int ee_soft_dpcmp(uint64_t a, uint64_t b)
{
    return dpcmp((long long)a, (long long)b);
}

uint64_t ee_soft_litodp(int32_t i)
{
    return (uint64_t)litodp(i);
}

/* ee-gcc's unsigned int to double: litodp, plus 2^32 when the value is a
   negative int (as __floatdisf's own `(USItype)` conversion is compiled) */
uint64_t ee_soft_litodp_u(uint32_t u)
{
    uint64_t r = (uint64_t)litodp((int)u);

    if ((int32_t)u < 0)
        r = ee_soft_dpadd(r, 0x41F0000000000000ULL);
    return r;
}

int32_t ee_soft_dptoli(uint64_t a)
{
    return dptoli((long long)a);
}

uint32_t ee_soft_dptoul(uint64_t a)
{
    return dptoul((long long)a);
}

/* fp_bit.c's fptodp and dp_bit.c's __make_dp */
uint64_t ee_soft_fptodp(uint32_t f)
{
    fp_number_type in = {0, 0, 0, 0};
    fp_number_type_d s;

    __unpack_f(&f, &in);
    s.class = in.class;
    s.sign = in.sign;
    s.normal_exp = in.normal_exp;
    s.pad = 0;
    s.fraction.ll = ((unsigned long long)in.fraction << 32) >> 2;
    return (uint64_t)__pack_d(&s);
}

/* dp_bit.c's dptofp and fp_bit.c's __make_fp */
uint32_t ee_soft_dptofp(uint64_t d)
{
    fp_number_type_d buf;
    fp_number_type s;
    unsigned long long m;
    int hi, t;

    buf.fraction.ll = 0;
    buf.normal_exp = 0;
    __unpack_d(&d, &buf);
    m = buf.fraction.ll;
    hi = (int)(m >> 30);
    t = hi | 1;
    if ((m & 0x3FFFFFFF) == 0)
        t = hi;
    s.class = buf.class;
    s.sign = buf.sign;
    s.normal_exp = buf.normal_exp;
    s.fraction = (unsigned int)t;
    return float_bits(__pack_f(&s));
}

uint32_t ee_soft_fptoui(uint32_t f)
{
    return fptoui(bits_float(f));
}

uint32_t ee_soft_floatdisf(int64_t i)
{
    return float_bits(__floatdisf(i));
}

/* ee-gcc's unsigned long (64-bit) to float: __floatdisf when the value is
   non-negative as a long, otherwise __floatdisf of (u >> 1 | u & 1) doubled
   with add.s */
uint32_t ee_soft_floatdisf_u(uint64_t u)
{
    uint32_t t;

    if (!(u >> 63))
        return float_bits(__floatdisf((long long)u));
    t = float_bits(__floatdisf((long long)((u >> 1) | (u & 1))));
    return ee_fpu_add(t, t);
}
