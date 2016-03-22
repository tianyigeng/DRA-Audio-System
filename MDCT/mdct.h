/*
 *  General MDCT Implementation
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __MDCT_H_
#define __MDCT_H_

#include "../util/vector.h"

void MDCT(struct vector* in, 
            struct vector* out);

void iMDCT(struct vector* in, 
            struct vector* out);

#endif
