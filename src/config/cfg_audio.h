#pragma once
#include "config.h"

enum audioMode
{
    AutoDetect = 0,
    Nosound,
    Unknown,

    // Windows
    WinMM = 10,
    DSound,
    WASAPI,
    ASIO,
    Atmos,

    // Linux
    PulseAudio = 20,
    ALSA,

    // Mac
    CoreAudio = 30,
};
const char* aud_mode = "OutputMode";
const char* aud_bufLen = "BufferLength";
const char* aud_bufCount = "BufferCount";

class CfgAudio : public vCfg
{
public:
    CfgAudio() {}

public:
    virtual void setDefaults() noexcept override;
protected:
    virtual int copyValues(const json& j) noexcept override;
    virtual int checkValues() noexcept override;
};