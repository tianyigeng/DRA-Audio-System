#include <stdint.h>
#include "huffbook.h"

struct TreeNode {
	int16_t val;
	unsigned char is_leaf;
	struct TreeNode* left;
	struct TreeNode* right;
};

struct TreeNode* build_tree(struct HuffCodeBook* book); /* build a tree to support encoding/decoding */
void erase_tree(struct TreeNode* root); 				/* free memory */
