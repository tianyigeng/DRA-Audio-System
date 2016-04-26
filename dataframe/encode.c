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
static void     init(struct bit_stream* bs);  /* set up essentials */
static void     clear();                      /* release memory */

static void     SetUpConfig();                /* set up configuration variables with the input frame */

static inline INT   Ceil(INT x, INT y);
static inline INT   Max(INT x, INT y);

static void     Pack(uint16_t bits, INT val); /* pack bits into bit_stream */

static void     PackFrame();                  /* encode a PackFrame */
static void     PackFrameHeader();            /* encode PackFrame header */
static void     PackWinSequence();
static void     PackCodeBooks();
static void     PackQIndex();
static void     PackQStepIndex();
static void     PackSumDff();
static void     PackJicScale();
static void     PackBitPad();
static void     PackAuxiliaryData();

static void     ConstructQuantUnit();
static void     Quantilize();

/* End of forward decls */

/* bitstream which we are writing data to */
struct bit_stream* bs;

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
    init(bs);

    for (uint32_t i = 0; i < after_mdct->size; i++) {
        struct vector* frame_to_enc = (struct vector*) vector_object_at(after_mdct, i);
        for (uint32_t j = 0; j < MAX_INDEX; j++) {
            afFreqVals[j] = vector_double_at(frame_to_enc, j);
        }
        PackFrame();
        // bitstream_print(bs);
        printf("exitting..\n");
        return;
    }

    clear();
}

static void SetUpConfig() {
    nFrmHeaderType   = 0;   /* standard frame */
    nNumWord         = 200;
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

    anHSNumBands[0]    = 1;       /* 1 band */
    mnHSBandEdge[0][0] = 256;     /* size of band is 256 * 4 = 1024 */
    mnHS[0][0]         = 9;       /* temporily use the largest codebook */

    anNumBlocksPerFrmPerCluster[0] = 1;    /* 1 block/frame in the cluster */

    anClusterBin0[0]   = 0;       /* the first cluster begin at 0 */

    ConstructQuantUnit();
    Quantilize();

    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            mnQStepIndex[nCluster][nBand] = 95;
        }
    }

    // printf("Indices before quant: \n");
    // for (nBin = 0; nBin < MAX_INDEX; nBin++) {
    //     printf("%.2f ", afFreqVals[nBin]);
    // }
    printf("Indices after quant: \n");
    for (nBin = 0; nBin < MAX_INDEX; nBin++) {
        printf("%d ", anQIndex[nBin]);
    }
}

static void PackFrame() {

    /* First, set up configuration variables */
    SetUpConfig();

    /* Pack nSyncWord */
    Pack(16, nSyncWord);

    /* Pack Frame Headers */
    PackFrameHeader();

    /* Normal channels */
    for (nCh = 0; nCh < nNumNormalCh; nCh++) {
        PackWinSequence();
        PackCodeBooks();
        PackQIndex();
        PackQStepIndex();
    }

    /* sum-diff-coding */
    if (bUseSumDiff == TRUE && (nCh % 2) == 1) {
        assert(0); /* unsupported now */

        PackSumDff();
    }

    /* joint-intensity-coding */
    if (bUseJIC == TRUE && nCh > 0) {
        assert(0); /* unsupported now */

        PackJicScale();
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

        PackCodeBooks();
        PackQIndex();
        PackQStepIndex();
    }

    /* bit pad */
    PackBitPad();

    /* user defined axuiliary data */
    PackAuxiliaryData();
}

static void PackFrameHeader() {
    /* PackFrame header type */
    Pack(1, nFrmHeaderType);

    /* num of words */
    if (nFrmHeaderType == 0) {
        Pack(10, nNumWord);
    } else {
        assert(0); /* unsupported now */
        Pack(13, nNumWord);
    }

    /* blocks per PackFrame */
    Pack(2, log2(nNumBlocksPerFrm));

    /* sample rate */
    Pack(4, nSampleRateIndex);

    /* num normal channel & lfe channel */
    if (nFrmHeaderType == 0) {
        Pack(3, nNumNormalCh - 1);
        Pack(1, nNumLfeCh);
    } else {

        assert(0); /* unsupported now */

        Pack(6, nNumNormalCh - 1);
        Pack(2, nNumLfeCh);
    }

    /* aux data */
    Pack(1, bAuxData);

    /* use sumdiff / use jic */
    if (nFrmHeaderType == 0) {
        if (nNumNormalCh > 1) {

            assert(0); /* unsupported now */

            Pack(1, bUseSumDiff);
            Pack(1, bUseJIC);
        }

        if (bUseJIC == 1) {

            assert(0); /* unsupported now */

            Pack(5, nJicCb - 1);
        }
    } else {
        assert(0); /* unsupported now */
    }
}

static void PackCodeBooks() {
    printf("packing code books...\n");
    assert(nNumCluster == 1); /* unsupported now */

    /* pack scope of books */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        Pack(5, anHSNumBands[nCluster]);
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
            Pack(4, mnHS[nCluster][0]);
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

static void PackQStepIndex() {
    printf("packing QStepIndex...\n");
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

static void PackQIndex() {
    printf("packing QIndex...\n");
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
                printf("selecting book %d\n", nHSelect);
                pQIndexBook = QIndexBooks[nHSelect];

                if (nHSelect == 8) {
                    printf("successfully selected book");
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
                                Pack(nQuotientWidth, abs(anQIndex[nBin]) / nMaxIndex - 1); /* the quotient */
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
                                Pack(1, 1);
                            } else {
                                Pack(1, 0);
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

static void PackWinSequence() {
    printf("packing WinSeq...\n");
    /* we only pack the info for the first channel, all others shall be the same */

    Pack(4, nWinTypeCurrent);

    if (nWinTypeCurrent > 8) {
        /* if current window is a short window */

        assert(0); /* unsupported now */

        Pack(2, nNumCluster - 1);
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
                printf("%d, %.6f\n", nBin, nStepSize);
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

static void PackAuxiliaryData() {
    /* Intentionally blank */
}

static void PackSumDff() {
    /* Intentionally blank */
}

static void PackJicScale() {
    /* Intentionally blank */
}

static void PackBitPad() {
    /* Intentionally blank */
}

static void Pack(uint16_t bits, INT val) {
    bitstream_push(bs, val, bits);
}

static void init(struct bit_stream* target_bs) {
    bs = target_bs;
}

static void clear() {
    bs = NULL;
}

#endif
