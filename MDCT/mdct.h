#ifndef __MDCT_H_
#define __MDCT_H_

#include "../util/vector.h"

/*
 *  Wrapper of fast_mdct based on fft, use fastmdct/mdct_fft.h 
 *  to utilize computational cost. 
 */

struct vector* MDCT(struct vector* in);
struct vector* iMDCT(struct vector* in);

#endif
