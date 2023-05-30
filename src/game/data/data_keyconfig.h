#pragma once
#include "data_internal.h"
#include "common/keymap.h"

namespace lunaticvibes
{

inline struct Struct_KeyConfigData
{
    TimerStorage timers;

    GameModeKeys currentMode;
    bool modeChanged = false;
    bool skinHasAbsAxis = false;

    std::pair<Input::Pad, int> selecting = { Input::Pad::K11, 0 };

    std::map<Input::Pad, std::string> bindName;
    std::map<Input::Pad, double> force;
    std::map<Input::Pad, double> deadzone;
    double scratchAxisSpeed[2] = {0.0, 0.0};
    std::string scratchAxisValueText[2];
} KeyConfigData;

}