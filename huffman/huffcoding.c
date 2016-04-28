/*
 *  Copyright (C) 2016 Tianyi Geng
 */

#ifndef __HUFFCODING_C_
#define __HUFFCODING_C_

#include <stdlib.h>
#include "huffcoding.h"

void ResetHuffIndex(struct huff_codebook* book, int32_t val) {
    book->nIndex = val;
}

int32_t HuffDec(struct huff_codebook* book, struct bs_iter* iter) {
    /* a tree to help decoding */
    struct tree_node* root = build_tree(book);

    struct tree_node* curr = root;

    while (1) {
        /* loop until we decoded a value */
        if (bs_iter_unpack(iter, 1)) {
            curr = curr->right;
        } else {
            curr = curr->left;
        }

        if (curr->is_leaf) {
            int32_t ret = curr->val;
            erase_tree(root);
            return ret;
        }
    }

    handle_error(ERROR_UNKNOWN);
    return -1;
}

int32_t HuffDecDiff(struct huff_codebook* book, struct bs_iter* iter) {
    int32_t nDiff = HuffDec(book, iter);
    book->nIndex = (book->nIndex + nDiff) % book->size;
    return book->nIndex;
}

int32_t HuffDecRecursive(struct huff_codebook* book, struct bs_iter* iter) {
    int32_t k = -1;
    int32_t nQIndex = 0;
    int32_t nNumCodes = book->size;
    do {
        k++;
        nQIndex = HuffDec(book, iter);
    } while (nQIndex == nNumCodes - 1);
    return k * (nNumCodes - 1) + nQIndex;
}

void HuffEnc(struct huff_codebook* book, struct bit_stream* bs, int32_t val) {
    uint32_t* temp_code = (uint32_t*) malloc(sizeof(uint32_t) * book->size);
    uint32_t* temp_bits = (uint32_t*) malloc(sizeof(uint32_t) * book->size);
    uint32_t cumulate_size = 0;

    if (temp_bits == NULL || temp_code == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }

    /* set up library here */
    for (uint32_t i = 0; i < book->size; i++) {
        cumulate_size += book->bit_incr[i];
        temp_code[book->index[i]] = book->code[i];
        temp_bits[book->index[i]] = cumulate_size;
    }

    /* push to bitstream */
    bitstream_push(bs, temp_code[val], temp_bits[val]);

    free(temp_code);
    free(temp_bits);
}

void HuffEncDiff(struct huff_codebook* book, struct bit_stream* bs, int32_t val) {
    HuffEnc(book, bs, (val + book->size - book->nIndex) % book->size);
    book->nIndex = val;
}

void HuffEncRecursive(struct huff_codebook* book, struct bit_stream* bs, int32_t val) {
    int32_t nNumCodes = book->size;
    while (val >= nNumCodes - 1) {
        HuffEnc(book, bs, nNumCodes - 1);
        val -= nNumCodes - 1;
    }
    HuffEnc(book, bs, val);
}

#endif
