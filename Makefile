CC=gcc
CFLAGS=-I. -std=gnu99 -Wall -Wextra
DEPS = traceroute.h
OBJ = traceroute.o sys_calls.o icmp_checksum.o

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)
	
traceroute: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
		rm -f *.o

distclean:
		rm -f *.o
		rm -f ./traceroute

