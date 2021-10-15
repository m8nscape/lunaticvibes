#pragma once
#include "config.h"

// Stores in player folder
inline const char* CONFIG_FILE_PROFILE = "profile.yml";

namespace cfg
{
    inline const char* P_PLAYERNAME = "PlayerName";

    inline const char* P_BASESPEED = "Basespeed";

    inline const char* P_HISPEED = "Hispeed";

    inline const char* P_REGULAR_SPEED = "RegularSpeed";           // 0.1f in 60fps

	inline const char* P_LOAD_BGA = "LoadBga";

    inline const char* P_SPEED_TYPE = "SpeedType";
    inline const char* P_SPEED_TYPE_NORMAL = "Normal";
    inline const char* P_SPEED_TYPE_END = "End";
    inline const char* P_SPEED_TYPE_MIN = "Min";
    inline const char* P_SPEED_TYPE_MAX = "Max";
    inline const char* P_SPEED_TYPE_AVG = "Average";
    inline const char* P_SPEED_TYPE_CONSTANT = "Constant";
    enum class eSpeedType
    {
        NORMAL,
        END,
        MIN,
        MAX,
        AVG,
        CONSTANT
    };
    
    inline const char* P_LANECOVER = "Lanecover";

    inline const char* P_LIFT = "Lift";

    inline const char* P_CHART_OP = "ChartOption";
    inline const char* P_CHART_OP_NORMAL = "Normal";
    inline const char* P_CHART_OP_MIRROR = "Mirror";
    inline const char* P_CHART_OP_RANDOM = "Random";
    inline const char* P_CHART_OP_SRAN = "SRandom";
    inline const char* P_CHART_OP_HRAN = "HRandom";
    inline const char* P_CHART_OP_ALLSCR = "AllScratch";
    enum class eChartOp
    {
        NORMAL,
        MIRROR,
        RANDOM,
        SRAN,
        HRAN,
        ASCR
    };

    inline const char* P_GAUGE_OP = "GaugeOption";
    inline const char* P_GAUGE_OP_NORMAL = "Groove";
    inline const char* P_GAUGE_OP_EASY = "Easy";
    inline const char* P_GAUGE_OP_ASSIST = "AssistEasy";
    inline const char* P_GAUGE_OP_HARD = "Hard";
    inline const char* P_GAUGE_OP_EXHARD = "ExHard";
    inline const char* P_GAUGE_OP_DEATH = "Death";

    inline const char* P_COMBO_POS_X = "ComboPositionOffsetX";
    inline const char* P_COMBO_POS_Y = "ComboPositionOffsetY";

    inline const char* P_JUDGE_POS_X = "JudgePositionOffsetX";
    inline const char* P_JUDGE_POS_Y = "JudgePositionOffsetY";

    inline const char* P_DISPLAY_OFFSET = "DisplayOffset";

    inline const char* P_JUDGE_OFFSET = "JudgeOffset";

    inline const char* P_JUDGE_STAT = "JudgeStatistics";       // off, on


    inline const char* P_GHOST_COLOR = "GhostColor";
    inline const char* P_GHOST_COLOR_NORMAL = "Normal";
    inline const char* P_GHOST_COLOR_MONOCHROME = "Mono";
    enum class eGhostColor
    {
        NORMAL,
        MONO
    };

    inline const char* P_GHOST_TYPE = "GhostType";;
    inline const char* P_GHOST_TYPE_A = "A";
    inline const char* P_GHOST_TYPE_B = "B";
    inline const char* P_GHOST_TYPE_C = "C";
    enum class eGhostType
    {
        OFF,
        ABOVE_JUDGE,
        NEAR_JUDGE,
        NEAR_JUDGE_DOWN,
    };

    inline const char* P_GHOST_TARGET = "GhostTarget";         // Rate (since multi ruleset)

    inline const char* P_TARGET = "Target";
    inline const char* P_TARGET_MYBEST = "Best score";
    inline const char* P_TARGET_RANK_A = "Rank A";
    inline const char* P_TARGET_RANK_AA = "Rank AA";
    inline const char* P_TARGET_RANK_AAA = "Rank AAA";


    inline const char* P_GHOST_FS_TRIGGER = "GhostFSTrigger";  // except PERFECT, always
    inline const char* P_GHOST_FS_TRIGGER_SUB = "Normal";
    inline const char* P_GHOST_FS_TRIGGER_ALWAYS = "Always";
    enum class eGhostFSTrigger
    {
        OFF,
        NORMAL,
        ALWAYS
    };

    inline const char* P_GHOST_FS_TYPE = "GhostFSType";   //  normal, ms
    inline const char* P_GHOST_FS_TYPE_NORMAL = "Normal";
    inline const char* P_GHOST_FS_TYPE_TIME = "Time";
    enum class eGhostFSType
    {
        NORMAL,
        TIME
    };

    inline const char* P_GHOST_FS_POS_X = "GhostFSPosX";           // off, normal, ms, both
    inline const char* P_GHOST_FS_POS_Y = "GhostFSPosY";           // off, normal, ms, both

