#ifndef __DECODE_C_
#define __DECODE_C_

#include <stdint.h>

typedef int32_t   INT;
typedef uint8_t   BOOL;

const BOOL TRUE  = 1;
const BOOL FALSE = 0;

const INT WIN_LONG_LONG2LONG    = 0;
const INT WIN_LONG_LONG2SHORT   = 1;
const INT WIN_LONG_SHORT2LONG   = 2;
const INT WIN_LONG_SHORT2SHORT  = 3;
const INT WIN_LONG_LONG2BRIEF   = 4;
const INT WIN_LONG_BRIEF2LONG   = 5;
const INT WIN_LONG_BRIEF2BRIEF  = 6;
const INT WIN_LONG_SHORT2BRIEF  = 7;
const INT WIN_LONG_BRIEF2SHORT  = 8;
const INT WIN_SHORT_SHORT2SHORT = 9;
const INT WIN_SHORT_SHORT2BRIEF = 10;
const INT WIN_SHORT_BRIEF2BRIEF = 11;
const INT WIN_SHORT_BRIEF2BRIEF = 12;

/*
 *  Forward decls
 */
static void     init(struct bit_stream* bs); /* set up essentials */
static void     clear();                     /* release memory */
static uint32_t Unpack(uint32_t bits);       /* unpack bits from bit_stream */
static void     Frame();                     /* decode a frame */
static void     FrameHeader();               /* decode frame header */
static void     UnpackWinSequence();
static void     UnpackCodeBooks();
static void     UnpackQIndex();
static void     UnpackQStepIndex();
static void     UnpackSumDff();
static void     UnpackJicScale();
static void     UnpackBitPad();
static void     AuxiliaryData();

#define MAX_INDEX 1024
#define MAX_CLUSTER 4
#define MAX_BAND    32

#define MAX_BOOK 9

/* configuration variables */

INT     nNumNormalCh, nNumLfeCh;
BOOL    bUseSumDiff, bUseJIC;

INT     nNumBlocksPerFrm;
INT     nFrmHeaderType;
INT     nJicCb;
INT     nNumWord;
INT     nSampleRateIndex;
INT     nNumCluster;
BOOL    bAuxData;

/* temp variables */
INT     nWinTypeCurrent;

INT     nCluster, nBand, nStart, nEnd, nHSelect, nBin, 
        nMaxIndex, nCtr, nQuotientWidth, nQIndex, nSign, 
        nLast, nCh, k;

/* one-dimentional arrays */
INT     anNumBlocksPerFrmPerCluster[MAX_CLUSTER];  /* num of blocks/frame in a cluster */
INT     anHSNumBands[MAX_CLUSTER];                 /* num of bands in a cluster */
INT     anClusterBin0[MAX_CLUSTER];                /* first index at every cluster */

INT     anQIndex[MAX_INDEX];                       /* indices before inv-unitstep */

/* two-dimentional arrays */
INT     mnHS[MAX_CLUSTER][MAX_BAND];                /* huffbook index at (nCluster, nBand) */
INT     mnHSBandEdge[MAX_CLUSTER][MAX_BAND];        /* huffbook scope at (nCluster, nBand) */
INT     mnQStepIndex[MAX_CLUSTER][MAX_BAND];        /* quan-step at (nCluster, nBand) */

/* Use global varibles to make logic clear. */
struct bit_stream*   bs;

/* decode the bitstream according to dra spec */
void dra_decode(struct bit_stream* bs) {
    init(bs);
    while (Unpack(16) == 0x7FFF) {
        Frame();
    }
    clear();
}

static void Frame() {
    /* Load frame headers and fill into configuration variables */
    FrameHeader();

    /* Normal Frames */
    for (nCh = 0; nCh < nNumNormalCh; nCh++) {
        UnpackWinSequence();
        UnpackCodeBooks();
        UnpackQIndex();
        UnpackQStepIndex();
    }

    /* sum-diff-coding */
    if (bUseSumDiff == TRUE && (nCh % 2) == 1) {
        assert(0); /* unused now */

        UnpackSumDff();
    }

    /* joint-intensity-coding */
    if (bUseJIC == TRUE && nCh > 0) {
        assert(0); /* unused now */

        UnpackJicScale();
    }

    /* LFE (low freq enhancement) Frames */
    for (nCh = nNumNormalCh; nCh < nNumNormalCh + nNumLfeCh; nCh++) {
        assert(0); /* unused now */

        if (nNumBlocksPerFrm == 8) {
            nWinTypeCurrent = WIN_LONG_LONG2LONG;
            nNumCluster = 1;

            anNumBlocksPerFrmPerCluster = (AINT) alloc_array(sizeof(INT), nNumCluster);
            anNumBlocksPerFrmPerCluster[0] = 1;
        } else {
            nWinTypeCurrent = WIN_SHORT_SHORT2SHORT;
            nNumCluster = 1;

            anNumBlocksPerFrmPerCluster = (AINT) alloc_array(sizeof(INT), nNumCluster);
            anNumBlocksPerFrmPerCluster[0] = nNumBlocksPerFrm;
        }

        UnpackCodeBooks();
        UnpackQIndex();
        UnpackQStepIndex();
    }

    /* bit pad */
    UnpackBitPad();

    /* user defined axuiliary data */
    AuxiliaryData();
}

