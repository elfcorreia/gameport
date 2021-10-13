#include "framebuffer.h"
#include <iostream>

using std::cin;
using std::cout;

int main() {

    const int black = 0x000000;
    const int white = 0xffffff;
    const int width = 6;
    const int height = 6;

    int tela[width][height] = {};

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            tela[x][y] = (x + y) % 2 ? white : black;
        }
    }

    fb_init(300, 300, "");
    fb_buffer(tela, width, height);
    fb_sync(tela);
    fb_finish();
    
    return 0;
}