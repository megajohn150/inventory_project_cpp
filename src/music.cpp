#include "music.h"
#include <iostream>
namespace Color {
const std::string RED     = "\033[91m";
const std::string GREEN   = "\033[92m";
const std::string YELLOW  = "\033[93m";
const std::string BLUE    = "\033[38;5;75m";
const std::string MAGENTA = "\033[95m";
const std::string CYAN    = "\033[96m";
const std::string WHITE   = "\033[97m";
const std::string BOLD    = "\033[1m";
const std::string GRAY = "\033[38;5;242m";
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

void Music::toggle()
{
    playing = !playing;
}

int Music::getVolume() const
{
    return volume;
}

void Music::setVolume(int newVolume)
{
    volume = newVolume;
}

Music::Music() : DisplayList(){
    currentlyPlaying = 0;
    playing = true;
    volume = 100;
}

Music::~Music()
{

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
    std::cout << Color::RESET <<" Currently playing: " << Color::BOLD<< Color::YELLOW <<names[currentlyPlaying];
    std::cout << Color::RESET << Color::GRAY
              << (playing
                      ? "   " + Color::YELLOW + "▐▐  " + Color::GRAY + "[PLAYING]"
                      : "    " + Color::YELLOW + Color::BOLD + "▶  " + Color::RESET + Color::GRAY + "[PAUSED]")
              << Color::RESET << std::endl;
    std::cout << Color::RESET << Color::GRAY << "─────────────────────────────\n\n" <<Color::RESET;
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
        color = Color::GRAY;
    }
    else{
        color = Color::YELLOW;
    }
    std::cout << Color::RESET << Color::GRAY << "\n VOLUME" <<Color::GRAY<< " [" << repeat("█", (volume/10)) << repeat("░", (10-(volume/10))) <<"] " <<color<<volume<< Color::GRAY <<"/100\n\n";
    std::cout << Color::RESET << Color::GRAY <<" [ENTER] play  [P] pause/resume  [DELETE] back" << Color::RESET <<"\n\n";
}
