/* newlib 1.8.2 libm/math/k_sin.c (https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz). Changes for this build: the __STDC__ side of its K&R conditionals kept, multi-variable constant declarations split one per line, include spelling. */

/* @(#)k_sin.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* __kernel_sin( x, y, iy)
 * kernel sin function on [-pi/4, pi/4], pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input iy indicates whether y is 0. (if iy=0, y assume to be 0). 
 *
 * Algorithm
 *	1. Since sin(-x) = -sin(x), we need only to consider positive x. 
 *	2. if x < 2^-27 (hx<0x3e400000 0), return x with inexact if x!=0.
 *	3. sin(x) is approximated by a polynomial of degree 13 on
 *	   [0,pi/4]
 *		  	         3            13
 *	   	sin(x) ~ x + S1*x + ... + S6*x
 *	   where
 *	
 * 	|sin(x)         2     4     6     8     10     12  |     -58
 * 	|----- - (1+S1*x +S2*x +S3*x +S4*x +S5*x  +S6*x   )| <= 2
 * 	|  x 					           | 
 * 
 *	4. sin(x+y) = sin(x) + sin'(x')*y
 *		    ~ sin(x) + (1-x*x/2)*y
 *	   For better accuracy, let 
 *		     3      2      2      2      2
 *		r = x *(S2+x *(S3+x *(S4+x *(S5+x *S6))))
 *	   then                   3    2
 *		sin(x) = x + (S1*x + (x *(r-y/2)+y))
 */

#include "fdlibm.h"

#ifndef _DOUBLE_IS_32BITS

#include "k_sin_k.h"

 /* 0x3DE5D93A, 0x5ACFD57C */

	double __kernel_sin(double x, double y, int iy)
{
	double z,r,v;
	__int32_t ix;
	GET_HIGH_WORD(ix,x);
	ix &= 0x7fffffff;			/* high word of x */
	if(ix<0x3e400000)			/* |x| < 2**-27 */
	   {if((int)x==0) return x;}		/* generate inexact */
	z	=  x*x;
	v	=  z*x;
	r	=  S2+z*(S3+z*(S4+z*(S5+z*S6)));
	if(iy==0) return x+v*(S1+z*r);
	else      return x-((z*(half*y-v*r)-y)-v*S1);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
