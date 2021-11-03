#pragma once
#include <vector>
#include <array>
#include <bitset>
#include <map>
#include <functional>
#include "input_keys.h"

namespace Input
{
    enum DeviceType
    {
        KEYBOARD = 0,
        JOYSTICK,
        CONTROLLER,
        MOUSE,          // ???
    };

    typedef size_t DeviceID;
    struct KeyMap
    {
        DeviceType  type;
        DeviceID    device;
        Keyboard    key;
    };

    typedef std::array<KeyMap, MAX_BINDINGS_PER_KEY> Bindings;

};

////////////////////////////////////////////////////////////////////////////////
// Input manager
// fetch real-time system keyscan status
// Do not use this class directly. Use InputWrapper instead.
class InputMgr
{
private:
    static InputMgr _inst;
    InputMgr() = default;
    ~InputMgr() = default;

public:

    // Game keys param / functions
private:
    std::bitset<Input::MAX_JOYSTICK_COUNT> joysticksConnected{};
    bool haveJoystick = false;
    std::array<Input::Bindings, Input::ESC> padBindings{};
	int mouse_x = 0, mouse_y = 0;
	int analogDeadZone = 25;

public:
    // Game keys param / functions
    static void updateDevices();
    static void updateBindings(GameModeKeys keys, Input::Pad K);
    static std::bitset<Input::KEY_COUNT> detect();
	static bool getMousePos(int& x, int& y);
};

////////////////////////////////////////////////////////////////////////////////
// System specific range

// Keyboard detect
bool isKeyPressed(Input::Keyboard c);

// Joystick detect
typedef int JoyBtn;
//bool isButtonPressed(Device d, Button b);

// Mouse detect
bool isMouseButtonPressed(int idx);