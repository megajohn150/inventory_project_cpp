#include "music.h"
#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace Color {
const std::string RED     = "\033[91m";
const std::string GREEN   = "\033[92m";
const std::string YELLOW  = "\033[93m";
const std::string BLUE    = "\033[38;5;75m";
const std::string MAGENTA = "\033[95m";
const std::string CYAN    = "\033[96m";
const std::string WHITE   = "\033[97m";
const std::string BOLD    = "\033[1m";
const std::string GRAY    = "\033[38;5;242m";
const std::string RESET   = "\033[0m";
}

int Music::getCurrentlyPlaying() const
{
    return currentlyPlaying;
}

void Music::setCurrentlyPlaying(int newCurrentlyPlaying)
{
    currentlyPlaying = newCurrentlyPlaying;
}

bool Music::getPlaying() const
{
    return playing;
}

void Music::setPlaying(bool newPlaying)
{
    playing = newPlaying;
}

void Music::toggle() {
    if (playing) pause();
    else {
        if (!soundLoaded) play(currentlyPlaying);
        else resume();
    }
}

int Music::getVolume() const
{
    return volume;
}

void Music::setVolume(int newVolume) {
    if (newVolume < 0) newVolume = 0;
    if (newVolume > 100) newVolume = 100;

    volume = newVolume;

    if (soundLoaded)
        ma_sound_set_volume(&sound, volume / 100.0f);
}

Music::Music() : DisplayList() {
    currentlyPlaying = 0;
    playing = false;
    volume = 100;
    soundLoaded = false;

    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cout << "engine error\n";
    }
}

Music::~Music() {
    if (soundLoaded) {
        ma_sound_uninit(&sound);
    }
    ma_engine_uninit(&engine);
}

void Music::display(){

    auto repeat = [](const std::string& s, int n) {
        std::string result;
        for (int i = 0; i < n; i++) result += s;
        return result;
    };

    std::cout << Color::BOLD << Color::CYAN << "╔═════════════════════════╗\n";
    std::cout << "║          " << Color::RESET << Color::BOLD <<  "MUSIC          " << Color::CYAN <<"║\n";
    std::cout << "╚═════════════════════════╝\n" << Color::RESET;

    // ==== FIXED LINE ====
    std::string name = names[currentlyPlaying];

    std::string status = playing
                             ? Color::YELLOW + std::string("▐▐  ") + Color::GRAY + "[PLAYING]"
                             : Color::YELLOW + std::string(" ▶  ") + Color::GRAY + "[PAUSED]";

    const int totalWidth = 53;

    int baseLen = 19 + name.length();
    int statusLen = 12;

    int spaces = totalWidth - baseLen - statusLen;
    if (spaces < 1) spaces = 1;

    std::cout << Color::RESET
              << " Currently playing: "
              << Color::BOLD << Color::YELLOW << name
              << Color::RESET
              << repeat(" ", spaces)
              << status
              << Color::RESET << std::endl;

    std::cout << Color::RESET << Color::GRAY << " ─────────────────────────────\n\n" <<Color::RESET;

    for(int i = 0; i < names.size(); i++){
        if(currentItem == i) {
            std::cout << Color::BOLD << Color::WHITE << "  >"<< Color::RESET;
        } else {
            std::cout << "   ";
        }

        if(currentlyPlaying == i){
            std::cout << Color::YELLOW << " ♫ " << Color::RESET;
        } else{
            std::cout << "   ";
        }

        if(currentlyPlaying == i){
            std::cout << Color::BOLD << Color::YELLOW;
        }
        else if(currentItem == i){
            std::cout << Color::BOLD << Color::WHITE;
        }

        std::cout << names[i] << Color::RESET << std::endl;
    }

    std::string color;
    if(volume == 0){
        color = Color::GRAY+"  ";
    }
    else if(volume == 100){
        color = Color::YELLOW + Color::BOLD;
    }
    else{
        color = Color::YELLOW+" ";
    }

    std::cout << Color::RESET << Color::GRAY
              << "\n VOLUME [" << repeat("█", (volume/10)) << repeat("░", (10-(volume/10))) << "] "
              << color << volume << Color::GRAY << "/100\n\n";

    std::cout << Color::RESET << Color::GRAY
              << " [ENTER] play  [A/D] adjust volume  [P] pause/resume  [DELETE] back\n\n"
              << Color::RESET;
}

std::string Music::getPath(int id) {
    switch(id) {
    case 0: return "../music/ancientharp.mp3";
    case 1: return "../music/castleexploration.mp3";
    case 2: return "../music/easytravel.mp3";
    case 3: return "../music/fantasyexploration.mp3";
    case 4: return "../music/habitat.mp3";
    case 5: return "../music/smoothwaters.mp3";
    default: return "../music/music.mp3";
    }
}

void Music::play(int id) {
    currentlyPlaying = id;

    if (soundLoaded) {
        ma_sound_uninit(&sound);
        soundLoaded = false;
    }

    std::string path = getPath(id);

    if (ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, &sound) != MA_SUCCESS) {
        std::cout << "error loading file\n";
        return;
    }

    soundLoaded = true;

    ma_sound_set_volume(&sound, volume / 100.0f);
    ma_sound_set_looping(&sound, MA_TRUE);

    ma_sound_start(&sound);
    playing = true;
}

void Music::pause() {
    if (!soundLoaded) return;
    ma_sound_stop(&sound);
    playing = false;
}

void Music::resume() {
    if (!soundLoaded) return;
    ma_sound_start(&sound);
    playing = true;
}

void Music::stop() {
    if (!soundLoaded) return;
    ma_sound_stop(&sound);
    ma_sound_seek_to_pcm_frame(&sound, 0);
    playing = false;
}