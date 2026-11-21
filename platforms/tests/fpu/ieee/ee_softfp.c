/* -DGGX_FPU_IEEE=ON: ee_softfp.h with the host's IEEE doubles (see ee_fpu.c).
 * fpu_lower.py spells -x as 0.0 - x, as ee-gcc does; here that stays -x. */
#include "ee_softfp.h"
typedef union { double d; uint64_t u; } du;
typedef union { float f; uint32_t u; } fu;
static double D(uint64_t u) { du v; v.u = u; return v.d; }
static uint64_t UD(double d) { du v; v.d = d; return v.u; }
static float F(uint32_t u) { fu v; v.u = u; return v.f; }
static uint32_t U(float f) { fu v; v.f = f; return v.u; }
uint64_t ee_soft_dpadd(uint64_t a, uint64_t b) { return UD(D(a) + D(b)); }
uint64_t ee_soft_dpsub(uint64_t a, uint64_t b) { return a == 0 ? UD(-D(b)) : UD(D(a) - D(b)); }
uint64_t ee_soft_dpmul(uint64_t a, uint64_t b) { return UD(D(a) * D(b)); }
uint64_t ee_soft_dpdiv(uint64_t a, uint64_t b) { return UD(D(a) / D(b)); }
int ee_soft_dpcmp(uint64_t a, uint64_t b) { return (D(a) > D(b)) - (D(a) < D(b)); }
uint64_t ee_soft_litodp(int32_t i) { return UD((double)i); }
uint64_t ee_soft_litodp_u(uint32_t u) { return UD((double)u); }
int32_t ee_soft_dptoli(uint64_t a) { return (int32_t)D(a); }
uint32_t ee_soft_dptoul(uint64_t a) { return (uint32_t)D(a); }
uint64_t ee_soft_fptodp(uint32_t f) { return UD((double)F(f)); }
uint32_t ee_soft_dptofp(uint64_t d) { return U((float)D(d)); }
uint32_t ee_soft_fptoui(uint32_t f) { return (uint32_t)F(f); }
uint32_t ee_soft_floatdisf(int64_t i) { return U((float)i); }
uint32_t ee_soft_floatdisf_u(uint64_t u) { return U((float)u); }
