TARGET=frontdown
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-Wall -static -g -O99

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
	rm -f $(TARGET).exe
	
new: distclean all
