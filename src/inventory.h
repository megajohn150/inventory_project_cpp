#ifndef INVENTORY_H
#define INVENTORY_H
#include "gridbase.h"
#include "equipment.h"
#include <map>


class Inventory : public GridBase
{
private:
    bool hasSmallBackpack = false;
    bool hasLargeBackpack = false;
    std::map<std::pair<int,int>, std::string> maskedItems;
    void expandCols(int addCols);
    bool filtered = false;

public:
    Inventory();
    int levenshteinDistance(std::string firstString, std::string secondString);
    std::pair<int, int> searchNames(std::string target);
    bool addSmallBackpack();
    bool addLargeBackpack();

    bool getHasSmallBackpack() const { return hasSmallBackpack; }
    bool getHasLargeBackpack() const { return hasLargeBackpack; }

    void display(Equipment* eq = nullptr, bool equipMode = false);
    void display() override { display(nullptr, false); }
    void clearInv(Equipment* eq);
    void cleanUp();
    void sortByRarity();
    void sortByType();

    void filterByRarity(enum Rarity);
    void filterByType(enum Type);

    void filter();
    void unfilter();

    bool invNotEmpCheckup();
    bool isFiltered() const { return filtered; }
};

#endif // INVENTORY_H
