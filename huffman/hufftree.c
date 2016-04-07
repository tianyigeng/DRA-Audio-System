#include <stdlib.h>
#include "hufftree.h"
#include "../util/errors.h"

/*
 *  A trie-like structure to support decoding huffman bitstream. 
 *  0 represents the left subtree;
 *  1 represents the right subtree.
 */

/* init a new trie node, alloc extra memory */
static struct tree_node* _new_node();

/* insert a new node in the trie */
static void _tree_insert(struct tree_node* root, 
                            uint16_t cnt, 
                            uint16_t code, 
                            uint16_t val);

/* build a tree from the huffman code book */
struct tree_node* build_tree(struct huff_codebook* book) {
    struct tree_node* root = _new_node();
    uint16_t currbits = 0;
    for (uint16_t i = 0; i < book->size; i++) {
        currbits += book->bit_incr[i];
        _tree_insert(root, currbits, book->code[i], book->index[i]);
    }
    return root;
}

/* free the memory to prevent from memory leakage */
void erase_tree(struct tree_node* root) {
    if (root == NULL) {
        return;
    }
    erase_tree(root->left);
    erase_tree(root->right);
    free(root);
}

static struct tree_node* _new_node() {
    struct tree_node* n = (struct tree_node*) malloc(sizeof(struct tree_node));
    if (n == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }

    n->val = -1;
    n->is_leaf = 0;
    n->left = NULL;
    n->right = NULL;
    return n;
}

static void _tree_insert(struct tree_node* root, 
                            uint16_t bitcnt, 
                            uint16_t code, 
                            uint16_t val) {
    struct tree_node* curr = root;
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
