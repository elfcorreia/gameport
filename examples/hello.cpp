#include <iostream>
#include <cstring>
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

    viewport* v = vcreate(480, 480, "verbose");
    
    // one shot sync
    vsync(v, tela);
    while (!getc(stdin) == 'd');
    
    vdestroy(v);    
    return 0;
}