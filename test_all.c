#include <stdio.h>
#include <stdlib.h>
#include "huffman/huffbook.h"
#include "huffman/huffcoding.h"
#include "bitstream/bitstream.h"
#include "mdct/mdct.h"
#include "util/vector.h"
#include "dataframe/dataframe.h"

static inline void message(const char* in) {
    printf("%s\n", in);
}

int main(int argc, char** argv) {

    if (argc != 3) {
        printf("INVALID!\n");
        handle_error(ERROR_INVALID_ARGV);
    }

    const char* in_file = argv[1];
    const char* out_file = argv[2];
    
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

    printf("mdct begin\n");
    struct vector* after_mdct = MDCT(test_data);
    printf("mdct done\n");
    // message("After MDCT:");
    // for (uint32_t i = 0; i < mdct->size; i++) {
    //     vector_print_double((struct vector*) vector_object_at(mdct, i));
    // }
    
    struct bit_stream* bs = bitstream_init();
    struct vector* pre_imdct = vector_init();
    dra_encode(after_mdct, bs);
    printf("original size: %d\n", test_data->size);
    printf("bitstream size: %d\n", bitstream_size(bs));
    printf("compression ratio: %.2f\n", test_data->size * 16.0 / bitstream_size(bs));
    dra_decode(bs, pre_imdct);
    bitstream_destroy(bs);
    // for (uint32_t i = 0; i < mdct->size; i++) {
    //     struct vector* tomdct = (struct vector*) vector_object_at(mdct, i);
    //     struct vector* stepped = unit_step(tomdct);
    //     // message("After unit step:");
    //     // vector_print_int32(stepped);

    //     struct bit_stream* bs = bitstream_init();
    //     huff_encode(&HuffDec27_256x1, stepped, bs);
    //     // message("After huffman encode:");
    //     // bitstream_print(bs);
        
    //     struct vector* dehuff = vector_init();
    //     huff_decode(&HuffDec27_256x1, bs, dehuff);
    //     // message("After huffman decode:");
    //     // vector_print_int32(dehuff);

    //     struct vector* destep = inv_unit_step(dehuff);
    //     // message("After inv unit step:");
    //     // vector_print_double(destep);

    //     vector_push_back_object(pre_imdct, (struct vector*) destep);

    //     vector_destroy(stepped, NULL);
    //     vector_destroy(dehuff, NULL);
    //     bitstream_destroy(bs);
    // }
    
    // for (uint32_t i = 0; i < pre_imdct->size; i++) {
    //     vector_print_double((struct vector*) vector_object_at(pre_imdct, i));
    // }

    message("After iMDCT:");
    struct vector* after_imdct = iMDCT(pre_imdct);
    // vector_print_double(after_imdct);

    /* dump data */
    FILE* fp_out = fopen(out_file, "w");
    for (uint32_t i = 0; i < after_imdct->size; i++) {
        fprintf(fp_out, "%.6f\n", vector_double_at(after_imdct, i));
    }
    fclose(fp_out);

    // message("Releasing memory...");
    vector_destroy(after_mdct, free_func_2dvec);

    vector_destroy(pre_imdct, free_func_2dvec);
    vector_destroy(after_imdct, NULL);
    vector_destroy(test_data, NULL);

    message("Done! Exitting");
    
    return 0;
}
