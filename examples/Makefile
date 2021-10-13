all: clean main

main: hello.exe xadrez.exe animation.exe
	
clean:
	-rm -rf *.exe

%.exe: %.cpp
	g++ -I .. ../src/x11.cpp $< -pthread -lX11 -o $@