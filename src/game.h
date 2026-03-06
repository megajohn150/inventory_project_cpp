#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "shop.h"
#include "displaylist.h"

struct Stats {
    int clicks        = 0;
    int zombiesKilled = 0;
    int zombiesHit    = 0;
    int coinsEarned   = 0;
    int coinsStolen   = 0;
    int hpHealed      = 0;
    int medkitsUsed   = 0;
    int itemsBroken   = 0;
    int upgrades      = 0;
    int repairs       = 0;
    int itemsSold     = 0;
    int itemsBought   = 0;
    int bossFightsWon = 0;
};

enum GameState {
    STATE_MENU,
    STATE_INVENTORY,
    STATE_MAP,
    STATE_NAVIGATION,
    STATE_STORE_SHOP,
    STATE_STORE_SELL,
    STATE_PLAY_MEDKIT,
    STATE_PLAY,
    STATE_BOSS,
    STATE_BOSS_COOLDOWN,
    STATE_SAVE
};

class Game
{
private:
    Player* player;
    Shop* shop;
    GameState state;
    DisplayList menu;
    DisplayList filters;
    DisplayList rarities;
    DisplayList types;
    DisplayList saves;
    std::string saveLoadMSG;
    Stats stats;
public:
    Game();
    ~Game();
    void setPlayerName();
    void play();
    void setPlayer(Player *newPlayer);
    Player *getPlayer() const;
    Shop *getShop() const;
    void setShop(Shop *newShop);


    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

};

#endif // GAME_H
