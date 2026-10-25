/* newlib 1.8.2 libm/math/e_rem_pio2.c (https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz). Changes for this build: the __STDC__ side of its K&R conditionals kept, multi-variable constant declarations split one per line, include spelling. */

/* @(#)e_rem_pio2.c 5.1 93/09/24 */
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

/* __ieee754_rem_pio2(x,y)
 * 
 * return the remainder of x rem pi/2 in y[0]+y[1] 
 * use __kernel_rem_pio2()
 */

#include "fdlibm.h"

#ifndef _DOUBLE_IS_32BITS

#include "e_rem_pio2_k.h"

 /* 0x397B839A, 0x252049C1 */

	__int32_t __ieee754_rem_pio2(double x, double *y)
{
	double z,w,t,r,fn;
	double tx[3];
	__int32_t i,j,n,ix,hx;
	int e0,nx;
	__uint32_t low;

	GET_HIGH_WORD(hx,x);		/* high word of x */
	ix = hx&0x7fffffff;
	if(ix<=0x3fe921fb)   /* |x| ~<= pi/4 , no need for reduction */
	    {y[0] = x; y[1] = 0; return 0;}
	if(ix<0x4002d97c) {  /* |x| < 3pi/4, special case with n=+-1 */
	    if(hx>0) { 
		z = x - pio2_1;
		if(ix!=0x3ff921fb) { 	/* 33+53 bit pi is good enough */
		    y[0] = z - pio2_1t;
		    y[1] = (z-y[0])-pio2_1t;
		} else {		/* near pi/2, use 33+33+53 bit pi */
		    z -= pio2_2;
		    y[0] = z - pio2_2t;
		    y[1] = (z-y[0])-pio2_2t;
		}
		return 1;
	    } else {	/* negative x */
		z = x + pio2_1;
		if(ix!=0x3ff921fb) { 	/* 33+53 bit pi is good enough */
		    y[0] = z + pio2_1t;
		    y[1] = (z-y[0])+pio2_1t;
		} else {		/* near pi/2, use 33+33+53 bit pi */
		    z += pio2_2;
		    y[0] = z + pio2_2t;
		    y[1] = (z-y[0])+pio2_2t;
		}
		return -1;
	    }
	}
	if(ix<=0x413921fb) { /* |x| ~<= 2^19*(pi/2), medium size */
	    t  = fabs(x);
	    n  = (__int32_t) (t*invpio2+half);
	    fn = (double)n;
	    r  = t-fn*pio2_1;
	    w  = fn*pio2_1t;	/* 1st round good to 85 bit */
	    if(n<32&&ix!=npio2_hw[n-1]) {	
		y[0] = r-w;	/* quick check no cancellation */
	    } else {
	        __uint32_t high;
	        j  = ix>>20;
	        y[0] = r-w; 
		GET_HIGH_WORD(high,y[0]);
	        i = j-((high>>20)&0x7ff);
	        if(i>16) {  /* 2nd iteration needed, good to 118 */
		    t  = r;
		    w  = fn*pio2_2;	
		    r  = t-w;
		    w  = fn*pio2_2t-((t-r)-w);	
		    y[0] = r-w;
		    GET_HIGH_WORD(high,y[0]);
		    i = j-((high>>20)&0x7ff);
		    if(i>49)  {	/* 3rd iteration need, 151 bits acc */
		    	t  = r;	/* will cover all possible cases */
		    	w  = fn*pio2_3;	
		    	r  = t-w;
		    	w  = fn*pio2_3t-((t-r)-w);	
		    	y[0] = r-w;
		    }
		}
	    }
	    y[1] = (r-y[0])-w;
	    if(hx<0) 	{y[0] = -y[0]; y[1] = -y[1]; return -n;}
	    else	 return n;
	}
    /* 
     * all other (large) arguments
     */
	if(ix>=0x7ff00000) {		/* x is inf or NaN */
	    y[0]=y[1]=x-x; return 0;
	}
    /* set z = scalbn(|x|,ilogb(x)-23) */
	GET_LOW_WORD(low,x);
	SET_LOW_WORD(z,low);
	e0 	= (int)((ix>>20)-1046);	/* e0 = ilogb(z)-23; */
	SET_HIGH_WORD(z, ix - ((__int32_t)e0<<20));
	for(i=0;i<2;i++) {
		tx[i] = (double)((__int32_t)(z));
		z     = (z-tx[i])*two24;
	}
	tx[2] = z;
	nx = 3;
	while(tx[nx-1]==zero) nx--;	/* skip zero term */
	n  =  __kernel_rem_pio2(tx,y,e0,nx,2,two_over_pi);
	if(hx<0) {y[0] = -y[0]; y[1] = -y[1]; return -n;}
	return n;
}

#endif /* defined(_DOUBLE_IS_32BITS) */
