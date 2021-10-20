#include "config.h"
#include "common/log.h"
#include <fstream>
#include <filesystem>

vConfig::vConfig()
{
    // placeholder, do nothing
}

vConfig::vConfig(const char* file)
{
	_path = file;
}

vConfig::vConfig(const std::string& profile, const char* file)
{
    _path = GAMEDATA_PATH;
    _path /= "profile";
    _path /= profile;
    _path /= file;
}

void vConfig::load()
{
	setDefaults();
    try
    {
        for (const auto& node : YAML::LoadFile(_path.string()))
        {
            _yaml[node.first.as<std::string>()] = node.second;
        }
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Config] Bad file: " << _path.string();
    }
}

void vConfig::save()
{
	std::ofstream fout(_path, std::ios_base::trunc);
	fout << _yaml;
    fout.close();
}

