#ifndef __DECODE_C_
#define __DECODE_C_

#include <stdlib.h>
#include <assert.h>
#include "../huffman/huffbook.h"
#include "../huffman/huffcoding.h"
#include "dataframe.h"

/* Forward decls */
static void     init(struct bit_stream* bs); /* set up essentials */
static void     clear();                     /* release memory */

static INT      Ceil(INT x, INT y);

static uint32_t Unpack(uint16_t bits);       /* unpack bits from bit_stream */

static void     Frame();                     /* decode a frame */
static void     FrameHeader();               /* decode frame header */
static void     UnpackWinSequence();
static void     UnpackCodeBooks();
static void     UnpackQIndex();
static void     UnpackQStepIndex();
static void     UnpackSumDff();
static void     UnpackJicScale();
static void     UnpackBitPad(INT nNumWord);
static void     AuxiliaryData();

static void     ReconstructQuantUnit();
static void     InverseQuant();
/* End of forward decls */

/* an iterator of the input bitstream */
struct bs_iter* iter;

/* configuration variables */
INT     nNumNormalCh, nNumLfeCh;
BOOL    bUseSumDiff, bUseJIC;

INT     nNumBlocksPerFrm;
INT     nFrmHeaderType;
INT     nJicCb;
INT     nNumWord;
INT     nNumCodes;
INT     nDim;
INT     nSampleRateIndex;
INT     nNumCluster;
BOOL    bAuxData;

/* temp variables */
struct huff_codebook* pQIndexBook;

INT     nInitPos;    /* mark the initial position of bs_iter when decoding a frame */

INT     nWinTypeCurrent;

INT     nCluster, nBand, nStart, nEnd, nHSelect, nBin, 
        nMaxIndex, nCtr, nQuotientWidth, nQIndex, nSign, 
        nLast, nCh, k, n, nCb, nMaxBin, nMaxBand, 
        nBin0, nNumBlocks, nQStepSelect;

DOUBLE  nStepSize;

/* one-dimentional arrays */
INT     anNumBlocksPerFrmPerCluster[MAX_CLUSTER];  /* num of blocks/frame in a cluster */
INT     anHSNumBands[MAX_CLUSTER];                 /* num of bands in a cluster */
INT     anClusterBin0[MAX_CLUSTER];                /* first index at every cluster */
INT     anMaxActCb[MAX_CLUSTER];                   /*  */

INT     anQIndex[MAX_INDEX];                       /* indices before inv-unitstep */

DOUBLE  afBinReconst[MAX_INDEX];                   /* store recovered data (before imdct) */

/* two-dimentional arrays */
INT     mnHS[MAX_CLUSTER][MAX_BAND];                /* huffbook index at (nCluster, nBand) */
INT     mnHSBandEdge[MAX_CLUSTER][MAX_BAND];        /* huffbook scope (ending point) at (nCluster, nBand) */
INT     mnQStepIndex[MAX_CLUSTER][MAX_BAND];        /* quan-step at (nCluster, nBand) */

/* decode the bitstream into pre-imdct according to dra spec */
void dra_decode(struct bit_stream* bs, struct vector* pre_imdct) {
    init(bs);

    while (bs_iter_has_next(iter)) {
        nInitPos = bs_iter_pos(iter);
        if (Unpack(16) == nSyncWord) {
            Frame();
            struct vector* frame_dec = vector_init();
            for (uint32_t i = 0; i < MAX_INDEX; i++) {
                vector_push_back_double(frame_dec, afBinReconst[i]);
            }
            vector_push_back_object(pre_imdct, frame_dec);
        } else {
            printf("nInitPos = %d, RUNTHEWRONGWAY\n", nInitPos);
            return;
        }
    }

    clear();
}

