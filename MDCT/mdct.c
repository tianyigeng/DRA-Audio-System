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
#include "fastmdct/mdct_fft.h"

/*
 *  Forward declarations of helper functions.
 */
static struct vector* _MDCT_frame(struct mdct_plan* m_plan, struct vector* in);
static struct vector* _iMDCT_frame(struct mdct_plan* m_plan, struct vector* in);

const uint32_t frame_size = 2048;

/* 
 *   in  -- 1-d vector of double
 *   ret -- 2-d vector of double
 *   ret value is MDCT of in
 */
struct vector* MDCT(struct vector* in) {
    struct vector* ret = vector_init(); /* 2-d vector */

    struct mdct_plan* m_plan = mdct_init(frame_size / 2);

    struct vector* helper = vector_init(); /* a helper vector to make logic clear */
    for (uint32_t i = 0; i < frame_size / 2; i++) {
        vector_push_back_double(helper, 0.0);
    }
    for (uint32_t i = 0; i < in->size; i++) {
        vector_push_back_double(helper, vector_double_at(in, i));
    }
    for (uint32_t i = 0; i < frame_size / 2; i++) {
        vector_push_back_double(helper, 0.0);
    }

    for (uint32_t i = 0; frame_size / 2 * (i + 1) < helper->size; i++) {
        struct vector* frame = vector_sub(helper, frame_size / 2 * i, frame_size);
        vector_push_back_object(ret, _MDCT_frame(m_plan, frame));
        vector_destroy(frame, NULL);
    }

    mdct_free(m_plan);
    vector_destroy(helper, NULL);

    return ret;
}

/* 
 *   in  -- 2-d vector of double
 *   ret -- 1-d vector of double
 *   ret value is iMDCT of in
 */
struct vector* iMDCT(struct vector* in) {
    struct vector* ret = vector_init();

    struct mdct_plan* m_plan = mdct_init(frame_size / 2);

    struct vector* prev_frame = NULL;

    for (uint32_t i = 0; i < in->size; i++) {
        struct vector* frame = _iMDCT_frame(
                m_plan, 
                (struct vector*) vector_object_at(in, i)
            );

        if (i == 0) {
            /* 
             * we need to deal with overlapping mdct frames, so omit the first frame
             */
            prev_frame = frame;
            continue;
        }

        /* perfect reconstruction */
        for (uint32_t j = 0; j < frame_size / 2; j++) {
            vector_push_back_double(ret, 
                    vector_double_at(prev_frame, frame_size / 2 + j)
                    +
                    vector_double_at(frame, j)
                );
        }

        vector_destroy(prev_frame, NULL);
        prev_frame = frame;
    }

    mdct_free(m_plan);
    vector_destroy(prev_frame, NULL);

    return ret;
}

/* 
 *   Compute the MDCT of a frame
 *   in  -- vector of double
 *   ret -- vector of double
 *   ret value is MDCT of in
 */
static struct vector* _MDCT_frame(struct mdct_plan* m_plan, struct vector* in) {
    uint32_t len = in->size;

    double* src = (double*) malloc(sizeof(double) * len);
    double* dst = (double*) malloc(sizeof(double) * len / 2);
    if (src == NULL || dst == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    for (uint32_t i = 0; i < len; i++) {
        src[i] = vector_double_at(in, i);
    }

    mdct(dst, src, m_plan);

    struct vector* ret = vector_init();

    for (uint32_t i = 0; i < len / 2; i++) {
        vector_push_back_double(ret, dst[i]);
    }

    free(src);
    free(dst);

    return ret;
}

/* 
 *   Compute the iMDCT of a frame
 *   in  -- vector of double
 *   ret -- vector of double
 *   ret value is inverse MDCT of in
 */
static struct vector* _iMDCT_frame(struct mdct_plan* m_plan, struct vector* in) {
    uint32_t len = in->size;

    double* src = (double*) malloc(sizeof(double) * len);
    double* dst = (double*) malloc(sizeof(double) * len * 2);
    if (src == NULL || dst == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    for (uint32_t i = 0; i < len; i++) {
        src[i] = vector_double_at(in, i);
    }

    imdct(dst, src, m_plan);

    struct vector* ret = vector_init();
    for (uint32_t i = 0; i < len * 2; i++) {
        vector_push_back_double(ret, dst[i]);
    }

    free(src);
    free(dst);
    
    return ret;
}

#endif
