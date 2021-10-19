#pragma once
#include "config.h"
#include "game/input/input_keys.h"

constexpr char CONFIG_FILE_INPUT_5[] = "input5.yml";
constexpr char CONFIG_FILE_INPUT_7[] = "input7.yml";
constexpr char CONFIG_FILE_INPUT_9[] = "input9.yml";
constexpr char CONFIG_FILE_INPUT_DEFAULT_5[] = "default5.yml";
constexpr char CONFIG_FILE_INPUT_DEFAULT_7[] = "default7.yml";
constexpr char CONFIG_FILE_INPUT_DEFAULT_9[] = "default9.yml";

namespace cfg
{
    constexpr char I_NOTBOUND[] = "_";

    constexpr char I_DEVICE_ID[] = "ID";             // USB: VID,PID,ID

    constexpr char I_BINDINGS_K1ScL[] = "1P_ScL";
    constexpr char I_BINDINGS_K1ScR[] = "1P_ScR";
    constexpr char I_BINDINGS_K11[] = "1P_1";
    constexpr char I_BINDINGS_K12[] = "1P_2";
    constexpr char I_BINDINGS_K13[] = "1P_3";
    constexpr char I_BINDINGS_K14[] = "1P_4";
    constexpr char I_BINDINGS_K15[] = "1P_5";
    constexpr char I_BINDINGS_K16[] = "1P_6";
    constexpr char I_BINDINGS_K17[] = "1P_7";
    constexpr char I_BINDINGS_K18[] = "1P_8";
    constexpr char I_BINDINGS_K19[] = "1P_9";
    constexpr char I_BINDINGS_K1Start[] = "1P_Start";
    constexpr char I_BINDINGS_K1Select[] = "1P_Select";
    constexpr char I_BINDINGS_K1SpdUp[] = "1P_SpdUp";
    constexpr char I_BINDINGS_K1SpdDn[] = "1P_SpdDn";

    constexpr char I_BINDINGS_K2ScL[] = "2P_ScL";
    constexpr char I_BINDINGS_K2ScR[] = "2P_ScR";
    constexpr char I_BINDINGS_K21[] = "2P_1";
    constexpr char I_BINDINGS_K22[] = "2P_2";
    constexpr char I_BINDINGS_K23[] = "2P_3";
    constexpr char I_BINDINGS_K24[] = "2P_4";
    constexpr char I_BINDINGS_K25[] = "2P_5";
    constexpr char I_BINDINGS_K26[] = "2P_6";
    constexpr char I_BINDINGS_K27[] = "2P_7";
    constexpr char I_BINDINGS_K28[] = "2P_8";
    constexpr char I_BINDINGS_K29[] = "2P_9";
    constexpr char I_BINDINGS_K2Start[] = "2P_Start";
    constexpr char I_BINDINGS_K2Select[] = "2P_Select";
    constexpr char I_BINDINGS_K2SpdUp[] = "2P_SpdUp";
    constexpr char I_BINDINGS_K2SpdDn[] = "2P_SpdDn";

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

    void clearKey(Input::Pad ingame);
    void bindKey(Input::Pad ingame, Input::Keyboard key, size_t slot);
    //void bindButton(unsigned id, Input::Pad ingame, unsigned input);
    std::vector<Input::Keyboard> getBindings(Input::Pad key);

    static std::string getKeyString(Input::Keyboard k);
};
