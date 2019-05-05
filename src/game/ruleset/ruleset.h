#pragma once
#include <vector>
#include "game/input/input_wrapper.h"
#include "game/scroll/scroll.h"
#include "beat.h"

enum class eRuleset
{
    SOUND_ONLY,
    CLASSIC,
};

class vRuleset
{
public:
    struct BasicData
    {
        unsigned score;
        unsigned score2;
        double health;
        double accuracy;
        unsigned combo;
        unsigned maxCombo;
        unsigned hit;       // total notes hit
        unsigned miss;      // total misses
    };
protected:
    vScroll *_scroll;
    BasicData _basic;
    std::vector<unsigned> _judgeCount;
public:
    vRuleset() = delete;
    vRuleset(vScroll* chartObj, size_t judgeCount) :
        _scroll(chartObj), _basic{ 0 }, _judgeCount(judgeCount, 0) {}
    virtual ~vRuleset() = default;
public:
    virtual void updatePress(InputMask& pg, timestamp t) = 0;
    virtual void updateHold(InputMask& hg, timestamp t) = 0;
    virtual void updateRelease(InputMask& rg, timestamp t) = 0;
    virtual void update(timestamp t) = 0;
public:
    constexpr BasicData getData() const { return _basic; }
};
