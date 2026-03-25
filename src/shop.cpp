#include "shop.h"
#include <iomanip>

namespace Color {
const std::string GRAY = "\033[38;5;245m";
const std::string RESET   = "\033[0m";
const std::string WHITE   = "\033[97m";
const std::string BOLD    = "\033[1m";
}

Shop::Shop() : GridBase(15, 1) {}

void Shop::displayTemplate(std::string cat)
{
    for (int i = 0; i < rows; i++){
        if (items[i][0] && items[i][0]->getCategory() == cat){
            if (i == currentRow) std::cout << Color::BOLD << Color::WHITE << " > " << Color::RESET;
            else                 std::cout << "   ";

            if(i == currentRow){
                std::cout << Color::WHITE << Color::BOLD << std::left << std::setw(25) << items[i][0]->getName() << Color::RESET;
            }
            else{
                std::cout << std::left << std::setw(25) << items[i][0]->getName();
            }

            if (i == currentRow){
                std::cout<< Color::GRAY << "Price: " <<  Color::BOLD<< std::setw(8) << items[i][0]->getPrice() << Color::RESET << Color::GRAY;
                if (items[i][0]->getStock() == 0)
                    std::cout << Color::BOLD<< "OUT OF STOCK" << Color::RESET;
                else
                    std::cout << "Stock: " << Color::BOLD << items[i][0]->getStock() << Color::RESET;
            }

            std::cout << "\n";
        }
    }
}

void Shop::display()
{
    std::cout << "~~~~~~~~~~~~~\nClose range weapons:\n";
    displayTemplate("close range weapons");
    std::cout << "~~~~~~~~~~~~~\nLong range weapons:\n";
    displayTemplate("long range weapons");
    std::cout << "~~~~~~~~~~~~~\nDefensive equipment:\n";
    displayTemplate("armor");
    std::cout << "~~~~~~~~~~~~~\nFirst aid kits:\n";
    displayTemplate("medkit");
    std::cout << "~~~~~~~~~~~~~\nBackpacks:\n";
    displayTemplate("backpack");
}
