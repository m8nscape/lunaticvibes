#include "pch.h"
#include "keymap.h"

std::string KeyMap::toString() const
{
    switch (type)
    {
    case DeviceType::UNDEF:    return "-";
    case DeviceType::KEYBOARD: return toStringK();
    case DeviceType::JOYSTICK: return toStringJ();
    case DeviceType::MOUSE:    return toStringM();
    default: break;
    }
    return "";
}

void KeyMap::setKeyboard(Input::Keyboard kb)
{
    type = DeviceType::KEYBOARD;
    keyboard = kb;
}

void KeyMap::setJoystick(size_t device, Input::Joystick::Type jtype, size_t index)
{
    type = DeviceType::JOYSTICK;
    joystick.type = jtype;
    joystick.device = device;
    joystick.index = index;
}

void KeyMap::loadFromString(const std::string_view& name)
{
    type = DeviceType::UNDEF;
    if (name.empty()) return;

    switch (name[0])
    {
    case 'K': loadFromStringK(name); break;
    case 'J': loadFromStringJ(name); break;
    case 'M': loadFromStringM(name); break;
    default: break;
    }
}

void KeyMap::loadFromStringK(const std::string_view& name)
{
    if (name.length() < 3) return;
    if (name.substr(0, 2) != "K_") return;
    type = DeviceType::KEYBOARD;

    auto keystr = name.substr(2);
    for (size_t i = 0; i < 0xFF; ++i)
    {
        if (Input::keyboardNameMap[i] != NULL && keystr == Input::keyboardNameMap[i])
        {
            keyboard = static_cast<Input::Keyboard>(i);
            return;
        }
    }

    keyboard = Input::Keyboard::K_ERROR;
}

void KeyMap::loadFromStringJ(const std::string_view& name)
{
    if (name.length() < 8) return;
    if (name[0] != 'J' || name[2] != '_') return;
    type = DeviceType::JOYSTICK;
    joystick.type = Input::Joystick::Type::UNDEF;

    std::string_view subType = name.substr(3, 4);
    if (subType == "BTN_")
    {
        joystick.type = Input::Joystick::Type::BUTTON;
        joystick.index = toInt(name.substr(7)) - 1;
    }
    else if (subType == "POV_" && name.length() > 10)
    {
        int povIndex = toInt(name.substr(7, 1)) - 1;
        size_t idxMask = 0;
        switch (name[9])
        {
        case 'L': idxMask = (1ul << 31); break;
        case 'D': idxMask = (1ul << 30); break;
        case 'U': idxMask = (1ul << 29); break;
        case 'R': idxMask = (1ul << 28); break;
        }
        if (idxMask != 0)
        {
            joystick.type = Input::Joystick::Type::POV;
            joystick.index = idxMask | povIndex;
        }
    }
    else if (subType == "REL_" && (name[name.size() - 1] == '+' || name[name.size() - 1] == '-'))
    {
        std::string_view axisName = name.substr(7, name.size() - 1 - 7);
        bool axisPositive = name[name.size() - 1] == '+';
        int axisIndex = -1;
        for (int i = 0; i < (int)Input::JoystickAxis::COUNT; ++i)
        {
            if (axisName == Input::joystickAxisName[i])
            {
                axisIndex = i;
                break;
            }
        }
        if (axisIndex != -1)
        {
            joystick.type = axisPositive ? Input::Joystick::Type::AXIS_RELATIVE_POSITIVE : Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE;
            joystick.index = (size_t)axisIndex;
        }
    }
    else if (subType == "ABS_")
    {
        std::string_view axisName = name.substr(7);
        int axisIndex = -1;
        for (int i = 0; i < (int)Input::JoystickAxis::COUNT; ++i)
        {
            if (axisName == Input::joystickAxisName[i])
            {
                axisIndex = i;
                break;
            }
        }
        if (axisIndex != -1)
        {
            joystick.type = Input::Joystick::Type::AXIS_ABSOLUTE;
            joystick.index = (size_t)axisIndex;
        }
    }

    if (joystick.type != Input::Joystick::Type::UNDEF)
    {
        int device = toInt(name.substr(1, 1));
        if (device >= 1 && device <= 8)
        {
            joystick.device = device - 1;
        }
        else
        {
            joystick.type = Input::Joystick::Type::UNDEF;
        }
    }
    
}

void KeyMap::loadFromStringM(const std::string_view& name)
{
    if (name.substr(0, 2) != "M_") return;
    type = DeviceType::MOUSE;
    assert(false);
}

std::string KeyMap::toStringK() const
{
    return "K_"s + Input::keyboardNameMap[static_cast<size_t>(keyboard)];
}

std::string KeyMap::toStringJ() const
{
    std::stringstream ss;
    ss << "J" << joystick.device + 1 << "_";
    switch (joystick.type)
    {
    case Input::Joystick::Type::BUTTON:        
        ss << "BTN_" << joystick.index + 1;
        break;
    case Input::Joystick::Type::POV:
        if      (joystick.index & (1ul << 31)) ss << "POV_" << (joystick.index & 0xFFFFFFF) + 1 << "_LEFT";
        else if (joystick.index & (1ul << 30)) ss << "POV_" << (joystick.index & 0xFFFFFFF) + 1 << "_DOWN";
        else if (joystick.index & (1ul << 29)) ss << "POV_" << (joystick.index & 0xFFFFFFF) + 1 << "_UP";
        else if (joystick.index & (1ul << 28)) ss << "POV_" << (joystick.index & 0xFFFFFFF) + 1 << "_RIGHT";
        break;
    case Input::Joystick::Type::AXIS_RELATIVE_POSITIVE: 
        ss << "REL_" << Input::joystickAxisName[joystick.index] << "+";
        break;
    case Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE:
        ss << "REL_" << Input::joystickAxisName[joystick.index] << "-";
        break;
    case Input::Joystick::Type::AXIS_ABSOLUTE: 
        ss << "ABS_" << Input::joystickAxisName[joystick.index];
        break;
    }
    return ss.str();
}

std::string KeyMap::toStringM() const
{
    assert(false);
    return "M_"s + "NULL";
}
