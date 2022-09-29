#pragma once
#include "config.h"

// Stores in player folder
constexpr char CONFIG_FILE_PROFILE[] = "settings.yml";

namespace cfg
{
    constexpr char P_PLAYERNAME[] = "PlayerName";

    constexpr char P_MISSBGA_LENGTH[] = "MissBGATime";
    constexpr char P_MIN_INPUT_INTERVAL[] = "MinInputInterval";
    constexpr char P_NEW_SONG_DURATION[] = "NewSongDuration";
    constexpr char P_MOUSE_ANALOG[] = "MouseAnalog";
    constexpr char P_NO_COMBINE_CHARTS[] = "NoCombineCharts";

    constexpr char P_BASESPEED[] = "Basespeed";

    constexpr char P_HISPEED[] = "Hispeed";

    constexpr char P_GREENNUMBER[] = "GreenNumber";
    constexpr char P_LOCK_SPEED[] = "LockSpeed";

    constexpr char P_BGA_TYPE[] = "BGAType";
    constexpr char P_BGA_TYPE_OFF[] = "Off";
    constexpr char P_BGA_TYPE_ON[] = "On";
    constexpr char P_BGA_TYPE_AUTOPLAY[] = "Autoplay";
    enum class eBgaType
    {
        OFF,
        ON,
        AUTOPLAY,
    };

    constexpr char P_BGA_SIZE[] = "BGASize";
    constexpr char P_BGA_SIZE_NORMAL[] = "Normal";
    constexpr char P_BGA_SIZE_EXTEND[] = "Extend";
    enum class eBgaSize
    {
        NORMAL,
        EXTEND,
    };

    constexpr char P_SPEED_TYPE[] = "SpeedType";
    constexpr char P_SPEED_TYPE_NORMAL[] = "Normal";
    constexpr char P_SPEED_TYPE_END[] = "End";
    constexpr char P_SPEED_TYPE_MIN[] = "Min";
    constexpr char P_SPEED_TYPE_MAX[] = "Max";
    constexpr char P_SPEED_TYPE_AVG[] = "Average";
    constexpr char P_SPEED_TYPE_CONSTANT[] = "Constant";
    enum class eSpeedType
    {
        NORMAL,
        END,
        MIN,
        MAX,
        AVG,
        CONSTANT
    };

    constexpr char P_LIFT[] = "Lift";

    constexpr char P_CHART_OP[] = "ChartOption";
    constexpr char P_CHART_OP_NORMAL[] = "Normal";
    constexpr char P_CHART_OP_MIRROR[] = "Mirror";
    constexpr char P_CHART_OP_RANDOM[] = "Random";
    constexpr char P_CHART_OP_SRAN[] = "SRandom";
    constexpr char P_CHART_OP_HRAN[] = "HRandom";
    constexpr char P_CHART_OP_ALLSCR[] = "AllScratch";
    enum class eChartOp
    {
        NORMAL,
        MIRROR,
        RANDOM,
        SRAN,
        HRAN,
        ASCR
    };

    constexpr char P_CHART_ASSIST_OP[] = "ChartAssistOption";
    constexpr char P_CHART_ASSIST_OP_NONE[] = "None";
    constexpr char P_CHART_ASSIST_OP_AUTOSCR[] = "AutoScratch";
    enum class eChartAssistOp
    {
        NORMAL,
        AUTOSCR,
    };

    constexpr char P_GAUGE_OP[] = "GaugeOption";
    constexpr char P_GAUGE_OP_NORMAL[] = "Groove";
    constexpr char P_GAUGE_OP_EASY[] = "Easy";
    constexpr char P_GAUGE_OP_ASSIST[] = "AssistEasy";
    constexpr char P_GAUGE_OP_HARD[] = "Hard";
    constexpr char P_GAUGE_OP_EXHARD[] = "ExHard";
    constexpr char P_GAUGE_OP_DEATH[] = "Death";

    constexpr char P_COMBO_POS_X[] = "ComboPositionOffsetX";
    constexpr char P_COMBO_POS_Y[] = "ComboPositionOffsetY";

    constexpr char P_JUDGE_POS_X[] = "JudgePositionOffsetX";
    constexpr char P_JUDGE_POS_Y[] = "JudgePositionOffsetY";

    constexpr char P_DISPLAY_OFFSET[] = "DisplayOffset";

    constexpr char P_JUDGE_OFFSET[] = "JudgeOffset";

    constexpr char P_JUDGE_STAT[] = "JudgeStatistics";       // off, on


    constexpr char P_GHOST_COLOR[] = "GhostColor";
    constexpr char P_GHOST_COLOR_NORMAL[] = "Normal";
    constexpr char P_GHOST_COLOR_MONOCHROME[] = "Mono";
    enum class eGhostColor
    {
        NORMAL,
        MONO
    };

    constexpr char P_GHOST_TYPE[] = "GhostType";;
    constexpr char P_GHOST_TYPE_A[] = "A";
    constexpr char P_GHOST_TYPE_B[] = "B";
    constexpr char P_GHOST_TYPE_C[] = "C";
    enum class eGhostType
    {
        OFF,
        ABOVE_JUDGE,
        NEAR_JUDGE,
        NEAR_JUDGE_DOWN,
    };

