#ifndef MUSIC_H
#define MUSIC_H
#include "displaylist.h"

class Music : public DisplayList
{
private:
    int currentlyPlaying;
    bool playing;
    int volume;
public:
    Music();
    ~Music();
    void display();
    int getCurrentlyPlaying() const;
    void setCurrentlyPlaying(int newCurrentlyPlaying);
    bool getPlaying() const;
    void setPlaying(bool newPlaying);
    void toggle();
    int getVolume() const;
    void setVolume(int newVolume);
};

#endif // MUSIC_H
