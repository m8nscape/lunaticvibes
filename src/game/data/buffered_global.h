#pragma once
#include <array>
#include <plog/Log.h>

template <class _Key, class _Value, size_t _size>
class buffered_global
{
public:
	constexpr buffered_global() : _data{ 0 }, _dataBuffer{ 0 } { static_assert(_size > 0); }
private:
    std::array<_Value, _size> _data;
    std::array<_Value, _size> _dataBuffer;
public:
    constexpr _Value get(_Key n) { return ((size_t)n < _size) ? _data[(size_t)n] : (_Value)0; }
    inline void queue(_Key n, _Value value) { if ((size_t)n < _size) _dataBuffer[(size_t)n] = value; }
    inline void flush() { _data = _dataBuffer; }
    inline void reset() { for (size_t i = 1; i < _size; ++i) set((_Key)i, _data[0]); }
//protected:
    constexpr void set(_Key n, _Value value)
    {
        LOG_DEBUG << "[Data] Set " << (int)n << " To " << (int)value;
        if ((size_t)n < _size)
            _data[(size_t)n] = _dataBuffer[(size_t)n] = value;
    }
};

