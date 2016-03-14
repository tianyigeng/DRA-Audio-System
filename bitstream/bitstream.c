#include <stdlib.h>
#include "bitstream.h"

struct bit_stream* bitstream_init() {
    struct bit_stream* bs = (struct bit_stream*) malloc(sizeof(struct bit_stream));
    bs->vec = vector_init();
    bs->buffer = 0;
    bs->buf_size = 0;
    return bs;
}

void bitstream_destroy(struct bit_stream* bs) {
    vector_destroy(bs->vec);
    free(bs);
}

void bitstream_push(
    struct bit_stream* bs, 
    uint32_t target,     /* holder of the bits */
    uint32_t bits        /* how many bits to write */
    ) {
    for (uint32_t i = bits - 1; i != 0; i--) {
        if (bs->buf_size == 32) {
            vector_push_back_int32(bs->vec, bs->buffer);
            bs->buffer = 0;
            bs->buf_size = 0;
        }
        bs->buffer |= ((bits >> i) & 0x1) << (31 - bs->buf_size);
        bs->buf_size++;
    }
}
