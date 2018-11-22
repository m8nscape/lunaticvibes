#include "config.h"
#include <fstream>

vCfg::vCfg()
{
    // placeholder, do nothing
}

vCfg::vCfg(StringPath file)
{
	_path = file;
	load();
}

vCfg::~vCfg()
{

}

void vCfg::load()
{
	std::string fileStr;
	fileStr.assign(_path.begin(), _path.end());
	_yaml = YAML::LoadFile(fileStr);
}

void vCfg::save()
{
	std::string fileStr;
	fileStr.assign(_path.begin(), _path.end());
	std::ofstream fout(fileStr, std::ios_base::out);
	fout << _yaml;
}

