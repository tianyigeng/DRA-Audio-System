#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stdint.h>
#include "errors.h"

/* A C++ vector like container implementation */

struct vector {
    uint32_t cap;
    uint32_t size;
    void** buf;
};

struct vector* vector_init();               /* constructor */
void vector_destroy(struct vector* v);      /* destructor */

void print_vec_int32(struct vector* v);     /* print the vector */

void vector_push_back_int32(struct vector* v, int elem);    /* push_back an integer */
void vector_pop_back(struct vector* v);                     /* pop_back */

uint32_t vector_uint32_at(struct vector* v, uint32_t pos);
int32_t vector_int32_at(struct vector* v, uint32_t pos);

#endif