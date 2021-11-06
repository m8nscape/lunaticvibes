#include "input_wrapper.h"
#include "game/data/number.h"
#include "common/log.h"
#include <cassert>

InputWrapper::InputWrapper(unsigned rate, bool background) : 
    AsyncLooper(std::bind(&InputWrapper::_loop, this), rate), _background(background)
{
}

InputWrapper::~InputWrapper()
{
    loopEnd();

    {
        std::unique_lock _lock(_inputMutex);
        _pCallbackMap.clear();
        _hCallbackMap.clear();
        _rCallbackMap.clear();
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

#ifdef RAWINPUT_AVAILABLE
        if (!_rawinputCallbackMap.empty())
        {
            for (int deviceID = 0; deviceID < (int)Input::rawinput::RIMgr::inst().getDeviceCount(); ++deviceID)
            {
                RawinputKeyMap pressedNew;
                auto axisdelta = Input::rawinput::RIMgr::inst().getAxisDelta(deviceID);

                RawinputKeyMap pressedTmp = Input::rawinput::RIMgr::inst().getPressed(deviceID);
                for (auto& [key, stat] : pressedTmp)
                    if (stat && !_riprev[deviceID][key])
                        pressedNew[key] = true;

                if (!pressedNew.empty())
                {
                    for (auto& [cbname, callback] : _rawinputCallbackMap)
                        callback(deviceID, pressedNew, axisdelta, now);
                }
                _riprev[deviceID] = pressedTmp;
            }
        }
#endif
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
        }
    }
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

#ifdef RAWINPUT_AVAILABLE
bool InputWrapper::register_ri(const std::string& key, RAWINPUTCALLBACK f)
{
    if (_rawinputCallbackMap.find(key) != _rawinputCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _rawinputCallbackMap[key] = f;
    return true;
}
bool InputWrapper::unregister_ri(const std::string& key)
{
    if (_rawinputCallbackMap.find(key) == _rawinputCallbackMap.end())
        return false;

    std::unique_lock _lock(_inputMutex);
    _rawinputCallbackMap.erase(key);
    return true;
}
#endif