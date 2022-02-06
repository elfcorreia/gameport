.PHONY: all clean

CXXFLAGS=-Wall -pedantic -fPIC -I ../..

all: evipo.a

clean:
	-rm *.o *.a

evipo.a: evipo.o
	ar rcs evipo.a evipo.o