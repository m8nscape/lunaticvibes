#pragma once
#include <filesystem>
#include <utility>
#include <string>
#include <string_view>

typedef std::string                             HashMD5;
typedef std::string                             HashSHA1;
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

enum class eMode {
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

    BGM,
    SOUND,

    RETRY_TRANS,
    COURSE_TRANS,

    TMPL,
    TEST,

    MODE_COUNT
};

enum class ePlayMode
{
    SINGLE,         // means "Single Player Mode", so DP is also included
    LOCAL_BATTLE,   // separate chart objects are required
    AUTO,           // do not save score, replay and result
    AUTO_BATTLE,    // AUTO + LOCAL_BATTLE
    REPLAY,         // do not save score
};

typedef unsigned GameModeKeys; // 5 7 9 10 14

enum class eModChart: uint8_t
{
    NONE = 0,
    MIRROR,
    RANDOM,
    SRAN,
    HRAN,			// Scatter
    ALLSCR,			// Converge
    // FIXME DP separated mod
};

enum class eModGauge : uint8_t
{
    NORMAL = 0,
    HARD,
    DEATH,
    EASY,
    PATTACK,
    GATTACK,
    ASSISTEASY,

    GRADE_NORMAL,
    GRADE_DEATH, // ?

    EXHARD,
    GRADE_EX,
};

inline const uint8_t PLAY_MOD_ASSIST_AUTO67     = 1 << 0; // 5keys
inline const uint8_t PLAY_MOD_ASSIST_AUTOSCR    = 1 << 1; // 
inline const uint8_t PLAY_MOD_ASSIST_LEGACY     = 1 << 2; // LN -> note
inline const uint8_t PLAY_MOD_ASSIST_NOMINES    = 1 << 3; // 5keys

enum class eModHs : uint8_t
{
    NONE,
    MAXBPM,
    MINBPM,
    AVERAGE,
    CONSTANT,
};

inline const uint8_t PLAY_MOD_VISUAL_SUD        = 1 << 0;
inline const uint8_t PLAY_MOD_VISUAL_HID        = 1 << 1;
inline const uint8_t PLAY_MOD_VISUAL_LIFT       = 1 << 2;

struct PlayMod
{
    eModChart chart = eModChart::NONE;
    eModGauge gauge = eModGauge::NORMAL;
    uint8_t assist_mask = 0;
    eModHs hs = eModHs::NONE;
    uint8_t visual_mask = 0;

    void clear()
    {
        chart = eModChart::NONE;
        gauge = eModGauge::NORMAL;
        assist_mask = 0;
        hs = eModHs::NONE;
        visual_mask = 0;
    }
};

enum class eGaugeOp
{
    GROOVE,
    SURVIVAL,
    EX_SURVIVAL,
};

enum class eLevel
{
    UNDEF,
    BEGINNER,
    NORMAL,
    HYPER,
    ANOTHER,
    INSANE,
    LEVEL_COUNT
};

class vScore
{
public:
    enum class Type
    {
        UNKNOWN,
        BMS,
    };
    Type type = Type::UNKNOWN;

    int notes = 0;
    int score = 0;
    double rate = 0.0;
    int fast = 0;
    int slow = 0;
    long long maxcombo = 0;
    long long addtime = 0;
    long long playcount = 0;
    long long reserved[1]{ 0 };
    double reservedlf[2]{ 0.0 };

public:
    vScore(Type t) : type(t) {}
};

class ScoreBMS : public vScore
{
public:
    ScoreBMS() : vScore(Type::BMS) {}

public:
    int exscore;

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
    };
    Lamp lamp;

    int pgreat;
    int great;
    int good;
    int bad;
    int bpoor;
    int miss;
    int bp;
    int combobreak;

    // extended info
    unsigned rival = 3; // win / lose / draw / noplay
    double rival_rate = 0;
    Lamp rival_lamp;
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