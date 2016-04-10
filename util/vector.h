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

struct vector* vector_init();                                    /* constructor */
void vector_destroy(struct vector* v, void (*free_func)(void*)); /* destructor */

struct vector* vector_sub(struct vector* v, 
                            uint32_t i, 
                            uint32_t len);  
                              /* return a subvector of orig vector, 
                               * starting at index i, length len, 
                               * padding with 0 if overflow.
                               */

void vector_print_int32(struct vector* v);      /* print the vector as int */
void vector_print_double(struct vector* v);     /* print the vector as double */

void vector_push_back_int32(struct vector* v, int32_t elem);    /* push_back an integer */
void vector_push_back_uint32(struct vector* v, uint32_t elem);  /* push_back an unsigned integer */
void vector_push_back_double(struct vector* v, double elem);    /* push_back a double */
void vector_push_back_object(struct vector* v, void* elem);     /* push_back a arbitrary object */

void vector_pop_back(struct vector* v);                     /* pop last element */

uint32_t vector_uint32_at(struct vector* v, uint32_t pos);
int32_t vector_int32_at(struct vector* v, uint32_t pos);
double vector_double_at(struct vector* v, uint32_t pos);
void* vector_object_at(struct vector* v, uint32_t pos);

void free_func_2dvec(void* vec);   /* used for free 2-d vector */

#endif