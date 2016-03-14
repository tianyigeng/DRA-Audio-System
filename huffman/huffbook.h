#ifndef __HUFFBOOK_H_
#define __HUFFBOOK_H_

#include <stdint.h>

#define HUFFBITS uint16_t
#define HUFFBOOKCNT 1

struct HuffCodeBook {
    uint16_t len;
    const HUFFBITS* BitIncr;
    const HUFFBITS* Code;
    const HUFFBITS* Index;
};

extern struct HuffCodeBook HuffDec1_7x1;
extern struct HuffCodeBook HuffDec1_7x2;

#endif