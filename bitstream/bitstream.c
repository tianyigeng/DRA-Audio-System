#include <stdlib.h>
#include "bitstream.h"

void open_bitstream(struct BitStream* bs, uint16_t size) {
	bs->data = (unsigned char*) malloc(size * sizeof(unsigned char));
	bs->size = size;
}

struct BitStream* copy_bitstream(struct BitStream* src) {
	
}