static void FrameHeader() {
    /* frame header type */
    nFrmHeaderType = Unpack(1);

    /* num of words */
    if (nFrmHeaderType == 0) {
        nNumWord = Unpack(10);
    } else {
        assert(0); /* unused now */
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
        assert(0); /* unused now */
        nNumNormalCh = Unpack(6) + 1;
        nNumLfeCh = Unpack(2);
    }

    /* aux data */
    bAuxData = Unpack(1);

    /* use sumdiff / use jic */
    if (nFrmHeaderType == 0) {
        if (nNumNormalCh > 1) {
            assert(0); /* unused now */
            bUseSumDiff = Unpack(1);
            bUseJIC = Unpack(1);
        } else {
            bUseSumDiff = 0;
            bUseJIC = 0;
        }

        if (bUseJIC == 1) {
            assert(0); /* unused now */
            nJicCb = Unpack(5) + 1;
        } else {
            nJicCb = 0;
        }
    } else {
        assert(0); /* unused now */
        bUseSumDiff = 0;
        bUseJIC = 0;
        nJicCb = 0;
    }
}

static void UnpackCodeBooks() {
    assert(nNumCluster == 1); /* unused now */

    /* unpack scope of books */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        anHSNumBands[nCluster] = Unpack(5);
        nLast = 0;
        for (nBand = 0; nBand < anHSNumBands[nCluster]; nBand++) {
            k = HuffDecRecursive(pRunLengthBook) + nLast + 1;
            mnHSBandEdge[nCluster][nBand] = k;
            nLast = k;
        }
    }

    /* unpack indices of code book */
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        if (anHSNumBands[nCluster] > 0) {
            nLast = Unpack(4);
            mnHS[nCluster][0] = nLast;
            for (nBand = 1; nBand < anHSNumBands[nCluster]; nBand++) {
                k = HuffDecode(pHSBook);
                if (k > 8) {
                    k -= 8;
                } else {
                    k -= 9;
                }
                k += nLast;
                mnHS[nCluster][nBand] = k;
                nLast = k;
            }
        }
    }
}

static void UnpackQStepIndex() {
    assert(nNumCluster == 1); /* unused now */

    /* reset state */
    ResetHuffIndex(pQStepBook, 0);
    for (nCluster = 0; nCluster < nNumCluster; nCluster++) {
        for (nBand = 0; nBand < anMaxActCb[nCluster]; nBand++) {
            mnQStepIndex[nCluster][nBand] = HuffDecDiff(pQStepBook);
        }
    }
}

static void UnpackQIndex() {
    assert(nNumCluster == 1); /* unused now */

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
                        nQIndex = HuffDec(pQIndexBook);
                        if (nQIndex == nMaxIndex) {
                            nCtr++;
                        }
                        anQIndex[nBin] = nQIndex;
                    }
                    if (nCtr > 0) {
                        nQuotientWidth = HuffDecDiff(pQuotientWidthBook) + 1;
                        for (nBin = nStart; nBin < nEnd; nBin++) {
                            nQIndex = anQIndex[nBin];
                            if (nQIndex == nMaxIndex) {
                                nQIndex *= Unpack(nQuotientWidth) + 1;
                                nQIndex += HuffDec(pQIndexBook);
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
                            nQIndex = HuffDec(pQIndexBook);
                            for (k = 0; k < nDim; k++) {
                                anQIndex[nBin + k] = nQIndex % nNumCodes;
                                nQIndex = nQIndex / nNumCodes;
                            }
                        }
                    } else {
                        for (nBin = nStart; nBin < nEnd; nBin++) {
                            anQIndex[nBin] = HuffDec(pQIndexBook);
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
    if (nCh == 0 || bUseJIC == FALSE && bUseSumDiff == FALSE) {
        nWinTypeCurrent = Unpack(4);
        if (nWinTypeCurrent != ANY_LONG_WIN) {

            assert(0); /* unused now */

            nNumCluster = Unpack(2) + 1;
            if (nNumCluster >= 2) {
                nLast = 0;
                for (nCluster = 0; nCluster < nNumCluster - 1; nCluster++) {
                    k = HuffDec(pClusterBook) + 1;
                    anNumBlocksPerFrmPerCluster[nCluster] = k;
                    nLast += k;
                }
                anNumBlocksPerFrmPerCluster[nCluster] = nNumBlocksPerFrm - nLast;
            } else {
                anNumBlocksPerFrmPerCluster[0] = nNumBlocksPerFrm;
            }
        } else {
            nNumCluster = 1;
            anNumBlocksPerFrmPerCluster[0] = 1;
        }
    } else {
        assert(0); /* unused now */
        
        nWinTypeCurrent = Ch0.nWinTypeCurrent;
        nNumCluster = Ch0.nNumCluster;
        for (n = 0; n < nNumCluster; n++) {
            anNumBlocksPerFrmPerCluster[n] = Ch0.anNumBlocksPerFrmPerCluster[n];
        }
    }
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

static void UnpackBitPad() {
    /* Intentionally blank */
}

static uint32_t Unpack(uint32_t bits) {
    return 0;
}

static void init(struct bit_stream* input_bs) {
    bs = input_bs;
}

static void clear() {
    bs = NULL;
}

#endif
