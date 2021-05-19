#pragma once

#include <functional>

namespace lr2skin::button
{

std::function<void(int)> getButtonCallback(int type);

}