#ifndef __ERRORS_C_
#define __ERRORS_C_

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"

const uint32_t ERROR_FAILURE_ALLOC_MEM = 0x500;
const uint32_t ERROR_UNKNOWN           = 0x999;

void handle_error(uint32_t code) {
    switch (code) {
        case ERROR_FAILURE_ALLOC_MEM:
            printf("memory allocation failure, exitting!");
            exit(ERROR_FAILURE_ALLOC_MEM);
            break;

        default:
            printf("unknown error, exitting!");
            exit(ERROR_UNKNOWN);
    }
}

#endif
