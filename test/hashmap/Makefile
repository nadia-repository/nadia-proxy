CC = gcc
CFLAGS = -O2 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: hashmap_test

hashmap_test: hashmap_test.c hashmap.o
	$(CC) $(CFLAGS) -o hashmap_test hashmap_test.c hashmap.o -I ../src/util $(LIB)

hashmap.o: ../src/util/hashmap.c
	$(CC) $(CFLAGS) -c ../src/util/hashmap.c

clean:
	rm -f *.o hashmap_test *~
	rm -rf *.dSYM