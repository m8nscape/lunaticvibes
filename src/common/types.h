#pragma once
#include <filesystem>
#include <utility>
#include <string>
#include <string_view>
#include <cassert>

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

template <size_t _Len>
class Hash
{
private:
    unsigned char data[_Len] = { 0 };
    bool set = false;
public:
    Hash() = default;
    Hash(const std::string& hex)
    {
        reset();
        if (!hex.empty())
        {
            set = true;
            std::string bin = hex2bin(hex);
            unsigned char* ubin = (unsigned char*)bin.data();
            for (int i = 0; i < bin.size() && i < _Len; ++i) 
                data[i] = ubin[i];
        }
    }
    Hash(const Hash<_Len>& rhs)
    {
        reset();
        if (!rhs.empty())
        {
            set = true;
            for (int i = 0; i < _Len; ++i) data[i] = rhs.data[i];
        }
    }

    constexpr size_t length() const { return _Len; }
    bool empty() const { return !set; }
    std::string hexdigest() const { return bin2hex(data, _Len); }
    const unsigned char* hex() const { return data; }
    void reset() { set = false; memset(data, 0, _Len); }

    template <size_t _Len2>
    bool operator<(const Hash<_Len2>& rhs) const { return memcmp(data, rhs.data, _Len) < 0; }
    template <size_t _Len2>
    bool operator>(const Hash<_Len2>& rhs) const { return memcmp(data, rhs.data, _Len) > 0; }
    template <size_t _Len2>
    bool operator<=(const Hash<_Len2>& rhs) const { return !(*this > rhs); }
    template <size_t _Len2>
    bool operator>=(const Hash<_Len2>& rhs) const { return !(*this > rhs); }
    template <size_t _Len2>
    bool operator==(const Hash<_Len2>& rhs) const { return _Len == _Len2 && memcmp(data, rhs.data, _Len) == 0; }
    template <size_t _Len2>
    bool operator!=(const Hash<_Len2>& rhs) const { return _Len != _Len2 || memcmp(data, rhs.data, _Len) != 0; }

    friend struct std::hash<Hash<_Len>>;
};

template<size_t _Len> 
struct std::hash<Hash<_Len>>
{
    size_t operator()(const Hash<_Len>& obj) const
    {
        size_t h = 0;
        int i = 0;
        if (_Len >= sizeof(size_t))
        {
            for (; i <= _Len - sizeof(size_t); i += sizeof(size_t))
            {
                h ^= *(size_t*)&obj.data[i];
            }
        }
        unsigned char* p = (unsigned char*)&h;
        for (; i < _Len; i++)
        {
            p[i % sizeof(size_t)] ^= obj.data[i];
        }
        return h;
    }
};

typedef Hash<16> HashMD5;
typedef Hash<32> HashSHA1;

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

enum class eModRandom: uint8_t
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

enum class eModGauge : uint8_t
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

enum class eModHs : uint8_t
{
    NONE,
    MAXBPM,
    MINBPM,
    AVERAGE,
    CONSTANT,
};

enum class eModLaneEffect : uint8_t
{
    OFF,
    HIDDEN,
    SUDDEN,
    SUDHID,
    LIFT,
    LIFTSUD,
};

struct PlayMod
{
    eModRandom randomLeft = eModRandom::NONE;
    eModRandom randomRight = eModRandom::NONE;
    eModGauge gauge = eModGauge::NORMAL;
    uint8_t assist_mask = 0;
    eModHs hispeedFix = eModHs::NONE;
    eModLaneEffect laneEffect = eModLaneEffect::OFF;
    bool DPFlip = false;

    void clear()
    {
        randomLeft = eModRandom::NONE;
        randomRight = eModRandom::NONE;
        gauge = eModGauge::NORMAL;
        assist_mask = 0;
        hispeedFix = eModHs::NONE;
        laneEffect = eModLaneEffect::OFF;
        DPFlip = false;
    }
};

enum class eGaugeOp
{
    GROOVE,
    SURVIVAL,
    EX_SURVIVAL,
    ASSIST_EASY,
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
    vScore() = default;
    virtual Type getType() const { return Type::UNKNOWN; }
};

class ScoreBMS : public vScore
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
    unsigned rival = 3; // win / lose / draw / noplay
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
