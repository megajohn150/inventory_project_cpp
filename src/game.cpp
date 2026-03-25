#include "game.h"
#include "charCheck.h"
#include "item.h"
#include "random.h"
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include "json.hpp"
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif
void Game::setPlayer(Player *newPlayer) { player = newPlayer; }
Player *Game::getPlayer() const { return player; }
Shop *Game::getShop() const { return shop; }
void Game::setShop(Shop *newShop) { shop = newShop; }

namespace Color {
const std::string RED     = "\033[91m";
const std::string GREEN   = "\033[92m";
const std::string YELLOW  = "\033[93m";
const std::string BLUE    = "\033[38;5;75m";
const std::string MAGENTA = "\033[95m";
const std::string CYAN    = "\033[96m";
const std::string WHITE   = "\033[97m";
const std::string BOLD    = "\033[1m";
const std::string GRAY = "\033[38;5;245m";
const std::string RESET   = "\033[0m";
}


static nlohmann::json serializeItem(Item* item) {
    if (!item) return nullptr;
    return {
        {"name",       item->getName()},
        {"price",      item->getPrice()},
        {"category",   item->getCategory()},
        {"rarity",     static_cast<int>(item->getRarity())},
        {"type",       static_cast<int>(item->getType())},
        {"durability", item->getDurability()},
        {"stock",      item->getStock()}
    };
}

static Item* deserializeItem(const nlohmann::json& j) {
    if (j.is_null()) return nullptr;
    Item* item = new Item(j["name"], j["price"], j["category"]);
    item->setRarity(static_cast<Rarity>(j["rarity"].get<int>()));
    item->setType(static_cast<Type>(j["type"].get<int>()));
    item->setDurability(j["durability"]);
    if (j.contains("stock")) item->setStock(j["stock"]);
    return item;
}

bool Game::saveGame(const std::string& filename) {
    saveLoadMSG = "";
    nlohmann::json j;
    // Player fields
    j["player"]["name"]  = player->getName();
    j["player"]["money"] = player->getMoney();
    j["player"]["hp"]    = player->getHp();
    // Backpacks
    j["player"]["hasSmallBackpack"] = player->getInv()->getHasSmallBackpack();
    j["player"]["hasLargeBackpack"] = player->getInv()->getHasLargeBackpack();
    // Inventory
    j["player"]["inventory"] = nlohmann::json::array();
    for (int i = 0; i < player->getInv()->getRows(); i++) {
        for (int k = 0; k < player->getInv()->getCols(); k++) {
            Item* item = player->getInv()->getItems()[i][k];
            nlohmann::json slot;
            slot["row"]  = i;
            slot["col"]  = k;
            slot["item"] = serializeItem(item);
            j["player"]["inventory"].push_back(slot);
        }
    }
    // Shop stock
    j["shop"] = nlohmann::json::array();
    for (int i = 0; i < shop->getRows(); i++) {
        Item* it = shop->getItems()[i][0];
        if (it) {
            j["shop"].push_back({{"name", it->getName()}, {"stock", it->getStock()}});
        }
    }
    // Stats
    j["stats"]["clicks"]        = stats.clicks;
    j["stats"]["zombiesKilled"] = stats.zombiesKilled;
    j["stats"]["zombiesHit"]    = stats.zombiesHit;
    j["stats"]["coinsEarned"]   = stats.coinsEarned;
    j["stats"]["coinsStolen"]   = stats.coinsStolen;
    j["stats"]["hpHealed"]      = stats.hpHealed;
    j["stats"]["medkitsUsed"]   = stats.medkitsUsed;
    j["stats"]["itemsBroken"]   = stats.itemsBroken;
    j["stats"]["upgrades"]      = stats.upgrades;
    j["stats"]["repairs"]       = stats.repairs;
    j["stats"]["itemsSold"]     = stats.itemsSold;
    j["stats"]["itemsBought"]   = stats.itemsBought;
    j["stats"]["bossFightsWon"] = stats.bossFightsWon;
    // Equipment
    j["player"]["equipment"]["armor"]  = serializeItem(player->getEquip()->getArmor());
    j["player"]["equipment"]["melee"]  = serializeItem(player->getEquip()->getMelee());
    j["player"]["equipment"]["ranged"] = serializeItem(player->getEquip()->getRanged());

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        saveLoadMSG = "Saved successfully!\n";
        return true;
    }
    else{
        saveLoadMSG = "Unable to write save, file not found!\n";
        return false;
    }
}

bool Game::loadGame(const std::string& filename){
    saveLoadMSG = "";
    std::ifstream file(filename);
    if(!file.is_open()){
        saveLoadMSG = "Something went wrong, file wasn't saved!\n";
        return false;
    }
    nlohmann::json j;
    try{
        file >> j;
    }
    catch(...){
        saveLoadMSG = "Unable to load game! File is corrupted!\n";
        return false;
    }
    // Restore player fields
    player->setName(j["player"]["name"]);
    player->setMoney(j["player"]["money"]);
    player->setHp(j["player"]["hp"]);
    // Clear current state
    player->getInv()->clearInv(player->getEquip());
    // Restore backpacks
    if (j["player"]["hasSmallBackpack"].get<bool>())
        player->getInv()->addSmallBackpack();
    if (j["player"]["hasLargeBackpack"].get<bool>())
        player->getInv()->addLargeBackpack();
    // Restore inventory
    for (auto& slot : j["player"]["inventory"]) {
        if (!slot["item"].is_null()) {
            Item* item = deserializeItem(slot["item"]);
            int row = slot["row"];
            int col = slot["col"];
            player->getInv()->getItems()[row][col] = item;
        }
    }
    // Restore shop stock
    if (j.contains("shop")) {
        for (auto& entry : j["shop"]) {
            std::string name = entry["name"];
            int stock = entry["stock"];
            for (int i = 0; i < shop->getRows(); i++) {
                Item* it = shop->getItems()[i][0];
                if (it && it->getName() == name) {
                    it->setStock(stock);
                    break;
                }
            }
        }
    }
    // Restore stats
    if (j.contains("stats")) {
        stats.clicks        = j["stats"]["clicks"];
        stats.zombiesKilled = j["stats"]["zombiesKilled"];
        stats.zombiesHit    = j["stats"]["zombiesHit"];
        stats.coinsEarned   = j["stats"]["coinsEarned"];
        stats.coinsStolen   = j["stats"]["coinsStolen"];
        stats.hpHealed      = j["stats"]["hpHealed"];
        stats.medkitsUsed   = j["stats"]["medkitsUsed"];
        stats.itemsBroken   = j["stats"]["itemsBroken"];
        stats.upgrades      = j["stats"]["upgrades"];
        stats.repairs       = j["stats"]["repairs"];
        stats.itemsSold     = j["stats"]["itemsSold"];
        stats.itemsBought   = j["stats"]["itemsBought"];
        stats.bossFightsWon = j["stats"].value("bossFightsWon", 0);
    }
    // Restore equipment
    if (!j["player"]["equipment"]["armor"].is_null())
        player->getEquip()->equipItem(deserializeItem(j["player"]["equipment"]["armor"]));
    if (!j["player"]["equipment"]["melee"].is_null())
        player->getEquip()->equipItem(deserializeItem(j["player"]["equipment"]["melee"]));
    if (!j["player"]["equipment"]["ranged"].is_null())
        player->getEquip()->equipItem(deserializeItem(j["player"]["equipment"]["ranged"]));
    saveLoadMSG = "Welcome back, " + player->getName();
    return true;
}

static void displayStats(const Stats& s) {
    std::cout << "<===== Statistics =====>\n\n";
    std::cout << " Clicks in play      : " << s.clicks        << "\n";
    std::cout << " Zombies scared off  : " << Color::GREEN  << s.zombiesKilled << Color::RESET << "\n";
    std::cout << " Zombie attacks      : " << Color::RED    << s.zombiesHit    << Color::RESET << "\n";
    std::cout << " Boss fights won     : " << Color::YELLOW << s.bossFightsWon << Color::RESET << "\n";
    std::cout << " Coins earned        : " << Color::YELLOW << s.coinsEarned   << Color::RESET << "\n";
    std::cout << " Coins stolen        : " << Color::RED    << s.coinsStolen   << Color::RESET << "\n";
    std::cout << " HP healed           : " << Color::GREEN  << s.hpHealed      << Color::RESET << "\n";
    std::cout << " Medkits used        : " << s.medkitsUsed  << "\n";
    std::cout << " Items broken        : " << s.itemsBroken  << "\n";
    std::cout << " Upgrades performed  : " << s.upgrades     << "\n";
    std::cout << " Repairs performed   : " << s.repairs      << "\n";
    std::cout << " Items bought        : " << s.itemsBought  << "\n";
    std::cout << " Items sold          : " << s.itemsSold    << "\n";
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
    if (dur > 0)  return Color::RED;
    return Color::MAGENTA;
}

