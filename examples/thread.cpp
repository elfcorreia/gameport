#include <iostream>
#include <cstring>
#include "evipo.hpp"

class pixelview {
public:
    pixelview(int width = 64, int height = 64, int pixel_size = 8, bool verbose = true) {
        _width = width;
        _height = height;
        _pixel_size = pixel_size;
        _frontbuffer = new int[width*height];
        std::memset(_frontbuffer, 0xffffff, sizeof(int)*width*height);
        _backbuffer = new int(width*height*pixel_size);
        _viewport = vcreate(width*pixel_size, height*pixel_size, verbose ? "verbose" : "");
    }
    ~pixelview() {
        delete[] _frontbuffer;
        delete[] _backbuffer;
        vdestroy(_viewport);
    }
    void put(int x, int y, int c) {
        _frontbuffer[y * _width + x] = c;        
    }

    void sync() {
        upsample(_frontbuffer, _width, _height, _backbuffer, _pixel_size);
        vsync(_viewport, _backbuffer);
    }
private:
    int _pixel_size;
    int _width;
    int _height;
    int* _frontbuffer;
    int* _backbuffer;
    viewport* _viewport;

    void upsample(int* src, int src_width, int src_height, int* dst, int factor = 1) {
        if (factor == 1) {
            return;
        }
        size_t ps = factor;
        int dst_width = src_width * factor;        
        size_t dst_linebytes = sizeof(int) * dst_width;

        for (int i = 0; i < src_height; i++) {
            int* dst_beginline = dst;
            // expande a linha horizontalmente
            for (int* src_endline = src + src_width; src != src_endline; src++) {
                for (size_t j = 0; j < ps; j++) {
                    *dst++ = *src;
                }
            }
            // clona a linha (ps - 1) vezes
            for (int j = 0; j < ps - 1; j++) {
                memcpy(dst, dst_beginline, dst_linebytes);
                dst += dst_width;
            }
        }    
    }
};

int main() {
    const int black = 0x000000;
    pixelview pv(8, 8, 24);

/*    pv.put(2, 2, black);
    pv.put(2, 3, black);

    pv.put(5, 2, black);
    pv.put(5, 3, black);

    pv.put(1, 5, black);
    pv.put(2, 6, black);
    pv.put(3, 6, black);
    pv.put(4, 6, black);
    pv.put(5, 6, black);
    pv.put(6, 5, black);
    
*/    pv.sync();

    while (!getc(stdin) == 'd');
    return 0;
}