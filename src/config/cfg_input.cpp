#include "common/log.h"
#include "cfg_input.h"

ConfigInput::ConfigInput(const std::string& profile, GameModeKeys k) : keys(k),
    vConfig(profile, (k == 5 ? CONFIG_FILE_INPUT_5 : (k == 7 ? CONFIG_FILE_INPUT_7 : (k == 9 ? CONFIG_FILE_INPUT_9 : "")))) 
{
}

ConfigInput::~ConfigInput() {}

void ConfigInput::setDefaults() noexcept
{
    using namespace cfg;
    _yaml.reset();
    clearAll();

    std::string path = (
        keys == 5 ? CONFIG_FILE_INPUT_DEFAULT_5 :
        keys == 7 ? CONFIG_FILE_INPUT_DEFAULT_7 :
        keys == 9 ? CONFIG_FILE_INPUT_DEFAULT_9 : "");
    try
    {
        _yaml = YAML::LoadFile(path);
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Config] Bad file while loading default input config: " << path;
    }
}

void ConfigInput::clearAll()
{
    using namespace cfg;
    
    set(I_BINDINGS_K1ScL, std::vector<std::string>());
    set(I_BINDINGS_K1ScR, std::vector<std::string>());
    set(I_BINDINGS_K11, std::vector<std::string>());
    set(I_BINDINGS_K12, std::vector<std::string>());
    set(I_BINDINGS_K13, std::vector<std::string>());
    set(I_BINDINGS_K14, std::vector<std::string>());
    set(I_BINDINGS_K15, std::vector<std::string>());
    set(I_BINDINGS_K16, std::vector<std::string>());
    set(I_BINDINGS_K17, std::vector<std::string>());
    set(I_BINDINGS_K18, std::vector<std::string>());
    set(I_BINDINGS_K19, std::vector<std::string>());
    set(I_BINDINGS_K1Start, std::vector<std::string>());
    set(I_BINDINGS_K1Select, std::vector<std::string>());
    set(I_BINDINGS_K1SpdUp, std::vector<std::string>());
    set(I_BINDINGS_K1SpdDn, std::vector<std::string>());

    set(I_BINDINGS_K2ScL, std::vector<std::string>());
    set(I_BINDINGS_K2ScR, std::vector<std::string>());
    set(I_BINDINGS_K21, std::vector<std::string>());
    set(I_BINDINGS_K22, std::vector<std::string>());
    set(I_BINDINGS_K23, std::vector<std::string>());
    set(I_BINDINGS_K24, std::vector<std::string>());
    set(I_BINDINGS_K25, std::vector<std::string>());
    set(I_BINDINGS_K26, std::vector<std::string>());
    set(I_BINDINGS_K27, std::vector<std::string>());
    set(I_BINDINGS_K28, std::vector<std::string>());
    set(I_BINDINGS_K29, std::vector<std::string>());
    set(I_BINDINGS_K2Start, std::vector<std::string>());
    set(I_BINDINGS_K2Select, std::vector<std::string>());
    set(I_BINDINGS_K2SpdUp, std::vector<std::string>());
    set(I_BINDINGS_K2SpdDn, std::vector<std::string>());
}

std::string getBindingKey(Input::Pad ingame)
{
    using namespace cfg;
    using namespace Input;
    switch (ingame)
    {
    case S1L:       return I_BINDINGS_K1ScL; 
    case S1R:       return I_BINDINGS_K1ScR; 
    case K11:       return I_BINDINGS_K11; 
    case K12:       return I_BINDINGS_K12; 
    case K13:       return I_BINDINGS_K13; 
    case K14:       return I_BINDINGS_K14; 
    case K15:       return I_BINDINGS_K15; 
    case K16:       return I_BINDINGS_K16; 
    case K17:       return I_BINDINGS_K17; 
    case K18:       return I_BINDINGS_K18; 
    case K19:       return I_BINDINGS_K19; 
    case K1START:   return I_BINDINGS_K1Start; 
    case K1SELECT:  return I_BINDINGS_K1Select; 
    case K1SPDUP:   return I_BINDINGS_K1SpdUp; 
    case K1SPDDN:   return I_BINDINGS_K1SpdDn; 
    
    case S2L:       return I_BINDINGS_K2ScL; 
    case S2R:       return I_BINDINGS_K2ScR; 
    case K21:       return I_BINDINGS_K21; 
    case K22:       return I_BINDINGS_K22; 
    case K23:       return I_BINDINGS_K23; 
    case K24:       return I_BINDINGS_K24; 
    case K25:       return I_BINDINGS_K25; 
    case K26:       return I_BINDINGS_K26; 
    case K27:       return I_BINDINGS_K27; 
    case K28:       return I_BINDINGS_K28; 
    case K29:       return I_BINDINGS_K29; 
    case K2START:   return I_BINDINGS_K2Start; 
    case K2SELECT:  return I_BINDINGS_K2Select; 
    case K2SPDUP:   return I_BINDINGS_K2SpdUp; 
    case K2SPDDN:   return I_BINDINGS_K2SpdDn; 

    default:        return I_NOTBOUND;
    }
}

void ConfigInput::clearKey(Input::Pad ingame)
{
    using namespace cfg;
    using namespace Input;

    StringContent mapKey = getBindingKey(ingame);
    if (mapKey != I_NOTBOUND)
        set(mapKey, std::vector<std::string>());
}

void ConfigInput::bindKey(Input::Pad ingame, Input::Keyboard key, size_t slot)
{
    using namespace cfg;
    using namespace Input;
    slot = std::min(slot, MAX_BINDINGS_PER_KEY - 1);
    std::string value = getKeyString(key);

    StringContent mapKey = getBindingKey(ingame);
    if (mapKey != I_NOTBOUND) set(mapKey, slot, value);
}

std::vector<Input::Keyboard> ConfigInput::getBindings(Input::Pad ingame)
{
    using namespace Input;
    using namespace cfg;
    StringContent mapKey = getBindingKey(ingame);

    auto keys = _yaml[mapKey];
    std::vector<Input::Keyboard> ret;
    for (const auto& k : keys)
    {
        std::string name = k.as<std::string>("INVALID");
        if (name.length() < 2) continue;

        switch (name[0])
        {
        case 'K':   // keyboard eg. K_A K_SLASH
            ret.push_back(Input::getByName(name.substr(2)));
            break;

        case 'J':   // joystick eg. J1_1 J2_4
            if (name[1] >= '0' && name[1] <= '9')
            {
                unsigned joyIdx = name[1] - '0';
                // push back 
            }
            break;

        default: break;
        }
        //ret.push_back(Input::getByName(k.as<std::string>(Input::keyboardNameMap[Input::K_ERROR])));
    }
    
    return ret;
}

std::string ConfigInput::getKeyString(Input::Keyboard k)
{
    return "K_"s + Input::keyboardNameMap[k];
}