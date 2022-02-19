#include <iostream>
#include <cstring>
#include "evipo.h"

int main() {
    int tela[8][8];
    memset(tela, 0xffffff, sizeof(tela));    

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {            
            if (i % 2) {
                tela[i][j] = j % 2 ? 0xffffff : 0x000000;             
            } else {
                tela[i][j] = j % 2 ? 0x000000 : 0xffffff;
            }
        }
    }

    viewport* v = vcreate(8, 8, "verbose;pixel_size=27");
    
    // one shot sync
    vsync(v, tela);
    while (!getc(stdin) == 'd');
    
    vdestroy(v);    
    return 0;
}