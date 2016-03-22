#ifndef __ERRORS_C_
#define __ERRORS_C_

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"

const ERROR_CODE ERROR_FAILURE_ALLOC_MEM  = 0x500;
const ERROR_CODE ERROR_INDEX_OUT_OF_BOUND = 0x501;
const ERROR_CODE ERROR_CONTAINER_EMPTY    = 0x502;
const ERROR_CODE ERROR_UNKNOWN            = 0x999;

void handle_error(ERROR_CODE code) {
    switch (code) {
        case ERROR_FAILURE_ALLOC_MEM:
            printf("memory allocation failure, exitting!");
            exit(ERROR_FAILURE_ALLOC_MEM);
            break;

        case ERROR_INDEX_OUT_OF_BOUND:
            printf("index out of bound failure, exitting!");
            exit(ERROR_INDEX_OUT_OF_BOUND);
            break;

        case ERROR_CONTAINER_EMPTY:
            printf("Error vector_pop_back, vector is empty");
            break;

        default:
            printf("unknown error, exitting!");
            exit(ERROR_UNKNOWN);
    }
}

#endif
