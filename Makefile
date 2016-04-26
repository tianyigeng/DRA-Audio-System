playorig:
	play audio/apple8000.wav 2> /dev/null 

wav2pcm:
	octave -q audio/wav2pcm.m apple8000.wav in.pcm

playafter:
	octave -q audio/testout.m

plotdiff:
	octave -q audio/plotdiff.m in.pcm out.pcm 

audioproc:
	make dev
	./dra audio/in.pcm audio/out.pcm
	-@rm dra || true

config:
	python configure/config.py

dev: main.o bitstream.o steps.o errors.o vector.o unitstep.o \
	 huffbook.o huffcoding.o hufftree.o mdct.o fastmdct.o
	gcc -o dra \
	main.o bitstream.o steps.o errors.o vector.o unitstep.o \
	huffbook.o huffcoding.o hufftree.o mdct.o fastmdct.o \
	-lfftw3

main.o: main.c
	gcc -c main.c \
		-o main.o

bs_iter.o: bitstream/bs_iter.c bitstream/bs_iter.h
	gcc -c bitstream/bs_iter.c \
		-o bs_iter.o

bitstream.o: bitstream/bitstream.c bitstream/bitstream.h
	gcc -c bitstream/bitstream.c \
		-o bitstream.o

mdct.o: mdct/mdct.h mdct/mdct.c
	gcc -c mdct/mdct.c \
		-o mdct.o

encode.o: dataframe/dataframe.h dataframe/encode.c
	gcc -c dataframe/encode.c\
		-o encode.o

decode.o: dataframe/dataframe.h dataframe/decode.c
	gcc -c dataframe/decode.c\
		-o decode.o

fastmdct.o: mdct/fastmdct/mdct_fft.c mdct/fastmdct/mdct_fft.h
	gcc -c mdct/fastmdct/mdct_fft.c \
		-o fastmdct.o

errors.o: util/errors.h util/errors.c
	gcc -c util/errors.c \
		-o errors.o

vector.o: util/vector.c util/vector.h
	gcc -c util/vector.c \
		-o vector.o

unitstep.o: unitstep/unitstep.h unitstep/unitstep.c
	gcc -c unitstep/unitstep.c \
		-o unitstep.o

steps.o: unitstep/unitstep.h unitstep/steps.c
	gcc -c unitstep/steps.c \
		-o steps.o

huffbook.o: huffman/huffbook.c huffman/huffbook.h
	gcc -c huffman/huffbook.c \
		-o huffbook.o

huffcoding.o: huffman/huffcoding.c huffman/huffcoding.h
	gcc -c huffman/huffcoding.c \
		-o huffcoding.o

hufftree.o: huffman/hufftree.c huffman/hufftree.h
	gcc -c huffman/hufftree.c \
		-o hufftree.o

test_unitstep.o: unitstep/test_unitstep.c
	gcc -c unitstep/test_unitstep.c \
		-o test_unitstep.o

testus: test_unitstep.o unitstep.o steps.o vector.o errors.o
	gcc -o testus test_unitstep.o unitstep.o steps.o vector.o errors.o
	-@./testus
	-@make clean
	-@rm testus

clean:
	-@find . -name \*.o -print -o -name \*.out | xargs rm || true
	-@rm dra 2> /dev/null || true

count:
	-@find . -name \*.h -print -o \
			 -name Makefile -print -o \
			 -name \*.c -print -o\
			 -name \*.m -print | xargs cat | wc -l

test_all.o: test_all.c
	gcc -c test_all.c \
		-o test_all.o

testall: test_all.o bitstream.o steps.o errors.o vector.o unitstep.o huffbook.o huffcoding.o hufftree.o mdct.o fastmdct.o
	gcc -o testall \
	test_all.o bitstream.o steps.o errors.o vector.o \
	unitstep.o huffbook.o huffcoding.o hufftree.o \
	mdct.o fastmdct.o -lfftw3
	-@./testall
	-@make clean
	-@rm testall

test_vector.o: util/test_vector.c
	gcc -c util/test_vector.c \
		-o test_vector.o

testvec: test_vector.o errors.o vector.o
	gcc -o testvec \
		test_vector.o errors.o vector.o
	-@./testvec
	-@make clean
	-@rm testvec

test_mdct.o: mdct/test_mdct.c
	gcc -c mdct/test_mdct.c \
		-o test_mdct.o

testmdct: test_mdct.o mdct.o fastmdct.o vector.o errors.o
	gcc -o testmdct \
		test_mdct.o mdct.o fastmdct.o vector.o errors.o -lfftw3
	-@./testmdct
	-@make clean
	-@rm testmdct

test_bitstream.o: bitstream/test_bitstream.c
	gcc -c bitstream/test_bitstream.c \
		-o test_bitstream.o

testbs: test_bitstream.o errors.o vector.o bitstream.o bs_iter.o
	gcc -o testbs \
		test_bitstream.o errors.o vector.o bitstream.o bs_iter.o
	-@./testbs
	-@make clean
	-@rm testbs

test_huff.o: huffman/test_huff.c
	gcc -c huffman/test_huff.c \
		-o test_huff.o

testhuff: test_huff.o errors.o vector.o bitstream.o huffcoding.o hufftree.o huffbook.o bs_iter.o
	gcc -o testhuff \
		test_huff.o errors.o vector.o bitstream.o huffcoding.o hufftree.o huffbook.o bs_iter.o
	-@./testhuff
	-@make clean
	-@rm testhuff
