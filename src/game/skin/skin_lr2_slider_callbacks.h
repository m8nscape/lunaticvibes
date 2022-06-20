#pragma once

#include <functional>
#include "common/keymap.h"

namespace lr2skin::slider
{
	std::function<void(double)> getSliderCallback(int type);

	// may be used externally
}