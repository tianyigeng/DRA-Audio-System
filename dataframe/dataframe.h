#ifndef __DATAFRAME_H_
#define __DATAFRAME_H_

/* some values shared by the encoder and decoder */

#include <stdint.h>
#include "../bitstream/bitstream.h"
#include "../bitstream/bs_iter.h"

typedef int32_t   INT;
typedef uint8_t   BOOL;
typedef double    DOUBLE;

#define TRUE  1
#define FALSE 0

#define WIN_LONG_LONG2LONG    0   /* size: 2048 samples */
#define WIN_LONG_LONG2SHORT   1   /* size: 2048 samples */
#define WIN_LONG_SHORT2LONG   2   /* size: 2048 samples */
#define WIN_LONG_SHORT2SHORT  3   /* size: 2048 samples */
#define WIN_LONG_LONG2BRIEF   4   /* size: 2048 samples */
#define WIN_LONG_BRIEF2LONG   5   /* size: 2048 samples */
#define WIN_LONG_BRIEF2BRIEF  6   /* size: 2048 samples */
#define WIN_LONG_SHORT2BRIEF  7   /* size: 2048 samples */
#define WIN_LONG_BRIEF2SHORT  8   /* size: 2048 samples */
#define WIN_SHORT_SHORT2SHORT 9   /* size: 256 samples */
#define WIN_SHORT_SHORT2BRIEF 10  /* size: 256 samples */
#define WIN_SHORT_BRIEF2BRIEF 11  /* size: 256 samples */
#define WIN_SHORT_BRIEF2SHORT 12  /* size: 256 samples */

#define MAX_INDEX   1024 /* max indices in a frame */
#define MAX_CLUSTER 4    /* max clusters in a frame */
#define MAX_BAND    32   /* max bands in a cluster */
#define MAX_STEP    116  /* max step values for unit-step */

extern INT pnCBEdge[MAX_BAND]; /* minimum nBand defined by Psychoacoustics model, 8kHz sample rate, long window */

extern DOUBLE aunStepSize[MAX_STEP];   /* store step sizes for unit step */

#define nSyncWord 0x7FFF

void dra_encode(struct vector* after_mdct, struct bit_stream* bs);
void dra_decode(struct bit_stream* bs);

#endif
