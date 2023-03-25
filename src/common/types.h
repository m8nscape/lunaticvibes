#pragma once
#include <algorithm>
#include <filesystem>
#include <utility>
#include <string>
#include <string_view>
#include <cassert>
#include "hash.h"

typedef std::filesystem::path                   Path;
typedef Path::string_type                       StringPath;
typedef std::basic_string_view<Path::value_type> StringPathView;
typedef std::string                             StringContent; // std::ifstream, std::getline
typedef std::string_view                        StringContentView; // std::ifstream, std::getline
using namespace std::string_literals;

const size_t INDEX_INVALID	= ~0;

[[nodiscard]] inline StringPath operator ""_p(const char* _Str, size_t _Len)
{
    return Path(std::string_view(_Str, _Len));
}

enum class SkinType {
    EXIT = 0,
    TITLE = 1,
    MUSIC_SELECT,
    DECIDE,
    THEME_SELECT,
    SOUNDSET,
    KEY_CONFIG,

    PLAY5,
    PLAY5_2,
    PLAY7,
    PLAY7_2,
    PLAY9,
    PLAY9_2,
    PLAY10,
    PLAY14,

    RESULT,
    COURSE_RESULT,

    RETRY_TRANS,
    COURSE_TRANS,
    EXIT_TRANS,

    PRE_SELECT,

    TMPL,
    TEST,

    MODE_COUNT
};

enum class ePlayMode
{
    SINGLE_PLAYER,  // means "Single Player Mode", so DP is also included
    LOCAL_BATTLE,   // separate chart objects are required
    GHOST_BATTLE,   // 
};

typedef unsigned GameModeKeys; // 5 7 9 10 14

enum class PlayModifierRandomType: uint8_t
{
    NONE = 0,
    MIRROR,
    RANDOM,
    SRAN,
    HRAN,			// Scatter
    ALLSCR,			// Converge
    RRAN,
    DB_SYNCHRONIZE,
    DB_SYMMETRY,
};

enum class PlayModifierGaugeType : uint8_t
{
    NORMAL = 0,
    HARD,
    DEATH,
    EASY,
    PATTACK,    // placeholder, not included ingame
    GATTACK,    // placeholder, not included ingame
    EXHARD,
    ASSISTEASY,

    GRADE_NORMAL = 10,
    GRADE_HARD,
    GRADE_DEATH,

};

inline const uint8_t PLAY_MOD_ASSIST_AUTO67     = 1 << 0; // 5keys, not implemented
inline const uint8_t PLAY_MOD_ASSIST_AUTOSCR    = 1 << 1; // 
inline const uint8_t PLAY_MOD_ASSIST_LEGACY     = 1 << 2; // LN head -> note, not implemented
inline const uint8_t PLAY_MOD_ASSIST_NOMINES    = 1 << 3; // from beatoraja, not implemented

enum class PlayModifierHispeedFixType : uint8_t
{
    NONE,
    MAXBPM,
    MINBPM,
    AVERAGE,
    CONSTANT,
    INITIAL,
    MAIN,
};

enum class PlayModifierLaneEffectType : uint8_t
{
    OFF,
    HIDDEN,
    SUDDEN,
    SUDHID,
    LIFT,
    LIFTSUD,
};

struct PlayModifiers
{
    PlayModifierRandomType randomLeft = PlayModifierRandomType::NONE;
    PlayModifierRandomType randomRight = PlayModifierRandomType::NONE;
    PlayModifierGaugeType gauge = PlayModifierGaugeType::NORMAL;
    uint8_t assist_mask = 0;
    PlayModifierHispeedFixType hispeedFix = PlayModifierHispeedFixType::NONE;
    PlayModifierLaneEffectType laneEffect = PlayModifierLaneEffectType::OFF;
    bool DPFlip = false;

    void clear()
    {
        randomLeft = PlayModifierRandomType::NONE;
        randomRight = PlayModifierRandomType::NONE;
        gauge = PlayModifierGaugeType::NORMAL;
        assist_mask = 0;
        hispeedFix = PlayModifierHispeedFixType::NONE;
        laneEffect = PlayModifierLaneEffectType::OFF;
        DPFlip = false;
    }
};

enum class GaugeDisplayType
{
    GROOVE,         // 80+20
    SURVIVAL,       // red
    EX_SURVIVAL,    // yellow
    ASSIST_EASY,    // 60+40
};

class ScoreBase
{
public:
    enum class Type
    {
        UNKNOWN,
        BMS,
    };

    int notes = 0;
    int score = 0;
    double rate = 0.0;
    int fast = 0;
    int slow = 0;
    long long maxcombo = 0;
    long long addtime = 0;
    long long playcount = 0;
    long long clearcount = 0;
    long long reserved[1]{ 0 };
    double reservedlf[2]{ 0.0 };

    std::string replayFileName;

public:
    ScoreBase() = default;
    virtual Type getType() const { return Type::UNKNOWN; }
};

class ScoreBMS : public ScoreBase
{
public:
    ScoreBMS() = default;

public:
    int exscore = 0;

    enum class Lamp
    {
        NOPLAY,
        FAILED,
        ASSIST,
        EASY,
        NORMAL,
        HARD,
        EXHARD,
        FULLCOMBO,
        PERFECT,
        MAX
    } lamp = Lamp::NOPLAY;

    int pgreat = 0;
    int great = 0;
    int good = 0;
    int bad = 0;
    int kpoor = 0;
    int miss = 0;
    int bp = 0;
    int combobreak = 0;

    // extended info
    unsigned rival_win = 3; // win / lose / draw / noplay
    double rival_rate = 0;
    Lamp rival_lamp = Lamp::NOPLAY;

    virtual Type getType() const override { return Type::BMS; }
};

class AxisDir
{
public:
    static constexpr int AXIS_UP = -1;
    static constexpr int AXIS_NONE = 0;
    static constexpr int AXIS_DOWN = 1;

private:
    int dir;

public:
    AxisDir() : dir(AXIS_NONE) {}
    AxisDir(double val, double minVal = 0)
    {
        if (val > minVal)
            dir = AXIS_DOWN;
        else if (val < -minVal)
            dir = AXIS_UP;
        else
            dir = AXIS_NONE;
    }
    operator int() const { return dir; }
};

class Ratio
{
protected:
    double _data;
public:
    Ratio() : Ratio(0) {}
    Ratio(double d) { _data = std::clamp(d, 0.0, 1.0); }
    operator double() const { return _data; }
};
