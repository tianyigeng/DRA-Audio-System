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

void ResetHuffIndex(struct huff_codebook* book, int val);

int32_t HuffDecDiff(struct huff_codebook* book);

int32_t GetNumHuffCodes(struct huff_codebook* book);

int32_t HuffDec(struct huff_codebook* book);

int32_t GetHuffDim(struct huff_codebook* book);

uint8_t GetHuffMidTread(struct huff_codebook* book);

int32_t HuffDecRecursive(struct huff_codebook* book);

int32_t HuffDecode(struct huff_codebook* book);

#endif
