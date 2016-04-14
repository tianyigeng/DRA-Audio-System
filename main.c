#include <stdio.h>
#include "huffman/huffbook.h"
#include "huffman/huffcoding.h"
#include "bitstream/bitstream.h"
#include "mdct/mdct.h"
#include "util/vector.h"
#include "unitstep/unitstep.h"

static inline void message(const char* in) {
    printf("%s\n", in);
}

int main(int argc, char** argv) {

    const int SIZE = 16 * 8;
    
    struct vector* test_data = vector_init();
    for (uint16_t i = 0; i < SIZE; i++) {
        vector_push_back_double(test_data, 9.0 - i);
    }
    message("Original data:");
    vector_print_double(test_data);

    struct vector* mdct = MDCT(test_data);
    message("After MDCT:");
    for (uint16_t i = 0; i < mdct->size; i++) {
        vector_print_double((struct vector*) vector_object_at(mdct, i));
    }
    
    struct vector* pre_imdct = vector_init();
    for (uint16_t i = 0; i < mdct->size; i++) {
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
    
    for (uint16_t i = 0; i < pre_imdct->size; i++) {
        vector_print_double((struct vector*) vector_object_at(pre_imdct, i));
    }

    message("After iMDCT:");
    struct vector* after_imdct = iMDCT(pre_imdct);
    vector_print_double(after_imdct);

    message("Releasing memory...");
    vector_destroy(mdct, free_func_2dvec);

    vector_destroy(pre_imdct, free_func_2dvec);
    vector_destroy(after_imdct, NULL);
    vector_destroy(test_data, NULL);

    message("Done! Exitting");
    
    return 0;
}
