//#include <iostream>
#include "game.h"
#ifdef _WIN32
#include "windows.h"
#endif


int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); //Sets console to UTF-8 mode to avoid issues with map
#endif
    Game game;
    game.setPlayerName();
    game.play();

    return 0;
}

