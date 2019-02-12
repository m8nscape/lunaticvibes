#include "cfg_general.h"

ConfigGeneral::ConfigGeneral() : vConfig(CONFIG_FILE_GENERAL)
{
}

ConfigGeneral::~ConfigGeneral()
{
}

void ConfigGeneral::setDefaults() noexcept
{
    using namespace cfg;
    _yaml[A_MODE] = A_MODE_AUTO;
    _yaml[A_BUFCOUNT] = 4;
    _yaml[A_BUFLEN] = 256;
    _yaml[V_RES_X] = 1280;
    _yaml[V_RES_Y] = 720;
    _yaml[V_FULL_RES_X] = 1280;
    _yaml[V_FULL_RES_Y] = 720;
    _yaml[V_WINMODE] = V_WINMODE_WINDOWED;
    _yaml[V_MAXFPS] = 480;
    _yaml[V_VSYNC] = ON;

}
