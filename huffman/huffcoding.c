#include <stdint.h>
#include "huffcoding.h"

void huff_decode(struct huff_codebook* book, 
                    struct bit_stream* src, 
                    struct vector* result) {
    struct TreeNode* root = build_tree(book);
    // TODO
    erase_tree(root);
}

void huff_encode(struct huff_codebook* book, 
                    struct vector* src, 
                    struct bit_stream* result) {
    
    for (uint32_t i = 0; i < src->size; i++) {
        // TODO
    }
}
