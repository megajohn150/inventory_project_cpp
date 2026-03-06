#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "shop.h"
#include "displaylist.h"
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
