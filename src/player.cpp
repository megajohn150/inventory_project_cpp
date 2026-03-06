#include "player.h"

Player::Player()
{
    this->name  = "John";
    this->money = 0;
    this->hp = 100;
    this->inv   = new Inventory();
    this->equip = new Equipment();
    inv->addItem(new Item("Sword", 15, "close range weapons"));
}

Player::Player(std::string name)
{
    this->name  = name;
    this->money = 0;
    this->hp = 100;
    this->inv   = new Inventory();
    this->equip = new Equipment();
    inv->addItem(new Item("Sword", 15, "close range weapons"));
}

Player::~Player()
{
    delete inv;
    delete equip;
}

bool Player::equipFromInventory(int row, int col)
{
    Item* item = inv->getItemOnSelectedRC(row, col);
    if (!item) return false;

    const std::string& cat = item->getCategory();
    if (cat != "armor" &&
        cat != "close range weapons" &&
        cat != "long range weapons")
        return false;

    inv->getItems()[row][col] = nullptr;

    Item* displaced = equip->equipItem(item);

    if (displaced) {
        if (!inv->addItem(displaced)) {
            delete displaced;
        }
    }

    return true;
}

bool Player::unequipToInventory()
{
    Item* item = equip->unequipSelected();
    if (!item) return false;

    if (!inv->addItem(item)) {
        equip->equipItem(item);
        return false;
    }

    return true;
}

void        Player::setName(const std::string& newName) { name = newName; }
std::string Player::getName()  const { return name; }
int         Player::getMoney() const { return money; }
void        Player::setMoney(int newMoney) { money = newMoney; }
int         Player::getHp() const { return hp; }
void        Player::setHp(int newHp) {hp = newHp; }
void        Player::setInv(Inventory* newInv) { inv = newInv; }
Inventory*  Player::getInv()   const { return inv; }
Equipment*  Player::getEquip() const { return equip; }
