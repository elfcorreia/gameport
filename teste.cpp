#include "canvas.h"
#include <sstream>

using std::stringstream;

void teste_stroke_rect() {
    const int altura = 50;
    const int largura = 50;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            stroke_rect(i*altura, j*largura, altura, largura);            
        }
    }
}

void teste_fill_rect() {
    const int altura = 50;
    const int largura = 50;    
    int x = 50;
    int y = 50;
    
    set_alpha(128);
    set_color(255, 0, 0);    
    fill_rect(x, y, altura, largura);
    x += 10; y += 10;
    set_color(0, 255, 0);
    fill_rect(x, y, altura, largura);
    x += 10; y += 10;
    set_color(0, 0, 255);
    fill_rect(x, y, altura, largura);        
}

void teste_caminhada_aleatoria() {
    int x = 50, y = 50;
    begin_path();
    move_to(50, 50);
    for (int i = 0; i < 100; i++) {                
        line_to(x, y);
        x += 10 - (rand() % 20);
        y += 10 - (rand() % 20);
    }
    stroke();
}

void teste_casa() {
    set_line_width(10);

    // parede
    stroke_rect(75, 140, 150, 110);

    // porta
    fill_rect(130, 190, 40, 60);

    // telhado
    begin_path();
    move_to(50, 140);
    line_to(150, 60);
    line_to(250, 140);
    close_path();
    stroke();
}



// void teste_fill_arc() {    
//     set_color(148, 0, 211);
//     fill_arc(200, 200, 100 + 80, 0, 180);
//     set_color(75, 0, 130);
//     fill_arc(200, 200, 100 + 70, 0, 180);
//     set_color(0, 0, 255);
//     fill_arc(200, 200, 100 + 60, 0, 180);
//     set_color(255, 255, 0);
//     fill_arc(200, 200, 100 + 50, 0, 180);
//     set_color(0, 255, 0);
//     fill_arc(200, 200, 100 + 40, 0, 180);
//     set_color(255, 127, 0);
//     fill_arc(200, 200, 100 + 30, 0, 180);
//     set_color(255, 0, 0);
//     fill_arc(200, 200, 100 + 20, 0, 180);
//     set_color(255, 255, 255);
//     fill_arc(200, 200, 100 + 10, 0, 180);
// }

int main() {
    canvas_set_title("Teste Canvas");
    canvas_set_size(500, 500);
    canvas_set_pos(100, 100);
    canvas_create();

    //teste_lines();
    //teste_stroke_rect();
    //teste_fill_rect();
    //teste_stroke_arc();
    //teste_fill_arc();
    //teste_caminhada_aleatoria();
    //teste_casa();
    

    canvas_destroy();
}