    constexpr char P_GHOST_TARGET[] = "GhostTarget";         // Rate (since multi ruleset)

    constexpr char P_TARGET[] = "Target";
    constexpr char P_TARGET_MYBEST[] = "Best score";
    constexpr char P_TARGET_RANK_A[] = "Rank A";
    constexpr char P_TARGET_RANK_AA[] = "Rank AA";
    constexpr char P_TARGET_RANK_AAA[] = "Rank AAA";


    constexpr char P_GHOST_FS_TRIGGER[] = "GhostFSTrigger";  // except PERFECT, always
    constexpr char P_GHOST_FS_TRIGGER_SUB[] = "Normal";
    constexpr char P_GHOST_FS_TRIGGER_ALWAYS[] = "Always";
    enum class eGhostFSTrigger
    {
        OFF,
        NORMAL,
        ALWAYS
    };

    constexpr char P_GHOST_FS_TYPE[] = "GhostFSType";   //  normal, ms
    constexpr char P_GHOST_FS_TYPE_NORMAL[] = "Normal";
    constexpr char P_GHOST_FS_TYPE_TIME[] = "Time";
    enum class eGhostFSType
    {
        NORMAL,
        TIME
    };

    constexpr char P_GHOST_FS_POS_X[] = "GhostFSPosX";           // off, normal, ms, both
    constexpr char P_GHOST_FS_POS_Y[] = "GhostFSPosY";           // off, normal, ms, both

    constexpr char P_FILTER_KEYS[] = "KeysFilter";
    constexpr char P_FILTER_KEYS_ALL[] = "All";
    constexpr char P_FILTER_KEYS_SINGLE[] = "SINGLE";
    constexpr char P_FILTER_KEYS_7K[] = "7K";
    constexpr char P_FILTER_KEYS_5K[] = "5K";
    constexpr char P_FILTER_KEYS_DOUBLE[] = "DOUBLE";
    constexpr char P_FILTER_KEYS_14K[] = "14K";
    constexpr char P_FILTER_KEYS_10K[] = "10K";
    constexpr char P_FILTER_KEYS_9K[] = "9K";

    constexpr char P_SORT_MODE[] = "SortMode";
    constexpr char P_SORT_MODE_FOLDER[] = "Folder";
    constexpr char P_SORT_MODE_TITLE[] = "Title";
    constexpr char P_SORT_MODE_LEVEL[] = "Level";
    constexpr char P_SORT_MODE_CLEAR[] = "Clear";
    constexpr char P_SORT_MODE_RATE[] = "Rate";

    constexpr char P_DIFFICULTY_FILTER[] = "DifficultyFilter";
    constexpr char P_DIFFICULTY_FILTER_ALL[] = "All";
    constexpr char P_DIFFICULTY_FILTER_BEGINNER[] = "Beginner";
    constexpr char P_DIFFICULTY_FILTER_NORMAL[] = "Normal";
    constexpr char P_DIFFICULTY_FILTER_HYPER[] = "Hyper";
    constexpr char P_DIFFICULTY_FILTER_ANOTHER[] = "Another";
    constexpr char P_DIFFICULTY_FILTER_INSANE[] = "Insane";

    constexpr char P_FLIP[] = "Flip";
    constexpr char P_SCORE_GRAPH[] = "ScoreGraph";
    constexpr char P_LANECOVER_ENABLE[] = "Lanecover";
    constexpr char P_LANECOVER_TOP[] = "LanecoverTop";
    constexpr char P_LANECOVER_BOTTOM[] = "LanecoverBottom";

    constexpr char P_LIST_SCROLL_TIME_INITIAL[] = "SongListScrollTimeInit";
    constexpr char P_LIST_SCROLL_TIME_HOLD[] = "SongListScrollTimeHold";

    constexpr char P_VOL_MASTER[] = "MasterVolume";
    constexpr char P_VOL_KEY[] = "KeyVolume";
    constexpr char P_VOL_BGM[] = "BgmVolume";

    constexpr char P_EQ[] = "EQ";
    constexpr char P_EQ0[] = "EQ 62";
    constexpr char P_EQ1[] = "EQ 160";
    constexpr char P_EQ2[] = "EQ 400";
    constexpr char P_EQ3[] = "EQ 1k";
    constexpr char P_EQ4[] = "EQ 2.5k";
    constexpr char P_EQ5[] = "EQ 6.3k";
    constexpr char P_EQ6[] = "EQ 16k";

    constexpr char P_FREQ[] = "Freq";
    constexpr char P_FREQ_TYPE[] = "FreqType";
    constexpr char P_FREQ_TYPE_FREQ[] = "Frequency";
    constexpr char P_FREQ_TYPE_PITCH[] = "Pitch";
    constexpr char P_FREQ_TYPE_SPEED[] = "Speed";
    constexpr char P_FREQ_VAL[] = "FreqVal";

