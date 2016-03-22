/*
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __MDCT_C_
#define __MDCT_C_

#include <math.h>
#include <stdint.h>
#include "mdct.h"

/*
 *  Forward declarations of helper functions.
 */
static inline double _winFunc(const int M, const int n);
static void _MDCT(const int M, const double* x, double* X);
static void _iMDCT(const int M, const double* X, double* x);


void MDCT(struct vector* in, struct vector* out) {

}

void iMDCT(struct vector* in, struct vector* out) {
    
}

/* Window function */
static inline double _winFunc(const int M, const int n) {
    return sin(M_PI * (n + 0.5) / (2.0 * M));
}

/* 
 *  M denotes the length of output signal, len(src) should be 2 * M 
 */
static void _MDCT(const int M, const double* x, double* X) {
    for (int k = 0; k < M; k++) {
        X[k] = 0.0;
        for (int n = 0; n < 2 * M; n++) {
            X[k] += x[n] * _winFunc(M, n) * cos(M_PI * (n + .5 + .5 * M) * (k + .5) / M);
        }
        X[k] *= sqrt(2.0 / M);
    }
}

static void _iMDCT(const int M, const double* X, double* x) {
    for (int n = 0; n < 2 * M; n++) {
        x[n] = 0.0;
        for (int k = 0; k < M; k++) {
            x[n] += X[k] * cos(M_PI * (n + .5 * (1. + M)) * (k + .5)/ M);
        }
        x[n] *= _winFunc(M, n) * sqrt(2.0 / M);
    }
}

#endif
