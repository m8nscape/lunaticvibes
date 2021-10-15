#include "cfg_skin.h"

void ConfigSkin::setDefaults() noexcept
{
    using namespace cfg;
    set(S_PATH_MUSIC_SELECT, Path(S_DEFAULT_PATH_MUSIC_SELECT).generic_string());
    set(S_PATH_DECIDE, Path(S_DEFAULT_PATH_DECIDE).generic_string());
    set(S_PATH_PLAY_7, Path(S_DEFAULT_PATH_PLAY_7).generic_string());
    set(S_PATH_RESULT, Path(S_DEFAULT_PATH_RESULT).generic_string());
    set(S_PATH_BGM, Path(S_DEFAULT_PATH_BGM).generic_string());
    set(S_PATH_SOUND, Path(S_DEFAULT_PATH_SOUND).generic_string());
}