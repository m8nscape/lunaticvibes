#pragma once
#include <vector>
#include "game/input/input_wrapper.h"
#include "common/chartformat/chartformat.h"
#include "game/chart/chart.h"
#include "common/beat.h"

enum class eRuleset
{
    SOUND_ONLY,
    BMS,
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
        unsigned totalnr;    // total notes reached

        unsigned fast;
        unsigned slow;
    };
protected:
    std::shared_ptr<vChartFormat> _format;
    std::shared_ptr<chart::vChart> _chart;
    BasicData _basic;
    double _minHealth;
    double _clearHealth;
    std::vector<unsigned> _judgeCount;
    bool _isCleared = false;
    bool _isFailed = false;
public:
    vRuleset() = delete;
    vRuleset(std::shared_ptr<vChartFormat> format, std::shared_ptr<chart::vChart> chart, size_t judgeCount) :
        _format(format), _chart(chart), _basic{ 0 }, _judgeCount(judgeCount, 0) {}
    virtual ~vRuleset() = default;
public:
    virtual void updatePress(InputMask& pg, Time t) = 0;
    virtual void updateHold(InputMask& hg, Time t) = 0;
    virtual void updateRelease(InputMask& rg, Time t) = 0;
    virtual void update(Time t) = 0;
public:
    constexpr BasicData getData() const { return _basic; }
    constexpr bool isFinished() const { return _basic.totaln == _chart->getNoteCount(); }
    double getClearHealth() const { return _clearHealth; }
    virtual bool isCleared() const { return _isCleared; }
    virtual bool isFailed() const { return _isFailed; }
    virtual unsigned getCurrentMaxScore() const = 0;
    virtual unsigned getMaxScore() const = 0;
    void fail() { _basic.health = 0.0; }
};
