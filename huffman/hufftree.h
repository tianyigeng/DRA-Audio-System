#include <stdint.h>

struct TreeNode {
	int16_t val;
	unsigned char is_leaf;
	struct TreeNode* left;
	struct TreeNode* right;
};
