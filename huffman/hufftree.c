#include <stdlib.h>
#include "hufftree.h"

static struct TreeNode* _new_node();
static void _tree_insert(struct TreeNode* root, uint16_t cnt, 
                            uint16_t code, uint16_t val);

struct TreeNode* build_tree(struct huff_codebook* book) {
    struct TreeNode* root = _new_node();
    uint16_t currbits = 0;
    for (uint16_t i = 0; i < book->size; i++) {
        currbits += book->bit_incr[i];
        _tree_insert(root, currbits, book->code[i], book->index[i]);
    }
    return root;
}

void erase_tree(struct TreeNode* root) {
    if (root == NULL) {
        return;
    }
    erase_tree(root->left);
    erase_tree(root->right);
    free(root);
}

static struct TreeNode* _new_node() {
    struct TreeNode* n = (struct TreeNode*) malloc(sizeof(struct TreeNode));
    n->val = -1;
    n->is_leaf = 0;
    n->left = NULL;
    n->right = NULL;
    return n;
}

static void _tree_insert(struct TreeNode* root, uint16_t bitcnt, uint16_t code, uint16_t val) {
    struct TreeNode* curr = root;
    for (int32_t i = bitcnt - 1; i >= 0; i--) {
        uint16_t choice = (code >> i) & 0x1;
        if (choice) {
            if (!curr->right) {
                curr->right = _new_node();
            }
            curr = curr->right;
        } else {
            if (!curr->left) {
                curr->left = _new_node();
            }
            curr = curr->left;
        }
    }
    curr->is_leaf = 1;
    curr->val = val;
}
