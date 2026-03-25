#include "displaylist.h"
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
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

std::vector<std::string> DisplayList::getNames() const
{
    return names;
}
void DisplayList::setNames(const std::vector<std::string> &newNames)
{
    names = newNames;
}

int DisplayList::getCurrentItem() const
{
    return currentItem;
}

void DisplayList::setCurrentItem(int newCurrentItem)
{
    currentItem = newCurrentItem;
}

DisplayList::DisplayList() {
    currentItem = 0;
}

DisplayList::~DisplayList()
{

}

void DisplayList::add(std::string name)
{
    names.push_back(name);
}

#include <sys/ioctl.h>

// void DisplayList::displayMenu(const std::string& playerName, const std::string& difficulty)
// {
//     const int WIDTH = 28;

//     struct winsize w;
//     ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//     int termWidth = w.ws_col;
//     int termHeight = w.ws_row;

//     int separators = 0;
//     for (int i = 0; i < (int)names.size(); i++)
//         if (i == 1 || i == 3 || i == 5) separators++;
//     int BOX_HEIGHT = 5 + (int)names.size() + separators;

//     int leftPad = /*(termWidth - WIDTH - 2) / 2;*/ 0
//         int topPad = /*(termHeight - BOX_HEIGHT) / 2;*/ 0;

//     std::string margin = std::string(leftPad > 0 ? leftPad : 0, ' ');

//     auto repeat = [](const std::string& s, int n) {
//         std::string result;
//         for (int i = 0; i < n; i++) result += s;
//         return result;
//     };

//     auto padLine = [&](const std::string& text) {
//         int padding = WIDTH - (int)text.size();
//         return text + std::string(padding > 0 ? padding : 0, ' ');
//     };

//     for (int i = 0; i < topPad; i++) std::cout << "\n";

//     std::string welcome = "Welcome, " + playerName + "!";
//     std::string diffText = "Difficulty: " + difficulty;
//     std::string diffColor = (difficulty == "Easy") ? Color::GREEN :
//                                 (difficulty == "Hard") ? Color::RED : Color::BOLD;

//     std::cout << margin << Color::CYAN << Color::BOLD;
//     std::cout << "╔" << repeat("═", WIDTH) << "╗\n";

//     std::cout << margin << "║" << Color::RESET;
//     int welcomePad = (WIDTH - (int)welcome.size()) / 2;
//     std::cout << std::string(welcomePad, ' ') << welcome
//               << std::string(WIDTH - welcomePad - (int)welcome.size(), ' ');
//     std::cout << Color::CYAN << Color::BOLD << "║\n";

//     std::cout << margin << "║" << Color::RESET << Color::BOLD;
//     int titlePad = (WIDTH - 9) / 2;
//     std::cout << std::string(titlePad, ' ') << "MAIN MENU"
//               << std::string(WIDTH - titlePad - 9, ' ');
//     std::cout << Color::CYAN << Color::BOLD << "║\n";

//     std::cout << margin << "║" << Color::RESET;
//     int diffPad = (WIDTH - (int)diffText.size()) / 2;
//     std::cout << std::string(diffPad, ' ') << "Difficulty: "
//               << Color::BOLD << (difficulty == "Easy" ? Color::GREEN : Color::RED) << difficulty
//               << Color::RESET
//               << std::string(WIDTH - diffPad - (int)diffText.size(), ' ');
//     std::cout << Color::CYAN << Color::BOLD << "║\n";

//     std::cout << margin << "╠" << repeat("═", WIDTH) << "╣\n";
//     std::cout << Color::RESET;

//     for (int i = 0; i < (int)names.size(); i++) {
//         std::string prefix = (currentItem == i) ? " > " : "   ";
//         std::string content = prefix + names[i];

//         std::cout << margin << Color::CYAN << Color::BOLD << "║" << Color::RESET;
//         if (currentItem == i) std::cout << Color::BOLD;
//         std::cout << padLine(content);
//         std::cout << Color::CYAN << Color::BOLD << "║\n" << Color::RESET;

//         if (i == 1 || i == 3 || i == 5) {
//             std::cout << margin << Color::CYAN << Color::BOLD;
//             std::cout << "╠" << repeat("─", WIDTH) << "╣\n";
//             std::cout << Color::RESET;
//         }
//     }

//     std::cout << margin << Color::CYAN << Color::BOLD;
//     std::cout << "╚" << repeat("═", WIDTH) << "╝\n";
//     std::cout << Color::RESET;
// }

void DisplayList::displayMenu(const std::string& playerName, const std::string& difficulty){

    auto repeat = [](const std::string& s, int n) {
        std::string result;
        for (int i = 0; i < n; i++) result += s;
        return result;
    };
    auto c = Color::CYAN;
    auto b = Color::BOLD;
    auto r = Color::RESET;
    auto w = Color::WHITE;
    std::cout << c << b <<"╔══════════════════════════╗\n" << r;

    std::string welcome = "Welcome, " + playerName +"!";
    int total = 26;
    int free = total - welcome.length();
    int freeleft, left, right;
    if(free % 2 == 0){
        left = free / 2;
        right = free / 2;
    }
    else{
        freeleft = free - 1;
        left = freeleft / 2;
        right = left + 1;
    }

    std::cout << b << c <<"║"<<r << repeat(" ", left) << welcome << repeat(" ", right) <<  c << b <<"║\n";
    std::cout << "║        "<< r << b <<"MAIN  MENU"<< r << b << c<<"        ║\n";
    std::cout << "║" <<  r <<"     Difficulty: ";
    std::cout << b << Color::YELLOW;
    std::cout << ((difficulty == "Easy") ? "Easy" : "Hard");
    std::cout << r << c << b << "     ║\n";
    std::cout << "╚══════════════════════════╝\n\n";
    for(int i = 0; i < names.size(); i++){
        if(i == 0){
            std::cout << r << Color::GRAY << " [ COMBAT ]\n" << r;
        }
        if(i == 2){
            std::cout << r << Color::GRAY << " [ GET STRONGER ]\n" << r;
        }
        if(i == 4){
            std::cout << r << Color::GRAY << " [ EXPLORE ]\n" << r;
        }
        if(i == 6){
            std::cout << r << Color::GRAY << " [ SYSTEM ]\n" << r;
        }


        if(currentItem == i) {
            std::cout << r<< b << w << "  >  " << r;
        } else {
            std::cout << "     ";
        }
        if(currentItem == i){
            std::cout << r <<b << w << names[i] << r;
        } else {
            std::cout << r << names[i];
        }
        std::cout << "\n";
    }
}

void DisplayList::displayFilters()
{
    for(int i = 0; i < names.size(); i++) {
        if(currentItem == i) {
            std::cout << " > ";
        } else {
            std::cout << " ";
        }
        std::cout << names[i] << "\n";
    }
}

void DisplayList::displayDiff()
{
    for(int i = 0; i < names.size(); i++){
        if(currentItem == i) {
            std::cout << " > ";
        } else {
            std::cout << "   ";
        }
        if(i == 0){
            std::cout << Color::GREEN << names[i] << Color::RESET << "\n";
        }
        else if(i == 1){
            std::cout << Color::BOLD << Color::RED << names[i] << Color::RESET << "\n";
        }
    }
}



void DisplayList::move(std::string dir)
{
    if(dir == "down") {
        currentItem++;
        if(currentItem >= (int)names.size()) {
            currentItem = 0;
        }
    }
    else {
        currentItem--;
        if(currentItem < 0) {
            currentItem = (int)names.size() - 1;
        }
    }
}

std::string DisplayList::getCurrentItemName()
{
    return names[currentItem];
}
