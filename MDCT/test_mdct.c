#include <stdio.h>
#include <stdlib.h>
#include "mdct.h"
#include "../util/vector.h"

int main(int argc, char** argv) {
    const int SIZE = 32;
    
    struct vector* test_data = vector_init();
    for (uint32_t i = 0; i < SIZE; i++) {
        vector_push_back_double(test_data, 9.0 - i);
    }
    vector_print_double(test_data);

    struct vector* out = MDCT(test_data);
    // printf("\nintermediate results:\n");
    // for (uint32_t i = 0; i < out->size; i++) {
    //     // printf("printing vector %d\n", i);
    //     vector_print_double((struct vector*) vector_object_at(out, i));
    // }

    printf("\n\nrecv:\n\n");
    struct vector* recv = iMDCT(out);
    vector_print_double(recv);

    return 0;
}
