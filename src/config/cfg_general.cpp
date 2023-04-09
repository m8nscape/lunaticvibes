#include "cfg_general.h"
#include "config_mgr.h"

void ConfigGeneral::setDefaults() noexcept
{
    using namespace cfg;
	set(A_MODE, A_MODE_AUTO);
	set(A_BUFCOUNT, 4);
	set(A_BUFLEN, 256);
	set(V_RES_SUPERSAMPLE, 1);
	set(V_DISPLAY_RES_X, CANVAS_WIDTH);
	set(V_DISPLAY_RES_Y, CANVAS_HEIGHT);
	set(V_WINMODE, V_WINMODE_WINDOWED);
	set(V_MAXFPS, 480);
	set(V_VSYNC, true);
	set(E_PROFILE, PROFILE_DEFAULT);
	set(E_LR2PATH, ".");
	set(E_FOLDERS, std::vector<std::string>());
	set(E_TABLES, std::vector<std::string>());
	set(E_LOG_LEVEL, E_LOG_LEVEL_INFO);
}


void ConfigGeneral::setFolders(const std::vector<std::string>& path)
{
	_yaml[cfg::E_FOLDERS] = path;
}

void ConfigGeneral::setTables(const std::vector<std::string>& urls)
{
	_yaml[cfg::E_TABLES] = urls;
}

std::vector<StringPath> ConfigGeneral::getFoldersPath()
{
	auto folderList = _yaml[cfg::E_FOLDERS].as<std::vector<std::string>>(std::vector<std::string>());
	std::vector<StringPath> ret;
	for (const auto& p : folderList)
	{
		ret.push_back(Path(p).native());
	}
	return ret;
}

std::vector<std::string> ConfigGeneral::getFoldersStr()
{
	return _yaml[cfg::E_FOLDERS].as<std::vector<std::string>>(std::vector<std::string>());
}

std::vector<std::string> ConfigGeneral::getTablesUrl()
{
	return _yaml[cfg::E_TABLES].as<std::vector<std::string>>(std::vector<std::string>());
}