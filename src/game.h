#ifndef GAME_H
#define GAME_H
#include "player.h"
#include "shop.h"
#include "displaylist.h"
#include <chrono>
#include <string>
#include "music.h"
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
    STATE_SAVE,
    STATE_MUSIC
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
    DisplayList difficulty;
    Music music;

    std::string saveLoadMSG;
    Stats stats;

    // Play-session state
    int  userInput      = 0;
    bool showInfo       = false;
    bool showStats      = false;
    bool equipMode      = false;
    bool used           = false;
    bool used2          = false;
    bool used3          = false;
    bool repairPending  = false;
    int  repairCost     = 0;
    int  repairTarget   = 0;
    bool upgradePending = false;
    int  upgradeCost    = 0;
    std::string upgradeFrom;
    std::string upgradeTo;
    bool sellPending    = false;
    int  sellPrice      = 0;
    int  sellCursor     = 0;
    int  medCursor      = 0;
    std::string medkitMsg;
    std::string secretCode;
    std::string lastEvent;
    int  bossClicks     = 0;
    bool bossInitialized = false;
    std::chrono::steady_clock::time_point bossStartTime;
    std::chrono::steady_clock::time_point bossEndTime;

    bool state_menu();
    bool state_play();
    bool state_play_medkit();
    bool state_boss();
    bool state_boss_cooldown();
    bool state_inventory();
    bool state_store_shop();
    bool state_store_sell();
    bool state_map();
    bool state_navigation();
    bool state_save();
    bool state_music();
public:
    Game();
    ~Game();
    void startGame();
    void playEasy();
    void playHard();

    void setPlayer(Player *newPlayer);
    Player *getPlayer() const;
    Shop *getShop() const;
    void setShop(Shop *newShop);


    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

    DisplayList getDifficulty() const;
    void setDifficulty(const DisplayList &newDifficulty);
};

#endif // GAME_H
