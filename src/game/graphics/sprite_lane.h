#pragma once
#include "sprite.h"
#include "game/scroll/scroll.h"
#include <vector>
#include <memory>

// Draw the whole lane (on screen) from one single note sprite.
// Currently only handles normal notes. LN, mines or others are meant to be managed by other Sprite class.
// Note that an instance of this class handles ONE note lane. That is, a 7+1 chart needs 8 instances of this class.
class SpriteLaneVertical: public SpriteStatic
{
private:
    NoteChannelIndex _channel;
    double _basespd;
    double _hispeed;
    std::list<Rect> _outRect;

private:
    std::shared_ptr<SpriteAnimated> _note;

public:
    SpriteLaneVertical() = delete;

    // note gap = beat_gap * hispeed * lane_height * (basespeed * lanespeed)
    // e.g. 16th gap of 400px with 1.0hs, 1.0*1.0spd is 20px
    SpriteLaneVertical(pTexture texture, Rect laneRect,
        unsigned animRows, unsigned animCols, unsigned frameTime = 0, eTimer timer = eTimer::PLAY_START,
        bool animVerticalIndexing = false,
        double basespeed = 1.0, double lanespeed = 1.0
        );

public:
    void setChannel(NoteChannelIndex ch);
    void getRectSize(int& w, int& h);
    void updateNoteRect(hTime t, vScroll* ps);
    virtual void draw() const;

public:
    constexpr void setHiSpeed(double s) { _hispeed = s; }
};