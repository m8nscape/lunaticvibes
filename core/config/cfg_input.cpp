#include <plog/Log.h>
#include "cfg_input.h"

ConfigInput::ConfigInput(unsigned k) : keys(k),
    vConfig((k == 5 ? CONFIG_FILE_INPUT_5 : (k == 7 ? CONFIG_FILE_INPUT_7 : (k == 9 ? CONFIG_FILE_INPUT_9 : "")))) 
{
    for (int i = 0; i < Input::MAX_BINDINGS_PER_KEY; ++i) blank_binding.push_back(cfg::I_NOTBOUND);
}

ConfigInput::~ConfigInput() {}

void ConfigInput::setDefaults() noexcept
{
    using namespace cfg;
    _yaml.reset();
    clearAll();

    std::string path;
    path =
        keys == 5 ? CONFIG_FILE_INPUT_DEFAULT_5 :
        keys == 7 ? CONFIG_FILE_INPUT_DEFAULT_7 :
        keys == 9 ? CONFIG_FILE_INPUT_DEFAULT_9 : "";
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
    
    _yaml[I_BINDINGS_K1ScL] = blank_binding;
    _yaml[I_BINDINGS_K1ScR] = blank_binding;
    _yaml[I_BINDINGS_K11] = blank_binding;
    _yaml[I_BINDINGS_K12] = blank_binding;
    _yaml[I_BINDINGS_K13] = blank_binding;
    _yaml[I_BINDINGS_K14] = blank_binding;
    _yaml[I_BINDINGS_K15] = blank_binding;
    _yaml[I_BINDINGS_K16] = blank_binding;
    _yaml[I_BINDINGS_K17] = blank_binding;
    _yaml[I_BINDINGS_K18] = blank_binding;
    _yaml[I_BINDINGS_K19] = blank_binding;
    _yaml[I_BINDINGS_K1Start] = blank_binding;
    _yaml[I_BINDINGS_K1Select] = blank_binding;
    _yaml[I_BINDINGS_K1SpdUp] = blank_binding;
    _yaml[I_BINDINGS_K1SpdDn] = blank_binding;

    _yaml[I_BINDINGS_K2ScL] = blank_binding;
    _yaml[I_BINDINGS_K2ScR] = blank_binding;
    _yaml[I_BINDINGS_K21] = blank_binding;
    _yaml[I_BINDINGS_K22] = blank_binding;
    _yaml[I_BINDINGS_K23] = blank_binding;
    _yaml[I_BINDINGS_K24] = blank_binding;
    _yaml[I_BINDINGS_K25] = blank_binding;
    _yaml[I_BINDINGS_K26] = blank_binding;
    _yaml[I_BINDINGS_K27] = blank_binding;
    _yaml[I_BINDINGS_K28] = blank_binding;
    _yaml[I_BINDINGS_K29] = blank_binding;
    _yaml[I_BINDINGS_K2Start] = blank_binding;
    _yaml[I_BINDINGS_K2Select] = blank_binding;
    _yaml[I_BINDINGS_K2SpdUp] = blank_binding;
    _yaml[I_BINDINGS_K2SpdDn] = blank_binding;
}

