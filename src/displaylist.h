#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <vector>
#include <string>

class DisplayList
{
private:
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
    void displayMenu();
    void displayFilters();
    void move(std::string dir);
};

#endif // DISPLAYLIST_H
