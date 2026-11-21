/* The EE's software floating point (ee_softfp.c): what ee-gcc 2.9 calls for
 * double arithmetic and for conversions the FPU has no instruction for. A
 * double is its uint64_t bit pattern, a float its uint32_t one. Core only:
 * it calls the decomp's matched fp-bit. */
#ifndef GGX_EE_SOFTFP_H
#define GGX_EE_SOFTFP_H

#include <stdint.h>

uint64_t ee_soft_dpadd(uint64_t a, uint64_t b);
uint64_t ee_soft_dpsub(uint64_t a, uint64_t b);
uint64_t ee_soft_dpmul(uint64_t a, uint64_t b);
uint64_t ee_soft_dpdiv(uint64_t a, uint64_t b);
/* -1, 0 or 1; 1 when either is a NaN. Each C comparison tests it against 0. */
int ee_soft_dpcmp(uint64_t a, uint64_t b);

uint64_t ee_soft_litodp(int32_t i);
uint64_t ee_soft_litodp_u(uint32_t u);
int32_t ee_soft_dptoli(uint64_t a);
uint32_t ee_soft_dptoul(uint64_t a);
uint64_t ee_soft_fptodp(uint32_t f);
uint32_t ee_soft_dptofp(uint64_t d);

uint32_t ee_soft_fptoui(uint32_t f);
uint32_t ee_soft_floatdisf(int64_t i);
uint32_t ee_soft_floatdisf_u(uint64_t u);

#endif
