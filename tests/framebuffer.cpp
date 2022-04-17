#include <cstring>
#include "evipo.h"
#include <iostream>

class engine {
public:
    engine(size_t width, size_t height) {
        _width = width;
        _height = height;
        _framebuffer = new int[width*height];        
    }
    ~engine() {
        delete _framebuffer;
    }
    auto framebuffer() { return _framebuffer; }
    auto width() { return _width; }
    auto height() { return _height; }
    void show() {
        evipo_port* port = evipo_port_create(_width, _height, "");
        struct evipo_event event;
        int finished = 0;
        while (!finished && evipo_port_event(port, &event)) {
            switch (event.type) {
            case EV_DRAW:
                evipo_port_draw(port, _framebuffer);
                break;                
            case EV_FINISH:
                finished = 1;
                break;
            }
        }
        evipo_port_destroy(port);
    }
private:
    int _width;
    int _height;
    int* _framebuffer;
};

int main() {

    engine e(64, 64);

    int* pos = e.framebuffer();
    for (int i = 0; i < e.width(); i++) {
        for (int j = 0; j < e.height(); j++) {
            *pos++ = 0xff00ff;            
        }
    }
    
    e.show();

    return 0;
}