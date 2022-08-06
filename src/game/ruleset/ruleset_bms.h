#pragma once
#include "ruleset.h"

class RulesetBMS : public vRuleset
{
public:
    enum class JudgeDifficulty {
        VERYHARD = 0,
        HARD,
        NORMAL,
        EASY,
        VERYEASY,

        WHAT = 6
    };

    enum class JudgeType {
        PERFECT = 0,
        GREAT,
        GOOD,
        BAD,
        BPOOR,
        MISS,
        COMBOBREAK,
        OPTION1,
        OPTION2,
    };

    enum class GaugeType {
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
    struct JudgeTime {
        Time PERFECT;
        Time GREAT;
        Time GOOD;
        Time BAD;
        Time BPOOR;
    };
    inline static const JudgeTime judgeTime[] = {
        { 8, 27, 40, 200, 600},        // VERY HARD
        {15, 32, 60, 200, 600},        // HARD
        {18, 40, 96, 200, 650},        // NORMAL
        {21, 60, 116, 200, 800},       // EASY
        {},                            // VERY EASY??
        {5, 5, 10, 200, 600}          // XD
    };

    typedef std::map<chart::NoteLaneIndex, eTimer> NoteLaneTimerMap;

    inline static const NoteLaneTimerMap bombTimer7k = {
        {chart::Sc1, eTimer::S1_BOMB},
        {chart::K1,  eTimer::K11_BOMB},
        {chart::K2,  eTimer::K12_BOMB},
        {chart::K3,  eTimer::K13_BOMB},
        {chart::K4,  eTimer::K14_BOMB},
        {chart::K5,  eTimer::K15_BOMB},
        {chart::K6,  eTimer::K16_BOMB},
        {chart::K7,  eTimer::K17_BOMB},
    };

