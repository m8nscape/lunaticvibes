#pragma once
enum class eMod
{
    // random
    RANDOM = 0,
    MIRROR,
    SRAN,
    HRAN,			// Scatter
    ALLSCR,			// Converge

    // gauge
    HARD = 6,
    EASY,
    DEATH,
    PATTACK,
    GATTACK,

    // assist
    AUTO67 = 12,	// 5keys
    AUTOSCR,
    LEGACY,			// LN -> Note
    NOMINES,

    // hs fix
    MAXBPM = 18,
    MINBPM,
    AVERAGE,
    CONSTANT,

    // effect
    SUDDEN = 24,
    HIDDEN,
    LIFT,

    // special

};
