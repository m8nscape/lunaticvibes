#include "common/log.h"
#include <filesystem>
#include <fstream>
#include "cfg_profile.h"

namespace fs = std::filesystem;

std::string ConfigProfile::getName() const
{
    return _yaml[cfg::P_PLAYERNAME].as<std::string>("Unnamed"); 
}
void ConfigProfile::rename(const std::string& name)
{
    using namespace cfg;
    _yaml[P_PLAYERNAME] = name;
}

void ConfigProfile::setDefaults() noexcept
{
	using namespace cfg;
	//_yaml[P_PLAYERNAME] = "Unnamed";
	_yaml[P_BASESPEED] = 1.0;
	_yaml[P_HISPEED] = 1.0;
	_yaml[P_REGULAR_SPEED] = 1200;
	_yaml[P_SPEED_TYPE] = P_SPEED_TYPE_NORMAL;
	_yaml[P_LOAD_BGA] = ON;
    _yaml[P_LANECOVER] = 0;
    _yaml[P_LIFT] = 0;
    _yaml[P_CHART_OP] = P_CHART_OP_NORMAL;
    _yaml[P_GAUGE_OP] = P_GAUGE_OP_NORMAL;
    _yaml[P_COMBO_POS_X] = 0;
    _yaml[P_COMBO_POS_Y] = 0;
    _yaml[P_JUDGE_POS_X] = 0;
    _yaml[P_JUDGE_POS_Y] = 0;
    _yaml[P_DISPLAY_OFFSET] = 0;
    _yaml[P_JUDGE_OFFSET] = 0;
    _yaml[P_JUDGE_STAT] = OFF;
    _yaml[P_GHOST_COLOR] = P_GHOST_COLOR_NORMAL;
    _yaml[P_GHOST_TYPE] = OFF;
    _yaml[P_GHOST_TARGET] = 50;
    _yaml[P_TARGET] = OFF;
    _yaml[P_GHOST_FS_TRIGGER] = OFF;
    _yaml[P_GHOST_FS_TYPE] = P_GHOST_FS_TYPE_NORMAL;
    _yaml[P_GHOST_FS_POS_X] = 0;
    _yaml[P_GHOST_FS_POS_Y] = 0;
    _yaml[P_PLAY_MODE] = P_PLAY_MODE_ALL;
    _yaml[P_SORT_MODE] = P_SORT_MODE_FOLDER;
    _yaml[P_DIFFICULTY_FILTER] = P_DIFFICULTY_FILTER_ALL;
    _yaml[P_BATTLE] = OFF;
    _yaml[P_FLIP] = OFF;
    _yaml[P_SCORE_GRAPH] = OFF;

    _yaml[P_LIST_SCROLL_TIME_INITIAL] = 300;
    _yaml[P_LIST_SCROLL_TIME_HOLD] = 150;

    _yaml[P_VOL_MASTER] = 1.0;
    _yaml[P_VOL_BGM] = 1.0;
    _yaml[P_VOL_KEY] = 1.0;

    _yaml[P_EQ] = OFF;
    _yaml[P_EQ0] = 0.0;
    _yaml[P_EQ1] = 0.0;
    _yaml[P_EQ2] = 0.0;
    _yaml[P_EQ3] = 0.0;
    _yaml[P_EQ4] = 0.0;
    _yaml[P_EQ5] = 0.0;
    _yaml[P_EQ6] = 0.0;

    _yaml[P_FREQ] = OFF;
    _yaml[P_FREQ_TYPE] = P_FREQ_TYPE_FREQ;
    _yaml[P_FREQ_VAL] = 0;

    _yaml[P_FX0] = OFF;
    _yaml[P_FX0_TARGET] = P_FX_TARGET_MASTER;
    _yaml[P_FX0_TYPE] = OFF;
    _yaml[P_FX0_P1] = 0;
    _yaml[P_FX0_P2] = 0;
    _yaml[P_FX1] = OFF;
    _yaml[P_FX1_TARGET] = P_FX_TARGET_MASTER;
    _yaml[P_FX1_TYPE] = OFF;
    _yaml[P_FX1_P1] = 0;
    _yaml[P_FX1_P2] = 0;
    _yaml[P_FX2] = OFF;
    _yaml[P_FX2_TARGET] = P_FX_TARGET_MASTER;
    _yaml[P_FX2_TYPE] = OFF;
    _yaml[P_FX2_P1] = 0;
    _yaml[P_FX2_P2] = 0;
}