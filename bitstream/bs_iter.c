#ifndef __BS_ITER_C_
#define __BS_ITER_C_

#include "../util/errors.h"
#include "bs_iter.h"
#include <stdlib.h>

// struct bs_iter {
//     bit_stream* bs;
//     uint32_t pos;
// };

static uint32_t bs_iter_unpack_1bit(struct bs_iter* iter);

struct bs_iter* bs_iter_init(struct bit_stream* in_bs) {
    struct bs_iter* ret = (struct bs_iter*) malloc(sizeof(struct bs_iter));
    if (ret == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }
    ret->bs = in_bs;
    ret->pos = 0;
    return ret;
}

void bs_iter_destroy(struct bs_iter* iter) {
    free(iter);
}

uint32_t bs_iter_unpack(struct bs_iter* iter, uint16_t bits) {
    if (bits > 32) {
        handle_error(ERROR_INVALID_ARGV);
        return -1;
    }
    
    uint32_t ret = 0;
    for (uint32_t i = 0; i < bits; i++) {
        ret |= (bs_iter_unpack_1bit(iter) << (bits - 1 - i));
    }
    return ret;
}

static uint32_t bs_iter_unpack_1bit(struct bs_iter* iter) {
    if (iter->pos >= bitstream_size(iter->bs)) {
        handle_error(ERROR_INDEX_OUT_OF_BOUND);
        return -1;
    }
    uint32_t ret = 0;
    ret |= bitstream_bit_at(iter->bs, iter->pos);
    iter->pos++;
    return ret;
}

uint8_t bs_iter_has_next(struct bs_iter* iter) {
    return iter->pos < bitstream_size(iter->bs);
}

uint32_t bs_iter_pos(struct bs_iter* iter) {
    return iter->pos;
}

#endif
