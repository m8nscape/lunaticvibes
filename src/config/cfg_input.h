#pragma once
#include "config.h"
#include "game/input/input_keys.h"

static const StringPath CONFIG_FILE_INPUT = "bindings.yml";

namespace cfg
{
    inline const char* I_DEVICE_TYPE = "DeviceType";
    inline const char* I_DEVICE_TYPE_KB = "Keyboard";
    inline const char* I_DEVICE_TYPE_CON = "Controller";

    inline const char* I_NOTBOUND = "_";

    inline const char* I_DEVICE_ID = "ID";             // USB: VID,PID,ID
}

class ConfigInput : public vConfig
{
public:
    ConfigInput();
    virtual ~ConfigInput();

    virtual void setDefaults() noexcept override;

public:
    void bindKey(Input::Ingame ingame, Input::Key key);
    //void bindButton(unsigned id, Input::Ingame ingame, unsigned input);
};
