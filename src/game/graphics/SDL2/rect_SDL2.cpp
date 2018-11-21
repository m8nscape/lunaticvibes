#include "graphics_SDL2.h"

Rect::Rect(int zero) { x = y = w = h = -1; }
Rect::Rect(int w, int h) { x = y = 0; w = h = -1; }
Rect::Rect(int x1, int y1, int w1, int h1) { x = x1; y = y1; w = w1; h = h1; }
Rect::Rect(const SDL_Rect& rect) : SDL_Rect(rect) {}

Rect Rect::operator+ (const Rect& rhs) const
{
    Rect r = *this;
    r.x += rhs.x;
    r.y += rhs.y;
    r.w += rhs.w;
    r.h += rhs.h;
    return r;
}

Rect Rect::operator* (const double& rhs) const
{
    Rect r = *this;
    r.x *= rhs;
    r.y *= rhs;
    r.w *= rhs;
    r.h *= rhs;
    return r;
}

bool Rect::operator== (const Rect& rhs) const
{
    return x == rhs.x
        && y == rhs.y
        && w == rhs.w
        && h == rhs.h;
}

Rect Rect::standardize(const Rect& validRect) const
{
    if (*this == Rect())
    {
        // (-1, -1, -1, -1)
        return validRect;
    }
    else
    {
        if (w == -1 && h == -1)
        {
            return Rect(
                x,
                y,
                validRect.w - x,
                validRect.h - y
            );
        }
        else if (x >= 0 && x <= validRect.w
              && y >= 0 && y <= validRect.h)
        {
            return Rect(x, y, w, h);
        }
        else
        {
            return Rect();
        }
    }
}

Rect Rect::standardize(const Image& image) const
{
    return standardize(image.getRect());
}