static std::string hpColor(int hp) {
    if (hp > 60) return Color::GREEN;
    if (hp > 30) return Color::YELLOW;
    return Color::RED;
}

DisplayList Game::getDifficulty() const
{
    return difficulty;
}

void Game::setDifficulty(const DisplayList &newDifficulty)
{
    difficulty = newDifficulty;
}

Game::Game() {
    this->player = new Player();
    this->shop   = new Shop();


    menu.add("Play");
    menu.add("Inventory");
    menu.add("Shop");
    menu.add("Market");
    menu.add("Map");
    menu.add("Guide");
    menu.add("Music");
    menu.add("Save & Load");

    music.add("Music 1");
    music.add("Music 2");
    music.add("Music 3");
    music.add("Music 4");
    music.add("Music 5");
    music.add("Music 6");

    saves.add("Save game");
    saves.add("Load game");

    filters.add("Search item");
    filters.add("Clean up");
    filters.add("Sort by rarity");
    filters.add("Sort by type");
    filters.add("Filter by type");
    filters.add("Filter by rarity");
    filters.add("Clear filters");

    rarities.add("Common");
    rarities.add("Uncommon");
    rarities.add("Rare");
    rarities.add("Epic");
    rarities.add("Legendary");
    rarities.add("Unknown");

    types.add("Wooden");
    types.add("Stone");
    types.add("Iron");
    types.add("Gold");
    types.add("Diamond");
    types.add("Unseen");

    difficulty.add("Easy");
    difficulty.add("Hard");

    shop->addItem(new Item("Sword",   15, "close range weapons"));
    shop->addItem(new Item("Katana",  35, "close range weapons"));
    shop->addItem(new Item("Hammer",  30, "close range weapons"));
    shop->addItem(new Item("Spear",   25, "close range weapons"));
    shop->addItem(new Item("Axe",     30, "close range weapons"));

    shop->addItem(new Item("Bow",      30, "long range weapons"));
    shop->addItem(new Item("Crossbow", 45, "long range weapons"));
    shop->addItem(new Item("Sling",    12, "long range weapons"));

    shop->addItem(new Item("Used armor", 120, "armor"));
    shop->addItem(new Item("Armor",      180, "armor"));

    shop->addItem(new Item("Small First-Aid kit",  15, "medkit"));
    shop->addItem(new Item("Medium First-Aid kit", 30, "medkit"));
    shop->addItem(new Item("Big First-Aid kit",    60, "medkit"));

    shop->addItem(new Item("Small backpack",    90, "backpack"));
    shop->addItem(new Item("Big backpack",    160, "backpack"));

    for(int i = 0; i < shop->getRows(); i++){
        if(shop->getItems()[i][0] != nullptr){
            if(shop->getItems()[i][0]->getCategory() == "backpack"){
                shop->getItems()[i][0]->setStock(1);
            }
        }
    }
}

Game::~Game() {
    delete this->player;
    delete this->shop;
}



static Item* pickActiveWeapon(Item* melee, Item* ranged) {
    if (melee && ranged) {
        int meleeScore  = melee->getType()  * 10 + melee->getRarity();
        int rangedScore = ranged->getType() * 10 + ranged->getRarity();
        return (meleeScore >= rangedScore) ? melee : ranged;
    }
    return melee ? melee : ranged;
}


static int baseEarnByName(const std::string& name) {
    if (name == "Sword")    return 2;
    if (name == "Spear")    return 2;
    if (name == "Hammer")   return 3;
    if (name == "Axe")      return 3;
    if (name == "Katana")   return 4;
    if (name == "Sling")    return 1;
    if (name == "Bow")      return 3;
    if (name == "Crossbow") return 4;
    return 2;
}

static float rarityMultiplier(Rarity r) {
    switch (r) {
    case common:    return 1.0f;
    case uncommon:  return 1.5f;
    case rare:      return 2.5f;
    case epic:      return 4.0f;
    case legendary: return 7.0f;
    case unknown:   return 12.0f;
    default:        return 1.0f;
    }
}

static int zombieSteal(Item* weapon, Item* armor) {
    int base = 10;
    if (weapon) {
        switch (weapon->getType()) {
        case wooden:  base = 8;  break;
        case stone:   base = 6;  break;
        case iron:    base = 4;  break;
        case gold:    base = 2;  break;
        case diamond: base = 1;  break;
        case unseen:  base = 0;  break;
        default:      base = 8;  break;
        }
        float wMult = 1.0f;
        switch (weapon->getRarity()) {
        case common:    wMult = 1.0f;  break;
        case uncommon:  wMult = 0.8f;  break;
        case rare:      wMult = 0.6f;  break;
        case epic:      wMult = 0.35f; break;
        case legendary: wMult = 0.15f; break;
        case unknown:   wMult = 0.0f;  break;
        default:        wMult = 1.0f;  break;
        }
        base = int(base * wMult);
    }

    if (base == 0) return 0;

    if (armor) {
        float armorBase = 0.0f;
        if      (armor->getName() == "Used armor") armorBase = 0.20f;
        else if (armor->getName() == "Armor")      armorBase = 0.40f;
        else                                       armorBase = 0.15f;

        float rarityBonus = 0.0f;
        switch (armor->getRarity()) {
        case common:    rarityBonus = 0.0f;  break;
        case uncommon:  rarityBonus = 0.05f; break;
        case rare:      rarityBonus = 0.10f; break;
        case epic:      rarityBonus = 0.18f; break;
        case legendary: rarityBonus = 0.28f; break;
        case unknown:   rarityBonus = 0.40f; break;
        default:        rarityBonus = 0.0f;  break;
        }

        float totalReduction = std::min(armorBase + rarityBonus, 0.90f);
        base = int(base * (1.0f - totalReduction));
    }

    return std::max(base, 0);
}

static int hpReducing(Item* armor) {
    int base = 0;
    if (armor == nullptr)
        base += Random::range(5, 20);
    return base;
}

static int calculateSellPrice(Item* item) {
    if (!item) return 0;

    float base = item->getPrice() * 0.65f;

    float typeMult = 1.0f;
    switch (item->getType()) {
    case wooden:  typeMult = 1.0f;  break;
    case stone:   typeMult = 1.6f;  break;
    case iron:    typeMult = 2.4f;  break;
    case gold:    typeMult = 3.8f;  break;
    case diamond: typeMult = 6.0f;  break;
    case unseen:  typeMult = 11.0f; break;
    default:      typeMult = 1.0f;  break;
    }

    float rarityMult = 1.0f;
    switch (item->getRarity()) {
    case common:    rarityMult = 1.0f;  break;
    case uncommon:  rarityMult = 1.5f;  break;
    case rare:      rarityMult = 2.2f;  break;
    case epic:      rarityMult = 3.2f;  break;
    case legendary: rarityMult = 5.5f;  break;
    case unknown:   rarityMult = 9.0f;  break;
    default:        rarityMult = 1.0f;  break;
    }

    float durMult = item->getDurability() / 100.0f;
    durMult = std::max(durMult, 0.1f);
    durMult = std::min(durMult, 1.0f);

    return std::max(1, int(base * typeMult * rarityMult * durMult));
}

static void displayPlayHeader(Player* player, Item* activeWeapon, const std::string& lastEvent) {
    std::cout << "<===== Play =====>\n\n";

    if (activeWeapon) {
        std::string slot = (activeWeapon == player->getEquip()->getMelee()) ? "melee" : "ranged";
        std::cout << "Active weapon [" << slot << "]: "
                  << activeWeapon->getTypeString() << " " << activeWeapon->getName()
                  << " (" << rarityColor(activeWeapon->getRarity())
                  << activeWeapon->getRarityString() << Color::RESET << ")"
                  << "  -  Durability: "
                  << durabilityColor(activeWeapon->getDurability())
                  << activeWeapon->getDurability() << Color::RESET << "/100\n";
    } else {
        std::cout << "Active weapon: none\n";
    }

    Item* armor = player->getEquip()->getArmor();
    if (armor) {
        std::cout << "Armor - Durability: "
                  << durabilityColor(armor->getDurability())
                  << armor->getDurability() << Color::RESET << "/100\n";
    } else {
        std::cout << "Armor: none\n";
    }

    std::cout << "Balance: " << player->getMoney()
              << " | Health points: " << hpColor(player->getHp())
              << player->getHp() << Color::RESET << "\n\n";
    std::cout << "Click to fight!!"<<Color::GRAY << "[H] use medkit\n" << Color::RESET;

    if (!lastEvent.empty())
        std::cout << "\n" << lastEvent << "\n";
}
static Item* findBestWeaponInInventory(Player* player) {
    Item* best = nullptr;
    float bestScore = -1.0f;
    for (int i = 0; i < player->getInv()->getRows(); i++) {
        for (int j = 0; j < player->getInv()->getCols(); j++) {
            Item* it = player->getInv()->getItems()[i][j];
            if (!it) continue;
            const std::string& cat = it->getCategory();
            if (cat != "close range weapons" && cat != "long range weapons") continue;
            float score = baseEarnByName(it->getName()) * rarityMultiplier(it->getRarity());
            if (score > bestScore) {
                bestScore = score;
                best = it;
            }
        }
    }
    return best;
}
#ifdef __APPLE__
#define KEY_BACK 127
#else
#define KEY_BACK 8
#endif

