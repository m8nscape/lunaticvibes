#pragma once
#include "game/runtime/state.h"

namespace lr2skin
{

IndexNumber num(int n);
IndexTimer timer(int n);
IndexText text(int n);

bool buttonSw(int n, IndexSwitch& sw);
bool buttonOp(int n, IndexOption& sw);
bool buttonFixed(int n, unsigned& out);
}