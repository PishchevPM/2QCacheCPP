CC = g++
CFLAGS = -std=c++11 -g -O2 -Wall -I include
OBJECTS = $(BASEOBJECTS) cache_test.o cache_test

all: cache_test

run:
	for i in testdata/*.dat; do echo $$(basename $$i); ./cache_test < $$i; done;

cache_test: source/cache_test.cpp
	$(CC) $(CFLAGS) source/cache_test.cpp -o cache_test

clean:
	rm -r -f $(OBJECTS)
	rm -f cache_test
