#pragma once
#include <vector>
#include "game/input/input_wrapper.h"
#include "common/chart/chart.h"
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
        double health = 1.0;
        double acc;
        double total_acc;
        unsigned combo;
        unsigned maxCombo;
        unsigned hit;       // total notes hit
        unsigned miss;      // total misses
        unsigned totaln;    // total notes expired
    };
protected:
    std::shared_ptr<vChart> _chart;
    std::shared_ptr<vScroll> _scroll;
    BasicData _basic;
    double _minHealth;
    double _clearHealth;
    std::vector<unsigned> _judgeCount;
public:
    vRuleset() = delete;
    vRuleset(std::shared_ptr<vChart> chart, std::shared_ptr<vScroll> scroll, size_t judgeCount) :
        _chart(chart), _scroll(scroll), _basic{ 0 }, _judgeCount(judgeCount, 0) {}
    virtual ~vRuleset() = default;
public:
    virtual void updatePress(InputMask& pg, timestamp t) = 0;
    virtual void updateHold(InputMask& hg, timestamp t) = 0;
    virtual void updateRelease(InputMask& rg, timestamp t) = 0;
    virtual void update(timestamp t) = 0;
public:
    constexpr BasicData getData() const { return _basic; }
    constexpr bool isFinished() const { return _basic.totaln == _scroll->getNoteCount(); }
};