void Game::startGame() {
    std::string name;
    const int MAX = 14;
    system(CLEAR);
    std::cout << "Welcome, to start the game, please input your nickname: ";

    while (true) {
        int c = getSingleChar();

        if (c == 10 || c == 13) {
            if (!name.empty()) break;
        } else if (c == 127 || c == 8) {
            if (!name.empty()) {
                name.pop_back();
                std::cout << "\b \b" << std::flush;
            }
        } else if (c >= 32 && c < 127) {
            if ((int)name.size() < MAX) {
                name += (char)c;
                std::cout << (char)c << std::flush;
            }
        }
    }
    this->player->setName(name);
    bool inLoop = true;
    while (inLoop) {
        system(CLEAR);
        std::cout << "Welcome " << name << ", at what difficulty would you like to start your game? (not changeable)\n\n";
        difficulty.displayDiff();
        std::cout << "\n\n\n\n\n\n";
        int userInput = getSingleChar();
        switch (userInput) {
        case 'w':
            difficulty.move("up");
            break;
        case 's':
            difficulty.move("down");
            break;
        case 10: case 13:
            player->setDiff(difficulty.getCurrentItemName());
            if(player->getDiff() == "Easy"){
                playEasy();
            }
            else{
                playEasy();
            }
            inLoop = false;
            break;
        }
    }

}

void Game::playHard(){
    state_inventory();
}

void Game::playEasy() {
    userInput      = 0;
    state          = STATE_MENU;
    showInfo       = false;
    showStats      = false;
    equipMode      = false;
    used           = false;
    used2          = false;
    used3          = false;
    repairPending  = false;
    repairCost     = 0;
    repairTarget   = 0;
    upgradePending = false;
    upgradeCost    = 0;
    upgradeFrom    = "";
    upgradeTo      = "";
    sellPending    = false;
    sellPrice      = 0;
    sellCursor     = 0;
    medCursor      = 0;
    medkitMsg      = "";
    secretCode     = "";
    lastEvent      = "";
    bossClicks     = 0;
    bossInitialized = false;
    bossStartTime  = std::chrono::steady_clock::now();
    bossEndTime    = std::chrono::steady_clock::now();

    while (true) {
        system(CLEAR);

        bool cont = true;
        switch (state) {
        case STATE_MENU:          cont = state_menu();          break;
        case STATE_PLAY:          cont = state_play();          break;
        case STATE_PLAY_MEDKIT:   cont = state_play_medkit();   break;
        case STATE_BOSS:          cont = state_boss();          break;
        case STATE_BOSS_COOLDOWN: cont = state_boss_cooldown(); break;
        case STATE_INVENTORY:     cont = state_inventory();     break;
        case STATE_STORE_SHOP:    cont = state_store_shop();    break;
        case STATE_STORE_SELL:    cont = state_store_sell();    break;
        case STATE_MAP:           cont = state_map();           break;
        case STATE_NAVIGATION:    cont = state_navigation();    break;
        case STATE_SAVE:          cont = state_save();          break;
        case STATE_MUSIC:         cont = state_music();         break;
        }
        if (!cont) return;
    }
}

bool Game::state_music(){
    music.display();
    userInput = int(getSingleChar());
    switch (userInput) {
    case 'w': music.move("up"); break;
    case 's': music.move("down"); break;
    case 'd':
        if(music.getVolume() != 100){
            music.setVolume(music.getVolume()+10);
        }
        break;
    case 'a':
        if(music.getVolume() != 0){
            music.setVolume(music.getVolume()-10);
        }
        break;
    case 'p':
        music.toggle();
        break;
    case 10: case 13:
        music.setCurrentlyPlaying(music.getCurrentItem());
        break;
    case KEY_BACK:
        state = STATE_MENU;
        break;
    }
    return true;
}

// ===================== MENU =====================
bool Game::state_menu() {
    // std::cout << "<============================>\n\n";
    menu.displayMenu(this->player->getName(), player->getDiff());
    std::cout << "\n\n\n";

    userInput = int(getSingleChar());
    switch (userInput) {
    case 'w': menu.move("up"); break;
    case 's': menu.move("down"); break;
    case 10: case 13: {
        int ci = menu.getCurrentItem();
        auto item = menu.getNames()[ci];
        if      (item == "Inventory") state = STATE_INVENTORY;
        else if (item == "Shop")      state = STATE_STORE_SHOP;
        else if (item == "Market")    state = STATE_STORE_SELL;
        else if (item == "Play")      state = STATE_PLAY;
        else if (item == "Map")       state = STATE_MAP;
        else if (item == "Guide")     state = STATE_NAVIGATION;
        else if (item == "Music")     state = STATE_MUSIC;
        else                          state = STATE_SAVE;
        break;
    }
    case KEY_BACK: {
        system(CLEAR);
        std::cout << "Are you sure you want to quit? (press " << Color::RED;
        if (KEY_BACK == 127) std::cout << "delete";
        else                 std::cout << "backspace";
        std::cout << Color::RESET << " to confirm)";
        int confirm = int(getSingleChar());
        if (confirm == KEY_BACK) {
            system(CLEAR);
            return false;
        }
        break;
    }
    }
    return true;
}

