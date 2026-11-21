/* -DGGX_FPU_IEEE=ON: the EE-FPU Model's interface, computed with the host's
 * IEEE floats instead of the EE's rules. A developer check, not a port: with
 * these, the routed Core must compute exactly what it computed before PORT-003
 * routed it (platforms/README.md, "The EE-FPU Model"), which shows that
 * tools/fpu_lower.py routes every operation to the right function. */
#include "ee_fpu.h"
typedef union { float f; uint32_t u; } fu;
static float F(uint32_t u) { fu v; v.u = u; return v.f; }
static uint32_t U(float f) { fu v; v.f = f; return v.u; }
uint32_t ee_fpu_add(uint32_t s, uint32_t t) { return U(F(s) + F(t)); }
uint32_t ee_fpu_sub(uint32_t s, uint32_t t) { return U(F(s) - F(t)); }
uint32_t ee_fpu_mul(uint32_t s, uint32_t t) { return U(F(s) * F(t)); }
uint32_t ee_fpu_div(uint32_t s, uint32_t t) { return U(F(s) / F(t)); }
uint32_t ee_fpu_neg(uint32_t s) { return U(-F(s)); }
int ee_fpu_c_eq(uint32_t s, uint32_t t) { return F(s) == F(t); }
int ee_fpu_c_lt(uint32_t s, uint32_t t) { return F(s) < F(t); }
int ee_fpu_c_le(uint32_t s, uint32_t t) { return F(s) <= F(t); }
uint32_t ee_fpu_cvt_s_w(int32_t w) { return U((float)w); }
uint32_t ee_fpu_cvt_s_wu(uint32_t u) { return U((float)u); }
int32_t ee_fpu_cvt_w_s(uint32_t s) { return (int32_t)F(s); }
