/*
 *  Copyright (C) 2016 Tianyi Geng
 */
 
#ifndef __BITSTREAM_H_
#define __BITSTREAM_H_

#include <stdint.h>
#include "../util/vector.h"
#include "../control.h"

struct bit_stream {
    struct vector*     vec;        /* use a vector of int32 to represent bit-stream */
    uint32_t           buffer;     /* a buffer */
    uint8_t            buf_size;   /* buffer size (how many bits are taken from last element) */
};

struct bit_stream* bitstream_init();            /* constructor */
void bitstream_destroy(struct bit_stream* bs);  /* destructor */

/* push a string of '0' and '1' to the bitstream */
void bitstream_push_str(struct bit_stream* bs, 
                        const char* str);

void bitstream_push(
    struct bit_stream* bs, 
    uint16_t target,      /* holder of the bits */
    uint16_t bits         /* how many bits to write */
    );

void bitstream_print(struct bit_stream* bs);

uint32_t bitstream_size(struct bit_stream* bs);

uint8_t bitstream_bit_at(struct bit_stream* bs, uint32_t pos); /* the bit(0/1) at the specified position */

#endif