void ConfigInput::clearKey(Input::Ingame ingame)
{
    using namespace cfg;
    using namespace Input;

    switch (ingame)
    {
    case S1L:       _yaml[I_BINDINGS_K1ScL] = blank_binding; break;
    case S1R:       _yaml[I_BINDINGS_K1ScR] = blank_binding; break;
    case K11:       _yaml[I_BINDINGS_K11] = blank_binding; break;
    case K12:       _yaml[I_BINDINGS_K12] = blank_binding; break;
    case K13:       _yaml[I_BINDINGS_K13] = blank_binding; break;
    case K14:       _yaml[I_BINDINGS_K14] = blank_binding; break;
    case K15:       _yaml[I_BINDINGS_K15] = blank_binding; break;
    case K16:       _yaml[I_BINDINGS_K16] = blank_binding; break;
    case K17:       _yaml[I_BINDINGS_K17] = blank_binding; break;
    case K18:       _yaml[I_BINDINGS_K18] = blank_binding; break;
    case K19:       _yaml[I_BINDINGS_K19] = blank_binding; break;
    case K1START:   _yaml[I_BINDINGS_K1Start] = blank_binding; break;
    case K1SELECT:  _yaml[I_BINDINGS_K1Select] = blank_binding; break;
    case K1SPDUP:   _yaml[I_BINDINGS_K1SpdUp] = blank_binding; break;
    case K1SPDDN:   _yaml[I_BINDINGS_K1SpdDn] = blank_binding; break;

    case S2L:       _yaml[I_BINDINGS_K2ScL] = blank_binding; break;
    case S2R:       _yaml[I_BINDINGS_K2ScR] = blank_binding; break;
    case K21:       _yaml[I_BINDINGS_K21] = blank_binding; break;
    case K22:       _yaml[I_BINDINGS_K22] = blank_binding; break;
    case K23:       _yaml[I_BINDINGS_K23] = blank_binding; break;
    case K24:       _yaml[I_BINDINGS_K24] = blank_binding; break;
    case K25:       _yaml[I_BINDINGS_K25] = blank_binding; break;
    case K26:       _yaml[I_BINDINGS_K26] = blank_binding; break;
    case K27:       _yaml[I_BINDINGS_K27] = blank_binding; break;
    case K28:       _yaml[I_BINDINGS_K28] = blank_binding; break;
    case K29:       _yaml[I_BINDINGS_K29] = blank_binding; break;
    case K2START:   _yaml[I_BINDINGS_K2Start] = blank_binding; break;
    case K2SELECT:  _yaml[I_BINDINGS_K2Select] = blank_binding; break;
    case K2SPDUP:   _yaml[I_BINDINGS_K2SpdUp] = blank_binding; break;
    case K2SPDDN:   _yaml[I_BINDINGS_K2SpdDn] = blank_binding; break;
        
    default: break;
    }
}

void ConfigInput::bindKey(Input::Ingame ingame, Input::Key key, size_t slot)
{
    using namespace cfg;
    using namespace Input;
    slot = std::min(slot, MAX_BINDINGS_PER_KEY - 1);
    std::string value = std::string("K_") + keyNameMap[key];

    switch (ingame)
    {
    case S1L:       _yaml[I_BINDINGS_K1ScL][slot] = value; break;
    case S1R:       _yaml[I_BINDINGS_K1ScR][slot] = value; break;
    case K11:       _yaml[I_BINDINGS_K11][slot] = value; break;
    case K12:       _yaml[I_BINDINGS_K12][slot] = value; break;
    case K13:       _yaml[I_BINDINGS_K13][slot] = value; break;
    case K14:       _yaml[I_BINDINGS_K14][slot] = value; break;
    case K15:       _yaml[I_BINDINGS_K15][slot] = value; break;
    case K16:       _yaml[I_BINDINGS_K16][slot] = value; break;
    case K17:       _yaml[I_BINDINGS_K17][slot] = value; break;
    case K18:       _yaml[I_BINDINGS_K18][slot] = value; break;
    case K19:       _yaml[I_BINDINGS_K19][slot] = value; break;
    case K1START:   _yaml[I_BINDINGS_K1Start][slot] = value; break;
    case K1SELECT:  _yaml[I_BINDINGS_K1Select][slot] = value; break;
    case K1SPDUP:   _yaml[I_BINDINGS_K1SpdUp][slot] = value; break;
    case K1SPDDN:   _yaml[I_BINDINGS_K1SpdDn][slot] = value; break;

    case S2L:       _yaml[I_BINDINGS_K2ScL][slot] = value; break;
    case S2R:       _yaml[I_BINDINGS_K2ScR][slot] = value; break;
    case K21:       _yaml[I_BINDINGS_K21][slot] = value; break;
    case K22:       _yaml[I_BINDINGS_K22][slot] = value; break;
    case K23:       _yaml[I_BINDINGS_K23][slot] = value; break;
    case K24:       _yaml[I_BINDINGS_K24][slot] = value; break;
    case K25:       _yaml[I_BINDINGS_K25][slot] = value; break;
    case K26:       _yaml[I_BINDINGS_K26][slot] = value; break;
    case K27:       _yaml[I_BINDINGS_K27][slot] = value; break;
    case K28:       _yaml[I_BINDINGS_K28][slot] = value; break;
    case K29:       _yaml[I_BINDINGS_K29][slot] = value; break;
    case K2START:   _yaml[I_BINDINGS_K2Start][slot] = value; break;
    case K2SELECT:  _yaml[I_BINDINGS_K2Select][slot] = value; break;
    case K2SPDUP:   _yaml[I_BINDINGS_K2SpdUp][slot] = value; break;
    case K2SPDDN:   _yaml[I_BINDINGS_K2SpdDn][slot] = value; break;
        
    default: break;
    }

}

