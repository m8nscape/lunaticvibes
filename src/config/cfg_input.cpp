#include "cfg_input.h"

ConfigInput::ConfigInput() : vConfig(CONFIG_FILE_INPUT) {}
ConfigInput::~ConfigInput() {}

void ConfigInput::setDefaults() noexcept
{
    _yaml.reset();
    
}