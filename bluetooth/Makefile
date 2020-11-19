CC = gcc
LNK = gcc

LIBDIR = /usr/lib/x86_64-linux-gnu/
LIBS = 'pkg-config --libs gio-2.0' -lbluetooth
CFLAGS = 'pkg-config --cflags gio-2.0' -c

all: client server

server: server.o
	$(LNK) -L$(LIBDIR) server.o -o server $(LIBS)
client : client.o
	$(LNK) -L$(LIBDIR) client.o -o client $(LIBS)

server.o : server.c
	$(CC) $(CFLAGS) server.c

client.o : client.c
	$(CC) $(CFLAGS) client.c

.PHONY: clean

clean:
	rm -f server client server.o client.o
	
