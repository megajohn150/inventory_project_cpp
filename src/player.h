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
    Inventory* inv;
    Equipment* equip;

public:
    Player();
    Player(std::string name);
    ~Player();


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
};

#endif // PLAYER_H
