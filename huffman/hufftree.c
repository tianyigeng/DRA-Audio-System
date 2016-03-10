#include <stdlib.h>
#include "hufftree.h"
#include "huffbook.h"

void build_tree(struct TreeNode* root, struct HuffCodeBook* book) {

}

static void _node_init(struct TreeNode* n) {
	n->val = -1;
	n->is_leaf = 0;
	n->left = NULL;
	n->right = NULL;
}

static void _tree_insert(struct TreeNode* root, uint16_t bitcnt, uint16_t code, uint16_t val) {
	struct TreeNode* curr = root;
	uint16_t i;
	for (i = bitcnt - 1; i >= 0; i--) {
		uint16_t choice = (code >> i) & 0x1;
		if (choice) {
			if (!curr->right) {
				curr->right = (struct TreeNode*) malloc(sizeof(struct TreeNode));
				_node_init(curr->right);
			}
			curr = curr->right;
		} else {
			if (!curr->left) {
				curr->left = (struct TreeNode*) malloc(sizeof(struct TreeNode));
				_node_init(curr->left);
			}
			curr = curr->left;
		}
	}
	curr->is_leaf = 1;
}
