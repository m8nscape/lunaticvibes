#pragma once

#include <functional>

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

}