/*
 *  MDCT Implementation
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __MDCT_H_
#define __MDCT_H_

#include "../util/vector.h"

struct vector* MDCT(struct vector* in);

struct vector* iMDCT(struct vector* in);

#endif
