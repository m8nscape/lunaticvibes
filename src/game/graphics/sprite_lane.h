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
protected:
	chart::NoteLaneCategory _category;
    chart::NoteLaneIndex _index;
    int _noteAreaHeight = 500;  // used to calculate note speed
    double _basespd;
    double _hispeed;
    std::list<Rect> _outRect;
    unsigned _currentMeasure = 0;

public:
    unsigned playerSlot;
    std::shared_ptr<SpriteAnimated> pNote;

public:
    SpriteLaneVertical(unsigned player = 0, double basespeed = 1.0, double lanespeed = 1.0);
    SpriteLaneVertical(pTexture texture, Rect laneRect,
        unsigned animFrames, unsigned frameTime = 0, eTimer timer = eTimer::PLAY_START,
        unsigned animRows = 1, unsigned animCols = 1, bool animVerticalIndexing = false,
        unsigned player = 0, double basespeed = 1.0, double lanespeed = 1.0
        );

public:
    void setLane(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx);
    void setHeight(int h) { _noteAreaHeight = h; }
    std::pair<chart::NoteLaneCategory, chart::NoteLaneIndex> getLane() const;
    void getRectSize(int& w, int& h);
	virtual bool update(const Time& t);
    virtual void updateNoteRect(const Time& t);
    virtual void draw() const;
};


class SpriteLaneVerticalLN : public SpriteLaneVertical
{
protected:
    std::list<Rect> _outRectBody, _outRectTail;
public:
    std::shared_ptr<SpriteAnimated> pNoteBody, pNoteTail;

public:
    SpriteLaneVerticalLN(unsigned player = 0, double basespeed = 1.0, double lanespeed = 1.0) : 
        SpriteLaneVertical(player, basespeed, lanespeed) {}

public:
    virtual void updateNoteRect(const Time& t) override;
    virtual void draw() const;
};