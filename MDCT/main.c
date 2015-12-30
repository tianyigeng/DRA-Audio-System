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
	int* test_data_stream = malloc(SIZE * 4 * sizeof(int));
	int* test_data_in;
	double* test_data_out = malloc(SIZE * sizeof(double));
	double* test_data_return = malloc(SIZE * 2 * sizeof(double));

	for (int i = 0; i < SIZE * 4; i++) {
		test_data_stream[i] = 8 - i > 0 ? 3 - i : i - 3;
	}

	// Original Array
	printf("Original Array: \n");
	dump_array_int(SIZE * 4, test_data_stream);
	printf("\n");

	// Frame 1
	test_data_in = test_data_stream;
	MDCT(SIZE, test_data_in, test_data_out);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_int(SIZE * 2, test_data_in);
	printf("Output Array: \n");
	dump_array_double(SIZE, test_data_out);

	iMDCT(SIZE, test_data_out, test_data_return);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_double(SIZE, test_data_out);
	printf("Output Array: \n");
	dump_array_double(SIZE * 2, test_data_return);
	printf("\n");

	// Frame 2
	test_data_in = test_data_in + SIZE;
	MDCT(SIZE, test_data_in, test_data_out);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_int(SIZE * 2, test_data_in);
	printf("Output Array: \n");
	dump_array_double(SIZE, test_data_out);

	iMDCT(SIZE, test_data_out, test_data_return);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_double(SIZE, test_data_out);
	printf("Output Array: \n");
	dump_array_double(SIZE * 2, test_data_return);
	printf("\n");

	// Frame 3
	test_data_in = test_data_in + SIZE;
	MDCT(SIZE, test_data_in, test_data_out);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_int(SIZE * 2, test_data_in);
	printf("Output Array: \n");
	dump_array_double(SIZE, test_data_out);

	iMDCT(SIZE, test_data_out, test_data_return);
	printf("Size: %d\n", SIZE);
	printf("Input Array: \n");
	dump_array_double(SIZE, test_data_out);
	printf("Output Array: \n");
	dump_array_double(SIZE * 2, test_data_return);
	printf("\n");
	
	free(test_data_stream);
	free(test_data_out);
	free(test_data_return);
	return 0;
}

// #endif