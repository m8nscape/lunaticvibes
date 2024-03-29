#include "cfg_skin.h"

void ConfigSkin::setDefaults() noexcept
{
    using namespace cfg;
    set(S_PATH_MUSIC_SELECT, S_DEFAULT_PATH_MUSIC_SELECT);
    set(S_PATH_DECIDE, S_DEFAULT_PATH_DECIDE);
    set(S_PATH_PLAY_5, S_DEFAULT_PATH_PLAY_5);
    set(S_PATH_PLAY_5_BATTLE, S_DEFAULT_PATH_PLAY_5_BATTLE);
    set(S_PATH_PLAY_7, S_DEFAULT_PATH_PLAY_7);
    set(S_PATH_PLAY_7_BATTLE, S_DEFAULT_PATH_PLAY_7_BATTLE);
    set(S_PATH_PLAY_9, S_DEFAULT_PATH_PLAY_9);
    set(S_PATH_PLAY_10, S_DEFAULT_PATH_PLAY_10);
    set(S_PATH_PLAY_14, S_DEFAULT_PATH_PLAY_14);
    set(S_PATH_RESULT, S_DEFAULT_PATH_RESULT);
    set(S_PATH_KEYCONFIG, S_DEFAULT_PATH_KEYCONFIG);
    set(S_PATH_CUSTOMIZE, S_DEFAULT_PATH_CUSTOMIZE);
    set(S_PATH_SOUNDSET, S_DEFAULT_PATH_SOUNDSET);
}