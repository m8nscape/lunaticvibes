#include "common/log.h"
#include "cfg_input.h"

ConfigInput::ConfigInput(const std::string& profile, GameModeKeys k) : keys(k),
    vConfig(profile, (k == 5 ? CONFIG_FILE_INPUT_5 : (k == 7 ? CONFIG_FILE_INPUT_7 : (k == 9 ? CONFIG_FILE_INPUT_9 : "")))) 
{
}

ConfigInput::~ConfigInput() {}

const char* getBindingKey(Input::Pad ingame)
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
    case S1A:       return I_BINDINGS_K1ScAxis;

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
    case S2A:       return I_BINDINGS_K2ScAxis;

    default:        return I_NOTBOUND;
    }
}

void ConfigInput::load()
{
    vConfig::load();

    for (auto p = Input::Pad::S1L; p < Input::Pad::ESC; p = Input::Pad(int(p) + 1))
    {
        const char* mapKey = getBindingKey(p);
        const auto& c = _yaml[mapKey];
        if (c.Type() == YAML::NodeType::Scalar)
        {
            buffer[p] = KeyMap(c.as<std::string>("INVALID"));
        }
        else
        {
            buffer[p] = KeyMap("INVALID");
        }
    }
}

void ConfigInput::setDefaults() noexcept
{
    using namespace cfg;
    _yaml.reset();
    clearAll();

    Path path = GAMEDATA_PATH;
    path /= keys == 5 ? CONFIG_FILE_INPUT_DEFAULT_5 :
            keys == 7 ? CONFIG_FILE_INPUT_DEFAULT_7 :
            keys == 9 ? CONFIG_FILE_INPUT_DEFAULT_9 : "";
    try
    {
        _yaml = YAML::LoadFile(path.string());
        for (auto p = Input::Pad::S1L; p < Input::Pad::ESC; p = Input::Pad(int(p) + 1))
        {
            const char* mapKey = getBindingKey(p);
            const auto& c = _yaml[mapKey];
            if (c.Type() == YAML::NodeType::Scalar)
            {
                buffer[p] = KeyMap(c.as<std::string>("INVALID"));
            }
            else
            {
                buffer[p] = KeyMap("INVALID");
            }
        }
    }
    catch (YAML::BadFile&)
    {
        LOG_WARNING << "[Config] Bad file while loading default input config: " << path;
    }
}

void ConfigInput::clearAll()
{
    using namespace cfg;
    
    set(I_BINDINGS_K1ScL, "");
    set(I_BINDINGS_K1ScR, "");
    set(I_BINDINGS_K11, "");
    set(I_BINDINGS_K12, "");
    set(I_BINDINGS_K13, "");
    set(I_BINDINGS_K14, "");
    set(I_BINDINGS_K15, "");
    set(I_BINDINGS_K16, "");
    set(I_BINDINGS_K17, "");
    set(I_BINDINGS_K18, "");
    set(I_BINDINGS_K19, "");
    set(I_BINDINGS_K1Start, "");
    set(I_BINDINGS_K1Select, "");
    set(I_BINDINGS_K1SpdUp, "");
    set(I_BINDINGS_K1SpdDn, "");
    set(I_BINDINGS_K1ScAxis, "");

    set(I_BINDINGS_K2ScL, "");
    set(I_BINDINGS_K2ScR, "");
    set(I_BINDINGS_K21, "");
    set(I_BINDINGS_K22, "");
    set(I_BINDINGS_K23, "");
    set(I_BINDINGS_K24, "");
    set(I_BINDINGS_K25, "");
    set(I_BINDINGS_K26, "");
    set(I_BINDINGS_K27, "");
    set(I_BINDINGS_K28, "");
    set(I_BINDINGS_K29, "");
    set(I_BINDINGS_K2Start, "");
    set(I_BINDINGS_K2Select, "");
    set(I_BINDINGS_K2SpdUp, "");
    set(I_BINDINGS_K2SpdDn, "");
    set(I_BINDINGS_K2ScAxis, "");

    for (auto p = Input::Pad::S1L; p < Input::Pad::ESC; p = Input::Pad(int(p) + 1))
    {
        buffer[p] = KeyMap("INVALID");
    }
}


void ConfigInput::clearKey(Input::Pad ingame)
{
    using namespace cfg;
    using namespace Input;

    StringContent mapKey = getBindingKey(ingame);
    if (mapKey != I_NOTBOUND)
    {
        set(mapKey, "");
        buffer[ingame] = KeyMap("INVALID");
    }
}

void ConfigInput::bind(Input::Pad ingame, const KeyMap& km)
{
    StringContent mapKey = getBindingKey(ingame);
    if (mapKey != cfg::I_NOTBOUND)
    {
        set(mapKey, km.toString());
        buffer[ingame] = km;

        switch (ingame)
        {
        case Input::Pad::S1A:
            clearKey(Input::Pad::S1L);
            clearKey(Input::Pad::S1R);
            break;
        case Input::Pad::S2A:
            clearKey(Input::Pad::S2L);
            clearKey(Input::Pad::S2R);
            break;
        case Input::Pad::S1L:
        case Input::Pad::S1R:
            clearKey(Input::Pad::S1A);
            break;
        case Input::Pad::S2L:
        case Input::Pad::S2R:
            clearKey(Input::Pad::S2A);
            break;
        }
    }
}

KeyMap ConfigInput::getBindings(Input::Pad ingame)
{
    if (ingame >= Input::Pad::ESC)
        return KeyMap("INVALID");
    else
        return buffer[ingame];
}
