#include "cfg_skin.h"

void ConfigSkin::setDefaults() noexcept
{
    using namespace cfg;
    _yaml[S_PATH_PLAY_7] = Path(S_DEFAULT_PATH_PLAY_7).generic_string();
    _yaml[S_PATH_RESULT] = Path(S_DEFAULT_PATH_RESULT).generic_string();
}