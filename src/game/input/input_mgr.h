#pragma once
#include <vector>
#include <array>
#include <bitset>
#include <map>
#include <functional>
#include "defs/defs_input.h"

typedef std::size_t size_t;

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
    
    struct Button
    {
        DeviceType  type;
        DeviceID    device;
        int		    scancode;
    };

    typedef std::vector<Button> Binding;

};


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
    static const std::size_t MAX_JOYSTICK_COUNT = 8;
    static const std::size_t MAX_BINDINGS_PER_KEY = 10;

    // Game keys param / functions
private:
    std::bitset<MAX_JOYSTICK_COUNT> joysticksConnected;
    std::array<Input::Binding, Input::ESC> bindings;
	bool haveJoystick = false;
	int analogDeadZone = 25;

public:
    // Game keys param / functions
    static void updateDevices();
    static void updateBindings(Input::k K);
    static std::bitset<Input::KEY_COUNT> detect();

};
