#include "cfg_profile.h"

ConfigProfile::ConfigProfile() : vConfig(CONFIG_FILE_PROFILE) {}
ConfigProfile::~ConfigProfile() {}

void ConfigProfile::setDefaults() noexcept
{
    using namespace cfg;
    set(P_PLAYERNAME, "Unnamed");
    set(P_BASESPEED, 1.0);
    set(P_HISPEED, 1.0);
    set(P_REGULAR_SPEED, 1200);
    set(P_SPEED_TYPE, P_SPEED_TYPE_NORMAL);
    set(P_LANECOVER, 0);
    set(P_LIFT, 0);
    set(P_CHART_OP, P_CHART_OP_NORMAL);
    set(P_GAUGE_OP, P_GAUGE_OP_NORMAL);
    set(P_COMBO_POS_X, 0);
    set(P_COMBO_POS_Y, 0);
    set(P_JUDGE_POS_X, 0);
    set(P_JUDGE_POS_Y, 0);
    set(P_DISPLAY_LATENCY, 0);
    set(P_JUDGE_LATENCY, 0);
    set(P_JUDGE_STAT, OFF);
    set(P_GHOST_TYPE, OFF);
    set(P_GHOST_POS_X, 0);
    set(P_GHOST_POS_Y, 0);
    set(P_GHOST_TARGET, 50);
    set(P_GHOST_FS_TRIGGER, OFF);
    set(P_GHOST_FS_TYPE, P_GHOST_FS_TYPE_NORMAL);
    set(P_GHOST_FS_POS_X, 0);
    set(P_GHOST_FS_POS_Y, 0);
}