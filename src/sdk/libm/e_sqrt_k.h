/* e_sqrt.c's constants. __ieee754_sqrt never loads them (gcc folds
   one-tiny and friends), so they are spelled as the values: a static const
   the code does not reference would be data the build cannot place. */
#define one 1.0
#define tiny 1.0e-300
