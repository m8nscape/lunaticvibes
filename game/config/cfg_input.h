#pragma once
#include "config.h"
#include "game/input/input_keys.h"

inline const char* CONFIG_FILE_INPUT_5 = "input5.yml";
inline const char* CONFIG_FILE_INPUT_7 = "input7.yml";
inline const char* CONFIG_FILE_INPUT_9 = "input9.yml";
inline const char* CONFIG_FILE_INPUT_DEFAULT_5 = "default5.yml";
inline const char* CONFIG_FILE_INPUT_DEFAULT_7 = "default7.yml";
inline const char* CONFIG_FILE_INPUT_DEFAULT_9 = "default9.yml";

namespace cfg
{
    inline const char* I_NOTBOUND = "_";

    inline const char* I_DEVICE_ID = "ID";             // USB: VID,PID,ID

    inline const char* I_BINDINGS_K1ScL = "1P_ScL";
    inline const char* I_BINDINGS_K1ScR = "1P_ScR";
    inline const char* I_BINDINGS_K11 = "1P_1";
    inline const char* I_BINDINGS_K12 = "1P_2";
    inline const char* I_BINDINGS_K13 = "1P_3";
    inline const char* I_BINDINGS_K14 = "1P_4";
    inline const char* I_BINDINGS_K15 = "1P_5";
    inline const char* I_BINDINGS_K16 = "1P_6";
    inline const char* I_BINDINGS_K17 = "1P_7";
    inline const char* I_BINDINGS_K18 = "1P_8";
    inline const char* I_BINDINGS_K19 = "1P_9";
    inline const char* I_BINDINGS_K1Start = "1P_Start";
    inline const char* I_BINDINGS_K1Select = "1P_Select";
    inline const char* I_BINDINGS_K1SpdUp = "1P_SpdUp";
    inline const char* I_BINDINGS_K1SpdDn = "1P_SpdDn";

    inline const char* I_BINDINGS_K2ScL = "1P_ScL";
    inline const char* I_BINDINGS_K2ScR = "1P_ScR";
    inline const char* I_BINDINGS_K21 = "2P_1";
    inline const char* I_BINDINGS_K22 = "2P_2";
    inline const char* I_BINDINGS_K23 = "2P_3";
    inline const char* I_BINDINGS_K24 = "2P_4";
    inline const char* I_BINDINGS_K25 = "2P_5";
    inline const char* I_BINDINGS_K26 = "2P_6";
    inline const char* I_BINDINGS_K27 = "2P_7";
    inline const char* I_BINDINGS_K28 = "2P_8";
    inline const char* I_BINDINGS_K29 = "2P_9";
    inline const char* I_BINDINGS_K2Start = "2P_Start";
    inline const char* I_BINDINGS_K2Select = "2P_Select";
    inline const char* I_BINDINGS_K2SpdUp = "2P_SpdUp";
    inline const char* I_BINDINGS_K2SpdDn = "2P_SpdDn";

}

class ConfigInput : public vConfig
{
private:
    int keys;
    YAML::Node blank_binding;
public:
    ConfigInput() = delete;
    ConfigInput(const std::string& profile, unsigned k);
    virtual ~ConfigInput();

    virtual void setDefaults() noexcept override;
    void setDefaultsUSLayout() noexcept;

public:
    void clearAll();

    void clearKey(Input::Ingame ingame);
    void bindKey(Input::Ingame ingame, Input::Key key, size_t slot);
    //void bindButton(unsigned id, Input::Ingame ingame, unsigned input);
    std::vector<Input::Key> getBindings(Input::Ingame key);
};
