#include "framebuffer.h"
#include <iostream>

using std::cin;
using std::cout;

int main() {

    const int red   = 0xff0000;
    const int green = 0x00ff00;
    const int blue  = 0x0000ff;
    const int white = 0xffffff;
    
    char width = 32;
    int* tela = create(width, width, 16);
    
    for (int i = 0, n = width * width; i < n; i++) {
        tela[i] = white;
    }

    for (int i = 0; i < width; i++) {
        tela[i * width + i] = blue;
    }

    show();

    char a;
    cin >> a;

    return 0;
}