#ifndef __VECTOR_C_
#define __VECTOR_C_

#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

/* A C++ vector like container implementation */

static const uint32_t INIT_CAP  = 16;
static const uint32_t SCALE     = 2;

static void _vector_grow(struct vector* v);
static void _vector_push_back(struct vector* v, void* elem);

struct vector* vector_init() {
    struct vector* v = (struct vector*) malloc(sizeof(struct vector));
    if (v == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    v->cap = INIT_CAP;
    v->size = 0;
    v->buf = (void**) malloc(sizeof(void*) * v->cap);
    if (v->buf == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
    }
    return v;
}

void vector_destroy(struct vector* v, void (*free_func)(void*)) {
    if (free_func == NULL) {
        /*
         *  Unless specified, we use the standard free.
         */
        free_func = free;
    }

    for (uint32_t i = 0; i < v->size; i++) {
        free_func(v->buf[i]);
    }
    free(v->buf);
    free(v);
}

void vector_print_int32(struct vector* v) {
    printf("================================\n");
    printf("vector size: %d\n", v->size);
    printf("vector cap: %d\n", v->cap);
    printf("vector content: ");
    for (uint32_t i = 0; i < v->size; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("%d ", *((int32_t*)(v->buf[i])));
    }
    printf("\n");
    printf("================================\n");
}

void vector_print_double(struct vector* v) {
    printf("================================\n");
    printf("vector size: %d\n", v->size);
    printf("vector cap: %d\n", v->cap);
    printf("vector content: ");
    for (uint32_t i = 0; i < v->size; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("%.2f ", *((double*)(v->buf[i])));
    }
    printf("\n");
    printf("================================\n");
}

void vector_push_back_int32(struct vector* v, int32_t elem) {
    int32_t* e = (int32_t*) malloc(sizeof(int32_t));
    if (e == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }

    *e = elem;
    _vector_push_back(v, (void*) e);
}

void vector_push_back_uint32(struct vector* v, uint32_t elem) {
    uint32_t* e = (uint32_t*) malloc(sizeof(uint32_t));
    if (e == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }

    *e = elem;
    _vector_push_back(v, (void*) e);
}

void vector_push_back_double(struct vector* v, double elem) {
    double* e = (double*) malloc(sizeof(double));
    if (e == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }
    *e = elem;

    _vector_push_back(v, (void*) e);
}

void vector_push_back_object(struct vector* v, void* elem) {
    void** e = (void**) malloc(sizeof(double));
    if (e == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }
    *e = elem;

    _vector_push_back(v, (void*) e);
}

void vector_pop_back(struct vector* v) {
    if (v->size == 0) {
        handle_error(ERROR_CONTAINER_EMPTY);
        return;
    }

    free(v->buf[v->size - 1]);
    v->size--;
}

uint32_t vector_uint32_at(struct vector* v, uint32_t pos) {
    if (pos >= v->size) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
        return 0;
    }

    return *(uint32_t*)(v->buf[pos]);
}

int32_t vector_int32_at(struct vector* v, uint32_t pos) {
    if (pos >= v->size) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
        return 0;
    }

    return *(int32_t*)(v->buf[pos]);
}

double vector_double_at(struct vector* v, uint32_t pos) {
    if (pos >= v->size) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
        return 0.0f;
    }

    return *(double*)(v->buf[pos]);
}

void* vector_object_at(struct vector* v, uint32_t pos) {
    if (pos >= v->size) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
        return NULL;
    }

    return *(void**)(v->buf[pos]);
}

struct vector* vector_sub(struct vector* v, 
                            uint32_t pos, 
                            uint32_t len) {
    struct vector* ret = vector_init();

    for (uint32_t i = 0; i < len; i++) {
        vector_push_back_int32(ret, 
                pos + i >= v->size ? 
                    0 : 
                    vector_int32_at(v, pos + i)
        );
    }

    return ret;
}

void free_func_2dvec(void* vec) {
    struct vector* v = (struct vector*) vec;
    vector_destroy(v, NULL);
}

static void _vector_push_back(struct vector* v, void* elem) {
    if (v->size == v->cap) {
        _vector_grow(v);
    }
    v->buf[v->size] = elem;
    v->size++;
}

static void _vector_grow(struct vector* v) {
    v->buf = realloc(v->buf, sizeof(void*) * v->cap * SCALE);
    if (v->buf == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }
    
    v->cap *= SCALE;
}

#endif
