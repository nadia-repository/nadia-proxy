CC = gcc
CFLAGS = -O2 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: thread_test

thread_test: thread_test.c csapp.o thread.o
	$(CC) $(CFLAGS) -o thread_test thread_test.c csapp.o thread.o -I ../../src/core $(LIB)

thread.o: ../../src/core/thread.c
	$(CC) $(CFLAGS) -c ../../src/core/thread.c

csapp.o: ../../src/core/csapp.c
	$(CC) $(CFLAGS) -c ../../src/core/csapp.c

clean:
	rm -f *.o thread_test *~
	rm -rf *.dSYM