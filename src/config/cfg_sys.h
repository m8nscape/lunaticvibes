#pragma once
#include "config.h"

const char* sys_basespd = "Basespeed";

class CfgSystem : public vCfg
{
public:
    CfgSystem() {}

public:
    virtual void setDefaults() noexcept override;
protected:
    virtual int copyValues(const json& j) noexcept override;
    virtual int checkValues() noexcept override;
};
