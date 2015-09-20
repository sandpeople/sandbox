all:
	cd ../clib/ && make
	gcc -o ./test -g -L../ -I../clib/ test.c -lSDL2 -lclib
gdb: all
	cd .. && gdb ./clib_test/test -ex "set environment LD_LIBRARY_PATH ./"
valgrind: all
	cd .. && LD_LIBRARY_PATH=./ valgrind ./clib_test/test
nemiver: all
	cd .. && nemiver ./clib_test/test --env="LD_LIBRARY_PATH=./"
