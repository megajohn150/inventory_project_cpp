#include "displaylist.h"
#include <iostream>
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

void DisplayList::displayMenu()
{
    for(int i = 0; i < names.size(); i++){
        if(currentItem == i){
            std::cout << " > ";
        }
        else{
            std::cout << " ";
        }std::cout << names[i] << "\n";
        if(i == 1)
            std::cout << "~~~~~~~~~~~~~\n";
        if(i == 3)
            std::cout << "~~~~~~~~~~~~~\n";
        if(i == 5)
            std::cout << "~~~~~~~~~~~~~\n";

    }
}

void DisplayList::displayFilters()
{
    for(int i = 0; i < names.size(); i++){
        if(currentItem == i){
            std::cout << " > ";
        }
        else{
            std::cout << " ";
        }
        std::cout << names[i] << "\n";
    }
}

void DisplayList::move(std::string dir)
{
    if(dir == "down"){
        currentItem++;
        if(currentItem >= names.size()){
            currentItem = 0;
        }
    }
    else{
        currentItem--;
        if(currentItem >= names.size()){
            currentItem = getNames().size()-1;
        }
    }

}
