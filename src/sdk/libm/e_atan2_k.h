/* e_atan2.c's constants, kept in a header so each capsule of the build compiles them
   (gcc folds a static const it can see; an extern would be loaded). */
static const double tiny  = 1.0e-300;
static const double zero  = 0.0;
static const double pi_o_4  = 7.8539816339744827900E-01;
static const double pi_o_2  = 1.5707963267948965580E+00;
static const double pi      = 3.1415926535897931160E+00;
static const double pi_lo   = 1.2246467991473531772E-16;
