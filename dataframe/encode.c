#ifndef __ENCODE_C_
#define __ENCODE_C_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../huffman/huffbook.h"
#include "../huffman/huffcoding.h"
#include "dataframe.h"

/* Forward decls */
static void     SetUpConfig();                /* set up configuration variables with the input frame */

static inline INT   Ceil(INT x, INT y);
static inline INT   Max (INT x, INT y);

static void     Pack(struct bit_stream* bs, uint16_t bits, INT val); /* pack bits into bit_stream */

static void     PackFrame(struct bit_stream* bs);                  /* encode a PackFrame */
static void     PackFrameHeader(struct bit_stream* bs);            /* encode PackFrame header */
static void     PackWinSequence(struct bit_stream* bs);
static void     PackCodeBooks(struct bit_stream* bs);
static void     PackQIndex(struct bit_stream* bs);
static void     PackQStepIndex(struct bit_stream* bs);
static void     PackSumDff(struct bit_stream* bs);
static void     PackJicScale(struct bit_stream* bs);
static void     PackBitPad(struct bit_stream* bs, INT nNumWord);
static void     PackAuxiliaryData(struct bit_stream* bs);

static void     ConstructQuantUnit();
static void     Quantilize();
static INT      CalculateHeaderSize();

/* End of forward decls */

DOUBLE  afFreqVals[MAX_INDEX];                   /* store values (after mdct) to be processed */

/* configuration variables */
INT     nNumNormalCh, nNumLfeCh;
BOOL    bUseSumDiff, bUseJIC;
INT     nNumBlocksPerFrm;
INT     nFrmHeaderType;
INT     nJicCb;
INT     nNumWord;
BOOL    bAuxData;
INT     nSampleRateIndex;

INT     nNumCodes;
INT     nDim;
INT     nNumCluster;

INT     nHeaderSize;    /* size of frame header, will need this to calculate the frame size (nNumWord) */

/* temp variables */
struct huff_codebook* pQIndexBook;

INT     nWinTypeCurrent;

INT     nCluster, nBand, nStart, nEnd, nHSelect, nBin, 
        nMaxIndex, nCtr, nQuotientWidth, nQIndex, nSign, 
        nLast, nCh, k, n, nCb, nMaxBin, nMaxBand, 
        nBin0, nNumBlocks, nQStepSelect;

DOUBLE  nStepSize;

/* one-dimentional arrays */
INT     anNumBlocksPerFrmPerCluster[MAX_CLUSTER];  /* num of blocks/PackFrame in a cluster */
INT     anHSNumBands[MAX_CLUSTER];                 /* num of bands in a cluster */
INT     anClusterBin0[MAX_CLUSTER];                /* first index at every cluster */
INT     anMaxActCb[MAX_CLUSTER];                   /*  */

INT     anQIndex[MAX_INDEX];                       /* indices before inv-unitstep */



/* two-dimentional arrays */
INT     mnHS[MAX_CLUSTER][MAX_BAND];                /* huffbook index at (nCluster, nBand) */
INT     mnHSBandEdge[MAX_CLUSTER][MAX_BAND];        /* huffbook scope (ending point) at (nCluster, nBand) */
INT     mnQStepIndex[MAX_CLUSTER][MAX_BAND];        /* quan-step at (nCluster, nBand) */

/* encode the frequencies into bitstream according to dra spec */
void dra_encode(struct vector* after_mdct, struct bit_stream* bs) {
    for (uint32_t i = 0; i < after_mdct->size; i++) {
        /* process every frame */
        struct vector* frame_to_enc = (struct vector*) vector_object_at(after_mdct, i);
        vector_print_double(frame_to_enc);
        for (uint32_t j = 0; j < MAX_INDEX; j++) {
            afFreqVals[j] = vector_double_at(frame_to_enc, j);
        }

        PackFrame(bs);
        printf("frame %d, bs size: %d\n", i, bitstream_size(bs));

        // bitstream_print(bs);
        return;
    }
}

