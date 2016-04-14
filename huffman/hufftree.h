#ifndef __HUFFTREE_H_
#define __HUFFTREE_H_

#include <stdint.h>
#include "huffbook.h"

struct tree_node {
	int32_t val;
	unsigned char is_leaf;
	struct tree_node* left;
	struct tree_node* right;
};

struct tree_node* build_tree(struct huff_codebook* book); /* build a tree to support encoding/decoding */
void erase_tree(struct tree_node* root); 				/* free memory */

#endif