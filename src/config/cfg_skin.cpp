#include "cfg_skin.h"

void ConfigSkin::setDefaults() noexcept
{
    using namespace cfg;
    set(S_PATH_MUSIC_SELECT, S_DEFAULT_PATH_MUSIC_SELECT);
    set(S_PATH_DECIDE, S_DEFAULT_PATH_DECIDE);
    set(S_PATH_PLAY_7, S_DEFAULT_PATH_PLAY_7);
    set(S_PATH_RESULT, S_DEFAULT_PATH_RESULT);
    set(S_PATH_KEYCONFIG, S_DEFAULT_PATH_KEYCONFIG);
    set(S_PATH_BGM, S_DEFAULT_PATH_BGM);
    set(S_PATH_SOUND, S_DEFAULT_PATH_SOUND);
}