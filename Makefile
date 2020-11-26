CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: server

server: src/server/server.c src/server/proxy.c src/server/worker.c csapp.o thread.o hashmap.o config.o
	$(CC) $(CFLAGS) -o server src/server/server.c src/server/proxy.c src/server/worker.c csapp.o thread.o hashmap.o config.o -I src/config -I src/core -I src/util $(LIB)

csapp.o: src/core/csapp.c
	$(CC) $(CFLAGS) -c src/core/csapp.c

thread.o: src/core/thread.c
	$(CC) $(CFLAGS) -c src/core/thread.c

hashmap.o: src/util/hashmap.c
	$(CC) $(CFLAGS) -c src/util/hashmap.c

config.o: src/config/config.c csapp.o
	$(CC) $(CFLAGS) -c src/config/config.c csapp.o -I src/core
	
clean:
	rm -f *.o server *~
