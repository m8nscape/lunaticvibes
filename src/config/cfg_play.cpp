#include "cfg_play.h"

void CfgPlay::setDefaults() noexcept
{
    set<unsigned>(pl_hs1, 100);
    set<unsigned>(pl_gr1, 1200);
    set<unsigned>(pl_sud1, 100);
    set<unsigned>(pl_hid1, 100);
    set<unsigned>(pl_lift1, 100);
    set<unsigned>(pl_mod1, 0);

    set<unsigned>(pl_hs2, 100);
    set<unsigned>(pl_gr2, 1200);
    set<unsigned>(pl_sud2, 100);
    set<unsigned>(pl_hid2, 100);
    set<unsigned>(pl_lift2, 100);
    set<unsigned>(pl_mod2, 0);
}

int CfgPlay::copyValues(const json& j) noexcept
{
    int c = 0;
    c += checkUnsigned(j, pl_hs1);
    c += checkUnsigned(j, pl_gr1);
    c += checkUnsigned(j, pl_sud1);
    c += checkUnsigned(j, pl_hid1);
    c += checkUnsigned(j, pl_lift1);
    c += checkUnsigned(j, pl_mod1);

    c += checkUnsigned(j, pl_hs2);
    c += checkUnsigned(j, pl_gr2);
    c += checkUnsigned(j, pl_sud2);
    c += checkUnsigned(j, pl_hid2);
    c += checkUnsigned(j, pl_lift2);
    c += checkUnsigned(j, pl_mod2);
    return c;
}

int CfgPlay::checkValues() noexcept
{
    int c = 0;

    // hs 1p
    if (get<unsigned>(pl_hs1) == 0)
    {
        c++;
        set<unsigned>(pl_hs1, 100);
    }
    // hs 2p
    if (get<unsigned>(pl_hs2) == 0)
    {
        c++;
        set<unsigned>(pl_hs2, 100);
    }
    // gr 1p
    if (get<unsigned>(pl_gr1) == 0)
    {
        c++;
        set<unsigned>(pl_gr1, 100);
    }
    // gr 2p
    if (get<unsigned>(pl_gr2) == 0)
    {
        c++;
        set<unsigned>(pl_gr2, 100);
    }
    // sud 1p
    if (get<unsigned>(pl_sud1) == 0)
    {
        c++;
        set<unsigned>(pl_sud1, 0);
    }
    // sud 2p
    if (get<unsigned>(pl_sud2) == 0)
    {
        c++;
        set<unsigned>(pl_sud2, 0);
    }
    // hid 1p
    if (get<unsigned>(pl_hid1) == 0)
    {
        c++;
        set<unsigned>(pl_hid1, 0);
    }
    // hid 2p
    if (get<unsigned>(pl_hid2) == 0)
    {
        c++;
        set<unsigned>(pl_hid2, 0);
    }
    // lift 1p
    if (get<unsigned>(pl_lift1) == 0)
    {
        c++;
        set<unsigned>(pl_lift1, 0);
    }
    // lift 2p
    if (get<unsigned>(pl_lift2) == 0)
    {
        c++;
        set<unsigned>(pl_lift2, 0);
    }

    return c;
}
