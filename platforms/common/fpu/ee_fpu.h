/* The EE-FPU Model (ADR-006, PORT-003): the PS2 EE's single-precision FPU
 * (COP1), computed in integer arithmetic so every host gets the same bits.
 *
 * Every value is a float's bit pattern. The rules are PCSX2's EE interpreter
 * (pcsx2/FPU.cpp) running under the EE's default host control register:
 *
 *   - operands with a zero exponent (zero or denormal) read as signed zero;
 *     operands with exponent 255 (an IEEE infinity or NaN) read as the largest
 *     finite value of their sign;
 *   - results round toward zero, so they never overflow to infinity: a result
 *     too large becomes the largest finite value, and one smaller than 2^-126
 *     becomes a signed zero;
 *   - x / 0 (or a denormal divisor) is the largest finite value with the sign
 *     of x ^ divisor, 0 / 0 included;
 *   - madd/msub are not fused: the product is rounded, then added;
 *   - nothing traps, and the FCR31 flags are not modelled (the game never
 *     reads them).
 *
 * tests/fpu/ checks the model against vectors computed by PCSX2's own
 * interpreter code.
 */
#ifndef GGX_EE_FPU_H
#define GGX_EE_FPU_H

#include <stdint.h>

/* arithmetic (add.s, sub.s, mul.s, div.s, sqrt.s, rsqrt.s) */
uint32_t ee_fpu_add(uint32_t s, uint32_t t);
uint32_t ee_fpu_sub(uint32_t s, uint32_t t);
uint32_t ee_fpu_mul(uint32_t s, uint32_t t);
uint32_t ee_fpu_div(uint32_t s, uint32_t t);
uint32_t ee_fpu_sqrt(uint32_t t);
uint32_t ee_fpu_rsqrt(uint32_t s, uint32_t t);

/* the accumulator forms: madd.s/msub.s give acc +/- s*t without changing
   ACC; madda.s/msuba.s return the new ACC. adda.s, suba.s and mula.s are
   ee_fpu_add, ee_fpu_sub and ee_fpu_mul written to ACC. */
uint32_t ee_fpu_madd(uint32_t acc, uint32_t s, uint32_t t);
uint32_t ee_fpu_msub(uint32_t acc, uint32_t s, uint32_t t);
uint32_t ee_fpu_madda(uint32_t acc, uint32_t s, uint32_t t);
uint32_t ee_fpu_msuba(uint32_t acc, uint32_t s, uint32_t t);

/* bit operations (abs.s, neg.s, max.s, min.s) */
uint32_t ee_fpu_abs(uint32_t s);
uint32_t ee_fpu_neg(uint32_t s);
uint32_t ee_fpu_max(uint32_t s, uint32_t t);
uint32_t ee_fpu_min(uint32_t s, uint32_t t);

/* compares (c.eq.s, c.lt.s, c.le.s): 1 when the condition holds. c.f.s is 0. */
int ee_fpu_c_eq(uint32_t s, uint32_t t);
int ee_fpu_c_lt(uint32_t s, uint32_t t);
int ee_fpu_c_le(uint32_t s, uint32_t t);

/* conversions: cvt.s.w (int to float, rounded toward zero) and cvt.w.s
   (float to int, truncated; out of range saturates by sign) */
uint32_t ee_fpu_cvt_s_w(int32_t w);
int32_t ee_fpu_cvt_w_s(uint32_t s);

/* EE-GCC 2.9's unsigned int to float: cvt.s.w when the value is a
   non-negative int, otherwise cvt.s.w of (u >> 1 | u & 1) doubled with add.s */
uint32_t ee_fpu_cvt_s_wu(uint32_t u);

#endif
