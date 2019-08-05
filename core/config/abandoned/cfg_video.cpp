#include "cfg_video.h"

// FIXME sleep function in C++ is based on '''SYSTEM SCHEDULE TICKS'''
// which means any values higher than '''15.6ms''' may not work as expected..
void CfgVideo::setDefaults() noexcept
{
    set<bool>(vid_FullHD, false);
    set<bool>(vid_fullscreen, false);
    set<bool>(vid_borderless, false);
    set<bool>(vid_vsync, false);
    set<unsigned>(vid_maxfps, 240);
}

int CfgVideo::copyValues(const json& j) noexcept
{
    int c = 0;
    c += checkBool(j, vid_FullHD);
    c += checkBool(j, vid_fullscreen);
    c += checkBool(j, vid_borderless);
    c += checkBool(j, vid_vsync);
    c += checkUnsigned(j, vid_maxfps);
    return c;
}

int CfgVideo::checkValues() noexcept
{
    int c = 0;

    if (get<unsigned>(vid_maxfps) > 1000)
    {
        c++;
        set<unsigned>(vid_maxfps, 240);
    }

    return c;
}
