#include "common/pch.h"
#include "cfg_profile.h"

namespace lunaticvibes
{

namespace fs = std::filesystem;

std::string ConfigProfile::getName() const
{
    return _yaml[std::string(cfg::P_PLAYERNAME)].as<std::string>("Unnamed");
}
void ConfigProfile::setName(const std::string& name)
{
    _yaml[cfg::P_PLAYERNAME] = name;
}
void ConfigProfile::setDefaults() noexcept
{
    using namespace cfg;
    set(P_PLAYERNAME, "Unnamed");
    set(P_MISSBGA_LENGTH, 500);
    set(P_MIN_INPUT_INTERVAL, 5);
    set(P_INPUT_POLLING_RATE, 1000);
    set(P_NEW_SONG_DURATION, 6);
    set(P_BASESPEED, 1.0);
    set(P_HISPEED, 1.0);
    set(P_HISPEED_2P, 1.0);
    set(P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
    set(P_TARGET_TYPE, P_TARGET_TYPE_MYBEST);
    set(P_LANECOVER_TOP, 0);
    set(P_LANECOVER_TOP_2P, 0);
    set(P_LANECOVER_BOTTOM, 0);
    set(P_LANECOVER_BOTTOM_2P, 0);
    set(P_CHART_OP, P_CHART_OP_NORMAL);
    set(P_CHART_OP_2P, P_CHART_OP_NORMAL);
    set(P_GAUGE_OP, P_GAUGE_OP_NORMAL);
    set(P_GAUGE_OP_2P, P_GAUGE_OP_NORMAL);
    set(P_CHART_ASSIST_OP, 0);
    set(P_CHART_ASSIST_OP_2P, 0);
    set(P_JUDGE_OFFSET, 0);
    set(P_GHOST_TYPE, false);
    set(P_GHOST_TYPE_2P, false);
    set(P_GHOST_TARGET, 50);
    set(P_FILTER_KEYS, P_FILTER_KEYS_ALL);
    set(P_SORT_MODE, P_SORT_MODE_FOLDER);
    set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
    set(P_FLIP, false);

    set(P_LIST_SCROLL_TIME_INITIAL, 300);
    set(P_LIST_SCROLL_TIME_HOLD, 150);

    set(P_VOL_MASTER, 1.0);
    set(P_VOL_BGM, 1.0);
    set(P_VOL_KEY, 1.0);

    set(P_EQ, false);
    set(P_EQ0, 0.0);
    set(P_EQ1, 0.0);
    set(P_EQ2, 0.0);
    set(P_EQ3, 0.0);
    set(P_EQ4, 0.0);
    set(P_EQ5, 0.0);
    set(P_EQ6, 0.0);

    set(P_FREQ_TYPE, "Off");
    set(P_FREQ_VAL, 0);

    set(P_FX0, false);
    set(P_FX0_TYPE, false);
    set(P_FX0_P1, 0);

    set(P_PREVIEW_DEDICATED, true);
    set(P_PREVIEW_DIRECT, true);
    set(P_SELECT_KEYBINDINGS, P_SELECT_KEYBINDINGS_7K);
    set(P_ENABLE_NEW_RANDOM, false);
    set(P_ENABLE_NEW_GAUGE, false);
    set(P_ENABLE_NEW_LANE_OPTION, false);

    set(P_ADJUST_HISPEED_WITH_ARROWKEYS, true);
    set(P_ADJUST_HISPEED_WITH_SELECT, false);
    set(P_ADJUST_LANECOVER_WITH_START_67, false);
    set(P_ADJUST_LANECOVER_WITH_MOUSEWHEEL, true);
    set(P_ADJUST_LANECOVER_WITH_ARROWKEYS, true);
}

}