static void SetUpConfig() {
    nFrmHeaderType   = 0;   /* standard frame */
    nNumBlocksPerFrm = 1;   /* 1 blocks/frame (stable frame) */
    nSampleRateIndex = 0;   /* sample rate @ 8000Hz */
    nNumNormalCh     = 1;   /* 1 normal channels */
    nNumLfeCh        = 0;   /* NO LFE channels */

    bAuxData         = 0;       /* DONOT set auxiliary data */

    bUseSumDiff      = FALSE;   /* DONOT use sum-diff coding */
    bUseJIC          = FALSE;   /* DONOT use joint-density coding */
    nJicCb           = 0;       /* DONOT use JIC */

    nNumCluster      = 1;       /* ONLY one cluster */

    nWinTypeCurrent  = WIN_LONG_LONG2LONG; /* the window used */

    anHSNumBands[0]    = 1;       /* 2 bands */
    mnHSBandEdge[0][0] = 256;       /* size of band is 256 * 4 = 1024 */
    mnHS[0][0]         = 9;       /* temporily use the largest codebook */

    anNumBlocksPerFrmPerCluster[0] = 1;    /* 1 block/frame in the cluster */

    anClusterBin0[0]   = 0;       /* the first cluster begin at 0 */

    ConstructQuantUnit();
    
    /* set up quant indices */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            mnQStepIndex[nCluster][nBand] = 95;
        }
    }

    Quantilize();
}

static void PackFrame(struct bit_stream* bs) {

    /* First, set up configuration variables */
    SetUpConfig();

    /* store all except header, in order to calculate nNumWord */
    struct bit_stream* bsCurrentFrame = bitstream_init();

    /* Normal channels */
    for (nCh = 0; nCh < nNumNormalCh; nCh++) {
        PackWinSequence(bsCurrentFrame);
        PackCodeBooks(bsCurrentFrame);
        PackQIndex(bsCurrentFrame);
        PackQStepIndex(bsCurrentFrame);
    }

    /* sum-diff-coding */
    if (bUseSumDiff == TRUE && (nCh % 2) == 1) {
        assert(0); /* unsupported now */

        PackSumDff(bsCurrentFrame);
    }

    /* joint-intensity-coding */
    if (bUseJIC == TRUE && nCh > 0) {
        assert(0); /* unsupported now */

        PackJicScale(bsCurrentFrame);
    }

    /* LFE (low freq enhancement) channels */
    for (nCh = nNumNormalCh; nCh < nNumNormalCh + nNumLfeCh; nCh++) {
        assert(0); /* unsupported now */

        if (nNumBlocksPerFrm == 8) {
            nWinTypeCurrent = WIN_LONG_LONG2LONG;
            nNumCluster = 1;
            anNumBlocksPerFrmPerCluster[0] = 1;
        } else {
            nWinTypeCurrent = WIN_SHORT_SHORT2SHORT;
            nNumCluster = 1;
            anNumBlocksPerFrmPerCluster[0] = nNumBlocksPerFrm;
        }

        PackCodeBooks(bsCurrentFrame);
        PackQIndex(bsCurrentFrame);
        PackQStepIndex(bsCurrentFrame);
    }

    
    /* after packing other bits, we now know how many bits this frame will have */
    nHeaderSize = CalculateHeaderSize();
    nNumWord = Ceil(16 + nHeaderSize + bitstream_size(bsCurrentFrame), 32);

    /* Pack nSyncWord to the target bitstream */
    Pack(bs, 16, nSyncWord);
    /* Pack Frame Headers to the target bitstream */
    PackFrameHeader(bs);

    bitstream_append(bs, bsCurrentFrame);

    /* bit pad */
    PackBitPad(bs, 32 * nNumWord - 16 - nHeaderSize - bitstream_size(bsCurrentFrame));

    /* user defined axuiliary data */
    PackAuxiliaryData(bsCurrentFrame);

    /* release memory */
    bitstream_destroy(bsCurrentFrame);
}

