#include "huffbook.h"

static const HUFFBITS Incr1[] = {2, 0, 1, 0, 0, 1, 0};
static const HUFFBITS Code1[] = {0, 3, 3, 2, 5, 9, 8};
static const HUFFBITS Indx1[] = {0, 1, 2, 3, 5, 4, 6};
struct HuffCodeBook HuffDec1_7x1 = { sizeof(Incr1) / sizeof(HUFFBITS), Incr1, Code1, Indx1};

static const HUFFBITS Incr2[] = {2, 0, 1, 0, 0, 1, 0};
static const HUFFBITS Code2[] = {0, 3, 3, 2, 5, 9, 8};
static const HUFFBITS Indx2[] = {0, 1, 2, 3, 5, 4, 6};
struct HuffCodeBook HuffDec1_7x2 = { sizeof(Incr2) / sizeof(HUFFBITS), Incr2, Code2, Indx2};
