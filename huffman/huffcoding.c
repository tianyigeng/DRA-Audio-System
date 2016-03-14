#include <stdint.h>
#include "huffcoding.h"

void huff_decode(struct HuffCodeBook* book, struct BitStream* src, struct vector* result) {
    struct TreeNode* root = build_tree(book);
    // TODO
    erase_tree(root);
}

void huff_encode(struct HuffCodeBook* book, struct vector* src, struct BitStream* result) {
    struct TreeNode* root = build_tree(book);
    for (uint32_t i = 0; i < src->size; i++) {
        // TODO
    }
    erase_tree(root);
}
