CC = g++
CFLAGS = -g -O2 -Wall -I include
BASEOBJECTS = cache.o
OBJECTS = $(BASEOBJECTS) cache_test.o cache_test

all: cache_test

cache.o: source/cache.cpp
	$(CC) $(CFLAGS) source/cache.cpp -c -o cache.o

cache_test.o: source/cache_test.cpp
	$(CC) $(CFLAGS) source/cache_test.cpp -c -o cache_test.o

cache_test: $(BASEOBJECTS) cache_test.o
	$(CC) $(CFLAGS) $(BASEOBJECTS) cache_test.o -o cache_test

clean:
	rm -r -f $(OBJECTS)
	rm -f cache_test