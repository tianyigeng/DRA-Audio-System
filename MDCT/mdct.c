/*
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __MDCT_C_
#define __MDCT_C_

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "mdct.h"
#include "../util/errors.h"

/*
 *  Forward declarations of helper functions.
 */
static inline double _win_func(const int M, const int n);
static void _MDCT(const int M, const double* x, double* X);
static void _iMDCT(const int M, const double* X, double* x);

/* 
 *   in  -- vector of double
 *   ret -- vector of double
 *   ret value is MDCT of in
 */
struct vector* MDCT(struct vector* in) {
    uint16_t len = in->size;

    double* src = (double*) malloc(sizeof(double) * len);
    double* dst = (double*) malloc(sizeof(double) * len / 2);
    if (src == NULL || dst == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    for (uint16_t i = 0; i < len; i++) {
        src[i] = vector_double_at(in, i);
    }

    _MDCT(len / 2, src, dst);

    struct vector* ret = vector_init();
    for (uint16_t i = 0; i < len / 2; i++) {
        vector_push_back_double(ret, dst[i]);
    }

    free(src);
    free(dst);

    return ret;
}

/* 
 *   in  -- vector of double
 *   ret -- vector of double
 *   ret value is inverse MDCT of in
 */
struct vector* iMDCT(struct vector* in) {
    uint16_t len = in->size;

    double* src = (double*) malloc(sizeof(double) * len);
    double* dst = (double*) malloc(sizeof(double) * len * 2);
    if (src == NULL || dst == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    for (uint16_t i = 0; i < len; i++) {
        src[i] = vector_double_at(in, i);
    }

    _iMDCT(len, src, dst);

    struct vector* ret = vector_init();
    for (uint16_t i = 0; i < len * 2; i++) {
        vector_push_back_double(ret, dst[i]);
    }

    free(src);
    free(dst);
    
    return ret;
}

/* Window function */
static inline double _win_func(const int M, const int n) {
    return sin(M_PI * (n + 0.5) / (2.0 * M));
}

/* 
 *  M denotes the length of output signal, len(src) should be 2 * M 
 */
static void _MDCT(const int M, const double* x, double* X) {
    for (int k = 0; k < M; k++) {
        X[k] = 0.0;
        for (int n = 0; n < 2 * M; n++) {
            X[k] += x[n] * _win_func(M, n) * cos(M_PI * (n + .5 + .5 * M) * (k + .5) / M);
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
        x[n] *= _win_func(M, n) * sqrt(2.0 / M);
    }
}

#endif
