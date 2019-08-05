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
}