// ===================== PLAY =====================
bool Game::state_play() {
    system(CLEAR);

    Item* activeWeapon = pickActiveWeapon(
        player->getEquip()->getMelee(),
        player->getEquip()->getRanged()
        );

    displayPlayHeader(player, activeWeapon, lastEvent);

    userInput = int(getSingleChar());

    if (userInput == KEY_BACK) {
        state     = STATE_MENU;
        lastEvent = "";
        return true;
    }

    if (userInput == 'h' || userInput == 'H') {
        medCursor = 0;
        state = STATE_PLAY_MEDKIT;
        return true;
    }

    if (player->getHp() <= 0) {
        return false;
    }

    Item* checkWeapon = pickActiveWeapon(
        player->getEquip()->getMelee(),
        player->getEquip()->getRanged()
        );
    if (!checkWeapon) {
        if (lastEvent.find("No weapon") != std::string::npos && (userInput == 'e' || userInput == 'E')) {
            Item* best = findBestWeaponInInventory(player);
            if (best) {
                for (int i = 0; i < player->getInv()->getRows(); i++) {
                    for (int j = 0; j < player->getInv()->getCols(); j++) {
                        if (player->getInv()->getItems()[i][j] == best) {
                            player->getInv()->setCurrentRow(i);
                            player->getInv()->setCurrentCol(j);
                            player->equipFromInventory(i, j);
                            lastEvent = "Equipped " + best->getName() + "!";
                            goto doneEquip;
                        }
                    }
                }
            doneEquip:;
            } else {
                lastEvent = "No weapons in inventory!";
            }
        } else {
            Item* best = findBestWeaponInInventory(player);
            if (best) {
                lastEvent = "No weapon equipped! Press [E] to auto-equip best: "
                            + best->getTypeString() + " " + best->getName();
            } else {
                lastEvent = "No weapon equipped and none in inventory!";
            }
        }
        return true;
    }

    stats.clicks++;

    activeWeapon->use();

    if (activeWeapon->getDurability() <= 0) {
        std::string brokenName = activeWeapon->getName();
        stats.itemsBroken++;

        int prevCursor = player->getEquip()->getCursorSlot();
        int targetSlot = (activeWeapon == player->getEquip()->getMelee()) ? 2 : 1;
        while (player->getEquip()->getCursorSlot() != targetSlot)
            player->getEquip()->moveCursor(1);

        Item* broken = player->getEquip()->unequipSelected();
        delete broken;

        while (player->getEquip()->getCursorSlot() != prevCursor)
            player->getEquip()->moveCursor(1);

        lastEvent = "!! " + brokenName + " has broken.";
        return true;
    }

    int earned = std::max(1, int(
        baseEarnByName(activeWeapon->getName()) * rarityMultiplier(activeWeapon->getRarity())
        ));

    int roll = Random::range(1, 100);
    if (roll <= 3) {
        bossClicks    = 0;
        bossStartTime = std::chrono::steady_clock::now();
        state = STATE_BOSS;
        return true;
    }
    else if (roll <= 25) {
        Item* weaponNow = pickActiveWeapon(
            player->getEquip()->getMelee(),
            player->getEquip()->getRanged()
            );
        int stolen = zombieSteal(weaponNow, player->getEquip()->getArmor());
        stolen = std::min(stolen, player->getMoney());
        player->setMoney(player->getMoney() - stolen);

        Item* armorItem  = player->getEquip()->getArmor();
        bool  armorBroke = false;

        int reducedHp = hpReducing(armorItem);
        reducedHp = std::min(reducedHp, player->getHp());
        player->setHp(player->getHp() - reducedHp);

        if (armorItem) {
            armorItem->use();
            if (armorItem->getDurability() <= 0) {
                int prevCursor = player->getEquip()->getCursorSlot();
                while (player->getEquip()->getCursorSlot() != 0)
                    player->getEquip()->moveCursor(1);
                Item* broken = player->getEquip()->unequipSelected();
                delete broken;
                while (player->getEquip()->getCursorSlot() != prevCursor)
                    player->getEquip()->moveCursor(1);
                armorBroke = true;
                stats.itemsBroken++;
            }
        }

        if (stolen == 0) {
            stats.zombiesKilled++;
            player->setMoney(player->getMoney() + earned);
            stats.coinsEarned += earned;
            lastEvent = ">> Zombie attacked! Your equipment scared him off. You earned: "
                        + std::to_string(earned) + "!"
                        + (armorBroke ? " Your armor broke!" : "");
        } else {
            stats.zombiesHit++;
            stats.coinsStolen += stolen;
            if (armorBroke) {
                lastEvent = ">> Zombie attacked! You lost " + std::to_string(stolen)
                + " coins and your armor broke!";
            } else {
                lastEvent = ">> Zombie attacked! You lost " + std::to_string(stolen) + " coins.";
            }
        }
    } else {
        lastEvent = ">> You earned +" + std::to_string(earned) + " coins!";
        player->setMoney(player->getMoney() + earned);
        stats.coinsEarned += earned;
    }

    if (player->getHp() <= 0) {
        player->setMoney(0);
        player->unequipToInventory();
        player->getInv()->clearInv(player->getEquip());

        system(CLEAR);
        std::cout << "Balance:" << player->getMoney() << "| Health points: " << Color::BOLD << Color::RED << "0" << Color::RESET << "\n\n";
        std::cout << Color::RED << Color::BOLD << "YOU DIED!\n" << Color::RESET << "\n";
        displayStats(stats);
        std::cout << "\nPress any key to exit...\n";
        getSingleChar();
        return false;
    }
    return true;
}

// ===================== PLAY MEDKIT OVERLAY =====================
bool Game::state_play_medkit() {
    system(CLEAR);

    Item* activeWeapon = pickActiveWeapon(
        player->getEquip()->getMelee(),
        player->getEquip()->getRanged()
        );
    displayPlayHeader(player, activeWeapon, lastEvent);

    std::cout << "\n~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "FIRST-AID KITS\n\n";

    struct MedEntry {
        Item* item;
        int   invRow;
        int   invCol;
    };
    std::vector<MedEntry> medList;

    for (int i = 0; i < player->getInv()->getRows(); i++) {
        for (int j = 0; j < player->getInv()->getCols(); j++) {
            Item* it = player->getInv()->getItems()[i][j];
            if (it && it->getCategory() == "medkit")
                medList.push_back({it, i, j});
        }
    }

    if (medList.empty()) {
        std::cout << "You have no First-Aid kits.\n\n";
        std::cout << Color::GRAY << " [BACKSPACE] back\n" << Color::RESET;
        userInput = int(getSingleChar());
        if (userInput == KEY_BACK) state = STATE_PLAY;
        return true;
    }

    if (medCursor >= int(medList.size())) medCursor = int(medList.size()) - 1;
    if (medCursor < 0) medCursor = 0;

    const int MED_COLS = 4;
    int totalMeds = int(medList.size());
    int medRows   = (totalMeds + MED_COLS - 1) / MED_COLS;

    for (int i = 0; i < medRows; i++) {
        for (int j = 0; j < MED_COLS; j++) {
            int idx = i * MED_COLS + j;
            if (idx >= totalMeds) { std::cout << "     "; continue; }
            bool selected = (idx == medCursor);
            char ch = char(std::toupper(medList[idx].item->getName()[0]));
            if (selected) std::cout << ">[" << ch << "]<";
            else          std::cout << " [" << ch << "] ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    Item* selMed = medList[medCursor].item;
    std::cout << selMed->getName() << "\n";

    int heals = 0;
    std::string healStr = "";
    if (selMed->getName() == "Small First-Aid kit") {
        heals   = 40;
        healStr = "Heals: +" + std::to_string(heals) + " HP";
    } else if (selMed->getName() == "Medium First-Aid kit") {
        heals   = 60;
        healStr = "Heals: +" + std::to_string(heals) + " HP";
    } else {
        heals   = 100 - player->getHp();
        healStr = "Heals: to full (+" + std::to_string(heals) + " HP)";
    }
    std::cout << Color::GREEN << healStr << Color::RESET << "\n\n";

    if (player->getHp() == 100)
        std::cout << Color::YELLOW << "Already at full HP!\n" << Color::RESET;

    std::cout << Color::GRAY << " [A/D] move  [U] use  [BACKSPACE] back\n" << Color::RESET;

    userInput = int(getSingleChar());
    switch (userInput) {
    case 'a':
        medCursor--;
        if (medCursor < 0) medCursor = int(medList.size()) - 1;
        break;
    case 'd':
        medCursor++;
        if (medCursor >= int(medList.size())) medCursor = 0;
        break;
    case 'u': case 'U': {
        if (player->getHp() < 100) {
            std::string usedMedkitName = selMed->getName();
            int healedAmount = std::min(heals, 100 - player->getHp());
            player->setHp(std::min(player->getHp() + heals, 100));
            player->getInv()->removeItem(medList[medCursor].invRow, medList[medCursor].invCol);
            stats.medkitsUsed++;
            stats.hpHealed += healedAmount;
            lastEvent = Color::GREEN + "Used " + usedMedkitName
                        + "! HP: " + std::to_string(player->getHp()) + "/100" + Color::RESET;
            medCursor = 0;
            state = STATE_PLAY;
        }
        break;
    }
    case KEY_BACK:
        medCursor = 0;
        state = STATE_PLAY;
        break;
    default:
        break;
    }
    return true;
}

// ===================== BOSS =====================
bool Game::state_boss() {
    if (!bossInitialized) {
        bossStartTime = std::chrono::steady_clock::now();
        bossInitialized = true;
    }

    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - bossStartTime).count();
    double timeLeft = 5.0 - elapsed;

    if (timeLeft <= 0.0) {
        int hpLoss    = std::min(player->getHp(),    Random::range(30, 60));
        int coinsLoss = std::min(player->getMoney(), Random::range(50, 100));
        player->setHp(player->getHp() - hpLoss);
        player->setMoney(player->getMoney() - coinsLoss);

        lastEvent = Color::RED + "BOSS defeated you! Lost "
                    + std::to_string(hpLoss) + " HP and "
                    + std::to_string(coinsLoss) + " coins." + Color::RESET;

        bossEndTime     = std::chrono::steady_clock::now();
        bossInitialized = false;
        state           = STATE_BOSS_COOLDOWN;

        if (player->getHp() <= 0) {
            player->setMoney(0);
            player->unequipToInventory();
            player->getInv()->clearInv(player->getEquip());
            system(CLEAR);
            std::cout << Color::RED << Color::BOLD << "YOU DIED TO THE BOSS!\n" << Color::RESET << "\n";
            displayStats(stats);
            std::cout << "\nPress any key to exit...\n";
            getSingleChar();
            return false;
        }
        return true;
    }

    if (bossClicks >= 30) {
        int coinsWon = Random::range(50, 150);
        player->setMoney(player->getMoney() + coinsWon);

        stats.coinsEarned += coinsWon;
        stats.bossFightsWon++;
        lastEvent = Color::YELLOW + Color::BOLD + "BOSS defeated! You earned +"
                    + std::to_string(coinsWon) + " coins!" + Color::RESET;

        bossEndTime     = std::chrono::steady_clock::now();
        bossInitialized = false;
        state           = STATE_BOSS_COOLDOWN;
        return true;
    }

    system(CLEAR);
    std::cout << "<===== !!! BOSS !!! =====>\n\n";
    std::cout << Color::RED << Color::BOLD;
    std::cout << "  (-.-)  BOSS APPEARED!\n";
    std::cout << Color::RESET << "\n";
    std::cout << "Click " << Color::YELLOW << (30 - bossClicks) << Color::RESET << " more times to defeat him!\n";
    std::cout << "Time left: " << Color::RED << int(timeLeft) + 1 << Color::RESET << "s\n\n";
    std::cout << "Attack! (any key)\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (kbhit()) {
        userInput = int(getSingleChar());

        if (userInput == KEY_BACK) {
            int coinsLoss = std::min(player->getMoney(), Random::range(25, 75));
            player->setMoney(player->getMoney() - coinsLoss);
            lastEvent = Color::YELLOW + "You fled from the boss! Lost "
                        + std::to_string(coinsLoss) + " coins." + Color::RESET;

            bossEndTime     = std::chrono::steady_clock::now();
            bossInitialized = false;
            state           = STATE_BOSS_COOLDOWN;
        } else {
            bossClicks++;
        }
    }
    return true;
}