    inline static const NoteLaneTimerMap bombTimer7kLN = { {
        {chart::Sc1, eTimer::S1_LN_BOMB },
        {chart::K1,  eTimer::K11_LN_BOMB},
        {chart::K2,  eTimer::K12_LN_BOMB},
        {chart::K3,  eTimer::K13_LN_BOMB},
        {chart::K4,  eTimer::K14_LN_BOMB},
        {chart::K5,  eTimer::K15_LN_BOMB},
        {chart::K6,  eTimer::K16_LN_BOMB},
        {chart::K7,  eTimer::K17_LN_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer14k = { {
        {chart::Sc1, eTimer::S1_BOMB},
        {chart::K1,  eTimer::K11_BOMB},
        {chart::K2,  eTimer::K12_BOMB},
        {chart::K3,  eTimer::K13_BOMB},
        {chart::K4,  eTimer::K14_BOMB},
        {chart::K5,  eTimer::K15_BOMB},
        {chart::K6,  eTimer::K16_BOMB},
        {chart::K7,  eTimer::K17_BOMB},
        {chart::K8,  eTimer::K21_BOMB},
        {chart::K9,  eTimer::K22_BOMB},
        {chart::K10, eTimer::K23_BOMB},
        {chart::K11, eTimer::K24_BOMB},
        {chart::K12, eTimer::K25_BOMB},
        {chart::K13, eTimer::K26_BOMB},
        {chart::K14, eTimer::K27_BOMB},
        {chart::Sc2, eTimer::S2_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer14kLN = { {
        {chart::Sc1, eTimer::S1_LN_BOMB},
        {chart::K1,  eTimer::K11_LN_BOMB},
        {chart::K2,  eTimer::K12_LN_BOMB},
        {chart::K3,  eTimer::K13_LN_BOMB},
        {chart::K4,  eTimer::K14_LN_BOMB},
        {chart::K5,  eTimer::K15_LN_BOMB},
        {chart::K6,  eTimer::K16_LN_BOMB},
        {chart::K7,  eTimer::K17_LN_BOMB},
        {chart::K8,  eTimer::K21_LN_BOMB},
        {chart::K9,  eTimer::K22_LN_BOMB},
        {chart::K10, eTimer::K23_LN_BOMB},
        {chart::K11, eTimer::K24_LN_BOMB},
        {chart::K12, eTimer::K25_LN_BOMB},
        {chart::K13, eTimer::K26_LN_BOMB},
        {chart::K14, eTimer::K27_LN_BOMB},
        {chart::Sc2, eTimer::S2_LN_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer9k = { {
        {chart::K1,  eTimer::K11_BOMB},
        {chart::K2,  eTimer::K12_BOMB},
        {chart::K3,  eTimer::K13_BOMB},
        {chart::K4,  eTimer::K14_BOMB},
        {chart::K5,  eTimer::K15_BOMB},
        {chart::K6,  eTimer::K16_BOMB},
        {chart::K7,  eTimer::K17_BOMB},
        {chart::K8,  eTimer::K18_BOMB},
        {chart::K9,  eTimer::K19_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer9kLN = { {
        {chart::K1,  eTimer::K11_LN_BOMB},
        {chart::K2,  eTimer::K12_LN_BOMB},
        {chart::K3,  eTimer::K13_LN_BOMB},
        {chart::K4,  eTimer::K14_LN_BOMB},
        {chart::K5,  eTimer::K15_LN_BOMB},
        {chart::K6,  eTimer::K16_LN_BOMB},
        {chart::K7,  eTimer::K17_LN_BOMB},
        {chart::K8,  eTimer::K18_LN_BOMB},
        {chart::K9,  eTimer::K19_LN_BOMB},
    } };

    // Judge area definitions.
    // e.g. SOUNDONLY: play hitsound only but not judging
    //      EARLY_PERFECT: Perfect early half part
    enum class judgeArea {
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

    enum class PlaySide {
        SINGLE,
        DOUBLE,
        BATTLE_1P,
        BATTLE_2P,
        AUTO,
        AUTO_2P,
        RIVAL,
    };

    struct judgeRes { judgeArea area = judgeArea::NOTHING; Time time; };

protected:
    JudgeDifficulty _diff;
    GaugeType _gauge;
    std::map<JudgeType, double> _healthIncrement;

    std::map<JudgeType, unsigned> _count;
    const NoteLaneTimerMap* _bombTimerMap = nullptr;
    const NoteLaneTimerMap* _bombLNTimerMap = nullptr;
    PlaySide _side;
	bool _k1P, _k2P;
    double inner_score = 0.0;
    std::array<judgeArea, chart::NOTELANEINDEX_COUNT> _lnJudge{ judgeArea::NOTHING };

    std::map<chart::NoteLane, decltype(_chart->firstNote(chart::NoteLaneCategory::_, chart::NoteLaneIndex::_))> _noteListIterators;

    AxisDir _scratchDir[2] = { 0, 0 };
    bool _scratchKey[2][2] = { {false, false}, {false, false} };
    Time _ttAxisLastUpdate[2] = { TIMER_NEVER, TIMER_NEVER };

public:
    RulesetBMS(
        std::shared_ptr<vChartFormat> format,
        std::shared_ptr<chart::vChart> chart,
        eModGauge gauge,
        GameModeKeys keys,
        JudgeDifficulty difficulty = JudgeDifficulty::NORMAL,
        double health = 1.0,
        PlaySide side = PlaySide::SINGLE);
protected:
    judgeRes _judge(const Note& note, Time time);
private:
    void _judgePress(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, judgeRes judge, const Time& t, int slot);
    void _judgeHold(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, judgeRes judge, const Time& t, int slot);
    void _judgeRelease(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, judgeRes judge, const Time& t, int slot);
    void judgeNotePress(Input::Pad k, const Time& t, const Time& rt, int slot);
    void judgeNoteHold(Input::Pad k, const Time& t, const Time& rt, int slot);
    void judgeNoteRelease(Input::Pad k, const Time& t, const Time& rt, int slot);
    void _updateHp(const double diff);
    void _updateHp(JudgeType judge);
public:
    // Register to InputWrapper
    virtual void updatePress(InputMask& pg, const Time& t) override;
    // Register to InputWrapper
    virtual void updateHold(InputMask& hg, const Time& t) override;
    // Register to InputWrapper
    virtual void updateRelease(InputMask& rg, const Time& t) override;
    // Register to InputWrapper
    virtual void updateAxis(InputAxisPlus& ag, const Time& t) override;
    // Called by ScenePlay
    virtual void update(const Time& t);
public:
    //constexpr auto getJudge() const { return _count; }
    void updateHit(const Time& t, chart::NoteLaneIndex ch, JudgeType judge, int slot);
    void updateMiss(const Time& t, chart::NoteLaneIndex ch, JudgeType judge, int slot);
    virtual bool isCleared() const { return !_isFailed && isFinished() && _basic.health >= _clearHealth; }
    virtual bool isFailed() const { return _isFailed; }
    virtual unsigned getCurrentMaxScore() const { return _basic.totalnr * 2; }
    virtual unsigned getMaxScore() const { return _chart->getNoteRegularCount() * 2 + _chart->getNoteLnCount() * 2; }
    unsigned getJudgeCount(JudgeType idx) const { return _count.find(idx) != _count.end() ? _count.at(idx) : 0; }
    GaugeType getGaugeType() const { return _gauge; }
    virtual unsigned getMaxCombo() const { return _chart->getNoteCount(); }
    virtual void fail();
    virtual void reset();
    virtual void updateGlobals();
};