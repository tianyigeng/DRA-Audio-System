#include <stdio.h>
#include "../util/vector.h"
#include "unitstep.h"

int main() {
    struct vector* v = vector_init();
    vector_push_back_double(v, 2.0f);
    vector_push_back_double(v, 1.0f);
    vector_push_back_double(v, 3.5f);

    vector_print_double(v);

    print_steps();

    struct vector* out = unit_step(v);

    vector_print_int32(out);

    struct vector* recv = inv_unit_step(out);
    vector_print_double(recv);

    vector_destroy(v);
    vector_destroy(out);
    vector_destroy(recv);
    return 0;
}
