#pragma once
//#include <array>
#include "buffered_global.h"

enum class eNumber: unsigned
{
    ZERO = 0,           // should be initialized with 0
    FPS,

    RANDOM,
    NUMBER_COUNT
};

typedef buffered_global<eNumber, int, (size_t)eNumber::NUMBER_COUNT> gNumbers;
/*
class gNumbers
{
protected:
    constexpr gNumbers() : _data{ 0 } {}
private:
    static gNumbers _inst;
    std::array<int, (size_t)eNumber::NUMBER_COUNT> _data;
public:
    static constexpr int get(eNumber n) { return _inst._data[(size_t)n]; }
    static constexpr void set(eNumber n, int value) { _inst._data[(size_t)n] = value; }
};
*/
