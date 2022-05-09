#pragma once
#include "buffered_global.h"

enum class eSlider : unsigned
{
    ZERO = 0,

    SELECT_LIST = 1,

    HISPEED_1P = 2,
    HISPEED_2P,

    SUD_1P = 4,
    SUD_2P,

    SONG_PROGRESS = 6,

    SKIN_CONFIG_OPTIONS = 7,

    EQ0 = 10,
    EQ1,
    EQ2,
    EQ3,
    EQ4,
    EQ5,
    EQ6,

    VOLUME_MASTER = 17,
    VOLUME_KEY,
    VOLUME_BGM,

    FX0_P1 = 20,
    FX0_P2,
    FX1_P1,
    FX1_P2,
    FX2_P1,
    FX2_P2,

    PITCH = 26,

    _TEST1,
    SLIDER_COUNT
};

inline buffered_global<eSlider, double, (size_t)eSlider::SLIDER_COUNT> gSliders;

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