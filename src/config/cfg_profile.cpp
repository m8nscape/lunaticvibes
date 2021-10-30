#include "common/log.h"
#include <filesystem>
#include <fstream>
#include "cfg_profile.h"

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
    set(P_NEW_SONG_DURATION, 6);
    set(P_MOUSE_ANALOG, false);
	set(P_BASESPEED, 1.0);
	set(P_HISPEED, 1.0);
	set(P_REGULAR_SPEED, 1200);
	set(P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
	set(P_LOAD_BGA, ON);
    set(P_LANECOVER, 0);
    set(P_LIFT, 0);
    set(P_CHART_OP, P_CHART_OP_NORMAL);
    set(P_GAUGE_OP, P_GAUGE_OP_NORMAL);
    set(P_CHART_ASSIST_OP, P_CHART_ASSIST_OP_NONE);
    set(P_COMBO_POS_X, 0);
    set(P_COMBO_POS_Y, 0);
    set(P_JUDGE_POS_X, 0);
    set(P_JUDGE_POS_Y, 0);
    set(P_DISPLAY_OFFSET, 0);
    set(P_JUDGE_OFFSET, 0);
    set(P_JUDGE_STAT, OFF);
    set(P_GHOST_COLOR, P_GHOST_COLOR_NORMAL);
    set(P_GHOST_TYPE, OFF);
    set(P_GHOST_TARGET, 50);
    set(P_TARGET, OFF);
    set(P_GHOST_FS_TRIGGER, OFF);
    set(P_GHOST_FS_TYPE, P_GHOST_FS_TYPE_NORMAL);
    set(P_GHOST_FS_POS_X, 0);
    set(P_GHOST_FS_POS_Y, 0);
    set(P_PLAY_MODE, P_PLAY_MODE_ALL);
    set(P_SORT_MODE, P_SORT_MODE_FOLDER);
    set(P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL);
    set(P_BATTLE, OFF);
    set(P_FLIP, OFF);
    set(P_SCORE_GRAPH, OFF);

    set(P_LIST_SCROLL_TIME_INITIAL, 300);
    set(P_LIST_SCROLL_TIME_HOLD, 150);

    set(P_VOL_MASTER, 1.0);
    set(P_VOL_BGM, 1.0);
    set(P_VOL_KEY, 1.0);

    set(P_EQ, OFF);
    set(P_EQ0, 0.0);
    set(P_EQ1, 0.0);
    set(P_EQ2, 0.0);
    set(P_EQ3, 0.0);
    set(P_EQ4, 0.0);
    set(P_EQ5, 0.0);
    set(P_EQ6, 0.0);

    set(P_FREQ, OFF);
    set(P_FREQ_TYPE, P_FREQ_TYPE_FREQ);
    set(P_FREQ_VAL, 0);

    set(P_FX0, OFF);
    set(P_FX0_TARGET, P_FX_TARGET_MASTER);
    set(P_FX0_TYPE, OFF);
    set(P_FX0_P1, 0);
    set(P_FX0_P2, 0);
    set(P_FX1, OFF);
    set(P_FX1_TARGET, P_FX_TARGET_MASTER);
    set(P_FX1_TYPE, OFF);
    set(P_FX1_P1, 0);
    set(P_FX1_P2, 0);
    set(P_FX2, OFF);
    set(P_FX2_TARGET, P_FX_TARGET_MASTER);
    set(P_FX2_TYPE, OFF);
    set(P_FX2_P1, 0);
    set(P_FX2_P2, 0);
}