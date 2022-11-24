#include "input_wrapper.h"
#include "game/runtime/state.h"
#include "game/runtime/generic_info.h"
#include "common/log.h"
#include <cassert>

InputWrapper::InputWrapper(unsigned rate, bool background) : 
    AsyncLooper("Input loop", std::bind(&InputWrapper::_loop, this), rate),
    _background(background)
{
}

InputWrapper::~InputWrapper()
{
    assert(!isRunning());
    {
        std::unique_lock _lock(_inputMutex);
        _pCallbackMap.clear();
        _hCallbackMap.clear();
        _rCallbackMap.clear();
        _aCallbackMap.clear();
        _keyboardCallbackMap.clear();
    }
}

void InputWrapper::setRate(unsigned rate)
{
    AsyncLooper::setRate(rate);
}

void InputWrapper::_loop()
{
    gFrameCount[FRAMECOUNT_IDX_INPUT]++;

    _prev = _curr;
    _curr = InputMgr::detect();
    Time now;

    // detect key / button
    InputMask p{ 0 }, h{ 0 }, r{ 0 };
    auto curr = _curr;
    if (!_background && !IsWindowForeground())
    {
        curr.reset();
    }
    if (mergeInput)
    {
        curr |= (curr >> Input::S2L) & INPUT_MASK_1P;
        curr &= ~INPUT_MASK_2P;
    }
    for (Input::Pad i = Input::S1L; i < Input::KEY_COUNT; ++(int&)i)
    {
        auto& [ms, stat] = _inputBuffer[i];
        if (curr[i] && !stat)
        {
            ms = now.norm();
            stat = true;
            p.set(i);
        }
        else if (!curr[i] && stat)
        {
            if (_releaseBuffer[i] == -1)
            {
                _releaseBuffer[i] = now.norm();
            }
            else if (now.norm() - _releaseBuffer[i] >= release_delay_ms)
            {
                ms = now.norm();
                stat = false;
                _releaseBuffer[i] = -1;
                r.set(i);
            }
        }
        else if (stat)
        {
            h.set(i);
        }
    }

    // detect absolute axis
    scratchAxisPrev[0] = scratchAxisCurr[0];
    scratchAxisPrev[1] = scratchAxisCurr[1];
    InputMgr::getScratchPos(scratchAxisCurr[0], scratchAxisCurr[1]);
    double aDelta[2] = { 0., 0. };
    if (scratchAxisSet)
    {
        aDelta[0] = normalizeLinearGrowth(scratchAxisPrev[0], scratchAxisCurr[0]) * InputMgr::getDeadzone(Input::S1A);
        aDelta[1] = normalizeLinearGrowth(scratchAxisPrev[1], scratchAxisCurr[1]) * InputMgr::getDeadzone(Input::S2A);
    }
    scratchAxisSet = true;

    // mouse pos
    InputMgr::getMousePos(_cursor_x, _cursor_y);

    // key config callbacks
    if (_background || IsWindowForeground())
    {
        if (!_keyboardCallbackMap.empty())
        {
            KeyboardMask mask;
            _kbprev = _kbcurr;
            for (Input::Keyboard k = Input::Keyboard::K_ESC; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
            {
                if (isKeyPressed(k)) mask.set(static_cast<size_t>(k));
            }
            _kbcurr = mask;

            KeyboardMask p;
            for (Input::Keyboard k = Input::Keyboard::K_ESC; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
            {
                size_t ki = static_cast<size_t>(k);
                if (_kbcurr[ki] && !_kbprev[ki]) p.set(ki);
            }
            if (p.any())
            {
                std::shared_lock l(_inputMutex, std::defer_lock);

                for (auto& [cbname, callback] : _keyboardCallbackMap)
                    callback(mask, now);
            }
        }

        if (!_joystickCallbackMap.empty())
        {
            _joyprev = _joycurr;
            for (int device = 0; device < InputMgr::MAX_JOYSTICK_COUNT; ++device)
            {
                JoystickMask mask;

                Input::Joystick j;
                j.device = device;
                size_t base = 0;

                j.type = Input::Joystick::Type::BUTTON;
                for (j.index = 0; j.index < InputMgr::MAX_JOYSTICK_BUTTON_COUNT; ++j.index)
                {
                    if (isButtonPressed(j)) mask.set(base + j.index);
                }
                base += InputMgr::MAX_JOYSTICK_BUTTON_COUNT;

                j.type = Input::Joystick::Type::POV;
                for (size_t idxPOV = 0; idxPOV < InputMgr::MAX_JOYSTICK_POV_COUNT; ++idxPOV)
                {
                    j.index = idxPOV | (1ul << 31);
                    if (isButtonPressed(j)) mask.set(base + idxPOV * 4 + 0);
                    j.index = idxPOV | (1ul << 30);
                    if (isButtonPressed(j)) mask.set(base + idxPOV * 4 + 1);
                    j.index = idxPOV | (1ul << 29);
                    if (isButtonPressed(j)) mask.set(base + idxPOV * 4 + 2);
                    j.index = idxPOV | (1ul << 28);
                    if (isButtonPressed(j)) mask.set(base + idxPOV * 4 + 3);
                }
                base += InputMgr::MAX_JOYSTICK_POV_COUNT * 4;

                j.type = Input::Joystick::Type::AXIS_RELATIVE_POSITIVE;
                for (j.index = 0; j.index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++j.index)
                {
                    if (isButtonPressed(j, 0.7) && !_joyprev[device][base + j.index])
                        mask.set(base + j.index);
                }
                base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

                j.type = Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE;
                for (j.index = 0; j.index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++j.index)
                {
                    if (isButtonPressed(j, 0.7) && !_joyprev[device][base + j.index])
                        mask.set(base + j.index);
                }
                base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

                _joycurr[device] = mask;

                JoystickMask p;
                for (size_t k = 0; k < MAX_JOYSTICK_MASK_BIT_COUNT; ++k)
                {
                    size_t ki = static_cast<size_t>(k);
                    if (_joycurr[device][ki] && !_joyprev[device][ki]) p.set(ki);
                }
                if (p.any())
                {
                    std::shared_lock l(_inputMutex, std::defer_lock);

                    for (auto& [cbname, callback] : _joystickCallbackMap)
                        callback(mask, device, now);
                }
            }
        }
    
        if (!_absaxisCallbackMap.empty())
        {
            _joyaxisprev = _joyaxiscurr;
            for (int device = 0; device < InputMgr::MAX_JOYSTICK_COUNT; ++device)
            {
                JoystickAxis mask;
                mask.fill(-1.0);
                bool moved = false;
                for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
                {
                    double axis = getJoystickAxis(device, Input::Joystick::Type::AXIS_ABSOLUTE, index);
                    if (axis != -1.0)
                    {
                        double delta = normalizeLinearGrowth(_joyaxisprev[device][index], axis);
                        if (std::abs(delta) > 0.05)
                        {
                            moved = true;
                            _joyaxiscurr[device][index] = axis;
                            mask[index] = axis;
                        }
                    }
                }
                if (moved)
                {
                    std::shared_lock l(_inputMutex, std::defer_lock);

                    for (auto& [cbname, callback] : _absaxisCallbackMap)
                        callback(mask, device, now);
                }
            }
        }
    }

    // regular callbacks
    {
        std::shared_lock l(_inputMutex, std::defer_lock);
        if (l.try_lock())
        {
            if (p != 0)
                for (auto& [cbname, callback] : _pCallbackMap)
                    callback(p, now);
            if (h != 0)
                for (auto& [cbname, callback] : _hCallbackMap)
                    callback(h, now);
            if (r != 0)
                for (auto& [cbname, callback] : _rCallbackMap)
                    callback(r, now);
            
            if (aDelta[0] != 0.0 || aDelta[1] != 0.0)
                for (auto& [cbname, callback] : _aCallbackMap)
                {
                    if (mergeInput)
                        callback(aDelta[0] + aDelta[1], 0.0, now);
                    else
                        callback(aDelta[0], aDelta[1], now);
                }
        }
    }
}

double InputWrapper::getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index)
{
    return ::getJoystickAxis(device, type, index);
}

double InputWrapper::getScratchAxis(int player)
{
    assert(player == 0 || player == 1);
    return scratchAxisCurr[player];
}

bool InputWrapper::_register(unsigned type, const std::string& key, INPUTCALLBACK f)
{
    if (_pCallbackMap.find(key) != _pCallbackMap.end())
        return false;

	std::unique_lock _lock(_inputMutex);

    switch (type)
    {
    case 0: _pCallbackMap[key] = f; break;
    case 1: _hCallbackMap[key] = f; break;
    case 2: _rCallbackMap[key] = f; break;
    }
    return true;
}

bool InputWrapper::_unregister(unsigned type, const std::string& key)
{
    if (_pCallbackMap.find(key) == _pCallbackMap.end())
        return false;

	std::unique_lock _lock(_inputMutex);

    switch (type)
    {
    case 0: _pCallbackMap.erase(key); break;
    case 1: _hCallbackMap.erase(key); break;
    case 2: _rCallbackMap.erase(key); break;
    }

    return true;
}


bool InputWrapper::register_a(const std::string& key, AXISPLUSCALLBACK f)
{
    if (_aCallbackMap.find(key) != _aCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _aCallbackMap[key] = f;
    return true;
}

bool InputWrapper::unregister_a(const std::string& key)
{
    if (_aCallbackMap.find(key) == _aCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _aCallbackMap.erase(key);
    return true;
}

bool InputWrapper::register_kb(const std::string& key, KEYBOARDCALLBACK f)
{
    if (_keyboardCallbackMap.find(key) != _keyboardCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _keyboardCallbackMap[key] = f;
    return true;
}
bool InputWrapper::unregister_kb(const std::string& key)
{
    if (_keyboardCallbackMap.find(key) == _keyboardCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _keyboardCallbackMap.erase(key);
    return true;
}

bool InputWrapper::register_joy(const std::string& key, JOYSTICKCALLBACK f)
{
    if (_joystickCallbackMap.find(key) != _joystickCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _joystickCallbackMap[key] = f;
    return true;
}
bool InputWrapper::unregister_joy(const std::string& key)
{
    if (_joystickCallbackMap.find(key) == _joystickCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _joystickCallbackMap.erase(key);
    return true;
}

bool InputWrapper::register_aa(const std::string& key, ABSAXISCALLBACK f)
{
    if (_absaxisCallbackMap.find(key) != _absaxisCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _absaxisCallbackMap[key] = f;
    return true;
}
bool InputWrapper::unregister_aa(const std::string& key)
{
    if (_absaxisCallbackMap.find(key) == _absaxisCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _absaxisCallbackMap.erase(key);
    return true;
}
