#include <iostream>
#include "evipo.h"

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

    viewport* v = vcreate(width, height, nullptr);
    while (!vfinished(v)) {
        vsync(v);
    }
    vdestroy(v);
    return 0;
}