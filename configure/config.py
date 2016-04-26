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
            _, total, dim = re.split('[_x]', name)
            assert(len(incr.split(" ")) == len(code.split(" ")))
            assert(len(code.split(" ")) == len(indx.split(" ")))
            print "static const uint32_t Incr{0}[] = {{".format(i + 1), ", ".join(incr.split(" ")), "};"
            print "static const uint32_t Code{0}[] = {{".format(i + 1), ", ".join(code.split(" ")), "};"
            print "static const uint32_t Indx{0}[] = {{".format(i + 1), ", ".join(indx.split(" ")), "};"
            print "struct huff_codebook {0} = {{ {1}, {2}, sizeof(Incr{3}) / sizeof(uint32_t), Incr{3}, Code{3}, Indx{3}}};"\
                        .format(name, (int)(dim), (int)((int)(total)**(1.0/(float)(dim))), i + 1)
            print "\n"
    print """
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