std::vector<Input::Key> ConfigInput::getBindings(Input::Ingame key)
{
    using namespace Input;
    using namespace cfg;
    StringContent mapKey = I_NOTBOUND;
    switch (key)
    {
    case S1L:       mapKey = I_BINDINGS_K1ScL; break;
    case S1R:       mapKey = I_BINDINGS_K1ScR; break;
    case K11:       mapKey = I_BINDINGS_K11; break;
    case K12:       mapKey = I_BINDINGS_K12; break;
    case K13:       mapKey = I_BINDINGS_K13; break;
    case K14:       mapKey = I_BINDINGS_K14; break;
    case K15:       mapKey = I_BINDINGS_K15; break;
    case K16:       mapKey = I_BINDINGS_K16; break;
    case K17:       mapKey = I_BINDINGS_K17; break;
    case K18:       mapKey = I_BINDINGS_K18; break;
    case K19:       mapKey = I_BINDINGS_K19; break;
    case K1START:   mapKey = I_BINDINGS_K1Start; break;
    case K1SELECT:  mapKey = I_BINDINGS_K1Select; break;
    case K1SPDUP:   mapKey = I_BINDINGS_K1SpdUp; break;
    case K1SPDDN:   mapKey = I_BINDINGS_K1SpdDn; break;

    case S2L:       mapKey = I_BINDINGS_K2ScL; break;
    case S2R:       mapKey = I_BINDINGS_K2ScR; break;
    case K21:       mapKey = I_BINDINGS_K21; break;
    case K22:       mapKey = I_BINDINGS_K22; break;
    case K23:       mapKey = I_BINDINGS_K23; break;
    case K24:       mapKey = I_BINDINGS_K24; break;
    case K25:       mapKey = I_BINDINGS_K25; break;
    case K26:       mapKey = I_BINDINGS_K26; break;
    case K27:       mapKey = I_BINDINGS_K27; break;
    case K28:       mapKey = I_BINDINGS_K28; break;
    case K29:       mapKey = I_BINDINGS_K29; break;
    case K2START:   mapKey = I_BINDINGS_K2Start; break;
    case K2SELECT:  mapKey = I_BINDINGS_K2Select; break;
    case K2SPDUP:   mapKey = I_BINDINGS_K2SpdUp; break;
    case K2SPDDN:   mapKey = I_BINDINGS_K2SpdDn; break;

    default:        break;
    }

    auto keys = _yaml[mapKey];
    std::vector<Input::Key> ret;
    for (auto& k : keys)
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
        ret.push_back(Input::getByName(k.as<std::string>(Input::keyNameMap[Input::K_ERROR])));
    }
    
    return ret;
}
