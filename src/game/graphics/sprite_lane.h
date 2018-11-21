#pragma once
#include "sprite.h"
#include "game/scroll/scroll.h"
#include <vector>
#include <memory>

class SpriteLaneVertical: public SpriteStatic
{
private:
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
        unsigned animRows, unsigned animCols, bool animVerticalIndexing = false,
        double basespeed = 1.0, double lanespeed = 1.0,
        unsigned frameTime = 0, eTimer timer = eTimer::MUSIC_BEAT);

public:
    void updateNoteRect(hTime t, vScroll* ps, size_t channel);
    virtual void draw() const;

public:
    constexpr void setHiSpeed(double s) { _hispeed = s; }
};