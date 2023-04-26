#pragma once

#include "common/keymap.h"

namespace lr2skin::button
{

std::function<void(int)> getButtonCallback(int type);

// may be used externally
void select_difficulty_filter(int plus, int iterateCount = 0);
void select_keys_filter(int plus, int iterateCount = 0);
void pitch_switch(int plus);
void gauge_type(int player, int plus);
void random_type(int player, int plus);
void autoscr(int player, int plus);
void lane_effect(int player, int plus);
void flip(int plus);
void hs_fix(int plus);
void battle(int plus);
void hs(int player, int plus);
void lock_speed_value(int player, int plus);
void enter_key_config();
void enter_skin_config();
void target_type(int plus);
void key_config_pad(Input::Pad pad, bool forceUpdate = false);
void key_config_slot(int slot);
void key_config_mode_rotate();
void skinselect_mode(int mode);
void skinselect_skin(int plus);
void skinselect_option(int index, int plus);
}