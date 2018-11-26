#pragma once
#include "config.h"

// Stores in player folder
static const StringPath CONFIG_FILE_PROFILE = "profile.yml";

namespace cfg
{
    const char* P_PLAYERNAME = "PlayerName";

    const char* P_BASESPEED = "Basespeed";

    const char* P_HISPEED = "Hispeed";

    const char* P_REGULAR_SPEED = "GreenNbr";           // 0.1f in 60fps, iidx style

    const char* P_SPEED_TYPE = "SpeedType";
    const char* P_SPEED_TYPE_NORMAL = "Normal";
    const char* P_SPEED_TYPE_END = "End";
    const char* P_SPEED_TYPE_MIN = "Min";
    const char* P_SPEED_TYPE_MAX = "Max";
    const char* P_SPEED_TYPE_AVG = "Average";
    const char* P_SPEED_TYPE_CONSTANT = "Constant";
    enum class eSpeedType
    {
        NORMAL,
        END,
        MIN,
        MAX,
        AVG,
        CONSTANT
    };

    const char* P_LANECOVER = "Lanecover";

    const char* P_LIFT = "Lift";

    const char* P_CHART_OP = "ChartOption";
    const char* P_CHART_OP_NORMAL = "Normal";
    const char* P_CHART_OP_MIRROR = "Mirror";
    const char* P_CHART_OP_RANDOM = "Random";
    const char* P_CHART_OP_SRAN = "SRandom";
    const char* P_CHART_OP_HRAN = "HRandom";
    const char* P_CHART_OP_ALLSCR = "AllScratch";
    enum class eChartOp
    {
        NORMAL,
        MIRROR,
        RANDOM,
        SRAN,
        HRAN,
        ASCR
    };

    const char* P_GAUGE_OP = "GaugeOption";
    const char* P_GAUGE_OP_NORMAL = "Groove";
    const char* P_GAUGE_OP_EASY = "Easy";
    const char* P_GAUGE_OP_ASSIST = "AssistEasy";
    const char* P_GAUGE_OP_HARD = "Hard";
    const char* P_GAUGE_OP_EXHARD = "ExHard";
    const char* P_GAUGE_OP_DEATH = "Death";
    enum class eGaugeOp
    {
        GROOVE,
        EASY,
        ASSIST,
        HARD,
        EXHARD,
        DEATH
    };

    const char* P_COMBO_POS_X = "ComboPositionOffsetX";
    const char* P_COMBO_POS_Y = "ComboPositionOffsetY";

    const char* P_JUDGE_POS_X = "JudgePositionOffsetX";
    const char* P_JUDGE_POS_Y = "JudgePositionOffsetY";

    const char* P_DISPLAY_LATENCY = "DisplayLatency";

    const char* P_JUDGE_LATENCY = "JudgeLatency";

    const char* P_JUDGE_STAT = "JudgeStatistics";       // off, on

    const char* P_GHOST_TYPE = "GhostType";
    const char* P_GHOST_TYPE_NORMAL = "Normal";
    const char* P_GHOST_TYPE_MONOCHROME = "Mono";
    enum class eGhostType
    {
        OFF,
        NORMAL,
        MONO
    };

    const char* P_GHOST_POS_X = "GhostPosX";
    const char* P_GHOST_POS_Y = "GhostPosY";

    const char* P_GHOST_TARGET = "GhostTarget";         // Rate (since multi ruleset)

    const char* P_GHOST_FS_TRIGGER = "GhostFSTrigger";  // except PERFECT, always
    const char* P_GHOST_FS_TRIGGER_SUB = "Normal";
    const char* P_GHOST_FS_TRIGGER_ALWAYS = "Always";
    enum class eGhostFSTrigger
    {
        OFF,
        NORMAL,
        ALWAYS
    };

    const char* P_GHOST_FS_TYPE = "GhostFSType";   //  normal, ms
    const char* P_GHOST_FS_TYPE_NORMAL = "Normal";
    const char* P_GHOST_FS_TYPE_TIME = "Time";
    enum class eGhostFSType
    {
        NORMAL,
        TIME
    };

    const char* P_GHOST_FS_POS_X = "GhostFSPosX";           // off, normal, ms, both
    const char* P_GHOST_FS_POS_Y = "GhostFSPosY";           // off, normal, ms, both

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
    ConfigProfile();
    virtual ~ConfigProfile();

    virtual void setDefaults() noexcept override;
};
