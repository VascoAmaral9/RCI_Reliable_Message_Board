CC=gcc
CFLAGS=-Wall -ansi -g -D_BSD_SOURCE

all: rmb msgserv

msgserv: msgserv.o core.o udp.o tcp.o LinkedList.o
	$(CC) $(CFLAGS) -o msgserv msgserv.o core.o udp.o tcp.o LinkedList.o

rmb: rmb.o core.o udp.o tcp.o LinkedList.o
	$(CC) $(CFLAGS) -o rmb rmb.o core.o udp.o tcp.o LinkedList.o

msgserv.o: msgserv.c
	$(CC) -c $(CFLAGS) msgserv.c

rmb.o: rmb.c
	$(CC) -c $(CFLAGS) rmb.c

core.o: core.c
	$(CC) -c $(CFLAGS) core.c

udp.o: udp.c
	$(CC) -c $(CFLAGS) udp.c

tcp.o: tcp.c
	$(CC) -c $(CFLAGS) tcp.c

LinkedList.o: LinkedList.c
	$(CC) -c $(CFLAGS) LinkedList.c


clean::
	rm -f *.o core a.out newWords *~
