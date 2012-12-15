OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
TARGET=frontdown
CC=gcc
CFLAGS=-Wall

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	
*.o: *.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
	
new: distclean all
	
