#ifndef __HUFFCODING_H_
#define __HUFFCODING_H_

#include "huffbook.h"
#include "hufftree.h"
#include "../bitstream/bitstream.h"
#include "../util/vector.h"

void huff_decode(struct huff_codebook* book, 
                    struct bit_stream* src, 
                    struct vector* result);

void huff_encode(struct huff_codebook* book, 
                    struct vector* src, 
                    struct bit_stream* result);

#endif
