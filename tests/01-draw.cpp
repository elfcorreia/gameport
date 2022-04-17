#include <gameport.h>
#include <iostream>
#include <cstring>

int main() {
    const int width = 320;
    const int height = 200;
    int framebuffer[width*height];

    int colors[] = {
        0xff0000,
        0x00ff00,
        0x0000ff,
        0xffff00,
        0xff00ff,
        0x00ffff,
        0xffffff,
        0x000000,
    };
    int colors_size = sizeof(colors) / sizeof(int);

    for (int a = 0,s=width*height; a < s; a++) {
        *(framebuffer + a) = colors[a % colors_size];        
    }

    gameport_t* gp = gameport_create(width, height, nullptr);
    
    gameport_event_t e;
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