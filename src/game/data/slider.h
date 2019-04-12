#pragma once
#include "buffered_global.h"
typedef double percent;

enum class eSlider : unsigned
{
    ZERO = 0,
    SLIDER_COUNT
};

inline buffered_global<eSlider, percent, (size_t)eSlider::SLIDER_COUNT> gSliders;

/*
class gSliders
{
protected:
    constexpr gSliders() : _data{ 0u } {}
private:
    std::array<percent, (size_t)eSlider::SLIDER_COUNT> _data;
public:
    constexpr int get(eSlider n) { return _data[(size_t)n]; }
    constexpr void set(eSlider n, percent value) { _data[(size_t)n] = value < 100u ? value : 100u; }
};
*/