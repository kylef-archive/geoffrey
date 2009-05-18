CC = gcc
CFLAGS = -O2

HEADERS = -I.
LIBS = -L.
LIB = -lgeoffrey

AR = ar
ARFLAGS = rcs


all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(HEADERS) -c src/net.c -o net.o
	$(CC) $(CFLAGS) $(HEADERS) -c src/main.c -o main.o
	$(CC) $(CFLAGS) $(HEADERS) -c src/helpers.c -o helpers.o
	$(AR) $(ARFLAGS) libgeoffrey.a main.o net.o helpers.o

clean:
	rm -rf *.a *.o bin

examples: all
	$(CC) $(CFLAGS) $(HEADERS) -c examples/echo.c -o echo.o
	$(CC) $(CFLAGS) $(HEADERS) -c examples/logger.c -o logger.o
	$(CC) $(CFLAGS) $(HEADERS) -c examples/relay.c -o relay.o
	$(CC) $(CFLAGS) $(LIBS) -o bin/echo echo.o $(LIB)
	$(CC) $(CFLAGS) $(LIBS) -o bin/logger logger.o $(LIB)
	$(CC) $(CFLAGS) $(LIBS) -o bin/relay relay.o $(LIB)
