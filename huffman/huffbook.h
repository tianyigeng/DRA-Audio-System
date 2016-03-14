#ifndef __HUFFBOOK_H_
#define __HUFFBOOK_H_

#include <stdint.h>

struct huff_codebook {
    uint16_t len;
    const uint16_t* BitIncr;
    const uint16_t* Code;
    const uint16_t* Index;
};

extern struct huff_codebook HuffDec1_7x1;
extern struct huff_codebook HuffDec1_7x2;

#endif