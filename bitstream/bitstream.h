#include <stdint.h>

struct BitStream {
	unsigned char* data;
	uint16_t size;
};

void open_bitstream(struct BitStream* bs, uint16_t size);
struct BitStream* copy_bitstream(struct BitStream* src);
