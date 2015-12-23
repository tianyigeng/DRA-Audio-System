// #ifndef MAIN_C
// #define MAIN_C

#include "GeneralMDCT.h"
#include <stdio.h>
#include <stdlib.h>

void dump_array_int(const int len, const int* data) {
	for (int i = 0; i < len; i++) {
		printf("%d ", data[i]);
	}
	printf("\n");
}

void dump_array_double(const int len, const double* data) {
	for (int i = 0; i < len; i++) {
		printf("%.2f ", data[i]);
	}
	printf("\n");
}

int main(int argc, char** argv) {
	const int SIZE = 4;
	int* test_data_in = malloc(SIZE * 2 * sizeof(int));
	double* test_data_out = malloc(SIZE * sizeof(double));

	for (int i = 0; i < SIZE * 2; i++) {
		test_data_in[i] = 3 - i > 0 ? 3 - i : i - 3;
	}

	MDCT(SIZE, test_data_in, test_data_out);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_int(SIZE * 2, test_data_in);
	printf("Output Array: \n");
	dump_array_double(SIZE, test_data_out);

	iMDCT(SIZE, test_data_out, test_data_in);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_double(SIZE, test_data_out);
	printf("Output Array: \n");
	dump_array_int(SIZE * 2, test_data_in);
	
	free(test_data_in);
	free(test_data_out);
	return 0;
}

// #endif