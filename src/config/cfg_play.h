#pragma once
#include "config.h"

const char* pl_hs1 = "HiSpeed1P";
const char* pl_hs2 = "HiSpeed2P";
const char* pl_sud1 = "Sud+1P";
const char* pl_sud2 = "Sud+2P";
const char* pl_hid1 = "Hid+1P";
const char* pl_hid2 = "Hid+2P";
const char* pl_lift1 = "Lift+1P";
const char* pl_lift2 = "Lift+2P";
const char* pl_gr1 = "Green1P";
const char* pl_gr2 = "Green2P";
const char* pl_mod1 = "Mode1P";
const char* pl_mod2 = "Mode2P";

class CfgPlay : public vCfg
{
public:
    CfgPlay() {}

public:
    virtual void setDefaults() noexcept override;
protected:
    virtual int copyValues(const json& j) noexcept override;
    virtual int checkValues() noexcept override;
};
