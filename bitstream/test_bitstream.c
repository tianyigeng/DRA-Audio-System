#include <stdio.h>
#include "bitstream.h"
#include "bs_iter.h"

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
    bitstream_push_str(bs, "10111011");
    bitstream_print(bs);

    struct bs_iter* iter = bs_iter_init(bs);
    printf("Unpack 4: %d\n", bs_iter_unpack(iter, 4));
    printf("Unpack 4: %d\n", bs_iter_unpack(iter, 4));
    printf("Unpack 4: %d\n", bs_iter_unpack(iter, 4));
    printf("Unpack 4: %d\n", bs_iter_unpack(iter, 4));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    printf("Unpack 8: %d\n", bs_iter_unpack(iter, 8));
    
    bitstream_destroy(bs);
    bs_iter_destroy(iter);
}