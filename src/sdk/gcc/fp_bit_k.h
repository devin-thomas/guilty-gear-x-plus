/* fp_bit.c's static inline helpers, kept in a header so the build's
   per-function capsules inline them instead of emitting them. */
/* The single-float number in unpacked form.  FRAC_NBITS is 32, NGARDS is 7,
   so the implicit one sits at bit 30 and the overflow bit at bit 31.  The
   file static shares its name with the double build's global in dp-bit.o. */

static __inline__ int isnan(fp_number_type *x)
{
    return x->class == CLASS_SNAN || x->class == CLASS_QNAN;
}

static __inline__ int isinf(fp_number_type *x)
{
    return x->class == CLASS_INFINITY;
}

static __inline__ int iszero(fp_number_type *x)
{
    return x->class == CLASS_ZERO;
}

/* fp-bit.c's nan(): the NaN this build hands back for inf - inf; its static is the
   member's whole .bss, VMA 0x736188, 0x10 B. */
static __inline__ fp_number_type *nan(void)
{
    static fp_number_type thenan;

    return &thenan;
}

static __inline__ fp_number_type *_fpmul_parts(fp_number_type *a, fp_number_type *b,
                                               fp_number_type *tmp)
{
    unsigned long long prod;
    unsigned int high;
    unsigned int low;

    if (isnan(a)) {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isnan(b)) {
        b->sign = a->sign != b->sign;
        return b;
    }
    if (isinf(a)) {
        if (iszero(b))
            return nan();
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isinf(b)) {
        if (iszero(a))
            return nan();
        b->sign = a->sign != b->sign;
        return b;
    }
    if (iszero(a)) {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (iszero(b)) {
        b->sign = a->sign != b->sign;
        return b;
    }

    tmp->normal_exp = a->normal_exp + b->normal_exp + 2;
    tmp->sign = a->sign != b->sign;

    /* the product of the two fractions is a 64 bit number */
    prod = (unsigned long long)a->fraction * b->fraction;
    high = prod >> 32;
    low = prod;

    while (high >= IMPLICIT_2) {
        tmp->normal_exp++;
        if (high & 1) {
            low >>= 1;
            low |= IMPLICIT_2;
        }
        high >>= 1;
    }

    while (high < IMPLICIT_1) {
        high <<= 1;
        if (low & IMPLICIT_2)
            high |= 1;
        tmp->normal_exp--;
        low <<= 1;
    }

    if ((high & 0x7F) == 0x40) {
        if (high & 0x80) {
            /* exactly half way, so round to even */
            high += 0x40;
        } else if (low) {
            /* not really half way, there are more bits below */
            high += 0x40;
        }
    }

    tmp->fraction = high;
    tmp->class = CLASS_NUMBER;
    return tmp;
}

static __inline__ fp_number_type *_fpdiv_parts(fp_number_type *a, fp_number_type *b)
{
    unsigned int bit;
    unsigned int numerator;
    unsigned int denominator;
    unsigned int quotient;

    if (isnan(a))
        return a;
    if (isnan(b))
        return b;

    a->sign = a->sign ^ b->sign;

    if (isinf(a) || iszero(a)) {
        /* dividing two infinities, or two zeroes, yields a NaN */
        if (a->class == b->class)
            return nan();
        return a;
    }

    if (isinf(b)) {
        a->fraction = 0;
        a->normal_exp = 0;
        return a;
    }
    if (iszero(b)) {
        a->class = CLASS_INFINITY;
        return a;
    }

    a->normal_exp = a->normal_exp - b->normal_exp;
    numerator = a->fraction;
    denominator = b->fraction;

    if (numerator < denominator) {
        /* the quotient would be less than one */
        numerator *= 2;
        a->normal_exp--;
    }

    bit = IMPLICIT_1;
    quotient = 0;
    while (bit) {
        if (numerator >= denominator) {
            quotient |= bit;
            numerator -= denominator;
        }
        bit >>= 1;
        numerator *= 2;
    }

    if ((quotient & 0x7F) == 0x40) {
        if (quotient & 0x80) {
            /* exactly half way, so round to even */
            quotient += 0x40;
        } else if (numerator) {
            /* not really half way, there are more bits below */
            quotient += 0x40;
        }
    }

    a->fraction = quotient;
    return a;
}

