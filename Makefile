CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: server

server: src/server/server.c src/server/proxy.c src/server/worker.c csapp.o thread.o config.o config_configs.o config_proxy.o hashmap.o match.o
	$(CC) $(CFLAGS) -o nadia \
	src/server/server.c \
	src/server/proxy.c \
	src/server/worker.c \
	csapp.o \
	thread.o \
	config.o \
	config_configs.o \
	config_proxy.o \
	hashmap.o \
	match.o \
	-I src/util \
	-I src/config \
	-I src/core \
	$(LIB)
	rm -f *.o

csapp.o: src/core/csapp.c
	$(CC) $(CFLAGS) -c src/core/csapp.c

thread.o: src/core/thread.c
	$(CC) $(CFLAGS) -c src/core/thread.c

hashmap.o: src/util/hashmap.c
	$(CC) $(CFLAGS) -c src/util/hashmap.c -I src/core

match.o: src/util/match.c
	$(CC) $(CFLAGS) -c src/util/match.c

config.o: src/config/config.c src/config/config_configs.c src/config/config_proxy.c hashmap.o csapp.o
	$(CC) $(CFLAGS) -c src/config/config.c src/config/config_configs.c src/config/config_proxy.c hashmap.o csapp.o -I src/core -I src/util
	
clean:
	rm -f *.o nadia *~
