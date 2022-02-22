#include <iostream>
#include <cstring>
#include "evipo.hpp"

class view {
public:
    view(int width = 64, int height = 64, bool verbose = true) {
        _width = width;
        _height = height;
        _buffer = new int[width*height];
        std::memset(_buffer, 0x00ffff, sizeof(int)*width*height);
        _viewport = vcreate(width, height, verbose ? "verbose" : "");
    }
    ~view() {
        delete[] _buffer;
        vdestroy(_viewport);
    }
    void put(int x, int y, int c) {
        _buffer[y * _width + x] = c;
    }

    void draw() {        
        vsync(_viewport, _buffer);
    }
private:
    int _width;
    int _height;
    int* _buffer;
    viewport* _viewport;
};

int main() {
    view v;
    v.draw();
    while (!getc(stdin) == 'd');
    return 0;
}