/* softfp_test: the EE's software floating point (common/fpu/ee_softfp.c over
 * the decomp's matched fp-bit), compiled natively.
 *
 * fp-bit rounds to nearest like IEEE 754, so wherever the operands and the
 * result are normal numbers it must give the host's IEEE double result, bit
 * for bit; that checks ee_softfp.c's wrappers and its copy of dpdiv. Where
 * they are not, fp-bit (built with NO_DENORMALS) differs from IEEE, and the
 * EE's behaviour is checked case by case. Exit status 1 on any difference. */
#include <stdio.h>
#include <string.h>

#include "ee_fpu.h"
#include "ee_softfp.h"

static unsigned long failures;

static uint64_t rng = 0x243F6A8885A308D3ull;

static uint64_t rnd(void)
{
    rng ^= rng >> 12;
    rng ^= rng << 25;
    rng ^= rng >> 27;
    return rng * 0x2545F4914F6CDD1Dull;
}

static uint64_t dbits(double d)
{
    uint64_t u;

    memcpy(&u, &d, 8);
    return u;
}

static double dval(uint64_t u)
{
    double d;

    memcpy(&d, &u, 8);
    return d;
}

static uint32_t fbits(float f)
{
    uint32_t u;

    memcpy(&u, &f, 4);
    return u;
}

static float fval(uint32_t u)
{
    float f;

    memcpy(&f, &u, 4);
    return f;
}

/* a random double with |exponent| below 300, so products and quotients of
   two stay normal */
static uint64_t rnd_double(void)
{
    uint64_t r = rnd();
    uint64_t e = 1023 - 300 + rnd() % 600;

    return (r & 0x800FFFFFFFFFFFFFull) | (e << 52);
}

static int normal(uint64_t d)
{
    uint64_t e = (d >> 52) & 0x7FF;

    return e != 0 && e != 0x7FF;
}

static void expect(const char *what, uint64_t got, uint64_t want, uint64_t a, uint64_t b)
{
    if (got == want)
        return;
    if (failures++ < 20)
        fprintf(stderr, "%s %016llx %016llx: fp-bit %016llx, expected %016llx\n", what,
                (unsigned long long)a, (unsigned long long)b, (unsigned long long)got,
                (unsigned long long)want);
}

int main(void)
{
    long i, n = 1000000;

    for (i = 0; i < n; i++) {
        uint64_t a = rnd_double(), b = rnd_double();
        double x = dval(a), y = dval(b);
        uint32_t f = (uint32_t)(rnd() >> 32);
        int32_t w = (int32_t)(rnd() >> 32) >> (rnd() % 32);
        int64_t l = (int64_t)rnd() >> (rnd() % 64);

        if (rnd() & 1)          /* sometimes close, for cancellation */
            b = (a & 0xFFF0000000000000ull) | (rnd() & 0x000FFFFFFFFFFFFFull) | ((rnd() & 1) << 63);
        y = dval(b);
        if (normal(dbits(x + y)))
            expect("dpadd", ee_soft_dpadd(a, b), dbits(x + y), a, b);
        if (normal(dbits(x - y)))
            expect("dpsub", ee_soft_dpsub(a, b), dbits(x - y), a, b);
        expect("dpmul", ee_soft_dpmul(a, b), dbits(x * y), a, b);
        expect("dpdiv", ee_soft_dpdiv(a, b), dbits(x / y), a, b);
        expect("dpcmp", (uint64_t)(int64_t)ee_soft_dpcmp(a, b), (uint64_t)(int64_t)((x > y) - (x < y)), a, b);
        if (((f >> 23) & 0xFF) != 0 && ((f >> 23) & 0xFF) != 0xFF)
            expect("fptodp", ee_soft_fptodp(f), dbits((double)fval(f)), f, 0);
        if (normal(a) && ((fbits((float)x) >> 23) & 0xFF) != 0 && ((fbits((float)x) >> 23) & 0xFF) != 0xFF)
            expect("dptofp", ee_soft_dptofp(a), fbits((float)x), a, 0);
        expect("litodp", ee_soft_litodp(w), dbits((double)w), (uint64_t)w, 0);
        expect("litodp_u", ee_soft_litodp_u((uint32_t)w), dbits((double)(uint32_t)w), (uint32_t)w, 0);
        {
            /* w plus under a half toward zero, so the truncation is w */
            double t = (double)w + dval(rnd_double() & 0x3FDFFFFFFFFFFFFFull) * (w < 0 ? -1 : 1) * (w != 0);
            expect("dptoli", (uint64_t)(int64_t)ee_soft_dptoli(dbits(t)), (uint64_t)(int64_t)w, dbits(t), 0);
            if (w >= 0)
                expect("dptoul", ee_soft_dptoul(dbits(t)), (uint32_t)w, dbits(t), 0);
        }
        expect("floatdisf", ee_soft_floatdisf(l), fbits((float)l), (uint64_t)l, 0);
    }

    /* where NO_DENORMALS makes fp-bit differ from IEEE */
    expect("fptodp of a denormal float reads it as zero", ee_soft_fptodp(0x00000001), 0, 1, 0);
    expect("fptodp of a negative denormal", ee_soft_fptodp(0x807FFFFF), 0x8000000000000000ull, 0x807FFFFF, 0);
    expect("dpadd of a denormal double reads it as zero", ee_soft_dpadd(1, 0), 0, 1, 0);
    /* a result below the normal range truncates to a denormal: 1.5 * 2^-1023 */
    expect("dpmul into the denormal range truncates",
           ee_soft_dpmul(0x0010000000000000ull, 0x3FE8000000000000ull), 0x000C000000000000ull, 0, 0);
    /* (float)(2^-127 * 1.75) truncates to a denormal float, 0x00700000 */
    expect("dptofp into the denormal float range truncates", ee_soft_dptofp(0x380C000000000000ull),
           0x00700000, 0, 0);
    /* ee-gcc negates a double as 0.0 - x: -(+0.0) is +0.0 */
    expect("0.0 - 0.0", ee_soft_dpsub(0, 0), 0, 0, 0);
    /* unsigned long to float, through __floatdisf and add.s */
    expect("floatdisf_u of 2^64 - 1", ee_soft_floatdisf_u(~0ull), 0x5F800000, ~0ull, 0);
    expect("fptoui of 3e9", ee_soft_fptoui(0x4F32D05E), 3000000000u, 0x4F32D05E, 0);
    expect("fptoui of a negative value", ee_soft_fptoui(0xBF800000), 0, 0xBF800000, 0);

    printf("%ld random cases per routine and the fp-bit edge cases: %lu differ\n", n, failures);
    return failures != 0;
}
