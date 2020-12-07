CC = gcc
CFLAGS = -O2 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: arraylist_test

arraylist_test: arraylist_test.c csapp.o arraylist.o
	$(CC) $(CFLAGS) -o arraylist_test arraylist_test.c csapp.o arraylist.o -I ../../src/core -I ../../src/util $(LIB)

arraylist.o: ../../src/util/arraylist.c
	$(CC) $(CFLAGS) -c ../../src/util/arraylist.c -I ../../src/core

csapp.o: ../../src/core/csapp.c
	$(CC) $(CFLAGS) -c ../../src/core/csapp.c

clean:
	rm -f *.o arraylist_test *~
	rm -rf *.dSYM