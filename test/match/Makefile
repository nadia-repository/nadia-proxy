CC = gcc
CFLAGS = -O2 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: match_test

match_test: match_test.c match.o
	$(CC) $(CFLAGS) -o match_test match_test.c match.o -I ../../src/util $(LIB)

match.o: ../../src/util/match.c
	$(CC) $(CFLAGS) -c ../../src/util/match.c

clean:
	rm -f *.o match_test *~
	rm -rf *.dSYM