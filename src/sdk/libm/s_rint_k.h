/* s_rint.c's constants, kept in a header so each capsule of the build compiles them
   (gcc folds a static const it can see; an extern would be loaded). */
static const double
TWO52[2]={
  4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
 -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};
