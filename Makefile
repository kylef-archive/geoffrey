CC = gcc
CFLAGS = -O2

HEADERS = -I.
LIBS = -L.
LIB = -lgeoffrey

AR = ar
ARFLAGS = rcs


all:
	$(CC) $(CFLAGS) $(HEADERS) -c libgeoffrey/net.c -o net.o
	$(CC) $(CFLAGS) $(HEADERS) -c libgeoffrey/main.c -o main.o
	$(CC) $(CFLAGS) $(HEADERS) -c libgeoffrey/helpers.c -o helpers.o
	$(AR) $(ARFLAGS) libgeoffrey.a main.o net.o helpers.o

clean:
	rm *.a *.o

bot: all
	$(CC) $(CFLAGS) $(HEADERS) -c bot/main.c -o main.o
	$(CC) $(CFLAGS) $(LIBS) -o geoffrey main.o $(LIB)

examples: all
	$(CC) $(CFLAGS) $(HEADERS) -c examples/echo.c -o echo.o
	$(CC) $(CFLAGS) $(HEADERS) -c examples/logger.c -o logger.o
	$(CC) $(CFLAGS) $(HEADERS) -c examples/relay.c -o relay.o
	$(CC) $(CFLAGS) $(LIBS) -o echo echo.o $(LIB)
	$(CC) $(CFLAGS) $(LIBS) -o logger logger.o $(LIB)
	$(CC) $(CFLAGS) $(LIBS) -o relay relay.o $(LIB)
