#include "input_wrapper.h"
#include <plog/Log.h>
#include <cassert>

InputWrapper::InputWrapper(unsigned rate) : AsyncLooper(std::bind(&InputWrapper::_loop, this), rate)
{
}

void InputWrapper::_loop()
{
    _prev= _curr;
    _curr = InputMgr::detect();
	timestamp t;
    auto p = Pressed();
    auto h = Holding();
    auto r = Released();
    if (p != 0)
        LOG_DEBUG << "[Input] " << p;
    // FIXME lock map
    for (auto& pg : _pCallbackMap)
		pg.second(p, t);
    for (auto& hg : _hCallbackMap)
		hg.second(h, t);
    for (auto& rg : _rCallbackMap)
		rg.second(r, t);
}

bool InputWrapper::_register(unsigned type, const std::string& key, INPUTCALLBACK f)
{
    if (_pCallbackMap.find(key) != _pCallbackMap.end())
        return false;

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

    switch (type)
    {
    case 0: _pCallbackMap.erase(key); break;
    case 1: _hCallbackMap.erase(key); break;
    case 2: _rCallbackMap.erase(key); break;
    }
    return true;
}
