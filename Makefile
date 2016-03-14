dev: 
	gcc main.c \
	huffman/huffbook.c huffman/huffbook.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	huffman/huffcoding.c huffman/huffcoding.h \
	huffman/hufftree.c huffman/hufftree.h \
	util/vector.c util/vector.h

testvec:
	gcc util/vector.c util/vector.h \
	util/test_vector.c