#include "cfg_sys.h"

void CfgSystem::setDefaults() noexcept
{
    set<unsigned>(sys_basespd, 100);
}

int CfgSystem::copyValues(const json& j) noexcept
{
    return checkUnsigned(j, sys_basespd) ? 1 : 0;
}

int CfgSystem::checkValues() noexcept
{
    int c = 0;

    // base speed
    if (get<unsigned>(sys_basespd) == 0)
    {
        c++;
        set<unsigned>(sys_basespd, 100);
    }

    return c;
}
