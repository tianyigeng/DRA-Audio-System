#include <stdlib.h>
#include "hufftree.h"

static struct TreeNode* _new_node();
static void _tree_insert(struct TreeNode* root, uint16_t cnt, 
                            uint16_t code, uint16_t val);

struct TreeNode* build_tree(struct HuffCodeBook* book) {
    struct TreeNode* root = _new_node();
    uint16_t currbits = 0;
    for (uint16_t i = 0; i < book->len; i++) {
        currbits += book->BitIncr[i];
        _tree_insert(root, currbits, book->Code[i], book->Index[i]);
    }
    return root;
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
    for (uint16_t i = bitcnt - 1; i >= 0; i--) {
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
}
