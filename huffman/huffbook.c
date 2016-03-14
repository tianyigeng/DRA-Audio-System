#include "huffbook.h"

static const uint16_t Incr1[] = {2, 0, 1, 0, 0, 1, 0};
static const uint16_t Code1[] = {0, 3, 3, 2, 5, 9, 8};
static const uint16_t Indx1[] = {0, 1, 2, 3, 5, 4, 6};
struct huff_codebook HuffDec1_7x1 = { sizeof(Incr1) / sizeof(uint16_t), Incr1, Code1, Indx1};

static const uint16_t Incr2[] = {2, 0, 1, 0, 0, 1, 0};
static const uint16_t Code2[] = {0, 3, 3, 2, 5, 9, 8};
static const uint16_t Indx2[] = {0, 1, 2, 3, 5, 4, 6};
struct huff_codebook HuffDec1_7x2 = { sizeof(Incr2) / sizeof(uint16_t), Incr2, Code2, Indx2};
