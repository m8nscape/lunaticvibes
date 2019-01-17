#include "config.h"
#include <plog/Log.h>
#include <fstream>

vConfig::vConfig()
{
	// placeholder, do nothing
}

vConfig::vConfig(StringPath file)
{
	_path = file;
}

vConfig::~vConfig()
{
}

void vConfig::load()
{
	std::string fileStr;
	fileStr.assign(_path.begin(), _path.end());
	setDefaults();
	try
	{
		//_yaml = YAML::LoadFile(fileStr);
		_toml = cpptoml::parse_file(fileStr);
	}
	catch (const cpptoml::parse_exception&)
	{
		LOG_WARNING << "[Config] Bad file: " << fileStr;
	}
}

void vConfig::save()
{
	std::string fileStr;
	fileStr.assign(_path.begin(), _path.end());
	std::ofstream fout(fileStr, std::ios_base::out);
	fout << (*_toml);
}