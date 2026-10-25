/* s_scalbn.c's constants, kept in a header so each capsule of the build compiles them
   (gcc folds a static const it can see; an extern would be loaded). */
static const double two54   =  1.80143985094819840000e+16;
static const double twom54  =  5.55111512312578270212e-17;
static const double huge   = 1.0e+300;
static const double tiny   = 1.0e-300;
