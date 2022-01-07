all: build

clean:
	-@rm *.so *.a *.o

build: libeasyview.so libeasyview.a
	
easyview.o: easyview.cpp	
	$(CXX) -I ../.. -Wall -c easyview.cpp -o easyview.o

libeasyview.a: easyview.o
	ar rc easyview.a easyview.o

libeasyview.so: easyview.o
	$(CXX) -shared easyview.o -o libeasyview.so