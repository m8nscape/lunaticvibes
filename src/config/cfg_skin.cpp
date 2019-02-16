#include "cfg_skin.h"


ConfigSkin::ConfigSkin() : vConfig(CONFIG_FILE_SKIN) {}
ConfigSkin::~ConfigSkin() {}

void ConfigSkin::setDefaults() noexcept
{
    using namespace cfg;
    _yaml[S_PATH_PLAY] = S_DEFAULT_PATH_PLAY;
}