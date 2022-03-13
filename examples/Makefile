.PHONY: all clean

CFLAGS=-I. -L. -lX11 -lgameport  -Wl,-rpath='$${ORIGIN}'
CXXFLAGS=-I. -L. -lX11 -lgameport -Wl,-rpath='$${ORIGIN}'

clean:
	find . -name "*.o" -or -executable -not -name "*.so" -delete