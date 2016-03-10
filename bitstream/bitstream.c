#include "bitstream.h"
#include "stdlib.h"

void open_bitstream(struct BitStream* bs, uint16_t size) {
	bs->data = (unsigned char*) malloc(size * sizeof(unsigned char));
	bs->size = size;
}