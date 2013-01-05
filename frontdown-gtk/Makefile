TARGET=frontdown-gtk
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-L./ -Wall -g -lpthread -lcurl -fPIC -D_GNU_SOURCE -lfrontdown `pkg-config gtk+-2.0 --cflags --libs` -D_GUI_

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
	
new: distclean all

install: all
	cp $(TARGET) /usr/bin/