// ===================== BOSS COOLDOWN =====================
bool Game::state_boss_cooldown() {
    auto now = std::chrono::steady_clock::now();
    double cooldownElapsed = std::chrono::duration<double>(now - bossEndTime).count();

    if (cooldownElapsed < 3.0) {
        system(CLEAR);
        std::cout << "<===== BOSS RESULT =====>\n\n";
        std::cout << lastEvent << "\n\n";
        std::cout << "Continuing in " << (3 - int(cooldownElapsed)) << "...\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        while (kbhit()) { getchar(); }
    } else {
        while (kbhit()) { getchar(); }
        bossClicks = 0;
        state = STATE_PLAY;
    }
    return true;
}

// ===================== INVENTORY =====================
bool Game::state_inventory() {
    std::cout << "<===== " << player->getName() << "'s inventory =====>\n\n";
    std::cout << "Balance: " << player->getMoney()
              << " | HP: " << hpColor(player->getHp())
              << player->getHp() << Color::RESET
              << "                 ";

    if(equipMode){
        std::cout <<Color::GRAY << " [inventory]  " << Color::RESET << ">[EQUIPMENT]<\n\n";
    }
    else{
        std::cout<< ">[INVENTORY]< " << Color::GRAY<< " [equipment]\n\n" << Color::RESET;
    }

    player->getInv()->display(player->getEquip(), equipMode);

    if (showStats) {
        std::cout << "~~~~~~~~~~~~~~~~~~~\n";
        displayStats(stats);
    } else if (showInfo) {
        std::cout << "~~~~~~~~~~~~~~~~~~~\n";
        if (!equipMode) {
            if (auto item = player->getInv()->getItemOnSelectedRC(
                    player->getInv()->getCurrentRow(),
                    player->getInv()->getCurrentCol())) {
                item->showInfo();
            } else {
                std::cout << "Empty slot\n";
            }
        } else {
            int s = player->getEquip()->getCursorSlot();
            Item* sel = nullptr;
            if      (s == 0) sel = player->getEquip()->getArmor();
            else if (s == 1) sel = player->getEquip()->getRanged();
            else             sel = player->getEquip()->getMelee();

            if (sel) {
                sel->showInfo();
            } else {
                std::string slotName = (s == 0) ? "Head (armor)"
                                       : (s == 1) ? "Left arm (ranged)"
                                                  : "Right arm (melee)";
                std::cout << slotName << ": Empty slot.\n";
            }
        }
    }

    if (repairPending) {
        Item* item = !equipMode
            ? player->getInv()->getItemOnSelectedRC(player->getInv()->getCurrentRow(), player->getInv()->getCurrentCol())
            : player->getEquip()->getSelectedItem();
        if (item) {
            std::cout << "~~~~~~~~~~~~~~~~~~~\n";
            std::cout << "Repair: " << item->getName()
                      << "  " << item->getDurability() << "/100 -> " << repairTarget << "/100"
                      << "\nPrice: " << repairCost << " coins\n";
            if (player->getMoney() >= repairCost)
                std::cout << " [R] confirm\n";
            else
                std::cout << Color::RED << "Not enough coins! ("
                          << player->getMoney() << "/" << repairCost << ")" << Color::RESET << "\n";
        } else {
            repairPending = false;
        }
    }

    if (upgradePending) {
        Item* item = !equipMode
            ? player->getInv()->getItemOnSelectedRC(player->getInv()->getCurrentRow(), player->getInv()->getCurrentCol())
            : player->getEquip()->getSelectedItem();
        if (item) {
            std::cout << "~~~~~~~~~~~~~~~~~~~\n";
            std::cout << "UPGRADE: " << item->getName()
                      << "  " << upgradeFrom << " -> " << upgradeTo
                      << "\nPrice: " << upgradeCost << " coins\n";
            if (player->getMoney() >= upgradeCost)
                std::cout << " [P] confirm\n";
            else
                std::cout << Color::RED << "Not enough coins! ("
                          << player->getMoney() << "/" << upgradeCost << ")" << Color::RESET << "\n";
        } else {
            upgradePending = false;
        }
    }

    if (!medkitMsg.empty()) {
        std::cout << "~~~~~~~~~~~~~~~~~~~\n";
        std::cout << medkitMsg << "\n";
    }

    if (!lastEvent.empty()) {
        std::cout << "~~~~~~~~~~~~~~~~~~~\n";
        std::cout << lastEvent << "\n";
    }

    std::cout << "\n" << Color::GRAY;
    if (equipMode) {
        std::cout << " [TAB] inventory  [A/D] move  [E] unequip  [I] info  [P] upgrade  [R] repair  [F] filters etc.  [V] stats\n";
    } else {
        auto hintItem = player->getInv()->getItemOnSelectedRC(
            player->getInv()->getCurrentRow(),
            player->getInv()->getCurrentCol());
        if (hintItem && hintItem->getCategory() == "medkit") {
            std::cout << " [TAB] equipment  [WASD] move  [I] info  [U] use  [F] search inventory  [V] stats\n";
        } else {
            std::cout << " [TAB] equipment  [WASD] move  [E] equip  [I] info  [P] upgrade  [R] repair  [F] filters etc.  [V] stats\n";
        }
    }
    std::cout << Color::RESET;

    userInput = int(getSingleChar());

    if (!equipMode) {
        secretCode += char(userInput);
        if (secretCode.length() > 20)
            secretCode = secretCode.substr(1);

        if (secretCode.find("111") != std::string::npos && !used) {
            if (auto item = player->getInv()->getItemOnSelectedRC(
                    player->getInv()->getCurrentRow(),
                    player->getInv()->getCurrentCol())) {
                if (item->getTypeString() == "Diamond" && player->getMoney() >= 1000) {
                    item->setType(unseen);
                    player->setMoney(player->getMoney() - 1000);
                    secretCode = "";
                    used = true;
                    system(CLEAR);
                    std::cout << Color::BOLD << Color::GREEN << "SECRET KEY (cannot be used more than once): upgraded item type to unseen (cost - 1000 coins)\n" << Color::RESET;
                    getSingleChar();
                } else { secretCode = ""; }
            } else { secretCode = ""; }
        } else if (secretCode.find("111") != std::string::npos && used) {
            secretCode = "";
        }

        if (secretCode.find("222") != std::string::npos && !used2) {
            if (auto item = player->getInv()->getItemOnSelectedRC(
                    player->getInv()->getCurrentRow(),
                    player->getInv()->getCurrentCol())) {
                if (item->getRarity() == common || item->getRarity() == uncommon || item->getRarity() == rare) {
                    secretCode = "";
                } else {
                    item->setRarity(unknown);
                    secretCode = "";
                    used2 = true;
                    system(CLEAR);
                    std::cout << Color::BOLD << Color::GREEN << "SECRET KEY (cannot be used more than once): upgraded item rarity to unknown\n" << Color::RESET;
                    getSingleChar();
                }
            } else { secretCode = ""; }
        } else if (secretCode.find("222") != std::string::npos && used2) {
            secretCode = "";
        }

        if (secretCode.find("999") != std::string::npos && !used3) {
            player->setMoney(player->getMoney() + 500);
            secretCode = "";
            used3 = true;
            system(CLEAR);
            std::cout << Color::BOLD << Color::GREEN << "SECRET KEY (cannot be used more than once): added 500 coins\n" << Color::RESET;
            getSingleChar();
        } else if (secretCode.find("999") != std::string::npos && used3) {
            secretCode = "";
        }
    }

    switch (userInput) {

    case '\t':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        equipMode      = !equipMode;
        break;

    case 'a':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        if (equipMode) player->getEquip()->moveCursor(-1);
        else           player->getInv()->setCurrentCol(player->getInv()->getCurrentCol() - 1);
        break;

    case 'd':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        if (equipMode) player->getEquip()->moveCursor(+1);
        else           player->getInv()->setCurrentCol(player->getInv()->getCurrentCol() + 1);
        break;

    case 'f': {
        auto displayInv = [&]() {
            system(CLEAR);
            std::cout << "<===== " << player->getName() << "'s inventory =====>\n\n";
            std::cout << "Balance: " << player->getMoney()
                      << " | HP: " << hpColor(player->getHp())
                      << player->getHp() << Color::RESET << "                 ";
            if(equipMode){
                std::cout <<Color::GRAY << " [inventory]  " << Color::RESET << ">[EQUIPMENT]<\n\n";
            }
            else{
                std::cout<< ">[INVENTORY]< " << Color::GRAY<< " [equipment]\n\n" << Color::RESET;
            }
            player->getInv()->display(player->getEquip(), equipMode);
            std::cout << "~~~~~~~~~~~~~~~~~~~\n";
        };
        bool inRarities = false;
        bool inTypes    = false;
        bool inFilters  = true;
        while (inFilters) {
            displayInv();
            filters.displayFilters();               // TO DOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
            std::cout << Color::GRAY << "\n [W/S] move  [ENTER] select  [BACKSPACE] back\n" << Color::RESET;
            userInput = int(getSingleChar());
            switch (userInput) {
            case 'w': filters.move("up");   break;
            case 's': filters.move("down"); break;
            case 10: case 13: {
                if (filters.getCurrentItem() == 0) {
                    std::string target;
                    displayInv();
                    std::cout << "Search an item in your inventory: ";
                    while (std::cin.peek() == '\n') std::cin.ignore();
                    std::getline(std::cin, target);
                    auto [givenRow, givenCol] = player->getInv()->searchNames(target);
                    if (givenRow == -1 && givenCol == -1) {
                        displayInv();
                        std::cout << "Item '" << target << "' not found in inventory :(";
                        getSingleChar();
                    } else {
                        player->getInv()->setCurrentRow(givenRow);
                        player->getInv()->setCurrentCol(givenCol);
                        equipMode = false;
                    }
                    inFilters = false;
                }
                else if (filters.getCurrentItem() == 1) {
                    auto inv = player->getInv();
                    lastEvent = inv->invNotEmpCheckup() ? "" : "You can't clean up your inventory because its empty!";
                    if (inv->invNotEmpCheckup()) inv->cleanUp();
                    inFilters = false;
                }
                else if (filters.getCurrentItem() == 2) {
                    auto inv = player->getInv();
                    lastEvent = inv->invNotEmpCheckup() ? "" : "You can't sort items in your inventory because it's empty!";
                    if (inv->invNotEmpCheckup()) inv->sortByRarity();
                    inFilters = false;
                }
                else if (filters.getCurrentItem() == 3) {
                    auto inv = player->getInv();
                    lastEvent = inv->invNotEmpCheckup() ? "" : "You can't sort items in your inventory because it's empty!";
                    if (inv->invNotEmpCheckup()) inv->sortByType();
                    inFilters = false;
                }
                else if (filters.getCurrentItem() == 4) {
                    auto inv = player->getInv();
                    if (!inv->invNotEmpCheckup()) {
                        lastEvent = "You can't filter items in your inventory because it's empty!";
                        inFilters = false;
                        break;
                    }
                    inTypes = true;
                    types.setCurrentItem(0);
                    while (inTypes) {
                        displayInv();
                        std::cout << "~~~~~~~~~~~~~~~~~~~\n";
                        std::cout << "Filter by type:\n";
                        types.displayFilters();
                        std::cout << Color::GRAY << "\n [W/S] move  [ENTER] select  [BACKSPACE] back\n" << Color::RESET;
                        int u = int(getSingleChar());
                        switch (u) {
                        case 'w': types.move("up");   break;
                        case 's': types.move("down"); break;
                        case 10: case 13: {
                            static const Type typeMap[] = { wooden, stone, iron, gold, diamond, unseen };
                            static const std::string typeNames[] = { "Wooden","Stone","Iron","Gold","Diamond","Unseen" };
                            int ci2 = types.getCurrentItem();
                            inv->filterByType(typeMap[ci2]);
                            lastEvent = "Showing " + typeNames[ci2] + " items only.";
                            inTypes   = false;
                            inFilters = false;
                            break;
                        }
                        case KEY_BACK: inTypes = false; break;
                        }
                    }
                }
                else if (filters.getCurrentItem() == 5) {
                    auto inv = player->getInv();
                    if (!inv->invNotEmpCheckup()) {
                        lastEvent = "You can't filter items in your inventory because it's empty!";
                        inFilters = false;
                        break;
                    }
                    inRarities = true;
                    rarities.setCurrentItem(0);
                    while (inRarities) {
                        displayInv();
                        std::cout << "~~~~~~~~~~~~~~~~~~~\n";
                        std::cout << "Filter by rarity:\n";
                        rarities.displayFilters();
                        std::cout << Color::GRAY << "\n [W/S] move  [ENTER] select  [BACKSPACE] back\n" << Color::RESET;
                        int u = int(getSingleChar());
                        switch (u) {
                        case 'w': rarities.move("up");   break;
                        case 's': rarities.move("down"); break;
                        case 10: case 13: {
                            static const Rarity rarityMap[] = { common, uncommon, rare, epic, legendary, unknown };
                            static const std::string rarityNames[] = { "Common","Uncommon","Rare","Epic","Legendary","Unknown" };
                            int ci2 = rarities.getCurrentItem();
                            inv->filterByRarity(rarityMap[ci2]);
                            lastEvent  = "Showing " + rarityNames[ci2] + " items only.";
                            inRarities = false;
                            inFilters  = false;
                            break;
                        }
                        case KEY_BACK: inRarities = false; break;
                        }
                    }
                }
                else {
                    auto inv = player->getInv();
                    if (!inv->invNotEmpCheckup()) {
                        lastEvent = "Inventory is empty!";
                    } else if (inv->isFiltered()) {
                        inv->unfilter();
                        lastEvent = "All filters cleared.";
                    } else {
                        lastEvent = "No active filters to clear.";
                    }
                    inFilters = false;
                }
                break;
            }
            case KEY_BACK: inFilters = false; break;
            }
        }
        break;
    }
    case 'm': {
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        showInfo       = false;

        auto playerInv = player->getInv();
        int  grabRow   = playerInv->getCurrentRow();
        int  grabCol   = playerInv->getCurrentCol();

        Item* item = playerInv->getItemOnSelectedRC(grabRow, grabCol);
        if (!item) break;

        Item* grabbed = item;

        bool movingItem = true;
        while (movingItem) {
            system(CLEAR);
            std::cout << "<===== " << player->getName() << "'s inventory =====>\n\n";
            std::cout << "Balance: " << player->getMoney()
                      << " | HP: " << hpColor(player->getHp())
                      << player->getHp() << Color::RESET << "                 ";
            if(equipMode){
                std::cout <<Color::GRAY << " [inventory]  " << Color::RESET << ">[EQUIPMENT]<\n\n";
            }
            else{
                std::cout<< ">[INVENTORY]< " << Color::GRAY<< " [equipment]\n\n" << Color::RESET;
            }

            int  curRow     = playerInv->getCurrentRow();
            int  curCol     = playerInv->getCurrentCol();
            std::string realName = grabbed->getName();
            grabbed->setName(".");
            playerInv->display(player->getEquip(), false);
            grabbed->setName(realName);

            std::cout<< Color::GRAY << "\n [WASD] move  [M] drop\n" << Color::RESET;

            int mv = int(getSingleChar());
            curRow = playerInv->getCurrentRow();
            curCol = playerInv->getCurrentCol();

            if (playerInv->getItemOnSelectedRC(curRow, curCol) != grabbed) {
                bool found = false;
                for (int r = 0; r < playerInv->getRows() && !found; r++) {
                    for (int c = 0; c < playerInv->getCols() && !found; c++) {
                        if (playerInv->getItemOnSelectedRC(r, c) == grabbed) {
                            playerInv->setCurrentRow(r);
                            playerInv->setCurrentCol(c);
                            curRow = r;
                            curCol = c;
                            found  = true;
                        }
                    }
                }
                if (!found) { movingItem = false; break; }
            }

            switch (mv) {
            case 'w': {
                int newRow = curRow - 1;
                if (newRow >= 0) {
                    playerInv->moveTo(curRow, curCol, newRow, curCol);
                    playerInv->setCurrentRow(newRow);
                }
                break;
            }
            case 's': {
                int newRow = curRow + 1;
                if (newRow < playerInv->getRows()) {
                    playerInv->moveTo(curRow, curCol, newRow, curCol);
                    playerInv->setCurrentRow(newRow);
                }
                break;
            }
            case 'a': {
                int newCol = curCol - 1;
                if (newCol >= 0) {
                    playerInv->moveTo(curRow, curCol, curRow, newCol);
                    playerInv->setCurrentCol(newCol);
                }
                break;
            }
            case 'd': {
                int newCol = curCol + 1;
                if (newCol < playerInv->getCols()) {
                    playerInv->moveTo(curRow, curCol, curRow, newCol);
                    playerInv->setCurrentCol(newCol);
                }
                break;
            }
            case 'm':
                movingItem = false;
                break;
            default:
                break;
            }
        }
        break;
    }
    case 'w':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        if (!equipMode) player->getInv()->setCurrentRow(player->getInv()->getCurrentRow() - 1);
        break;

    case 's':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        if (!equipMode) player->getInv()->setCurrentRow(player->getInv()->getCurrentRow() + 1);
        break;

    case 'e': case 'E':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        if (equipMode) {
            int s = player->getEquip()->getCursorSlot();
            Item* slotItem = nullptr;
            if      (s == 0) slotItem = player->getEquip()->getArmor();
            else if (s == 1) slotItem = player->getEquip()->getRanged();
            else             slotItem = player->getEquip()->getMelee();

            if (slotItem && !player->unequipToInventory()) {
                system(CLEAR);
                std::cout << "Inventory full!";
                getSingleChar();
            }
        } else {
            player->equipFromInventory(
                player->getInv()->getCurrentRow(),
                player->getInv()->getCurrentCol()
                );
        }
        break;

    case 'i':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        showInfo       = !showInfo;
        break;

    case 'v': case 'V':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showInfo       = false;
        showStats      = !showStats;
        break;

    case 'p': {
        repairPending = false;
        medkitMsg     = "";
        showStats     = false;
        Item* item = !equipMode
            ? player->getInv()->getItemOnSelectedRC(player->getInv()->getCurrentRow(), player->getInv()->getCurrentCol())
            : player->getEquip()->getSelectedItem();
        if (upgradePending) {
            if (player->getMoney() >= upgradeCost && item) {
                player->setMoney(player->getMoney() - upgradeCost);
                item->upgradeType();
                stats.upgrades++;
                system(CLEAR);
                getSingleChar();
            }
            upgradePending = false;
        } else if (item) {
            std::string type = item->getTypeString();
            int price = 0;
            std::string to = "";
            if      (type == "Wooden") { price = 20;  to = "Stone";   }
            else if (type == "Stone")  { price = 50;  to = "Iron";    }
            else if (type == "Iron")   { price = 120; to = "Gold";    }
            else if (type == "Gold")   { price = 280; to = "Diamond"; }

            if (price > 0) {
                upgradeCost    = price;
                upgradeFrom    = type;
                upgradeTo      = to;
                upgradePending = true;
            }
        }
        break;
    }

    case 'r': {
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        Item* item = !equipMode
            ? player->getInv()->getItemOnSelectedRC(player->getInv()->getCurrentRow(), player->getInv()->getCurrentCol())
            : player->getEquip()->getSelectedItem();
        if (repairPending) {
            if (player->getMoney() >= repairCost && item) {
                player->setMoney(player->getMoney() - repairCost);
                stats.repairs++;
                system(CLEAR);
                std::cout << "Successfully repaired " << item->getName()
                          << " durability from " << item->getDurability() << " to " << repairTarget << "!";
                item->setDurability(repairTarget);
                getSingleChar();
            }
            repairPending = false;
        } else if (item && item->getDurability() < 100) {
            int repairBase = 0;
            switch (item->getType()) {
            case wooden:  repairBase = 10;  break;
            case stone:   repairBase = 25;  break;
            case iron:    repairBase = 60;  break;
            case gold:    repairBase = 150; break;
            case diamond: repairBase = 300; break;
            case unseen:  repairBase = 600; break;
            default:      repairBase = 10;  break;
            }

            int missing  = 100 - item->getDurability();
            int fullCost = std::max(1, int(repairBase * missing / 100.0f));

            if (player->getMoney() >= fullCost) {
                repairCost    = fullCost;
                repairTarget  = 100;
                repairPending = true;
            } else if (player->getMoney() > 0) {
                float ratio   = float(player->getMoney()) / float(fullCost);
                int canFix    = std::max(1, int(missing * ratio));
                repairTarget  = item->getDurability() + canFix;
                repairCost    = player->getMoney();
                repairPending = true;
            }
        }
        break;
    }

    case 'u':
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        if (!equipMode) {
            if (auto item = player->getInv()->getItemOnSelectedRC(
                    player->getInv()->getCurrentRow(),
                    player->getInv()->getCurrentCol())) {
                if (item->getCategory() == "medkit") {
                    if (player->getHp() == 100) {
                        medkitMsg = Color::YELLOW + "Already at full HP!" + Color::RESET;
                    } else {
                        int healAmount = 0;
                        if      (item->getName() == "Small First-Aid kit")  healAmount = 40;
                        else if (item->getName() == "Medium First-Aid kit") healAmount = 60;
                        else                                                 healAmount = 100 - player->getHp();

                        int actualHeal = std::min(healAmount, 100 - player->getHp());
                        player->setHp(std::min(player->getHp() + healAmount, 100));
                        player->getInv()->removeItem(
                            player->getInv()->getCurrentRow(),
                            player->getInv()->getCurrentCol()
                            );
                        stats.medkitsUsed++;
                        stats.hpHealed += actualHeal;
                        medkitMsg = Color::GREEN + "Healed! HP: "
                                    + std::to_string(player->getHp()) + "/100" + Color::RESET;
                    }
                }
            }
        }
        break;

    case KEY_BACK:
        state          = STATE_MENU;
        showInfo       = false;
        showStats      = false;
        equipMode      = false;
        repairPending  = false;
        upgradePending = false;
        lastEvent      = "";
        medkitMsg      = "";
        secretCode     = "";
        break;

    default:
        repairPending  = false;
        upgradePending = false;
        medkitMsg      = "";
        showStats      = false;
        break;
    }
    return true;
}