static INT CalculateHeaderSize() {
    INT nHeaderSize = 0;
    /* PackFrame header type */
    nHeaderSize += 1; // Pack(1, nFrmHeaderType);

    /* num of words */
    if (nFrmHeaderType == 0) {
        nHeaderSize += 10; // Pack(10, nNumWord);
    } else {
        assert(0); /* unsupported now */
        nHeaderSize += 13; // Pack(13, nNumWord);
    }

    /* blocks per PackFrame */
    nHeaderSize += 2; // Pack(2, log2(nNumBlocksPerFrm));

    /* sample rate */
    nHeaderSize += 4; // Pack(4, nSampleRateIndex);

    /* num normal channel & lfe channel */
    if (nFrmHeaderType == 0) {
        nHeaderSize += 3; // Pack(3, nNumNormalCh - 1);
        nHeaderSize += 1; // Pack(1, nNumLfeCh);
    } else {

        assert(0); /* unsupported now */

        nHeaderSize += 6; // Pack(6, nNumNormalCh - 1);
        nHeaderSize += 2; // Pack(2, nNumLfeCh);
    }

    /* aux data */
    nHeaderSize += 1; // Pack(1, bAuxData);

    /* use sumdiff / use jic */
    if (nFrmHeaderType == 0) {
        if (nNumNormalCh > 1) {

            assert(0); /* unsupported now */

            nHeaderSize += 1; // Pack(1, bUseSumDiff);
            nHeaderSize += 1; // Pack(1, bUseJIC);
        }

        if (bUseJIC == 1) {

            assert(0); /* unsupported now */

            nHeaderSize += 5; // Pack(5, nJicCb - 1);
        }
    } else {
        assert(0); /* unsupported now */
    }

    return nHeaderSize;
}

static void PackFrameHeader(struct bit_stream* bs) {
    /* PackFrame header type */
    Pack(bs, 1, nFrmHeaderType);

    /* num of words */
    if (nFrmHeaderType == 0) {
        Pack(bs, 10, nNumWord);
    } else {
        assert(0); /* unsupported now */
        Pack(bs, 13, nNumWord);
    }

    /* blocks per PackFrame */
    Pack(bs, 2, log2(nNumBlocksPerFrm));

    /* sample rate */
    Pack(bs, 4, nSampleRateIndex);

    /* num normal channel & lfe channel */
    if (nFrmHeaderType == 0) {
        Pack(bs, 3, nNumNormalCh - 1);
        Pack(bs, 1, nNumLfeCh);
    } else {

        assert(0); /* unsupported now */

        Pack(bs, 6, nNumNormalCh - 1);
        Pack(bs, 2, nNumLfeCh);
    }

    /* aux data */
    Pack(bs, 1, bAuxData);

    /* use sumdiff / use jic */
    if (nFrmHeaderType == 0) {
        if (nNumNormalCh > 1) {

            assert(0); /* unsupported now */

            Pack(bs, 1, bUseSumDiff);
            Pack(bs, 1, bUseJIC);
        }

        if (bUseJIC == 1) {

            assert(0); /* unsupported now */

            Pack(bs, 5, nJicCb - 1);
        }
    } else {
        assert(0); /* unsupported now */
    }
}

static void PackCodeBooks(struct bit_stream* bs) {
    // printf("packing code books...\n");
    assert(nNumCluster == 1); /* unsupported now */

    /* pack scope of books */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        Pack(bs, 5, anHSNumBands[nCluster]);
        nLast = 0;
        for (nBand = 0; nBand < anHSNumBands[nCluster]; nBand++) {
            /* pRunLengthBook = HuffDec2_64x1 / HuffDec3_32x1 */
            HuffEncRecursive(pRunLengthBook, bs, mnHSBandEdge[nCluster][nBand] - nLast - 1);
            nLast = mnHSBandEdge[nCluster][nBand];
        }
    }

    /* pack indices of code book */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        if (anHSNumBands[nCluster] > 0) {
            Pack(bs, 4, mnHS[nCluster][0]);
            for (nBand = 1; nBand < anHSNumBands[nCluster]; nBand++) {
                INT nDiff = mnHS[nCluster][nBand] - mnHS[nCluster][nBand - 1];
                if (nDiff > 0) {
                    /* pHSBook = HuffDec4_18x1 or HuffDec5_18x1 */
                    HuffEnc(pHSBook, bs, nDiff + 8);
                } else {
                    HuffEnc(pHSBook, bs, nDiff + 9);
                }
            }
        }
    }
}

