#include <string.h>
#include "evipo.h"

int main() {

    int tela[480][480];
    memset(tela, 0xffffff, sizeof(tela));
    int cores[] = {
        0x000000, 0xff0000, 0x00ff00, 0x0000ff,
        0xffff00, 0xff00ff, 0xffffff, 0x00ffff
    };

    for (int i = 0; i < 480; i++) {
        for (int j = 0; j < 480; j++) {
            int cor = cores[j / 60];
            tela[i][j] = cor;
        }
    }

    evipo_port* port = evipo_port_create(480, 480, "");
    struct evipo_event event;
    int finished = 0;
    while (!finished && evipo_port_event(port, &event)) {
        switch (event.type) {
        case EV_DRAW:
            evipo_port_draw(port, tela);
            break;                
        case EV_FINISH:
            finished = 1;
            break;
        }
    }
    evipo_port_destroy(port);    
    return 0;
}