// ===================== SHOP (buy) =====================
bool Game::state_store_shop() {
    std::cout << "<===== Shop =====>\n\n";
    std::cout << "Your current balance: " << player->getMoney() << "\n";
    shop->display();

    userInput = int(getSingleChar());
    switch (userInput) {
    case 'w': shop->setCurrentRow(shop->getCurrentRow() - 1); break;
    case 's': shop->setCurrentRow(shop->getCurrentRow() + 1); break;
    case 'b': {
        auto src = shop->getItemOnSelectedRC(shop->getCurrentRow(), shop->getCurrentCol());
        if (!src) break;
        int price = src->getPrice();

        if (src->getStock() <= 0) break;

        if (player->getMoney() < price) {
            system(CLEAR);
            std::cout << "Not enough funds to buy " << src->getName() << " (" << price << ")";
            getSingleChar();
            break;
        }

        if (src->getName() == "Small backpack") {
            if (!player->getInv()->addSmallBackpack()) {
                system(CLEAR);
                std::cout << "You already have a small backpack!";
            } else {
                player->setMoney(player->getMoney() - price);
                stats.itemsBought++;
                system(CLEAR);
                src->setStock(0);
                std::cout << "Small backpack bought! +1 inventory column.";
            }
            getSingleChar();
            break;
        }
        if (src->getName() == "Big backpack") {
            if (!player->getInv()->addLargeBackpack()) {
                system(CLEAR);
                std::cout << "You already have a big backpack!";
            } else {
                player->setMoney(player->getMoney() - price);
                stats.itemsBought++;
                system(CLEAR);
                src->setStock(0);
                std::cout << "Big backpack bought! +2 inventory columns.";
            }
            getSingleChar();
            break;
        }

        Item* newItem = new Item(src->getName(), src->getPrice(), src->getCategory());
        if (player->getInv()->addItem(newItem)) {
            player->setMoney(player->getMoney() - price);
            stats.itemsBought++;
            src->setStock(src->getStock() - 1);
            system(CLEAR);
            std::cout << "Successfully bought " << src->getName();
        } else {
            delete newItem;
            system(CLEAR);
            std::cout << "Cannot buy any more items, inventory is full";
        }
        getSingleChar();
        break;
    }
    case KEY_BACK:
        state = STATE_MENU;
        break;
    }
    return true;
}

