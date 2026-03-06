#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "menu.h"
#include "shop.h"
#include "store.h"
enum GameState {
    STATE_MENU,
    STATE_INVENTORY,
    STATE_STORE,
    STATE_MAP,
    STATE_NAVIGATION,
    STATE_STORE_SHOP,
    STATE_STORE_SELL,
    STATE_PLAY_MEDKIT,
    STATE_PLAY,
    STATE_BOSS,
    STATE_BOSS_COOLDOWN,
    STATE_SAVE,
    STATE_LOAD
};

class Game
{
private:
    Player* player;
    Menu* menu;
    Shop* shop;
    Store* store;
    GameState state;
    std::string saveLoadMSG;

public:
    Game();
    ~Game();
    void setPlayerName();
    void play();
    void setPlayer(Player *newPlayer);
    Player *getPlayer() const;
    Menu *getMenu() const;
    void setMenu(Menu *newMenu);
    Shop *getShop() const;
    void setShop(Shop *newShop);
    Store *getStore() const;
    void setStore(Store *newStore);
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
};

#endif // GAME_H
