#ifndef __UNITSTEP_H_
#define __UNITSTEP_H_

#include <stdint.h>
#include "../util/vector.h"

extern const double STEP[];

struct vector* unit_step(struct vector* in);

void print_steps();

#endif