// ===================== SELL =====================
bool Game::state_store_sell() {
    system(CLEAR);
    std::cout << "<===== Sell =====>\n\n";
    std::cout << "Balance: " << player->getMoney() << "\n\n";

    struct SellEntry {
        Item* item;
        bool  equipped;
        int   source;
        int   invRow;
        int   invCol;
    };

    std::vector<SellEntry> sellList;

    for (int i = 0; i < player->getInv()->getRows(); i++) {
        for (int j = 0; j < player->getInv()->getCols(); j++) {
            Item* it = player->getInv()->getItems()[i][j];
            if (it && it->getCategory() != "backpack" && it->getCategory() != "medkit")
                sellList.push_back({it, false, 0, i, j});
        }
    }
    if (player->getEquip()->getArmor())
        sellList.push_back({player->getEquip()->getArmor(),  true, 1, -1, -1});
    if (player->getEquip()->getRanged())
        sellList.push_back({player->getEquip()->getRanged(), true, 2, -1, -1});
    if (player->getEquip()->getMelee())
        sellList.push_back({player->getEquip()->getMelee(),  true, 3, -1, -1});

    if (sellList.empty()) {
        std::cout << "No items to sell.\n\n";
        std::cout << Color::GRAY << " [BACKSPACE] back\n" << Color::RESET;
        userInput = int(getSingleChar());
        if (userInput == KEY_BACK) {
            sellPending = false;
            state = STATE_MENU;
        }
        return true;
    }

    if (sellCursor >= int(sellList.size())) sellCursor = int(sellList.size()) - 1;
    if (sellCursor < 0) sellCursor = 0;

    const int SELL_COLS = 4;
    int totalItems = int(sellList.size());
    int rows = (totalItems + SELL_COLS - 1) / SELL_COLS;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < SELL_COLS; j++) {
            int idx = i * SELL_COLS + j;
            if (idx >= totalItems) { std::cout << "     "; continue; }
            bool selected = (idx == sellCursor);
            Item* it = sellList[idx].item;
            char  ch = char(std::toupper(it->getName()[0]));
            if (selected) std::cout << ">[" << ch << "]<";
            else          std::cout << " [" << ch << "] ";
        }
        std::cout << "\n";
    }

    std::cout << "~~~~~~~~~~~~~~~~~~~\n";
    Item* sel = sellList[sellCursor].item;
    sel->showInfo();
    std::cout << "Sell price: " << calculateSellPrice(sel) << " coins\n\n";

    if (sellPending)
        std::cout << "[S] confirm\n";
    else
        std::cout << Color::GRAY << " [A/D] move  [S] sell  [BACKSPACE] back\n" << Color::RESET;

    userInput = int(getSingleChar());
    switch (userInput) {
    case 'a':
        sellPending = false;
        sellCursor--;
        if (sellCursor < 0) sellCursor = int(sellList.size()) - 1;
        break;
    case 'd':
        sellPending = false;
        sellCursor++;
        if (sellCursor >= int(sellList.size())) sellCursor = 0;
        break;
    case 's':
        if (sellPending) {
            SellEntry& entry = sellList[sellCursor];
            if (entry.equipped) {
                int prevCursor = player->getEquip()->getCursorSlot();
                int equipSlot  = entry.source - 1;
                while (player->getEquip()->getCursorSlot() != equipSlot)
                    player->getEquip()->moveCursor(1);
                Item* unequipped = player->getEquip()->unequipSelected();
                while (player->getEquip()->getCursorSlot() != prevCursor)
                    player->getEquip()->moveCursor(1);
                player->setMoney(player->getMoney() + sellPrice);
                delete unequipped;
            } else {
                player->setMoney(player->getMoney() + sellPrice);
                player->getInv()->removeItem(entry.invRow, entry.invCol);
            }
            stats.itemsSold++;
            sellPending = false;
            sellCursor  = 0;
        } else {
            sellPrice   = calculateSellPrice(sellList[sellCursor].item);
            sellPending = true;
        }
        break;
    case KEY_BACK:
        sellPending = false;
        sellCursor  = 0;
        state = STATE_MENU;
        break;
    default:
        sellPending = false;
        break;
    }
    return true;
}

