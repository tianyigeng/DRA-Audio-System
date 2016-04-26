import re

def main():
    header = """
#ifndef __HUFFBOOK_C_
#define __HUFFBOOK_C_
/*
 *   huffbook.c
 *   This is an auto-generated file, DONOT edit. 
 */
#include "huffbook.h"
    """
    print header
    # dotc = open("huffman/huffbook.c", "w")
    # doth = open("huffman/huffbook.c", "w")
    with open("configure/hufftable.txt") as f:
        for i, book in enumerate(f.read().split("\n\n")):
            name, incr, code, indx = book.split("\n")
            _id, total, dim = filter(None, re.split('[(HuffDec)|_x]', name))
            assert(len(incr.split(" ")) == len(code.split(" ")))
            assert(len(code.split(" ")) == len(indx.split(" ")))
            print "static const uint32_t Incr{0}[] = {{".format(i + 1), ", ".join(incr.split(" ")), "};"
            print "static const uint32_t Code{0}[] = {{".format(i + 1), ", ".join(code.split(" ")), "};"
            print "static const uint32_t Indx{0}[] = {{".format(i + 1), ", ".join(indx.split(" ")), "};"
            print "struct huff_codebook {0} = {{ {1}, {2}, {3}, sizeof(Incr{4}) / sizeof(uint32_t), Incr{4}, Code{4}, Indx{4}}};"\
                        .format(name, _id, (int)(dim), (int)((int)(total)**(1.0/(float)(dim))), i + 1)
            print "\n"
    print """
struct huff_codebook* pQuotientWidthBook = &HuffDec8_16x1;
struct huff_codebook* pQStepBook         = &HuffDec6_116x1;
struct huff_codebook* pClusterBook       = &HuffDec1_7x1;
struct huff_codebook* pRunLengthBook     = &HuffDec2_64x1;
struct huff_codebook* pHSBook            = &HuffDec4_18x1;

struct huff_codebook* QIndexBooks[MAX_BOOK] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &HuffDec27_256x1};

int32_t GetHuffDim(struct huff_codebook* book) {
    return book->dim;
}

uint8_t GetHuffMidTread(struct huff_codebook* book) {
    return book->num_codes != 256;
}

int32_t GetNumHuffCodes(struct huff_codebook* book) {
    return book->num_codes;
}
    """
    print "#endif"
    # dotc.close()
    # doth.close()

if __name__ == '__main__':
    main()