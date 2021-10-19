#pragma once

#include <functional>
#include "game/input/input_keys.h"

namespace lr2skin::button
{

std::function<void(int)> getButtonCallback(int type);

// may be used externally
void gauge_type(int player, int plus);
void random_type(int player, int plus);
void autoscr(int player, int plus);
void flip(int plus);
void hs_fix(int plus);
void battle(int plus);
void hs(int player, int plus);
void key_config_pad(Input::Pad pad);
void key_config_slot(int slot);
void key_config_mode_rotate();
}