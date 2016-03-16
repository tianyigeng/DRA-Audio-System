#include <stdio.h>
#include "bitstream.h"

int main() {
    struct bit_stream* bs = bitstream_init();
    bitstream_push(bs, 15, 15);
    bitstream_push(bs, 1, 2);
    bitstream_push(bs, 22, 20);
    bitstream_push(bs, 122, 11);
    bitstream_push(bs, 1023, 19);
    bitstream_print(bs);
    // for (uint32_t i = 0; i < bitstream_size(bs); i++) {
    //     if (i % 8 == 0) {
    //         printf(" ");
    //     }
    //     printf("%d", bitstream_bit_at(bs, i));
    // }
    bitstream_destroy(bs);
}