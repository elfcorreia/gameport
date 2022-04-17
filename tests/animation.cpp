#include "evipo.hpp"
#include <iostream>
#include <cstring>

int main() {
    int tela[50][50];


    viewport* v = vcreate(300, 300, "");
    while (!vfinished(v)) {
        for (int y = 0; y < 50; y++) {
            for (int x = 0; x < 50; x++) {
                tela[y][x] = rand();
            }
        }
        vsync(v);
    }
    vdestroy(v);
    
    return 0;
}