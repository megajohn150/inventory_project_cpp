#include "player.h"
#include "random.h"
std::string Player::getDiff() const
{
    return diff;
}

void Player::setDiff(const std::string &newDiff)
{
    diff = newDiff;
}

int Player::getExp() const
{
    return exp;
}

void Player::setExp(int newExp)
{
    exp = newExp;
}

int Player::getLvl() const
{
    return lvl;
}

void Player::setLvl(int newLvl)
{
    lvl = newLvl;
}

Player::Player()
{
    this->name = "John";
    this->money = 0;
    this->exp = 0;
    this->lvl = 1;
    this->hp = 100;
    this->inv = new Inventory();
    this->equip = new Equipment();
    this->diff = "";
    inv->addItem(new Item("Sword", 15, "close range weapons"));
}

Player::~Player()
{
    delete inv;
    delete equip;
}

void Player::levelUp(int min, int max)
{
    int value = Random::range(min, max);
    this->exp += value;

    while (this->exp >= 100) {
        this->lvl++;
        this->exp -= 100;
    }
}

bool Player::equipFromInventory(int row, int col)
{
    // Move item from inventory to equipment
    // If slot was occupied, displaced item goes back to inventory
    Item* item = inv->getItemOnSelectedRC(row, col);
    if (!item) return false;

    const std::string& cat = item->getCategory();
    // Only certain item types can be equipped
    if (cat != "armor" &&
        cat != "close range weapons" &&
        cat != "long range weapons")
        return false;

    inv->getItems()[row][col] = nullptr;

    // Equip the item and get the item that was displaced (if any)
    Item* displaced = equip->equipItem(item);

    // Return displaced item to inventory, or delete if no space
    if (displaced) {
        if (!inv->addItem(displaced)) {
            delete displaced;
        }
    }

    return true;
}

bool Player::unequipToInventory()
{
    // Move currently selected equipment item back to inventory
    Item* item = equip->unequipSelected();
    if (!item) return false;

    // If inventory is full, re-equip the item and return failure
    if (!inv->addItem(item)) {
        equip->equipItem(item);
        return false;
    }

    return true;
}

void Player::setName(const std::string& newName) { name = newName; }
std::string Player::getName() const { return name; }
int Player::getMoney() const { return money; }
void Player::setMoney(int newMoney) { money = newMoney; }
int Player::getHp() const { return hp; }
void Player::setHp(int newHp) { hp = newHp; }
void Player::setInv(Inventory* newInv) { inv = newInv; }
Inventory* Player::getInv() const { return inv; }
Equipment* Player::getEquip() const { return equip; }
