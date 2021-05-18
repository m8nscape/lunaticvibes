#pragma once

#include <functional>

namespace lr2skin::button
{

std::function<void(bool)> getButtonCallback(int type);

}