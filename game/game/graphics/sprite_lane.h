#pragma once
#include "sprite.h"
#include "game/chart/chart.h"
#include <vector>
#include <memory>

// Draw the whole lane (on screen) with only one sprite per key.
// Currently only handles normal notes. LN, mines or others are meant to be managed by other Sprite class.
// Note that an instance of this class handles ONE note lane. That is, a 7+1 chart needs 8 instances of this class.
class SpriteLaneVertical: public SpriteStatic
{
private:
	NoteLaneCategory _category;
    NoteLaneIndex _index;
	bool _haveDst = false;
    double _basespd;
    double _hispeed;
    std::list<Rect> _outRect;
    unsigned _currentMeasure = 0;

public:
    unsigned _playerSlot;
    std::shared_ptr<SpriteAnimated> pNote;

public:
    SpriteLaneVertical() = delete;

    // note gap = beat_gap * hispeed * lane_height * (basespeed * lanespeed)
    // e.g. 16th gap of 400px with 1.0hs, 1.0*1.0spd is g*1.0*400*(1.0*1.0) = 20px (?)
    SpriteLaneVertical(pTexture texture, Rect laneRect,
        unsigned animFrames, unsigned frameTime = 0, eTimer timer = eTimer::PLAY_START,
        bool animVerticalIndexing = false,
        unsigned player = 0, double basespeed = 1.0, double lanespeed = 1.0
        );

public:
    void setLane(NoteLaneCategory cat, NoteLaneIndex idx);
    decltype(_category) getLaneCat() const { return _category; }
    decltype(_index) getLaneIdx() const { return _index; }
    void getRectSize(int& w, int& h);
	virtual bool update(Time t);
    void updateNoteRect(Time t, vChart* ps, double beat, unsigned measure);
    virtual void draw() const;

public:
    constexpr void setHiSpeed(double s) { _hispeed = s; }
	constexpr bool haveDst() { return _haveDst; }
};