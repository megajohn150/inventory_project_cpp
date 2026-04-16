#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "inventory.h"
#include "equipment.h"

class Player
{
private:
    std::string name;
    int money;
    int hp;
    int exp;
    int lvl;
    Inventory* inv;
    Equipment* equip;
    std::string diff;

public:
    Player();
    ~Player();

    void levelUp(int min, int max);

    bool equipFromInventory(int row, int col);

    bool unequipToInventory();

    // setters and getters
    void       setName(const std::string& newName);
    std::string getName() const;
    int        getMoney() const;
    void       setMoney(int newMoney);
    int        getHp() const;
    void       setHp(int newHp);
    void       setInv(Inventory* newInv);
    Inventory* getInv()   const;
    Equipment* getEquip() const;
    std::string getDiff() const;
    void setDiff(const std::string &newDiff);
    int getExp() const;
    void setExp(int newExp);
    int getLvl() const;
    void setLvl(int newLvl);
};

#endif // PLAYER_H
