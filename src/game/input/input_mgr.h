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
    static constexpr std::size_t MAX_JOYSTICK_BUTTON_COUNT = 32;
    static constexpr std::size_t MAX_JOYSTICK_POV_COUNT = 4;    // 4 directions each pov
    static constexpr std::size_t MAX_JOYSTICK_AXIS_COUNT = 8;       // 2 directions each axis
    enum class eAxisMode { AXIS_NORMAL, AXIS_ABSOLUTE };

    // Game keys param / functions
private:
    std::bitset<MAX_JOYSTICK_COUNT> joysticksConnected{};
    std::array<KeyMap, Input::ESC> padBindings{};
    std::array<double, Input::ESC> padDeadzones{};
    double scratch1, scratch2;

    int debounceTime = 0;
    std::array<Time, Input::Pad::KEY_COUNT> pressedTime;

public:
    // Game keys param / functions
    static void init();
    static void updateDevices();
    static void updateBindings(GameModeKeys keys);
    static void updateBindings(GameModeKeys keys, Input::Pad K);
    static void updateDeadzones();
    static double getDeadzone(Input::Pad k);

    std::bitset<Input::KEY_COUNT> _detect();
    static std::bitset<Input::KEY_COUNT> detect();
	static bool getMousePos(int& x, int& y);
    static bool getScratchPos(double& s1, double& s2);

    static void setDebounceTime(int ms);

};

////////////////////////////////////////////////////////////////////////////////
// System specific range

void initInput();

void refreshInputDevices();

void pollInput();

// Keyboard detect
bool isKeyPressed(Input::Keyboard c);

// Joystick detect
bool isButtonPressed(Input::Joystick c, double deadzone = 0.0);
double getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index);

// Mouse detect
bool isMouseButtonPressed(int idx);

short getLastMouseWheelState();

// absolute axis

