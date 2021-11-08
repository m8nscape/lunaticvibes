#include "cfg_general.h"
#include "config_mgr.h"

void ConfigGeneral::setDefaults() noexcept
{
    using namespace cfg;
	set(A_MODE, A_MODE_AUTO);
	set(A_BUFCOUNT, 4);
	set(A_BUFLEN, 256);
	set(V_RES_X, 1280);
	set(V_RES_Y, 720);
	set(V_WINMODE, V_WINMODE_WINDOWED);
	set(V_MAXFPS, 480);
	set(V_VSYNC, true);
	set(E_PROFILE, PROFILE_DEFAULT);
	set(E_LR2PATH, ".");
	set(E_FOLDERS, std::vector<std::string>());
}


void ConfigGeneral::setFolders(const std::vector<StringPath>& path)
{
	std::vector<std::string> folderList;
	for (const auto& p : path)
	{
		folderList.push_back(Path(p).string());
	}
	_yaml[cfg::E_FOLDERS] = folderList;
}

void ConfigGeneral::setFolders(const std::vector<std::string>& path)
{
	_yaml[cfg::E_FOLDERS] = path;
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