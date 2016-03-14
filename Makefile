dev: 
	gcc main.c \
	huffman/huffbook.c huffman/huffbook.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	huffman/huffmandec.c huffman/huffmandec.h \
	huffman/hufftree.c huffman/hufftree.h 

testvec:
	gcc util/vector.c util/vector.h \
	util/test_vector.c