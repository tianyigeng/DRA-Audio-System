#ifndef __DATAFRAME_H_
#define __DATAFRAME_H_

/* some values shared by the encoder and decoder */

#include <stdint.h>

typedef int32_t   INT;
typedef uint8_t   BOOL;
typedef double    DOUBLE;

const BOOL TRUE  = 1;
const BOOL FALSE = 0;

const INT WIN_LONG_LONG2LONG    = 0;   /* size: 2048 samples */
const INT WIN_LONG_LONG2SHORT   = 1;   /* size: 2048 samples */
const INT WIN_LONG_SHORT2LONG   = 2;   /* size: 2048 samples */
const INT WIN_LONG_SHORT2SHORT  = 3;   /* size: 2048 samples */
const INT WIN_LONG_LONG2BRIEF   = 4;   /* size: 2048 samples */
const INT WIN_LONG_BRIEF2LONG   = 5;   /* size: 2048 samples */
const INT WIN_LONG_BRIEF2BRIEF  = 6;   /* size: 2048 samples */
const INT WIN_LONG_SHORT2BRIEF  = 7;   /* size: 2048 samples */
const INT WIN_LONG_BRIEF2SHORT  = 8;   /* size: 2048 samples */
const INT WIN_SHORT_SHORT2SHORT = 9;   /* size: 256 samples */
const INT WIN_SHORT_SHORT2BRIEF = 10;  /* size: 256 samples */
const INT WIN_SHORT_BRIEF2BRIEF = 11;  /* size: 256 samples */
const INT WIN_SHORT_BRIEF2SHORT = 12;  /* size: 256 samples */

#define MAX_INDEX   1024 /* max indices in a frame */
#define MAX_CLUSTER 4    /* max clusters in a frame */
#define MAX_BAND    32   /* max bands in a cluster */
#define MAX_STEP    116  /* max step values for unit-step */

/* TODO: Accumulate that!!! */
/* minimum nBand defined by Psychoacoustics model, 8kHz sample rate, long window */
INT     pnCBEdge[MAX_BAND] = {26, 27, 27, 28, 30, 32, 34, 37, 41, 46, 53, 62, 74, 89, 109, 133, 162, 14};

DOUBLE  aunStepSize[MAX_STEP];                     /* store step sizes for unit step */

INT     nSyncWord = 0x7FFF;

#endif
