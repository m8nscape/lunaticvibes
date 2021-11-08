#pragma once
#include <vector>
#include <array>
#include <bitset>
#include <map>
#include <functional>
#include "common/keymap.h"

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
    static constexpr std::size_t MAX_JOYSTICK_COUNT = 8;
    static constexpr std::size_t MAX_BINDINGS_PER_KEY = 10;
    enum class eAxisMode { AXIS_NORMAL, AXIS_RELATIVE };

    // Game keys param / functions
private:
    std::bitset<MAX_JOYSTICK_COUNT> joysticksConnected{};
    bool haveJoystick = false;
    std::array<std::array<KeyMap, MAX_BINDINGS_PER_KEY>, Input::ESC> padBindings{};
	int mouse_x = 0, mouse_y = 0;
	int analogDeadZone = 25;

    eAxisMode axisMode = eAxisMode::AXIS_NORMAL;

public:
    // Game keys param / functions
    static void init();
    static void updateDevices();
    static void updateBindings(GameModeKeys keys, Input::Pad K);
    static std::bitset<Input::KEY_COUNT> detect();
    static std::map<Input::Pad, int> detectRelativeAxis();
	static bool getMousePos(int& x, int& y);
    static int getDeadzone() { return _inst.analogDeadZone; }
    static void setAxisMode(eAxisMode mode) { _inst.axisMode = mode; }
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
