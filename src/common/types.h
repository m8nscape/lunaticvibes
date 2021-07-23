#pragma once
#include <filesystem>
#include <utility>
#include <string>

typedef std::string                             HashMD5;
typedef std::string                             HashSHA1;
typedef std::filesystem::path                   Path;
typedef Path::string_type                       StringPath;
typedef std::string                             StringContent; // std::ifstream, std::getline
using namespace std::string_literals;

const size_t INDEX_INVALID	= ~0;

[[nodiscard]] inline StringPath operator ""_p(const char *_Str, size_t _Len)
{
    std::string tmp(_Str, _Len);
    return StringPath(tmp.begin(), tmp.end());
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

    TMPL,
    TEST,

    MODE_COUNT
};

enum class eModChart: uint8_t
{
    NONE = 0,
    MIRROR,
    RANDOM,
    SRAN,
    HRAN,			// Scatter
    ALLSCR,			// Converge
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
    EXHARD,
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
        CLASSIC,
    };
    Type type = Type::UNKNOWN;

    int notes = 0;
    int score = 0;
    double rate = 0.0;
    int fast = 0;
    int slow = 0;
    int reserved[4]{ 0 };
    double reservedlf[2]{ 0.0 };

public:
    vScore(Type t) : type(t) {}
};

class ScoreClassic : public vScore
{
public:
    ScoreClassic() : vScore(Type::CLASSIC) {}

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
};
