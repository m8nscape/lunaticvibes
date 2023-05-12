#pragma once

class SongDB;
class ScoreDB;
extern std::shared_ptr<SongDB> g_pSongDB;
extern std::shared_ptr<ScoreDB> g_pScoreDB;

namespace lunaticvibes::data
{

enum class GameWindowMode
{
    FULLSCREEN,
    BORDERLESS,
    WINDOWED
};
enum class GameVsyncMode {
    OFF,
    ON,
    ADAPTIVE
};

enum class FXType
{
    Off,
    Compressor,
    Echo,
    SfxReverb,
    LowPass,
    HighPass,

    // ...
};

enum class FreqModifierType
{
    Off,
    Frequency,
    PitchOnly,
    SpeedOnly,
};

inline struct Struct_SystemData
{
    std::unordered_map<std::string, long long> timers;

    SceneType gNextScene = SceneType::SELECT;

    GameWindowMode windowMode = GameWindowMode::WINDOWED;
    GameVsyncMode vsyncMode = GameVsyncMode::OFF;
    unsigned currentRenderFPS = 0;
    unsigned currentInputFPS = 0;
    unsigned currentUpdateFPS = 0;

    unsigned dateYear = 0;
    unsigned dateMonthOfYear = 0;
    unsigned dateDayOfMonth = 0;
    unsigned timeHour = 0;
    unsigned timeMin = 0;
    unsigned timeSec = 0;

    double volumeMaster = 0.8;
    double volumeKey = 0.8;
    double volumeBgm = 0.8;

    bool equalizerEnabled = false;
    int equalizerVal62_5hz = 0;
    int equalizerVal160hz = 0;
    int equalizerVal400hz = 0;
    int equalizerVal1khz = 0;
    int equalizerVal2_5khz = 0;
    int equalizerVal6_25khz = 0;
    int equalizerVal16khz = 0;

    FXType fxType = FXType::Off;
    double fxVal = 0.0;

    FreqModifierType freqType = FreqModifierType::Off;
    int freqVal = 0;    // -12 ~ 12

    bool IROnline = false;

    std::string playerName;

    int scratchAxisValue[2] = { 0, 0 };

} SystemData;

}