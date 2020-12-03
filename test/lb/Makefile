CC = gcc
CFLAGS = -O2 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: lb_test

lb_test: lb_test.c lb.o
	$(CC) $(CFLAGS) -o lb_test lb_test.c lb.o -I ../../src/proxy $(LIB)

lb.o: ../../src/proxy/lb.c
	$(CC) $(CFLAGS) -c ../../src/proxy/lb.c

clean:
	rm -f *.o lb_test *~
	rm -rf *.dSYM