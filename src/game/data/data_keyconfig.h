#pragma once

namespace lv
{

inline struct Struct_KeyConfigData
{
    GameModeKeys currentMode;
    bool modeChanged = false;

    std::pair<Input::Pad, int> selecting = { Input::Pad::K11, 0 };
} KeyConfigData;

}