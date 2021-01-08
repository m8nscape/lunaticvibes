#pragma once
#include <mutex>
#include "input_mgr.h"
#include "asynclooper.h"
#include "beat.h"

typedef std::bitset<Input::KEY_COUNT> InputMask;
typedef std::function<void(InputMask&, Time)> INPUTCALLBACK;
//typedef void(*PressedHandleCallback)(void* owner, InputMask&);
//typedef void(*HoldHandleCallback)(void* owner, InputMask&);
//typedef void(*ReleasedHandleCallback)(void* owner, InputMask&);

// FUNC:                                   BRDUEHDIRLDU543210987654321_
inline const InputMask INPUT_MASK_FUNC  { "1111111111111111111111111111000000000000000000000000000000" };
// 1P:                                                                                DUEA987654321SS
inline const InputMask INPUT_MASK_1P    { "0000000000000000000000000000000000000000000111111111111111" };
// 2P:                                                                 DUEA987654321SS
inline const InputMask INPUT_MASK_2P    { "0000000000000000000000000000111111111111111000000000000000" };

//                                                                     2P: 9 7 5 3 1  1P: 9 7 5 3 1  
inline const InputMask INPUT_MASK_DECIDE{ "0000000000000000000000000000000010101010100000010101010100" };
//                                                                     2P:  8 6 4 2   1P:  8 6 4 2
inline const InputMask INPUT_MASK_CANCEL{ "0000000000000000000000000000000001010101000000001010101000" };

// InputWrapper
//  Start a process to check input by 1000hz rolling.
// Interface: 
//  Pressed / Holding / Released FULL bitset
//  Pressed / Holding / Released per key
class InputWrapper: public AsyncLooper
{
private:
	std::mutex _mutex;
    bool _busy;

protected:
    InputMask _prev = 0;
    InputMask _curr = 0;

public:
    InputWrapper(unsigned rate = 1000);
    virtual ~InputWrapper() { loopEnd(); }

private:
    virtual void _loop();
    
public:
    bool isPressed(Input::Ingame k) 
    {
        return (!_prev[k]) && (_curr[k]);
    }

    bool isReleased(Input::Ingame k) 
    {
        return (_prev[k]) && (!_curr[k]);
    }

    bool isHolding(Input::Ingame k) 
    {
        return (_prev[k]) && (_curr[k]);
    }

    InputMask Pressed() { return ~_prev & _curr; }
    InputMask Holding() { return _prev & _curr; }
    InputMask Released() { return _prev & ~_curr; }

private:
    // Callback function maps
    std::map<const std::string, INPUTCALLBACK> _pCallbackMap;
    std::map<const std::string, INPUTCALLBACK> _hCallbackMap;
    std::map<const std::string, INPUTCALLBACK> _rCallbackMap;
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
};

