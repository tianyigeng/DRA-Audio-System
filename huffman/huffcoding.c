/*
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __HUFFCODING_C_
#define __HUFFCODING_C_

#include <stdint.h>
#include <stdlib.h>
#include "huffcoding.h"

/*
 *  Decode a bitstream into list of integers according to the code book
 */
void huff_decode(struct huff_codebook* book, 
                    struct bit_stream* src, 
                    struct vector* result) {
    /* a tree to help decoding */
    struct TreeNode* root = build_tree(book);

    struct TreeNode* curr = root;

    const uint32_t len = bitstream_size(src);
    const uint16_t max_book_indx = book->size - 1;

    /*
     * note that we may encounter large indices, e.g.
     * we have a huffbook for range 0~7, now, we need to encode 15 with this book, 
     * the solution is simple -- just to divide 15 into [7, 7, 1]
     * and make the encoder & decoder capable of this.
     * All will be done. 
     */
    uint32_t cum_val = 0;

    for (uint32_t i = 0; i < len; i++) {
        if (bitstream_bit_at(src, i)) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }

        if (curr->is_leaf) {
            cum_val += curr->val;
            if (curr->val != max_book_indx) {
                vector_push_back_uint32(result, cum_val);
                cum_val = 0;
            }
            curr = root;
        }
    }

    erase_tree(root);
}

/*
 *  Encode list of integers into a bitstream according to the code book
 */
void huff_encode(struct huff_codebook* book, 
                    struct vector* src, 
                    struct bit_stream* result) {
    uint16_t* temp_code = (uint16_t*) malloc(sizeof(uint16_t) * book->size);
    uint16_t* temp_bits = (uint16_t*) malloc(sizeof(uint16_t) * book->size);
    uint16_t cumulate_size = 0;

    if (temp_bits == NULL || temp_code == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
    }

    const uint16_t max_book_indx = book->size - 1;

    /* set up library here */
    for (uint16_t i = 0; i < book->size; i++) {
        cumulate_size += book->bit_incr[i];
        temp_code[book->index[i]] = book->code[i];
        temp_bits[book->index[i]] = cumulate_size;
    }

    /* query from the library, and push to bitstream */
    for (uint32_t i = 0; i < src->size; i++) {

        uint32_t indx = vector_uint32_at(src, i);

        /* handle overflow */
        while (indx >= max_book_indx) {
            indx = indx - max_book_indx;
            bitstream_push(result, temp_code[max_book_indx], temp_bits[max_book_indx]);
        }

        /* process the remainder */
        bitstream_push(result, temp_code[indx], temp_bits[indx]);
    }

    free(temp_code);
    free(temp_bits);
}

#endif
