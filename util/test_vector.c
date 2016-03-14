#include <stdio.h>
#include "vector.h"

int main() {
	struct vector* v = vector_init();
	for (int i = 0; i < 111; i++) {
		vector_push_back_int32(v, i + 1);
	}
	print_vec_int32(v);
	vector_pop_back(v);
	print_vec_int32(v);
	vector_destroy(v);
	return 0;
}