    inline const char* P_PLAY_MODE = "PlayMode";
    inline const char* P_PLAY_MODE_ALL = "All";
    inline const char* P_PLAY_MODE_7K = "7K";
    inline const char* P_PLAY_MODE_5K = "5K";
    inline const char* P_PLAY_MODE_14K = "14K";
    inline const char* P_PLAY_MODE_10K = "10K";
    inline const char* P_PLAY_MODE_9K = "9K";

    inline const char* P_SORT_MODE = "SortMode";
    inline const char* P_SORT_MODE_FOLDER = "Folder";
    inline const char* P_SORT_MODE_TITLE = "Title";
    inline const char* P_SORT_MODE_LEVEL = "Level";
    inline const char* P_SORT_MODE_CLEAR = "Clear";
    inline const char* P_SORT_MODE_RATE = "Rate";

    inline const char* P_DIFFICULTY_FILTER = "DifficultyFilter";
    inline const char* P_DIFFICULTY_FILTER_ALL = "All";
    inline const char* P_DIFFICULTY_FILTER_BEGINNER = "Beginner";
    inline const char* P_DIFFICULTY_FILTER_NORMAL = "Normal";
    inline const char* P_DIFFICULTY_FILTER_HYPER = "Hyper";
    inline const char* P_DIFFICULTY_FILTER_ANOTHER = "Another";
    inline const char* P_DIFFICULTY_FILTER_INSANE = "Insane";

    inline const char* P_BATTLE = "Battle";
    inline const char* P_FLIP = "Flip";
    inline const char* P_SCORE_GRAPH = "ScoreGraph";

    inline const char* P_LIST_SCROLL_TIME_INITIAL = "SongListScrollTimeInit";
    inline const char* P_LIST_SCROLL_TIME_HOLD = "SongListScrollTimeHold";

    inline const char* P_VOL_MASTER = "MasterVolume";
    inline const char* P_VOL_KEY = "KeyVolume";
    inline const char* P_VOL_BGM = "BgmVolume";

    inline const char* P_EQ = "EQ";
    inline const char* P_EQ0 = "EQ 62";
    inline const char* P_EQ1 = "EQ 160";
    inline const char* P_EQ2 = "EQ 400";
    inline const char* P_EQ3 = "EQ 1k";
    inline const char* P_EQ4 = "EQ 2.5k";
    inline const char* P_EQ5 = "EQ 6.3k";
    inline const char* P_EQ6 = "EQ 16k";

    inline const char* P_FREQ = "Freq";
    inline const char* P_FREQ_TYPE = "FreqType";
    inline const char* P_FREQ_TYPE_FREQ = "Frequency";
    inline const char* P_FREQ_TYPE_PITCH = "Pitch";
    inline const char* P_FREQ_TYPE_SPEED = "Speed";
    inline const char* P_FREQ_VAL = "FreqVal";

    inline const char* P_FX0 = "FX0";
    inline const char* P_FX1 = "FX1";
    inline const char* P_FX2 = "FX2";
    inline const char* P_FX0_TARGET = "FX0Target";
    inline const char* P_FX1_TARGET = "FX1Target";
    inline const char* P_FX2_TARGET = "FX2Target";
    inline const char* P_FX0_TYPE = "FX0Type";
    inline const char* P_FX1_TYPE = "FX1Type";
    inline const char* P_FX2_TYPE = "FX2Type";
    inline const char* P_FX0_P1 = "FX0P1";
    inline const char* P_FX1_P1 = "FX1P1";
    inline const char* P_FX2_P1 = "FX2P1";
    inline const char* P_FX0_P2 = "FX0P2";
    inline const char* P_FX1_P2 = "FX1P2";
    inline const char* P_FX2_P2 = "FX2P2";
    inline const char* P_FX_TARGET_MASTER = "MASTER";
    inline const char* P_FX_TARGET_KEY = "KEY";
    inline const char* P_FX_TARGET_BGM = "BGM";
    inline const char* P_FX_TYPE_REVERB = "Reverb";
    inline const char* P_FX_TYPE_DELAY = "Delay";
    inline const char* P_FX_TYPE_LOWPASS = "Low Pass";
    inline const char* P_FX_TYPE_HIGHPASS = "High Pass";
    inline const char* P_FX_TYPE_FLANGER = "Flanger";
    inline const char* P_FX_TYPE_CHORUS = "Chorus";
    inline const char* P_FX_TYPE_DIST = "Distortion";

    enum class eProfile
    {
        PLAYERNAME,
        BASESPD,
        HISPEED,
        REGULARSPD,
        SPEEDTYPE,
        LANECOVER,
        LIFT,
        CHART_OP,
        GAUGE_OP,
        COMBO_POS_X,
        COMBO_POS_Y,
        JUDGE_POS_X,
        JUDGE_POS_Y,
        DISPLAY_LATENCY,
        JUDGE_LATENCY,
        JUDGE_STAT,
        GHOST_TYPE,
        GHOST_POS_X,
        GHOST_POS_Y,
        GHOST_TARGET,
        GHOST_FS_TRIGGER,
        GHOST_FS_TYPE,
        GHOST_FS_POS_X,
        GHOST_FS_POS_Y
    };
}

class ConfigProfile : public vConfig
{
public:
    ConfigProfile(const std::string& profile) : vConfig(profile, CONFIG_FILE_PROFILE) {}
    virtual ~ConfigProfile() = default;

    std::string getName() const;
    virtual void setDefaults() noexcept override;
};
