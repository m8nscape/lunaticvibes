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
        Key		    key;
    };

    typedef std::array<KeyMap, MAX_BINDINGS_PER_KEY> Binding;

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
    std::array<Input::Binding, Input::ESC> bindings{};
	bool haveJoystick = false;
	int mouse_x = 0, mouse_y = 0;
	int analogDeadZone = 25;

public:
    // Game keys param / functions
    static void updateDevices();
    static void updateBindings(unsigned keys, Input::Ingame K);
    static std::bitset<Input::KEY_COUNT> detect();
	friend int getMousePos(int& x, int& y);
};

////////////////////////////////////////////////////////////////////////////////
// System specific range

extern const std::array<std::string, 0xFF> keyNameMap;

// Keyboard detect
bool isKeyPressed(Input::Key c);

// Joystick detect
typedef int JoyBtn;
//bool isButtonPressed(Device d, Button b);

// Mouse detect
int getMousePos(int& x, int& y);