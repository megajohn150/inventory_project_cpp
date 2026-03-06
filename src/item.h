#ifndef ITEM_H
#define ITEM_H
#include <string>
#include <iostream>
#include <time.h>
enum Rarity {
    common,
    uncommon,
    rare,
    epic,
    legendary,
    unknown
};
enum Type {
    wooden,
    stone,
    iron,
    gold,
    diamond,
    unseen
};
class Item
{
protected:
    std::string name;
    Rarity rarity;
    int durability;
    int price;
    Type type;
    int stock;
    std::string category;
    // close range weapons
    // long range weapons
    // defensive equipment

public:
    Item();
    Item(const std::string &name, int price, const std::string &category);
    virtual ~Item() = default;
    //methods
    void use();
    void showInfo();
    std::string getRarityString();
    std::string getTypeString();
    //setters and getters
    std::string getName() const;
    void setName(const std::string &newName);
    Rarity getRarity() const;
    void setRarity(Rarity newRarity);
    void upgradeType();
    int getDurability() const;
    void setDurability(int newDurability);
    int getPrice() const;
    void setPrice(int newPrice);
    std::string getCategory() const;
    void setCategory(const std::string &newCategory);
    Type getType() const;
    void setType(Type newType);
    int getStock() const;
    void setStock(int newStock);
};

#endif // ITEM_H
