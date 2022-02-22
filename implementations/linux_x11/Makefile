# TODO: clang, .so, cmake?
.PHONY: all clean install

CFLAGS=-Wall -pedantic -fPIC -I ../..
ifeq ($(DESTDIR),)
	DESTDIR := /usr/local
endif


all: libevipo.a libevipo.so

clean:
	@echo "==> Cleaning"
	-rm *.o *.a

libevipo.a: evipo.o
	@echo "==> Building evipo static library"
	ar rcs libevipo.a evipo.o

libevipo.so: evipo.o
	@echo "==> Building evipo dynamic library"
	$(CC) -shared $(CFLAGS) $< -o $@

install: libevipo.a libevipo.so
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libevipo.a $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libevipo.so $(DESTDIR)$(PREFIX)/lib/
