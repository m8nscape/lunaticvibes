#include "keymap.h"

std::string KeyMap::toString() const
{
    switch (type)
    {
    case DeviceType::KEYBOARD:   return toStringKeyboard();
    case DeviceType::JOYSTICK:   return toStringJoystick();
    case DeviceType::CONTROLLER: return toStringController();
    case DeviceType::MOUSE:      return toStringMouse();
    case DeviceType::RAWINPUT:   return toStringRawInput();
    default: break;
    }
    return "";
}


bool KeyMap::isAxis() const { assert(type == DeviceType::JOYSTICK || type == DeviceType::RAWINPUT); return !!(code & 0x40000000); }
unsigned KeyMap::getAxis() const { return isAxis() ? (code & ~0x70000000) : 0; }
KeyMap::AxisDir KeyMap::getAxisDir() const { return isAxis() ? ((code & 0x20000000) ? 1 : -1) : 0; }

void KeyMap::setKeyboard(Input::Keyboard kb)
{
    type = DeviceType::KEYBOARD;
    device = 0;
    keyboard = kb;
}

void KeyMap::setRawInputKey(int deviceID, int code)
{
    type = DeviceType::RAWINPUT;
    device = deviceID;
    this->code = code;
}


void KeyMap::setRawInputAxis(int deviceID, int idx, KeyMap::AxisDir direction)
{
    type = DeviceType::RAWINPUT;
    device = deviceID;
    this->code = idx & 0x7fffffff;
    this->code |= 0x40000000;
    if (direction > 0) this->code |= 0x20000000;
}

void KeyMap::fromString(const std::string_view& name)
{
    type = DeviceType::UNDEF;
    if (name.empty()) return;

    switch (name[0])
    {
    case 'K': fromStringKeyboard(name); break;
    case 'J': fromStringJoystick(name); break;
    case 'C': fromStringController(name); break;
    case 'M': fromStringMouse(name); break;
    case 'R': fromStringRawInput(name); break;
    default: break;
    }
}

void KeyMap::fromStringKeyboard(const std::string_view& name)
{
    if (name.substr(0, 2) != "K_") return;
    auto keystr = name.substr(2);
    for (size_t i = 0; i < 0xFF; ++i)
    {
        if (Input::keyboardNameMap[i] != NULL && keystr == Input::keyboardNameMap[i])
        {
            setKeyboard(static_cast<Input::Keyboard>(i));
            return;
        }
    }
    setKeyboard(Input::Keyboard::K_ERROR);
}

void KeyMap::fromStringJoystick(const std::string_view& name)
{
    if (name.substr(0, 2) != "J_") return;
    type = DeviceType::JOYSTICK;
    assert(false);
}

void KeyMap::fromStringController(const std::string_view& name)
{
    if (name.substr(0, 2) != "C_") return;
    type = DeviceType::CONTROLLER;
    assert(false);
}

void KeyMap::fromStringMouse(const std::string_view& name)
{
    if (name.substr(0, 2) != "M_") return;
    type = DeviceType::MOUSE;
    assert(false);
}

void KeyMap::fromStringRawInput(const std::string_view& name)
{
    if (name.substr(0, 2) != "R_") return;
    std::string_view val = name.substr(2);
    size_t sep = val.find('_');
    if (sep == val.npos) return;
    if (sep == val.length() - 1) return;
    
    type = DeviceType::RAWINPUT;
    device = toInt(val.substr(0, sep), -1);
    code = toInt(val.substr(sep + 1), 0);
}

std::string KeyMap::toStringKeyboard() const
{
    return "K_"s + Input::keyboardNameMap[static_cast<size_t>(keyboard)];
}

std::string KeyMap::toStringJoystick() const
{
    assert(false);
    return "J_"s + "NULL";
}

std::string KeyMap::toStringController() const
{
    assert(false);
    return "C_"s + "NULL";
}

std::string KeyMap::toStringMouse() const
{
    assert(false);
    return "M_"s + "NULL";
}

std::string KeyMap::toStringRawInput() const
{
    return "R_"s + std::to_string(device) + "_" + std::to_string(code);
}