#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <vector>
#include <string>

class DisplayList
{
protected:
    std::vector<std::string> names;
    int currentItem;
public:
    DisplayList();
    ~DisplayList();
    int getCurrentItem() const;
    void setCurrentItem(int newCurrentItem);

    std::vector<std::string> getNames() const;
    void setNames(const std::vector<std::string> &newNames);
    void add(std::string name);
    void displayMenu(const std::string& playerName, const std::string& difficulty);
    void displayFilters();
    void displayDiff();
    void move(std::string dir);
    std::string getCurrentItemName();
};

#endif // DISPLAYLIST_H
