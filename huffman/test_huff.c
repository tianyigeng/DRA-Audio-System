#include <stdio.h>
#include "../bitstream/bitstream.h"
#include "../util/vector.h"
#include "../util/errors.h"
#include "huffcoding.h"
#include "huffbook.h"

int main() {
    struct vector* v = vector_init();
    struct vector* result = vector_init();
    struct bit_stream* bs = bitstream_init();

    vector_push_back_int32(v, 147);
    vector_push_back_int32(v, 5);
    vector_push_back_int32(v, 16);
    vector_push_back_int32(v, 12);
    vector_push_back_int32(v, 13);
    vector_push_back_int32(v, 2);
    vector_push_back_int32(v, 1);
    vector_push_back_int32(v, 1);
    vector_push_back_int32(v, 0);
    vector_print_int32(v);

    huff_encode(&HuffDec27_256x1, v, bs);
    bitstream_print(bs);
    
    huff_decode(&HuffDec27_256x1, bs, result);
    vector_print_int32(result);

    vector_destroy(v);
    vector_destroy(result);
    bitstream_destroy(bs);
}