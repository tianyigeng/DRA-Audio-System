#ifndef __ERRORS_H_
#define __ERRORS_H_

#include <stdint.h>

#define ERROR_CODE uint32_t

extern const ERROR_CODE ERROR_FAILURE_ALLOC_MEM;
extern const ERROR_CODE ERROR_INDEX_OUT_OF_BOUND;
extern const ERROR_CODE ERROR_CONTAINER_EMPTY;
extern const ERROR_CODE ERROR_INVALID_ARGV;
extern const ERROR_CODE ERROR_UNKNOWN;

void handle_error(ERROR_CODE code);

#endif
