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
    uint32_t len = bitstream_size(src);

    for (uint32_t i = 0; i < len; i++) {
        if (bitstream_bit_at(src, i)) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }

        if (curr->is_leaf) {
            vector_push_back_uint32(result, curr->val);
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

    /* set up library here */
    for (uint16_t i = 0; i < book->size; i++) {
        cumulate_size += book->bit_incr[i];
        temp_code[book->index[i]] = book->code[i];
        temp_bits[book->index[i]] = cumulate_size;
    }

    /* query from the library, and push to bitstream */
    for (uint32_t i = 0; i < src->size; i++) {
        uint32_t indx = vector_uint32_at(src, i);
        bitstream_push(result, temp_code[indx], temp_bits[indx]);
    }

    free(temp_code);
    free(temp_bits);
}

#endif
