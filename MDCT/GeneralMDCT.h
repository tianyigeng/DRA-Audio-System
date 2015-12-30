/*
 *  General MDCT Implementation
 *  Copyright (C) 2015 Tianyi Geng
 *  All Rights Reserved
 */

#ifndef GENERAL_MDCT_H
#define GENERAL_MDCT_H

int MDCT(const int M, 
		 const int* src, 
		 double* dst);

int iMDCT(const int M, 
		  const double* src, 
		  double* dst);

#endif
