#include <stdint.h>
#include "huffbook.h"
#include "../bitstream/bitstream.h"

struct DecResult {
    uint16_t len;
    int16_t* array;
};

void huff_encode(struct HuffCodeBook* book, struct BitStream* bs, struct DecResult* result);
void huff_decode(struct HuffCodeBook* book, struct BitStream* bs, struct DecResult* result);
