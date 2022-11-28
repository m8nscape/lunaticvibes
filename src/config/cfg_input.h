#pragma once
#include "config.h"
#include "common/keymap.h"

constexpr char CONFIG_FILE_INPUT_5[] = "input5.yml";
constexpr char CONFIG_FILE_INPUT_7[] = "input7.yml";
constexpr char CONFIG_FILE_INPUT_9[] = "input9.yml";
constexpr char CONFIG_FILE_INPUT_DEFAULT_5[] = "resources/keyconfig/default5.yml";
constexpr char CONFIG_FILE_INPUT_DEFAULT_7[] = "resources/keyconfig/default7.yml";
constexpr char CONFIG_FILE_INPUT_DEFAULT_9[] = "resources/keyconfig/default9.yml";

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
    constexpr char I_BINDINGS_K1ScAxis[] = "1P_ScAxis";

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
    constexpr char I_BINDINGS_K2ScAxis[] = "2P_ScAxis";

    constexpr char I_INPUT_DEADZONE_S1L[] = "InputDeadZoneS1L";
    constexpr char I_INPUT_DEADZONE_S1R[] = "InputDeadZoneS1R";
    constexpr char I_INPUT_DEADZONE_K1Start[] = "InputDeadZoneK1Start";
    constexpr char I_INPUT_DEADZONE_K1Select[] = "InputDeadZoneK1Select";
    constexpr char I_INPUT_DEADZONE_K11[] = "InputDeadZoneK11";
    constexpr char I_INPUT_DEADZONE_K12[] = "InputDeadZoneK12";
    constexpr char I_INPUT_DEADZONE_K13[] = "InputDeadZoneK13";
    constexpr char I_INPUT_DEADZONE_K14[] = "InputDeadZoneK14";
    constexpr char I_INPUT_DEADZONE_K15[] = "InputDeadZoneK15";
    constexpr char I_INPUT_DEADZONE_K16[] = "InputDeadZoneK16";
    constexpr char I_INPUT_DEADZONE_K17[] = "InputDeadZoneK17";
    constexpr char I_INPUT_DEADZONE_K18[] = "InputDeadZoneK18";
    constexpr char I_INPUT_DEADZONE_K19[] = "InputDeadZoneK19";
    constexpr char I_INPUT_DEADZONE_S2L[] = "InputDeadZoneS2L";
    constexpr char I_INPUT_DEADZONE_S2R[] = "InputDeadZoneS2R";
    constexpr char I_INPUT_DEADZONE_K2Start[] = "InputDeadZoneK2Start";
    constexpr char I_INPUT_DEADZONE_K2Select[] = "InputDeadZoneK2Select";
    constexpr char I_INPUT_DEADZONE_K21[] = "InputDeadZoneK21";
    constexpr char I_INPUT_DEADZONE_K22[] = "InputDeadZoneK22";
    constexpr char I_INPUT_DEADZONE_K23[] = "InputDeadZoneK23";
    constexpr char I_INPUT_DEADZONE_K24[] = "InputDeadZoneK24";
    constexpr char I_INPUT_DEADZONE_K25[] = "InputDeadZoneK25";
    constexpr char I_INPUT_DEADZONE_K26[] = "InputDeadZoneK26";
    constexpr char I_INPUT_DEADZONE_K27[] = "InputDeadZoneK27";
    constexpr char I_INPUT_DEADZONE_K28[] = "InputDeadZoneK28";
    constexpr char I_INPUT_DEADZONE_K29[] = "InputDeadZoneK29";
    constexpr char I_INPUT_SPEED_S1A[] = "InputSpeedS1L";
    constexpr char I_INPUT_SPEED_S2A[] = "InputSpeedS1R";
}



class ConfigInput : public vConfig
{
private:
    GameModeKeys keys;
    std::map<Input::Pad, KeyMap> buffer;

public:
    ConfigInput() = delete;
    ConfigInput(const std::string& profile, GameModeKeys k);
    virtual ~ConfigInput();

    virtual void load() override;
    virtual void setDefaults() noexcept override;
    void setDefaultsUSLayout() noexcept;

public:
    void clearAll();

    void clearKey(Input::Pad ingame);
    void bind(Input::Pad ingame, const KeyMap& km);
    KeyMap getBindings(Input::Pad key);
};
