#include <stdio.h>
#include "bitstream.h"

int main() {
	struct bit_stream* bs = bitstream_init();
	bitstream_push(bs, 15, 15);
	bitstream_print(bs);
	bitstream_destroy(bs);
}