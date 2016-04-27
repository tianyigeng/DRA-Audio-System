#ifndef __BITSTREAM_C_
#define __BITSTREAM_C_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitstream.h"

static inline uint8_t _uint32_bit_at(uint32_t num, uint8_t i);

struct bit_stream* bitstream_init() {
    struct bit_stream* bs = (struct bit_stream*) malloc(sizeof(struct bit_stream));
    if (bs == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    bs->vec = vector_init();
    bs->buffer = 0;
    bs->buf_size = 0;
    return bs;
}

void bitstream_print(struct bit_stream* bs) {
    printf("================================\n");
    printf("bitstream size: %d\n", bitstream_size(bs));
    printf("bitstream content: \n");
    for (int32_t i = 0; i < bs->vec->size; i++) {
        for (int32_t j = 31; j >= 0; j--) {
            printf("%d", (*(uint32_t*)(bs->vec->buf[i]) >> j) & 0x1);
            if (j % 8 == 0) {
                printf(" ");
            }
        }
        printf("\n");
    }

#ifdef DEBUG
    printf("buffer value: 0x%08x\n", bs->buffer);
#endif

    for (int32_t k = 31; k >= 32 - bs->buf_size; k--) {
        printf("%d", (bs->buffer >> k) & 0x1);
        if (k % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
    printf("================================\n");
}

void bitstream_destroy(struct bit_stream* bs) {
    vector_destroy(bs->vec, NULL);
    free(bs);
}

void bitstream_push_str(struct bit_stream* bs, 
                        const char* str) {
    uint32_t len = strlen(str);
    for (uint32_t i = 0; i < len; i++) {
        if (str[i] != '0' && str[i] != '1') {
            handle_error(ERROR_INVALID_ARGV);
        }

        bitstream_push(bs, str[i] - '0', 1);
    }
}

void bitstream_push(
    struct bit_stream* bs, 
    uint32_t target,     /* holder of the bits */
    uint32_t bits        /* how many bits to write */
    ) {

    /*
     *  This module to be optimized, we actually don't need to manipulate bit by bit. 
     */

    for (int32_t i = bits - 1; i >= 0; i--) {
        bs->buffer |= ((target >> i) & 0x1) << (31 - bs->buf_size);

#ifdef DEBUG
        printf("i: %d, tosft: %d, buffer size: %d, buffer value: %x\n", i, (target >> i) & 0x1, bs->buf_size, bs->buffer);
#endif

        bs->buf_size++;
        if (bs->buf_size == 32) {
            vector_push_back_int32(bs->vec, bs->buffer);
            bs->buffer = 0;
            bs->buf_size = 0;
        }
    }
}

uint32_t bitstream_size(struct bit_stream* bs) {
    return bs->vec->size * 32 + bs->buf_size;
}

/*
 *  Find out the i-th bit in the bitstream
 */
uint8_t bitstream_bit_at(struct bit_stream* bs, uint32_t i) {
    if (i >= bitstream_size(bs)) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
    }

    uint32_t cnt = i / 32;
    uint32_t rem = i % 32;
    if (cnt < bs->vec->size) {
        return _uint32_bit_at(vector_uint32_at(bs->vec, cnt), rem);
    }
    return _uint32_bit_at(bs->buffer, rem);
}

void bitstream_append(struct bit_stream* dst, struct bit_stream* src) {
    uint32_t len = bitstream_size(src);
    for (uint32_t i = 0; i < len; i++) {
        bitstream_push_str(dst, bitstream_bit_at(src, i) == 1 ? "1" : "0");
    }
}

/*
 *  Find out the i-th bit in the 32-bit form of num
 */
static inline uint8_t _uint32_bit_at(uint32_t num, uint8_t i) {
    if (i >= 32) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
    }
    return (num >> (31 - i)) & 0x1;
}

#endif
