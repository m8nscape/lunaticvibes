#pragma once

/*
* !!! WARNING !!!
* These indices are currently DIRECTLY casted to LR2skin indices.
* When adding new value, do not modify existing values.
*/
enum class IndexSlider : unsigned
{
    ZERO = 0,

    SELECT_LIST = 1,

    HISPEED_1P = 2,
    HISPEED_2P,

    SUD_1P = 4,
    SUD_2P,

    SONG_PROGRESS = 6,

    SKIN_CONFIG_OPTIONS = 7,

    HID_1P = 8,
    HID_2P,

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

    // new
	// key config extended
    DEADZONE_S1L = 30,
    DEADZONE_S1R,
    DEADZONE_K1Start,
    DEADZONE_K1Select,

    DEADZONE_S2L = 35,
    DEADZONE_S2R,
    DEADZONE_K2Start,
    DEADZONE_K2Select,

    SPEED_S1A = 40,
    DEADZONE_K11,
    DEADZONE_K12,
    DEADZONE_K13,
    DEADZONE_K14,
    DEADZONE_K15,
    DEADZONE_K16,
    DEADZONE_K17,
    DEADZONE_K18,
    DEADZONE_K19,

    SPEED_S2A = 50,
    DEADZONE_K21,
    DEADZONE_K22,
    DEADZONE_K23,
    DEADZONE_K24,
    DEADZONE_K25,
    DEADZONE_K26,
    DEADZONE_K27,
    DEADZONE_K28,
    DEADZONE_K29,

    // internal values
    _TEST1 = 60,

    SLIDER_COUNT
};
