dev: 
	gcc main.c \
	util/errors.h util/errors.c \
	huffman/huffbook.c huffman/huffbook.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	huffman/huffcoding.c huffman/huffcoding.h \
	huffman/hufftree.c huffman/hufftree.h \
	util/vector.c util/vector.h

testvec:
	gcc control.h \
	util/errors.h util/errors.c \
	util/vector.c util/vector.h \
	util/test_vector.c

testmdct:
	gcc control.h \
	mdct/mdct.c mdct/mdct.h \
	mdct/test_mdct.c

testbs:
	gcc control.h \
	util/errors.h util/errors.c \
	util/vector.c util/vector.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	bitstream/test_bitstream.c

testhuff:
	gcc control.h \
	util/errors.h util/errors.c \
	util/vector.c util/vector.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	huffman/huffcoding.c huffman/huffcoding.h \
	huffman/hufftree.c huffman/hufftree.h \
	huffman/huffbook.c huffman/huffbook.h \
	huffman/test_huff.c

clean:
	-@rm *.gch */*.gch *.out */*.out 2> /dev/null || true

count:
	-@find . -name \*.h -print -o -name \*.c -print | xargs cat | wc -l
