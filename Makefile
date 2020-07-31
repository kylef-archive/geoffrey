CC := gcc
CFLAGS := -O2

HEADERS = -I.
LIBS = -L.
LIB = -lgeoffrey

AR = ar
ARFLAGS = rcs

all: libgeoffrey.a
.PHONY: test
test: runtests
	@./runtests

runtests: libgeoffrey.a tests/main.o tests/test_message.o tests/test_helpers.o
	$(CC) -g $(CFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

libgeoffrey.a: src/net.o src/main.o src/helpers.o src/message.o
	$(AR) $(ARFLAGS) $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(HEADERS) -c $^ -o $@

tests/%.o: tests/%.c tests/greatest/greatest.h
	$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

examples/%.o: examples/%.c
	$(CC) $(CFLAGS) $(HEADERS) -c $^ -o $@

bin/%: examples/%.o libgeoffrey.a
	@mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

examples: bin/echo bin/logger bin/relay

clean:
	rm -rf *.a src/*.o examples/*.o bin tests/*.o runtests

tests/greatest/greatest.h:
	git clone https://github.com/silentbicycle/greatest.git tests/greatest
