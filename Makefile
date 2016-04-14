dev:
	gcc main.c control.h \
	util/errors.h util/errors.c \
	huffman/huffbook.c huffman/huffbook.h \
	huffman/huffcoding.c huffman/huffcoding.h \
	huffman/hufftree.c huffman/hufftree.h \
	unitstep/unitstep.h unitstep/unitstep.c unitstep/steps.c \
	mdct/mdct.c mdct/mdct.h \
	mdct/fastmdct/mdct_fft.c mdct/fastmdct/mdct_fft.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	util/vector.c util/vector.h -lfftw3

config:
	python configure/config.py

testall: 
	gcc test_all.c control.h \
	util/errors.h util/errors.c \
	huffman/huffbook.c huffman/huffbook.h \
	huffman/huffcoding.c huffman/huffcoding.h \
	huffman/hufftree.c huffman/hufftree.h \
	unitstep/unitstep.h unitstep/unitstep.c unitstep/steps.c \
	mdct/mdct.c mdct/mdct.h \
	mdct/fastmdct/mdct_fft.c mdct/fastmdct/mdct_fft.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	util/vector.c util/vector.h -lfftw3

testvec:
	gcc control.h \
	util/errors.h util/errors.c \
	util/vector.c util/vector.h \
	util/test_vector.c 

testmdct:
	gcc control.h \
	mdct/mdct.c mdct/mdct.h \
	mdct/fastmdct/mdct_fft.c mdct/fastmdct/mdct_fft.h \
	util/vector.h util/vector.c \
	util/errors.h util/errors.c \
	mdct/test_mdct.c -lfftw3

testbs:
	gcc control.h \
	util/errors.h util/errors.c \
	util/vector.c util/vector.h \
	bitstream/bitstream.c bitstream/bitstream.h \
	bitstream/test_bitstream.c

testus:
	gcc control.h \
	util/errors.h util/errors.c \
	util/vector.c util/vector.h \
	unitstep/unitstep.h unitstep/unitstep.c unitstep/steps.c \
	unitstep/test_unitstep.c

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
	-@find . -name \*.gch -print -o -name \*.out | xargs rm || true

count:
	-@find . -name \*.h -print -o -name Makefile -print -o -name \*.c -print | xargs cat | wc -l
