#ifndef __DECODE_C_
#define __DECODE_C_

#include <stdint.h>

typedef int32_t   INT;
typedef int32_t*  AINT; /* int array */
typedef int32_t** MINT; /* 2-d int array */
typedef uint8_t   BOOL;
typedef enum window {
    WIN_LONG_LONG2LONG, 
    WIN_SHORT_SHORT2SHORT
} WINTYPE;

const BOOL TRUE  = 1;
const BOOL FALSE = 0;

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

static AINT alloc_aint(uint32_t sz, uint32_t len);                 /* helper function to alloc arrays */
static void free_aint(AINT arr);
static MINT alloc_mint(uint32_t sz, uint32_t xlen, uint32_t ylen); /* helper function to alloc 2-d arrays */
static void free_mint(MINT arr, uint32_t xlen);

struct frame_config {
    INT     nNumNormalCh;
    INT     nNumLfeCh;
    BOOL    bUseSumDiff;
    BOOL    bUseJIC;
    INT     nNumBlocksPerFrm;
    INT     nFrmHeaderType;
    INT     nJicCb;
    INT     nNumWord;
    INT     nSampleRateIndex;
    INT     nNumCluster;
    BOOL    bAuxData;
    WINTYPE nWinTypeCurrent;

    AINT    anNumBlocksPerFrmPerCluster;
    AINT    anHSNumBands;

    MINT    mnHS;
    MINT    mnHSBandEdge;
};

/* Use global varibles to make logic clear. */
struct frame_config* config;
struct bit_stream*   bs;

/* decode the bitstream according to dra spec */
void dra_decode(struct bit_stream* bs) {
    init(bs);
    while (Unpack(16) == 0x7FFF) {
        Frame();
    }
    clear();
}

static void Frame(struct bs_iter* bs) {
    FrameHeader(bs);

    INT nCh;

    /* Normal Frames */
    for (nCh = 0; nCh < config->nNumNormalCh; nCh++) {
        UnpackWinSequence(bs);
        UnpackCodeBooks(bs);
        UnpackQIndex(bs);
        UnpackQStepIndex(bs);
    }

    if (bUseSumDiff == TRUE && (nCh % 2) == 1) {
        UnpackSumDff(bs);
    }

    if (bUseJIC == TRUE && nCh > 0) {
        UnpackJicScale(bs);
    }

    /* LFE Frames */
    for (nCh = config->nNumNormalCh; nCh < config->nNumNormalCh + config->nNumLfeCh; nCh++) {
        if (config->nNumBlocksPerFrm == 8) {
            config->nWinTypeCurrent = WIN_LONG_LONG2LONG;
            config->nNumCluster = 1;

            config->anNumBlocksPerFrmPerCluster = (AINT) alloc_array(sizeof(INT), config->nNumCluster);
            config->anNumBlocksPerFrmPerCluster[0] = 1;
        } else {
            config->nWinTypeCurrent = WIN_SHORT_SHORT2SHORT;
            config->nNumCluster = 1;

            config->anNumBlocksPerFrmPerCluster = (AINT) alloc_array(sizeof(INT), config->nNumCluster);
            config->anNumBlocksPerFrmPerCluster[0] = nNumBlocksPerFrm;
        }

        UnpackCodeBooks();
        UnpackQIndex();
        UnpackQStepIndex();
    }

    UnpackBitPad();
    AuxiliaryData();
}

static void FrameHeader() {
    /* frame header type */
    config->nFrmHeaderType = Unpack(1);

    /* num of words */
    if (config->nFrmHeaderType == 0) {
        config->nNumWord = Unpack(10);
    } else {
        config->nNumWord = Unpack(13);
    }

    /* blocks per frame */
    config->nNumBlocksPerFrm = 1 << Unpack(2);

    /* sample rate */
    config->nSampleRateIndex = Unpack(4);

    /* num normal channel & lfe channel */
    if (config->nFrmHeaderType == 0) {
        config->nNumNormalCh = Unpack(3) + 1;
        config->nNumLfeCh = Unpack(1);
    } else {
        config->nNumNormalCh = Unpack(6) + 1;
        config->nNumLfeCh = Unpack(2);
    }

    /* aux data */
    config->bAuxData = Unpack(1);

    /* use sumdiff / use jic */
    if (config->nFrmHeaderType == 0) {
        if (config->nNumNormalCh > 1) {
            bUseSumDiff = Unpack(1);
            bUseJIC = Unpack(1);
        } else {
            config->bUseSumDiff = 0;
            config->bUseJIC = 0;
        }
        if (bUseJIC == 1) {
            config->nJicCb = Unpack(5) + 1;
        } else {
            config->nJicCb = 0;
        }
    } else {
        config->bUseSumDiff = 0;
        config->bUseJIC = 0;
        config->nJicCb = 0;
    }
}

static void UnpackCodeBooks() {
    INT nCluster;
    INT nLast;
    INT nBand;
    INT k;

    /* unpack scope of books */
    for (nCluster = 0; nCluster < config->nNumCluster; nCluster++) {
        anHSNumBands[nCluster] = Unpack(5);
        nLast = 0;
        for (nBand = 0; nBand < anHSNumBands[nCluster]; nBand++) {
            k = HuffDecRecursive(pRunLengthBook) + nLast + 1;
            mnHSBandEdge[nCluster][nBand] = k;
            nLast = k;
        }
    }

    /* unpack indices of code book */
    for (nCluster = 0; nCluster < config->nNumCluster; nCluster++) {
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
    ResetHuffIndex(pQStepBook, 0);
    INT nCluster, nBand;
    for (nCluster = 0; nCluster < config->nNumCluster; nCluster++) {
        for (nBand = 0; nBand < config->anMaxActCb[nCluster]; nBand++) {
            config->mnQStepIndex[nCluster][nBand] = HuffDecDiff(pQStepBook);
        }
    }
}

static void UnpackQIndex() {
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
    config = (struct frame_config*) malloc(sizeof(struct frame_config));
    if (config == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return;
    }
    bs = input_bs;

    /* init arrays */
    config->anNumBlocksPerFrmPerCluster = NULL;
}

static void clear() {
    bs = NULL;

    free_mint(config->mnHSBandEdge, config->nNumCluster);
    free_mint(config->mnHS, config->nNumCluster);

    free_aint(config->anNumBlocksPerFrmPerCluster);
    free_aint(anHSNumBands);

    free(config);
}

static AINT alloc_aint(uint32_t persize, uint32_t len) {
    AINT ret = (AINT) malloc(size * len);
    if (ret == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }
    return ret;
}

static MINT alloc_mint(uint32_t persize, uint32_t xlen, uint32_t ylen) {
    MINT ret = (MINT) malloc(sizeof(AINT) * xlen);
    if (ret == NULL) {
        handle_error(ERROR_FAILURE_ALLOC_MEM);
        return NULL;
    }
    for (uint32_t i = 0; i < ylen; i++) {
        ret[i] = alloc_aint(sizeof(INT), ylen);
    }
    return ret;
}

static void free_aint(AINT arr) {
    free(arr);
}

static void free_mint(MINT arr, uint32_t xlen) {
    for (uint32_t i = 0; i < xlen; i++) {
        free(arr[i]);
    }
    free(arr);
}

#endif
