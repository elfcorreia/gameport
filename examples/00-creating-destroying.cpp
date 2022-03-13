#include <gameport.h>
#include <iostream>
#include <cstring>

int main() {
    std::cout << "started\n";
    gameport* gp = gameport_create(100, 100, nullptr);
    std::cout << "gameport created\n";
    gameport_destroy(gp);
    std::cout << "gameport destroyed\n";
    return 0;
}