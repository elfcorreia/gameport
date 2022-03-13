#include <gameport.h>
#include <iostream>
#include <cstring>

int main() {
    const int width = 320;
    const int height = 200;
    int framebuffer[width][height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            framebuffer[y][x] = 0xff0000;
            if (x + y % 1000 == 0) {
                std::cout << y << x << "\n";
            }
//              (int) ((y / (float) height) * 0xff0000) 
//            + (int) ((x / (float) width) * 0x00ff00);
        }
    }

    gameport* gp = gameport_create(width, height, nullptr);
    
    gameport_event e;
    bool end = false;
    while (!end && gameport_next_event(gp, &e)) {
        switch (e.type) {
        case GAMEPORT_DRAW:
            gameport_draw(gp, framebuffer);
            break;
        case GAMEPORT_EXIT: 
            end = true;
        }
    }
    gameport_destroy(gp);

    return 0;
}