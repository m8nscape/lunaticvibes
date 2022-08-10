#include "input_wrapper.h"
#include "game/data/number.h"
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

void InputWrapper::_loop()
{
	gNumbers.set(eNumber::INPUT_DETECT_FPS, getRateRealtime());
    _prev = _curr;
    _curr = InputMgr::detect();
    Time now;

    InputMask p{ 0 }, h{ 0 }, r{ 0 };

    auto d = _curr;
    if (!_background && !IsWindowForeground()) d.reset();

    for (Input::Pad i = Input::S1L; i < Input::KEY_COUNT; ++(int&)i)
    {
        auto& [ms, stat] = _inputBuffer[i];
        if (d[i] && !stat)
        {
            ms = now.norm();
            stat = true;
            p.set(i);
        }
        else if (!d[i] && stat)
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
                    if (isButtonPressed(j, 0.7)) mask.set(base + j.index);
                }
                base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

                j.type = Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE;
                for (j.index = 0; j.index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++j.index)
                {
                    if (isButtonPressed(j, 0.7)) mask.set(base + j.index);
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
    }

    InputMgr::getMousePos(_cursor_x, _cursor_y);

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
            
            //auto a = InputMgr::detectAbsoluteAxis();
            //if (!a.empty())
                //for (auto& [cbname, callback] : _aCallbackMap)
                //    callback(a, now);
        }
    }
}

double InputWrapper::getJoystickAxis(size_t device, Input::Joystick::Type type, size_t index)
{
    return ::getJoystickAxis(device, type, index);
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
