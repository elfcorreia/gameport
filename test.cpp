#include "framebuffer.h"
#include <iostream>

using std::cin;
using std::cout;

int main() {

    const int red   = 0xff0000;
    const int green = 0x00ff00;
    const int blue  = 0x0000ff;
    const int white = 0xffffff;
    
    framebuffer_options opcoes = {
        .width = 32,
        .height = 32,
        .pixel_scale = 16,
        .verbose = true
    };
    framebuffer fb;
    int* buffer = fb_init(&opcoes, &fb);

    for (int i = 0; i < fb.buffer_size; i++) {
        buffer[i] = white;
    }
    fb_update(&fb);

    char a;
    cin >> a;
    fb_close(&fb);
    return 0;
}