#pragma once
#include "config.h"

static const StringPath CONFIG_FILE_INPUT = "bindings.yml";

namespace cfg
{
    const char* I_DEVICE_TYPE = "DeviceType";
    const char* I_DEVICE_TYPE_KB = "Keyboard";
    const char* I_DEVICE_TYPE_CON = "Controller";

    const char* I_NOTBOUND = "_";

    const char* I_DEVICE_ID = "ID";             // USB: VID,PID,ID
}

class ConfigInput : public vConfig
{
public:
    ConfigInput();
    virtual ~ConfigInput();

    virtual void setDefaults() noexcept override;

public:
    void bindKey(unsigned b, unsigned input);
    void bindButton(unsigned id, unsigned b, unsigned input);
};
