#ifndef __UNITSTEP_C_
#define __UNITSTEP_C_

#include <stdio.h>
#include "unitstep.h"

static void _unit_step_helper(struct vector* in,    /* input vector */
                                struct vector* out, /* output vector */
                                uint32_t pos,       /* from which position to unitstep */
                                uint32_t len,       /* how many samples to unitstep */
                                uint32_t index      /* which index to use */
                                );

static void _inv_unit_step_helper(struct vector* in,    /* input vector */
                                    struct vector* out, /* output vector */
                                    uint32_t pos,       /* from which position to unitstep */
                                    uint32_t len,       /* how many samples to unitstep */
                                    uint32_t index      /* which index to use */
                                    );

// in  --- vector of double
// ret --- vector of int32
struct vector* unit_step(struct vector* in) {
    struct vector* ret = vector_init();

    //TODO
    _unit_step_helper(in, ret, 0, in->size, 2);

    return ret;
}

// in  --- vector of int32
// ret --- vector of double
struct vector* inv_unit_step(struct vector* in) {
    struct vector* ret = vector_init();

    //TODO
    _inv_unit_step_helper(in, ret, 0, in->size, 2);

    return ret;
}

// helper function for unit steping
static void _unit_step_helper(struct vector* in, 
                                struct vector* out, 
                                uint32_t pos, 
                                uint32_t len, 
                                uint32_t index) {
    for (uint32_t i = pos; i < pos + len; i++) {
        vector_push_back_uint32(out, (int32_t) (vector_double_at(in, i) / STEP[index]));
    }
}

// helper function for inv unit steping
static void _inv_unit_step_helper(struct vector* in, 
                                    struct vector* out, 
                                    uint32_t pos, 
                                    uint32_t len, 
                                    uint32_t index) {
    for (uint32_t i = pos; i < pos + len; i++) {
        vector_push_back_double(out, (double) (vector_int32_at(in, i)) * STEP[index]);
    }
}

#endif
