#ifndef EQUIPMENT_H
#define EQUIPMENT_H
#include "item.h"
#include <string>

class Equipment
{
private:
    Item* armor;   // category: "armor"
    Item* melee;   // category: "close range weapons"
    Item* ranged;  // category: "long range weapons"

    int cursorSlot;

public:
    Equipment();
    ~Equipment();

    Item* equipItem(Item* newItem);
    Item* getSelectedItem();
    Item* unequipSelected();

    void moveCursor(int dir);

    void clear();

    int   getCursorSlot() const { return cursorSlot; }
    Item* getArmor()      const { return armor; }
    Item* getMelee()      const { return melee; }
    Item* getRanged()     const { return ranged; }

    void display(bool active) const;
};

#endif // EQUIPMENT_H
