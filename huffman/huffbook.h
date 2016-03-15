#ifndef __HUFFBOOK_H_
#define __HUFFBOOK_H_

#include <stdint.h>

struct huff_codebook {
    uint16_t size;
    const uint16_t* bit_incr;
    const uint16_t* code;
    const uint16_t* index;
};

extern struct huff_codebook HuffDec1_7x1;
extern struct huff_codebook HuffDec1_7x2;

#endif