def main():
    header = """
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
            assert(len(incr.split(" ")) == len(code.split(" ")))
            assert(len(code.split(" ")) == len(indx.split(" ")))
            print "static const uint16_t Incr{0}[] = {{".format(i + 1), ", ".join(incr.split(" ")), "};"
            print "static const uint16_t Code{0}[] = {{".format(i + 1), ", ".join(code.split(" ")), "};"
            print "static const uint16_t Indx{0}[] = {{".format(i + 1), ", ".join(indx.split(" ")), "};"
            print "struct huff_codebook {0} = {{ sizeof(Incr{1}) / sizeof(uint16_t), Incr{1}, Code{1}, Indx{1}}};"\
                        .format(name, i + 1)
            print "\n"
            """
            static const uint16_t Incr1[] = {2, 0, 1, 0, 0, 1, 0};
            static const uint16_t Code1[] = {0, 3, 3, 2, 5, 9, 8};
            static const uint16_t Indx1[] = {0, 1, 2, 3, 5, 4, 6};
            struct huff_codebook HuffDec1_7x1 = { sizeof(Incr1) / sizeof(uint16_t), Incr1, Code1, Indx1};
            """
    # dotc.close()
    # doth.close()

if __name__ == '__main__':
    main()