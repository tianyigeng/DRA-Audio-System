#ifndef __STEPS_C_
#define __STEPS_C_

#include <stdio.h>
#include "unitstep.h"

const double STEP[] = {1.0f, 0.5f, 0.003148f};

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
