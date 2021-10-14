#pragma once
#include "game/data/data.h"
#include <vector>
#include <variant>

namespace lr2skin
{

eNumber num(int n);
eTimer timer(int n);
eText text(int n);

bool buttonSw(int n, eSwitch& sw);
bool buttonOp(int n, eOption& sw);
}