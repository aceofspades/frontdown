TARGET=frontdown-gtk
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-Wall `pkg-config gtk+-2.0 --cflags --libs` -g -lcurl -D_GNU_SOURCE

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
	
