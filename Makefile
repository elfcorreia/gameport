.PHONY: docs

all: teste.exe

teste.exe: teste.cpp canvas.cpp
	g++ canvas.cpp teste.cpp -lgdiplus -o teste.exe

doxygen: docs/Doxyfile
	cd docs && doxygen Doxyfile && cd -

sphinx:
	cd docs && make html && cd -

docs: doxygen sphinx