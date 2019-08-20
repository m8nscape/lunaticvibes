#pragma once
#include <filesystem>
#include <utility>
#include <string>

typedef std::string                             HashMD5;
typedef std::string                             HashSHA1;
typedef std::filesystem::path                   Path;
typedef decltype(std::declval<Path>().string()) StringPath;
typedef std::string                             StringContent; // std::ifstream, std::getline

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

    TMPL,
    TEST,

    MODE_COUNT
};

enum class eModChart: uint8_t
{
    NONE = 0,
    RANDOM,
    MIRROR,
    SRAN,
    HRAN,			// Scatter
    ALLSCR,			// Converge
};

enum class eModGauge : uint8_t
{
    NORMAL = 0,
    HARD,
    EASY,
    DEATH,
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