#ifndef __HUFFBOOK_H_
#define __HUFFBOOK_H_

#include <stdint.h>

#define MAX_BOOK    9

struct huff_codebook {
    uint32_t id;
    uint32_t dim;
    uint32_t num_codes;
    uint32_t size;
    const uint32_t* bit_incr;
    const uint32_t* code;
    const uint32_t* index;

    uint32_t nIndex; /* state variable, used by HuffEncDiff/HuffDecDiff, reset by ResetHuffIndex */
};

extern struct huff_codebook HuffDec1_7x1;
extern struct huff_codebook HuffDec27_256x1;

extern struct huff_codebook* pQuotientWidthBook;
extern struct huff_codebook* pQIndexBook;
extern struct huff_codebook* pQStepBook;
extern struct huff_codebook* pClusterBook;
extern struct huff_codebook* pRunLengthBook;
extern struct huff_codebook* pHSBook;

extern struct huff_codebook* QIndexBooks[MAX_BOOK]; /* shoule either be StableBooks or TransientBooks */

extern struct huff_codebook* StableBooks   [MAX_BOOK];
extern struct huff_codebook* TransientBooks[MAX_BOOK];

int32_t GetHuffDim(struct huff_codebook* book);

uint8_t GetHuffMidTread(struct huff_codebook* book);

int32_t GetNumHuffCodes(struct huff_codebook* book);

#endif