    constexpr char P_FX0[] = "FX0";
    constexpr char P_FX1[] = "FX1";
    constexpr char P_FX2[] = "FX2";
    constexpr char P_FX0_TARGET[] = "FX0Target";
    constexpr char P_FX1_TARGET[] = "FX1Target";
    constexpr char P_FX2_TARGET[] = "FX2Target";
    constexpr char P_FX0_TYPE[] = "FX0Type";
    constexpr char P_FX1_TYPE[] = "FX1Type";
    constexpr char P_FX2_TYPE[] = "FX2Type";
    constexpr char P_FX0_P1[] = "FX0P1";
    constexpr char P_FX1_P1[] = "FX1P1";
    constexpr char P_FX2_P1[] = "FX2P1";
    constexpr char P_FX0_P2[] = "FX0P2";
    constexpr char P_FX1_P2[] = "FX1P2";
    constexpr char P_FX2_P2[] = "FX2P2";
    constexpr char P_FX_TARGET_MASTER[] = "MASTER";
    constexpr char P_FX_TARGET_KEY[] = "KEY";
    constexpr char P_FX_TARGET_BGM[] = "BGM";
    constexpr char P_FX_TYPE_REVERB[] = "Reverb";
    constexpr char P_FX_TYPE_DELAY[] = "Delay";
    constexpr char P_FX_TYPE_LOWPASS[] = "Low Pass";
    constexpr char P_FX_TYPE_HIGHPASS[] = "High Pass";
    constexpr char P_FX_TYPE_FLANGER[] = "Flanger";
    constexpr char P_FX_TYPE_CHORUS[] = "Chorus";
    constexpr char P_FX_TYPE_DIST[] = "Distortion";

    constexpr char P_INPUT_DEADZONE_S1L[] = "InputDeadZoneS1L";
    constexpr char P_INPUT_DEADZONE_S1R[] = "InputDeadZoneS1R";
    constexpr char P_INPUT_DEADZONE_K1Start[] = "InputDeadZoneK1Start";
    constexpr char P_INPUT_DEADZONE_K1Select[] = "InputDeadZoneK1Select";
    constexpr char P_INPUT_DEADZONE_K11[] = "InputDeadZoneK11";
    constexpr char P_INPUT_DEADZONE_K12[] = "InputDeadZoneK12";
    constexpr char P_INPUT_DEADZONE_K13[] = "InputDeadZoneK13";
    constexpr char P_INPUT_DEADZONE_K14[] = "InputDeadZoneK14";
    constexpr char P_INPUT_DEADZONE_K15[] = "InputDeadZoneK15";
    constexpr char P_INPUT_DEADZONE_K16[] = "InputDeadZoneK16";
    constexpr char P_INPUT_DEADZONE_K17[] = "InputDeadZoneK17";
    constexpr char P_INPUT_DEADZONE_K18[] = "InputDeadZoneK18";
    constexpr char P_INPUT_DEADZONE_K19[] = "InputDeadZoneK19";
    constexpr char P_INPUT_DEADZONE_S2L[] = "InputDeadZoneS2L";
    constexpr char P_INPUT_DEADZONE_S2R[] = "InputDeadZoneS2R";
    constexpr char P_INPUT_DEADZONE_K2Start[] = "InputDeadZoneK2Start";
    constexpr char P_INPUT_DEADZONE_K2Select[] = "InputDeadZoneK2Select";
    constexpr char P_INPUT_DEADZONE_K21[] = "InputDeadZoneK21";
    constexpr char P_INPUT_DEADZONE_K22[] = "InputDeadZoneK22";
    constexpr char P_INPUT_DEADZONE_K23[] = "InputDeadZoneK23";
    constexpr char P_INPUT_DEADZONE_K24[] = "InputDeadZoneK24";
    constexpr char P_INPUT_DEADZONE_K25[] = "InputDeadZoneK25";
    constexpr char P_INPUT_DEADZONE_K26[] = "InputDeadZoneK26";
    constexpr char P_INPUT_DEADZONE_K27[] = "InputDeadZoneK27";
    constexpr char P_INPUT_DEADZONE_K28[] = "InputDeadZoneK28";
    constexpr char P_INPUT_DEADZONE_K29[] = "InputDeadZoneK29";
    constexpr char P_INPUT_SPEED_S1A[] = "InputSpeedS1L";
    constexpr char P_INPUT_SPEED_S2A[] = "InputSpeedS1R";

    enum class eProfile
    {
        PLAYERNAME,
        BASESPD,
        HISPEED,
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
protected:
    Path _dirPath;
public:
    ConfigProfile(const std::string& profile) : vConfig(profile, CONFIG_FILE_PROFILE)
    {
        _dirPath = GAMEDATA_PATH;
        _dirPath /= "profile";
        _dirPath /= profile;
        
        Path p(_dirPath);
        if (!fs::exists(p)) fs::create_directories(p);
    }
    virtual ~ConfigProfile() = default;

    std::string getName() const;
    void setName(const std::string& name);
    Path getPath() const { return _dirPath; }
    virtual void setDefaults() noexcept override;
};
