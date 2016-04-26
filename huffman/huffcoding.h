#ifndef __HUFFCODING_H_
#define __HUFFCODING_H_

#include <stdint.h>
#include "huffbook.h"
#include "hufftree.h"
#include "../bitstream/bs_iter.h"
#include "../bitstream/bitstream.h"
#include "../util/vector.h"

void huff_decode(struct huff_codebook* book, 
                    struct bit_stream* src, 
                    struct vector* result);

void huff_encode(struct huff_codebook* book, 
                    struct vector* src, 
                    struct bit_stream* result);

void ResetHuffIndex(struct huff_codebook* book, int32_t val);

int32_t HuffDecDiff(struct huff_codebook* book, struct bs_iter* iter);
int32_t HuffDec(struct huff_codebook* book, struct bs_iter* iter);
int32_t HuffDecRecursive(struct huff_codebook* book, struct bs_iter* iter);

void HuffEnc(struct huff_codebook* book, struct bit_stream* bs, int32_t val);
void HuffEncDiff(struct huff_codebook* book, struct bit_stream* bs, int32_t val);
void HuffEncRecursive(struct huff_codebook* book, struct bit_stream* bs, int32_t val);

#endif
