TARGET=frontdown
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-Wall -g -lcurl -D_GNU_SOURCE

all: $(OBJS)
	$(CC) $(OBJS) -v -Wl,-rpath,'$$ORIGIN/../lib' -L../lib -lfrontdown $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
	
new: distclean all

install: all
	cp $(TARGET) /usr/bin/
	
