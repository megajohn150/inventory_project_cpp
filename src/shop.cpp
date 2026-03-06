#include "shop.h"
#include <iomanip>
Shop::Shop() : GridBase(15, 1){};


void Shop::displayTemplate(std::string cat){
    for (int i = 0; i < rows; i++){
        if (items[i][0] && items[i][0]->getCategory() == cat){
            if (i == currentRow) std::cout << " > ";
            else                 std::cout << "   ";

            std::cout << std::left << std::setw(25) << items[i][0]->getName();

            if (i == currentRow){
                std::cout << "Price: " << std::setw(8) << items[i][0]->getPrice();
                if (items[i][0]->getStock() == 0)
                    std::cout << "OUT OF STOCK";
                else
                    std::cout << "Stock: " << items[i][0]->getStock();
            }

            std::cout << "\n";
        }
    }
}

void Shop::display()
{
    int total = 0;
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
