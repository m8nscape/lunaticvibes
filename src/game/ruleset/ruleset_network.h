#pragma once
#include "ruleset.h"
#include <vector>

class vRulesetNetwork: virtual public vRuleset
{
public:
    static std::vector<unsigned char> packInit(std::shared_ptr<vRuleset> local);
    bool virtual unpackInit(const std::vector<unsigned char>& payload) = 0;
    static std::vector<unsigned char> packFrame(std::shared_ptr<vRuleset> local);
    bool virtual unpackFrame(std::vector<unsigned char>& payload) = 0;
};