static void Frame() {
    /* Load frame headers and fill into configuration variables */
    FrameHeader();

    /* Normal channels */
    for (nCh = 0; nCh < nNumNormalCh; nCh++) {
        UnpackWinSequence();
        UnpackCodeBooks();
        UnpackQIndex();
        UnpackQStepIndex();
    }

    /* sum-diff-coding */
    if (bUseSumDiff == TRUE && (nCh % 2) == 1) {
        assert(0); /* unsupported now */

        UnpackSumDff();
    }

    /* joint-intensity-coding */
    if (bUseJIC == TRUE && nCh > 0) {
        assert(0); /* unsupported now */

        UnpackJicScale();
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

        UnpackCodeBooks();
        UnpackQIndex();
        UnpackQStepIndex();
    }

    /* bit pad */
    UnpackBitPad(nNumWord);

    /* user defined axuiliary data */
    AuxiliaryData();

    /* do the inverse unit step */
    ReconstructQuantUnit();
    InverseQuant();
}

static void FrameHeader() {
    // printf("unpacking frame header...\n");
    /* frame header type */
    nFrmHeaderType = Unpack(1);

    /* num of words */
    if (nFrmHeaderType == 0) {
        nNumWord = Unpack(10);
    } else {

        assert(0); /* unsupported now */

        nNumWord = Unpack(13);
    }

    /* blocks per frame */
    nNumBlocksPerFrm = 1 << Unpack(2);

    /* sample rate */
    nSampleRateIndex = Unpack(4);

    /* num normal channel & lfe channel */
    if (nFrmHeaderType == 0) {
        nNumNormalCh = Unpack(3) + 1;
        nNumLfeCh = Unpack(1);
    } else {

        assert(0); /* unsupported now */

        nNumNormalCh = Unpack(6) + 1;
        nNumLfeCh = Unpack(2);
    }

    /* aux data */
    bAuxData = Unpack(1);

    /* use sumdiff / use jic */
    if (nFrmHeaderType == 0) {
        if (nNumNormalCh > 1) {

            assert(0); /* unsupported now */

            bUseSumDiff = Unpack(1);
            bUseJIC = Unpack(1);
        } else {
            bUseSumDiff = 0;
            bUseJIC = 0;
        }

        if (bUseJIC == 1) {

            assert(0); /* unsupported now */

            nJicCb = Unpack(5) + 1;
        } else {
            nJicCb = 0;
        }
    } else {
        assert(0); /* unsupported now */
        bUseSumDiff = 0;
        bUseJIC = 0;
        nJicCb = 0;
    }
    // printf("nFrmHeaderType: %d\n", nFrmHeaderType);
    // printf("nNumWord: %d\n", nNumWord);
    // printf("nNumBlocksPerFrm: %d\n", nNumBlocksPerFrm);
    // printf("nSampleRateIndex: %d\n", nSampleRateIndex);
    // printf("nNumNormalCh: %d\n", nNumNormalCh);
    // printf("nNumLfeCh: %d\n", nNumLfeCh);
    // printf("bUseSumDiff: %d\n", bUseSumDiff);
    // printf("bUseJIC: %d\n", bUseJIC);
    // printf("nJicCb: %d\n", nJicCb);
}

static void UnpackCodeBooks() {
    // printf("unpacking codebooks...\n");
    assert(nNumCluster == 1); /* unsupported now */

    /* unpack scope of books */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        anHSNumBands[nCluster] = Unpack(5);
        nLast = 0;
        for (nBand = 0; nBand < anHSNumBands[nCluster]; nBand++) {
            k = HuffDecRecursive(pRunLengthBook, iter) + nLast + 1; /* pRunLengthBook = HuffDec2_64x1 / HuffDec3_32x1 */
            mnHSBandEdge[nCluster][nBand] = k;
            nLast = k;
        }
    }
    // printf("anHSNumBands[0]: %d\n", anHSNumBands[0]);
    // printf("mnHSBandEdge[0][0]: %d\n", mnHSBandEdge[0][0]);
    // printf("mnHSBandEdge[0][1]: %d\n", mnHSBandEdge[0][1]);

    /* unpack indices of code book */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        if (anHSNumBands[nCluster] > 0) {
            nLast = Unpack(4);
            mnHS[nCluster][0] = nLast;
            for (nBand = 1; nBand < anHSNumBands[nCluster]; nBand++) {
                k = HuffDec(pHSBook, iter); /* pHSBook = HuffDec4_18x1 / HuffDec5_18x1 */
                printf("k=%d\n", k);
                if (k > 8) {
                    k -= 8;
                } else {
                    k -= 9;
                }
                k += nLast;
                mnHS[nCluster][nBand] = k;
                printf("k=%d\n", k);
                nLast = k;
            }
        }
    }
    // printf("mnHS[0][0]: %d\n", mnHS[0][0]);
    // printf("mnHS[0][1]: %d\n", mnHS[0][1]);
}

