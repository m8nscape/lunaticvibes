#pragma once
#include <shared_mutex>
#include <array>
#include <queue>
#include <set>
#include "input_mgr.h"
#include "common/asynclooper.h"
#include "common/beat.h"

typedef std::bitset<Input::Pad::KEY_COUNT> InputMask;
typedef std::function<void(InputMask&, const Time&)> INPUTCALLBACK;
//typedef void(*PressedHandleCallback)(void* owner, InputMask&);
//typedef void(*HoldHandleCallback)(void* owner, InputMask&);
//typedef void(*ReleasedHandleCallback)(void* owner, InputMask&);

typedef std::bitset<Input::keyboardKeyCount> KeyboardMask;
typedef std::function<void(KeyboardMask&, const Time&)> KEYBOARDCALLBACK;

typedef std::function<void(double, double, const Time&)> AXISPLUSCALLBACK;

constexpr size_t MAX_JOYSTICK_MASK_BIT_COUNT = InputMgr::MAX_JOYSTICK_BUTTON_COUNT + InputMgr::MAX_JOYSTICK_POV_COUNT * 4 + InputMgr::MAX_JOYSTICK_AXIS_COUNT * 2;
typedef std::bitset<MAX_JOYSTICK_MASK_BIT_COUNT> JoystickMask;
typedef std::function<void(JoystickMask&, size_t, const Time&)> JOYSTICKCALLBACK;

typedef std::array<double, InputMgr::MAX_JOYSTICK_AXIS_COUNT> JoystickAxis;
typedef std::function<void(JoystickAxis&, size_t, const Time&)> ABSAXISCALLBACK;

// FUNC:                                          BRDUEHDI><v^543210987654321_
inline const InputMask INPUT_MASK_FUNC  { "0000000111111111111111111111111111100000000000000000000000000000000" };
// 1P:                                                                                        sDUEA987654321SS
inline const InputMask INPUT_MASK_1P    { "0000000000000000000000000000000000000000000000000001111111111111111" };
// 2P:                                                                        sDUEA987654321SS
inline const InputMask INPUT_MASK_2P    { "0000000000000000000000000000000000011111111111111110000000000000000" };
// Mouse:                                  DU54321
inline const InputMask INPUT_MASK_MOUSE { "1111111000000000000000000000000000000000000000000000000000000000000" };
                                                  
//                                                 Return >                     2P: 9 7 5 3 1  1P: 9 7 5 3 1  
inline const InputMask INPUT_MASK_DECIDE{ "0000000010000001000000000000000000000000010101010100000010101010100" };
//                                                Bksp     <                    2P:  8 6 4 2   1P:  8 6 4 2
inline const InputMask INPUT_MASK_CANCEL{ "0000000100000000100000000000000000000000001010101000000001010101000" };
//                                                           ^                  2P:           S1P:           S
inline const InputMask INPUT_MASK_NAV_UP{ "0000000000000000001000000000000000000000000000000001000000000000001" };
//                                                          v                   2P:          S 1P:          S
inline const InputMask INPUT_MASK_NAV_DN{ "0000000000000000010000000000000000000000000000000010000000000000010" };

//                                                    Return >                     2P:     5      1P:     5      
inline const InputMask INPUT_MASK_DECIDE_9K{ "0000000010000001000000000000000000000000000001000000000000001000000" };
//                                                   Bksp     <                    2P:   7   3    1P:   7   3 
inline const InputMask INPUT_MASK_CANCEL_9K{ "0000000100000000100000000000000000000000000100010000000000100010000" };
//                                                              ^                  2P:      4    S1P:      4    S
inline const InputMask INPUT_MASK_NAV_UP_9K{ "0000000000000000001000000000000000000000000000100001000000000100001" };
//                                                             v                   2P:    6     S 1P:    6     S
inline const InputMask INPUT_MASK_NAV_DN_9K{ "0000000000000000010000000000000000000000000010000010000000010000010" };

