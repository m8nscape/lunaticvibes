#pragma once
#include "ruleset.h"

namespace rc {

struct judge_t {
    Time PERFECT;
    Time GREAT;
    Time GOOD;
    Time BAD;
    Time BPOOR;
};

enum class judgeDiff
{
    VERYHARD = 0,
    HARD,
    NORMAL,
    EASY,
    VERYEASY,

    WHAT = 6
};

enum judge_idx {
    PERFECT = 0,
    GREAT,
    GOOD,
    BAD,
    BPOOR,
    MISS,

    JUDGE_COUNT
};

enum gauge_ty {
    GROOVE,
    EASY,
    ASSIST,
    HARD,
    EXHARD,
    DEATH,
    P_ATK,
    G_ATK,
    GRADE,
    EXGRADE,
};

// Judge Time definitions.
// Values are one-way judge times in ms, representing
// PERFECT, GREAT, GOOD, BAD, ПеPOOR respectively.
const judge_t judgeTime[] = {
    { 8, 27, 40, 200, 600},        // VERY HARD
    {15, 32, 60, 200, 600},        // HARD
    {18, 40, 96, 200, 650},        // NORMAL
    {21, 60, 116, 200, 800},       // EASY
    {},                            // VERY EASY??
    {5, 5, 10, 200, 600}          // XD
};

const eTimer bombTimer7k[] = {
	eTimer::S1_BOMB,

    eTimer::K11_BOMB,
    eTimer::K12_BOMB,
    eTimer::K13_BOMB,
    eTimer::K14_BOMB,
    eTimer::K15_BOMB,
    eTimer::K16_BOMB,
    eTimer::K17_BOMB,

	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,

	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,

	eTimer::_NEVER,

    eTimer::S1_LN_BOMB,

    eTimer::K11_LN_BOMB,
    eTimer::K12_LN_BOMB,
    eTimer::K13_LN_BOMB,
    eTimer::K14_LN_BOMB,
    eTimer::K15_LN_BOMB,
    eTimer::K16_LN_BOMB,
    eTimer::K17_LN_BOMB,

    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,

    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,

    eTimer::_NEVER
};

const eTimer bombTimer14k[] = {
	eTimer::S1_BOMB,

    eTimer::K11_BOMB,
    eTimer::K12_BOMB,
    eTimer::K13_BOMB,
    eTimer::K14_BOMB,
    eTimer::K15_BOMB,
    eTimer::K16_BOMB,
    eTimer::K17_BOMB,

    eTimer::K21_BOMB,
    eTimer::K22_BOMB,
    eTimer::K23_BOMB,
    eTimer::K24_BOMB,
    eTimer::K25_BOMB,
    eTimer::K26_BOMB,
    eTimer::K27_BOMB,

	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,

	eTimer::S2_BOMB,

    eTimer::S1_LN_BOMB,

    eTimer::K11_LN_BOMB,
    eTimer::K12_LN_BOMB,
    eTimer::K13_LN_BOMB,
    eTimer::K14_LN_BOMB,
    eTimer::K15_LN_BOMB,
    eTimer::K16_LN_BOMB,
    eTimer::K17_LN_BOMB,

    eTimer::K21_LN_BOMB,
    eTimer::K22_LN_BOMB,
    eTimer::K23_LN_BOMB,
    eTimer::K24_LN_BOMB,
    eTimer::K25_LN_BOMB,
    eTimer::K26_LN_BOMB,
    eTimer::K27_LN_BOMB,

    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,

    eTimer::S2_LN_BOMB
};

const eTimer bombTimer9k[] = {
	eTimer::_NEVER,

    eTimer::K11_BOMB,
    eTimer::K12_BOMB,
    eTimer::K13_BOMB,
    eTimer::K14_BOMB,
    eTimer::K15_BOMB,
    eTimer::K16_BOMB,
    eTimer::K17_BOMB,

    eTimer::K18_BOMB,
    eTimer::K19_BOMB,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,

	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,
	eTimer::_NEVER,

	eTimer::_NEVER,

    eTimer::_NEVER,

    eTimer::K11_LN_BOMB,
    eTimer::K12_LN_BOMB,
    eTimer::K13_LN_BOMB,
    eTimer::K14_LN_BOMB,
    eTimer::K15_LN_BOMB,
    eTimer::K16_LN_BOMB,
    eTimer::K17_LN_BOMB,

    eTimer::K18_LN_BOMB,
    eTimer::K19_LN_BOMB,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,

    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,
    eTimer::_NEVER,

    eTimer::_NEVER
};

// Judge area definitions.
// e.g. SOUNDONLY: play hitsound only but not judging
//      EARLY_PERFECT: Perfect early half part
namespace judgeArea {
    enum area {
        NOTHING = 0,
        EARLY_BPOOR,
        EARLY_BAD,
        EARLY_GOOD,
        EARLY_GREAT,
        EARLY_PERFECT,
        EXACT_PERFECT,
        LATE_PERFECT,
        LATE_GREAT,
        LATE_GOOD,
        LATE_BAD,
        MISS,

    };
}

enum class player {
	SP,
	DP,
	BATTLE_1P,
	BATTLE_2P,
};

struct judgeRes { judgeArea::area area; Time time; };
}

class RulesetClassic : public vRuleset
{
protected:
    rc::judgeDiff _diff;
    double _health[rc::judge_idx::JUDGE_COUNT];
    rc::gauge_ty _gauge;
    std::array<unsigned, rc::judge_idx::JUDGE_COUNT> _count;
	rc::player _player;
	bool _k1P, _k2P;
    double inner_score = 0.0;
    std::array<rc::judge_idx, NOTECHANNEL_COUNT> _lnJudge{rc::judge_idx::MISS};

    std::map<std::pair<NoteLaneCategory, NoteLaneIndex>, decltype(_chart->firstNoteOfLane(NoteLaneCategory::_, NoteLaneIndex::_))> _noteListIterators;

public:
    RulesetClassic(
        std::shared_ptr<vChartFormat> format, 
        std::shared_ptr<vChart> chart,
        rc::judgeDiff difficulty = rc::judgeDiff::NORMAL,
        rc::gauge_ty gauge = rc::gauge_ty::GROOVE, 
        double health = 1.0,
        rc::player playerSP = rc::player::SP);
private:
    rc::judgeRes _judge(const Note& note, Time time);
    void _updateHp(const double delta);
public:
    // Register to InputWrapper
    virtual void updatePress(InputMask& pg, Time t);
    // Register to InputWrapper
    virtual void updateHold(InputMask& hg, Time t);
    // Register to InputWrapper
    virtual void updateRelease(InputMask& rg, Time t);
    // Called by ScenePlay
    virtual void update(Time t);
public:
    constexpr auto getJudge() const { return _count; }
    void updateHit(Time& t, NoteLaneIndex ch, size_t judge, unsigned slot);
    void updateMiss(Time& t, NoteLaneIndex ch, size_t judge, unsigned slot);
    virtual unsigned getCurrentMaxScore() const { return _basic.totalnr * 2; }
    virtual unsigned getMaxScore() const { return _chart->getNoteRegularCount() * 2 + _chart->getNoteLnCount() * 2; }
};