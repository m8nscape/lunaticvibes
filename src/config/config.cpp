#include "config.h"
#include <fstream>

vConfig::vConfig()
{
    // placeholder, do nothing
}

vConfig::vConfig(StringPath file)
{
	_path = file;
	load();
}

vConfig::~vConfig()
{
}

void vConfig::load()
{
	std::string fileStr;
	fileStr.assign(_path.begin(), _path.end());
	setDefaults();
	_yaml = YAML::LoadFile(fileStr);
}

void vConfig::save()
{
	std::string fileStr;
	fileStr.assign(_path.begin(), _path.end());
	std::ofstream fout(fileStr, std::ios_base::out);
	fout << _yaml;
}

