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
    bitstream_destroy(bs);
}