static void UnpackQStepIndex() {
    // printf("unpacking QStepIndex...\n");
    assert(nNumCluster == 1); /* unsupported now */

    /* reset state */
    ResetHuffIndex(pQStepBook, 0);
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            mnQStepIndex[nCluster][nBand] = HuffDecDiff(pQStepBook, iter);
        }
    }
}

static void UnpackQIndex() {
    // printf("unpacking QIndex...\n");
    assert(nNumCluster == 1); /* otherwise unsupported now */

    /* reset state */
    ResetHuffIndex(pQuotientWidthBook, 0);

    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        nStart = anClusterBin0[nCluster];
        for (nBand = 0; nBand < anHSNumBands[nCluster]; nBand++) {
            nEnd = anClusterBin0[nCluster] + mnHSBandEdge[nCluster][nBand] * 4;
            nHSelect = mnHS[nCluster][nBand];

            if (nHSelect == 0) {
                /* pad with zero */
                for (nBin = nStart; nBin < nEnd; nBin++) {
                    anQIndex[nBin] = 0;
                }
            } else {
                /* decode with selected code book */
                nHSelect--;
                pQIndexBook = QIndexBooks[nHSelect];
                if (nHSelect == 8) {
                    /* the largest code book */
                    nMaxIndex = GetNumHuffCodes(pQIndexBook) - 1;
                    nCtr = 0;
                    for (nBin = nStart; nBin < nEnd; nBin++) {
                        nQIndex = HuffDec(pQIndexBook, iter);
                        if (nQIndex == nMaxIndex) {
                            nCtr++;
                        }
                        anQIndex[nBin] = nQIndex;
                    }
                    if (nCtr > 0) {
                        nQuotientWidth = HuffDecDiff(pQuotientWidthBook, iter) + 1;
                        for (nBin = nStart; nBin < nEnd; nBin++) {
                            nQIndex = anQIndex[nBin];
                            if (nQIndex == nMaxIndex) {
                                nQIndex *= Unpack(nQuotientWidth) + 1;
                                nQIndex += HuffDec(pQIndexBook, iter);
                                anQIndex[nBin] = nQIndex;
                            }
                        }
                    }
                } else {
                    /* a normal code book */
                    nDim = GetHuffDim(pQIndexBook);
                    if (nDim > 1) {
                        nNumCodes = GetNumHuffCodes(pQIndexBook);
                        for (nBin = nStart; nBin < nEnd; nBin += nDim) {
                            nQIndex = HuffDec(pQIndexBook, iter);
                            for (k = 0; k < nDim; k++) {
                                anQIndex[nBin + k] = nQIndex % nNumCodes;
                                nQIndex = nQIndex / nNumCodes;
                            }
                        }
                    } else {
                        for (nBin = nStart; nBin < nEnd; nBin++) {
                            anQIndex[nBin] = HuffDec(pQIndexBook, iter);
                        }
                    }
                }

                if (GetHuffMidTread(pQIndexBook) == TRUE) {
                    /* mid tread huff codebook, subtract the mean value to get the actual value */
                    nMaxIndex = GetNumHuffCodes(pQIndexBook) / 2;
                    for (nBin = nStart; nBin < nEnd; nBin++) {
                        anQIndex[nBin] -= nMaxIndex;
                    }
                } else {
                    /* non-midtread huff codebook, use an additional bit to decode */
                    for (nBin = nStart; nBin < nEnd; nBin++) {
                        nQIndex = anQIndex[nBin];
                        if (nQIndex != 0) {
                            nSign = Unpack(1);
                            if (nSign == 0) {
                                nQIndex = -nQIndex;
                            }
                        }
                        anQIndex[nBin] = nQIndex;
                    }
                }
            }
            nStart = nEnd;
        }
    }
}

