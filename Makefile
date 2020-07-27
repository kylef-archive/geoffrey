CC = gcc
CFLAGS = -O2

HEADERS = -I.
LIBS = -L.
LIB = -lgeoffrey

AR = ar
ARFLAGS = rcs

all: libgeoffrey.a

libgeoffrey.a: src/net.o src/main.o src/helpers.o
	$(AR) $(ARFLAGS) $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(HEADERS) -c $^ -o $@

examples/%.o: examples/%.c
	$(CC) $(CFLAGS) $(HEADERS) -c $^ -o $@

bin/%: examples/%.o libgeoffrey.a
	@mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

examples: bin/echo bin/logger bin/relay

clean:
	rm -rf *.a src/*.o examples/*.o bin
