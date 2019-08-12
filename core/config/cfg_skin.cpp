#include "cfg_skin.h"


ConfigSkin::ConfigSkin() : vConfig(CONFIG_FILE_SKIN) {}
ConfigSkin::~ConfigSkin() {}

void ConfigSkin::setDefaults() noexcept
{
    using namespace cfg;
    _yaml[S_PATH_PLAY_7] = S_DEFAULT_PATH_PLAY_7;
    _yaml[S_PATH_RESULT] = S_DEFAULT_PATH_RESULT;
}