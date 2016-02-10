/*
 *  General MDCT Implementation
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef GENERAL_MDCT_C
#define GENERAL_MDCT_C

#include <math.h>
#include "GeneralMDCT.h"
#include <stdio.h>
#include <stdlib.h>

// Window function
static inline double _winFunc(const int M, const int n) {
    return sin(M_PI * (n + 0.5) / (2.0 * M));
}

// M denotes the length of output signal, len(src) should be 2 * M
int MDCT(const int M, const double* x, double* X) {
    for (int k = 0; k < M; k++) {
        X[k] = 0.0;
        for (int n = 0; n < 2 * M; n++) {
            X[k] += x[n] * _winFunc(M, n) * cos(M_PI * (n + .5 + .5 * M) * (k + .5) / M);
        }
        X[k] *= sqrt(2.0 / M);
    }

    return 0;
}

int iMDCT(const int M, const double* X, double* x) {
    for (int n = 0; n < 2 * M; n++) {
        x[n] = 0.0;
        for (int k = 0; k < M; k++) {
            x[n] += X[k] * cos(M_PI * (n + .5 * (1. + M)) * (k + .5)/ M);
        }
        x[n] *= _winFunc(M, n) * sqrt(2.0 / M);
    }

    return 0;
}

#endif
