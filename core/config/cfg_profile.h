#pragma once
#include "config.h"

// Stores in player folder
inline const StringPath CONFIG_FILE_PROFILE = "profile.yml"_p;

namespace cfg
{
    inline const StringContent P_PLAYERNAME = "PlayerName";

    inline const StringContent P_BASESPEED = "Basespeed";

    inline const StringContent P_HISPEED = "Hispeed";

    inline const StringContent P_REGULAR_SPEED = "RegularSpeed";           // 0.1f in 60fps

    inline const StringContent P_SPEED_TYPE = "SpeedType";
    inline const StringContent P_SPEED_TYPE_NORMAL = "Normal";
    inline const StringContent P_SPEED_TYPE_END = "End";
    inline const StringContent P_SPEED_TYPE_MIN = "Min";
    inline const StringContent P_SPEED_TYPE_MAX = "Max";
    inline const StringContent P_SPEED_TYPE_AVG = "Average";
    inline const StringContent P_SPEED_TYPE_CONSTANT = "Constant";
    enum class eSpeedType
    {
        NORMAL,
        END,
        MIN,
        MAX,
        AVG,
        CONSTANT
    };

    inline const StringContent P_LANECOVER = "Lanecover";

    inline const StringContent P_LIFT = "Lift";

    inline const StringContent P_CHART_OP = "ChartOption";
    inline const StringContent P_CHART_OP_NORMAL = "Normal";
    inline const StringContent P_CHART_OP_MIRROR = "Mirror";
    inline const StringContent P_CHART_OP_RANDOM = "Random";
    inline const StringContent P_CHART_OP_SRAN = "SRandom";
    inline const StringContent P_CHART_OP_HRAN = "HRandom";
    inline const StringContent P_CHART_OP_ALLSCR = "AllScratch";
    enum class eChartOp
    {
        NORMAL,
        MIRROR,
        RANDOM,
        SRAN,
        HRAN,
        ASCR
    };

    inline const StringContent P_GAUGE_OP = "GaugeOption";
    inline const StringContent P_GAUGE_OP_NORMAL = "Groove";
    inline const StringContent P_GAUGE_OP_EASY = "Easy";
    inline const StringContent P_GAUGE_OP_ASSIST = "AssistEasy";
    inline const StringContent P_GAUGE_OP_HARD = "Hard";
    inline const StringContent P_GAUGE_OP_EXHARD = "ExHard";
    inline const StringContent P_GAUGE_OP_DEATH = "Death";

    inline const StringContent P_COMBO_POS_X = "ComboPositionOffsetX";
    inline const StringContent P_COMBO_POS_Y = "ComboPositionOffsetY";

    inline const StringContent P_JUDGE_POS_X = "JudgePositionOffsetX";
    inline const StringContent P_JUDGE_POS_Y = "JudgePositionOffsetY";

    inline const StringContent P_DISPLAY_OFFSET = "DisplayOffset";

    inline const StringContent P_JUDGE_OFFSET = "JudgeOffset";

    inline const StringContent P_JUDGE_STAT = "JudgeStatistics";       // off, on

    inline const StringContent P_GHOST_TYPE = "GhostType";
    inline const StringContent P_GHOST_TYPE_NORMAL = "Normal";
    inline const StringContent P_GHOST_TYPE_MONOCHROME = "Mono";
    enum class eGhostType
    {
        OFF,
        NORMAL,
        MONO
    };

    inline const StringContent P_GHOST_POS_X = "GhostPosX";
    inline const StringContent P_GHOST_POS_Y = "GhostPosY";

    inline const StringContent P_GHOST_TARGET = "GhostTarget";         // Rate (since multi ruleset)

    inline const StringContent P_GHOST_FS_TRIGGER = "GhostFSTrigger";  // except PERFECT, always
    inline const StringContent P_GHOST_FS_TRIGGER_SUB = "Normal";
    inline const StringContent P_GHOST_FS_TRIGGER_ALWAYS = "Always";
    enum class eGhostFSTrigger
    {
        OFF,
        NORMAL,
        ALWAYS
    };

    inline const StringContent P_GHOST_FS_TYPE = "GhostFSType";   //  normal, ms
    inline const StringContent P_GHOST_FS_TYPE_NORMAL = "Normal";
    inline const StringContent P_GHOST_FS_TYPE_TIME = "Time";
    enum class eGhostFSType
    {
        NORMAL,
        TIME
    };

    inline const StringContent P_GHOST_FS_POS_X = "GhostFSPosX";           // off, normal, ms, both
    inline const StringContent P_GHOST_FS_POS_Y = "GhostFSPosY";           // off, normal, ms, both

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
    void rename(const std::string& name);
    virtual void setDefaults() noexcept override;
};
