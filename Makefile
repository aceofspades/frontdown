OBJS=frontdown.o
TARGET=frontdown
CC=gcc
CFLAGS=-Wall

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	
*.o: *.c
	$(CC) $(CFLAGS) -c $< -o $@
