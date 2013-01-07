TARGET=libfrontdown.so
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc
CFLAGS=-Wall -g -lcurl -fPIC -D_GNU_SOURCE

all: $(OBJS)
	$(CC) $(CFLAGS) -shared  $(OBJS) -o $(TARGET)
	mkdir -p lib
	cp $(TARGET) ./lib
	objdump lib/libfrontdown.so -t | grep frontdown
	cd frontdown-cli && make

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
	rm -rf lib
	
new: distclean all

install: all
	cp $(TARGET) /usr/lib/
	cd frontdown-cli && make install

install_gui: all
    cp $(TARGET) /usr/lib/
	cd frontdown-gtk && make install
