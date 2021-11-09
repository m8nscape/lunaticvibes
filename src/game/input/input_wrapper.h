#pragma once
#include <shared_mutex>
#include <array>
#include <queue>
#include <set>
#include "input_mgr.h"
#include "common/asynclooper.h"
#include "common/beat.h"
#include "input_rawinput.h"

typedef std::bitset<Input::Pad::KEY_COUNT> InputMask;
typedef std::function<void(InputMask&, const Time&)> INPUTCALLBACK;
//typedef void(*PressedHandleCallback)(void* owner, InputMask&);
//typedef void(*HoldHandleCallback)(void* owner, InputMask&);
//typedef void(*ReleasedHandleCallback)(void* owner, InputMask&);

typedef std::bitset<Input::keyboardKeyCount> KeyboardMask;
typedef std::function<void(KeyboardMask, const Time&)> KEYBOARDCALLBACK;

typedef std::map<Input::Pad, std::pair<double, int>> InputAxisPlus;
typedef std::function<void(InputAxisPlus&, const Time&)> AXISPLUSCALLBACK;

typedef std::map<int, bool> RawinputKeyMap;
typedef std::map<int, std::pair<double, int>> RawinputAxisSpeedMap;
typedef std::function<void(int, RawinputKeyMap&, RawinputAxisSpeedMap&, const Time&)> RAWINPUTCALLBACK;

// FUNC:                                   BRDUEHDI><v^543210987654321_
inline const InputMask INPUT_MASK_FUNC  { "1111111111111111111111111111000000000000000000000000000000" };
// 1P:                                                                                DUEA987654321SS
inline const InputMask INPUT_MASK_1P    { "0000000000000000000000000000000000000000000111111111111111" };
// 2P:                                                                 DUEA987654321SS
inline const InputMask INPUT_MASK_2P    { "0000000000000000000000000000111111111111111000000000000000" };

//                                                                     2P: 9 7 5 3 1  1P: 9 7 5 3 1  
inline const InputMask INPUT_MASK_DECIDE{ "0100000000000000000000000000000010101010100000010101010100" };
//                                                                     2P:  8 6 4 2   1P:  8 6 4 2
inline const InputMask INPUT_MASK_CANCEL{ "1000000000000000000000000000000001010101000000001010101000" };
//                                                  < ^                2P:           S1P:           S
inline const InputMask INPUT_MASK_NAV_UP{ "0000000001010000000000000000000000000000001000000000000001" };
//                                                 > v                 2P:          S 1P:          S
inline const InputMask INPUT_MASK_NAV_DN{ "0000000010100000000000000000000000000000010000000000000010" };

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

    InputMask _prev = 0;
    InputMask _curr = 0;

public:
    InputWrapper(unsigned rate = 1000, bool background = false);
    virtual ~InputWrapper();

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

    InputMask Pressed() { return ~_prev & _curr; }
    InputMask Holding() { return _prev & _curr; }
    InputMask Released() { return _prev & ~_curr; }

    std::pair<int, int> getCursorPos() { return { _cursor_x, _cursor_y }; }

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

    // Should only used for keyconfig
#ifdef RAWINPUT_AVAILABLE
protected:
    std::map<int, RawinputKeyMap> _riprev;
    std::map<int, RawinputKeyMap> _ricurr;
private:
    std::map<const std::string, RAWINPUTCALLBACK> _rawinputCallbackMap;
public:
    bool register_ri(const std::string& key, RAWINPUTCALLBACK f);
    bool unregister_ri(const std::string& key);
#endif
};

