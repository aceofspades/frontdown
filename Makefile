TARGET=libfrontdown.so
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-L./ -Wall -g -lpthread -lcurl -fPIC -D_GNU_SOURCE

all: $(OBJS)
	$(CC) $(CFLAGS) -shared $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
	rm -f $(TARGET).exe
	
new: distclean all
