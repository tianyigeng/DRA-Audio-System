#ifndef __UNITSTEP_C_
#define __UNITSTEP_C_

#include <stdio.h>
#include "unitstep.h"

const double STEP[] = {1.0f, 0.5f, 0.03148f};

static void _unit_step_helper(struct vector* in,    /* input vector */
                                struct vector* out, /* output vector */
                                uint16_t pos,       /* from which position to unitstep */
                                uint16_t len,       /* how many samples to unitstep */
                                uint16_t index      /* which index to use */
                                );

// in  --- vector of double
// ret --- vector of uint32
struct vector* unit_step(struct vector* in) {
    struct vector* ret = vector_init();

    //TODO
    _unit_step_helper(in, ret, 0, in->size, 1);

    return ret;
}

// helper function for unit steping
static void _unit_step_helper(struct vector* in, struct vector* out, uint16_t pos, uint16_t len, uint16_t index) {
    for (uint16_t i = pos; i < pos + len; i++) {
        vector_push_back_uint32(out, (uint32_t) (vector_double_at(in, i) / STEP[index]));
    }
}

// print out available steps
void print_steps() {
    uint32_t len = sizeof(STEP) / sizeof(double);

    printf("steps: \n");
    printf("================================\n");
    printf("size: %d\n", len);
    printf("content: ");
    for (uint32_t i = 0; i < len; i++) {
        printf("%.2f ", STEP[i]);
    }
    printf("\n");
    printf("================================\n");
}

#endif
