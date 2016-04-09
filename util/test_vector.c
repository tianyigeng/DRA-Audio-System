#include <stdio.h>
#include "vector.h"

int main() {
    {
        printf("\n\nTest case1:\n");
        struct vector* v = vector_init();
        for (int i = 0; i < 111; i++) {
            vector_push_back_int32(v, i + 1);
        }
        vector_print_int32(v);
        vector_pop_back(v);
        vector_print_int32(v);
        
        vector_destroy(v, NULL);
    }

    {
        printf("\n\nTest case2:\n");
        struct vector* v = vector_init();
        for (int i = 0; i < 5; i++) {
            vector_push_back_object(v, (void*) vector_init());
            vector_push_back_int32((struct vector*) vector_object_at(v, i), 200);
        }
        for (int i = 0; i < 5; i++) {
            vector_print_int32((struct vector*) vector_object_at(v, i));
        }
        
        vector_destroy(v, NULL);
    }

    return 0;
}