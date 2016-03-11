#include <stdlib.h>
#include "hufftree.h"
#include "huffbook.h"

void build_tree(struct TreeNode* root, struct HuffCodeBook* book) {

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
    uint16_t i;
    for (i = bitcnt; i > 0; i--) {
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
