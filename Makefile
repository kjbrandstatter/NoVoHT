PROJECT=HashTable
CC=g++
CFLAGS=-c -Wall
LDFLAGS=-I
SOURCE=hashmap.cxx main.cxx
OBJECTS=phashmap.o main.o
HEADERS=phashmap.h
BINS=kbench tonybench iobench fbench

all: $(BINS)

fbench: phashmap.o fbench.cxx
	$(CC) phashmap.o fbench.cxx -o fbench

kbench: phashmap.o kbench.cxx
	$(CC) phashmap.o kbench.cxx -o kbench

tonybench: phashmap.o test_hash.cpp
	$(CC) phashmap.o test_hash.cpp -o tonybench

iobench: phashmap.o inoutbench.cpp
	$(CC) phashmap.o inoutbench.cpp -o iobench

phashmap.o: phashmap.cxx
	$(CC) $(CFLAGS) phashmap.cxx

clean:
	rm *.o $(BINS)

rebuild: clean

test: kbench
	./kbench
