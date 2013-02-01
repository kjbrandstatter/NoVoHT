PROJECT=HashTable
CC=g++
CFLAGS=-Wall
BINS=fbench pbench genbench

SRCDIR=src
BINDIR=bin
TESTDIR=tests
LIBDIR=lib

OBJECTS=novoht.o
LDFLAGS=-I$(SRCDIR) -lpthread

all: $(BINS)

%.o : $(SRCDIR)/%.cxx
	$(CC) $(CFLAGS) -c $< -o $(SRCDIR)/$@

pbench: $(OBJECTS) $(TESTDIR)/pbench.cpp
	[[ -d $(BINDIR) ]] || mkdir -p $(BINDIR)
	$(CC) $(SRCDIR)/novoht.o $(TESTDIR)/pbench.cpp -o $(BINDIR)/pbench $(LDFLAGS)

fbench: $(OBJECTS) $(TESTDIR)/fbench.cxx
	[[ -d $(BINDIR) ]] || mkdir -p $(BINDIR)
	$(CC) $(SRCDIR)/novoht.o $(TESTDIR)/fbench.cxx -o $(BINDIR)/fbench $(LDFLAGS)

gpbbench: $(OBJECTS) $(TESTDIR)/gpbbench.cxx
	$(CC) $(SRCDIR)/novoht.o $(TESTDIR)/gpbbench.cxx -o $(BINDIR)/gpbbench -lz -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotoc meta.pb.cc $(LDFLAGS)

genbench: $(OBJECTS) $(TESTDIR)/genbench.cpp
	[[ -d $(BINDIR) ]] || mkdir -p $(BINDIR)
	$(CC) $(SRCDIR)/novoht.o $(TESTDIR)/genbench.cpp -I$(TESTDIR) $(LDFLAGS) -o $(BINDIR)/genbench

appendtest: $(OBJECTS) $(TESTDIR)/appendtest.cpp
	$(CC) $(SRCDIR)/novoht.o $(TESTDIR)/appendtest.cpp -o $(BINDIR)/appendtest -lpthread $(LDFLAGS)
#
libnovoht: $(OBJECTS)
	rm -rf $(LIBDIR)
	mkdir -p $(LIBDIR)
	g++ $(CFLAGS) -c -fPIC -o $(SRCDIR)/novoht.obj $(SRCDIR)/novoht.cxx $(LDFLAGS)
	g++ -shared -o $(LIBDIR)/libnovoht.so $(SRCDIR)/novoht.obj
	rm $(SRCDIR)/novoht.obj
	ar -cvq $(LIBDIR)/libnovoht.a $(SRCDIR)/novoht.o

clean:
	rm -f $(SRCDIR)/*.o
	rm -rv $(BINDIR) $(LIBDIR)