static void UnpackWinSequence() {
    /* record the state for further use */
    static INT Ch0_nWinTypeCurrent;
    static INT Ch0_nNumCluster;
    static INT Ch0_anNumBlocksPerFrmPerCluster[MAX_CLUSTER];

    if (nCh == 0 || (bUseJIC == FALSE && bUseSumDiff == FALSE)) {
        nWinTypeCurrent = Unpack(4);
        if (nWinTypeCurrent > 8) {
            /* if current window is a short window */

            assert(0); /* unsupported now */

            nNumCluster = Unpack(2) + 1;
            if (nNumCluster >= 2) {
                nLast = 0;
                for (nCluster = 0; nCluster < nNumCluster - 1; nCluster++) {
                    k = HuffDec(pClusterBook, iter) + 1;
                    anNumBlocksPerFrmPerCluster[nCluster] = k;
                    nLast += k;
                }
                anNumBlocksPerFrmPerCluster[nCluster] = nNumBlocksPerFrm - nLast;
            } else {
                anNumBlocksPerFrmPerCluster[0] = nNumBlocksPerFrm;
            }
        } else {
            /* if current window is a long window */
            nNumCluster = 1;
            anNumBlocksPerFrmPerCluster[0] = 1;
        }

        if (nCh == 0) {
            /* copy the state information to static variables for further use */
            Ch0_nWinTypeCurrent = nWinTypeCurrent;
            Ch0_nNumCluster = nNumCluster;
            for (n = 0; n < nNumCluster; n++) {
                Ch0_anNumBlocksPerFrmPerCluster[n] = anNumBlocksPerFrmPerCluster[n];
            }
        }
    } else {
        assert(0); /* unsupported now */

        /* copy the info of Ch0 */
        nWinTypeCurrent = Ch0_nWinTypeCurrent;
        nNumCluster = Ch0_nNumCluster;
        for (n = 0; n < nNumCluster; n++) {
            anNumBlocksPerFrmPerCluster[n] = Ch0_anNumBlocksPerFrmPerCluster[n];
        }
    }
}

static void ReconstructQuantUnit() {
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

static void InverseQuant() {

    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        nBin0 = anClusterBin0[nCluster];
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            nNumBlocks = anNumBlocksPerFrmPerCluster[nCluster];
            nStart = nBin0 + nNumBlocks * pnCBEdge[nBand];
            nEnd = nBin0 + nNumBlocks * pnCBEdge[nBand + 1];
            nQStepSelect = mnQStepIndex[nCluster][nBand];
            nStepSize = aunStepSize[nQStepSelect];

            for (nBin = nStart; nBin < nEnd; nBin++) {
                afBinReconst[nBin] = anQIndex[nBin] * nStepSize;
            }
        }
    }
}

static INT Ceil(INT x, INT y) {
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

static void AuxiliaryData() {
    /* Intentionally blank */
}

static void UnpackSumDff() {
    /* Intentionally blank */
}

static void UnpackJicScale() {
    /* Intentionally blank */
}

static void UnpackBitPad(INT nNumWord) {
    Unpack(32 * nNumWord - (bs_iter_pos(iter) - nInitPos));
}

static uint32_t Unpack(uint16_t bits) {
    return bs_iter_unpack(iter, bits);
}

static void init(struct bit_stream* input_bs) {
    iter = bs_iter_init(input_bs);
}

static void clear() {
    if (iter != NULL) {
        bs_iter_destroy(iter);
    }
}

#endif
