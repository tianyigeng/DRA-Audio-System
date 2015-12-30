/*
 *  General MDCT Implementation
 *  Copyright (C) 2015 Tianyi Geng
 *  All Rights Reserved
 */

#ifndef GENERAL_MDCT_C
#define GENERAL_MDCT_C

static const double M_PI = 3.1415926;

#include <math.h>
#include "GeneralMDCT.h"
#include <stdio.h>
#include <stdlib.h>

// Window function
static double w(const int M, const int n) {
    // return sin(M_PI * (n + 0.5) / (2.0 * M));
    return 1.0;
}

// M denotes the length of output signal, len(src) should be 2 * M
int MDCT(const int M, const int* x, double* X) {
    for (int k = 0; k < M; k++) {
        X[k] = 0.0;
        for (int n = 0; n < 2 * M; n++) {
            // printf("x: %d, cos: %.2f\n", 
            //                  x[n], 
            //                  cos(M_PI * (n + .5 + .5 * M) * (k + .5) / M)
            //                  );
            X[k] += x[n] * w(M, n) * cos(M_PI * (n + .5 + .5 * M) * (k + .5) / M);
        }
        // printf("\n");
        // X[k] *= sqrt(2.0 / M);
    }
    return 0;
}

int iMDCT(const int M, const double* X, double* x) {
    double* temp_dst = malloc(M * sizeof(double));
    for (int n = 0; n < 2 * M; n++) {
        temp_dst[n] = 0.0;
        for (int k = 0; k < M; k++) {
            temp_dst[n] += X[k] * cos(M_PI * (n + .5 * (1. + M)) * (k + .5)/ M);
            // printf("X: %.2f, cos: %.2f\n", 
            //                     X[k], 
            //                     cos(M_PI * (n + .5 * (1. + M)) * (k + .5)/ M));
        }
        // temp_dst[n] *= sqrt(2.0) * w(M, n);
        temp_dst[n] /= M;
        // temp_dst[n] *= w(M, n) * sqrt(2.0);
        // printf("n: %d, temp_dst[n]: %.2f\n", n, temp_dst[n]);
    }

    for (int n = 0; n < 2 * M; n++) {
        x[n] = temp_dst[n];
    }

    free(temp_dst);
    return 0;
}

#endif