#pragma once

namespace lunaticvibes
{

enum class eSoundSample : size_t
{
    BGM_TITLE,  // unused
    BGM_MODE,   // unused

    BGM_SELECT, // loop
    BGM_DECIDE,

    SOUND_SCRATCH,
    SOUND_O_OPEN,
    SOUND_O_CLOSE,
    SOUND_O_CHANGE,
    SOUND_F_OPEN,
    SOUND_F_CLOSE,
    SOUND_DIFFICULTY,

    SOUND_DECIDE,
    SOUND_PLAYSTOP,
    SOUND_CLEAR,
    SOUND_FAIL,

    SOUND_LANDMINE,

    SOUND_SCREENSHOT,

    SOUND_COURSE_CLEAR,
    SOUND_COURSE_FAIL,
};

void loadLR2Sound();

}
