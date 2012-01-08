PROJECT=HashTable
CC=g++
CFLAGS=-c -Wall
LDFLAGS=-I
SOURCE=hashmap.cxx main.cxx
OBJECTS=novoht.o main.o
HEADERS=novoht.h
BINS=kbench tonybench fbench

all: $(BINS)

fbench: novoht.o fbench.cxx
	$(CC) novoht.o fbench.cxx -o fbench #-lz -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc

kbench: novoht.o kbench.cxx
	$(CC) novoht.o kbench.cxx -o kbench

tonybench: novoht.o test_hash.cpp
	$(CC) novoht.o test_hash.cpp -o tonybench

novoht.o: novoht.cxx
	$(CC) $(CFLAGS) novoht.cxx

clean:
	rm *.o $(BINS)

rebuild: clean

test: kbench
	./kbench
