#include "graphics_SDL2.h"
#include "gtest/gtest.h"

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
bool Rect::operator!=(const Rect& rhs) const
{
    return !(*this == rhs);
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

TEST(Graphics_Rect, self_equal)
{
    Rect r1{ 0, 0, 40, 60 };
    EXPECT_EQ(r1, r1);

    Rect r2{};
    EXPECT_EQ(r2, r2);

    Rect r3{ 20, 30 };
    EXPECT_EQ(r3, r3);

    Rect r4{ 10, 20, 30, 40 };
    Rect r4_c = r4;
    EXPECT_EQ(r4, r4_c);

    EXPECT_NE(r1, r2);
    EXPECT_NE(r1, r3);
    EXPECT_NE(r1, r4);
    EXPECT_NE(r2, r3);
    EXPECT_NE(r2, r4);
    EXPECT_NE(r3, r4);
}

TEST(Graphics_Rect, wrapping)
{
    Rect r0{ 0, 0, 1024, 1024 };

    Rect full1{};
    EXPECT_EQ(r0, full1.standardize(r0));

    Rect full2{ -1, -1 };
    EXPECT_EQ(r0, full2.standardize(r0));

    Rect full3{ 0, 0, -1, -1 };
    EXPECT_EQ(r0, full3.standardize(r0));

    Rect r_oobx{ -300, 0, 1024, 1024 };
    EXPECT_NE(r0, r_oobx.standardize(r0));
    Rect r_ooby{ 0, -40, 1024, 1024 };
    EXPECT_NE(r0, r_ooby.standardize(r0));
    Rect r_oobw{ 0, 0, 2048, 1024 };
    EXPECT_NE(r0, r_oobw.standardize(r0));
    Rect r_oobh{ 0, 0, 1024, 2048 };
    EXPECT_NE(r0, r_oobh.standardize(r0));

    Rect r_oobx2{ 1025, 0, 1024, 1024 };
    EXPECT_NE(r0, r_oobx2.standardize(r0));
    Rect r_ooby2{ 0, 1025, 1024, 1024 };
    EXPECT_NE(r0, r_ooby2.standardize(r0));
    Rect r_oobw2{ 0, 0, 1025, 1024 };
    EXPECT_NE(r0, r_oobw2.standardize(r0));
    Rect r_oobh2{ 0, 0, 1024, 1025 };
    EXPECT_NE(r0, r_oobh2.standardize(r0));

    Rect large{ -1024, -1024, 4096, 4096 };
    EXPECT_NE(r0, large);

}

TEST(Graphics_Rect, add_normal)
{
    Rect r1{ 0, 0, 40, 60 };
    Rect r2{ 0, 0, 40, 60 };
    Rect res = r1 + r2;

}