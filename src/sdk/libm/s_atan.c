/* newlib 1.8.2 libm/math/s_atan.c (https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz). Changes for this build: the __STDC__ side of its K&R conditionals kept, multi-variable constant declarations split one per line, include spelling. */

/* @(#)s_atan.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/*
FUNCTION
        <<atan>>, <<atanf>>---arc tangent

INDEX
   atan
INDEX
   atanf

ANSI_SYNOPSIS
        #include <math.h>
        double atan(double <[x]>);
        float atanf(float <[x]>);

TRAD_SYNOPSIS
        #include <math.h>
        double atan(<[x]>);
        double <[x]>;

        float atanf(<[x]>);
        float <[x]>;

DESCRIPTION

<<atan>> computes the inverse tangent (arc tangent) of the input value.

<<atanf>> is identical to <<atan>>, save that it operates on <<floats>>.

RETURNS
@ifinfo
<<atan>> returns a value in radians, in the range of -pi/2 to pi/2.
@end ifinfo
@tex
<<atan>> returns a value in radians, in the range of $-\pi/2$ to $\pi/2$.
@end tex

PORTABILITY
<<atan>> is ANSI C.  <<atanf>> is an extension.

*/

/* atan(x)
 * Method
 *   1. Reduce x to positive by atan(x) = -atan(-x).
 *   2. According to the integer k=4t+0.25 chopped, t=x, the argument
 *      is further reduced to one of the following intervals and the
 *      arctangent of t is evaluated by the corresponding formula:
 *
 *      [0,7/16]      atan(x) = t-t^3*(a1+t^2*(a2+...(a10+t^2*a11)...)
 *      [7/16,11/16]  atan(x) = atan(1/2) + atan( (t-0.5)/(1+t/2) )
 *      [11/16.19/16] atan(x) = atan( 1 ) + atan( (t-1)/(1+t) )
 *      [19/16,39/16] atan(x) = atan(3/2) + atan( (t-1.5)/(1+1.5t) )
 *      [39/16,INF]   atan(x) = atan(INF) + atan( -1/t )
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 */

#include "fdlibm.h"

#ifndef _DOUBLE_IS_32BITS

#include "s_atan_k.h"



	double atan(double x)
{
	double w,s1,s2,z;
	__int32_t ix,hx,id;

	GET_HIGH_WORD(hx,x);
	ix = hx&0x7fffffff;
	if(ix>=0x44100000) {	/* if |x| >= 2^66 */
	    __uint32_t low;
	    GET_LOW_WORD(low,x);
	    if(ix>0x7ff00000||
		(ix==0x7ff00000&&(low!=0)))
		return x+x;		/* NaN */
	    if(hx>0) return  atanhi[3]+atanlo[3];
	    else     return -atanhi[3]-atanlo[3];
	} if (ix < 0x3fdc0000) {	/* |x| < 0.4375 */
	    if (ix < 0x3e200000) {	/* |x| < 2^-29 */
		if(huge+x>one) return x;	/* raise inexact */
	    }
	    id = -1;
	} else {
	x = fabs(x);
	if (ix < 0x3ff30000) {		/* |x| < 1.1875 */
	    if (ix < 0x3fe60000) {	/* 7/16 <=|x|<11/16 */
		id = 0; x = (2.0*x-one)/(2.0+x); 
	    } else {			/* 11/16<=|x|< 19/16 */
		id = 1; x  = (x-one)/(x+one); 
	    }
	} else {
	    if (ix < 0x40038000) {	/* |x| < 2.4375 */
		id = 2; x  = (x-1.5)/(one+1.5*x);
	    } else {			/* 2.4375 <= |x| < 2^66 */
		id = 3; x  = -1.0/x;
	    }
	}}
    /* end of argument reduction */
	z = x*x;
	w = z*z;
    /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
	s1 = z*(aT[0]+w*(aT[2]+w*(aT[4]+w*(aT[6]+w*(aT[8]+w*aT[10])))));
	s2 = w*(aT[1]+w*(aT[3]+w*(aT[5]+w*(aT[7]+w*aT[9]))));
	if (id<0) return x - x*(s1+s2);
	else {
	    z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
	    return (hx<0)? -z:z;
	}
}

#endif /* _DOUBLE_IS_32BITS */
