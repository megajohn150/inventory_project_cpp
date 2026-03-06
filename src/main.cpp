//#include <iostream>
#include "game.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#ifdef _WIN32
#include "windows.h"
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); //Sets console to UTF-8 mode to avoid issues with map
#endif
    ma_engine engine;
    ma_engine_init(NULL, &engine);
    ma_engine_play_sound(&engine, "music/music.mp3", NULL);
    Game game;
    game.setPlayerName();
    game.play();
    ma_engine_uninit(&engine);
    return 0;
}

