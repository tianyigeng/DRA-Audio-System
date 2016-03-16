/*
 *  General MDCT Implementation
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __MDCT_H_
#define __MDCT_H_

int MDCT(const int M, 
         const double* src, 
         double* dst);

int iMDCT(const int M, 
          const double* src, 
          double* dst);

#endif
