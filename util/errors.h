#ifndef __ERRORS_H_
#define __ERRORS_H_

#include <stdint.h>

extern const uint32_t ERROR_FAILURE_ALLOC_MEM;
extern const uint32_t ERROR_UNKNOWN;

void handle_error(uint32_t code);

#endif
