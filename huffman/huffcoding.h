#include <stdint.h>
#include "huffbook.h"
#include "../bitstream/bitstream.h"
#include "../util/vector.h"

void huff_decode(struct HuffCodeBook* book, struct BitStream* src, struct vector* result);
void huff_encode(struct HuffCodeBook* book, struct vector* src, struct BitStream* result);
