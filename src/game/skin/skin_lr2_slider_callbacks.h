#pragma once

#include "common/keymap.h"

namespace lr2skin::slider
{
std::function<void(double)> getSliderCallback(int type);

// may be used externally
void pitch(double p);
}