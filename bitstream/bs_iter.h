#ifndef __BS_ITER_H_
#define __BS_ITER_H_

#include "bitstream.h"

/* an iterator of bit_stream, for the purpose of unpacking/huffdec in dra-decoder */
struct bs_iter {
    struct bit_stream* bs;
    uint32_t pos;
};

struct bs_iter* bs_iter_init(struct bit_stream* bs);
void bs_iter_destroy(struct bs_iter* iter);

uint32_t bs_iter_unpack(struct bs_iter* iter, uint16_t bits);

uint8_t bs_iter_has_next(struct bs_iter* iter);

uint32_t bs_iter_pos(struct bs_iter* iter);

#endif
