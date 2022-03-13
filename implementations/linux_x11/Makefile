.PHONY: all clean install

CFLAGS=-Wall -pedantic -fPIC -I ../..

all: libgameport.a libgameport.so

clean:
	@echo "=> Cleaning"
	-rm *.o *.a

libgameport.a: gameport.o
	@echo "=> Building gameport static library"
	ar rcs libgameport.a gameport.o

libgameport.so: gameport.o
	@echo "=> Building gameport dynamic library"
	$(CC) -shared $(CFLAGS) $< -o $@