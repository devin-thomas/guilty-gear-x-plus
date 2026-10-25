/* k_rem_pio2.c's constants, kept in a header so each capsule of the build compiles them
   (gcc folds a static const it can see; an extern would be loaded). */
static const int init_jk[] = {2,3,4,6};
 /* initial value for jk */

static const double PIo2[] = {
  1.57079625129699707031e+00, /* 0x3FF921FB, 0x40000000 */
  7.54978941586159635335e-08, /* 0x3E74442D, 0x00000000 */
  5.39030252995776476554e-15, /* 0x3CF84698, 0x80000000 */
  3.28200341580791294123e-22, /* 0x3B78CC51, 0x60000000 */
  1.27065575308067607349e-29, /* 0x39F01B83, 0x80000000 */
  1.22933308981111328932e-36, /* 0x387A2520, 0x40000000 */
  2.73370053816464559624e-44, /* 0x36E38222, 0x80000000 */
  2.16741683877804819444e-51, /* 0x3569F31D, 0x00000000 */
};
static const double zero   = 0.0;
static const double one    = 1.0;
static const double two24   =  1.67772160000000000000e+07;
static const double twon24  =  5.96046447753906250000e-08;