// ===================== MAP =====================
bool Game::state_map() {
    std::cout << "<================ Map ================>\n\n";
    std::cout << " _______________________________________\n";
    std::cout << "|     ''                                |\n";
    std::cout << "|           ''             ''           |\n";
    std::cout << "|                                       |\n";
    std::cout << "|                                   M   |\n";
    std::cout << "|     ''                          M M   |\n";
    std::cout << "|               @@                M M   |\n";
    std::cout << "| @       ''    @&@        M    M M M   |\n";
    std::cout << "|@@@            @&@      M  M  M    M   |\n";
    std::cout << "|@&@@             &     M      M     M  |\n";
    std::cout << "|@&@@             M   M              M  |\n";
    std::cout << "| &              M M  M              MM |\n";
    std::cout << "| &             M   M M               M |\n";
    std::cout << "|##             M   MM                M |\n";
    std::cout << "|###~~~~~~~~~~M                       M |\n";
    std::cout << "|####~~~~~~~~~M                       MM|\n";
    std::cout << "|#####~~~~~~~M                          |\n";
    std::cout << "\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\xe2\x80\xbe\n";

    userInput = int(getSingleChar());
    if (userInput == KEY_BACK) state = STATE_MENU;
    return true;
}

// ===================== GUIDE =====================
bool Game::state_navigation() {
    std::cout << "For better user experience, please run this game in bigger screen view\n\n";
    std::cout << "<===== Navigation guide =====>\n\n";
    std::cout << "Movement:\n W, S, A, D or arrow keys\n~~~~~~~~~~~~~\n";
    std::cout << "Inventory functions:\n tab - change select\n i   - toggle info\n v   - toggle stats\n p   - weapon upgrade\n f   - filters\n r   - repair item\n e   - equip/unequip item\n u   - use medkit\n~~~~~~~~~~~~~\n";
    std::cout << "Store functions:\n p - toggle price & stock\n b - buy\n s - sell\n\n";
    std::cout << "<===== Upgrade prices =====>\n\n";
    std::cout << " Wooden -> Stone   - 20\n";
    std::cout << " Stone  -> Iron    - 50\n";
    std::cout << " Iron   -> Gold    - 120\n";
    std::cout << " Gold   -> Diamond - 280\n\n";
    std::cout << "<===== Repair prices =====>\n\n";
    std::cout << " Wooden  - 10\n Stone   - 25\n Iron    - 60\n Gold    - 150\n Diamond - 300\n Unseen  - 600\n\n";
    std::cout << "<===== Item rarity chances =====>\n\n";
    std::cout << " Common    - 50%\n Uncommon  - 28%\n Rare      - 14%\n Epic      -  6%\n Legendary -  2%\n";

    userInput = int(getSingleChar());
    if (userInput == KEY_BACK) state = STATE_MENU;
    return true;
}

// ===================== SAVE =====================
bool Game::state_save() {
    std::cout << "<===== Save or load your game! =====>\n\n~~~~~~~~~~~~~\n";
    saves.displayFilters();
    std::cout << "\n\n\n";

    userInput = int(getSingleChar());
    switch (userInput) {
    case 'w': saves.move("up"); break;
    case 's': saves.move("down"); break;
    case 10: case 13: {
        int ci = saves.getCurrentItem();
        auto item = saves.getNames()[ci];
        if (item == "Save game") {
            system(CLEAR);
            saveGame("savegame.json");
            std::cout << saveLoadMSG << "\n";
            getSingleChar();
        } else {
            system(CLEAR);
            loadGame("savegame.json");
            std::cout << saveLoadMSG << "\n";
            getSingleChar();
        }
    }
    case KEY_BACK: state = STATE_MENU; break;
    }
    return true;
}

// enter : 10  backspace : 8  del : 127
// w:119  a:97  s:115  d:100  i:105  u:117  h:104  v:118