static void PackQStepIndex(struct bit_stream* bs) {
    // printf("packing QStepIndex...\n");
    assert(nNumCluster == 1); /* otherwise unsupported now */

    /* reset state */
    ResetHuffIndex(pQStepBook, 0);

    /* use HuffEncDiff to put QStep indices into bitstream */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            HuffEncDiff(pQStepBook, bs, mnQStepIndex[nCluster][nBand]);
        }
    }
}

static void PackQIndex(struct bit_stream* bs) {
    // printf("packing QIndex...\n");
    assert(nNumCluster == 1); /* otherwise unsupported now */

    /* reset state */
    ResetHuffIndex(pQuotientWidthBook, 0);

    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        nStart = anClusterBin0[nCluster];
        for (nBand = 0; nBand < anHSNumBands[nCluster]; nBand++) {
            nEnd = anClusterBin0[nCluster] + mnHSBandEdge[nCluster][nBand] * 4;
            nHSelect = mnHS[nCluster][nBand];

            if (nHSelect == 0) {
                /* no need to put anything into the bitstream, pad with zero */

                continue; /* go to next nBand */

            } else {
                /* decode with selected code book */
                nHSelect--;
                // printf("selecting book %d\n", nHSelect);
                pQIndexBook = QIndexBooks[nHSelect];

                if (nHSelect == 8) {
                    // printf("successfully selected book");
                    /* the largest code book, handle overflow */
                    nMaxIndex = GetNumHuffCodes(pQIndexBook) - 1;

                    nCtr = 0; /* record occurance of overflow */

                    nQuotientWidth = 0; /* maximum quotient width, used by subsequent code. */

                    for (nBin = nStart; nBin < nEnd; nBin++) {
                        // printf("working on sample %d\n", nBin);
                        if (abs(anQIndex[nBin]) >= nMaxIndex) {
                            /* overflow occurs */
                            nQuotientWidth = Max(nQuotientWidth, log2(abs(anQIndex[nBin]) / nMaxIndex));
                            nCtr++;
                            HuffEnc(pQIndexBook, bs, nMaxIndex);
                        } else {
                            HuffEnc(pQIndexBook, bs, abs(anQIndex[nBin]));
                        }
                    }

                    /* if some sort of overflow occurs, hanle that */
                    if (nCtr > 0) {
                        HuffEncDiff(pQuotientWidthBook, bs, nQuotientWidth - 1);
                        for (nBin = nStart; nBin < nEnd; nBin++) {
                            if (abs(anQIndex[nBin]) >= nMaxIndex) {
                                /* overflow happened here */
                                Pack(bs, nQuotientWidth, abs(anQIndex[nBin]) / nMaxIndex - 1); /* the quotient */
                                HuffEnc(pQIndexBook, bs, abs(anQIndex[nBin]) % nMaxIndex); /* the remainder */
                            }
                        }
                    }

                    /* deal with the sign (+/-) */
                    /* non-midtread huff codebook, use an additional bit to decode */
                    for (nBin = nStart; nBin < nEnd; nBin++) {
                        if (anQIndex[nBin] != 0) {
                            /* deal with the sign */
                            if (anQIndex[nBin] > 0) {
                                Pack(bs, 1, 1);
                            } else {
                                Pack(bs, 1, 0);
                            }
                        }
                    }

                } else {

                    /* a normal code book, use standard way to decode it */
                    nMaxIndex = GetNumHuffCodes(pQIndexBook) / 2; /* used to resolve sign issues */
                    nDim = GetHuffDim(pQIndexBook);

                    if (nDim > 1) {
                        /* multi-dimentional codebooks */
                        nNumCodes = GetNumHuffCodes(pQIndexBook);
                        for (nBin = nStart; nBin < nEnd; nBin += nDim) {
                            nQIndex = 0;
                            for (k = 0; k < nDim; k++) {
                                anQIndex[nBin + k] = nQIndex % nNumCodes;
                                nQIndex = nQIndex / nNumCodes;
                                nQIndex += (anQIndex[nBin + k] + nMaxIndex) * pow(nNumCodes, k);
                            }
                            HuffEnc(pQIndexBook, bs, nQIndex);
                        }
                    } else {
                        /* one-dimentional codebook */
                        for (nBin = nStart; nBin < nEnd; nBin++) {
                            /* deal with the sign (+/-) */
                            HuffEnc(pQIndexBook, bs, anQIndex[nBin] + nMaxIndex);
                        }
                    }

                } // if (nHSelect == 8)
            }
            nStart = nEnd;
        }
    }
}

