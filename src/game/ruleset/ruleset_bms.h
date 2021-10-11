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

    typedef std::map<NoteLaneIndex, eTimer> NoteLaneTimerMap;

    inline static const NoteLaneTimerMap bombTimer7k = {
        {NoteLaneIndex::Sc1, eTimer::S1_BOMB},
        {NoteLaneIndex::K1,  eTimer::K11_BOMB},
        {NoteLaneIndex::K2,  eTimer::K12_BOMB},
        {NoteLaneIndex::K3,  eTimer::K13_BOMB},
        {NoteLaneIndex::K4,  eTimer::K14_BOMB},
        {NoteLaneIndex::K5,  eTimer::K15_BOMB},
        {NoteLaneIndex::K6,  eTimer::K16_BOMB},
        {NoteLaneIndex::K7,  eTimer::K17_BOMB},
    };

    inline static const NoteLaneTimerMap bombTimer7kLN = { {
        {NoteLaneIndex::Sc1, eTimer::S1_LN_BOMB },
        {NoteLaneIndex::K1,  eTimer::K11_LN_BOMB},
        {NoteLaneIndex::K2,  eTimer::K12_LN_BOMB},
        {NoteLaneIndex::K3,  eTimer::K13_LN_BOMB},
        {NoteLaneIndex::K4,  eTimer::K14_LN_BOMB},
        {NoteLaneIndex::K5,  eTimer::K15_LN_BOMB},
        {NoteLaneIndex::K6,  eTimer::K16_LN_BOMB},
        {NoteLaneIndex::K7,  eTimer::K17_LN_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer14k = { {
        {NoteLaneIndex::Sc1, eTimer::S1_BOMB},
        {NoteLaneIndex::K1,  eTimer::K11_BOMB},
        {NoteLaneIndex::K2,  eTimer::K12_BOMB},
        {NoteLaneIndex::K3,  eTimer::K13_BOMB},
        {NoteLaneIndex::K4,  eTimer::K14_BOMB},
        {NoteLaneIndex::K5,  eTimer::K15_BOMB},
        {NoteLaneIndex::K6,  eTimer::K16_BOMB},
        {NoteLaneIndex::K7,  eTimer::K17_BOMB},
        {NoteLaneIndex::K8,  eTimer::K21_BOMB},
        {NoteLaneIndex::K9,  eTimer::K22_BOMB},
        {NoteLaneIndex::K10, eTimer::K23_BOMB},
        {NoteLaneIndex::K11, eTimer::K24_BOMB},
        {NoteLaneIndex::K12, eTimer::K25_BOMB},
        {NoteLaneIndex::K13, eTimer::K26_BOMB},
        {NoteLaneIndex::K14, eTimer::K27_BOMB},
        {NoteLaneIndex::Sc2, eTimer::S2_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer14kLN = { {
        {NoteLaneIndex::Sc1, eTimer::S1_LN_BOMB},
        {NoteLaneIndex::K1,  eTimer::K11_LN_BOMB},
        {NoteLaneIndex::K2,  eTimer::K12_LN_BOMB},
        {NoteLaneIndex::K3,  eTimer::K13_LN_BOMB},
        {NoteLaneIndex::K4,  eTimer::K14_LN_BOMB},
        {NoteLaneIndex::K5,  eTimer::K15_LN_BOMB},
        {NoteLaneIndex::K6,  eTimer::K16_LN_BOMB},
        {NoteLaneIndex::K7,  eTimer::K17_LN_BOMB},
        {NoteLaneIndex::K8,  eTimer::K21_LN_BOMB},
        {NoteLaneIndex::K9,  eTimer::K22_LN_BOMB},
        {NoteLaneIndex::K10, eTimer::K23_LN_BOMB},
        {NoteLaneIndex::K11, eTimer::K24_LN_BOMB},
        {NoteLaneIndex::K12, eTimer::K25_LN_BOMB},
        {NoteLaneIndex::K13, eTimer::K26_LN_BOMB},
        {NoteLaneIndex::K14, eTimer::K27_LN_BOMB},
        {NoteLaneIndex::Sc2, eTimer::S2_LN_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer9k = { {
        {NoteLaneIndex::K1,  eTimer::K11_BOMB},
        {NoteLaneIndex::K2,  eTimer::K12_BOMB},
        {NoteLaneIndex::K3,  eTimer::K13_BOMB},
        {NoteLaneIndex::K4,  eTimer::K14_BOMB},
        {NoteLaneIndex::K5,  eTimer::K15_BOMB},
        {NoteLaneIndex::K6,  eTimer::K16_BOMB},
        {NoteLaneIndex::K7,  eTimer::K17_BOMB},
        {NoteLaneIndex::K8,  eTimer::K18_BOMB},
        {NoteLaneIndex::K9,  eTimer::K19_BOMB},
    } };

    inline static const NoteLaneTimerMap bombTimer9kLN = { {
        {NoteLaneIndex::K1,  eTimer::K11_LN_BOMB},
        {NoteLaneIndex::K2,  eTimer::K12_LN_BOMB},
        {NoteLaneIndex::K3,  eTimer::K13_LN_BOMB},
        {NoteLaneIndex::K4,  eTimer::K14_LN_BOMB},
        {NoteLaneIndex::K5,  eTimer::K15_LN_BOMB},
        {NoteLaneIndex::K6,  eTimer::K16_LN_BOMB},
        {NoteLaneIndex::K7,  eTimer::K17_LN_BOMB},
        {NoteLaneIndex::K8,  eTimer::K18_LN_BOMB},
        {NoteLaneIndex::K9,  eTimer::K19_LN_BOMB},
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
        SP,
        DP,
        BATTLE_1P,
        BATTLE_2P,
    };

    struct judgeRes { judgeArea area; Time time; };

protected:
    JudgeDifficulty _diff;
    GaugeType _gauge;
    std::map<JudgeType, double> _health;
    std::map<JudgeType, unsigned> _count;
    const NoteLaneTimerMap* _bombTimerMap = nullptr;
    const NoteLaneTimerMap* _bombLNTimerMap = nullptr;
	bool _k1P, _k2P;
    double inner_score = 0.0;
    std::array<JudgeType, NOTELANEINDEX_COUNT> _lnJudge{JudgeType::MISS};

    std::map<NoteLane, decltype(_chart->firstNote(NoteLaneCategory::_, NoteLaneIndex::_))> _noteListIterators;

public:
    RulesetBMS(
        std::shared_ptr<vChartFormat> format,
        std::shared_ptr<vChart> chart,
        eModGauge gauge,
        unsigned keys,
        JudgeDifficulty difficulty = JudgeDifficulty::NORMAL,
        double health = 1.0,
        PlaySide side = PlaySide::SP);
private:
    judgeRes _judge(const Note& note, Time time);
    void _updateHp(const double delta);
    void _updateHp(JudgeType judge);
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
    //constexpr auto getJudge() const { return _count; }
    void updateHit(Time& t, NoteLaneIndex ch, JudgeType judge, int slot);
    void updateMiss(Time& t, NoteLaneIndex ch, JudgeType judge, int slot);
    virtual bool isCleared() const { return !_isFailed && isFinished() && _basic.health >= _clearHealth; }
    virtual bool isFailed() const { return _isFailed; }
    virtual unsigned getCurrentMaxScore() const { return _basic.totalnr * 2; }
    virtual unsigned getMaxScore() const { return _chart->getNoteRegularCount() * 2 + _chart->getNoteLnCount() * 2; }
};