#include "cfg_input.h"

ConfigInput::ConfigInput() : vConfig(CONFIG_FILE_INPUT) {}
ConfigInput::~ConfigInput() {}

void ConfigInput::setDefaults() noexcept
{
    //_yaml.reset();
	_toml = cpptoml::make_table(); // just makes a new table to replace old one
}

void ConfigInput::bindKey(Input::Ingame ingame, Input::Key key)
{
    
}
