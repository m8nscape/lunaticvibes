#pragma once
#include "config.h"

// Stores in player folder
constexpr char CONFIG_FILE_PROFILE[] = "settings.yml";

namespace cfg
{
    constexpr char P_PLAYERNAME[] = "PlayerName";

    constexpr char P_MISSBGA_LENGTH[] = "MissBGATime";
    constexpr char P_MIN_INPUT_INTERVAL[] = "MinInputInterval";
    constexpr char P_INPUT_POLLING_RATE[] = "InputPollingRate";
    constexpr char P_NEW_SONG_DURATION[] = "NewSongDuration";

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

    constexpr char P_TARGET_TYPE[] = "TargetType";
    constexpr char P_TARGET_TYPE_0[] = "NotSet";
    constexpr char P_TARGET_TYPE_MYBEST[] = "MyBest";
    constexpr char P_TARGET_TYPE_AAA[] = "RankAAA";
    constexpr char P_TARGET_TYPE_AA[] = "RankAA";
    constexpr char P_TARGET_TYPE_A[] = "RankA";
    constexpr char P_TARGET_TYPE_DEFAULT[] = "Default";
    constexpr char P_TARGET_TYPE_IR_TOP[] = "IRTop";
    constexpr char P_TARGET_TYPE_IR_NEXT[] = "IRNext";
    constexpr char P_TARGET_TYPE_IR_AVERAGE[] = "IRAverage";
    enum class eTargetType
    {
        NOTSET,
        MYBEST,
        RANK_AAA,
        RANK_AA,
        RANK_A,
        DEFAULT,
        IR_TOP,
        IR_NEXT,
        IR_AVERAGE,
    };

    constexpr char P_CHART_OP[] = "ChartOption";
    constexpr char P_CHART_OP_NORMAL[] = "Normal";
    constexpr char P_CHART_OP_MIRROR[] = "Mirror";
    constexpr char P_CHART_OP_RANDOM[] = "Random";
    constexpr char P_CHART_OP_SRAN[] = "SRandom";
    constexpr char P_CHART_OP_HRAN[] = "HRandom";
    constexpr char P_CHART_OP_ALLSCR[] = "AllScratch";
    constexpr char P_CHART_OP_RRAN[] = "RRandom";
    constexpr char P_CHART_OP_DB_SYNCHRONIZE[] = "Synchronize";
    constexpr char P_CHART_OP_DB_SYMMETRY[] = "Symmetry";
    enum class eChartOp
    {
        NORMAL,
        MIRROR,
        RANDOM,
        SRAN,
        HRAN,
        ASCR,
        RRAN,
        DB_SYNCHRONIZE,
        DB_SYMMETRY
    };

    constexpr char P_CHART_ASSIST_OP[] = "ChartAssistOption";
    constexpr char P_CHART_ASSIST_OP_NONE[] = "None";
    constexpr char P_CHART_ASSIST_OP_AUTOSCR[] = "AutoScratch";
    enum class eChartAssistOp
    {
        NORMAL,
        AUTOSCR,
    };

    constexpr char P_LANE_EFFECT_OP[] = "LaneEffect";
    constexpr char P_LANE_EFFECT_OP_OFF[] = "Off";
    constexpr char P_LANE_EFFECT_OP_HIDDEN[] = "Hidden+";
    constexpr char P_LANE_EFFECT_OP_SUDDEN[] = "Sudden+";
    constexpr char P_LANE_EFFECT_OP_SUDHID[] = "SUD+&HID+";
    constexpr char P_LANE_EFFECT_OP_LIFT[] = "Lift";
    constexpr char P_LANE_EFFECT_OP_LIFTSUD[] = "LIFT&SUD+";
    enum class eLaneEffectOp
    {
        OFF,
        HIDDEN,
        SUDDEN,
        SUDHID,
        LIFT,
        LIFTSUD,
    };

    constexpr char P_GAUGE_OP[] = "GaugeOption";
    constexpr char P_GAUGE_OP_NORMAL[] = "Groove";
    constexpr char P_GAUGE_OP_EASY[] = "Easy";
    constexpr char P_GAUGE_OP_ASSISTEASY[] = "AssistEasy";
    constexpr char P_GAUGE_OP_HARD[] = "Hard";
    constexpr char P_GAUGE_OP_EXHARD[] = "ExHard";
    constexpr char P_GAUGE_OP_DEATH[] = "Death";

    constexpr char P_JUDGE_OFFSET[] = "JudgeOffset";

    constexpr char P_GHOST_TYPE[] = "GhostType";;
    constexpr char P_GHOST_TYPE_OFF[] = "Off";
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

    constexpr char P_GHOST_TARGET[] = "GhostTarget";         // Ghost Target Rate% (Target: Default)

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

    constexpr char P_LANGUAGE[] = "Language";

    constexpr char P_ADJUST_HISPEED_WITH_ARROWKEYS[] = "AdjustHispeedWithArrowKeys";
    constexpr char P_ADJUST_LANECOVER_WITH_START_67[] = "AdjustLanecoverWithStart67";
    constexpr char P_ADJUST_LANECOVER_WITH_MOUSEWHEEL[] = "AdjustLanecoverWithMouseWheel";
    constexpr char P_ADJUST_LANECOVER_WITH_ARROWKEYS[] = "AdjustLanecoverWithArrowKeys";

    constexpr char P_PREVIEW_DEDICATED[] = "PreviewDedicated";
    constexpr char P_PREVIEW_DIRECT[] = "PreviewDirect";

    constexpr char P_SELECT_KEYBINDINGS[] = "SelectKeybindings";
    constexpr char P_SELECT_KEYBINDINGS_7K[] = "7Keys";
    constexpr char P_SELECT_KEYBINDINGS_5K[] = "5Keys";
    constexpr char P_SELECT_KEYBINDINGS_9K[] = "9Keys";

    constexpr char P_ENABLE_NEW_RANDOM[] = "EnableNewRandomOptions";
    constexpr char P_ENABLE_NEW_GAUGE[] = "EnableNewGauges";
    constexpr char P_ENABLE_NEW_LANE_OPTION[] = "EnableNewLaneOptions";
    constexpr char P_ONLY_DISPLAY_MAIN_TITLE_ON_BARS[] = "OnlyDisplayMainTitleOnBars";
    constexpr char P_DISABLE_PLAYMODE_ALL[] = "DisablePlaymodeFilterAll";
    constexpr char P_DISABLE_DIFFICULTY_ALL[] = "DisableDifficultyFilterAll";
    constexpr char P_DISABLE_PLAYMODE_SINGLE[] = "DisablePlaymodeFilterSingle";
    constexpr char P_DISABLE_PLAYMODE_DOUBLE[] = "DisablePlaymodeFilterDouble";
    constexpr char P_IGNORE_DP_CHARTS[] = "IgnoreDPCharts";
    constexpr char P_IGNORE_9KEYS_CHARTS[] = "Ignore9kCharts";
    constexpr char P_IGNORE_5KEYS_IF_7KEYS_EXIST[] = "Ignore5keysIf7keysExist";
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
