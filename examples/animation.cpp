#include "framebuffer.h"
#include <iostream>
#include <cstring>

int main() {
    int tela[50][50];


    fb_init(300, 300, "");
    fb_buffer(tela, 50, 50);
    while (true) {
        for (int y = 0; y < 50; y++) {
            for (int x = 0; x < 50; x++) {
                tela[y][x] = rand();
            }
        }
        fb_sync(tela);
    }
    fb_finish();
    
    return 0;
}