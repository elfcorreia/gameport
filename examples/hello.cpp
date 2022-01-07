#include <easyview.h>
#include <iostream>
#include <cstring>

int main() {
    int tela[6][6];
    memset(tela, 0xffffff, sizeof(tela));

    for (int i = 0; i < 6; i++) {
        tela[i][0] = 0xff0000;
        tela[i][1] = 0xffff00;
        tela[i][2] = 0x00ff00;
        tela[i][3] = 0x00ffff;
        tela[i][4] = 0x0000ff;
        tela[i][5] = 0xffffff;
    }

    ev_init(300, 300, "");
    ev_buffer(tela, 6, 6);
    ev_sync(tela);
    ev_finish();
    
    return 0;
}