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
        double acc;         // 0.0 - 100.0
        double total_acc;   // 0.0 - 100.0
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
    bool _failWhenNoHealth = false;

    bool _isCleared = false;
    bool _isFailed = false;
    bool _isAutoplay = false;
public:
    vRuleset() = delete;
    vRuleset(std::shared_ptr<vChartFormat> format, std::shared_ptr<chart::vChart> chart) :
        _format(format), _chart(chart), _basic{ 0 }{}
    virtual ~vRuleset() = default;
public:
    virtual void updatePress(InputMask& pg, const Time& t) = 0;
    virtual void updateHold(InputMask& hg, const Time& t) = 0;
    virtual void updateRelease(InputMask& rg, const Time& t) = 0;
    virtual void updateAxis(InputAxisPlus& m, const Time& t) = 0;
    virtual void update(const Time& t) = 0;
public:
    constexpr BasicData getData() const { return _basic; }
    double getClearHealth() const { return _clearHealth; }
    bool failWhenNoHealth() const { return _failWhenNoHealth; }
    virtual bool isFinished() const { return _basic.totaln == _chart->getNoteCount(); }
    virtual bool isCleared() const { return _isCleared; }
    virtual bool isFailed() const { return _isFailed; }

    virtual unsigned getCurrentMaxScore() const = 0;
    virtual unsigned getMaxScore() const = 0;
    virtual unsigned getMaxCombo() const = 0;
    virtual void fail() { _isFailed = true; }
    virtual void reset() { _basic = { 0 }; };
    virtual void updateGlobals() = 0;
};
