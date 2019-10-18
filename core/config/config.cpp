#include "config.h"
#include <plog/Log.h>
#include <fstream>
#include <filesystem>

vConfig::vConfig()
{
    // placeholder, do nothing
}

vConfig::vConfig(const StringPath& file)
{
	_path = file;
}

vConfig::vConfig(const std::string& profile, const StringPath& file)
{
    _path = ".";
    _path /= profile;
    _path /= file;
}

void vConfig::load()
{
	setDefaults();
    try
    {
        _yaml = YAML::LoadFile(_path.generic_string());
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Config] Bad file: " << _path.generic_string();
    }
}

void vConfig::save()
{
	std::ofstream fout(_path, std::ios_base::out);
	fout << _yaml;
}

