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

    const int SIZE = 32;
    
    struct vector* test_data = vector_init();
    for (uint16_t i = 0; i < SIZE; i++) {
        vector_push_back_double(test_data, 9.0 - i);
    }
    message("Original data:");
    vector_print_double(test_data);

    struct vector* mdct = MDCT(test_data);
    message("After MDCT:");
    vector_print_double(mdct);

    struct vector* stepped = unit_step(mdct);
    message("After unit step:");
    vector_print_int32(stepped);

    struct bit_stream* bs = bitstream_init();
    huff_encode(&HuffDec1_7x1, stepped, bs);
    message("After huffman encode:");
    bitstream_print(bs);
    
    struct vector* dehuff = vector_init();
    huff_decode(&HuffDec1_7x1, bs, dehuff);
    message("After huffman decode:");
    vector_print_int32(dehuff);

    struct vector* destep = inv_unit_step(dehuff);
    message("After inv unit step:");
    vector_print_double(destep);

    vector_destroy(test_data);
    vector_destroy(mdct);
    vector_destroy(destep);
    vector_destroy(stepped);
    vector_destroy(dehuff);
    bitstream_destroy(bs);
    return 0;
}