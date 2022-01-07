all: clean main

main: hello.exe xadrez.exe animation.exe
	
clean:
	-rm -rf *.exe

%.exe: %.cpp
	$(CCX) -I .. ../build/easyview.a $< $@