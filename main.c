#ifndef __MAIN_C_
#define __MAIN_C_

#include <stdio.h>
#include <stdlib.h>
#include "huffman/huffbook.h"
#include "huffman/huffcoding.h"
#include "bitstream/bitstream.h"
#include "mdct/mdct.h"
#include "util/vector.h"
#include "util/errors.h"
#include "unitstep/unitstep.h"

static inline void message(const char* in) {
    printf("%s\n", in);
}

int main(int argc, char** argv) {
    const char* in_file = "audio/in.pcm";
    const char* out_file = "audio/out.pcm";
    
    /* read input data */
    struct vector* test_data = vector_init();

    FILE* fp_in = fopen(in_file, "r");
    
    double* buf = (double*) malloc(sizeof(double));
    if (buf == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return -1;
    }

    while (fscanf(fp_in, "%lf\n", buf) == 1) {
        vector_push_back_double(test_data, *buf);
    }

    free(buf);
    fclose(fp_in);

    message("Original data:");
    vector_print_double(test_data);

    /* processing */
    struct vector* mdct = MDCT(test_data);
    message("After MDCT:");
    for (uint32_t i = 0; i < mdct->size; i++) {
        vector_print_double((struct vector*) vector_object_at(mdct, i));
    }
    
    struct vector* pre_imdct = vector_init();
    for (uint32_t i = 0; i < mdct->size; i++) {
        struct vector* tomdct = (struct vector*) vector_object_at(mdct, i);
        struct vector* stepped = unit_step(tomdct);
        message("After unit step:");
        // vector_print_int32(stepped);

        struct bit_stream* bs = bitstream_init();
        huff_encode(&HuffDec27_256x1, stepped, bs);
        message("After huffman encode:");
        // bitstream_print(bs);
        
        struct vector* dehuff = vector_init();
        huff_decode(&HuffDec27_256x1, bs, dehuff);
        message("After huffman decode:");
        // vector_print_int32(dehuff);

        struct vector* destep = inv_unit_step(dehuff);
        message("After inv unit step:");
        // vector_print_double(destep);

        vector_push_back_object(pre_imdct, (struct vector*) destep);

        vector_destroy(stepped, NULL);
        vector_destroy(dehuff, NULL);
        bitstream_destroy(bs);
    }

    message("After iMDCT:");
    struct vector* after_imdct = iMDCT(pre_imdct);

    /* dump data */
    FILE* fp_out = fopen(out_file, "w");
    for (uint32_t i = 0; i < after_imdct->size; i++) {
        fprintf(fp_out, "%.6f\n", vector_double_at(after_imdct, i));
    }
    fclose(fp_out);

    /* release memory */
    message("Releasing memory...");
    vector_destroy(mdct, free_func_2dvec);

    vector_destroy(pre_imdct, free_func_2dvec);
    vector_destroy(after_imdct, NULL);
    vector_destroy(test_data, NULL);

    message("Done! Exitting");
    
    return 0;
}

#endif
