#pragma once
#include "data_internal.h"

namespace lunaticvibes
{

inline struct Struct_ResultData
{
    TimerStorage timers;

    bool cleared = false;
    bool flipResult = false;

    bool playerCleared[2] = { false, false };

} ResultData;

}