// InputWrapper
//  Start a process to check input upon 1000hz polling.
// Interface: 
//  Pressed / Holding / Released FULL bitset
//  Pressed / Holding / Released per key
class InputWrapper: public AsyncLooper
{
public:
    unsigned release_delay_ms = 5;
private:
	std::shared_mutex _inputMutex;

protected:
    std::array<std::pair<long long, bool>, Input::KEY_COUNT> _inputBuffer{ {{0, false}} };
    std::array<long long, Input::KEY_COUNT> _releaseBuffer{ -1 };
    int _cursor_x = 0, _cursor_y = 0;
    bool _background = false;
    bool _countFPS = true;

    InputMask _prev = 0;
    InputMask _curr = 0;

    bool scratchAxisSet = false;
    double scratchAxisPrev[2] = { 0. };
    double scratchAxisCurr[2] = { 0. };

    bool mergeInput = false;

public:
    InputWrapper(unsigned rate = 1000, bool background = false);
    virtual ~InputWrapper();

public:
    void setRate(unsigned rate_per_sec);

private:
    virtual void _loop();

public:
    bool isPressed(Input::Pad k) 
    {
        return (!_prev[k]) && (_curr[k]);
    }

    bool isReleased(Input::Pad k) 
    {
        return (_prev[k]) && (!_curr[k]);
    }

    bool isHolding(Input::Pad k) 
    {
        return (_prev[k]) && (_curr[k]);
    }

    InputMask Pressed() const { return ~_prev & _curr; }
    InputMask Holding() const { return _prev & _curr; }
    InputMask Released() const { return _prev & ~_curr; }

    std::pair<int, int> getCursorPos() const { return { _cursor_x, _cursor_y }; }

    double getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index);

    double getScratchAxis(int player);

    // Merge 2P button inputs into 1P. Note that abs axis are ALSO merged.
    void setMergeInput() { mergeInput = true; }

    void disableCountFPS() { _countFPS = false; }

private:
    // Callback function maps
    std::map<const std::string, INPUTCALLBACK> _pCallbackMap;
    std::map<const std::string, INPUTCALLBACK> _hCallbackMap;
    std::map<const std::string, INPUTCALLBACK> _rCallbackMap;
    std::map<const std::string, AXISPLUSCALLBACK> _aCallbackMap;
private:
    // Callback registering
    bool _register(unsigned type, const std::string& key, INPUTCALLBACK);
    bool _unregister(unsigned type, const std::string& key);
public:
    bool register_p(const std::string& key, INPUTCALLBACK f) { return _register(0, key, f); }
    bool register_h(const std::string& key, INPUTCALLBACK f) { return _register(1, key, f); }
    bool register_r(const std::string& key, INPUTCALLBACK f) { return _register(2, key, f); }
    bool unregister_p(const std::string& key) { return _unregister(0, key); }
    bool unregister_h(const std::string& key) { return _unregister(1, key); }
    bool unregister_r(const std::string& key) { return _unregister(2, key); }
    bool register_a(const std::string& key, AXISPLUSCALLBACK f);
    bool unregister_a(const std::string& key);

    // Should only used for keyconfig
protected:
    KeyboardMask _kbprev = 0;
    KeyboardMask _kbcurr = 0;
private:
    std::map<const std::string, KEYBOARDCALLBACK> _keyboardCallbackMap;
public:
    bool register_kb(const std::string& key, KEYBOARDCALLBACK f);
    bool unregister_kb(const std::string& key);

protected:
    std::array<JoystickMask, InputMgr::MAX_JOYSTICK_COUNT> _joyprev = { 0 };
    std::array<JoystickMask, InputMgr::MAX_JOYSTICK_COUNT> _joycurr = { 0 };
private:
    std::map<const std::string, JOYSTICKCALLBACK> _joystickCallbackMap;
public:
    bool register_joy(const std::string& key, JOYSTICKCALLBACK f);
    bool unregister_joy(const std::string& key);

protected:
    std::array<JoystickAxis, InputMgr::MAX_JOYSTICK_COUNT> _joyaxisprev = { 0 };
    std::array<JoystickAxis, InputMgr::MAX_JOYSTICK_COUNT> _joyaxiscurr = { 0 };
private:
    std::map<const std::string, ABSAXISCALLBACK> _absaxisCallbackMap;
public:
    bool register_aa(const std::string& key, ABSAXISCALLBACK f);
    bool unregister_aa(const std::string& key);
};

