#pragma once
#include "config.h"

const char* vid_FullHD = "FullHD";
const char* vid_fullscreen = "FullScreen";
const char* vid_borderless = "Borderless";
const char* vid_maxfps = "MaxFPS";
const char* vid_vsync = "VSync";

class CfgVideo : public vCfg
{
public:
    CfgVideo() {}

public:
    virtual void setDefaults() noexcept override;
protected:
    virtual int copyValues(const json& j) noexcept override;
    virtual int checkValues() noexcept override;
};
