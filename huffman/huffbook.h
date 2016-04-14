#ifndef __HUFFBOOK_H_
#define __HUFFBOOK_H_

#include <stdint.h>

struct huff_codebook {
    uint32_t size;
    const uint32_t* bit_incr;
    const uint32_t* code;
    const uint32_t* index;
};

extern struct huff_codebook HuffDec1_7x1;
extern struct huff_codebook HuffDec27_256x1;

#endif