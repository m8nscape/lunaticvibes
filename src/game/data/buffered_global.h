#pragma once
#include <array>

template <class _Key, class _Value, size_t _size>
class buffered_global
{
protected:
	constexpr buffered_global() : _data{ 0 }, _dataBuffer{ 0 } { static_assert(_size > 0); }
private:
    static buffered_global _inst;
    std::array<_Value, _size> _data;
    std::array<_Value, _size> _dataBuffer;
public:
    static constexpr _Value get(_Key n) { return ((size_t)n < _size) ? _inst._data[(size_t)n] : (_Value)0; }
    static constexpr void queue(_Key n, _Value value) { if ((size_t)n < _size) _inst._dataBuffer[(size_t)n] = value; }
    static constexpr void flush() { _inst._data = _inst._dataBuffer; }
    static constexpr void reset() { for (size_t i = 1; i < _size; ++i) set((_Key)i, _inst._data[0]); }
//protected:
    static constexpr void set(_Key n, _Value value) { if ((size_t)n < _size) _inst._data[(size_t)n] = _inst._dataBuffer[(size_t)n] = value; }
};

