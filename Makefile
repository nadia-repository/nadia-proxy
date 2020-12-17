CC = gcc
# CFLAGS = -O2 -Wall -I . -g
CFLAGS = -O0 -Wall -I . -g

# This flag includes the Pthreads library on a Linux box.
# Others systems will probably require something different.
LIB = -lpthread

all: server

server: src/server/server.c proxy.o proxy_common.o proxy_static.o proxy_dynamic.o lb.o match.o hashmap.o stack.o dynamic_string.o arraylist.o csapp.o thread.o config.o config_configs.o config_proxy.o  worker.o
	$(CC) $(CFLAGS) -o nadia src/server/server.c \
	match.o \
	hashmap.o \
	stack.o \
	dynamic_string.o \
	arraylist.o \
	csapp.o \
	thread.o \
	config.o \
	config_configs.o \
	config_proxy.o \
	proxy.o \
	proxy_common.o \
	proxy_static.o \
	proxy_dynamic.o \
	lb.o \
	worker.o \
	-I src/util \
	-I src/config \
	-I src/os \
	-I src/proxy \
	$(LIB)
	rm -f *.o

worker.o: src/server/worker.c 
	$(CC) $(CFLAGS) -c src/server/worker.c -I src/config -I src/os -I src/util -I src/proxy

proxy.o: src/proxy/proxy.c src/proxy/proxy_common.c src/proxy/proxy_static.c src/proxy/proxy_dynamic.c src/proxy/lb.c
	$(CC) $(CFLAGS) -c src/proxy/proxy.c src/proxy/proxy_common.c src/proxy/proxy_static.c src/proxy/proxy_dynamic.c src/proxy/lb.c -I src/config -I src/os -I src/util

config.o: src/config/config.c src/config/config_configs.c src/config/config_proxy.c 
	$(CC) $(CFLAGS) -c src/config/config.c src/config/config_configs.c src/config/config_proxy.c  -I src/os -I src/util

thread.o: src/os/thread.c
	$(CC) $(CFLAGS) -c src/os/thread.c

match.o: src/util/match.c
	$(CC) $(CFLAGS) -c src/util/match.c

csapp.o: src/os/csapp.c
	$(CC) $(CFLAGS) -c src/os/csapp.c
	
hashmap.o: src/util/hashmap.c
	$(CC) $(CFLAGS) -c src/util/hashmap.c -I src/os

stack.o: src/util/stack.c
	$(CC) $(CFLAGS) -c src/util/stack.c -I src/os

dynamic_string.o: src/util/dynamic_string.c
	$(CC) $(CFLAGS) -c src/util/dynamic_string.c -I src/os

arraylist.o: src/util/arraylist.c
	$(CC) $(CFLAGS) -c src/util/arraylist.c -I src/os

clean:
	rm -rf *.dSYM
	rm -f *.o nadia *~
	
install:
	mkdir -p /home/nadia
	cp nadia /home/nadia
	cp home.html /home/nadia
	cp -r config /home/nadia