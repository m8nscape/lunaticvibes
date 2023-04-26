#include "common/pch.h"
#include "graphics_SDL2.h"

Rect::Rect(int zero) { x = y = w = h = 0; }
Rect::Rect(int w1, int h1) { x = y = 0; w = w1; h = h1; }
Rect::Rect(int x1, int y1, int w1, int h1) { x = x1; y = y1; w = w1; h = h1; }
Rect::Rect(const SDL_Rect& rect) : SDL_Rect(rect) {}

Rect Rect::operator+ (const Rect& rhs) const
{
    Rect r = *this;
    r.x += rhs.x;
    r.y += rhs.y;

    if (r.w == RECT_FULL.w || rhs.w == RECT_FULL.w)
        r.w = RECT_FULL.w;
    else
        r.w += rhs.w;

    if (r.h == RECT_FULL.h || rhs.h == RECT_FULL.h)
        r.h = RECT_FULL.h;
    else
        r.h += rhs.h;

    return r;
}

Rect Rect::operator* (const double& rhs) const
{
    Rect r = *this;
    r.x *= (int)rhs;
    r.y *= (int)rhs;
    if (r.w != RECT_FULL.w) r.w *= rhs;
    if (r.h != RECT_FULL.h) r.h *= rhs;
    return r;
}

bool Rect::operator== (const Rect& rhs) const
{
    return x == rhs.x
        && y == rhs.y
        && w == rhs.w
        && h == rhs.h;
}
bool Rect::operator!=(const Rect& rhs) const
{
    return !(*this == rhs);
}

RectF::RectF(int zero) { x = y = w = h = 0.; }
RectF::RectF(float w1, float h1) { x = y = 0; w = w1; h = h1; }
RectF::RectF(float x1, float y1, float w1, float h1) { x = x1; y = y1; w = w1; h = h1; }
RectF::RectF(const SDL_FRect& RectF) : SDL_FRect(RectF) {}

RectF RectF::operator+ (const RectF& rhs) const
{
    RectF r = *this;
    r.x += rhs.x;
    r.y += rhs.y;

    if (r.w == RECTF_FULL.w || rhs.w == RECTF_FULL.w)
        r.w = RECTF_FULL.w;
    else
        r.w += rhs.w;

    if (r.h == RECTF_FULL.h || rhs.h == RECTF_FULL.h)
        r.h = RECTF_FULL.h;
    else
        r.h += rhs.h;

    return r;
}

RectF RectF::operator* (const double& rhs) const
{
    RectF r = *this;
    r.x *= (int)rhs;
    r.y *= (int)rhs;
    if (r.w != RECTF_FULL.w) r.w *= rhs;
    if (r.h != RECTF_FULL.h) r.h *= rhs;
    return r;
}

bool RectF::operator== (const RectF& rhs) const
{
    return x == rhs.x
        && y == rhs.y
        && w == rhs.w
        && h == rhs.h;
}
bool RectF::operator!=(const RectF& rhs) const
{
    return !(*this == rhs);
}
