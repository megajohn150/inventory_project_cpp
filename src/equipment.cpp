#include "equipment.h"
#include <iostream>
#include <cctype>

Equipment::Equipment()
    : armor(nullptr), melee(nullptr), ranged(nullptr), cursorSlot(0)
{}

Equipment::~Equipment()
{
    delete armor;
    delete melee;
    delete ranged;
}
Item* Equipment::getSelectedItem(){
    switch(getCursorSlot()){
    case 0: return armor;
    case 1: return ranged;
    case 2: return melee;
    default: return nullptr;
    }
}

Item* Equipment::equipItem(Item* newItem)
{
    if (!newItem) return nullptr;

    const std::string& cat = newItem->getCategory();

    if (cat == "armor") {
        Item* old = armor;
        armor = newItem;
        return old;
    }
    if (cat == "close range weapons") {
        Item* old = melee;
        melee = newItem;
        return old;
    }
    if (cat == "long range weapons") {
        Item* old = ranged;
        ranged = newItem;
        return old;
    }

    return newItem;
}

Item* Equipment::unequipSelected()
{
    switch (cursorSlot) {
    case 0: { Item* old = armor;  armor  = nullptr; return old; }
    case 1: { Item* old = ranged; ranged = nullptr; return old; }
    case 2: { Item* old = melee;  melee  = nullptr; return old; }
    }
    return nullptr;
}

void Equipment::moveCursor(int dir)
{
    cursorSlot += dir;
    if (cursorSlot < 0) cursorSlot = 2;
    if (cursorSlot > 2) cursorSlot = 0;
}

void Equipment::clear()
{
    delete armor;  armor  = nullptr;
    delete melee;  melee  = nullptr;
    delete ranged; ranged = nullptr;
}

void Equipment::display(bool active) const
{
    auto ch = [](Item* item) -> char {
        if (!item) return ' ';
        return char(std::toupper(item->getName()[0]));
    };

    char a = ch(armor);
    char m = ch(melee);
    char r = ch(ranged);

    auto slotStr = [&](int s, char open, char close, char content) -> std::string {
        std::string res;
        res += (active && cursorSlot == s) ? '>' : open;
        res += content;
        res += (active && cursorSlot == s) ? '<' : close;
        return res;
    };

    // slot 0 = head  (armor)
    // slot 1 = left   (ranged)
    // slot 2 = right  (melee,  right)
    std::string head      = slotStr(0, '[', ']', a);
    std::string leftHand  = slotStr(1, '{', '}', r);
    std::string rightHand = slotStr(2, '{', '}', m);


    std::cout << "         " << head                                        << "\n";
    std::cout << "     [" << a << "][" << a << a << a << "][" << a << "]\n";
    std::cout << "     " << leftHand << "[" << a << a << a << "]" << rightHand << "\n";
    std::cout << "        [" << a << "|" << a << "]\n";
    std::cout << "        [" << a << "|" << a << "]\n";


}
