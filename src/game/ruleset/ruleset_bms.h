#pragma once
#include "ruleset.h"

namespace lunaticvibes
{

class RulesetBMS : virtual public RulesetBase
{
public:
    enum JudgeIndex
    {
        JUDGE_PERFECT,
        JUDGE_GREAT,
        JUDGE_GOOD,
        JUDGE_BAD,
        JUDGE_POOR,
        JUDGE_KPOOR,
        JUDGE_MISS,
        JUDGE_BP,
        JUDGE_CB,
        JUDGE_EARLY,
        JUDGE_LATE,

        JUDGE_EARLY_POOR,
        JUDGE_EARLY_BAD,
        JUDGE_EARLY_GOOD,
        JUDGE_EARLY_GREAT,
        JUDGE_EARLY_PERFECT,
        JUDGE_EXACT_PERFECT,
        JUDGE_LATE_PERFECT,
        JUDGE_LATE_GREAT,
        JUDGE_LATE_GOOD,
        JUDGE_LATE_BAD,
        JUDGE_LATE_POOR,
        JUDGE_MINE_POOR,
    };

    enum class JudgeDifficulty {
        VERYHARD = 0,
        HARD,
        NORMAL,
        EASY,
        VERYEASY,

        WHAT = 6
    };

    enum class JudgeType {
        PERFECT = 0,    // Option::JUDGE_0
        GREAT,
        GOOD,
        BAD,
        KPOOR,
        MISS,
    };
    inline static const std::map<JudgeType, Option::e_judge_type> JudgeTypeOptMap =
    {
        { JudgeType::PERFECT, Option::JUDGE_0 },
        { JudgeType::GREAT, Option::JUDGE_1 },
        { JudgeType::GOOD, Option::JUDGE_2 },
        { JudgeType::BAD, Option::JUDGE_3 },
        { JudgeType::KPOOR, Option::JUDGE_4 },
        { JudgeType::MISS, Option::JUDGE_5 },
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

    using LampType = ScoreBMS::Lamp;

    // Judge Time definitions.
    // Values are one-way judge times in ms, representing
    // PERFECT, GREAT, GOOD, BAD, ПеPOOR respectively.
    struct JudgeTime {
        Time PERFECT;
        Time GREAT;
        Time GOOD;
        Time BAD;
        Time KPOOR;
    };
    inline static const JudgeTime judgeTime[] = {
        { 8, 27, 40, 200, 600},        // VERY HARD
        {15, 32, 60, 200, 600},        // HARD
        {18, 40, 96, 200, 650},        // NORMAL
        {21, 60, 116, 200, 800},       // EASY
        {},                            // VERY EASY??
        {5, 5, 10, 200, 600}          // XD
    };

    // Judge area definitions.
    // e.g. EARLY_PERFECT: Perfect early half part
    enum class JudgeArea {
        NOTHING = 0,
        EARLY_KPOOR,
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
        LATE_KPOOR,
        MINE_KPOOR,
    };
    inline static const std::map<JudgeArea, JudgeType> JudgeAreaTypeMap =
    {
        { JudgeArea::NOTHING, JudgeType::MISS },
        { JudgeArea::EARLY_KPOOR, JudgeType::KPOOR },
        { JudgeArea::EARLY_BAD, JudgeType::BAD },
        { JudgeArea::EARLY_GOOD, JudgeType::GOOD },
        { JudgeArea::EARLY_GREAT, JudgeType::GREAT },
        { JudgeArea::EARLY_PERFECT, JudgeType::PERFECT },
        { JudgeArea::EXACT_PERFECT, JudgeType::PERFECT },
        { JudgeArea::LATE_PERFECT, JudgeType::PERFECT },
        { JudgeArea::LATE_GREAT, JudgeType::GREAT },
        { JudgeArea::LATE_GOOD, JudgeType::GOOD },
        { JudgeArea::LATE_BAD, JudgeType::BAD },
        { JudgeArea::MISS, JudgeType::MISS },
        { JudgeArea::LATE_KPOOR, JudgeType::KPOOR },
        { JudgeArea::MINE_KPOOR, JudgeType::KPOOR },
    };
    inline static const std::map<JudgeArea, std::vector<JudgeIndex>> JudgeAreaIndexAccMap =
    {
        { JudgeArea::NOTHING, {} } ,
        { JudgeArea::EARLY_KPOOR, { JUDGE_KPOOR, JUDGE_POOR, JUDGE_BP, JUDGE_EARLY } } ,
        { JudgeArea::EARLY_BAD, { JUDGE_BAD, JUDGE_EARLY_BAD, JUDGE_BP, JUDGE_CB, JUDGE_EARLY } } ,
        { JudgeArea::EARLY_GOOD, { JUDGE_GOOD, JUDGE_EARLY_GOOD, JUDGE_EARLY } } ,
        { JudgeArea::EARLY_GREAT, { JUDGE_GREAT, JUDGE_EARLY_GREAT, JUDGE_EARLY } } ,
        { JudgeArea::EARLY_PERFECT, { JUDGE_PERFECT, JUDGE_EARLY_PERFECT } } ,
        { JudgeArea::EXACT_PERFECT, { JUDGE_PERFECT, JUDGE_EXACT_PERFECT } } ,
        { JudgeArea::LATE_PERFECT, { JUDGE_PERFECT, JUDGE_LATE_PERFECT} } ,
        { JudgeArea::LATE_GREAT, { JUDGE_GREAT, JUDGE_LATE_GREAT, JUDGE_LATE } } ,
        { JudgeArea::LATE_GOOD, { JUDGE_GOOD, JUDGE_LATE_GOOD, JUDGE_LATE} } ,
        { JudgeArea::LATE_BAD, { JUDGE_BAD, JUDGE_LATE_BAD, JUDGE_BP, JUDGE_CB, JUDGE_LATE } } ,
        { JudgeArea::MISS, { JUDGE_MISS, JUDGE_POOR, JUDGE_BP, JUDGE_CB, JUDGE_LATE } } ,
        { JudgeArea::LATE_KPOOR, { JUDGE_KPOOR, JUDGE_POOR, JUDGE_BP, JUDGE_LATE } },
        { JudgeArea::MINE_KPOOR, { JUDGE_KPOOR, JUDGE_POOR, JUDGE_BP } },
    };

    /// /////////////////////////////////////////////////////////////////////

    typedef std::map<chart::NoteLaneIndex, IndexTimer> NoteLaneTimerMap;

    enum class PlaySide {
        SINGLE,
        DOUBLE,
        BATTLE_1P,
        BATTLE_2P,
        AUTO,
        AUTO_2P,
        AUTO_DOUBLE,
        RIVAL,
        MYBEST,
        NETWORK,
    };

    struct JudgeRes { JudgeArea area = JudgeArea::NOTHING; Time time; };

protected:
    // members set on construction
    PlaySide _side = PlaySide::SINGLE;
    bool _k1P = false, _k2P = false;
    JudgeDifficulty _judgeDifficulty = JudgeDifficulty::NORMAL;
    GaugeType _gauge = GaugeType::GROOVE;

    std::map<JudgeType, double> _healthGain;

    bool doJudge = true;
    bool _judgeScratch = true;

    bool showJudge = true;
    const NoteLaneTimerMap* _bombTimerMap = nullptr;
    const NoteLaneTimerMap* _bombLNTimerMap = nullptr;

    int total = -1;
    unsigned noteCount = 0;

    std::string modifierText, modifierTextShort;
    Option::e_lamp_type saveLampMax;

protected:
    // members change in game
    std::array<JudgeArea, chart::NOTELANEINDEX_COUNT> _lnJudge{ JudgeArea::NOTHING };
    std::array<JudgeRes, 2> _lastNoteJudge;

    std::map<chart::NoteLane, ChartObjectBase::NoteIterator> _noteListIterators;

    std::array<AxisDir, 2>  playerScratchDirection = { 0, 0 };
    std::array<Time, 2>     playerScratchLastUpdate = { TIMER_NEVER, TIMER_NEVER };
    std::array<double, 2>   playerScratchAccumulator = { 0, 0 };

protected:
    // members essential for score calculation
    double moneyScore = 0.0;
    double maxMoneyScore = 200000.0;
    unsigned exScore = 0;


public:
    RulesetBMS(
        std::shared_ptr<ChartFormatBase> format,
        std::shared_ptr<ChartObjectBase> chart,
        PlayModifierGaugeType gauge,
        GameModeKeys keys,
        JudgeDifficulty difficulty = JudgeDifficulty::NORMAL,
        double health = 1.0,
        PlaySide side = PlaySide::SINGLE);

    void initGaugeParams(PlayModifierGaugeType gauge);

protected:
    JudgeRes _judge(const Note& note, Time time);
private:
    void _judgePress(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot);
    void _judgeHold(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot);
    void _judgeRelease(chart::NoteLaneCategory cat, chart::NoteLaneIndex idx, HitableNote& note, JudgeRes judge, const Time& t, int slot);
    void judgeNotePress(Input::Pad k, const Time& t, const Time& rt, int slot);
    void judgeNoteHold(Input::Pad k, const Time& t, const Time& rt, int slot);
    void judgeNoteRelease(Input::Pad k, const Time& t, const Time& rt, int slot);
    void _updateHp(double diff);
    void _updateHp(JudgeArea judge);
public:
    // Register to InputWrapper
    virtual void updatePress(InputMask& pg, const Time& t) override;
    // Register to InputWrapper
    virtual void updateHold(InputMask& hg, const Time& t) override;
    // Register to InputWrapper
    virtual void updateRelease(InputMask& rg, const Time& t) override;
    // Register to InputWrapper
    virtual void updateAxis(double s1, double s2, const Time& t) override;
    // Called by ScenePlay
    virtual void update(const Time& t);

public:
    //constexpr auto getJudge() const { return _count; }
    void updateJudge(const Time& t, chart::NoteLaneIndex ch, JudgeArea judge, int slot, bool force = false);

public:
    GaugeType getGaugeType() const { return _gauge; }
    LampType getClearType() const;

    double getScore() const;
    double getMaxMoneyScore() const;
    unsigned getExScore() const;
    unsigned getJudgeCount(JudgeType idx) const;
    unsigned getJudgeCountEx(JudgeIndex idx) const;
    std::string getModifierText() const;
    std::string getModifierTextShort() const;

    JudgeRes getLastJudge(int side) const;

    virtual bool isNoScore() const { return moneyScore == 0.0; }
    virtual bool isCleared() const { return !isFailed() && isFinished() && _basic.health >= getClearHealth(); }
    virtual bool isFailed() const { return _isFailed; }

    virtual unsigned getCurrentMaxScore() const { return notesReached * 2; }
    virtual unsigned getMaxScore() const { return getNoteCount() * 2; }

    virtual unsigned getNoteCount() const;
    virtual unsigned getMaxCombo() const;

    virtual void fail();

    virtual void updateGlobals();
};

}