static void PackWinSequence(struct bit_stream* bs) {
    // printf("packing WinSeq...\n");
    /* we only pack the info for the first channel, all others shall be the same */

    Pack(bs, 4, nWinTypeCurrent);

    if (nWinTypeCurrent > 8) {
        /* if current window is a short window */

        assert(0); /* unsupported now */

        Pack(bs, 2, nNumCluster - 1);
        if (nNumCluster >= 2) {
            for (nCluster = 0; nCluster < nNumCluster - 1; nCluster++) {
                HuffEnc(pClusterBook, bs, anNumBlocksPerFrmPerCluster[nCluster] - 1);
            }
        }
    }
}

static void ConstructQuantUnit() {
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        nMaxBand = anHSNumBands[nCluster];
        nMaxBin = mnHSBandEdge[nCluster][nMaxBand - 1] * 4;
        nMaxBin = Ceil(nMaxBin, anNumBlocksPerFrmPerCluster[nCluster]);
        nCb = 0;
        while (pnCBEdge[nCb] < nMaxBin) {
            nCb++;
        }
        anMaxActCb[nCluster] = nCb;
    }
}

static void Quantilize() {
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        nBin0 = anClusterBin0[nCluster];
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            nNumBlocks = anNumBlocksPerFrmPerCluster[nCluster];
            nStart = nBin0 + nNumBlocks * pnCBEdge[nBand];
            nEnd = nBin0 + nNumBlocks * pnCBEdge[nBand + 1];
            nQStepSelect = mnQStepIndex[nCluster][nBand];
            nStepSize = aunStepSize[nQStepSelect];
            for (nBin = nStart; nBin < nEnd; nBin++) {
                anQIndex[nBin] = afFreqVals[nBin] / nStepSize;
            }
        }
    }
}

static inline INT Ceil(INT x, INT y) {
    // return the min t where t >= x/y
    if (y == 0) {
        handle_error(ERROR_INVALID_ARGV);
        return -1;
    }
    
    if (x % y == 0) {
        return x / y;
    }
    return x / y + 1;
}

static inline INT Max(INT x, INT y) {
    return x > y ? x : y;
}

static void PackAuxiliaryData(struct bit_stream* bs) {
    /* Intentionally blank */
}

static void PackSumDff(struct bit_stream* bs) {
    /* Intentionally blank */
}

static void PackJicScale(struct bit_stream* bs) {
    /* Intentionally blank */
}

static void PackBitPad(struct bit_stream* bs, INT nNumToPad) {
    // printf("n=%d padded\n", nNumToPad);
    /* pad unused bits with 1 */
    Pack(bs, nNumToPad, 0xFFFFFFFF);
}

static void Pack(struct bit_stream* bs, uint16_t bits, INT val) {
    bitstream_push(bs, val, bits);
}

#endif
