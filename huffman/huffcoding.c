/*
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __HUFFCODING_C_
#define __HUFFCODING_C_

#include <stdlib.h>
#include "huffcoding.h"

/* used for encoding/decoding bit_stream with codebook HuffDec27_256x1 */
static void _huff_decode_overflow(struct huff_codebook* book, 
                                    struct bit_stream* src, 
                                    struct vector* result);
static void _huff_encode_overflow(struct huff_codebook* book, 
                                    struct vector* src, 
                                    struct bit_stream* result);

/* used for encoding/decoding bit_stream with other codebooks */
static void _huff_decode_standard(struct huff_codebook* book, 
                                    struct bit_stream* src, 
                                    struct vector* result);
static void _huff_encode_standard(struct huff_codebook* book, 
                                    struct vector* src, 
                                    struct bit_stream* result);

/*
 *  Decode a bitstream into list of integers according to the code book
 */
void huff_decode(struct huff_codebook* book, 
                    struct bit_stream* src, 
                    struct vector* result) {
    if (book == &HuffDec27_256x1) {
        _huff_decode_overflow(book, src, result);
        return;
    }
    _huff_decode_standard(book, src, result);
}

/*
 *  Encode list of integers into a bitstream according to the code book
 */
void huff_encode(struct huff_codebook* book, 
                    struct vector* src, 
                    struct bit_stream* result) {
    if (book == &HuffDec27_256x1) {
        _huff_encode_overflow(book, src, result);
        return;
    }
    _huff_encode_standard(book, src, result);
}

static void _huff_decode_overflow(struct huff_codebook* book, 
                                    struct bit_stream* src, 
                                    struct vector* result) {
    /* a tree to help decoding */
    struct tree_node* root = build_tree(book);

    struct tree_node* curr = root;

    const uint32_t len = bitstream_size(src);
    const uint32_t max_book_indx = book->size - 1;

    /*
     * note that we may encounter large indices, e.g.
     * we have a huffbook for range 0~7, now, we need to encode 15 with this book, 
     * the solution is simple -- just to divide 15 into [7, 7, 1]
     * and make the encoder & decoder capable of this.
     * All will be done. 
     * 
     * besides, another bit is appended to any index to indicate the sign
     * 1 - negative, 0 - positive
     */
    int32_t cum_val = 0;

    for (uint32_t i = 0; i < len; i++) {
        if (bitstream_bit_at(src, i)) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }

        if (curr->is_leaf) {
            i++; /* get the sign of the index */
            int32_t sign = bitstream_bit_at(src, i) ? -1 : 1;
            cum_val += sign * curr->val;
            if (curr->val != max_book_indx) {
                vector_push_back_uint32(result, cum_val);
                cum_val = 0;
            }
            curr = root;
        }
    }

    erase_tree(root);
}

static void _huff_decode_standard(struct huff_codebook* book, 
                                    struct bit_stream* src, 
                                    struct vector* result) {
    /* a tree to help decoding */
    struct tree_node* root = build_tree(book);

    struct tree_node* curr = root;

    const uint32_t len = bitstream_size(src);
    const uint32_t max_book_indx = book->size - 1;

    /*
     * note that no overflow need to be considered here. 
     */

    for (uint32_t i = 0; i < len; i++) {
        if (bitstream_bit_at(src, i)) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }

        if (curr->is_leaf) {
            vector_push_back_int32(result, curr->val - (int32_t) (len / 2));
            curr = root;
        }
    }

    erase_tree(root);
}

static void _huff_encode_overflow(struct huff_codebook* book, 
                                    struct vector* src, 
                                    struct bit_stream* result) {
    uint32_t* temp_code = (uint32_t*) malloc(sizeof(uint32_t) * book->size);
    uint32_t* temp_bits = (uint32_t*) malloc(sizeof(uint32_t) * book->size);
    uint32_t cumulate_size = 0;

    if (temp_bits == NULL || temp_code == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }

    const uint32_t max_book_indx = book->size - 1;

    /* set up library here */
    for (uint32_t i = 0; i < book->size; i++) {
        cumulate_size += book->bit_incr[i];
        temp_code[book->index[i]] = book->code[i];
        temp_bits[book->index[i]] = cumulate_size;
    }

    /* query from the library, and push to bitstream */
    for (uint32_t i = 0; i < src->size; i++) {

        int32_t indx = vector_int32_at(src, i);
        int32_t sign = indx < 0 ? -1 : 1;
        indx *= sign;

        /* handle overflow */
        while (indx >= max_book_indx) {
            indx = indx - max_book_indx;
            bitstream_push(result, temp_code[max_book_indx], temp_bits[max_book_indx]);
            bitstream_push_str(result, sign == -1 ? "1" : "0");
        }

        /* process the remainder */
        bitstream_push(result, temp_code[indx], temp_bits[indx]);
        bitstream_push_str(result, sign == -1 ? "1" : "0");
    }

    free(temp_code);
    free(temp_bits);
}

static void _huff_encode_standard(struct huff_codebook* book, 
                                    struct vector* src, 
                                    struct bit_stream* result) {
    uint32_t* temp_code = (uint32_t*) malloc(sizeof(uint32_t) * book->size);
    uint32_t* temp_bits = (uint32_t*) malloc(sizeof(uint32_t) * book->size);
    uint32_t cumulate_size = 0;

    if (temp_bits == NULL || temp_code == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }

    const uint32_t max_book_indx = book->size - 1;

    /* set up library here */
    for (uint32_t i = 0; i < book->size; i++) {
        cumulate_size += book->bit_incr[i];
        temp_code[book->index[i]] = book->code[i];
        temp_bits[book->index[i]] = cumulate_size;
    }

    /* 
     * query from the library, and push to bitstream
     * note that no overflow here. 
     */
    for (uint32_t i = 0; i < src->size; i++) {
        uint32_t indx = (uint32_t) (vector_int32_at(src, i) + book->size / 2);
        bitstream_push(result, temp_code[indx], temp_bits[indx]);
    }

    free(temp_code);
    free(temp_bits);
}

#endif
