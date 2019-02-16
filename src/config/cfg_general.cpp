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
    /* TOML set
	set(A_MODE, A_MODE_AUTO);
	set(A_BUFCOUNT, 4);
	set(A_BUFLEN, 256);
	set(V_RES_X, 1280);
	set(V_RES_Y, 720);
	set(V_FULL_RES_X, 1280);
	set(V_FULL_RES_Y, 720);
	set(V_WINMODE, V_WINMODE_WINDOWED);
	set(V_MAXFPS, 480);
	set(V_VSYNC, ON);
    */

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
