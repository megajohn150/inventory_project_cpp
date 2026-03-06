#include "item.h"
#include "random.h"

namespace Color {
const std::string RED     = "\033[31m";
const std::string GREEN = "\033[92m";
const std::string YELLOW  = "\033[93m";
const std::string BLUE = "\033[38;5;75m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN    = "\033[36m";
const std::string WHITE   = "\033[37m";
const std::string BOLD    = "\033[1m";
const std::string RESET   = "\033[0m";
}

static std::string rarityColor(Rarity r) {
    switch (r) {
    case common:    return Color::WHITE;
    case uncommon:  return Color::GREEN;
    case rare:      return Color::BLUE;
    case epic:      return Color::MAGENTA;
    case legendary: return Color::YELLOW;
    case unknown:   return Color::CYAN;
    default:        return Color::WHITE;
    }
}

static std::string durabilityColor(int dur) {
    if (dur > 60) return Color::GREEN;
    if (dur > 30) return Color::YELLOW;
    if (dur > 0) return Color::RED;
    return Color::MAGENTA;
}

static std::string medkitColor(std::string name){
    if(name == "Small First-Aid kit") return Color::GREEN;
    if(name == "Medium First-Aid kit") return Color::YELLOW;
    return Color::RED;
}

std::string Item::getName() const
{
    return name;
}

void Item::setName(const std::string &newName)
{
    name = newName;
}

Rarity Item::getRarity() const
{
    return rarity;
}

void Item::setRarity(Rarity newRarity)
{
    rarity = newRarity;
}

void Item::upgradeType()
{
    switch (this->type) {
    case wooden:
        this->type = stone;
        std::cout << "Successfully upgraded " << this->name << " from wooden to stone!";
        break;
    case stone:
        this->type = iron;
        std::cout << "Successfully upgraded " << this->name << " from stone to iron!";
        break;
    case iron:
        this->type = gold;
        std::cout << "Successfully upgraded " << this->name << " from iron to gold!";
        break;
    case gold:
        this->type = diamond;
        std::cout << "Successfully upgraded " << this->name << " from gold to diamond!";
        break;
    case diamond:
        std::cout << "Cannot upgrade, item is already diamond";
        break;
    case unseen:
        std::cout << "Cannot upgrade, item is already unseen";
        break;
    default:
        std::cout << "Cannot upgrade item";
        break;
    }
}

int Item::getDurability() const
{
    return durability;
}

void Item::setDurability(int newDurability)
{
    durability = newDurability;
}



int Item::getPrice() const
{
    return price;
}

void Item::setPrice(int newPrice)
{
    price = newPrice;
}


void Item::use()
{
    int dmg = 0;

    switch (this->type)
    {
    case wooden:
        dmg = Random::range(1, 10);
        break;
    case stone:
        dmg = Random::range(1, 8);
        break;
    case iron:
        dmg = Random::range(1, 6);
        break;
    case gold:
        dmg = Random::range(1, 4);
        break;
    case diamond:
        dmg = Random::range(1, 2);
        break;
    case unseen:
        dmg = 1;
        break;
    default:
        return;
    }

    if (this->getDurability() > 0)
        this->setDurability(this->getDurability() - dmg);
}



void Item::showInfo()
{
    if(category != "medkit"){
    std::cout << "Item - " << Item::getTypeString() << " " <<this->name << " (" << rarityColor(Item::getRarity()) << Item::getRarityString()<<Color::RESET<<")" << "\n";
    std::cout << "Price - " << this->price << "\n";
    std::cout << "Durability - " << durabilityColor(this->durability)
              << this->durability << Color::RESET<< "/100"  << "\n";
    }
    else {
        std::cout << "Item - " << medkitColor(this->name) << this->name << Color::RESET
                  << " (heals " << (this->name == "Small First-Aid kit" ? " 40 HP" : this->name == "Medium First-Aid kit" ? " 60 HP" : "to full HP") << ")" << "\n";
        std::cout << "Price - " << this->price << "\n";
    }
}

std::string Item::getRarityString()
{
    switch (this->rarity) {
    case common:
        return "common";
        break;
    case uncommon:
        return "uncommon";
        break;
    case rare:
        return "rare";
        break;
    case epic:
        return "epic";
        break;
    case legendary:
        return "legendary";
        break;
    case unknown:
        return "unknown";
        break;
    default:
        return "common";
        break;
    }
}

std::string Item::getTypeString()
{
    switch (this->type) {
    case wooden:
        return "Wooden";
        break;
    case stone:
        return "Stone";
        break;
    case iron:
        return "Iron";
        break;
    case gold:
        return "Gold";
        break;
    case diamond:
        return "Diamond";
        break;
    case unseen:
        return "Unseen";
        break;
    default:
        return "Wooden";
        break;
    }
}

std::string Item::getCategory() const
{
    return category;
}

void Item::setCategory(const std::string &newCategory)
{
    category = newCategory;
}

Type Item::getType() const
{
    return type;
}

void Item::setType(Type newType)
{
    type = newType;
}

int Item::getStock() const
{
    return stock;
}

void Item::setStock(int newStock)
{
    stock = newStock;
}

Item::Item() {
    name = "random";
    price = 1;
    durability = 100;
    rarity = common;
    type = wooden;
    category = "random";
    stock = Random::range(1,5);
}


Item::Item(const std::string &name, int price, const std::string &category)
    : name(name),
    durability(100),
    price(price),
    category(category),
    stock(Random::range(1,5))

{
    int randNum = Random::range(1,100);
    if(randNum <= 50) {
        rarity = common;
    }
    else if(randNum > 50 && randNum <= 78) {
        rarity = uncommon;
    }
    else if(randNum > 78 && randNum <= 92) {
        rarity = rare;
    }
    else if(randNum > 92 && randNum <= 98) {
        rarity = epic;
    }
    else{
        rarity = legendary;
    }
    type = wooden;
}
