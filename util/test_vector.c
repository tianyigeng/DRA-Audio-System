#include <stdio.h>
#include "vector.h"

int main() {
    struct vector* v = vector_init();
    for (int i = 0; i < 111; i++) {
        vector_push_back_int32(v, i + 1);
    }
    vector_print_int32(v);
    vector_pop_back(v);
    vector_print_int32(v);
    
    vector_destroy(v, NULL);
    return 0;
}